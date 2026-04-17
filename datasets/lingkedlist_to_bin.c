#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 5000000
#define MAX_EDGES 50000000

struct LL {
    int numVertices;
    int numEdges;
    int *head;
    int *to;
    int *next;
};

void save_ll_binary(const char* filename, struct LL* graph) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        perror("Gagal membuat file bin");
        exit(1);
    }

    fwrite(&graph->numVertices, sizeof(int), 1, f);
    fwrite(&graph->numEdges, sizeof(int), 1, f);
    fwrite(graph->head, sizeof(int), graph->numVertices, f);
    fwrite(graph->to, sizeof(int), graph->numEdges, f);
    fwrite(graph->next, sizeof(int), graph->numEdges, f);

    fclose(f);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "datasets/subgraph/%s", argv[1]);

    FILE* file = fopen(filepath, "r");
    if (!file) {
        perror("File error");
        return 1;
    }

    struct LL graph;

    graph.head = (int*)malloc(MAX_NODES * sizeof(int));
    graph.to   = (int*)malloc(MAX_EDGES * sizeof(int));
    graph.next = (int*)malloc(MAX_EDGES * sizeof(int));

    for (int i = 0; i < MAX_NODES; i++) {
        graph.head[i] = -1;
    }

    int src, dest;
    int edgeCount = 0;
    int maxNode = 0;

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        if (sscanf(line, "%d %d", &src, &dest) == 2) {

            if (src >= MAX_NODES || dest >= MAX_NODES) {
                printf("ERROR: node melebihi MAX_NODES (%d, %d)\n", src, dest);
                exit(1);
            }

            if (edgeCount >= MAX_EDGES) {
                printf("ERROR: edge melebihi kapasitas\n");
                exit(1);
            }

            graph.to[edgeCount] = dest;
            graph.next[edgeCount] = graph.head[src];
            graph.head[src] = edgeCount;

            edgeCount++;

            if (src > maxNode) maxNode = src;
            if (dest > maxNode) maxNode = dest;
        }
    }

    fclose(file);

    graph.numVertices = maxNode + 1;
    graph.numEdges = edgeCount;

    printf("Node: %d, Edge: %d\n", graph.numVertices, graph.numEdges);

    char outpath[256];
    snprintf(outpath, sizeof(outpath), "bin/datasets/linkedlist/%s.bin", argv[1]);

    save_ll_binary(outpath, &graph);

    printf("✔ LinkedList berhasil disimpan ke: %s\n", outpath);

    free(graph.head);
    free(graph.to);
    free(graph.next);

    return 0;
}