//  Consultar el estado de un vuelo​: se mostrará la disposición de asientos del vuelo
//  (DAV) indicado, donde en cada asiento se deberá indicar si el mismo está disponible
//  o reservado.

//  Realizar una reserva de un vuelo: ​se mostrará la DAV al momento para el vuelo
//  indicado, se le pide qué asiento desea reservar, y luego se mostrará la DAV después
//  del intento de realización de la reserva.

//  Cancelar una reserva: se le mostrará la DAV al momento para el vuelo indicado, se
//  le pide qué asiento desea cancelar, y luego se mostrará la DAV después del intento
//  de cancelación de la reserva.

//  Alta y Baja de vuelos: ​Permitir desde el cliente el alta y baja de vuelos. Al momento
//  de realizar el alta de un vuelo este cuenta con todos sus asientos disponibles.
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

#define PORT 8080
//#define MAX_CONNECTIONS 50


int clientSocket = 0;
int serverSocket = 0;
int errorClient = 0;
int errorServer = 0;
int generalError = 0;
int clientConnection = -1;
int serverConnection = -1;
struct sockaddr_in serverAddr;

sqlite3 *db;
int rc;

int main()
{
  runTest();
}

void runTest()
{
  printf("Test Connection with Server : \n", );
  testConectionWithServer();

  printf("Test Cli Request: \n");
  if (generalError != 0)
    // testCliRequest();
  else
    fail("Failed on connection");

}

void testConectionWithServer()
{
  givenSockets();
  givenServerAddr();

  whenConectClientServer();
  thenCheckConnection();

  whenBindConnectionServerClient();
  thenCheckBinding();
}


void givenSockets()
{
  clientSocket = socket(AF_INET,SOCK_STREAM,0);
  errorClient = verifySocket(clientSocket);
  serverSocket = socket(AF_INET,SOCK_STREAM,0);
  errorServer = verifySocket(serverSocket);
  errorClient = 0;
  errorServer = 0;
}

void givenServerAddr()
{
  serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
}

void whenConectClientServer()
{
  if (verifySocket(clientSocket))
  {
    clientConnection = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
  }
}

void thenCheckConnection()
{
  if (clientConnection < 0)
  {
    generalError = 1;
    fail("Error in connection from client");
    return;
  }
  ok("Connection success.");
}

void whenBindConnectionServerClient()
{
  if (verifySocket(serverSocket))
  {
    serverConnection = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  }

}

void thenCheckBinding()
{
  if (serverConnection < 0 )
  {
    generalError = 1;
    fail("Error in binding connection");
    return;
  }
  ok("Binding success.");
}

void testCliRequest()
{
  givenADB();
  whenEstablishedARequest();
  thenCheckResponse();
}

void givenADB()
{
  rc = sqlite3_open("flights.db", &db);
}

void whenEstablishedARequest()
{
  if(errorClient == 0 && erroServer == 0 && !rc)
  {

  }
}

void thenCheckResponse()
{
  if (errorClient == 1 || errorServer == 1 || rc) {
    generalError = 1;
    fail("Unable to create the request.");
  }
  else
  {

  }
}

void flightReserveTest()
{

}

void flightCancelReserveTest()
{

}

void flightCreationTest()
{

}

void flightDeletionTest()
{

}


int verifySocket(int socket)
{
  if (socket < 0)
  {
    return 1;

  }
  return 0;
}

void ok(char * msg)
{
	printf("	Ok! %s\n", msg);
}

void fail(char * withError)
{
	putchar('\n');
	fprintf(stderr, "%s", withError);
  putchar('\n');
}
