#ifndef __DB_H__
#define __DB_H__

#include "list.h"
#include "dbObj.h"

typedef struct Db * Db_t;

typedef enum {DB_OK, DB_CONNERR, DB_INSTALLERR, DB_INSERTERR, DB_SELECTERR, DB_DELETEERR, DB_UPDATEERR, FLIGHTNOERR, SEATERR} DbCode;

DbCode dbInit(Db_t * db);

void dbClose(Db_t db);

DbCode installDb(Db_t db);

DbCode addFlightDb(Db_t db, FlightObj * flight);

DbCode bookFlightDb(Db_t db, ReservationObj * reserv);

DbCode cancelReservationDb(Db_t db, int reservationNo);

DbCode removeFlightDb(Db_t db, int flightNo);

ListPtr getFlightsDb(Db_t db);

ListPtr getReservationsDb(Db_t db, int flightNo);

ListPtr getReservationsCancelledDb(Db_t db, int flightNo);

ListPtr getFlightSeatsBookedDb(Db_t db, int flightNo);


#endif