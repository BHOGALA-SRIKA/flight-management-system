#include "flight.h"

/* ---------- Utilities ---------- */

/* Validate HHMM (hours 0-23, minutes 0-59) */
int validateHHMM(int hhmm) {
    if (hhmm < 0 || hhmm > 2359) return 0;
    int mm = hhmm % 100;
    int hh = hhmm / 100;
    return (mm >= 0 && mm <= 59 && hh >= 0 && hh <= 23);
}

int hhmm_to_minutes(int hhmm) {
    int hh = hhmm / 100;
    int mm = hhmm % 100;
    return hh * 60 + mm;
}

int minutes_to_hhmm(int minutes) {
    if (minutes < 0) return 0;
    minutes %= (24 * 60);
    int hh = minutes / 60;
    int mm = minutes % 60;
    return hh * 100 + mm;
}

/* ---------- File I/O ---------- */

/* Save the whole linked list to file (overwrite) */
void saveAllFlights(struct Flight *head) {
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) return;
    struct Flight *t = head;
    while (t) {
        fprintf(fp, "%s|%s|%s|%d|%d|%d\n",
                t->Number, t->Source, t->Destination,
                t->Departure, t->Arrival, t->Delay);
        t = t->next;
    }
    fclose(fp);
}

/* Find by flight number */
struct Flight *findFlight(struct Flight *head, const char *flightNumber) {
    while (head) {
        if (strcmp(head->Number, flightNumber) == 0) return head;
        head = head->next;
    }
    return NULL;
}

/* Load from file (pipe-delimited). Prevent duplicates and respect max_size. */
void loadFlights(struct Flight **head, struct Flight **tail, int *counter, int max_size, int silent) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        if (!silent) printf("No flight data file found (%s). Starting with empty list.\n", FILE_NAME);
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (*counter >= max_size) {
            if (!silent) printf("Reached max flight capacity (%d). Remaining file lines ignored.\n", max_size);
            break;
        }
        /* trim newline */
        char *nl = strchr(line, '\n'); if (nl) *nl = '\0';
        if (line[0] == '\0') continue;

        struct Flight *f = malloc(sizeof(struct Flight));
        if (!f) { fclose(fp); return; }
        f->next = NULL;

        /* parse format: Number|Source|Destination|Departure|Arrival|Delay */
        int read = sscanf(line, "%15[^|]|%63[^|]|%63[^|]|%d|%d|%d",
                          f->Number, f->Source, f->Destination,
                          &f->Departure, &f->Arrival, &f->Delay);
        if (read != 6 || !validateHHMM(f->Departure) || !validateHHMM(f->Arrival) || f->Delay < 0) {
            free(f);
            continue; /* skip malformed line */
        }

        if (findFlight(*head, f->Number)) { free(f); continue; } /* skip duplicates */

        /* append */
        if (*tail == NULL) {
            *head = *tail = f;
        } else {
            (*tail)->next = f;
            *tail = f;
        }
        (*counter)++;
    }

    fclose(fp);
    if (!silent) printf("Loaded %d flights from %s\n", *counter, FILE_NAME);
}

/* ---------- Queue / list operations ---------- */

int isDuplicateFlight(struct Flight *head, const char *flightNumber) {
    return findFlight(head, flightNumber) != NULL;
}

void enqueueFlight(struct Flight **head, struct Flight **tail, struct Flight *f, int *counter, int max_size) {
    if (*counter >= max_size) {
        printf("Queue is full! cannot add %s\n", f->Number);
        free(f);
        return;
    }
    if (isDuplicateFlight(*head, f->Number)) {
        printf("Duplicate flight number: %s — not added.\n", f->Number);
        free(f);
        return;
    }
    f->next = NULL;
    if (*tail == NULL) { *head = *tail = f; }
    else { (*tail)->next = f; *tail = f; }
    (*counter)++;
    saveAllFlights(*head);
    printf("Flight %s added successfully.\n", f->Number);
}

/* Interactive creation of flight node (validates inputs). Returns allocated Flight or NULL */
struct Flight *createFlightInteractive() {
    struct Flight *f = malloc(sizeof(struct Flight));
    if (!f) return NULL;
    f->next = NULL;

    printf("Enter Flight Number: ");
    scanf(" %15[^\n]", f->Number);
    printf("Enter Source: ");
    scanf(" %63[^\n]", f->Source);
    printf("Enter Destination: ");
    scanf(" %63[^\n]", f->Destination);
    printf("Enter Departure time (HHMM): ");
    if (scanf("%d", &f->Departure) != 1 || !validateHHMM(f->Departure)) { printf("Invalid HHMM\n"); free(f); while(getchar()!='\n'); return NULL; }
    printf("Enter Arrival time (HHMM): ");
    if (scanf("%d", &f->Arrival) != 1 || !validateHHMM(f->Arrival)) { printf("Invalid HHMM\n"); free(f); while(getchar()!='\n'); return NULL; }
    printf("Enter Delay (minutes): ");
    if (scanf("%d", &f->Delay) != 1 || f->Delay < 0) { printf("Invalid delay\n"); free(f); while(getchar()!='\n'); return NULL; }
    return f;
}

