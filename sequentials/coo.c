#include <stdio.h>
#include <stdlib.h>

struct COO {
    int numVertices;
    int numEdges;
    int *row;
    int *col;
};

void load_coo_binary(const char* filename, struct COO* graph) {
    FILE* f = fopen(filename, "rb");
    printf("%s\n", filename);
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

void BFS_COO(struct COO* graph, int start) {
    int *visited = (int*)calloc(graph->numVertices, sizeof(int));
    int *queue = (int*)malloc(graph->numVertices * sizeof(int));

    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int u = queue[front++];

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

    free(visited);
    free(queue);
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

    printf("Node: %d, Edge: %d\n", graph.numVertices, graph.numEdges);

    BFS_COO(&graph, 0);

    free(graph.row);
    free(graph.col);

    return 0;
}