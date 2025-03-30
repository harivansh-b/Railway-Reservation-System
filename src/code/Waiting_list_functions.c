#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/Structures.h"
#include "../header/Colors.h"

int add_to_waiting_list(int dd, char *train_name, char *train_id, char *arrival_station_name, char *destination_station_name, char *coach_type, char *coach_no,char *booked_status) {
    FILE* file1 = fopen("../../db/WaitingListDB.csv", "r");
    if (!file1) {
        perror("Error opening waiting_list.csv");
        return 0;
    }

    struct waiting_list reader;
    char coach[MAX_LINE];
    snprintf(coach, sizeof(coach), "%s %s", coach_type, coach_no);

    char arrival[150],destination[150];
    sscanf(arrival_station_name,"%[^-]-in",arrival);
    sscanf(destination_station_name,"%[^-]-out",destination);

    char wl_read[MAX_LINE_LENGTH];
    int wl_count = 0;

    if (fgets(wl_read, sizeof(wl_read), file1) == NULL) {
        fclose(file1);
        perror("Error reading header from waiting_list.csv");
        return 0;
    }

    while (fgets(wl_read, sizeof(wl_read), file1) != NULL) {
        sscanf(wl_read, "%[^,],%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]\n",reader.pnr,&reader.date, reader.train_name, reader.train_id,reader.arrival, reader.destination, reader.wl_pos,reader.coach) ;
        if ((dd == reader.date) && (strncasecmp(reader.train_name, train_name,strlen(train_name)) == 0) && (strncasecmp(reader.train_id, train_id,strlen(train_id)) == 0) && (strncasecmp(reader.arrival, arrival,strlen(arrival)) == 0) && (strncasecmp(reader.destination, destination,strlen(destination)) == 0) && (strncasecmp(reader.coach, coach,strlen(coach)) == 0)) {
                wl_count++;
        }
    }
    fclose(file1);

    FILE* file2 = fopen("../../db/WaitingListDB.csv", "a");
    if (!file2) {
        perror("Error opening waiting_list.csv");
        return 0;
    }

    char wl_pos[10];
    sprintf(wl_pos, "WL%d", wl_count + 1);

    fprintf(file2, "PNR,%d,%s,%s,%s,%s,%s,%s\n",
            dd, train_name, train_id,
            arrival, destination,
            wl_pos, coach);
    strcpy(booked_status,wl_pos);
    printf(CYAN "Waiting list ticket is booked with %s\n" RESET, wl_pos);
    fclose(file2);
    return -1;
}

void add_pnr_to_waitlist(char *pnr, int dd, char *train_name, char *train_id, char *arrival, char *destination, int wait_count, char wait_status[10][7], char *coach) {
    FILE *file1 = fopen("../../db/WaitingListDB.csv", "r");
    FILE *file2 = fopen("../../db/temp.csv", "w+");
    if (!file1 || !file2) {
        perror("Error opening files");
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        return;
    }

    struct waiting_list reader;
    char wl_read[MAX_LINE_LENGTH];

    if (fgets(wl_read, sizeof(wl_read), file1) != NULL) {
        fputs(wl_read, file2);
    } else {
        fclose(file1);
        fclose(file2);
        perror("Error reading header from waiting_list.csv");
        return;
    }

    int k = 0;
    int updated = 0;  

    while (fgets(wl_read, sizeof(wl_read), file1) != NULL) {
        sscanf(wl_read, "%[^,],%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
               reader.pnr, &reader.date, reader.train_name, reader.train_id,
               reader.arrival, reader.destination, reader.wl_pos, reader.coach);

        int should_update = 0;

        if (k < wait_count && 
            (dd == reader.date) &&
            (strcasecmp(reader.train_name, train_name) == 0) &&
            (strcasecmp(reader.train_id, train_id) == 0) &&
            (strcasecmp(reader.arrival, arrival) == 0) &&
            (strcasecmp(reader.destination, destination) == 0) &&
            (strcasecmp(reader.wl_pos, wait_status[k]) == 0) &&
            (strcasecmp(reader.coach, coach) == 0)) {
            should_update = 1;
        }

        if (should_update) {
            strcpy(reader.pnr, pnr);
            k++;
            updated = 1;  
        }

        fprintf(file2, "%s,%d,%s,%s,%s,%s,%s,%s\n",
                reader.pnr, reader.date, reader.train_name, reader.train_id,
                reader.arrival, reader.destination, reader.wl_pos, reader.coach);
    }

    fclose(file1);
    fclose(file2);

    if (updated) {
        remove("../../db/WaitingListDB.csv");
        rename("../../db/temp.csv", "../../db/WaitingListDB.csv");\
    } else {
        remove("../../db/temp.csv");
    }
}

void remove_waiting_list(char *pnr) {
    FILE *file1 = fopen("../../db/WaitingListDB.csv", "r");
    FILE *file2 = fopen("../../db/temp.csv", "w+");
    if (!file1 || !file2) {
        perror("Error opening files");
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        return;
    }

    struct waiting_list reader;
    char wl_read[MAX_LINE_LENGTH];
    int removed = 0;

    if (fgets(wl_read, sizeof(wl_read), file1) != NULL) {
        fputs(wl_read, file2);
    } else {
        fclose(file1);
        fclose(file2);
        perror("Error reading header from waiting_list.csv");
        return;
    }

    while (fgets(wl_read, sizeof(wl_read), file1) != NULL) {
        sscanf(wl_read, "%[^,],%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
               reader.pnr, &reader.date, reader.train_name, reader.train_id,
               reader.arrival, reader.destination, reader.wl_pos, reader.coach);

        if (strcasecmp(reader.pnr, pnr) == 0) {
            removed = 1;
            continue;  
        }

        fprintf(file2, "%s,%d,%s,%s,%s,%s,%s,%s\n",
                reader.pnr, reader.date, reader.train_name, reader.train_id,
                reader.arrival, reader.destination, reader.wl_pos, reader.coach);
    }

    fclose(file1);
    fclose(file2);

    if (removed) {
        remove("../../db/WaitingListDB.csv");
        rename("../../db/temp.csv", "../../db/WaitingListDB.csv");
    } else {
        remove("../../db/temp.csv");
    }
}
