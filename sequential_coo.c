#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 500000
#define MAX_EDGES 5000000


struct COO {
    int numVertices;
    int numEdges;
    int *row;
    int *col;
};

void BFS_COO(struct COO* graph, int start) {
    int *visited = (int*)calloc(graph->numVertices, sizeof(int));
    int *queue = (int*)malloc(graph->numVertices * sizeof(int));

    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    printf("Urutan BFS: ");

    while (front < rear) {
        int u = queue[front++];
        printf("%d ", u);

        for (int i = 0; i < graph->numEdges; i++) {
            if (graph->row[i] == u) {
                int v = graph->col[i];

                if (!visited[v]) {
                    visited[v] = 1;
                    queue[rear++] = v;
                }
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

    struct COO graph;
    graph.row = (int*)malloc(MAX_EDGES * sizeof(int));
    graph.col = (int*)malloc(MAX_EDGES * sizeof(int));

    int src, dest;
    int edgeCount = 0;
    int maxNode = 0;
    int firstNode = -1;

    char line[100];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        if (sscanf(line, "%d %d", &src, &dest) == 2) {
            graph.row[edgeCount] = src;
            graph.col[edgeCount] = dest;

            edgeCount++;

            if (src > maxNode) maxNode = src;
            if (dest > maxNode) maxNode = dest;

            if (firstNode == -1) firstNode = src;
        }
    }

    fclose(file);

    graph.numEdges = edgeCount;
    graph.numVertices = maxNode + 1;

    printf("Jumlah node: %d\n", graph.numVertices);
    printf("Jumlah edge: %d\n", graph.numEdges);

    if (firstNode != -1) {
        printf("Mulai BFS dari node: %d\n", firstNode);
        BFS_COO(&graph, firstNode);
    } else {
        printf("File kosong.\n");
    }

    free(graph.row);
    free(graph.col);

    return 0;
}