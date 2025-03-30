#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../header/Structures.h"


int number(char string[]) {
    char *ptr = string;
    while (*ptr && !isdigit(*ptr)) {
        ptr++;
    }
    int num = atoi(ptr);
    return num;
}

void skip_bom1(FILE *fp) {
    unsigned char bom[3];
    if (fread(bom, 1, 3, fp) == 3) {
        if (bom[0] != 0xEF || bom[1] != 0xBB || bom[2] != 0xBF) {
            fseek(fp, 0, SEEK_SET);
        }
    } else {
        fseek(fp, 0, SEEK_SET);
    }
}

char* trim_whitespace(char* str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
    return str;
}


void minus_admin(char pnr[], int current) {
   
    FILE *fp_admin = fopen("../../db/AdminDB.csv", "r");
    if (fp_admin == NULL) {
        perror("Failed to open Admin.csv");
        exit(EXIT_FAILURE);
    }
    
    struct admin obj[1000];
    int count_admin = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), fp_admin)) {
        sscanf(line, "%[^,],%[^,],%d,%[^,],%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%f,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
               obj[count_admin].pnr, obj[count_admin].username, &obj[count_admin].date, obj[count_admin].train_name, obj[count_admin].train_no, obj[count_admin].arrival, obj[count_admin].destination,
               &obj[count_admin].seats[0], &obj[count_admin].seats[1], &obj[count_admin].seats[2], &obj[count_admin].seats[3], &obj[count_admin].seats[4], &obj[count_admin].seats[5], &obj[count_admin].seats[6],
               &obj[count_admin].price, obj[count_admin].status[0], obj[count_admin].status[1], obj[count_admin].status[2], obj[count_admin].status[3], obj[count_admin].status[4], obj[count_admin].status[5],
               obj[count_admin].arrt, obj[count_admin].destt, obj[count_admin].coach, obj[count_admin].acc, &obj[count_admin].pin);
        count_admin++;
    }
    fclose(fp_admin);

    for (int i = 0; i < count_admin; i++) {
        if (strcmp(obj[i].pnr, pnr) == 0) {
            for (int x = 0; x < 6; x++) {
                if (obj[i].seats[x] == -1) {
                    int temp_num = number(obj[i].status[x]);
                    if (temp_num == current) {
                        sprintf(obj[i].status[x], "WL%d", temp_num-1);
                        break;
                    }
                }
            }
        }
    }

    FILE *temp_admin = fopen("../../db/temp_admin.csv", "w");
    if (temp_admin == NULL) {
        perror("Failed to open temp_admin.csv");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count_admin; i++) {
        fprintf(temp_admin, "%s,%s,%d,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%f,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",
                obj[i].pnr, obj[i].username, obj[i].date, obj[i].train_name, obj[i].train_no, obj[i].arrival, obj[i].destination,
                obj[i].seats[0], obj[i].seats[1], obj[i].seats[2], obj[i].seats[3], obj[i].seats[4], obj[i].seats[5], obj[i].seats[6],
                obj[i].price, obj[i].status[0], obj[i].status[1], obj[i].status[2], obj[i].status[3], obj[i].status[4], obj[i].status[5],
                obj[i].arrt, obj[i].destt, obj[i].coach, obj[i].acc, obj[i].pin);
    }

    fclose(temp_admin);

    if (remove("../../db/AdminDB.csv") != 0) {
        perror("Error deleting original file");
        exit(EXIT_FAILURE);
    }
    if (rename("../../db/temp_admin.csv", "../../db/AdminDB.csv") != 0) {
        perror("Error renaming temporary file");
        exit(EXIT_FAILURE);
    }
}


void minus_waiting(char train[], char train_no[], int date, char coach[]) {
    int count = 0;
    struct waiting arr[1000];

    FILE *fp = fopen("../../db/WaitingPriorityDB.csv", "r");
    if (fp == NULL) {
        perror("Failed to open waiting_prior.csv");
        exit(EXIT_FAILURE);
    }

    skip_bom1(fp);
    int i = 0;
    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d,%[^\n]\n", arr[i].pnr, arr[i].train_name, arr[i].train_number, arr[i].arrival, arr[i].destination, &arr[i].date, &arr[i].seat, arr[i].coach) == 8) {
        if (strcmp(arr[i].train_name, train) == 0 &&
            strcmp(arr[i].train_number, train_no) == 0 &&
            arr[i].date == date &&
            strcmp(trim_whitespace(arr[i].coach), coach) == 0) {
            minus_admin(arr[i].pnr, arr[i].seat);
            arr[i].seat -= 1;
         }
        i++;
        count++;
    }
    fclose(fp);

    FILE *temp = fopen("../../db/temp.csv", "w");
    if (temp == NULL) {
        perror("Failed to open temp.csv");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < count; j++) {
        fprintf(temp, "%s,%s,%s,%s,%s,%d,%d,%s\n", arr[j].pnr, arr[j].train_name, arr[j].train_number, arr[j].arrival, arr[j].destination, arr[j].date, arr[j].seat, arr[j].coach);
    }

    fclose(temp);

    if (remove("../../db/WaitingPriorityDB.csv") != 0) {
        perror("Error deleting original file");
        exit(EXIT_FAILURE);
    }
    if (rename("../../db/temp.csv", "../../db/WaitingPriorityDB.csv") != 0) {
        perror("Error renaming temporary file");
        exit(EXIT_FAILURE);
    }
}

void minus_waiting1(char train[], char train_no[], int date, char coach[], int pos) {
    int count = 0;
    struct waiting arr[1000];

    FILE *fp = fopen("../../db/WaitingPriorityDB.csv", "r");
    if (fp == NULL) {
        perror("Failed to open waiting_prior.csv");
        exit(EXIT_FAILURE);
    }

    skip_bom1(fp); 

    int i = 0;
    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d,%[^\n]\n",
                  arr[i].pnr, arr[i].train_name, arr[i].train_number,
                  arr[i].arrival, arr[i].destination, &arr[i].date,
                  &arr[i].seat, arr[i].coach) == 8) {
        if (strcmp(arr[i].train_name, train) == 0 &&
            strcmp(arr[i].train_number, train_no) == 0 &&
            arr[i].date == date &&
            strcmp(trim_whitespace(arr[i].coach), coach) == 0 &&
            arr[i].seat >= pos) {
            minus_admin(arr[i].pnr, arr[i].seat);
            arr[i].seat -= 1;
        }
        i++;
        count++;
    }
    fclose(fp);

    FILE *temp = fopen("../../db/temp.csv", "w");
    if (temp == NULL) {
        perror("Failed to open temp.csv");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < count; j++) {
        fprintf(temp, "%s,%s,%s,%s,%s,%d,%d,%s\n", arr[j].pnr, arr[j].train_name,
                arr[j].train_number, arr[j].arrival, arr[j].destination,
                arr[j].date, arr[j].seat, arr[j].coach);
    }

    fclose(temp);

    if (remove("../../db/WaitingPriorityDB.csv") != 0) {
        perror("Error deleting original file");
        exit(EXIT_FAILURE);
    }
    if (rename("../../db/temp.csv", "../../db/WaitingPriorityDB.csv") != 0) {
        perror("Error renaming temporary file");
        exit(EXIT_FAILURE);
    }
}


int dhmain() {
    char train[] = "Kovai SF Express";
    char train_no[] = "12675";
    char coach[] = "EC Coach-1";
    int date = 1;

    minus_waiting(train, train_no, date, coach);
    printf("Successful\n");
    return 0;
}
