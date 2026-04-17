#include <stdio.h>
#include <stdlib.h>

#define STEP 50000
#define MAX_FILES 100

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "datasets/%s", argv[1]);


    FILE *in = fopen(filepath, "r");
    if (!in) { 
        printf("File tidak ditemukan!\n");
        return 1;
    }

    int u, v;
    int maxNode = 0;

    while (fscanf(in, "%d %d", &u, &v) != EOF) {
        if (u > maxNode) maxNode = u;
        if (v > maxNode) maxNode = v;
    }

    rewind(in);

    int numFiles = (maxNode / STEP) + 1;
    printf("Max node: %d\n", maxNode);
    printf("Jumlah file: %d\n", numFiles);

    FILE *outs[numFiles];

    for (int i = 0; i < numFiles; i++) {
        char filename[50];
        sprintf(filename, "datasets/subgraph/dataset_%dK.txt", (i + 1) * 50);
        outs[i] = fopen(filename, "w");
    }

    while (fscanf(in, "%d %d", &u, &v) != EOF) {
        for (int i = 0; i < numFiles; i++) {
            int limit = (i + 1) * STEP;

            if (u < limit && v < limit) {
                fprintf(outs[i], "%d %d\n", u, v);
            }
        }
    }

    fclose(in);

    for (int i = 0; i < numFiles; i++) {
        fclose(outs[i]);
    }

    printf("Selesai. Dataset bertingkat berhasil dibuat.\n");

    return 0;
}