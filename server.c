#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define MAX_PATIENTS 100

 int number_of_patients = 0;
typedef struct Patients{
    char patient_fname[50];
    char patient_sname[50];
    char patient_gender[50];
    char DOI[100];
    char status[100];
} patient;

// Prototypes
void add_patient(void);
void check_status(void);
void add_patient(void);
void add_patient_list(void);
void search_details(void);

char User_name[50];
char district_name[30];


void bzero(void *a, size_t n)
{
    memset(a, 0, n);
}

void bcopy(const void *src, void *dest, size_t n)
{
    memmove(dest, src, n);
}

struct sockaddr_in *init_sockaddr_in(uint16_t port_number)
{
    struct sockaddr_in *socket_address = malloc(sizeof(struct sockaddr_in));
    memset(socket_address, 0, sizeof(*socket_address));
    socket_address->sin_family = AF_INET;
    socket_address->sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address->sin_port = htons(port_number);
    return socket_address;
}
FILE *sick_file;
char *process_operation(char *input){

    char *output = malloc(256);
   
    switch (input[0]){
    case  '1':
        add_patient();
        memcpy(output,"Command 1 executed", 30);
        return output;
    break;

    case '2':
         add_patient_list();
         memcpy(output,"Command 2 executed",30);
         return output;
         break;

    case '3':
        check_status();
        memcpy(output,"Command 3 executed",30);
        return output;
        break;

    case '4':
        search_details();
        memcpy(output,"Command 4 executed",30);
        return output;
        break;

    default:
        memcpy(output,"Command not found",30);
        return output;
        break;
    }

}

int main(int argc, char *argv[]){
    //  sick_file = fopen("patients.txt", "a+");

    const uint16_t port_number = 4000;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in *server_sockaddr = init_sockaddr_in(port_number);
    struct sockaddr_in *client_sockaddr = malloc(sizeof(struct sockaddr_in));
    socklen_t server_socklen = sizeof(*server_sockaddr);
    socklen_t client_socklen = sizeof(*client_sockaddr);

    if (bind(server_fd, (const struct sockaddr *)server_sockaddr, server_socklen) < 0)
    {
        printf("Error! Bind has failed\n");
        exit(0);
    }
    if (listen(server_fd, 3) < 0) {
        printf("Error! Can't listen\n");
        exit(0);
    }

    //variable buffer to store strings receiver over the network
    const size_t buffer_len = 256;
    //memory allocation.
    char *buffer = malloc(buffer_len * sizeof(char));
    //char *line= malloc(buffer_len * sizeof(char));
    char *response = NULL;
    time_t last_operation;
    __pid_t pid = -1;
    char name[256];

    //Run forever
    while (1)  {
         strcpy(name, buffer);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_sockaddr,&client_socklen);

        pid = fork();

        if (pid == 0)
        {
            close(server_fd);

            if (client_fd == -1){
                exit(0);
            }

            printf("\nConnection with `%d` has been established and delegated to the process %d.\nWaiting for a command...\n", client_fd, getpid());

            last_operation = clock();

            while (1){
                read(client_fd, buffer, 256);
                //printf("Name:%d", re);
                if (!strcmp(buffer, "close"))
                {
                    printf("Process %d: ", getpid());
                    close(client_fd);
                    printf("Closing session with `%d`. Bye!\n", client_fd);
                    break;
                }

                if (strlen(buffer) == 0)
                {
                    clock_t d = clock() - last_operation;
                    double dif = 5.0 * d / CLOCKS_PER_SEC;

                    if (dif > 1.0)
                    {
                        printf("Process %d: ", getpid());
                        close(client_fd);
                        printf("Connection timed out after %.3lf seconds. ", dif);
                        printf("Closing session with `%d`. Bye!\n", client_fd);
                        break;
                    }

                    // continue;
                }
                free(response);

                response = process_operation(buffer);
                // if(response == "A"){
                  send(client_fd,response, 256, 0);
               
                bzero(buffer, buffer_len * sizeof(char));
                last_operation = clock();
            }
            exit(0);
        }
        else
        {
            close(client_fd);
        }
    }
}