/* Remove expired flights with interactive current time */
void removeExpiredFlightsInteractive(struct Flight **head, struct Flight **tail, int *counter) {
    int time;
    printf("Enter current time (HHMM): ");
    if (scanf("%d", &time) != 1 || !validateHHMM(time)) { printf("Invalid time.\n"); while(getchar()!='\n'); return; }
    removeExpiredFlightsAtTime(head, tail, counter, time);
}

/* Remove expired flights by comparing arrival+delay to current HHMM (accurate across minutes) */
void removeExpiredFlightsAtTime(struct Flight **head, struct Flight **tail, int *counter, int currentHHMM) {
    if (*head == NULL) { printf("No flights to remove.\n"); return; }
    int currentMin = hhmm_to_minutes(currentHHMM);
    struct Flight *temp = *head, *prev = NULL;
    int removed = 0;
    while (temp) {
        int arrivalMin = hhmm_to_minutes(temp->Arrival) + temp->Delay;
        if (arrivalMin < currentMin) {
            /* remove temp */
            printf("Removing expired flight: %s (%s -> %s)\n", temp->Number, temp->Source, temp->Destination);
            struct Flight *toFree = temp;
            if (prev == NULL) {
                *head = temp->next;
                temp = *head;
                if (*head == NULL) *tail = NULL;
            } else {
                prev->next = temp->next;
                if (temp == *tail) *tail = prev;
                temp = prev->next;
            }
            free(toFree);
            (*counter)--;
            removed++;
        } else {
            prev = temp;
            temp = temp->next;
        }
    }
    if (!removed) printf("No expired flights found.\n");
    saveAllFlights(*head);
}

/* Display functions */
void displayFlights(struct Flight *head) {
    if (!head) { printf("No flights available.\n"); return; }
    printf("\n%-10s | %-12s -> %-12s | %-7s | %-7s | %s\n", "Flight", "Source", "Destination", "Depart", "Arrive", "Delay");
    printf("--------------------------------------------------------------------------------\n");
    while (head) {
        printf("%-10s | %-12s -> %-12s | %04d    | %04d    | %3d mins\n",
               head->Number, head->Source, head->Destination, head->Departure, head->Arrival, head->Delay);
        head = head->next;
    }
}

void displayFlightsByDeparture(struct Flight *head) {
    /* copy pointers to array then sort */
    int n = 0;
    struct Flight *t = head;
    while (t) { n++; t = t->next; }
    if (n == 0) { printf("No flights available.\n"); return; }
    struct Flight **arr = malloc(n * sizeof(struct Flight *));
    t = head;
    for (int i = 0; i < n; i++) { arr[i] = t; t = t->next; }
    for (int i = 0; i < n-1; i++)
        for (int j = i+1; j < n; j++)
            if (arr[i]->Departure > arr[j]->Departure) {
                struct Flight *swap = arr[i]; arr[i] = arr[j]; arr[j] = swap;
            }
    printf("\nFlights Sorted by Departure Time:\n");
    for (int i = 0; i < n; i++) {
        printf("%s | %s -> %s | Dep:%04d | Arr:%04d | Delay:%d mins\n",
               arr[i]->Number, arr[i]->Source, arr[i]->Destination,
               arr[i]->Departure, arr[i]->Arrival, arr[i]->Delay);
    }
    free(arr);
}

/* Show only flights which have Delay > 0 */
void trackDelays(struct Flight *head) {
    int found = 0;
    while (head) {
        if (head->Delay > 0) {
            if (!found) { printf("\nDelayed Flights:\n"); found = 1; }
            int actualMin = hhmm_to_minutes(head->Arrival) + head->Delay;
            int actualHHMM = minutes_to_hhmm(actualMin);
            printf("%s | %s -> %s | Scheduled:%04d | Actual:%04d | Delay:%d mins\n",
                   head->Number, head->Source, head->Destination,
                   head->Arrival, actualHHMM, head->Delay);
        }
        head = head->next;
    }
    if (!found) printf("No delayed flights.\n");
}

/* Simple admin login (ID/password stored here as requested) */
int adminLogin() {
    char id[32], pwd[32];
    printf("Employee ID: ");
    scanf(" %31s", id);
    printf("Password: ");
    scanf(" %31s", pwd);
    if (strcmp(id, "abcd") == 0 && strcmp(pwd, "1234") == 0) {
        printf("Login successful.\n"); return 1;
    }
    printf("Invalid credentials.\n"); return 0;
}

