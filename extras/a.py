import pandas as pd
import numpy as np

# Reading the CSV file with the specified columns
df = pd.read_csv(r"D:\Project\Reservation page\night_available.csv", usecols=["Coach Type", "Coach No", "Seat No"])
print(df)

# Adding new columns with default 
df['Madurai-in']=0
df['Dindigul-out']=0
df['Dindigul-in']=0
df['Villupuram-out']=0
df['Villupuram-in']=0
df['Chennai-out']=0
# Display the updated DataFrame
print(df)

# Saving the updated DataFrame to a new CSV file
df.to_csv(r"D:\Project\Reservation page\Vaigai Express-12635.csv", index=False)

'''
Vaigai Express,12635,nt,4
Madurai,22:30,22:32,0
Dindigul,00:07,00:10,62
Villupuram,04:15,04:20,272
Chennai,06:25,END,159
'''
'''printf("\n\n\t\t\t\t\t\t\t\tTICKET");
                printf("\n***************************************************************************");
                printf("\nPassenger:%s\t\tPNR:%s\t\tDATE OF JOURNEY%d",ticket.username,ticket.pnr,ticket.date);
                printf("\n\t\t\t\t\t\t\t\tTRAIN NAME:%s",ticket.train_name);
                printf("\n\t\t\t\t\t\t\t\tTRAIN NO.%s",ticket.train_no);
                printf("\nARRIVAL STATION & TIME:%s-%s\nDEPARTURE STATION & TIME:%s-%s",ticket.arrival,ticket.arrt,ticket.destination,ticket.destt);
                printf("\n\t\t\t\t\t\t\t\tCOACH TYPE:%s",ticket.coach);
                printf("\n***************************************************************************");
                for(int i=0;i<ticket.seats[7];i++)
                {
                    printf("\n%d\t\t\t\t\t\t\t%s",ticket.seats[i],ticket.status[i]);
                }
                printf("\n***************************************************************************");
                printf("\nTICKET PRICE:\t\t\t\t\\t\t\t\t\t%.2f",ticket.price);
                printf("\n\nHAVE A GREAT JOURNEY AHEAD!!VISIT US AGAIN!!!\n");
                printf("\n***************************************************************************");'''


void (char *pnr, int dd, char *train_name, char *train_id, char *arrival, char *destination, int wait_count, char wait_status[][10], char *coach){
    FILE *file1 = fopen("waiting_list.csv", "r");
    FILE *file2 = fopen("temp.csv", "w+");
    if (!file1 || !file2) {
        perror("Error opening files");
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        return;
    }

    struct waiting_list reader;
    char wl_read[MAX_LINE_LENGTH];

    // Write the header to the temp file
    if (fgets(wl_read, sizeof(wl_read), file1) != NULL) {
        fputs(wl_read, file2);
    } else {
        fclose(file1);
        fclose(file2);
        perror("Error reading header from waiting_list.csv");
        return;
    }

    int k = 0;
    while (fgets(wl_read, sizeof(wl_read), file1) != NULL) {
        sscanf(wl_read, "%[^,],%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
               reader.pnr, &reader.date, reader.train_name, reader.train_id,
               reader.arrival, reader.destination, reader.wl_pos, reader.coach);

        if (!((dd == reader.date) &&
              (strncasecmp(reader.train_name, train_name, strlen(train_name)) == 0) &&
              (strncasecmp(reader.train_id, train_id, strlen(train_id)) == 0) &&
              (strncasecmp(reader.arrival, arrival, strlen(arrival)) == 0) &&
              (strncasecmp(reader.destination, destination, strlen(destination)) == 0) &&
              (k < wait_count) &&
              (strncasecmp(reader.wl_pos, wait_status[k], strlen(wait_status[k])) == 0) &&
              (strncasecmp(reader.coach, coach, strlen(coach)) == 0))) {
            fprintf(file2, "%s,%d,%s,%s,%s,%s,%s,%s\n",
                    reader.pnr, reader.date, reader.train_name, reader.train_id,
                    reader.arrival, reader.destination, reader.wl_pos, reader.coach);
        } else {
            k++;
        }
    }

    fclose(file1);
    fclose(file2);

    remove("waiting_list.csv");
    rename("temp.csv", "waiting_list.csv");

}