#include <stdio.h>
#include <stdlib.h>

#include "../header/Structures.h"
#include "../header/Colors.h"

#include "../code/Login.c"
#include "../code/Reservation.c"

int main()
{
    char username[500];
    login_main(username);
    int result = 0, ch;
    int y;

    while (result != 1)
    {
        special();
        printf(WHITE BOLD "\n\n\t\t\t\t\t\t\t\t\tMAIN PAGE\n\n\n" RESET);
        printf(YELLOW "\t\t\t\t\t\t\t\t1: Reservation\n");
        printf(CYAN "\t\t\t\t\t\t\t\t2: PNR\n");
        printf(GREEN "\t\t\t\t\t\t\t\t3: Cancellation\n");
        printf(LIME BOLD "\t\t\t\t\t\t\t\t4: Check Confirmation Probability\n");
        printf(BLACK "\t\t\t\t\t\t\t\t5: Exit\n\n" RESET);
        printf(MAGENTA "\t\t\t\t\t\t\t\tEnter your choice: ");

        scanf("%d", &ch);
        printf(RESET);
        if (ch == 1)
        {
            result = reservation_main(username);
        }
        else if (ch == 2)
        {
            special();
            pnr_status();
            especial();
        }
        else if (ch == 3)
        {
            special();
            result = cancel_main();
            especial();
        }
        else if (ch == 4)
        {
            special();
            y = confirmation_probability();
            especial();
        }
        else if (ch == 5)
        {
            printf(WHITE BOLD "\nHave a great journey ahead!! Visit us again!!!\n" RESET);
            exit(1);
        }
        else
        {
            printf(RED "\nInvalid choice\n" RESET);
        }
        fflush(stdin);
    }

    return 0;
}