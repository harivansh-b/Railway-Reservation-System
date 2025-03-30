#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#include "../header/Colors.h"
#include "../header/Structures.h"

#include "../code/Waiting_list_conversion.c"
#include "../code/Waiting_list_updation_2.c"

int pass_pnr_details(struct admin* obj,char pareno[]){
    FILE* fp;
    fp = fopen("../../db/AdminDB.csv", "r");
    if (fp == NULL) {
        printf(DARK_RED "File Doesn't exist\n" RESET);
        return 0;
    }

    shift(pareno);

    while (fscanf(fp, "%[^,],%[^,],%d,%[^,],%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%f,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
                  obj->pnr, obj->username, &obj->date, obj->train_name, obj->train_no, obj->arrival, obj->destination,
                  &obj->seats[0], &obj->seats[1], &obj->seats[2], &obj->seats[3], &obj->seats[4], &obj->seats[5], &obj->seats[6],
                  &obj->price, obj->status[0], obj->status[1], obj->status[2], obj->status[3], obj->status[4], obj->status[5],
                  obj->arrt, obj->destt, obj->coach,obj->acc,&obj->pin) == 26) {
        if (strcmp(obj->pnr, pareno) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int find_station_indices(char *station_name, char *stations[], int num_stations) {
    for (int i = 0; i < num_stations; i++) {
        if (strncasecmp(station_name, stations[i], strlen(station_name)) == 0)
            return i;
    }
    return -1;
}

int update_ticket(int dd, char *coach_type, char *coach_no, char *arrival_station_name, char *destination_station_name, char *file_name, int seat_no) {
    char file_path[250];
    snprintf(file_path, sizeof(file_path), "../../db/%d/%s", dd, file_name);
    FILE *file1 = fopen(file_path, "r");
    FILE *file2 = fopen("../../db/temp.csv", "w");
    if (!file1 || !file2) {
        printf(DARK_RED "\nUnable to open file." RESET);
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        return 1; 
    }

    struct trains row[MAX_LINE];
    int line_count = 0;
    int num_stations = 0;
    char *stations[MAX_STATIONS];

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
                    return 1; 
                }
            }
            token = strtok(NULL, ",");
            num_stations++;
        }
    }
    num_stations -= 3; 

    int arrival_station_index = find_station_indices(arrival_station_name, stations, num_stations);
    int destination_station_index = find_station_indices(destination_station_name, stations, num_stations);

    if (arrival_station_index == -1 || destination_station_index == -1) {
        printf(DARK_RED "Invalid arrival or destination station.\n" RESET);
        fclose(file1);
        fclose(file2);
        return 1; 
    }

    rewind(file1);
    fgets(line, sizeof(line), file1);
    fprintf(file2, "%s", line);

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

        if (strcmp(coach_type, row[line_count].coach_type) == 0 &&
            strcmp(coach_no, row[line_count].coach_no) == 0 &&
            row[line_count].seat_no == seat_no) {
            
            int can_book = 1;
            for (int j = arrival_station_index; j <= destination_station_index; j++) {
                if (row[line_count].stations[j] == 1) {
                    can_book = 0;
                    break;
                }
            }

            if (can_book) {
                for (int j = arrival_station_index; j <= destination_station_index; j++) {
                    row[line_count].stations[j] = 1;
                }
            }
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

    if (remove(file_path) != 0) {
        printf(DARK_RED "\nError deleting original file." RESET);
        return 1; 
    }

    if (rename("../../db/temp.csv", file_path) != 0) {
        printf(DARK_RED "\nError renaming temporary file." RESET);
        return 1; 
    }

    for (int i = 0; i < num_stations; i++) {
        free(stations[i]);
    }

    return 0; 
}



