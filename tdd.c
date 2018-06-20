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

//#define PORT 4445
//#define MAX_CONNECTIONS 50


int clientSocket = 0;
int serverSocket = 0;
int errorClient = 0;
int errorServer = 0;
struct sockaddr_in serverAddr;

void runTest()
{
  printf("Test Cli Request: \n");
  testCliRequest();


}

void testCliRequest()
{
  givenASocket();
  givenServerAddr();

  whenEstablishedARequest();
  thenCheckResponse();
}

void givenSockets()
{
  clientSocket = socket(AF_INET,SOCK_STREAM,0);
  serverSocket = socket(AF_INET,SOCK_STREAM,0);
  errorClient = 0;
  errorServer = 0;
}

void givenServerAddr()
{
  serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
}

void whenEstablishedARequest()
{
  errorClient = verifySocket(clientSocket);
  errorServer = verifySocket(serverSocket);

  if(errorClient == 0 && erroServer == 0)
  {

  }
}

int verifySocket(int socket)
{
  if (socket < 0)
  {
    return 1;

  }
  return 0;
}


void thenCheckResponse()
{
  if (errorClient == 1 || errorServer == 1) {
    fail("Unable to create the socket");
  }
  else
  {

  }
}


void ok()
{
	printf("	Ok!\n");
}

void fail(char * withError)
{
	putchar('\n');
	fprintf(stderr, "%s", withError);
  putchar('\n');
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
