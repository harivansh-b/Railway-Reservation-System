#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/Structures.h"
#include "../header/Colors.h"

#include "../code/Waiting_list_probability.c"

struct train {
    char coach_type[20];
    char coach_no[20];
    int seat_no;
    int stations[MAX_STATIONS];
};

struct swaiting_list {
    char pnr[15];
    int date;
    char train_name[1000];
    char train_id[10];
    char arrival[MAX_LINE];
    char destination[MAX_LINE];
    char wl_pos[10];
    char coach[MAX_LINE];
};

int availablity_seats[MAX_LINE];
int availablity;

int find_station(char *station_name, char *stations[], int num_stations);
void show_availablity(int dd, char *coach_type, char *arrival_station_name, char *destination_station_name, char *file_name, int max_coaches);
float ticket_price(struct Station array[][100], char coach[], int start, int train_index, char arr[], char dest[]);
int day_or_night(int number);
float price(char arr[], char dept[], char coach[], char name[]);
float check_train(int dd, char arr[], char dept[], char coach[], char arrt[], char dest[]);
void check_schedule(char train_name[]);
int menu();

int show_waiting_list(int dd, const char *train_name, const char *train_id, char *arrival_station_name, char *destination_station_name,char *coach_type, char *coach_no) {
    FILE *file1 = fopen("../../db/WaitingListDB.csv", "r");
    if (!file1) {
        perror("Error opening waiting_list.csv");
        return 0;
    }

    struct swaiting_list reader;
    char coach[100];
    snprintf(coach, sizeof(coach), "%s %s", coach_type, coach_no);

    char arrival[150], destination[150];
    sscanf(arrival_station_name, "%[^-]-in", arrival);
    sscanf(destination_station_name, "%[^-]-out", destination);

    char wl_read[1000];
    int wl_count = 0;

    if (fgets(wl_read, sizeof(wl_read), file1) == NULL) {
        fclose(file1);
        perror("Error reading header from waiting_list.csv");
        return 0;
    }

    while (fgets(wl_read, sizeof(wl_read), file1) != NULL) {
        sscanf(wl_read, "%[^,],%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]\n",
               reader.pnr, &reader.date, reader.train_name, reader.train_id,
               reader.arrival, reader.destination, reader.wl_pos, reader.coach);
        if ((dd == reader.date) && 
            (strncasecmp(reader.train_name, train_name, strlen(train_name)) == 0) && 
            (strncasecmp(reader.train_id, train_id, strlen(train_id)) == 0) && 
            (strncasecmp(reader.arrival, arrival, strlen(arrival)) == 0) && 
            (strncasecmp(reader.destination, destination, strlen(destination)) == 0) && 
            (strncasecmp(reader.coach, coach, strlen(coach)) == 0)) {
                wl_count++;
        }
    }
    fclose(file1);

    return wl_count+1;
}

int find_station(char *station_name, char *stations[], int num_stations) {
    for (int i = 0; i < num_stations; i++) {
        if (strncasecmp(station_name, stations[i], strlen(station_name)) == 0)
            return i;
    }
    return -1;
}

