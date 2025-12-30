#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "flight.h"
#include "graph.h"

int main(void) {
    struct Flight *head = NULL, *tail = NULL;
    int counter = 0;
    int max_size = MAX_FLIGHTS;

    /* load existing flights from file (silent at startup) */
    loadFlights(&head, &tail, &counter, max_size, 1);

    while (1) {
        char role;
        printf("\n==== Flight Management System ====\n");
        printf("U = User | E = Employee | Q = Quit\n");
        printf("Enter role: ");
        if (scanf(" %c", &role) != 1) break;
        role = toupper(role);

        if (role == 'Q') {
            printf("Exiting. Goodbye.\n");
            break;
        }

        if (role == 'E') {
            if (!adminLogin()) continue;
            int ch = 0;
            while (1) {
                printf("\n--- Employee Menu ---\n");
                printf("1. Add flight\n2. Remove expired flights\n3. Build route graph\n");
                printf("4. Find connecting flights\n5. Display flights by departure\n6. Track delays\n7. Logout\nChoice: ");
                if (scanf("%d", &ch) != 1) { while(getchar()!='\n'); ch = -1; }
                if (ch == 7) break;

                struct Graph *g = createGraph();
                buildGraphFromFlights(g, head);

                if (ch == 1) {
                    struct Flight *newF = createFlightInteractive();
                    if (!newF) { freeGraph(g); continue; }
                    enqueueFlight(&head, &tail, newF, &counter, max_size);
                } else if (ch == 2) {
                    removeExpiredFlightsInteractive(&head, &tail, &counter);
                } else if (ch == 3) {
                    printf("Route graph built. Airports: %d\n", g->n);
                } else if (ch == 4) {
                    char src[64], dest[64];
                    printf("Enter Source: "); scanf(" %63s", src);
                    printf("Enter Destination: "); scanf(" %63s", dest);
                    findConnectingFlights(g, head, src, dest);
                } else if (ch == 5) {
                    displayFlightsByDeparture(head);
                } else if (ch == 6) {
                    trackDelays(head);
                } else {
                    printf("Invalid choice.\n");
                }

                freeGraph(g);
            }
        } else if (role == 'U') {
            int ch = 0;
            while (1) {
                printf("\n--- User Menu ---\n");
                printf("1. Find connecting flights\n2. Display flights by departure\n3. Track flight delays\n4. Exit User Menu\nChoice: ");
                if (scanf("%d", &ch) != 1) { while(getchar()!='\n'); ch = -1; }
                if (ch == 4) break;

                struct Graph *g = createGraph();
                buildGraphFromFlights(g, head);

                if (ch == 1) {
                    char src[64], dest[64];
                    printf("Enter Source: "); scanf(" %63s", src);
                    printf("Enter Destination: "); scanf(" %63s", dest);
                    findConnectingFlights(g, head, src, dest);
                } else if (ch == 2) {
                    displayFlightsByDeparture(head);
                } else if (ch == 3) {
                    trackDelays(head);
                } else {
                    printf("Invalid choice.\n");
                }

                freeGraph(g);
            }
        } else {
            printf("Invalid role.\n");
        }
    }

    /* cleanup flights list */
    struct Flight *cur = head;
    while (cur) {
        struct Flight *n = cur->next; free(cur); cur = n;
    }

    return 0;
}

