#include <stdio.h>
#include <stdlib.h>

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

void BFS_CSR(struct CSR* graph, int start) {
    int *visited = (int*)calloc(graph->numVertices, sizeof(int));
    int *queue = (int*)malloc(graph->numVertices * sizeof(int));

    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int u = queue[front++];

        for (int i = graph->row_ptr[u]; i < graph->row_ptr[u + 1]; i++) {
            int v = graph->col_idx[i];

            if (!visited[v]) {
                visited[v] = 1;
                queue[rear++] = v;
            }
        }
    }

    free(visited);
    free(queue);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Penggunaan: %s <file.bin>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "bin/datasets/csr/%s", argv[1]);

    struct CSR graph;

    load_csr_binary(filepath, &graph);

    printf("Node: %d, Edge: %d\n", graph.numVertices, graph.numEdges);

    BFS_CSR(&graph, 0);

    free(graph.row_ptr);
    free(graph.col_idx);

    return 0;
}