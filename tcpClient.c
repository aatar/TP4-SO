#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcpClient.h"



int clientSocket = 0, ret = 0, option = 0, validateOperation = 0;
struct sockaddr_in serverAddr;
char buffer[1024];
char message[1024];
char c;

int main(){

	memset(&serverAddr, '\0', sizeof(serverAddr));
	bzero(buffer, sizeof(buffer));

	setupClientSocket();

	


	printf("WELCOME TO THE AIRPLANE SEAT BOOKING SYSTEM!!!\n");

  
   	while(1) {
      	option = 0;
      	while(!(option >=1 && option <= 6)) {

	         option = 0;

	         printf("You can choose one of these options: \n");
	         printf("1- Create a new flight \n");
	         printf("2- Cancel an existing flight \n");
	         printf("3- See the seating arrangement of an existing flight \n");
	         printf("4- Book a seat of an existing flight \n");
	         printf("5- Cancel a booking of a seat \n");
	         printf("6- Exit \n");
	         printf("\n");

	      
	         while(!(option >=1 && option <= 6)) {
               option = getint("Choose your option: ");
               if(!(option >=1 && option <= 6))
                  printf("Incorrect number, it must be an integer between 1 and 6\n");
            }
            //while (getchar() != '\n');
            //while ((c = getchar()) != '\n' && c != EOF) { }

      	}
      
      	switch(option) {
	         case 1:
	            createFlight();
	         break;
	         case 2:
	            cancelFlight();
	         break;
	         case 3:
	            seeFlight();
	         break;
	         case 4:
	            bookSeat();
	         break;
	         case 5:
	            cancelSeatBooking();
	         break;
	         break;
	         case 6:
	            //sqlite3_close(db);
	         	bzero(buffer, sizeof(buffer));
               sprintf(buffer, "exit");
	         	send(clientSocket, buffer, strlen(buffer), 0);
	         	close(clientSocket);
   				printf("[-]Disconnected from server.\n");
   				exit(1);
	         break;
      	}
   	}

	/*while(1){
		
		printf("Client: \t");
		fgets(buffer, 1024, stdin);
		if ((strlen(buffer) > 0) && (buffer[strlen (buffer) - 1] == '\n'))
        	buffer[strlen (buffer) - 1] = '\0';
        
        send(clientSocket, buffer, strlen(buffer), 0);

		if(strcmp(buffer, ":exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

		if(recv(clientSocket, buffer, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			printf("Server: \t%s\n", buffer);
		}
	}*/

	return 0;
}

void setupClientSocket() {
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");
}

void createFlight() {
   validateOperation = 1;

   do {
      validateOperation = 1;

      printf("PLEASE WRITE THE NAME OF THE NEW FLIGHT, OR 'BACK' IF YOU WANT TO GO BACK:\n");
      memset(buffer, '\0', sizeof(buffer));
      memset(message, '\0', sizeof(message));
      fgets(buffer, 1024, stdin);
      if ((strlen(buffer) > 0) && (buffer[strlen (buffer) - 1] == '\n'))
         buffer[strlen (buffer) - 1] = '\0';
      //scanf("%s", buffer);
      //while (getchar() != '\n');

      toUpperCase(buffer);

      if(strcmp(buffer, "BACK") == 0) return;

      
      strcpy(message, "1");
      strcat(message, buffer);
      send(clientSocket, message, strlen(message), 0);

      memset(message, '\0', sizeof(message));
      if(recv(clientSocket, message, 1024, 0) < 0){
         printf("[-]Error in receiving data.\n");
      }else{
         if(strcmp(message, "ok") == 0) {
            validateOperation = 1;
            printf("FLIGHT '%s' CREATED SUCCESSFULLY!!!\n", buffer);
         }
         else {
            validateOperation = 0;
            printf("SORRY, THE FLIGHT YOU WROTE ALREADY EXISTS, PLEASE TRY WITH ANOTHER ONE\n");
         }
      }
      
   } while(!validateOperation);
}

void cancelFlight() {
   printf("OK, LET'S SEE...\n");
   showExistingFlights();

   validateOperation = 0;

   while(!validateOperation) {
      printf("PLEASE WRITE THE NAME OF THE FLIGHT TO CANCEL, OR 'BACK' IF YOU WANT TO GO BACK: \n");
      memset(buffer, '\0', sizeof(buffer));
      memset(message, '\0', sizeof(message));
      fgets(buffer, 1024, stdin);
      if ((strlen(buffer) > 0) && (buffer[strlen (buffer) - 1] == '\n'))
         buffer[strlen (buffer) - 1] = '\0';

      //scanf("%s", buffer);
      //while (getchar() != '\n');
      toUpperCase(buffer);

      if(strcmp(buffer, "BACK") == 0) return;

      strcpy(message, "2");
      strcat(message, buffer);
      send(clientSocket, message, strlen(message), 0);

      memset(message, '\0', sizeof(message));
      if(recv(clientSocket, message, 1024, 0) < 0){
         printf("[-]Error in receiving data.\n");
      }else{
         if(strcmp(message, "ok") == 0) {
            validateOperation = 1;
            printf("FLIGHT '%s' CANCELLED SUCCESSFULLY!!!\n", buffer);
         }
         else {
            validateOperation = 0;
            printf("SORRY, THE FLIGHT YOU WANT TO DELETE DOES NOT EXIST, PLEASE TRY WITH ANOTHER ONE\n");
         }
      }
   }

}

