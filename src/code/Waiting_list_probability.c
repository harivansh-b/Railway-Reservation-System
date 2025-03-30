#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../header/PNR.h"
#include "../header/Structures.h"

#include "../code/PNR.c"


struct historical {
    char train_name[500];
    char coach[100];
    int cancel_no;
};

int total_capacity(char *coach) {
    if (strcasecmp(coach, "EC") == 0)
        return 2 * 45;
    else if (strcasecmp(coach, "AC") == 0)
        return 5 * 60;
    else if (strcasecmp(coach, "CC") == 0)
        return 7 * 120;
    else if (strcasecmp(coach, "1AC") == 0)
        return 16;
    else if (strcasecmp(coach, "2AC") == 0)
        return 3 * 32;
    else if (strcasecmp(coach, "3AC") == 0)
        return 4 * 64;
    else if (strcasecmp(coach, "Sleeper") == 0)
        return 12 * 72;
    else
        return -1;
}

int reserved_tickets(int date, char *filename, char *coach_type, char *arrival, char *destination) {
    char file_path[1000], full_arrival[1000], full_destination[1000];
    snprintf(file_path, sizeof(file_path), "../../db/%d/%s", date, filename);
    FILE *file = fopen(file_path, "r");
    if (!file) {
        printf(DARK_RED "\nUnable to open file: %s\n" RESET, file_path);
        return -1;
    }

    sprintf(full_arrival, "%s-in", arrival);
    sprintf(full_destination, "%s-out", destination);

    int count = 0;
    char line[MAX_LINE_LENGTH];
    char headers[MAX_LINE_LENGTH];

    if (!fgets(headers, sizeof(headers), file)) {
        printf(DARK_RED "\nUnable to read headers from file: %s\n" RESET, file_path);
        fclose(file);
        return -1;
    }

    int arrival_index = -1, destination_index = -1;
    int num_stations = 0;
    char *tokens[MAX_STATIONS];
    char *token = strtok(headers, ",");
    token = strtok(NULL, ",");
    token = strtok(NULL, ",");
    while (token != NULL && num_stations < MAX_STATIONS) {
        tokens[num_stations] = token;
        if (strncasecmp(token, full_arrival, strlen(full_arrival)) == 0) {
            arrival_index = num_stations;
        }
        if (strncasecmp(token, full_destination, strlen(full_destination)) == 0) {
            destination_index = num_stations;
        }
        token = strtok(NULL, ",");
        num_stations++;
    }

    if (arrival_index == -1 || destination_index == -1) {
        printf(DARK_RED "Invalid arrival or destination station.\n" RESET);
        fclose(file);
        return -1;
    }

    while (fgets(line, sizeof(line), file)) {
        char temp_coach_type[100], temp_coach_no[100];
        int seat_no;
        int reserved = 1;

        char *line_copy = strdup(line);
        if (!line_copy) {
            printf(DARK_RED "Memory allocation failed.\n" RESET);
            fclose(file);
            return -1;
        }

        sscanf(line, "%[^,],%[^,],%d", temp_coach_type, temp_coach_no, &seat_no);

        if (strcasecmp(temp_coach_type, coach_type) == 0) {
            char *token = strtok(line_copy, ",");
            for (int i = 0; i < num_stations; i++) {
                token = strtok(NULL, ",");
                if (i >= arrival_index && i <= destination_index) {
                    if (token && atoi(token) == 0) {
                        reserved = 0;
                        break;
                    }
                }
            }

            if (reserved) {
                count++;
            }
        }
        free(line_copy);
    }

    fclose(file);
    return count;
}

int cancellations_no(char *train_name, char *coach) {
    FILE *file = fopen("../../db/WaitingListHistoryDB.csv", "r"); 
    if (file == NULL) {
        printf(DARK_RED "Error opening file.\n" RESET);
        return -1;
    }

    int count = 0, total = 0;
    struct historical data;

    char line[1000];
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%[^,],%d", data.train_name, data.coach, &data.cancel_no);
        if (strcasecmp(train_name, data.train_name) == 0 && strcasecmp(coach, data.coach) == 0) {
            total += data.cancel_no;
            count++;
        }
    }

    fclose(file);

    if (count == 0) {
        return 0;
    }

    return total / count;
}

