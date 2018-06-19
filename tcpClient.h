#include <stdarg.h>

#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#define PORT 4443
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)



void setupClientSocket();
void createFlight();
void cancelFlight();
void seeFlight();
void bookSeat();
void cancelSeatBooking();
void showExistingFlights();
void showSeatsArrangement(char * flight);
int book(char * flight, int seatNumber);
int cancelBooking(char * flight, int seatNumber);
int changeSeatsArrangement(char * flight, int seatNumber, int hasToBook);
int startsWith(const char *a, const char *b);
int getint(const char message[], ...);
void toUpperCase(char* s);

#endif