int check_between(const char arr1[], const char dest1[], const char arr2[], const char dest2[], const char train[], const char train_no[], char coach[]) {
    struct Train arr_t[MAX_TRAINS];
    struct Station arr_s[MAX_TRAINS][MAX_STATIONS];
    char a[50], b[10], c[10];
    int d;
    FILE *fp = fopen("../../db/StationsDB.csv", "r");
    if (fp == NULL) {
        perror("Failed to open All_stations.csv");
        exit(EXIT_FAILURE);
    }

    skip_bom(fp);

    int i = 0, j = 0;
    while (fscanf(fp, "%[^,],%[^,],%[^,],%d\n", a, b, c, &d) == 4) {
        if (strcasecmp(c, "dt") == 0 || strcasecmp(c, "nt") == 0) {
            if (i >= MAX_TRAINS) {
                fprintf(stderr, "Exceeded maximum number of trains\n");
                break;
            }
            strcpy(arr_t[i].t_name, a);
            strcpy(arr_t[i].t_n, b);
            strcpy(arr_t[i].tron, c);
            arr_t[i].stations = d;
            i++;
            j = 0; // Reset station index for new train
        } else {
            if (j >= MAX_STATIONS) {
                fprintf(stderr, "Exceeded maximum number of stations for train %d\n", i - 1);
                continue;
            }
            strcpy(arr_s[i - 1][j].station, a);
            strcpy(arr_s[i - 1][j].arrival, b);
            strcpy(arr_s[i - 1][j].departure, c);
            arr_s[i - 1][j].distance = d;
            j++;
        }
    }
    fclose(fp);

    int check = -1;
    for (int x = 0; x < i; x++) {
        if (strcasecmp(arr_t[x].t_name, train) == 0 && strcmp(arr_t[x].t_n, train_no) == 0) {
            check = x;
            break;
        }
    }

    if (check == -1) {
        fprintf(stderr, "Train not found: %s %s\n", train, train_no);
        return 0;
    }

    int arrivalx = -1, destinationx = -1, arrivaly = -1, destinationy = -1;
    for (int y = 0; y < arr_t[check].stations; y++) {
        if (strcasecmp(arr_s[check][y].station, arr1) == 0) {
            arrivalx = y;
        }
        if (strcasecmp(arr_s[check][y].station, arr2) == 0) {
            arrivaly = y;
        }
        if (strcasecmp(arr_s[check][y].station, dest1) == 0) {
            destinationx = y;
        }
        if (strcasecmp(arr_s[check][y].station, dest2) == 0) {
            destinationy = y;
        }
    }

    if (arrivalx != -1 && destinationx != -1 && arrivaly != -1 && destinationy != -1) {
        if (arrivaly >= arrivalx && destinationy <= destinationx) {
            return 1;
        }
    }

    return 0;
}

int priority(const char arri[], const char dest[], const char train_name[], const char train_number[], int date, char* result, char* temp_dest, char coachx[], int *temp_num) {
    struct waiting arr[MAX_WAITING];
    FILE *fp = fopen("../../db/WaitingPriorityDB.csv", "r");
    if (fp == NULL) {
        perror("Failed to open waiting_prior.csv");
        exit(EXIT_FAILURE);
    }

    skip_bom(fp);
    int flag = 0;
    int i = 0;
    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d,%[^\n]\n", arr[i].pnr, arr[i].train_name, arr[i].train_number, arr[i].arrival, arr[i].destination, &arr[i].date, &arr[i].seat, arr[i].coach) == 8) {
        int check = check_between(arri, dest, arr[i].arrival, arr[i].destination, arr[i].train_name, arr[i].train_number, arr[i].coach);
        if (check == 1 && date == arr[i].date) {
            strcpy(result, arr[i].pnr);
            strcpy(temp_dest, arr[i].destination);
            *temp_num = arr[i].seat;
            flag++;
            fclose(fp);
            return flag;
        }
        i++;
    }
    fclose(fp);
    return flag;
}

void remove_pnr(char *pnr) {
    FILE *fp = fopen("../../db/AdminDB.csv", "r");
    FILE *temp = fopen("../../db/temp.csv", "w");

    if (fp == NULL || temp == NULL) {
        printf(DARK_RED "Error opening file.\n" RESET);
        return;
    }

    char line[MAX_LINE_LENGTH];
    struct admin obj;

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%[^,],%d,%[^,],%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%f,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
               obj.pnr, obj.username, &obj.date, obj.train_name, obj.train_no, obj.arrival, obj.destination,
               &obj.seats[0], &obj.seats[1], &obj.seats[2], &obj.seats[3], &obj.seats[4], &obj.seats[5], &obj.seats[6],
               &obj.price, obj.status[0], obj.status[1], obj.status[2], obj.status[3], obj.status[4], obj.status[5],
               obj.arrt, obj.destt, obj.coach, obj.acc, &obj.pin);

        if (strcmp(obj.pnr, pnr) != 0) {
            fprintf(temp, "%s,%s,%d,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%f,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",
                    obj.pnr, obj.username, obj.date, obj.train_name, obj.train_no, obj.arrival, obj.destination,
                    obj.seats[0], obj.seats[1], obj.seats[2], obj.seats[3], obj.seats[4], obj.seats[5], obj.seats[6],
                    obj.price, obj.status[0], obj.status[1], obj.status[2], obj.status[3], obj.status[4], obj.status[5],
                    obj.arrt, obj.destt, obj.coach, obj.acc, obj.pin);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("../../db/AdminDB.csv");
    rename("../../db/temp.csv", "../../db/AdminDB.csv");
}

void print_string_info(const char* label, const char* str) {
    printf(SILVER "%s: '%s' (length: %zu)\n" RESET, label, str, strlen(str));
    for (size_t i = 0; i < strlen(str); i++) {
        printf(SILVER "%02x " RESET, (unsigned char)str[i]);
    }
    printf("\n");
}

