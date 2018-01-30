#ifndef __CLIENT_REQUEST_BUILDER_H__
#define __CLIENT_REQUEST_BUILDER_H__

#include "socket.h"
#include "list.h"
#include "dbObj.h"

/* Returns 1 if flight is added, 0 if error */
int addFlight(Socket_t socket, FlightObj * flight);

/*  */
int bookFlight(Socket_t socket, ReservationObj * reserv);


int deleteFlight(Socket_t socket, int flightNo);

int cancelReservation(Socket_t socket, int reservNo);

ListPtr getFlights(Socket_t socket);

ListPtr getReservations(Socket_t socket, int flighNo);

ListPtr getCanReservations(Socket_t socket, int flighNo);

ListPtr getSeats(Socket_t socket, int flighNo);


#endif