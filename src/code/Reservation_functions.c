#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "../header/Colors.h"

#include "../code/Booking_functions.c"
#include "../code/Stations_search.c"
#include "../code/Payment_bank.c"

void special();
void is_valid_doj(int *dd, int *mm, int *yyyy);
void is_valid_arrival(char *arrival);
void is_valid_destination(char *destination);
int are_stations_different(char *arrival, char *destination);
void is_valid_coach_type(char *coach_type);
int is_valid_ticket();
int isSeatDuplicate(int seats[], int index);


struct train_details {
    char name[MAX_LENGTH];
    char no[10];
};

struct train_details t[] = {
    {"Kovai SF Express", "12675"},
    {"Kovai SF Express", "12676"},
    {"Kashi Tamil Express", "16367"},
    {"Coimbatore Nagercoil SF Express", "22668"},
    {"Nagercoil Coimbatore SF Express", "22667"},
    {"Cheran SF Express", "12674"},
    {"Vaigai Express", "12635"}
};

int no_t = sizeof(t) / sizeof(t[0]);

char coaches[][100] = {"EC", "AC", "CC", "1AC", "2AC", "3AC", "Sleeper"};
int num_coaches = sizeof(coaches) / sizeof(coaches[0]);

char railway_stations[][200] = {"Chennai", "Salem", "Erode", "Coimbatore", "Villupuram", "Thanjavur", "Dindigul", "Madurai", "Tirunelveli", "Nagercoil", "Kanyakumari"};
int num_railway_stations = sizeof(railway_stations) / sizeof(railway_stations[0]);


