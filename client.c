#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int clientSocket, ret;
struct sockaddr_in serverAddr;
char buffer[1024];

void connection()
{

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
	{
		printf("oops there was an error .\n");
		exit(1);
	}
	printf("client socket created successfully.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("Error in connection please try again.\n");
		exit(1);
	}
	printf("serverConnected.\n");
}
void usage_commands(){

	printf("\n\tHOW TO USE THE SYSTEM\n");
	printf("\nCommands are specified as below for the betterment of the system use the commands as instructed\n");
	printf("\nAddPatient command> usage Addpatient patient_name tabkey date(y-m-d) gender\n");
	printf("\nAddpatient filename.txt>usage Addpatient specific the file name(only text based files)\n");
	printf("\nCheck_status >usage Check_status\n");
	printf("\nSearch command>usage Search enter what to search enter by date(y-m-d) or name\n");
	printf("\n");
   
	
}
int check_command(char *command){
	 int com1 = strncmp(command, "Addpatient", 10);
	 int com2 = strncmp(command, "Check_status", 12);
	 int com3 = strncmp(command, "Search",6);
	 int com4 = strncmp(command, "Addpatientlist", 14);
	

	 if(com1==0|| com2==0 || com3==0 || com4==0){
		 return 1;
	 }
	else{
		return 0;
	}
}

void main()
{   
	printf("\n\tOUR COVID 19  SOCKET SYSTEM\n");
	printf("\n");
		char District[50];
	printf("\nEnterDistrict:");
	fgets(District, 50, stdin);
	char officer_user_name[50];
	printf("\nEnter the officer_user_name:");
	//scanf("%s", officer_user_name);
	fgets(officer_user_name, 50, stdin);
	usage_commands();
	while (1)
	{	
		char command[50];
		char name[30];
		char date[30];

	back_to_top:
	    printf("\nOfficerUserName:%-10s \nDistrict\t:%-20s\n", officer_user_name, District);
		printf("\n----------------------------------------\n");
		bzero(command, sizeof(command));
		bzero(name, sizeof(name));
		bzero(date, sizeof(date));
		printf("ExecuteCommand:");
		fgets(buffer, 1024, stdin);
		if(!check_command(buffer)){
			printf("\n\nWrong command entered please try again\n\n");
 			goto back_to_top;

		}

		buffer[strlen(buffer) - 1] = '\0';

		sscanf(buffer, "%s %s %s", command, name, date);
		//Addpatient katende nicolas 2021-02 M
		//Addpatient bruno.txt
		printf("%s, %s, %s \n", command, name, date);
	
		//strncmp() checker

		if ((strlen(date) == 0) && (!strncmp(buffer ,"Addpatient",10)))
		{    
			char *txtExt, *check_txt;
			//chat tx[];
		//	Addpatient bruno txt//.

			txtExt = strtok(buffer, ".");
			while (txtExt != NULL)
			{
				check_txt = txtExt;
				txtExt = strtok(NULL, ".");
			}

			if (strcmp(check_txt, "txt") == 0)
			{
				FILE *filePtr = fopen(name, "r+");
				if (filePtr == NULL)
				{
					printf("file read error!\n");
				}
				else
				{
			
					while (fgets(buffer, 100, filePtr) != NULL)
					{
						buffer[strlen(buffer)-1]='\0';
						//puts(store);
						connection();
						send(clientSocket, buffer, strlen(buffer), 0);
						bzero(buffer, sizeof(buffer));
					}
				}

				goto back_to_top;
			}
			else
			{
				printf("Wrong file format\n");
				goto back_to_top;
			}
		}
		else if((strlen(date) !=0) && (!strncmp(buffer, "Addpatient", 10))){
	
		
		//concat
		strcat(buffer , " ");
		strcat(buffer, officer_user_name);
		buffer[strlen(buffer)-1]='\0';
		connection();
		send(clientSocket, buffer, strlen(buffer), 0);
		


		}
		else{
	
		connection();
		send(clientSocket, buffer, strlen(buffer), 0);

		}



		if (strcmp(buffer, ":exit") == 0)
		{
			close(clientSocket);
			printf("Server has been disconnected.\n");
			exit(1);
		}

		if (recv(clientSocket, buffer, 1024, 0) < 0)
		{
			printf("Error in receiving info.\n");
		}
		else
		{ 	
			printf("\n\nserver:%s\n\n", buffer);
		}
		goto back_to_top;
		//close(clientSocket);
	}

	return;
}
