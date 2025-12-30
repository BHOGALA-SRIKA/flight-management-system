#include "graph.h"

/* allocate empty graph */
struct Graph *createGraph() {
    struct Graph *g = malloc(sizeof(struct Graph));
    if (!g) return NULL;
    g->n = 0;
    for (int i = 0; i < MAX_AIRPORTS; i++) g->adj[i] = NULL;
    return g;
}

void freeGraph(struct Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) {
        struct Node *cur = g->adj[i];
        while (cur) {
            struct Node *nx = cur->next;
            free(cur);
            cur = nx;
        }
    }
    free(g);
}

/* find existing airport index, or -1 if not present */
int findAirportIndex(struct Graph *g, const char *name) {
    for (int i = 0; i < g->n; i++) if (strcmp(g->airports[i], name) == 0) return i;
    return -1;
}

/* add directed edge src->dest (adds airports if new) */
void buildGraphFromFlights(struct Graph *g, struct Flight *head) {
    while (head) {
        int s = findAirportIndex(g, head->Source);
        if (s == -1) { strncpy(g->airports[g->n], head->Source, 63); g->airports[g->n][63]=0; s = g->n++; }
        int d = findAirportIndex(g, head->Destination);
        if (d == -1) { strncpy(g->airports[g->n], head->Destination, 63); g->airports[g->n][63]=0; d = g->n++; }

        /* avoid duplicate edges: check list */
        int dup = 0;
        struct Node *t = g->adj[s];
        while (t) { if (t->idx == d) { dup = 1; break; } t = t->next; }
        if (!dup) {
            struct Node *nnode = malloc(sizeof(struct Node));
            nnode->idx = d;
            nnode->next = g->adj[s];
            g->adj[s] = nnode;
        }
        head = head->next;
    }
}

/* DFS util: print paths and flight numbers between legs */
static void dfs_util(struct Graph *g, struct Flight *flights, int u, int dest,
                     int visited[], int path[], int depth, int *found) {
    visited[u] = 1;
    path[depth++] = u;

    if (u == dest) {
        *found = 1;
        /* print airports path */
        printf("\nRoute: ");
        for (int i = 0; i < depth; i++) {
            printf("%s", g->airports[path[i]]);
            if (i < depth - 1) printf(" -> ");
        }
        /* print flight numbers for legs */
        printf("\n   Flights: ");
        int totalDelay = 0;
        for (int i = 0; i < depth - 1; i++) {
            /* find first flight matching Source->Destination */
            struct Flight *f = flights;
            int printed = 0;
            while (f) {
                if (strcmp(f->Source, g->airports[path[i]]) == 0 &&
                    strcmp(f->Destination, g->airports[path[i+1]]) == 0) {
                    if (printed) printf(", ");
                    printf("%s (Dep:%04d Arr:%04d Delay:%d)", f->Number, f->Departure, f->Arrival, f->Delay);
                    totalDelay += f->Delay;
                    printed = 1;
                    break; /* choose first available flight for that leg */
                }
                f = f->next;
            }
            if (!printed) printf("[No direct flight]");
            if (i < depth - 2) printf(" -> ");
        }
        printf("\n   Total delay across route: %d minutes\n\n", totalDelay);
    } else {
        struct Node *n = g->adj[u];
        while (n) {
            if (!visited[n->idx]) dfs_util(g, flights, n->idx, dest, visited, path, depth, found);
            n = n->next;
        }
    }

    visited[u] = 0;
}

/* BFS/DFS: find any connecting paths and print flights numbers & times */
void findConnectingFlights(struct Graph *g, struct Flight *flights, const char *src, const char *dest) {
    int s = findAirportIndex(g, src);
    int d = findAirportIndex(g, dest);
    if (s == -1 || d == -1) {
        printf("No such airport(s) in current routes.\n"); return;
    }

    int visited[MAX_AIRPORTS] = {0};
    int path[MAX_AIRPORTS];
    int found = 0;
    printf("\nSearching routes from %s to %s ...\n", src, dest);

    /* try direct flights displayed as well (the dfs will also find direct if edge exists) */
    dfs_util(g, flights, s, d, visited, path, 0, &found);
    if (!found) printf("No connecting route found between %s and %s.\n", src, dest);
}

