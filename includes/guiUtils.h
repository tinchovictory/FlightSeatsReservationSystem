#ifndef __GUI_UTILS__
#define __GUI_UTILS__

#include "dbObj.h"
#include "socket.h"

#define STRINGMAXLENGTH 50
#define ROWQUANTITY 3
#define DATE_LENGHT 11

typedef enum {SEAT_OK, SEAT_TAKEN, FULL_FLIGHT, SEAT_NOT_FOUND, FLIGHT_NOT_FOUND} CheckSeatResp;

int flightsLoaded(Socket_t socket);

FlightObj * getFlightObj(Socket_t socket, int flightNo);

char * readString(char * buff, int buffSize);

int readInt();

char * readAndCpyStr();

char * readDate();

int readIntHelp();

char * readPureStr(char * buff, int buffSize);

int isValidDate(int day, int month, int year);

void clearBuff();

int checkFlight(Socket_t socket, int flightNo);

CheckSeatResp checkSeat(Socket_t socket, int flightNo, int seat);

int checkReservation(Socket_t socket, int flightNo, int reservNo);

#endif