int reservation(char *username) 
{
    int res = 0;
    while (!res) {
        special();
        printf(WHITE BOLD"\n\n\t\t\t\t\t\t\t\tTICKET BOOKING" RESET);
        struct admin ticket;

        strcpy(ticket.username,username);
        int dd,mm,yyyy;
        is_valid_doj(&dd,&mm,&yyyy);
        ticket.date = dd;

        printf(LIME "\nThe stations are:\n" RESET);
        for(int i=0;i<num_railway_stations;i++)
            printf(ANSI_COLOR_GREEN "%s  " ANSI_COLOR_RESET,railway_stations[i]);
        printf("\n\n");

        char arrival[MAX_LENGTH], destination[MAX_LENGTH];
        is_valid_arrival(arrival);
        is_valid_destination(destination);

        strcpy(ticket.arrival,arrival);
        strcpy(ticket.destination,destination);

        if (!are_stations_different(arrival, destination)) {
            printf(SALMON "Arrival and destination stations cannot be the same. Please enter different stations.\n" RESET);
            continue;
        }
        char coach_type[MAX_LENGTH];
        is_valid_coach_type(coach_type);
        
        special();
        printf(WHITE BOLD"\n\t\t\t\t\t\t\t\tTICKET BOOKING" RESET);

        printf(GREEN BOLD "\nAvailable Trains:\n" RESET);

        float ticket_price=check_train(dd, arrival, destination, coach_type , ticket.arrt ,ticket.destt);

        int ch;
        printf(GREEN "\n\n1.Yes" RESET);
        printf(RED "\n2.No" RESET);
        printf(ORANGE"\nDo you want to book the tickets:");
        scanf("%d", &ch);
        printf( RESET);
        if (ch == 1) {
            fflush(stdin);

            char train_name[10000];
            printf(LIME BOLD"\nEnter the Train you wish to travel: ");
            fgets(train_name, sizeof(train_name), stdin);
            printf( RESET);
            train_name[strcspn(train_name,"\n")] = '\0';

            fflush(stdin);
            char train_no[10];
            printf(TEAL BOLD "Enter the train no: ");
            scanf("%s", &train_no);
            printf( RESET);
            fflush(stdin);

            int train_index = -1;
            for (int i = 0; i < no_t; i++) {
                if ((strcasecmp(train_name, t[i].name) == 0) && (strcasecmp(train_no, t[i].no)==0)) {
                    strcpy(ticket.train_name,train_name);
                    strcpy(ticket.train_no,train_no);
                    train_index=i;
                    break;
                }
            }

            if (train_index == -1) {
                printf(SALMON "\nInvalid train name. Please try again.\n" RESET);
                continue;
            }

            int coach_no;
            printf(GOLD "Enter the coach no: ");
            scanf("%d", &coach_no);
            printf( RESET);
            char coach[10];
            sprintf(coach, "Coach-%d", coach_no);

            char full_coach_type[75];
            sprintf(full_coach_type, "%s %s", coach_type, coach);

            if (!((strcmp(full_coach_type, "EC Coach-1") == 0) ||
                  (strcmp(full_coach_type, "EC Coach-2") == 0) ||
                  (strcmp(full_coach_type, "AC Coach-1") == 0) ||
                  (strcmp(full_coach_type, "AC Coach-2") == 0) ||
                  (strcmp(full_coach_type, "AC Coach-3") == 0) ||
                  (strcmp(full_coach_type, "AC Coach-4") == 0) ||
                  (strcmp(full_coach_type, "AC Coach-5") == 0) ||
                  (strcmp(full_coach_type, "CC Coach-1") == 0) ||
                  (strcmp(full_coach_type, "CC Coach-2") == 0) ||
                  (strcmp(full_coach_type, "CC Coach-3") == 0) ||
                  (strcmp(full_coach_type, "CC Coach-4") == 0) ||
                  (strcmp(full_coach_type, "CC Coach-5") == 0) ||
                  (strcmp(full_coach_type, "CC Coach-6") == 0) ||
                  (strcmp(full_coach_type, "CC Coach-7") == 0) ||
                  (strcmp(full_coach_type, "1AC Coach-1") == 0) ||
                  (strcmp(full_coach_type, "2AC Coach-1") == 0) ||
                  (strcmp(full_coach_type, "2AC Coach-2") == 0) ||
                  (strcmp(full_coach_type, "2AC Coach-3") == 0) ||
                  (strcmp(full_coach_type, "3AC Coach-1") == 0) ||
                  (strcmp(full_coach_type, "3AC Coach-2") == 0) ||
                  (strcmp(full_coach_type, "3AC Coach-3") == 0) ||
                  (strcmp(full_coach_type, "3AC Coach-4") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-1") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-2") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-3") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-4") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-5") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-6") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-7") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-8") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-9") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-10") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-11") == 0) ||
                  (strcmp(full_coach_type, "Sleeper Coach-12") == 0))) {
                printf(SALMON "\nInvalid coach type. Please try again.\n" RESET);
                fflush(stdin);
                continue;
            }

            strcpy(ticket.coach,full_coach_type);
            fflush(stdin);

            char train_database[MAX_LENGTH];
            sprintf(train_database, "%s-%s.csv", train_name,train_no);

            
            fflush(stdin);

            char full_arrival[MAX_LENGTH], full_destination[MAX_LENGTH];
            sprintf(full_arrival, "%s-in", arrival);
            sprintf(full_destination, "%s-out", destination);

            if(day_or_night(atoi(train_no))==0)
            {
                printf(SALMON "\nError occured in returning day or night!" RESET);
                continue;
            }
            
            memset(ticket.seats, 0, sizeof(ticket.seats));
            for (int i = 0; i < 6; i++) 
                strcpy(ticket.status[i], "NIL");
            int no_ticket = is_valid_ticket();
            ticket.seats[6]=no_ticket;

            char *layout_path=(day_or_night(atoi(train_no))==1) ? "../../db/DayTrainDB.txt":"../../db/NightTrainDB.txt";
            special();
            printf(WHITE BOLD "\n\t\t\t\t\t\t\t\tTICKET BOOKING\n\n" RESET);
            show_train(layout_path,full_coach_type);

            int wait_count=0;
            for (int i = 0; i < no_ticket; i++) {            
                printf(LIME_GREEN "\n\n\n\t\tAlloting %dth seats\n" RESET,i+1);                
                ticket.seats[i] = book_ticket(dd, coach_type, coach, full_arrival, full_destination, train_database, ticket.status[i]);
                if (isSeatDuplicate(ticket.seats, i) &&  ticket.seats[i]>0 ) {
                    fflush(stdin);
                    printf(SALMON "\nSeat not available or invalid input. Please try again." RESET);
                    i--;
                }
                else if(ticket.seats[i]==0)
                {
                    fflush(stdin);
                    printf(SALMON "\nSeat not available or invalid input. Please try again." RESET);
                    ticket.seats[6]--;
                }
                else if(ticket.seats[i]==-1)
                    wait_count++;
            }
            float final_ticket_price=ticket_price*ticket.seats[6];
            ticket.price=final_ticket_price;
            if(!ticket.seats[6])
            {
                printf(SALMON "\n Check other coaches" RESET);
                continue;
            }
            special();
            
            char wait_list[wait_count][7];

            for(int i=0,j=0;i<6;i++)
            {
                if(strncasecmp(ticket.status[i],"WL",2)==0)
                    strcpy(wait_list[j++],ticket.status[i]);
            }

            char bank_id[10];
            int pin;
            if(bank_menu(ticket.price,bank_id,&pin))
            {
                char pnr_string[12];
                generate_pnr(ticket.train_no,ticket.destination,pnr_string);
                strcpy(ticket.pnr,pnr_string);
                if(wait_count>0)
                    add_pnr_to_waitlist(ticket.pnr,ticket.date,ticket.train_name,ticket.train_no,ticket.arrival,ticket.destination,wait_count,wait_list,ticket.coach);
                strcpy(ticket.acc,bank_id);
                ticket.pin=pin;
                special();

                write_admin(ticket);

                show_ticket(ticket.pnr);
                res=1;
                return 1;
            }
            else
            {
                if (wait_count == 0)   
                    remove_ticket(dd, coach_type, coach, full_arrival, full_destination, train_database, ticket.seats[6], ticket.status, ticket.seats);
                else if (wait_count > 0) {
                    if(wait_count==ticket.seats[6])
                    {
                        remove_waiting_list("PNR");
                    }
                    else
                    {
                        remove_ticket(dd, coach_type, coach, full_arrival, full_destination, train_database, ticket.seats[6 - wait_count], ticket.status, ticket.seats);
                        remove_waiting_list("PNR");
                    }
                }
            }
        } else if (ch == 2) {
            printf(WHITE BOLD "\nThank you! Visit Again!!" RESET);
            fflush(stdin);
            res = 1;
        } else {
            printf(SALMON "\nInvalid Input! Enter 1 to book tickets or 2 to exit." RESET);
            fflush(stdin);
            continue;
        }
    }
    return res;
}


