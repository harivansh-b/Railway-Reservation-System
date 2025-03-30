#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../header/Structures.h"
#include "../header/Colors.h"

void shift(char* string);
void write_admin(struct admin obj);
void show_status(int n, char string[]);
void show_ticket(char pnr[]);
int pnr_status();
int check_pnr(char pnr[]);
int rnumber(char no[], char dest[]);
void create_pnr(int number, char *result);
void generate_pnr(char *no, char *destination, char* pnr_string);
int test();
int another_test();

void shift(char* string) {
    int len = strlen(string);
    if (len < 10) {
        return;
    }
    if (string[3] != '-') {
        for (int i = len; i >= 3; i--) {
            string[i + 1] = string[i];
        }
        string[3] = '-';
        string[len + 1] = '\0';
    }
}

void write_admin(struct admin obj) {
    FILE* fp;
    fp = fopen("../../db/AdminDB.csv", "a");
    if (fp == NULL) {
        perror("Unable to open file!");
        exit(1);
    }
    shift(obj.pnr);
    fprintf(fp, "%s,%s,%d,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%.2f,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",
            obj.pnr, obj.username, obj.date, obj.train_name, obj.train_no, obj.arrival, obj.destination,
            obj.seats[0], obj.seats[1], obj.seats[2], obj.seats[3], obj.seats[4], obj.seats[5], obj.seats[6],
            obj.price, obj.status[0], obj.status[1], obj.status[2], obj.status[3], obj.status[4], obj.status[5], obj.arrt, obj.destt, obj.coach,obj.acc,obj.pin);
    fclose(fp);
}
void show_status(int n, char string[]) {
    if (n > 0) {
        printf( GREEN "|%d-%s| ", n, string);
    } else if (n == -1) {
        printf( RED "|%s| ", string);
    }
}

