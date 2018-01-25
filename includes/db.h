#ifndef __DB_H__
#define __DB_H__

typedef struct Db * Db_t;

typedef enum {DB_OK, DB_CONNERR, DB_INSTALLERR, DB_INSERTERR, DB_SELECTERR, DB_DELETE, DB_UPDATEERR} DbCode;

DbCode dbInit(Db_t * db);

void dbClose(Db_t db);

DbCode installDb(Db_t db);

DbCode addFlight(Db_t db, int flightNo, char * departure, char * arrival, int price, int seats, char * date);

DbCode bookFlight(Db_t db, int flightNo, char * name, char * seat);

DbCode cancelReservation(Db_t db, int reservationNo);

DbCode removeFlight(Db_t db, int flightNo);

#endif