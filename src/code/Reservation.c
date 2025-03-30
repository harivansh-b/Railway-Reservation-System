#include <stdio.h>
#include <stdlib.h>

#include "../header/Structures.h"
#include "../header/Colors.h"

#include "../code/Cancellation.c"

int reservation_main(char *username)
{
   int ch;
   while(1)
   {
      special();
      printf(WHITE BOLD "\n\t\t\t\t\t\t\t\t     RESERVATION PAGE\n" RESET);
      printf(BLUE "\n\n\t\t\t\t\t\t\t\t1: View Train Schedule By Train Name\n");
      printf(GREEN "\t\t\t\t\t\t\t\t2: Search Trains By Stations\n");
      printf(YELLOW "\t\t\t\t\t\t\t\t3: Book Tickets\n");
      printf(MAGENTA "\t\t\t\t\t\t\t\t4: Go Back to Main Menu\n");
      printf(BLACK "\t\t\t\t\t\t\t\t5: Exit\n\n");
      printf(CYAN "\t\t\t\t\t\t\t\tEnter your choice:");
      scanf("%d", &ch);
      printf(RESET);
      if(ch==1)
      {
         special();
         printf(WHITE BOLD "\n\t\t\t\t\t\t\t\tTRAINS SEARCHING\n" RESET);
         char train[50];
         printf(TURQUOISE BOLD"\nEnter the Train name: ");
         fflush(stdin);
         fgets(train, sizeof(train), stdin);
         printf(RESET);
         train[strcspn(train, "\n")] = '\0';
         printf("\n"); 
         check_schedule(train);
         especial();
      }
      else if (ch==2)
      {
         special();
         printf(WHITE BOLD "\n\t\t\t\t\t\t\t\tTRAINS SEARCHING\n" RESET);
         char coach[100];
         char arr[50];
         char dest[50];
         printf(BOLD ORANGE "\nEnter the Arrival: ");
         scanf("%s", arr);
         printf(RESET);
         printf(BOLD PEACH "Enter the Destination: ");
         scanf("%s", dest);
         printf(RESET);
         printf(BOLD LIME_GREEN "Enter the coach:");
         scanf("%s",coach);
         printf(RESET);
         printf("\n");
         search_stations(arr, dest, coach);
         especial();
      }
      else if (ch==3)
      {
         reservation(username);
         especial();
      }
      else if (ch==4)
      {
         printf(WHITE BOLD "\nReturning to the Main Menu\n" RESET);
         return 0;
      }
      else if (ch==5)
      {
         printf(WHITE BOLD "\nHave a great journey ahead!! Visit us again!!!\n" RESET);
         exit(0);
      }
      else
      {
         printf("Invalid choice\n");
      }
      fflush(stdin);
   }
   return 0;
}
