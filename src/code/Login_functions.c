#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../header/Colors.h"

#include "../code/Password_validation.c"

#define MAX_USERS 1500

struct user_data {
    char file_username[250];
    char file_password[250];
};

void lspecial();
int read_file_contents(struct user_data *data);
int login(char *username, char *password);
int update_password(const char *filename, const char *username, const char *new_password);
int signup(char *username, char *password);
int login_successful(char name[]);
void signup_successful();
void update_successfully();


#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
void lspecial() {
    sleep(1.5);
    system("clear");
    printf(BLUE BOLD"************************************************************************************************************************************************************\n" RESET);
    printf(GOLD BOLD"\n\t\t\t\t\t\t\tRAILWAY RESERVATION SYSTEM\n\n" RESET);
    printf(BLUE BOLD"************************************************************************************************************************************************************\n" RESET);
}
#elif defined(_WIN32)
#include <windows.h>
void lspecial() {
    Sleep(1500); 
    system("cls");
    printf(BLUE BOLD"************************************************************************************************************************************************************\n" RESET);
    printf(GOLD BOLD"\n\t\t\t\t\t\t\t\tRAILWAY RESERVATION SYSTEM\n\n" RESET);
    printf(BLUE BOLD"************************************************************************************************************************************************************\n" RESET);
}
#else
void lspecial() {
    printf("Unknown OS\n");
}
#endif

int read_file_contents(struct user_data *data) {
    FILE *file = fopen("../../db/UserDB.csv", "r");
    if (file == NULL) {
        printf(DARK_RED "Error opening file.\n" RESET);
        return 0;
    }
    
    int i = 0;
    while (fscanf(file, "%249[^,],%249[^\n]%*c", data[i].file_username, data[i].file_password) == 2 && i < MAX_USERS) {
        i++;
    }

    fclose(file);
    return i; 
}

int login(char *username, char *password) {
    struct user_data data[MAX_USERS];
    int num_users = read_file_contents(data);
    if (num_users == 0) {
        printf(DARK_RED "No users found in the file.\n" RESET);
        return 0;
    }

    for (int i = 0; i < num_users; i++) {
        if (strcmp(username, data[i].file_username) == 0) {
            if (strcmp(password, data[i].file_password) == 0) {
                return 1;
            } else {
                return 2;
            }
        }
    }

    return 0; 
}

int update_password(const char *filename, const char *username, const char *new_password) {
    struct user_data data[MAX_USERS];
    int num_users = read_file_contents(data);
    if (num_users == 0) {
        printf(DARK_RED "No users found in the file.\n" RESET);
        return 0;
    }

    FILE *temp_file = fopen("../../db/temp.csv", "w");
    if (temp_file == NULL) {
        perror("Error opening temp file");
        return 0;
    }

    int username_exists = 0;

    for (int i = 0; i < num_users; i++) {
        if (strcmp(username, data[i].file_username) == 0) {
            username_exists = 1;
            strcpy(data[i].file_password, new_password); 
        }
        fprintf(temp_file, "%s,%s\n", data[i].file_username, data[i].file_password);
    }

    fclose(temp_file);

    if (!username_exists) {
        printf(DARK_RED "User '%s' not found in file.\n" RESET, username);
        remove("../../db/temp.csv"); 
        return 0;
    }

    if (remove(filename) != 0) {
        perror("Error deleting original file");
        return 0;
    }
    if (rename("../../db/temp.csv", filename) != 0) {
        perror("Error renaming temp file");
        return 0;
    }

    printf(GREEN "\n\nPassword updated successfully for user '%s'.\n" RESET, username);
    return 1;
}

int signup(char *username, char *password) {
    const char *filename = "../../db/UserDB.csv";
    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        printf(DARK_RED "Error opening file.\n" RESET);
        return 0;
    }

    struct user_data data[MAX_USERS];
    int num_users = read_file_contents(data);
    if (num_users == MAX_USERS) {
        printf(DARK_RED "Maximum number of users reached.\n" RESET);
        fclose(file);
        return 0;
    }

    for (int i = 0; i < num_users; i++) {
        if (strcmp(username, data[i].file_username) == 0) {
            printf(DARK_RED "Username already exists.\n" RESET);
            fclose(file);
            return 0;
        }
    }

    strcpy(data[num_users].file_username, username);
    strcpy(data[num_users].file_password, password);
    num_users++;

    fprintf(file, "%s,%s\n", username, password);
    printf(GREEN "\nSignup Successful.\n" RESET);

    fclose(file); 
    return 1;
}

int login_successful(char name[]) {
    char username[250],password[250]; 
    printf(WHITE BOLD"\n\n\t\t\t\t\t\t\t\t\tLOGIN\n\n" RESET);
    
    fflush(stdin);
    printf(INDIGO BOLD"Enter the username: ");
    fgets(username, sizeof(username), stdin);
    printf(RESET); 
    username[strcspn(username, "\n")] = '\0';

    fflush(stdin);
    printf(TURQUOISE BOLD"Enter the password: ");
    fgets(password, sizeof(password), stdin); 
    printf(RESET);
    password[strcspn(password, "\n")] = '\0';

    int result = login(username, password);

    if (result == 1) {
        strcpy(name, username);
        printf(GREEN "\nLogged in Successfully!\n" RESET);
        return 1;
    } else if (result == 0) {
        printf(RED "\nUsername not found! Try again or Sign up.\n" RESET);
    } else {
        printf(RED "\nPassword is incorrect. Try again or Sign up.\n" RESET);
    }
    return 0;
}

void signup_successful() {
    char username[250], password[250]; 
    printf(WHITE BOLD"\n\n\t\t\t\t\t\t\t\t\tSIGNUP\n\n" RESET);

    fflush(stdin);
    printf(INDIGO BOLD"Enter the username: ");
    fgets(username, sizeof(username), stdin); 
    printf(RESET);
    username[strcspn(username, "\n")] = '\0';

    do {
        fflush(stdin);
        printf(TURQUOISE BOLD"Enter the password: ");
        fgets(password, sizeof(password), stdin); 
        printf(RESET);
        password[strcspn(password, "\n")] = '\0';
    } while (!check_password(password));

    signup(username, password);
}

void update_successfully() {
    char username[250], password[250], new_password[250]; 
    printf(WHITE BOLD"\n\n\t\t\t\t\t\t\t\t\tUPDATE PASSWORD\n\n" RESET);

    fflush(stdin);
    printf(INDIGO BOLD"Enter the username: ");
    fgets(username, sizeof(username), stdin);
    printf(RESET); 
    username[strcspn(username, "\n")] = '\0';

    fflush(stdin);
    printf(TURQUOISE BOLD"Enter the password: ");
    fgets(password, sizeof(password), stdin);
    printf(RESET);
    password[strcspn(password, "\n")] = '\0';

    if (login(username, password) == 1) {
        do {
            printf(LAVENDER BOLD"Enter the new password: " );
            fgets(new_password, sizeof(new_password), stdin);
            printf(RESET);
            new_password[strcspn(new_password, "\n")] = '\0';
        } while (!check_password(new_password));

        if (update_password("../../db/UserDB.csv", username, new_password)) {
            printf(GREEN "Password updated successfully!\n" RESET);
        } else {
            printf(RED "Failed to update password.\n" RESET);
        }
    } else {
        printf(ORANGE "Failed to update password due to incorrect username or password.\n" RESET);
    }
}
