#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "../header/Colors.h"

int check_length(char *string);
int check_upper(char *string);
int check_lower(char *string);
int check_special(char *string);
int check_digit(char *string);
int check_password(char *password);


int check_length(char *string)
{
    return (strlen(string) >= 8 && strlen(string) < 16) ? 1 : 0;
}

int check_upper(char *string)
{
    for (int i = 0; i < strlen(string); i++)
    {
        int ascii = string[i];
        if (ascii >= 65 && ascii <= 90)
        {
            return 1;
        }
    }
    return 0;
}

int check_lower(char *string)
{
    for (int i = 0; i < strlen(string); i++)
    {
        int ascii = string[i];
        if (ascii >= 97 && ascii <= 122) 
        {
            return 1;
        }
    }
    return 0;
}

int check_special(char *string) 
{
    for (int i = 0; i < strlen(string); i++) {
        int ascii = string[i];
        if (!((ascii >= 65 && ascii <= 90) || (ascii >= 97 && ascii <= 122) || (ascii >= 48 && ascii <= 57))) { 
            return 1; 
        }
    }
    return 0;
}

int check_digit(char *string)
{
    for (int i = 0; i < strlen(string); i++)
    {
        if (isdigit(string[i]))
        {
            return 1;
        }
    }
    return 0;
}

int check_password(char *password)
{

    int length = strlen(password);
    int crt_length = check_length(password), crt_upper = check_upper(password), crt_lower = check_lower(password), crt_special = check_special(password), crt_digit = check_digit(password);


    if (crt_length == 1 && crt_upper == 1 && crt_lower == 1 && crt_special == 1 && crt_digit == 1)
        return 1;
    else
    {
        if (crt_length == 0)
        {
            printf(RED_back "\nThe length of the password must be greater than 7 or lesser than 16\n"RESET);
        }
        else if (crt_upper == 0)
        {
            printf(RED_back "\nThe password must have at least one upper case\n"RESET);
        }
        else if (crt_lower == 0)
        {
            printf(RED_back "\nThe password must have at least one lower case\n"RESET);
        }
        else if (crt_digit == 0)
        {
            printf(RED_back "\nThe password must have at least one digit\n"RESET);
        }
        else if (crt_special == 0)
        {
            printf(RED_back "\nThe password must have at least one special character\n"RESET);
        }
        printf(RED "\nPlease try again\n"RESET);
    }
    return 0;
}

