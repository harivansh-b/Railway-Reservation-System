#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/Structures.h"
#include "../header/Colors.h"

#include "../code/Waiting_list_functions.c"

int available_seats[MAX_LINE];
int availablity;

int find_station_index(char *station_name, char *stations[], int num_stations);
void show_train(char *file_name, char *search);
int book_ticket(int dd, char *coach_type, char *coach_no, char *arrival_station_name, char *destination_station_name, char *file_name, char *booked_status);
int remove_ticket(int dd, char *coach_type, char *coach_no, char *arrival_station_name, char *destination_station_name, char *file_name, int num_seats_to_delete, char (*booked_status)[num_seats_to_delete], int seats_to_delete[num_seats_to_delete]);

int find_station_index(char *station_name, char *stations[], int num_stations) {
    for (int i = 0; i < num_stations; i++) {
        if (strncasecmp(station_name, stations[i], strlen(station_name)) == 0)
            return i;
    }
    return -1;
}

void show_train(char *file_name, char *search) {
    FILE *train_file = fopen(file_name, "r");

    if (train_file == NULL) {
        perror("Error opening the file");
        return;
    }
    
    char line[50];
    int emptyLineCount = 0;
    int found = 0; 

    while (fgets(line, sizeof(line), train_file) != NULL) {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';

        if (strcmp(search, line) == 0) {
            found = 1;
            printf(BOLD MINT "\n\t\t\t\t%s\n\n" RESET, line);
            emptyLineCount = 0;
            while (fgets(line, sizeof(line), train_file) != NULL) {
                if (line[0] == '\n') {
                    emptyLineCount++;
                    if (emptyLineCount == 2) {
                        fclose(train_file);
                        return;
                    }
                } else {
                    emptyLineCount = 0;
                    printf(BOLD MINT "%s\n" RESET, line);
                }
            }
            break;
        }
    }

    if (!found) {
        printf(SALMON "Coach not found.\n" RESET);
    }

    fclose(train_file);
}

int book_ticket(int dd,char *coach_type, char *coach_no, char *arrival_station_name, char *destination_station_name, char *file_name, char *booked_status) {
    char file_path[10000];
    snprintf(file_path, sizeof(file_path), "../../db/%d/%s", dd, file_name);
    FILE *file1 = fopen(file_path, "r");
    FILE *file2 = fopen("../../db/temp.csv", "w");

    if (!file1 || !file2) {
        printf(DARK_RED "\nUnable to open file." RESET);
        exit(1);
    }

    struct trains row[MAX_LINE];
    availablity=0;
    int booked = 0;
    int line_count = 0;
    int seat_available = 0;

    char *stations[MAX_STATIONS];
    int num_stations = 0;

    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), file1)) {
        char *token = strtok(line, ",");
        while (token != NULL && num_stations < MAX_STATIONS) {
            if (num_stations >= 3) {
                stations[num_stations - 3] = strdup(token);
                if (!stations[num_stations - 3]) {
                    printf(DARK_RED "\nMemory allocation failed.\n" RESET);
                    fclose(file1);
                    fclose(file2);
                    exit(1);
                }
            }
            token = strtok(NULL, ",");
            num_stations++;
        }
    }
    num_stations -= 3;

    int arrival_station_index = find_station_index(arrival_station_name, stations, num_stations);
    int destination_station_index = find_station_index(destination_station_name, stations, num_stations);

    if (arrival_station_index == -1 || destination_station_index == -1) {
        printf(DARK_RED "Invalid arrival or destination station.\n" RESET);
        fclose(file1);
        fclose(file2);
        return 0;
    }

    rewind(file1);
    fgets(line, sizeof(line), file1); 
    
    while (fgets(line, sizeof(line), file1)) {
        char *temp_line = strdup(line);
        if (!temp_line) {
            printf(DARK_RED "\nMemory allocation failed.\n" RESET);
            break;
        }
        char *token = strtok(temp_line, ",");
        strcpy(row[line_count].coach_type, token);
        token = strtok(NULL, ",");
        strcpy(row[line_count].coach_no, token);
        token = strtok(NULL, ",");
        row[line_count].seat_no = atoi(token);
        token = strtok(NULL, ",");
        for (int j = 0; j < num_stations; j++) {
            row[line_count].stations[j] = atoi(token);            
            token = strtok(NULL, ",");
        }

        free(temp_line);

        int can_book = 1;

        for (int j = arrival_station_index; j <= destination_station_index; j++) {
            if (row[line_count].stations[j] == 1) {
                can_book = 0;
                break;
            }
        }
        if (strcmp(coach_type, row[line_count].coach_type) == 0 && strcmp(coach_no, row[line_count].coach_no) == 0 && can_book) {
            available_seats[availablity++]=row[line_count].seat_no;
            seat_available = 1;
        }
        
        line_count++;
    }

    if (!seat_available) {
        fclose(file1);
        fclose(file2);
        
        char train_name[1000],train_id[10];

        sscanf(file_name, "%[^-]-%[^.].csv", train_name, train_id);

        return add_to_waiting_list(dd,train_name,train_id,arrival_station_name,destination_station_name,coach_type,coach_no,booked_status);
    }

    printf(GOLD "The available seats in %s %s from station %s to %s are: AVL %d\n" RESET, coach_type, coach_no, arrival_station_name, destination_station_name,availablity);
    for(int i=0;i<availablity;i++)
       printf(LIGHT_YELLOW "%d " RESET,available_seats[i]);

    rewind(file1);
    fgets(line, sizeof(line), file1);
    fprintf(file2, "%s", line);

    int seat_no;
    printf(PURPLE"\n\nEnter the seat no:");
    scanf("%d", &seat_no);
    printf(RESET);
    for (int i = 0; i < line_count; i++) {
        int can_book = 1;
        for (int j = arrival_station_index; j <= destination_station_index; j++) {
            if (row[i].stations[j] == 1) {
                can_book = 0;
                break;
            }
        }

        if (strcmp(coach_type, row[i].coach_type) == 0 && strcmp(coach_no, row[i].coach_no) == 0 && can_book && row[i].seat_no == seat_no) {
            for (int j = arrival_station_index; j <= destination_station_index; j++) {
                row[i].stations[j] = 1;
            }
            booked = 1;
            
            printf(CYAN "\nSeat no %d in the coach %s-%s has been allotted for booking." RESET,row[i].seat_no,row[i].coach_type,row[i].coach_no);
            strcpy(booked_status,"CNF");
        }

        fprintf(file2, "%s,%s,%d", row[i].coach_type, row[i].coach_no, row[i].seat_no);
        for (int j = 0; j < num_stations; j++) {
            fprintf(file2, ",%d", row[i].stations[j]);
        }
        fprintf(file2, "\n");
    }

    fclose(file1);
    fclose(file2);

    if (remove(file_path) != 0) {
        printf(DARK_RED"\nError deleting original file." RESET);
        exit(1);
    }

    if (rename("../../db/temp.csv", file_path) != 0) {
        printf(DARK_RED "\nError renaming temporary file." RESET);
        exit(1);
    }

    return seat_no;
}


