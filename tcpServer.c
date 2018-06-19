#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sqlite3.h> 
#include "tcpServer.h"


sqlite3 *db;
char *zErrMsg = 0;
int rc;
char sql[1024];
char buffer[1024];
char ans[1024];
int validateOperation = 0;
int serverSocket = 0, ret = 0, currentConnections = 0;
struct sockaddr_in serverAddr;

int newSocket;
struct sockaddr_in newAddr;

socklen_t addr_size;

pid_t childpids[MAX_CONNECTIONS];
int status[MAX_CONNECTIONS];

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int callbackFlights(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      strcat(buffer, argv[i] ? argv[i] : "NULL");
      strcat(buffer, "\n");
   }
   return 0;
}

static int callbackSeats(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      strcat(buffer, azColName[i]);
      strcat(buffer, " : ");
      strcat(buffer, argv[i] ? argv[i] : "NULL");
      strcat(buffer, "\n");
   }
   return 0;
}

static int checkIfFree(void *NotUsed, int argc, char **argv, char **azColName) {
   if(strcmp(argv[0],"Free") == 0) validateOperation = 1;
   printf("\n");
   return 0;
}

static int checkIfOccupied(void *NotUsed, int argc, char **argv, char **azColName) {
   if(strcmp(argv[0],"Free") != 0) validateOperation = 1;
   printf("\n");
   return 0;
}

static int checkIfNotExists(void *NotUsed, int argc, char **argv, char **azColName) {
   validateOperation = 0;
   return 0;
}
static int checkIfExists(void *NotUsed, int argc, char **argv, char **azColName) {
   validateOperation = 1;
   return 0;
}

