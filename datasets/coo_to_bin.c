#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EDGES 5000000

struct COO {
    int numVertices;
    int numEdges;
    int *row;
    int *col;
};

void save_coo_binary(const char* filename, struct COO* graph) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        perror("Gagal buat file bin");
        exit(1);
    }

    fwrite(&graph->numVertices, sizeof(int), 1, f);
    fwrite(&graph->numEdges, sizeof(int), 1, f);
    fwrite(graph->row, sizeof(int), graph->numEdges, f);
    fwrite(graph->col, sizeof(int), graph->numEdges, f);

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

    struct COO graph;
    graph.row = (int*)malloc(MAX_EDGES * sizeof(int));
    graph.col = (int*)malloc(MAX_EDGES * sizeof(int));

    int src, dest;
    int edgeCount = 0;
    int maxNode = 0;

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        if (sscanf(line, "%d %d", &src, &dest) == 2) {

            if (edgeCount >= MAX_EDGES) {
                printf("ERROR: edge melebihi kapasitas\n");
                exit(1);
            }

            graph.row[edgeCount] = src;
            graph.col[edgeCount] = dest;

            edgeCount++;

            if (src > maxNode) maxNode = src;
            if (dest > maxNode) maxNode = dest;
        }
    }

    fclose(file);

    graph.numEdges = edgeCount;
    graph.numVertices = maxNode + 1;

    printf("Node: %d, Edge: %d\n", graph.numVertices, graph.numEdges);

    char outpath[256];
    snprintf(outpath, sizeof(outpath), "bin/datasets/coo/%s.bin", argv[1]);

    save_coo_binary(outpath, &graph);

    printf("✔ Berhasil simpan ke: %s\n", outpath);

    free(graph.row);
    free(graph.col);

    return 0;
}