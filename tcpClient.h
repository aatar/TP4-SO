#include <stdarg.h>

#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define ALL_OCCUPIED 1
#define ALL_FREE 2



void setupClientSocket();
void createFlight();
void cancelFlight();
void seeFlight();
void bookSeat();
void cancelSeatBooking();
void showExistingFlights();
int showSeatsArrangement(char * flight);
int book(char * flight, int seatNumber);
int cancelBooking(char * flight, int seatNumber);
int changeSeatsArrangement(char * flight, int seatNumber, int hasToBook);
int startsWith(const char *a, const char *b);
int getint(const char message[], ...);
void toUpperCase(char* s);
void selectFlight();
void printMenu();

#endif