float waiting_to_cnf_probability(int date, char *train_name, char *train_id, char *arrival, char *destination, char *coach, char *coach_no, int wl_pos) {
    int C = total_capacity(coach);
    if (C == -1) {
        printf(DARK_RED "Invalid coach type: %s\n" RESET, coach);
        return 0.0;
    }

    char file_name[1000];
    sprintf(file_name, "%s-%s.csv", train_name, train_id);
    int R = reserved_tickets(date, file_name, coach, arrival, destination);
    if (R == -1) {
        printf(DARK_RED "Error reading reserved tickets.\n" RESET);
        return 0.0;
    }

    int X = cancellations_no(train_name, coach);
    if (X == -1) {
        printf(DARK_RED "Error reading cancellations.\n" RESET);
        return 0.0;
    }

    int A = C - R + X;

    float W = (float) wl_pos;
    float P = 100 - ((float) W / A * 100);
    return P;
}

int fzhhmain() {
printf("%.2f", waiting_to_cnf_probability(1, "Kovai SF Express", "12675", "Chennai", "Coimbatore", "EC", "Coach-2", 9));
return 0;
}

int confirmation_probability() {
    getchar();
    char pnr[14];
    printf(WHITE BOLD "\n\t\t\t\t\t\t\t\tCNF PROBABILITY PAGE\n\n" RESET);
    printf(LIME BOLD "Enter the PNR number: ");
    fgets(pnr, sizeof(pnr), stdin);
    printf("\n\n"RESET);
    pnr[strcspn(pnr, "\n")] = '\0';

    if (check_pnr(pnr)) {
        struct admin obj;
        FILE *fp = fopen("../../db/AdminDB.csv", "r");
        if (fp == NULL) {
            printf(ANSI_COLOR_RED "File Doesn't exist\n" ANSI_COLOR_RESET);
            return 0;
        }

        int wl_found = 0;

        while (fscanf(fp, "%[^,],%[^,],%d,%[^,],%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%f,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
                      obj.pnr, obj.username, &obj.date, obj.train_name, obj.train_no, obj.arrival, obj.destination,
                      &obj.seats[0], &obj.seats[1], &obj.seats[2], &obj.seats[3], &obj.seats[4], &obj.seats[5], &obj.seats[6],
                      &obj.price, obj.status[0], obj.status[1], obj.status[2], obj.status[3], obj.status[4], obj.status[5],
                      obj.arrt, obj.destt, obj.coach, &obj.acc, &obj.pin) == 26) {
            if (strcmp(obj.pnr, pnr) == 0) {
                for (int i = 0; i < 6; i++) {
                    if (obj.seats[i] == -1) {
                        char coach[10], coach_type[20], coach_no[20];
                        strcpy(coach, obj.coach);
                        int num;
                        sscanf(coach, "%s %s", coach_type, coach_no);
                        if (sscanf(obj.status[i], "WL%d", &num) == 1) {
                            wl_found = 1;
                            printf(ANSI_COLOR_YELLOW "Waiting List Status: %s\t" ANSI_COLOR_RESET, obj.status[i]);
                            printf(ANSI_COLOR_GREEN "Probability: %.2f%%\n" ANSI_COLOR_RESET,
                                   waiting_to_cnf_probability(obj.date, obj.train_name, obj.train_no, obj.arrival, obj.destination, coach_type, coach_no, num));
                        }
                    }
                }
                if (!wl_found) {
                    printf(ANSI_COLOR_MAGENTA "PNR %s does not have a waiting list status.\n" ANSI_COLOR_RESET, pnr);
                }
                break;
            }
        }
        fclose(fp);
    } else {
        printf(ANSI_COLOR_RED "Invalid PNR\n" ANSI_COLOR_RESET);
    }
    return 0;
}

