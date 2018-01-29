#ifndef __DB_H__
#define __DB_H__

#include "list.h"
#include "dbObj.h"

typedef struct Db * Db_t;

typedef enum {DB_OK, DB_CONNERR, DB_INSTALLERR, DB_INSERTERR, DB_SELECTERR, DB_DELETEERR, DB_UPDATEERR, FLIGHTNOERR, SEATERR} DbCode;

DbCode dbInit(Db_t * db);

void dbClose(Db_t db);

DbCode installDb(Db_t db);

DbCode addFlight(Db_t db, FlightObj * flight);

DbCode bookFlight(Db_t db, ReservationObj * reserv);

DbCode cancelReservation(Db_t db, int reservationNo);

DbCode removeFlight(Db_t db, int flightNo);

ListPtr getFlights(Db_t db);

ListPtr getReservations(Db_t db, int flightNo);

ListPtr getReservationsCancelled(Db_t db, int flightNo);

ListPtr getFlightSeatsBooked(Db_t db, int flightNo);


#endif