int main(){

	memset(&serverAddr, '\0', sizeof(serverAddr));
	bzero(buffer, sizeof(buffer));

	setupServerSocket();

	if(!openDatabase()) return 0;
	
	
	
	while(1){
		newSocket = accept(serverSocket, (struct sockaddr*)&newAddr, &addr_size);
		
		if(newSocket < 0){
			exit(1);
		}
		
		else if(currentConnections < MAX_CONNECTIONS) {
			currentConnections++;

			
			printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

			if((childpids[currentConnections-1] = fork()) == 0){
				close(serverSocket);

				while(1){
					recv(newSocket, buffer, 1024, 0);
					if(strcmp(buffer, "exit") == 0){
						printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
						close(newSocket);
						exit(0);
					}
					else if(startsWith(buffer, "1")) {
						removeFirst(buffer);
						

						if(createFlight(buffer)) {
							printf("Client %d created flight '%s'!!!\n", currentConnections, buffer);
							//printf("A client created a flight\n");
							memset(ans, '\0', sizeof(ans));
							sprintf(ans, "ok");
							
						}
						else {
							memset(ans, '\0', sizeof(ans));
							sprintf(ans, "error");
						}
						send(newSocket, ans, strlen(ans), 0);
						memset(ans, '\0', sizeof(ans));
							
					}
					else if(startsWith(buffer, "2")) {
						removeFirst(buffer);
						

						if(cancelFlight(buffer)) {
							printf("Client %d cancelled flight '%s'!!!\n", currentConnections, buffer);
							memset(ans, '\0', sizeof(ans));
							sprintf(ans, "ok");
						}
						else {
							memset(ans, '\0', sizeof(ans));
							sprintf(ans, "error");
						}
						send(newSocket, ans, strlen(ans), 0);
						memset(ans, '\0', sizeof(ans));
							
					}
					else if(startsWith(buffer, "3")) {
						removeFirst(buffer);
						char flight[1024];
						memset(flight, '\0', sizeof(flight));
						strcpy(flight, buffer);
						memset(buffer, '\0', sizeof(buffer));
						sprintf(buffer, "SEATS ARRANGEMENT FOR THE FLIGHT '%s':\n", flight);
						memset(sql, '\0', sizeof(sql));
					   	sprintf(sql, "SELECT SEAT1, SEAT2, SEAT3, SEAT4, SEAT5 FROM FLIGHT WHERE NAME='%s';", flight);

					   	/* Execute SQL statement */
					   	rc = sqlite3_exec(db, sql, callbackSeats, 0, &zErrMsg);
					   
					   	if( rc != SQLITE_OK ){
					   	   fprintf(stderr, "SQL error: %s\n", zErrMsg);
					   	   sqlite3_free(zErrMsg);
					   	}
					   	else {
					    	send(newSocket, buffer, strlen(buffer), 0);
							memset(buffer, '\0', sizeof(buffer));
							printf("Client %d saw the seats arrangements for the flight '%s'!!!:\n", currentConnections, flight);
					    }
						
							
					}
					else if(startsWith(buffer, "4") || startsWith(buffer, "5")) {
						int hasToBook = startsWith(buffer, "4");

						removeFirst(buffer);
						char aux[1];
						aux[0] = buffer[0];
						int seatNumber = atoi(aux);
						removeFirst(buffer);
						char flight[1024];
						memset(flight, '\0', sizeof(flight));
						strcpy(flight, buffer);
						memset(buffer, '\0', sizeof(buffer));

						memset(ans, '\0', sizeof(ans));

						if(seatNumber < 1 || seatNumber > 5) {
							sprintf(ans, "INVALID SEAT NUMBER, IT HAS TO BE AN INTEGER NUMBER BETWEEN 1 AND 5\n");
						}
						else {
							memset(sql, '\0', sizeof(sql));
						   	sprintf(sql, "SELECT SEAT%d FROM FLIGHT WHERE NAME='%s';", seatNumber, flight);

						   	validateOperation = 0;
						   	/* Execute SQL statement */
						   	if(hasToBook) rc = sqlite3_exec(db, sql, checkIfFree, 0, &zErrMsg);
						   	else rc = sqlite3_exec(db, sql, checkIfOccupied, 0, &zErrMsg);

						   	if( rc != SQLITE_OK ){
						      	fprintf(stderr, "SQL error: %s\n", zErrMsg);
						      	sqlite3_free(zErrMsg);
						      	sprintf(ans, "ERROR WITH THE DATABASE\n");
						   	}
						   	else {
						   		if(validateOperation) {
								      memset(sql, '\0', sizeof(sql));
								   
								      if(hasToBook) sprintf(sql, "UPDATE FLIGHT SET SEAT%d='Occupied' WHERE NAME='%s';", seatNumber, flight);
								      else sprintf(sql, "UPDATE FLIGHT SET SEAT%d='Free' WHERE NAME='%s';", seatNumber, flight);
								      
								      /* Execute SQL statement */
								      rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
								      
								      if( rc != SQLITE_OK ){
								         fprintf(stderr, "SQL error: %s\n", zErrMsg);
								         sqlite3_free(zErrMsg);
								         sprintf(ans, "ERROR WITH THE DATABASE\n");
								      } else {
								         sprintf(ans, "ok");
								         if(hasToBook) printf("Client %d booked seat %d of flight '%s'\n", currentConnections, seatNumber, flight);
								         else printf("Client %d cancelled the booking of seat %d, flight '%s'\n", currentConnections, seatNumber, flight);
								      }
								}
							   	else {
								      if(hasToBook) sprintf(ans, "THE SEAT YOU WANT TO BOOK IS ALREADY OCCUPIED, PLEASE TRY WITH ANOTHER ONE\n");
								      else sprintf(ans, "YOU CANNOT CANCEL THAT BOOKING BECAUSE THE SEAT IS NOW FREE, PLEASE TRY WITH ANOTHER ONE\n");
								}
						   	}
						}
						
						send(newSocket, ans, strlen(ans), 0);
						memset(ans, '\0', sizeof(ans));
						
							
					}
					
					else if(startsWith(buffer, "6")) {
						memset(buffer, '\0', sizeof(buffer));
						strcpy(buffer, "THESE ARE THE EXISTING FLIGHTS:\n");
						memset(sql, '\0', sizeof(sql));
					    sprintf(sql, "SELECT NAME FROM FLIGHT;");

					    /* Execute SQL statement */
					    rc = sqlite3_exec(db, sql, callbackFlights, 0, &zErrMsg);
					   
					    if( rc != SQLITE_OK ){
					       fprintf(stderr, "SQL error: %s\n", zErrMsg);
					       sqlite3_free(zErrMsg);
					    }
					    else {
					    	send(newSocket, buffer, strlen(buffer), 0);
							memset(buffer, '\0', sizeof(buffer));
					    }
					}
					
					/*else{
						printf("Client %d: %s\n", currentConnections, buffer);
						send(newSocket, buffer, strlen(buffer), 0);
						bzero(buffer, sizeof(buffer));
					}*/
				}
			}
		}
		else {
			printf("Cannot connect, clients have exceeded the limit of %d\n", MAX_CONNECTIONS);
			break;
		}

	}

	for(int j = 0; j < MAX_CONNECTIONS; j++)
	{
		waitpid(childpids[j], &(status[j]), 0);
	}

	close(newSocket);
	close(serverSocket);
	sqlite3_close(db);

	return 0;
}

