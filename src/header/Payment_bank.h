#ifndef PAYMENT_BANK_H
#define PAYMENT_BANK_H

int account_criteria(const char acc[]);
int check_account(const char acc[]);
int check_pin(const char acc[], int p);
void add_account();
void withdraw(const char acc[], int p, float b);
void deposit(const char acc[], int p, float b);
void show_balance(const char acc[], int p);
int bank_menu(float price, char* account_number, int* pin_number);
void payment_refund(char* acc, int p, float b);

#endif 
