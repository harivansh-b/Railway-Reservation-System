#ifndef PNR_H
#define PNR_H

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

#endif // PNR_H
