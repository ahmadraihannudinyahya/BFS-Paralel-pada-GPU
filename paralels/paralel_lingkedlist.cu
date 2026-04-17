#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

struct Graph {
    int numVertices;
    int numEdges;

    int *head;
    int *to;
    int *next;
};

void load_ll_binary(const char* filename, struct Graph* g) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Gagal membuka file bin");
        exit(1);
    }

    fread(&g->numVertices, sizeof(int), 1, f);
    fread(&g->numEdges, sizeof(int), 1, f);

    g->head = (int*)malloc(g->numVertices * sizeof(int));
    g->to   = (int*)malloc(g->numEdges * sizeof(int));
    g->next = (int*)malloc(g->numEdges * sizeof(int));

    fread(g->head, sizeof(int), g->numVertices, f);
    fread(g->to, sizeof(int), g->numEdges, f);
    fread(g->next, sizeof(int), g->numEdges, f);

    fclose(f);
}

__global__ void bfs_kernel(int *head, int *to, int *next,
                           int *current, int *next_frontier,
                           int *visited, int *size_current,
                           int *size_next) {

    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    for (int i = tid; i < *size_current; i += blockDim.x * gridDim.x) {

        int u = current[i];

        int edge = head[u];

        while (edge != -1) {
            int v = to[edge];

            if (atomicExch(&visited[v], 1) == 0) {
                int pos = atomicAdd(size_next, 1);
                next_frontier[pos] = v;
            }

            edge = next[edge];
        }
    }
}

void BFS_LL_GPU(struct Graph* g, int start) {

    int n = g->numVertices;
    int m = g->numEdges;

    int *d_head, *d_to, *d_next;
    int *d_current, *d_next_frontier;
    int *d_visited, *d_size_current, *d_size_next;

    cudaMalloc(&d_head, n * sizeof(int));
    cudaMalloc(&d_to, m * sizeof(int));
    cudaMalloc(&d_next, m * sizeof(int));

    cudaMemcpy(d_head, g->head, n*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_to, g->to, m*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_next, g->next, m*sizeof(int), cudaMemcpyHostToDevice);

    cudaMalloc(&d_current, n*sizeof(int));
    cudaMalloc(&d_next_frontier, n*sizeof(int));
    cudaMalloc(&d_visited, n*sizeof(int));
    cudaMalloc(&d_size_current, sizeof(int));
    cudaMalloc(&d_size_next, sizeof(int));

    int *h_visited = (int*)calloc(n, sizeof(int));

    h_visited[start] = 1;
    cudaMemcpy(d_visited, h_visited, n*sizeof(int), cudaMemcpyHostToDevice);

    int h_current[1] = {start};
    int size_current = 1;

    cudaMemcpy(d_current, h_current, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_size_current, &size_current, sizeof(int), cudaMemcpyHostToDevice);

    int level = 0;


    while (size_current > 0) {

        int zero = 0;
        cudaMemcpy(d_size_next, &zero, sizeof(int), cudaMemcpyHostToDevice);

        int blocks = (size_current + 255) / 256;

        bfs_kernel<<<blocks, 256>>>(
            d_head, d_to, d_next,
            d_current, d_next_frontier,
            d_visited, d_size_current, d_size_next
        );

        cudaDeviceSynchronize();

        cudaMemcpy(&size_current, d_size_next, sizeof(int), cudaMemcpyDeviceToHost);

        if (size_current == 0) break;

        cudaMemcpy(d_current, d_next_frontier, size_current*sizeof(int), cudaMemcpyDeviceToDevice);
        cudaMemcpy(d_size_current, &size_current, sizeof(int), cudaMemcpyHostToDevice);

        level++;
    }


    free(h_visited);

    cudaFree(d_head);
    cudaFree(d_to);
    cudaFree(d_next);
    cudaFree(d_current);
    cudaFree(d_next_frontier);
    cudaFree(d_visited);
    cudaFree(d_size_current);
    cudaFree(d_size_next);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: %s <file.bin>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "bin/datasets/linkedlist/%s", argv[1]);

    struct Graph graph;

    load_ll_binary(filepath, &graph);


    BFS_LL_GPU(&graph, 0);

    free(graph.head);
    free(graph.to);
    free(graph.next);

    return 0;
}