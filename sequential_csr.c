#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 500000
#define MAX_EDGES 5000000 

struct CSR {
    int numVertices;
    int numEdges;
    int *row_ptr;
    int *col_idx;
};

void BFS_CSR(struct CSR* graph, int start) {
    int *visited = (int*)calloc(graph->numVertices, sizeof(int));
    int *queue = (int*)malloc(graph->numVertices * sizeof(int));

    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    printf("Urutan BFS: ");

    while (front < rear) {
        int u = queue[front++];
        printf("%d ", u);

        for (int i = graph->row_ptr[u]; i < graph->row_ptr[u + 1]; i++) {
            int v = graph->col_idx[i];

            if (!visited[v]) {
                visited[v] = 1;
                queue[rear++] = v;
            }
        }
    }

    printf("\n");
    free(visited);
    free(queue);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Penggunaan: %s <nama_file>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "datasets/subgraph/%s", argv[1]);

    FILE* file = fopen(filepath, "r");
    if (!file) {
        perror("Gagal membuka file");
        return 1;
    }

    printf("Membaca file: %s...\n", filepath);

    int *degree = (int*)calloc(MAX_NODES, sizeof(int));

    int src, dest;
    int maxNode = 0;
    int edgeCount = 0;

    char line[100];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        if (sscanf(line, "%d %d", &src, &dest) == 2) {
            degree[src]++;
            edgeCount++;

            if (src > maxNode) maxNode = src;
            if (dest > maxNode) maxNode = dest;
        }
    }

    int numVertices = maxNode + 1;

    struct CSR graph;
    graph.numVertices = numVertices;
    graph.numEdges = edgeCount;

    graph.row_ptr = (int*)malloc((numVertices + 1) * sizeof(int));
    graph.col_idx = (int*)malloc(edgeCount * sizeof(int));

    graph.row_ptr[0] = 0;
    for (int i = 1; i <= numVertices; i++) {
        graph.row_ptr[i] = graph.row_ptr[i - 1] + degree[i - 1];
    }

    int *temp = (int*)malloc(numVertices * sizeof(int));
    memcpy(temp, graph.row_ptr, numVertices * sizeof(int));

    rewind(file);

    int firstNode = -1;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        if (sscanf(line, "%d %d", &src, &dest) == 2) {
            int pos = temp[src]++;
            graph.col_idx[pos] = dest;

            if (firstNode == -1) firstNode = src;
        }
    }

    fclose(file);
    free(temp);
    free(degree);

    if (firstNode != -1) {
        printf("Mulai BFS dari node: %d\n", firstNode);
        BFS_CSR(&graph, firstNode);
    } else {
        printf("File kosong.\n");
    }

    free(graph.row_ptr);
    free(graph.col_idx);

    return 0;
}