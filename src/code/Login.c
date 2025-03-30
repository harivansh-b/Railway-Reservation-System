#include <stdio.h>
#include <stdlib.h>

#include "../header/Colors.h"

#include "../code/Login_functions.c"

int login_main(char *username)
{
    int ch;
    int result=0;
    while (result!=1)
    {	
        lspecial();
        printf(WHITE BOLD"\n\t\t\t\t\t\t\t\t\tLOGIN PAGE\n\n\n" RESET);
        printf(GREEN "\t\t\t\t\t\t\t\t1: Login\n" RESET);
        printf(YELLOW "\t\t\t\t\t\t\t\t2: Signup\n" RESET);
        printf(MAGENTA "\t\t\t\t\t\t\t\t3: Update Password\n" RESET);
        printf(BLACK "\t\t\t\t\t\t\t\t4: Exit\n\n" RESET);
        printf(LIME_GREEN "\t\t\t\t\t\t\t\tPlease choose your choice: ");
        scanf("%d", &ch);
        printf(RESET);
        lspecial();
        

        if (ch==1){
	        result=login_successful(username);
            if(result)
                printf(GREEN "\n%s has logged into the system\n" RESET, username);

        }

        else if (ch==2)        
            signup_successful();

        else if(ch==3)
            update_successfully();
        else if (ch==4)
        {
            printf(WHITE "\n\nExiting the Application\n" RESET);
            exit(0);
        }

        else
            printf(RED_back "\n\nWrong Choice!! Please enter again.\n"RESET);
	
	}
	
	return result;
}
