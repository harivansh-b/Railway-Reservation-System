#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_LINE_LENGTH 5000
#define MAX_TRAINS 10
#define MAX_STATIONS 20
#define MAX_WAITING 1000
#define MAX_LINE 2500
#define MAX_LENGTH 500

struct trains {
    char coach_type[20];
    char coach_no[20];
    int seat_no;
    int stations[MAX_STATIONS];
};


struct update {
    char arrival[100];
    char destination[100];
};

struct waiting {
    char pnr[13];
    char train_name[250];
    char train_number[10];
    char arrival[50];
    char destination[50];
    int date;
    int seat;
    char coach[10];
};

struct Train {
    char t_name[50];
    char t_n[10];
    char tron[10];
    int stations;
};

struct Station {
    char station[50];
    char arrival[10];
    char departure[10];
    int distance;
};

struct admin {
    char pnr[13];
    char username[100];
    int date;
    char train_name[50];
    char train_no[20];
    char arrival[50];
    char destination[50];
    int seats[7];
    float price;
    char status[6][20];
    char arrt[20];
    char destt[20];
    char coach[20];
    char acc[10];
    int pin;
};


struct waiting_list {
    char pnr[15];
    int date;
    char train_name[500];
    char train_id[10];
    char arrival[MAX_LINE];
    char destination[MAX_LINE];
    char wl_pos[6];
    char coach[MAX_LINE];
};

#define MAX_ACCOUNTS 500

struct bank {
    char account[10];
    int pin;
    float balance;
};

#endif