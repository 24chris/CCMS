#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#define MAX_PATIENTS 100

FILE *sick_file;



//socket globals
int sockfd, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];
socklen_t portno = 4000;
char Name_of_district[30];




void display_commands(){

    char choice[5][30] = {"1:ADD_YOUR_PATIENT", "2:ADD_MULTIPLE_CASES","3:CHECK_CURRENT_FILE_STATUS", "4:SEARCH_CRITERIA"};
    int next_choice;
    puts("-------Lists of Choices-----\t");
    for (next_choice = 0; next_choice < 5; next_choice++)
    {
        puts(choice[next_choice]);
    }
};
char *get_command(int selected_choice)
// call functions
{
    if (selected_choice == 1){
        return "1";
    }
    else if (selected_choice == 2){
        return "2";
    }
    else if (selected_choice == 3){
        printf("\n");
        return "3";
    }
    else if (selected_choice == 4){
        return "4";
    } else{
      return "5";
    }
}
int main(int argc, char *argv[]){
    int Choose;
    if (argc != 2){
        printf("\nUsage:%s <ip server>\n",argv[2]);
    }

      sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname("127.0.0.1");

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(4000);
    //connect to server with server address which is set above (serv_addr)

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR while connecting");
        exit(1);
    }

    puts("-------------------------Welcome to the covid-19 case management and reporting system-------------------");
    puts("\n");


    printf("Enter name of district for example Kampala: ");

    scanf("%s", Name_of_district);

      while (1){
          display_commands();

         printf("\nSelectCommand:");
        scanf("%d", &Choose);
       snprintf(buffer,sizeof(buffer),"%s",get_command(Choose));

        n = write(sockfd, buffer, 256);

        if (n < 0){
            perror("ERROR while writing to socket");
            exit(1);
        }

        bzero(buffer, 256);
        int rnd = read(sockfd, buffer, 256);

        if (rnd < 0) {
            perror("ERROR while reading from socket");
            exit(1);
        }
        printf("\n\nServer replied: %s \n\n", buffer);


    }
}