// Function to display ticket details
void show_ticket(char pnr[]) {
    struct admin obj;
    FILE* fp;
    fp = fopen("../../db/AdminDB.csv", "r");
    if (fp == NULL) {
        printf(RED "File Doesn't exist\n" RESET);
        return;
    }
    shift(pnr);
    int found = 0;
    while (fscanf(fp, "%[^,],%[^,],%d,%[^,],%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%f,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
                  obj.pnr, obj.username, &obj.date, obj.train_name, obj.train_no, obj.arrival, obj.destination,
                  &obj.seats[0], &obj.seats[1], &obj.seats[2], &obj.seats[3], &obj.seats[4], &obj.seats[5], &obj.seats[6],
                  &obj.price, obj.status[0], obj.status[1], obj.status[2], obj.status[3], obj.status[4], obj.status[5],
                  obj.arrt, obj.destt, obj.coach, obj.acc, &obj.pin) == 26) 
    {
        if (strcmp(obj.pnr, pnr) == 0) {
            printf( WHITE"\n\n\t\t\tTICKET DETAILS:\n\n" );
            printf(MINT "*******************************************************************\n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR_CYAN "PNR No: %s   Train No: %s    Date: %d/6/2024\n" ANSI_COLOR_RESET, obj.pnr, obj.train_no, obj.date);
            printf(ANSI_COLOR_MAGENTA "Train Name: %s || %s ==========> %s\n" ANSI_COLOR_RESET, obj.train_name, obj.arrival, obj.destination);
            printf(ANSI_COLOR_YELLOW "Arrival Time:   %s\nDestination Time: %s\n" ANSI_COLOR_RESET, obj.arrt, obj.destt);
            printf(ANSI_COLOR_GREEN "No of Tickets: %d\n" ANSI_COLOR_RESET, obj.seats[6]);
            printf(ANSI_COLOR_RED "Coach: %s\n" ANSI_COLOR_RESET, obj.coach);
            printf(ANSI_COLOR_BLUE "Ticket Price: %.2f\n" ANSI_COLOR_RESET, obj.price);
            printf(ANSI_COLOR_CYAN "Seat Details:\n" ANSI_COLOR_RESET);
            show_status(obj.seats[0], obj.status[0]);
            show_status(obj.seats[1], obj.status[1]);
            show_status(obj.seats[2], obj.status[2]);
            show_status(obj.seats[3], obj.status[3]);
            show_status(obj.seats[4], obj.status[4]);
            show_status(obj.seats[5], obj.status[5]);
            printf(MINT "\n*******************************************************************\n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR_GREEN "Have a great journey ahead!! Visit us again!!!" ANSI_COLOR_RESET);
            printf(MINT "\n*******************************************************************\n" ANSI_COLOR_RESET);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf(RED "\nWrong PNR, try again\n" RESET);
    }
    fclose(fp);
}

// Function to handle PNR status
int pnr_status() {
    printf(WHITE BOLD "\n\t\t\t\t\t\t\t\t\tPNR PAGE\n\n" RESET);
    char string[12];
    printf(CYAN "Enter the PNR number: " );
    scanf("%s", string);
    printf(RESET);
    show_ticket(string);
    return 0;
}

int check_pnr(char pnr[]) {
    struct admin obj;
    FILE* fp;
    fp = fopen("../../db/AdminDB.csv", "r");
    if (fp == NULL) {
        printf(DARK_RED "File Doesn't exist\n" RESET);
        return 0;
    }

    shift(pnr);

    while (fscanf(fp, "%[^,],%[^,],%d,%[^,],%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%f,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
                  obj.pnr, obj.username, &obj.date, obj.train_name, obj.train_no, obj.arrival, obj.destination,
                  &obj.seats[0], &obj.seats[1], &obj.seats[2], &obj.seats[3], &obj.seats[4], &obj.seats[5], &obj.seats[6],
                  &obj.price, obj.status[0], obj.status[1], obj.status[2], obj.status[3], obj.status[4], obj.status[5],
                  obj.arrt, obj.destt, obj.coach,obj.acc,&obj.pin) == 26) {
        if (strcmp(obj.pnr, pnr) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int rnumber(char no[], char dest[]) {
    srand(time(NULL));
    int temp_rand = 0, random = 0;
    for (int i = 0; i < 3; i++) {
        temp_rand = rand() % 10;
        random = random * 10 + temp_rand;
    }
    if (strcmp(no, "12675") == 0 || strcmp(no, "12676") == 0 || strcmp(no, "12674") == 0) {
        if (strcasecmp(dest, "Chennai") == 0)
            return 490;
        else if (strcasecmp(dest, "Salem") == 0)
            return 461;
        else if (strcasecmp(dest, "Erode") == 0)
            return 450;
        else if (strcasecmp(dest, "Coimbatore") == 0)
            return 451;
    } else if (strcmp(no, "16367") == 0) {
        if (strcasecmp(dest, "Madurai") == 0)
            return 416;
        else if (strcasecmp(dest, "Chennai") == 0)
            return 420;
        else if (strcasecmp(dest, "Dindigul") == 0)
            return 416;
        else if (strcasecmp(dest, "Thanjavur") == 0)
            return 471;
        else if (strcasecmp(dest, "Villupuram") == 0)
            return 471;
        else if (strcasecmp(dest, "Thirunelveli") == 0)
            return 470;
        else if (strcasecmp(dest, "Nagercoil") == 0)
            return 470;
        else if (strcasecmp(dest, "Kanyakumari") == 0)
            return 470;
    } else if (strcmp(no, "22668") == 0 || strcmp(no, "22667") == 0) {
        if (strcasecmp(dest, "Coimbatore") == 0)
            return 420;
        else if (strcasecmp(dest, "Erode") == 0)
            return 490;
        else if (strcasecmp(dest, "Madurai") == 0)
            return 430;
        else if (strcasecmp(dest, "Nagercoil") == 0)
            return 410;
        else if (strcasecmp(dest, "Dindigul") == 0)
            return 430;
        else if (strcasecmp(dest, "Thirunelveli") == 0)
            return 410;
    } else if (strcmp(no, "12635") == 0) {
        if (strcasecmp(dest, "Madurai") == 0)
            return 424;
        else if (strcasecmp(dest, "Dindigul") == 0)
            return 424;
        else if (strcasecmp(dest, "Villupuram") == 0)
            return 481;
        else if (strcasecmp(dest, "Chennai") == 0)
            return 420;
    }
    return random;
}

void create_pnr(int number, char *result) {
    srand(time(NULL));
    int random_number = 0, temp_rand = 0;
    for (int i = 0; i < 7; i++) {
        temp_rand = rand() % 10;
        random_number = random_number * 10 + temp_rand;
    }
    char temp_char1[13], temp_char2[10];
    sprintf(temp_char1, "%d", number);
    sprintf(temp_char2, "%07d", random_number);
    temp_char1[3]='-';
    strcat(temp_char1, temp_char2);
    strcpy(result, temp_char1);
}

void generate_pnr(char *no,char *destination,char* pnr_string) {
    srand(time(NULL));

    int num = rnumber(no,destination);
    create_pnr(num, pnr_string);
        while (check_pnr(pnr_string) == 1) {
        create_pnr(num, pnr_string);
        
    }
}

int test() {
    struct admin test_ticket = {
        .pnr = "092-1223567", 
        .username = "admin",
        .date = 15,
        .train_name = "Express",
        .train_no = "EXP123",
        .arrival = "City A",
        .destination = "City B",
        .seats = {1, 2, 3, 4, 5, 6, 6},
        .price = 100.0,
        .status = {"confirmed", "confirmed", "confirmed", "confirmed", "confirmed", "confirmed"},
        .arrt = "10:00 AM",
        .destt = "2:00 PM",
        .coach = "S1",
        .acc="abc77777",
        .pin=5432
    };

    write_admin(test_ticket);

    pnr_status();

    return 0;
}

int another_test()
{
    char pnr_string[15];
    generate_pnr("12675","Coimbatore",pnr_string);    
    printf("%s\n", pnr_string);
}
