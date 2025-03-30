#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../header/Structures.h"
#include "../header/Colors.h"

int account_criteria(const char acc[]);
int check_account(const char acc[]);
int check_pin(const char acc[], int p);
void add_account();
void withdraw(const char acc[], int p, float b);
void deposit(const char acc[], int p, float b);
void show_balance(const char acc[], int p);
void payment_refund(char* acc, int p, float b);
int bank_menu(float price, char* account_number, int* pin_number);

int account_criteria(const char acc[]) {
    if (strlen(acc) == 8) {
        if (acc[0] == 'a' && acc[1] == 'b' && acc[2] == 'c') {
            for (int i = 3; i < 8; i++) {
                if (!isdigit(acc[i])) {
                    return 0;
                }
            }
            return 1;
        }
    }
    return 0;
}

int check_account(const char acc[]) {
    FILE *fp = fopen("../../db/BankDB.csv", "r");
    if (fp == NULL) {
        return 1;
    }

    struct bank account;
    while (fscanf(fp, "%9[^,],%d,%f\n", account.account, &account.pin, &account.balance) == 3) {
        if (strcmp(acc, account.account) == 0) {
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return 1;
}

int check_pin(const char acc[], int p) {
    FILE *fp = fopen("../../db/BankDB.csv", "r");
    if (fp == NULL) {
        return 1;
    }

    struct bank account;
    while (fscanf(fp, "%9[^,],%d,%f\n", account.account, &account.pin, &account.balance) == 3) {
        if (strcmp(acc, account.account) == 0) {
            fclose(fp);
            if (p == account.pin) {
                return 0; 
            }
            return -1;
        }
    }
    fclose(fp);
    return 1;
}

void add_account() {
    FILE *fp = fopen("../../db/BankDB.csv", "a");
    if (fp == NULL) {
        printf(DARK_RED "File doesn't exist\n" RESET);
        return;
    }

    char new_acc[10];
    int new_p;
    float new_b;

    printf(TURQUOISE "Enter a New Account ID in format abcxxxxx: ");
    scanf("%9s", new_acc);
    printf(RESET);

    if (!account_criteria(new_acc)) {
        printf(RED "Invalid Account ID format\n" RESET);
        fclose(fp);
        return;
    }

    if (!check_account(new_acc)) {
        printf(RED "Account ID already exists\n" RESET);
        fclose(fp);
        return;
    }

    printf(MINT "Enter the New PIN number: ");
    scanf("%d", &new_p);
    printf(RESET);
    if (new_p < 1000 || new_p > 9999) {
        printf(RED "The PIN should have exactly 4 digits\n" RESET);
        fclose(fp);
        return;
    }

    printf(LAVENDER "Enter the Initial Balance you want to deposit: ");
    scanf("%f", &new_b);
    printf(RESET);
    fprintf(fp, "%s,%d,%.2f\n", new_acc, new_p, new_b);
    fclose(fp);
    printf(DARK_GREEN "Account Added Successfully\n" RESET);
}

void withdraw(const char acc[], int p, float b) {
    FILE *original_file = fopen("../../db/BankDB.csv", "r");
    FILE *temp_file = fopen("../../db/temp_bank.csv", "w");

    if (original_file == NULL || temp_file == NULL) {
        perror("Error opening file");
        return;
    }

    struct bank account;
    int account_exists = 0;

    while (fscanf(original_file, "%9[^,],%d,%f\n", account.account, &account.pin, &account.balance) == 3) {
        if (strcmp(acc, account.account) == 0) {
            if (p == account.pin) {
                if (b > account.balance) {
                    printf(RED "Insufficient balance\n" RESET);
                    fclose(original_file);
                    fclose(temp_file);
                    remove("../../db/temp_bank.csv");
                    return;
                }
                account.balance -= b;
                account_exists = 1;
            }
        }
        fprintf(temp_file, "%s,%d,%.2f\n", account.account, account.pin, account.balance);
    }

    fclose(original_file);
    fclose(temp_file);

    if (!account_exists) {
        printf(RED "Invalid PIN or Account ID\n" RESET);
        remove("../../db/temp_bank.csv");
        return;
    }

    if (remove("../../db/BankDB.csv") != 0) {
        perror("Error deleting original file");
        return;
    }

    if (rename("../../db/temp_bank.csv", "../../db/BankDB.csv") != 0) {
        perror("Error renaming temp file");
        return;
    }

    printf(GREEN "\nPayment Successful\n" RESET);
}

void deposit(const char acc[], int p, float b) {
    FILE *original_file = fopen("../../db/BankDB.csv", "r");
    FILE *temp_file = fopen("../../db/temp_bank.csv", "w");

    if (original_file == NULL || temp_file == NULL) {
        perror("Error opening file");
        return;
    }

    struct bank account;
    int account_exists = 0;

    while (fscanf(original_file, "%9[^,],%d,%f\n", account.account, &account.pin, &account.balance) == 3) {
        if (strcmp(acc, account.account) == 0) {
            if (p == account.pin) {
                account.balance += b;
                account_exists = 1;
            }
        }
        fprintf(temp_file, "%s,%d,%.2f\n", account.account, account.pin, account.balance);
    }

    fclose(original_file);
    fclose(temp_file);

    if (!account_exists) {
        printf(RED "Invalid PIN or Account ID\n" RESET);
        remove("../../db/temp_bank.csv");
        return;
    }

    if (remove("../../db/BankDB.csv") != 0) {
        perror("Error deleting original file");
        return;
    }

    if (rename("../../db/temp_bank.csv", "../../db/BankDB.csv") != 0) {
        perror("Error renaming temp file");
        return;
    }
    printf(DARK_GREEN "\nDeposit Successful\n" RESET);
}

void show_balance(const char acc[], int p) {
    FILE *fp = fopen("../../db/BankDB.csv", "r");
    if (fp == NULL) {
        printf(DARK_RED "File doesn't exist\n" RESET);
        return;
    }

    struct bank account;
    int account_exists = 0;

    while (fscanf(fp, "%9[^,],%d,%f\n", account.account, &account.pin, &account.balance) == 3) {
        if (strcmp(acc, account.account) == 0 && p == account.pin) {
            printf(GOLD "Account Balance: %.2f\n" RESET, account.balance);
            account_exists = 1;
            break;
        }
    }

    fclose(fp);

    if (!account_exists) {
        printf(RED "Invalid PIN or Account ID\n" RESET);
    }
}

void payment_refund(char* acc, int p, float b) {
    FILE *original_file = fopen("../../db/BankDB.csv", "r");
    FILE *temp_file = fopen("../../db/temp_bank.csv", "w");

    if (original_file == NULL || temp_file == NULL) {
        perror("Error opening file");
        return;
    }

    struct bank account;
    int account_exists = 0;

    while (fscanf(original_file, "%9[^,],%d,%f\n", account.account, &account.pin, &account.balance) == 3) {
        if (strcmp(acc, account.account) == 0) {
            if (p == account.pin) {
                printf(LIGHT_BLUE "Account No:%s\n" RESET,account.account);
                account.balance += b;
                printf(PURPLE "Refunded Amount:%0.f" RESET,b);
                account_exists = 1;
            }
        }
        fprintf(temp_file, "%s,%d,%.2f\n", account.account, account.pin, account.balance);
    }

    fclose(original_file);
    fclose(temp_file);

    if (!account_exists) {
        printf(RED "Invalid PIN or Account ID\n" RESET);
        remove("../../db/temp_bank.csv");
        return;
    }

    if (remove("../../db/BankDB.csv") != 0) {
        perror("Error deleting original file");
        return;
    }

    if (rename("../../db/temp_bank.csv", "../../db/BankDB.csv") != 0) {
        perror("Error renaming temp file");
        return;
    }
    printf(GREEN"\nPayment Refund Successful\n" RESET);
}

int bank_menu(float price,char* account_number,int* pin_number) 
{    
    printf(WHITE BOLD"\n\n\t\t\t\t\t\t\t\t\tABC Bank\n\n");
    int n;
    while (1) {
        printf(BLUE "\n1-Make a payment\n" RESET);
        printf(LIME_GREEN "2-Open a new Account\n" RESET);
        printf(YELLOW "3-Deposit\n" RESET);
        printf(MAGENTA "4-Check Balance\n" RESET);
        printf(CYAN "5-Cancel\n" RESET);
        printf("\n" PEACH "Enter your Choice:");
        scanf("%d", &n);
        printf(RESET);

        if (n == 1) {
            char acc[10];
            printf(INDIGO "Enter your Bank ID:");
            scanf("%s", acc);
            printf(RESET);
            if (check_account(acc) == 0) {
                int pin;
                printf(ORANGE "Enter the 4-digit PIN Number:");
                scanf("%d", &pin);
                printf(RESET);
                if (check_pin(acc, pin) == 0) {
                    
                    withdraw(acc, pin, price);
                    strcpy(account_number,acc);
                    *pin_number=pin;
                    return 1;
                } else {
                    printf(RED "Wrong PIN Number, retry payment\n" RESET);
                }
            } else {
                printf(RED "Wrong Account Number, retry payment\n" RESET);
            }
        } else if (n == 2) {
            add_account();
        } else if (n == 3) {
            char acc[10];
            int pin;
            float amount;
            printf(INDIGO "Enter the Account ID:");
            scanf("%s", acc);
            printf(RESET);
            if(check_account(acc)==0){
                printf(ORANGE "Enter the PIN:");
                scanf("%d", &pin);
                printf(RESET);
                if(check_pin(acc,pin)==0){
                    printf(BROWN BOLD "Enter the amount to deposit:");
                    scanf("%f", &amount);
                    printf(RESET);
                    deposit(acc, pin, amount);
                } else {
                    printf(RED "Wrong PIN Number\n" RESET);
                }
            } 
            else {
                printf(RED "Account does not exist\n" RESET);
            }
        } 
        else if (n == 4) {
            char acc[10];
            int pin;
            printf(INDIGO "Enter the Account ID:");
            scanf("%s", acc);
            printf(RESET);
            if(check_account(acc)==0)
            {
                printf(ORANGE "Enter the PIN:");
                scanf("%d", &pin);
                printf(RESET);
                if(check_pin(acc,pin)==0)
                {
                    show_balance(acc, pin);
                }
                else{
                    printf(RED "Wrong Pin number\n" RESET);
                }
            }
            else{
                printf(RED "Account Does not exists\n" RESET);
            }
        } else if (n == 5) {
            return 0;
        } else {
            printf(BLACK "Invalid Command\n" RESET);
        }
    }
}
