#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void skip_bom(FILE *fp) {
    fseek(fp, 0, SEEK_SET);
    if (fgetc(fp) == 0xEF && fgetc(fp) == 0xBB && fgetc(fp) == 0xBF) {
    } else {
        fseek(fp, 0, SEEK_SET);
    }
}

void convert_waiting_list(const char *input_file, const char *output_file) {
    FILE *fp = fopen(input_file, "r");
    FILE *tempx = fopen(output_file, "w");
    
    if (fp == NULL || tempx == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    skip_bom(fp);
    
    char line[256];
    fgets(line, sizeof(line), fp);
    while (fgets(line, sizeof(line), fp)) {

        char *token;
        char *tokens[8]; 
        
        int token_count = 0;
        token = strtok(line, ",");
        
        while (token != NULL && token_count < 8) {
            tokens[token_count++] = token;
            token = strtok(NULL, ",");
        }
        
        if (token_count == 8) {
            char *pnr = tokens[0];
            int date = atoi(tokens[1]);
            char *train_name = tokens[2];
            char *train_number = tokens[3];
            char *arrival = tokens[4];
            char *destination = tokens[5];
            int wl_position ;
            sscanf(tokens[6],"WL%d",&wl_position);
            char *coach = tokens[7];
        
                fprintf(tempx, "%s,%s,%s,%s,%s,%d,%d,%s", 
                        pnr,train_name, train_number, arrival, destination, date, wl_position, coach);
            
        }
    }
    
    fclose(fp);
    fclose(tempx);
}

void convertion() {
    const char *input_file = "../../db/WaitingListDB.csv";
    const char *output_file = "../../db/WaitingPriorityDB.csv";

    convert_waiting_list(input_file, output_file);
}

void convert_back_to_waiting_list(const char *input_file, const char *output_file) {
    FILE *fp = fopen(input_file, "r");
    FILE *tempx = fopen(output_file, "w");
    
    if (fp == NULL) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }
    
    if (tempx == NULL) {
        perror("Failed to open output file");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    skip_bom(fp);
    
    char line[1000];

    // Write the new header
    fprintf(tempx, "PNR,Date,Train Name,Train No,Arrival,Destination,WL Position,Coach\n");
    
    while (fgets(line, sizeof(line), fp)) {
        char *token;
        char *tokens[8];
        int token_count = 0;
        
        token = strtok(line, ",");
        while (token != NULL && token_count < 8) {
            tokens[token_count++] = token;
            token = strtok(NULL, ",");
        }
        if (token_count == 8) {
            char *pnr = tokens[0];
            char *train_name = tokens[1];
            char *train_number = tokens[2];
            char *arrival = tokens[3];
            char *destination = tokens[4];
            int date = atoi(tokens[5]);
            int wl_position = atoi(tokens[6]);
            char *coach = tokens[7];
            fprintf(tempx, "%s,%d,%s,%s,%s,%s,WL%d,%s", 
                    pnr, date, train_name, train_number, arrival, destination, wl_position, coach);
        } else {
            fprintf(stderr, "Incorrect format in line: %s", line);
        }
    }
    
    fclose(fp);
    fclose(tempx);
}

void convertion_back() {
    const char *input_file = "../../db/WaitingPriorityDB.csv";
    const char *output_file = "../../db/WaitingListDB.csv";

    convert_back_to_waiting_list(input_file, output_file);
}