int remove_ticket(int dd, char *coach_type, char *coach_no, char *arrival_station_name, char *destination_station_name, char *file_name, int num_seats_to_delete, char (*booked_status)[num_seats_to_delete], int seats_to_delete[num_seats_to_delete]) {
    char file_path[100];
    snprintf(file_path, sizeof(file_path), "../../db/%d/%s", dd, file_name);
    FILE *file1 = fopen(file_path, "r");
    FILE *file2 = fopen("../../db/temp.csv", "w");

    if (!file1 || !file2) {
        printf(DARK_RED "Unable to open file.\n" RESET);
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        return 0;
    }

    struct trains row[MAX_LINE];
    availablity = 0;
    int line_count = 0;
    int deleted = 0;

    char *stations[MAX_STATIONS];
    int num_stations = 0;

    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), file1)) {
        char *token = strtok(line, ",");
        while (token != NULL && num_stations < MAX_STATIONS) {
            if (num_stations >= 3) {
                stations[num_stations - 3] = strdup(token);
                if (!stations[num_stations - 3]) {
                    printf(DARK_RED "Memory allocation failed.\n" RESET);
                    fclose(file1);
                    fclose(file2);
                    return 0;
                }
            }
            token = strtok(NULL, ",");
            num_stations++;
        }
    }
    num_stations -= 3;

    int arrival_station_index = find_station_index(arrival_station_name, stations, num_stations);
    int destination_station_index = find_station_index(destination_station_name, stations, num_stations);

    if (arrival_station_index == -1 || destination_station_index == -1) {
        printf(DARK_RED "Invalid arrival or destination station.\n" RESET);
        fclose(file1);
        fclose(file2);
        return 0;
    }

    rewind(file1);
    fgets(line, sizeof(line), file1); 
    fprintf(file2, "%s", line);

    while (fgets(line, sizeof(line), file1)) {
        char *temp_line = strdup(line);
        if (!temp_line) {
            printf(DARK_RED "Memory allocation failed.\n" RESET);
            break;
        }
        char *token = strtok(temp_line, ",");
        strcpy(row[line_count].coach_type, token);
        token = strtok(NULL, ",");
        strcpy(row[line_count].coach_no, token);
        token = strtok(NULL, ",");
        row[line_count].seat_no = atoi(token);
        token = strtok(NULL, ",");
        for (int j = 0; j < num_stations; j++) {
            row[line_count].stations[j] = atoi(token);            
            token = strtok(NULL, ",");
        }

        free(temp_line);

        int can_delete = 0;
        for (int j = 0; j < num_seats_to_delete; j++) {
            if (seats_to_delete[j] == row[line_count].seat_no &&
                strcmp(coach_type, row[line_count].coach_type) == 0 &&
                strcmp(coach_no, row[line_count].coach_no) == 0) {
                can_delete = 1;
                break;
            }
        }

        if (can_delete) {
            for (int j = arrival_station_index; j <= destination_station_index; j++) {
                row[line_count].stations[j] = 0;
            }
            deleted++;
            strcpy(booked_status[line_count], "Removed");
            //printf("Seat %d removed successfully.\n", row[line_count].seat_no);
        }
        
        fprintf(file2, "%s,%s,%d", row[line_count].coach_type, row[line_count].coach_no, row[line_count].seat_no);
        for (int j = 0; j < num_stations; j++) {
            fprintf(file2, ",%d", row[line_count].stations[j]);
        }
        fprintf(file2, "\n");
        line_count++;
    }

    fclose(file1);
    fclose(file2);
    remove(file_path);
    rename("../../db/temp.csv", file_path);

    for (int i = 0; i < num_stations; i++) {
        free(stations[i]);
    }

    return deleted;
}

int nbbbmain() {
    int dd = 1;
    char coach_type[] = "EC";
    char coach_no[] = "Coach-2";
    char arrival_station_name[] = "Chennai-in";
    char destination_station_name[] = "Coimbatore-out";
    char file_name[] = "Kovai SF Express-12675.csv";
    char *booked_status;

    int result = book_ticket(dd, coach_type, coach_no, arrival_station_name, destination_station_name, file_name, booked_status);
    if (result == 1) {
        printf("Ticket booked successfully.\n");
    } else if (result == -1) {
        printf("Added to waiting list.\n");
    } else {
        printf("Booking failed.\n");
    }

    return 0;
}