#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    #include <unistd.h>
    void special() {
        sleep(1.5);
        system("clear");
    }
#elif defined(_WIN32)
    #include <windows.h>
    void special() {
            Sleep(2000); 
            system("cls");
            printf(BLUE BOLD"************************************************************************************************************************************************************\n" RESET);
            printf(GOLD BOLD"\n\t\t\t\t\t\t\t\tRAILWAY RESERVATION SYSTEM\n\n" RESET);
            printf(BLUE BOLD"************************************************************************************************************************************************************\n" RESET);

    }
    void especial()
    {
        printf(SILVER BOLD "\n\nPress any key to continue..." RESET);
        _getch(); // Use _getch() to wait for a key press without requiring the user to press Enter
        Sleep(1000);
        system("cls");
        printf(BLUE BOLD"************************************************************************************************************************************************************\n" RESET);
        printf(GOLD BOLD"\n\t\t\t\t\t\t\t\tRAILWAY RESERVATION SYSTEM\n\n" RESET);
        printf(BLUE BOLD"************************************************************************************************************************************************************\n" RESET);
    }
#else
    void special() {
        printf("Unknown OS\n");
    }
#endif

void is_valid_doj(int *dd, int *mm, int *yyyy) {
    while (1) {
        printf(YELLOW "\nEnter the date of journey (dd-mm-yyyy): ");
        scanf("%d-%d-%d", dd, mm, yyyy);
        printf(RESET);
        if (!((*dd >= 1 && *dd <= 31) && (*mm==3) && (*yyyy==2025))) {
            printf(SALMON "Wrong date entered, please enter a valid date for July 2024 (1 to 31):\n" RESET);
            while (getchar() != '\n');
        } else {
            break;
        }
    }
}