void seeFlight() {
   showExistingFlights();

   printf("PLEASE WRITE THE NAME OF THE FLIGHT TO SEE THE SEATING ARRANGEMENT, OR 'BACK' IF YOU WANT TO GO BACK: \n");
   memset(buffer, '\0', sizeof(buffer));
   fgets(buffer, 1024, stdin);
      if ((strlen(buffer) > 0) && (buffer[strlen (buffer) - 1] == '\n'))
         buffer[strlen (buffer) - 1] = '\0';
   //scanf("%s", buffer);
   //while (getchar() != '\n');

   toUpperCase(buffer);

   if(strcmp(buffer, "BACK") == 0) return;

   showSeatsArrangement(buffer);

}

void bookSeat() {
   char flight[1024];
   memset(flight, '\0', sizeof(buffer));
   int seatNumber = 0, res = 0;

   showExistingFlights();

   printf("PLEASE WRITE THE NAME OF THE FLIGHT TO SEE, OR 'BACK' IF YOU WANT TO GO BACK: \n");
   memset(buffer, '\0', sizeof(buffer));
   fgets(buffer, 1024, stdin);
      if ((strlen(buffer) > 0) && (buffer[strlen (buffer) - 1] == '\n'))
         buffer[strlen (buffer) - 1] = '\0';
   //scanf("%s", buffer);
   //while (getchar() != '\n');

   toUpperCase(buffer);

   if(strcmp(buffer, "BACK") == 0) return;

   strcpy(flight, buffer);

   showSeatsArrangement(flight);

   while(!res) {
      seatNumber = 0;
      seatNumber = getint("PLEASE ENTER THE NUMBER OF SEAT TO BOOK: \n");
      /*scanf("%d", &seatNumber);
      while (getchar() != '\n');*/

      res = book(flight, seatNumber);
   }

   showSeatsArrangement(flight);

}

void cancelSeatBooking() {
   char flight[1024];
   memset(flight, '\0', sizeof(buffer));
   int seatNumber = 0, res = 0;

   showExistingFlights();

   printf("PLEASE WRITE THE NAME OF THE FLIGHT TO SEE, OR 'BACK' IF YOU WANT TO GO BACK: \n");
   memset(buffer, '\0', sizeof(buffer));
   fgets(buffer, 1024, stdin);
      if ((strlen(buffer) > 0) && (buffer[strlen (buffer) - 1] == '\n'))
         buffer[strlen (buffer) - 1] = '\0';
   //scanf("%s", buffer);
   //while (getchar() != '\n');

   toUpperCase(buffer);

   if(strcmp(buffer, "BACK") == 0) return;

   strcpy(flight, buffer);

   showSeatsArrangement(flight);

   while(!res) {
      seatNumber = 0;
      seatNumber = getint("PLEASE ENTER THE NUMBER OF SEAT TO CANCEL THE BOOKING: \n");

      res = cancelBooking(flight, seatNumber);
   }

   showSeatsArrangement(flight);
}

void showExistingFlights() {
   memset(message, '\0', sizeof(message));
   sprintf(message, "6");
   send(clientSocket, message, strlen(message), 0);
   if(recv(clientSocket, message, 1024, 0) < 0){
      printf("[-]Error in receiving data.\n");
   }else{
      printf("%s\n", message);
   }

}

void showSeatsArrangement(char * flight) {
   bzero(message, sizeof(message));
   sprintf(message, "3%s", flight);
   send(clientSocket, message, strlen(message), 0);
   if(recv(clientSocket, message, 1024, 0) < 0){
      printf("[-]Error in receiving data.\n");
   }else{
      printf("%s\n", message);
   }
}

int book(char * flight, int seatNumber) {
   return changeSeatsArrangement(flight, seatNumber, 1);
}

int cancelBooking(char * flight, int seatNumber) {
   return changeSeatsArrangement(flight, seatNumber, 0);
}

int changeSeatsArrangement(char * flight, int seatNumber, int hasToBook) {
   bzero(message, sizeof(message));
   if(hasToBook) sprintf(message, "4%d%s", seatNumber, flight);
   else sprintf(message, "5%d%s", seatNumber, flight);
   
   send(clientSocket, message, strlen(message), 0);
   if(recv(clientSocket, message, 1024, 0) < 0){
      printf("[-]Error in receiving data.\n");
   }else{
      if(startsWith(message, "ok")){
         if(hasToBook) printf("SEAT%d OF FLIGHT '%s' BOOKED SUCCESSFULLY\n", seatNumber, flight);
         else printf("BOOKING OF SEAT%d, FLIGHT '%s', HAS BEEN CANCELLED SUCCESSFULLY\n", seatNumber, flight);

         return 1;
      }
      else {
         printf("%s\n", message);
         return 0;
      }
   }
   return 0;

}

int startsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

static int
finalBuffer()
{
   int c;
   int flag = 1;
   while ( ( c= getchar()) != '\n')
      if (! isspace(c))
         flag = 0;
   
   return flag;
}

int 
getint(const char message[], ... )
{
   int n, exitFlag = 0;
   va_list ap;
      
   do 
   {
      va_start(ap, message);
      vprintf(message, ap);
      va_end(ap);

      if ( scanf("%d",&n) != 1)
         while (getchar() != '\n');
      else
         exitFlag = finalBuffer();
      if ( !exitFlag)
         printf("Error, that's not a number\n");

   } while (! exitFlag);
   return n;
}

void toUpperCase(char* s)
{
    while(*s) {
        *s = toupper(*s);
        s ++;
    }
}

