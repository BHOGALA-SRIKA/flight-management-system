#ifndef GRAPH_H
#define GRAPH_H

#include "flight.h"

#define MAX_AIRPORTS 100

/* adjacency list node */
struct Node {
    int idx;
    struct Node *next;
};

struct Graph {
    int n; /* number of airports */
    char airports[MAX_AIRPORTS][64];
    struct Node *adj[MAX_AIRPORTS];
};

struct Graph *createGraph();
void freeGraph(struct Graph *g);
int findAirportIndex(struct Graph *g, const char *name);
void buildGraphFromFlights(struct Graph *g, struct Flight *head);
void findConnectingFlights(struct Graph *g, struct Flight *flights, const char *src, const char *dest);

#endif