void update_admin(char *panero, int seat_num) {
    FILE *fp = fopen("../../db/AdminDB.csv", "r+");
    if (fp == NULL) {
        perror("Failed to open Admin.csv");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    struct admin obj;
    struct admin update;
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%[^,],%d,%[^,],%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%f,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
               obj.pnr, obj.username, &obj.date, obj.train_name, obj.train_no, obj.arrival, obj.destination,
               &obj.seats[0], &obj.seats[1], &obj.seats[2], &obj.seats[3], &obj.seats[4], &obj.seats[5], &obj.seats[6],
               &obj.price, obj.status[0], obj.status[1], obj.status[2], obj.status[3], obj.status[4], obj.status[5],
               obj.arrt, obj.destt, obj.coach, obj.acc, &obj.pin);
        if (strcmp(obj.pnr, panero) == 0) {
            update = obj;
            found = 1;
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf(RED BOLD"PNR %s not found.\n"  RESET, panero);
        return;
    }
    remove_pnr(panero);
    for (int i = 0; i < 6; i++) {
        if (update.seats[i] == -1) {
            update.seats[i] = seat_num;
            strcpy(update.status[i], "CNF");
            break;
        }
    }
    fp = fopen("../../db/AdminDB.csv", "a+");
    if (fp == NULL) {
        perror("Failed to open Admin.csv");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s,%s,%d,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%.2f,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",
            update.pnr, update.username, update.date, update.train_name, update.train_no, update.arrival, update.destination,
            update.seats[0], update.seats[1], update.seats[2], update.seats[3], update.seats[4], update.seats[5], update.seats[6],
            update.price, update.status[0], update.status[1], update.status[2], update.status[3], update.status[4], update.status[5],
            update.arrt, update.destt, update.coach, update.acc, update.pin);

    fclose(fp);
}
void remove_wait(char panero[], int seatx) {
    struct waiting arr[100];
    FILE *fp = fopen("../../db/WaitingPriorityDB.csv", "r");
    FILE *tempx = fopen("../../db/temp.csv", "w");
    int i = 0;
    
    if (fp == NULL || tempx == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    skip_bom(fp);
    
    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d,%[^\n]\n", 
                  arr[i].pnr, arr[i].train_name, arr[i].train_number, arr[i].arrival, 
                  arr[i].destination, &arr[i].date, &arr[i].seat, arr[i].coach) == 8) {
        if (!(strcmp(panero, arr[i].pnr) == 0 && arr[i].seat == seatx)) {
            fprintf(tempx, "%s,%s,%s,%s,%s,%d,%d,%s\n", arr[i].pnr, arr[i].train_name, arr[i].train_number, 
                    arr[i].arrival, arr[i].destination, arr[i].date, arr[i].seat, arr[i].coach);
        }
        i++;
    }
    
    fclose(fp);
    fclose(tempx);
    
    remove("../../db/WaitingPriorityDB.csv");
    rename("../../db/temp.csv", "../../db/WaitingPriorityDB.csv");
}

void pnr_status_updation(char arr[], char dest[], char train_name[], char train_no[], int date, int seat_num, char coach[]) {
    char temp_dest[100],temp_pnr[12];
    int temp_num;
    char result[12];
    struct update temp;
    strcpy(temp.arrival, arr);
    strcpy(temp.destination, dest);
    struct admin prior;
    while (1) {
        int check = priority(temp.arrival, temp.destination, train_name, train_no, date, result, temp_dest, coach, &temp_num);
        int v=pass_pnr_details(&prior,result);
        if (check == 1 && strcasecmp(prior.coach,coach)==0 & v==1){
            char coach_type[20], coach_no[20],file_name[250];
            sscanf(coach, "%s %s", coach_type, coach_no);
            sprintf(file_name,"%s-%s.csv",train_name,train_no);
            update_ticket(date,coach_type,coach_no,temp.arrival,temp.destination,file_name,seat_num);
            update_admin(result, seat_num);
            strcpy(temp.arrival, temp_dest);
            strcpy(temp_pnr,result);
            remove_wait(temp_pnr,temp_num);
        } else {
            break;
        }
    }
}

int test_ASDG_main() {
    char arr[] = "Chennai";
    char dest[] = "Coimbatore";
    char train_name[] = "Kovai SF Express";
    char train_no[] = "12675";
    char coach[] = "EC Coach-1";
    pnr_status_updation(arr, dest, train_name, train_no, 1, 40, coach);
    pnr_status_updation(arr, dest, train_name, train_no, 1, 41, coach);
    pnr_status_updation(arr, dest, train_name, train_no, 1, 42, coach);
    return 0;
}