void is_valid_arrival(char *arrival) {
    char input[200];
    int valid_station = 0;
    
    while (!valid_station) {
        printf(BLUE BOLD"Enter the Arrival Station:");
        scanf("%s", input);
        printf(RESET);
        for (int i = 0; i < num_railway_stations; i++) {
            if (strcasecmp(input, railway_stations[i]) == 0) {
                strcpy(arrival, input);
                valid_station = 1;
                break;
            }
        }
        
        if (!valid_station) {
            printf(SALMON "Invalid station. Please enter a valid station.\n" RESET);
        }
    }
}

void is_valid_destination(char *destination) {
    char input[200];
    int valid_station = 0;
    
    while (!valid_station) {
        printf(LIME_GREEN BOLD"Enter the Destination Station:");
        scanf("%s", input);
        printf(RESET);
        for (int i = 0; i < num_railway_stations; i++) {
            if (strcasecmp(input, railway_stations[i]) == 0) {
                strcpy(destination, input);
                valid_station = 1;
                break;
            }
        }
        
        if (!valid_station) {
            printf(SALMON "Invalid station. Please enter a valid station.\n" RESET);
        }
    }
}


int are_stations_different(char *arrival, char *destination) {
    return strcasecmp(arrival, destination) != 0;
}

void is_valid_coach_type(char *coach_type) {
    printf(BOLD MAGENTA "\nThe coaches are:\n");
    for (int i = 0; i < num_coaches; i++) {
        printf(LAVENDER "%s\t" RESET, coaches[i]);
    }

    fflush(stdin);
    printf(BOLD PURPLE "\nEnter the coach type: ");
    scanf("%[^\n]",coach_type);
    printf(RESET);
    if (!((strcasecmp(coach_type, "EC") == 0) ||
          (strcasecmp(coach_type, "AC") == 0) ||
          (strcasecmp(coach_type, "CC") == 0) ||
          (strcasecmp(coach_type, "1AC") == 0) ||
          (strcasecmp(coach_type, "2AC") == 0) ||
          (strcasecmp(coach_type, "3AC") == 0) ||
          (strcasecmp(coach_type, "Sleeper") == 0))) {
        printf(SALMON "\nInvalid Input! Enter a valid coach type." RESET);
        fflush(stdin);
        is_valid_coach_type(coach_type);
    }
}

int is_valid_ticket() {
    int tickets;

    while (1) {
        printf(BOLD TURQUOISE "Enter the number of tickets you want to book (max 6): ");
        if (scanf("%d", &tickets) != 1) {
            printf(RESET);
            while (getchar() != '\n');
            printf(SALMON "Invalid input. Please enter a number.\n" RESET);
        } else if (tickets < 1 || tickets > 6) {
            printf(SALMON "Only 1 to 6 tickets can be booked at a time! Please try again.\n" RESET);
        } else {
            break;
        }
    }

    return tickets;
}

int isSeatDuplicate(int seats[], int index) {
    for (int j = 0; j < index; j++) {
        if (seats[j] == seats[index]) {
            return 1;
        }
    }
    return 0;
}