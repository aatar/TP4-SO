#ifndef TCPSERVER_H
#define TCPSERVER_H

#define PORT 4443
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define MAX_CONNECTIONS 50

void setupServerSocket();
int openDatabase();
int startsWith(const char *a, const char *b);
int createFlight(char * buffer);
int cancelFlight(char * buffer);
void removeFirst(char * s);

#endif