#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

struct COO {
    int numVertices;
    int numEdges;
    int *row;
    int *col;
};

void load_coo_binary(const char* filename, struct COO* graph) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Gagal membuka file bin");
        exit(1);
    }

    fread(&graph->numVertices, sizeof(int), 1, f);
    fread(&graph->numEdges, sizeof(int), 1, f);

    graph->row = (int*)malloc(graph->numEdges * sizeof(int));
    graph->col = (int*)malloc(graph->numEdges * sizeof(int));

    fread(graph->row, sizeof(int), graph->numEdges, f);
    fread(graph->col, sizeof(int), graph->numEdges, f);

    fclose(f);
}

__global__ void bfs_coo_kernel(
    int *row, int *col,
    int *in_frontier,
    int *visited,
    int *next_frontier,
    int *size_next,
    int numEdges
) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid >= numEdges) return;

    int u = row[tid];
    int v = col[tid];

    if (in_frontier[u]) {
        if (atomicExch(&visited[v], 1) == 0) {
            int pos = atomicAdd(size_next, 1);
            next_frontier[pos] = v;
        }
    }
}


void BFS_COO_GPU(struct COO* graph, int start) {

    int n = graph->numVertices;
    int m = graph->numEdges;

    int *d_row, *d_col;
    int *d_in_frontier, *d_next_frontier;
    int *d_visited;
    int *d_size_next;

    cudaMalloc(&d_row, m * sizeof(int));
    cudaMalloc(&d_col, m * sizeof(int));
    cudaMalloc(&d_in_frontier, n * sizeof(int));
    cudaMalloc(&d_next_frontier, n * sizeof(int));
    cudaMalloc(&d_visited, n * sizeof(int));
    cudaMalloc(&d_size_next, sizeof(int));


    cudaMemcpy(d_row, graph->row, m * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_col, graph->col, m * sizeof(int), cudaMemcpyHostToDevice);

    int *h_in_frontier = (int*)calloc(n, sizeof(int));
    int *h_next_frontier = (int*)malloc(n * sizeof(int));
    int *h_visited = (int*)calloc(n, sizeof(int));

    h_in_frontier[start] = 1;
    h_visited[start] = 1;

    cudaMemcpy(d_in_frontier, h_in_frontier, n * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_visited, h_visited, n * sizeof(int), cudaMemcpyHostToDevice);

    int size_frontier = 1;
    int level = 0;


    while (size_frontier > 0) {

        int zero = 0;
        cudaMemcpy(d_size_next, &zero, sizeof(int), cudaMemcpyHostToDevice);

        int blocks = (m + 255) / 256;


        bfs_coo_kernel<<<blocks, 256>>>(
            d_row, d_col,
            d_in_frontier,
            d_visited,
            d_next_frontier,
            d_size_next,
            m
        );


        cudaDeviceSynchronize();


        cudaMemcpy(&size_frontier, d_size_next, sizeof(int), cudaMemcpyDeviceToHost);

        if (size_frontier == 0) break;

        cudaMemcpy(h_next_frontier, d_next_frontier, size_frontier * sizeof(int), cudaMemcpyDeviceToHost);

        memset(h_in_frontier, 0, n * sizeof(int));

        for (int i = 0; i < size_frontier; i++) {
            h_in_frontier[h_next_frontier[i]] = 1;
        }

        cudaMemcpy(d_in_frontier, h_in_frontier, n * sizeof(int), cudaMemcpyHostToDevice);

        level++;
    }


    free(h_in_frontier);
    free(h_next_frontier);
    free(h_visited);

    cudaFree(d_row);
    cudaFree(d_col);
    cudaFree(d_in_frontier);
    cudaFree(d_next_frontier);
    cudaFree(d_visited);
    cudaFree(d_size_next);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: %s <file.bin>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "bin/datasets/coo/%s", argv[1]);

    struct COO graph;

    load_coo_binary(filepath, &graph);


    BFS_COO_GPU(&graph, 0);

    free(graph.row);
    free(graph.col);

    return 0;
}