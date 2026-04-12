#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Gunakan batas untuk dataset NotreDame
#define MAX_NODES 500000 

struct Node {
    int vertex;
    struct Node* next;
};

struct Graph {
    int numVertices;
    struct Node** adjLists;
    int* visited;
};

struct Node* createNode(int v) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

struct Graph* createGraph(int vertices) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->numVertices = vertices;

    graph->adjLists = (struct Node**)malloc(vertices * sizeof(struct Node*));
    graph->visited = (int*)malloc(vertices * sizeof(int));

    for (int i = 0; i < vertices; i++) {
        graph->adjLists[i] = NULL;
        graph->visited[i] = 0;
    }
    return graph;
}

void addEdge(struct Graph* graph, int src, int dest) {
    if (src >= graph->numVertices || dest >= graph->numVertices) return;

    // Tambahkan Edge (Directed sesuai format web-graph)
    struct Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

void BFS(struct Graph* graph, int startVertex) {
    if (startVertex >= graph->numVertices || graph->adjLists[startVertex] == NULL) {
        printf("Vertex awal tidak ditemukan atau tidak memiliki tetangga.\n");
        return;
    }

    int* queue = (int*)malloc(graph->numVertices * sizeof(int));
    int front = 0, rear = 0;

    graph->visited[startVertex] = 1;
    queue[rear++] = startVertex;

    printf("Urutan BFS: ");
    while (front < rear) {
        int currentVertex = queue[front++];
        printf("%d ", currentVertex);

        struct Node* temp = graph->adjLists[currentVertex];
        while (temp) {
            int adjVertex = temp->vertex;
            if (!graph->visited[adjVertex]) {
                graph->visited[adjVertex] = 1;
                queue[rear++] = adjVertex;
            }
            temp = temp->next;
        }
    }
    printf("\n");
    free(queue);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Penggunaan: %s <nama_file_di_datasets/>\n", argv[0]);
        printf("Contoh: %s datasets_1000.txt\n", argv[0]);
        return 1;
    }

    // Bangun path: datasets/<nama_file>
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "datasets/subgraph/%s", argv[1]);

    FILE* file = fopen(filepath, "r");
    if (!file) {
        perror("Gagal membuka file");
        return 1;
    }

    struct Graph* graph = createGraph(MAX_NODES);

    char line[100];
    int src, dest;
    int firstNode = -1;

    printf("Membaca file: %s...\n", filepath);
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        if (sscanf(line, "%d %d", &src, &dest) == 2) {
            addEdge(graph, src, dest);
            if (firstNode == -1) firstNode = src;
        }
    }
    fclose(file);

    if (firstNode != -1) {
        printf("Memulai BFS dari node pertama yang ditemukan: %d\n", firstNode);
        BFS(graph, firstNode);
    } else {
        printf("File kosong atau tidak valid.\n");
    }

    return 0;
}