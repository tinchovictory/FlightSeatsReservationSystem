#include "socket.h"
#include "list.h"
#include "dbObj.h"




void app(Socket_t  socket);
void flow();
ListIteratorPtr getFlightsIterator();
char * getFlightInfo(int flightNo);
FlightObj * getFlightObj(int flightNo);
void flightStatus();
void makeReservation();
void removeReservation();
void manageflights();