void show_availablity(int dd, char *coach_type, char *arrival_station_name, char *destination_station_name, char *file_name, int max_coaches) 
{
    char file_path[10000];
    snprintf(file_path, sizeof(file_path), "../../db/%d/%s", dd, file_name);
    FILE *file = fopen(file_path, "r");
        
    if (!file) {
        printf(DARK_RED "\nUnable to open file: %s\n" RESET, file_path);
        exit(1);
    }

    struct train row[MAX_LINE];
    int line_count = 0;

    char *stations[MAX_STATIONS];
    int num_stations = 0;

    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        while (token != NULL && num_stations < MAX_STATIONS) {
            if (num_stations >= 3) {
                stations[num_stations - 3] = strdup(token);
                if (!stations[num_stations - 3]) {
                    printf(DARK_RED "\nMemory allocation failed.\n" RESET);
                    fclose(file);
                    exit(1);
                }
            }
            token = strtok(NULL, ",");
            num_stations++;
        }
    }
    num_stations -= 3;

    int arrival_station_index = find_station(arrival_station_name, stations, num_stations);
    int destination_station_index = find_station(destination_station_name, stations, num_stations);

    if (arrival_station_index == -1 || destination_station_index == -1) {
        printf(DARK_RED "Invalid arrival or destination station.\n" RESET);
        fclose(file);
        for (int i = 0; i < num_stations; i++) {
            free(stations[i]);
        }
        return;
    }
    
    for (int i = 1; i <= max_coaches; i++) 
    {
        char coach_no[100];
        snprintf(coach_no, sizeof(coach_no), "Coach-%d", i);

        int availablity = 0,line_count=0; 
        rewind(file);
        fgets(line, sizeof(line), file);

        while (fgets(line, sizeof(line), file)) 
        {
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
            if (strcasecmp(coach_type, row[line_count].coach_type) == 0 && strcmp(coach_no, row[line_count].coach_no) == 0 && can_book) 
                availablity++;
        }

        if (availablity > 0) 
        {
            printf(GREEN "%s %s : AVL %d seats\n" RESET, coach_type, coach_no, availablity);
        } 
        else {
            char train_name[100],train_id[10],arrival[100],destination[100];
            sscanf(file_name, "%[^-]-%[^.].csv", train_name, train_id);
            sscanf(arrival_station_name,"%[^-]-in",arrival);
            sscanf(destination_station_name,"%[^-]-out",destination);
            int waiting_list_pos = show_waiting_list(dd, train_name, train_id, arrival_station_name, destination_station_name, coach_type, coach_no);
            printf(RED "%s %s : WL- %d", coach_type, coach_no, waiting_list_pos);
            printf(" -- %.2f%%\n" RESET, waiting_to_cnf_probability(dd, train_name, train_id, arrival, destination , coach_type, coach_no, waiting_list_pos));
        }
        line_count++;
    }
    fclose(file);
    for (int i = 0; i < num_stations; i++) 
        free(stations[i]);
}

float ticket_price(struct Station array[][100], char coach[], int start, int train_index,char arr[], char dest[]) {
    float tkpt = 0;
    int j = start;
    float rate;

    if (strcasecmp(coach, "EC") == 0) {
        rate = 3.5;
    } else if (strcasecmp(coach, "AC") == 0) {
        rate = 2.0;
    } else if (strcasecmp(coach, "CC") == 0) {
        rate = 1.25;
    }else if (strcasecmp(coach, "1AC") == 0) {
        rate = 3.5;
    }else if (strcasecmp(coach, "2AC") == 0) {
        rate = 2;
    }else if (strcasecmp(coach, "3AC") == 0) {
        rate = 1.25;
    }else if (strcasecmp(coach, "Sleeper") == 0) {
        rate = 0.5;
    } else {
        return -1;
    }

    while (strcasecmp(array[train_index][j].station, dest) != 0) {
        tkpt += array[train_index][j].distance * rate;
        j++;
    }

    tkpt += array[train_index][j].distance * rate;

    return tkpt;
}

int day_or_night(int number)
{
    int i=0;
    struct Train array[100];
    FILE* fp;
    fp=fopen("../../db/StationsDB.csv","r");
    if(fp==NULL){
        printf(DARK_RED "Error opening file\n" RESET);
        return 0;
    }
    while(fscanf(fp,"%[^,],%[^,],%[^,],%d\n",array[i].t_name,array[i].t_n,array[i].tron,&array[i].stations)!=EOF){
        int train_number=atoi(array[i].t_n);
        if(train_number==number){
            if(strcmp(array[i].tron,"dt")==0){
                return 1;
            }
            else if(strcmp(array[i].tron,"nt")==0){
                return 2;
            }
        }
    }
    return 0;
}