//----------------------------------------------------------
void check_status(){
    sick_file = fopen("sick_file.txt", "r");
    if (sick_file == NULL) {
        puts("\nFiles not there\n");
    }
    else{
        char store[200];
        int total_cases = 0;
        while (fgets(store, 100, sick_file) != NULL)  {
            total_cases++;
        }
        total_cases == 1 ? printf("\n %d cases stored in the file", total_cases): printf("\n %d cases stored in the file\n", total_cases);
    }
}
/*----------------------------------------------------------------------*/
void search_details(){
    //date/name
    sick_file = fopen("sick_file.txt", "r");
    char search_name[50];
    printf("\nSearch by name or date(dd/mm/yy):");
    scanf("%s", search_name);
   // printf("Record returned", search_name);
    char store[200];

    int records = 0;
    int total_records = 0;
    puts("\n****************************************");
    puts("\n");
    puts("Patient_Name\t\tDate\t\tGender\t\tOfficer_Name");
    puts("\n");

    while (fgets(store, 100,sick_file) != NULL){
        total_records++;
        if (strstr(store, search_name) != NULL){
            puts(store);
            records++;
        }
    }

    if (records == 0)
        printf("\nNo records found please re-search\n");
    else
    {
        int i = 0;
        for (i = 0; i <= total_records; i++){
            if (records == i){
                int get_current = records;
                get_current == 1 ? printf("\n%d record available out of %d\n", get_current, total_records)
                                   : printf("\n%d records available out of %d\n", get_current, total_records);
            }
        }
    }
    printf("\n");
}


void add_patient_list(void){
      patient patients[MAX_PATIENTS];
   
    if (number_of_patients == 0){
        printf("\n No patients yet !!\n");
       // get_command(display_commands());
    } else {
        sick_file = fopen("sick_file.txt", "a+");
        //loop
        if (sick_file == NULL)
        {
            printf("\nNot found\n");
        }else{
            //loop
            int i;
            for (i = 0; i < number_of_patients; i++) {
                fprintf(sick_file, "%s,\t %s,\t\t\t%s,\t%s,\t%s\n", patients[i].patient_fname,patients[i].patient_sname, patients[i].DOI, patients[i].patient_gender, User_name);
            }
            printf("\n------ All Patients Added ------\n");
            number_of_patients = 0;
            fclose(sick_file);
        }
    }
}
//----------------------------------------------------
 void add_patient(void){
   
    patient patients[MAX_PATIENTS];

    printf("USERNAME");
    scanf("%s", User_name);
    char stop,s1[20],s2[]="F",s3[]="M";
    char a1[20],a2[20],a3[20],a4[20];
    char d1[20],d2[20];
    char q1[20],q2[20],p1[]="not",p2[]="yes";
    int loop;
    puts("\t\n------- ENTER_PATIENT_INFORMATION ------\n");
    for (loop = number_of_patients; loop <= MAX_PATIENTS; loop++)
    {
        while(1){
         puts("Name of patient. (BOTH NAMES)");
         scanf("%s %s", a1,a2);
         puts("Please re-enter names in order for verification");
         scanf("%s %s",a3,a4);
         if(strcmp(a1,a3)!=0 && strcmp(a2,a4)!=0){
          puts("Error, repeat the names");
          continue;
         }
         else{
          strcpy(patients[loop].patient_fname,a1);
          strcpy(patients[loop].patient_sname,a2);
          break;
         }
         }
               
        while(1){
         puts("Gender_of_patient [F or M]");
         scanf("%s",s1);
         if(strcmp(s1,s2)!=0 && strcmp(s1,s3)!=0){
          printf("Re-input gender");
          continue;
         }
         else{
          strcpy(patients[loop].patient_gender,s1);
          break;
         }
        }
               
        while(1){
         puts("Input Date:format 01/25/2021");
         scanf("%s", d1);
         puts("Re-enter date for verification");
         scanf("%s",d2);
         if(strcmp(d1,d2)!=0){
          puts("Error,repeat the date");
          continue;
         }
         else if(ispunct(d1[2])==0 || ispunct(d1[5])==0){
          puts("Error,repeat the date");
          continue;
        }
         else if(strlen(a1)>10){
          puts("Error, repeat the date");
          continue;
         }
         else{
          strcpy(patients[loop].DOI,d1);
          break;
         }
	}
         
        while(1){
         puts("Asymptomatic/NotAsymptomatic. Please enter not for NotAsypmtomatic or yes for Asymptomatic");
         scanf("%s",q1);
         puts("Please re-enter condition");
         scanf("%s",q2);
         if(strcmp(q1,p1)!=0 && strcmp(q1,p2)!=0){
		        puts("Error,repeat");
		        continue;
         }
         else{
          strcpy(patients[loop].status,q1);
	  break; 
         }
        }
                 number_of_patients += 1;
         printf("\n");
         puts("More patients y/n?");
        scanf(" %c", &stop);
        if (stop == 'n' || stop == 'N'){
            break;
        }
        puts("\n -----------NEXT_case-----------\n");
    }
}