void setupServerSocket() {
	
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", PORT);

	if(listen(serverSocket, MAX_CONNECTIONS) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}
}

int openDatabase() {
	/* Open database */
   rc = sqlite3_open("flights.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return 0;
   }
   else
   	return 1;
}

int startsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void removeFirst(char * s) {
	int i,len=strlen(s);
	for(i = 1;i < len;i++)
	{
		s[i-1] = s[i];
	}
	s[i-1] = '\0';
}

int createFlight(char * name) {
	validateOperation = 1;
    memset(sql, '\0', sizeof(sql));
    sprintf(sql, "SELECT * FROM FLIGHT WHERE NAME='%s';", name);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, checkIfNotExists, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }

    if(validateOperation) {

	     memset(sql, '\0', sizeof(sql));
	     sprintf(sql, "INSERT INTO FLIGHT VALUES ('%s', 'Free', 'Free', 'Free', 'Free', 'Free');", name);
	     
	     /* Execute SQL statement */
	     rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	     
	     if( rc != SQLITE_OK ){
	        fprintf(stderr, "SQL error: %s\n", zErrMsg);
	        sqlite3_free(zErrMsg);
	     } else {
	        return 1;
	        //printf("FLIGHT '%s' CREATED SUCCESSFULLY!!!\n", buffer);
	     }
	}
	else {
	     return 0;
	     //printf("SORRY, THE FLIGHT YOU WROTE ALREADY EXISTS, PLEASE TRY WITH ANOTHER ONE\n");
	}
	return 0;
}

int cancelFlight(char * name) {
	validateOperation = 0;
    memset(sql, '\0', sizeof(sql));
    sprintf(sql, "SELECT * FROM FLIGHT WHERE NAME='%s';", name);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, checkIfExists, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }

    if(validateOperation) {

	     memset(sql, '\0', sizeof(sql));
	     sprintf(sql, "DELETE FROM FLIGHT WHERE NAME='%s';", name);
	     
	     /* Execute SQL statement */
	     rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	     
	     if( rc != SQLITE_OK ){
	        fprintf(stderr, "SQL error: %s\n", zErrMsg);
	        sqlite3_free(zErrMsg);
	     } else {
	        return 1;
	        //printf("FLIGHT '%s' CREATED SUCCESSFULLY!!!\n", buffer);
	     }
	}
	else {
	     return 0;
	     //printf("SORRY, THE FLIGHT YOU WROTE ALREADY EXISTS, PLEASE TRY WITH ANOTHER ONE\n");
	}
	return 0;
}