float price(char arr[], char dept[], char coach[], char name[]) {
    FILE *fp;
    fp = fopen("../../db/StationsDB.csv", "r");
    if (fp == NULL) {
        printf(DARK_RED "FILE NOT FOUND\n" RESET);
        return 0;
    }

    struct Train arr_t[100];
    struct Station arr_s[100][100];

    int train_count = 0;
    int station_count[100] = {0};

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char t_name[50], t_n[10], tron[10];
        int stations;
        sscanf(line, "%[^,],%[^,],%[^,],%d\n", t_name, t_n, tron, &stations);

        if ((strcasecmp(tron, "dt") == 0) || (strcasecmp(tron, "nt") == 0)) {
            struct Train train;
            strcpy(train.t_name, t_name);
            strcpy(train.t_n, t_n);
            strcpy(train.tron, tron);
            train.stations = stations;
            arr_t[train_count++] = train;
        } else {
            struct Station station;
            strcpy(station.station, t_name);
            strcpy(station.arrival, t_n);
            strcpy(station.departure, tron);
            station.distance = stations;
            arr_s[train_count - 1][station_count[train_count - 1]++] = station;
        }
    }

    fclose(fp);
    for (int x = 0; x < train_count; x++) {
        if (strcasecmp(arr_t[x].t_name, name) == 0) {
            int arro = -1, depto = -1;
            float tktp = 0;
            for (int y = 0; y < station_count[x]; y++) {
                if (strcasecmp(arr_s[x][y].station, arr) == 0) {
                    arro = y;
                }
                if (strcasecmp(arr_s[x][y].station, dept) == 0) {
                    depto = y;
                }
            }
            if (arro >= 0 && depto >= 0 && depto > arro) {
                tktp = ticket_price(arr_s, coach, arro, x, arr, dept);
                return tktp;
            }
        }
    }
    return 0;
}

