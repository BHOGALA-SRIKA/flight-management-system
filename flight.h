#ifndef FLIGHT_H
#define FLIGHT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLIGHTS 200
#define FILE_NAME "flight_all.txt"

struct Flight {
    char Number[16];
    char Source[64];
    char Destination[64];
    int Departure;    // HHMM
    int Arrival;      // HHMM
    int Delay;        // minutes
    struct Flight *next;
};

/* Flight list / queue functions */
void loadFlights(struct Flight **head, struct Flight **tail, int *counter, int max_size, int silent);
void saveAllFlights(struct Flight *head);
int isDuplicateFlight(struct Flight *head, const char *flightNumber);
struct Flight *createFlightInteractive();
void enqueueFlight(struct Flight **head, struct Flight **tail, struct Flight *f, int *counter, int max_size);
struct Flight *findFlight(struct Flight *head, const char *flightNumber);
void removeExpiredFlightsInteractive(struct Flight **head, struct Flight **tail, int *counter);
void removeExpiredFlightsAtTime(struct Flight **head, struct Flight **tail, int *counter, int currentHHMM);
void displayFlights(struct Flight *head);
void displayFlightsByDeparture(struct Flight *head);
void trackDelays(struct Flight *head);

/* utils */
int validateHHMM(int hhmm);
int hhmm_to_minutes(int hhmm);
int minutes_to_hhmm(int minutes);

int adminLogin();

#endif

