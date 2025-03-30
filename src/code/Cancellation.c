#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../header/Structures.h"
#include "../header/Colors.h"

#include "../code/Reservation_functions.c"
#include "../code/Waiting_list_updation_1.c"

int available_seats[MAX_LINE];
int availablity;

int find_station_index2(char *station_name, char *stations[], int num_stations) {
    for (int i = 0; i < num_stations; i++) {
        if (strncasecmp(station_name, stations[i], strlen(station_name)) == 0)
            return i;
    }
    return -1;
}

int pass_pnr(struct admin* obj,char pareno[]){
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

void cancel_waiting_list(const char *pnr) {
    FILE *file1 = fopen("../../db/WaitingListDB.csv", "r");
    FILE *file2 = fopen("../../db/temp.csv", "w");
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

        if (strcasecmp(reader.pnr, pnr) == 0) 
        {
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

int cancel_ticket(char *pnr,int dd, char *coach_type, char *coach_no, char *arrival_station_name, char *destination_station_name, char *file_name,int num_seats_to_delete, char (*booked_status)[num_seats_to_delete], int seats_to_delete[num_seats_to_delete] ) 
{
    char file_path[10000];
    snprintf(file_path, sizeof(file_path), "../../db/%d/%s", dd, file_name);
    FILE *file1 = fopen(file_path, "r");
    FILE *file2 = fopen("../../db/temp.csv", "w");

    if (!file1 || !file2) {
        printf(DARK_RED "\nUnable to open file." RESET);
        exit(1);
    }

    struct trains row[MAX_LINE];
    int availablity = 0;
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

    int arrival_station_index = find_station_index2(arrival_station_name, stations, num_stations);
    int destination_station_index = find_station_index2(destination_station_name, stations, num_stations);

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
            available_seats[availablity++] = row[line_count].seat_no;
            seat_available = 1;
        }

        line_count++;
    }
    fclose(file1);
    fclose(file2);

    for(int x=0;x<num_seats_to_delete;x++)
    {
        if (seats_to_delete[x]==-1) {
            fclose(file1);        
            fclose(file2);
            for (int i = 0; i < num_stations; i++) {
                free(stations[i]);
            }

            cancel_waiting_list(pnr);
                   
            return 1;
        }

        else
        {
            char file_path[10000];
            snprintf(file_path, sizeof(file_path), "../../db/%d/%s", dd, file_name);
            FILE *file1 = fopen(file_path, "r");
            FILE *file2 = fopen("../../db/temp.csv", "w");

            if (!file1 || !file2) {
                printf(DARK_RED "\nUnable to open file." RESET);
                exit(1);
            }

            rewind(file1);
            fgets(line, sizeof(line), file1);
            fprintf(file2, "%s", line);

            for (int i = 0; i < line_count; i++) {
                int is_booked = 1;
                for (int j = arrival_station_index; j <= destination_station_index; j++) {
                    if (row[i].stations[j] == 0) {
                        is_booked = 0;
                        break;
                    }
                }

                if (strcmp(coach_type, row[i].coach_type) == 0 && strcmp(coach_no, row[i].coach_no) == 0 && is_booked && row[i].seat_no == seats_to_delete[x]) {
                    for (int j = arrival_station_index; j <= destination_station_index; j++) {
                        row[i].stations[j] = 0;
                    }
                    booked = 1;
                    seats_to_delete[x] = 0;
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
                printf(DARK_RED "\nError deleting original file." RESET);
                exit(1);
            }

            if (rename("../../db/temp.csv", file_path) != 0) {
                printf(DARK_RED "\nError renaming temporary file." RESET);
                exit(1);
            }
        }
    }
}

float deduct(int date,float amt,char coach[]){
    time_t current_time;
    struct tm *local_time;
    current_time = time(NULL);
    local_time = localtime(&current_time);
    int a=local_time->tm_mday;
    int t_day=30-a+date;
    float r_amt=0;
    if(t_day>2){
        if(strcmp(coach,"EC")==0){
            r_amt=amt-240;
        }
        else if(strcmp(coach,"AC")==0){
            r_amt=amt-180;
        }
        else if(strcmp(coach,"CC")==0){
            r_amt=amt-60;
        }
        else if(strcmp(coach,"1AC")==0){
            r_amt=amt-240;
        }
        else if(strcmp(coach,"2AC")==0){
            r_amt=amt-200;
        }
        else if(strcmp(coach,"3AC")==0){
            r_amt=amt-180;
        }
        else if(strcmp(coach,"Sleeper")==0){
            r_amt=amt-120;
        }
    }
    else if(t_day==2){
        r_amt=0.75 * amt;
    }
    else if(t_day==1){
        r_amt=0.50 * amt;
    }

    return r_amt;
}


int cancel_main()
{
    printf(WHITE BOLD "\n\t\t\t\t\t\t\t\t    CANCELLATION PAGE\n\n" RESET);
    struct admin pnr_info;
    char pnr[20];
    printf(GREEN "\nEnter pnr no:");
    scanf("%s",pnr);
    printf(RESET);

    if(!pass_pnr(&pnr_info,pnr))
    {
        printf(RED "PNR not found ! Try again" RESET);
        return -1;
    }

    show_ticket(pnr_info.pnr);

    int ch;
    printf(LIME_GREEN "\n\n1: Yes\n" RESET);
    printf(RED "2: No\n" RESET);
    printf(ORANGE "Do you want to cancel the ticket: ");
    scanf("%d",&ch);
    printf(RESET);
    if(ch==2)
    {
        printf(WHITE "\nThank you visit again!" RESET);
        return 0;
    }
    
    int seat_num=0,wait_num=0,seats[6];
    int pos=0,p=0;
    for(int i=0;i<6;i++){
        if(pnr_info.seats[i]!=-1 && pnr_info.seats[i]!=0)
            seats[seat_num++]=pnr_info.seats[i];
        else if(pnr_info.seats[i]==-1)
            wait_num++;     
    }
    for(int i=0;i<6;i++){
        if(pnr_info.seats[i]==-1){
            p=i;
            break;
        }
    }
    char str[8];
    strcpy(str,pnr_info.status[p]);
    char *start = strstr(str, "WL");
    
    if (start != NULL) {
        start += 2; 
        pos = atoi(start);
    }

    char coach_type[1000];
    char coach_no[1000];
    sscanf(pnr_info.coach,"%s %s",coach_type,coach_no);

    char file_name[1000];
    sprintf(file_name,"%s-%s.csv",pnr_info.train_name,pnr_info.train_no);

    char arrival[500],destination[500];
    sprintf(arrival,"%s-in",pnr_info.arrival);
    sprintf(destination,"%s-out",pnr_info.destination);
    
    if(cancel_ticket(pnr_info.pnr,pnr_info.date,coach_type,coach_no,arrival,destination,file_name,pnr_info.seats[6],pnr_info.status,pnr_info.seats))
    {
        convertion();
        printf(DARK_GREEN "\nDeleted the ticket successfully!!!\n" RESET);
        remove_pnr(pnr_info.pnr);
        printf(YELLOW "\nProcessing the payment refund...\n" RESET);
        float refund_amt=deduct(pnr_info.date,pnr_info.price,coach_type);
        payment_refund(pnr_info.acc,pnr_info.pin,refund_amt);

        for(int i=0;i<seat_num;i++)
            pnr_status_updation(pnr_info.arrival,pnr_info.destination,pnr_info.train_name,pnr_info.train_no,pnr_info.date,seats[i],pnr_info.coach);
        
        if(seat_num>0)
        {
        for(int i=0;i<seat_num+wait_num;i++)
            minus_waiting(pnr_info.train_name,pnr_info.train_no,pnr_info.date,pnr_info.coach);
        }
        else{
            
            for(int i=0;i<wait_num;i++)
            minus_waiting1(pnr_info.train_name,pnr_info.train_no,pnr_info.date,pnr_info.coach,pos);
        }
            
        convertion_back();    
    }
    
    return 0;
}