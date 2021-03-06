#ifndef TCPSERVER_H
#define TCPSERVER_H

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define MAX_CONNECTIONS 50

void setupServerSocket();
int openDatabase();
int startsWith(const char *a, const char *b);
int createFlight(char * name);
int cancelFlight(char * name);
void removeFirst(char * s);
int flightExists(char * name);
void sig_handler(int signo);
void sig_handler_child(int signo);
void* wait_child(void* arg);

#endif