float check_train(int dd, char arr[], char dept[], char coach[], char arrt[], char deptt[]) 
{
    FILE *fp = fopen("../../db/StationsDB.csv", "r");
    if (fp == NULL) {
        printf(DARK_RED "FILE NOT FOUND\n" RESET);
        exit(1);
    }

    struct Train arr_t[100];
    struct Station arr_s[100][100];

    int train_count = 0;
    int station_count[100] = {0};

    char day[4][10] = {"EC", "AC", "CC"};
    char night[4][10] = {"1AC", "2AC", "3AC", "Sleeper"};

    int is_day_coach = 0, is_night_coach = 0;
    for (int i = 0; i < 3; i++) {
        if (strcmp(coach, day[i]) == 0) {
            is_day_coach = 1;
            break;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (strcmp(coach, night[i]) == 0) {
            is_night_coach = 1;
            break;
        }
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char t_name[50], t_n[10], tron[10];
        int stations;
        sscanf(line, "%[^,],%[^,],%[^,],%d\n", t_name, t_n, tron, &stations);

        if (strcasecmp(tron, "dt") == 0 || strcasecmp(tron, "nt") == 0) {
            struct Train train;
            strcpy(train.t_name, t_name);
            strcpy(train.t_n, t_n);
            strcpy(train.tron, tron);
            train.stations = stations;
            arr_t[train_count++] = train;
        } else {
            struct Station station;
            strcpy(station.station, t_name);
            strcpy(station.arrival, t_n);
            strcpy(station.departure, tron);
            station.distance = stations;
            arr_s[train_count - 1][station_count[train_count - 1]++] = station;
        }
    }

    fclose(fp);

    int avai = 0;
    if (is_day_coach || is_night_coach) {
        for (int x = 0; x < train_count; x++) {
            if ((is_day_coach && strcmp(arr_t[x].tron, "dt") == 0) || (is_night_coach && strcmp(arr_t[x].tron, "nt") == 0)) {
                int arro = -1, depto = -1;
                float tktp = 0;
                char at[10], dt[10];
                for (int y = 0; y < station_count[x]; y++) {
                    if (strcasecmp(arr_s[x][y].station, arr) == 0) {
                        arro = y;
                        strcpy(at, arr_s[x][y].arrival);
                    }
                    if (strcasecmp(arr_s[x][y].station, dept) == 0) {
                        depto = y;
                        strcpy(dt, arr_s[x][y].arrival);
                    }
                }

                if (arro >= 0 && depto >= 0 && depto > arro) {
                    avai++;
                    printf(MINT "\n************************************************************" RESET);
                    printf(ANSI_COLOR_MAGENTA "\n%s " ANSI_COLOR_RESET, arr_t[x].t_name);
                    printf(ANSI_COLOR_CYAN "\nTrain No: %s " ANSI_COLOR_RESET, arr_t[x].t_n);
                    printf(ANSI_COLOR_YELLOW "\nScheduled time: %s - %s " ANSI_COLOR_RESET, at, dt);
                    tktp = ticket_price(arr_s, coach, arro, x, arr, dept);
                    printf(ANSI_COLOR_BLUE "\nTicket Price: %.2f\n" ANSI_COLOR_RESET, tktp);
                    strcpy(arrt,at);
                    strcpy(deptt,dt);

                    char file_name[100];
                    sprintf(file_name, "%s-%s.csv", arr_t[x].t_name, arr_t[x].t_n);
                    int max_coaches = 0;

                    char arrival[100], destination[100];
                    sprintf(arrival, "%s-in", arr);
                    sprintf(destination, "%s-out", dept);

                    if (strcasecmp(coach, "EC") == 0) max_coaches = 2;
                    else if (strcasecmp(coach, "AC") == 0) max_coaches = 5;
                    else if (strcasecmp(coach, "CC") == 0) max_coaches = 7;
                    else if (strcasecmp(coach, "1AC") == 0) max_coaches = 1;
                    else if (strcasecmp(coach, "2AC") == 0) max_coaches = 3;
                    else if (strcasecmp(coach, "3AC") == 0) max_coaches = 4;
                    else if (strcasecmp(coach, "Sleeper") == 0) max_coaches = 12;

                    show_availablity(dd, coach, arrival, destination, file_name, max_coaches);
                    printf(MINT "************************************************************\n" RESET);
                    return tktp;
                }
            }
        }
    }

    if (avai == 0) {
        printf(SALMON "No Trains Available\n" RESET);
    }
    return 0;
}

void search_stations(char arr[], char dept[], char coach[]) 
{
    FILE *fp = fopen("../../db/StationsDB.csv", "r");
    if (fp == NULL) {
        printf(DARK_RED"FILE NOT FOUND\n" RESET);
        exit(1);
    }

    struct Train arr_t[100];
    struct Station arr_s[100][100];

    int train_count = 0;
    int station_count[100] = {0};

    char day[4][10] = {"EC", "AC", "CC"};
    char night[4][10] = {"1AC", "2AC", "3AC", "Sleeper"};

    int is_day_coach = 0, is_night_coach = 0;
    for (int i = 0; i < 3; i++) {
        if (strcmp(coach, day[i]) == 0) {
            is_day_coach = 1;
            break;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (strcmp(coach, night[i]) == 0) {
            is_night_coach = 1;
            break;
        }
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char t_name[50], t_n[10], tron[10];
        int stations;
        sscanf(line, "%[^,],%[^,],%[^,],%d\n", t_name, t_n, tron, &stations);

        if (strcasecmp(tron, "dt") == 0 || strcasecmp(tron, "nt") == 0) {
            struct Train train;
            strcpy(train.t_name, t_name);
            strcpy(train.t_n, t_n);
            strcpy(train.tron, tron);
            train.stations = stations;
            arr_t[train_count++] = train;
        } else {
            struct Station station;
            strcpy(station.station, t_name);
            strcpy(station.arrival, t_n);
            strcpy(station.departure, tron);
            station.distance = stations;
            arr_s[train_count - 1][station_count[train_count - 1]++] = station;
        }
    }

    fclose(fp);

    int avai = 0;
    if (is_day_coach || is_night_coach) {
        for (int x = 0; x < train_count; x++) {
            if ((is_day_coach && strcmp(arr_t[x].tron, "dt") == 0) || (is_night_coach && strcmp(arr_t[x].tron, "nt") == 0)) {
                int arro = -1, depto = -1;
                float tktp = 0;
                char at[10], dt[10];
                for (int y = 0; y < station_count[x]; y++) {
                    if (strcasecmp(arr_s[x][y].station, arr) == 0) {
                        arro = y;
                        strcpy(at, arr_s[x][y].arrival);
                    }
                    if (strcasecmp(arr_s[x][y].station, dept) == 0) {
                        depto = y;
                        strcpy(dt, arr_s[x][y].arrival);
                    }
                }

                if (arro >= 0 && depto >= 0 && depto > arro) {
                    avai++;
                    printf(MINT "\n************************************************************" RESET);
                    printf(ANSI_COLOR_MAGENTA "\n%s " ANSI_COLOR_RESET, arr_t[x].t_name);
                    printf(ANSI_COLOR_CYAN "\nTrain No: %s" ANSI_COLOR_RESET, arr_t[x].t_n);
                    printf(ANSI_COLOR_YELLOW "\nScheduled time: %s - %s" ANSI_COLOR_RESET, at, dt);
                    tktp = ticket_price(arr_s, coach, arro, x, arr, dept);
                    printf(ANSI_COLOR_BLUE "\nTicket Price: %.2f\n" ANSI_COLOR_RESET, tktp);
                    printf(MINT "************************************************************\n" RESET);
                }
            }
        }
    }

    if (avai == 0) {
        printf(SALMON "No Trains Available\n" RESET);
    }
}

void check_schedule(char train_name[]) {
    FILE *fp;
    fp = fopen("../../db/StationsDB.csv", "r");
    if (fp == NULL) {
        printf(DARK_RED "FILE NOT FOUND\n" RESET);
        return;
    }

    struct Train arr_t[100];
    struct Station arr_s[100][100];

    int train_count = 0;
    int station_count[100] = {0};

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char t_name[50], t_n[10], tron[10];
        int stations;
        sscanf(line, "%[^,],%[^,],%[^,],%d\n", t_name, t_n, tron, &stations);

        if ((strcasecmp(tron, "dt") == 0) || (strcasecmp(tron, "nt") == 0)) {
            struct Train train;
            strcpy(train.t_name, t_name);
            strcpy(train.t_n, t_n);
            strcpy(train.tron, tron);
            train.stations = stations;
            arr_t[train_count++] = train;
        } else {
            struct Station station;
            strcpy(station.station, t_name);
            strcpy(station.arrival, t_n);
            strcpy(station.departure, tron);
            station.distance = stations;
            arr_s[train_count - 1][station_count[train_count - 1]++] = station;
        }
    }

    fclose(fp);

    int avai = 0;
    for (int x = 0; x < train_count; x++) {
        if (strcasecmp(arr_t[x].t_name, train_name) == 0) {
            avai++;
            printf(BOLD ORANGE"%s-%s\n" RESET,arr_t[x].t_name,arr_t[x].t_n);
            for (int y = 0; y < arr_t[x].stations; y++) {
                printf(BOLD LIME"%s %s %s\n" RESET, arr_s[x][y].station, arr_s[x][y].arrival, arr_s[x][y].departure);
            }
        }
        printf("\n");
    }
    if (avai == 0) {
        printf(SALMON "Train Not found\n" RESET);
    }
}

int menu() {
    int opt = 0;
    while (1) {
        printf("1-Train Name\n2-Search by Station\n3-price\n4-Day or Night\n5-Exit\n");
        printf("\nEnter: ");
        scanf("%d", &opt);
        if (opt == 1) {
            char train[50];
            printf("Enter the Train name: ");
            getchar();
            fgets(train, sizeof(train), stdin);
            train[strcspn(train, "\n")] = '\0'; 
            check_schedule(train);
        } else if (opt == 2) {
            char arrt[100],dept[100];
            int dd;
            printf("Enter the date:");
            scanf("%d",&dd);
            fflush(stdin);
            char arr[50];
            char dest[50];
            printf("Enter the Arrival: ");
            scanf("%s", arr);
            printf("Enter the Destination: ");
            scanf("%s", dest);
            check_train(dd, arr, dest, "EC", arrt, dept);
        } else if (opt == 3) {
            char arr[50], dest[50], train_name[50];
            printf("Enter the Arrival: ");
            scanf("%s", arr);
            printf("Enter the Destination: ");
            scanf("%s", dest);
            getchar();
            printf("Enter the Train name: ");
            fgets(train_name, sizeof(train_name), stdin);
            train_name[strcspn(train_name, "\n")] = '\0'; 
            float priceo = price(arr, dest, "EC", train_name);
            printf("Ticket Price: %.2f\n", priceo);
        }
        else if(opt==4){
                int result=0;
                result=day_or_night(12635);
                printf("%d",result);
        } 
        else if (opt == 5) {
            break;
        } 
        else {
            printf("Enter a Valid Option\n");
        }
    }
    return 0;
}
