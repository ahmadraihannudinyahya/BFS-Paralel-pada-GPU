#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

struct CSR {
    int numVertices;
    int numEdges;
    int *row_ptr;
    int *col_idx;
};

void load_csr_binary(const char* filename, struct CSR* graph) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Gagal membuka file bin");
        exit(1);
    }

    fread(&graph->numVertices, sizeof(int), 1, f);
    fread(&graph->numEdges, sizeof(int), 1, f);

    graph->row_ptr = (int*)malloc((graph->numVertices + 1) * sizeof(int));
    graph->col_idx = (int*)malloc(graph->numEdges * sizeof(int));

    fread(graph->row_ptr, sizeof(int), graph->numVertices + 1, f);
    fread(graph->col_idx, sizeof(int), graph->numEdges, f);

    fclose(f);
}

__global__ void bfs_kernel(int *row_ptr, int *col_idx,
                           int *frontier, int *next_frontier,
                           int *visited,
                           int *size_frontier, int *size_next) {

    int tid = threadIdx.x;

    for (int i = tid; i < *size_frontier; i += blockDim.x) {

        int u = frontier[i];

        for (int e = row_ptr[u]; e < row_ptr[u + 1]; e++) {
            int v = col_idx[e];

            if (atomicExch(&visited[v], 1) == 0) {
                int pos = atomicAdd(size_next, 1);
                next_frontier[pos] = v;
            }
        }
    }
}

void BFS_CSR_GPU(struct CSR* graph, int start) {

    int *d_row_ptr, *d_col_idx;
    int *d_frontier, *d_next_frontier;
    int *d_visited, *d_size_frontier, *d_size_next;

    int n = graph->numVertices;

    cudaMalloc(&d_row_ptr, (n + 1) * sizeof(int));
    cudaMalloc(&d_col_idx, graph->numEdges * sizeof(int));

    cudaMemcpy(d_row_ptr, graph->row_ptr, (n + 1)*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_col_idx, graph->col_idx, graph->numEdges*sizeof(int), cudaMemcpyHostToDevice);

    cudaMalloc(&d_frontier, n*sizeof(int));
    cudaMalloc(&d_next_frontier, n*sizeof(int));
    cudaMalloc(&d_visited, n*sizeof(int));
    cudaMalloc(&d_size_frontier, sizeof(int));
    cudaMalloc(&d_size_next, sizeof(int));

    int *h_visited = (int*)calloc(n, sizeof(int));

    h_visited[start] = 1;
    cudaMemcpy(d_visited, h_visited, n*sizeof(int), cudaMemcpyHostToDevice);

    int h_frontier[1] = {start};
    int size_frontier = 1;

    cudaMemcpy(d_frontier, h_frontier, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_size_frontier, &size_frontier, sizeof(int), cudaMemcpyHostToDevice);

    int level = 0;


    while (size_frontier > 0) {

        int zero = 0;
        cudaMemcpy(d_size_next, &zero, sizeof(int), cudaMemcpyHostToDevice);

        bfs_kernel<<<1, 256>>>(
            d_row_ptr, d_col_idx,
            d_frontier, d_next_frontier,
            d_visited,
            d_size_frontier, d_size_next
        );

        cudaDeviceSynchronize();

        cudaMemcpy(&size_frontier, d_size_next, sizeof(int), cudaMemcpyDeviceToHost);

        if (size_frontier == 0) break;

        cudaMemcpy(d_frontier, d_next_frontier, size_frontier*sizeof(int), cudaMemcpyDeviceToDevice);
        cudaMemcpy(d_size_frontier, &size_frontier, sizeof(int), cudaMemcpyHostToDevice);

        level++;
    }


    free(h_visited);

    cudaFree(d_row_ptr);
    cudaFree(d_col_idx);
    cudaFree(d_frontier);
    cudaFree(d_next_frontier);
    cudaFree(d_visited);
    cudaFree(d_size_frontier);
    cudaFree(d_size_next);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: %s <file.bin>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "bin/datasets/csr/%s", argv[1]);

    struct CSR graph;

    load_csr_binary(filepath, &graph);


    BFS_CSR_GPU(&graph, 0);

    free(graph.row_ptr);
    free(graph.col_idx);

    return 0;
}