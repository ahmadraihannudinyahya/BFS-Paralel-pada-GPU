#include <stdio.h>
#include <stdlib.h>

struct LL
{
    int numVertices;
    int numEdges;
    int *head;
    int *to;
    int *next;
};

void load_ll_binary(const char *filename, struct LL *graph)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        perror("Gagal buka file bin");
        exit(1);
    }

    fread(&graph->numVertices, sizeof(int), 1, f);
    fread(&graph->numEdges, sizeof(int), 1, f);

    graph->head = (int *)malloc(graph->numVertices * sizeof(int));
    graph->to = (int *)malloc(graph->numEdges * sizeof(int));
    graph->next = (int *)malloc(graph->numEdges * sizeof(int));

    fread(graph->head, sizeof(int), graph->numVertices, f);
    fread(graph->to, sizeof(int), graph->numEdges, f);
    fread(graph->next, sizeof(int), graph->numEdges, f);

    fclose(f);
}

void BFS_LL(struct LL *graph, int start)
{
    int *visited = (int *)calloc(graph->numVertices, sizeof(int));
    int *queue = (int *)malloc(graph->numVertices * sizeof(int));

    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear)
    {
        int u = queue[front++];

        for (int i = graph->head[u]; i != -1; i = graph->next[i])
        {
            int v = graph->to[i];

            if (!visited[v])
            {
                visited[v] = 1;
                queue[rear++] = v;
            }
        }
    }

    free(visited);
    free(queue);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <file.bin>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "bin/datasets/linkedlist/%s", argv[1]);

    struct LL graph;

    load_ll_binary(filepath, &graph);

    printf("Node: %d, Edge: %d\n", graph.numVertices, graph.numEdges);

    BFS_LL(&graph, 0);

    free(graph.head);
    free(graph.to);
    free(graph.next);

    return 0;
}