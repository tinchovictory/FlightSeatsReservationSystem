#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite/sqlite3.h"

#include "includes/db.h"


typedef struct Db {
	sqlite3 * db;
} Db;


int checkFlightNoDb(Db_t db, int flightNo);
int checkSeatDb(Db_t db, int flightNo, int seat);
DbCode insertReservationDb(Db_t db, ReservationObj * reserv);



DbCode dbInit(Db_t * dbPtr) {

	int resp;
	Db_t db = (Db_t) malloc(sizeof(Db));

    resp = sqlite3_open("airline.db", &(db->db));
    
    if (resp != SQLITE_OK) {
        sqlite3_close(db->db);
        
        return DB_CONNERR;
    }

	*dbPtr = db;

	return DB_OK;
}

void dbClose(Db_t db) {
	sqlite3_close(db->db);
}

DbCode installDb(Db_t db) {
	int resp;
	char * errMsg;

	char *sql = "DROP TABLE IF EXISTS Reservations;"
				"DROP TABLE IF EXISTS Flights;" 
                "CREATE TABLE Flights(FlightNo INTEGER PRIMARY KEY, Departure TEXT, Arrival TEXT, Price INTEGER, Seats INTEGER, Date TEXT);"
                "CREATE TABLE Reservations(ReservationNo INTEGER PRIMARY KEY, FlightNo INTEGER, Name TEXT, State TEXT, Seat INTEGER,"
                "FOREIGN KEY (FlightNo) REFERENCES Flights (FlightNo) ON DELETE CASCADE ON UPDATE NO ACTION);";

	resp = sqlite3_exec(db->db, sql, 0, 0, &errMsg);
    
    if (resp != SQLITE_OK ) {        
		printf("SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(db->db);

		return DB_INSTALLERR;
	}

	//Initialize Flights
	int contInic = 0;
	char from[5][10] = {"Bs As", "Miami", "Cordoba", "Iguazu", "Chile"},to[5][10] = {"Madrid" ,"Venecia", "Orlando", "Los Andes", "Brazil"}, date[5][11] = {"12/12/2018", "14/11/2018", "15/03/2018", "07/08/2018", "24/05/18"};
	int price[5] = {100, 200, 300, 400, 500}, seats[5] = {35, 304, 123, 122, 643};
	while(contInic<5){
		char *sql2 = sqlite3_mprintf("INSERT INTO Flights VALUES(%d, '%q', '%q', %d, %d, '%q');",contInic++ ,from[contInic], to[contInic], price[contInic], seats[contInic], date[contInic] );
		resp = sqlite3_exec(db->db, sql2, 0, 0, &errMsg);
	    
	    if (resp != SQLITE_OK ) {        
			printf("SQL error2: %s\n", errMsg);
			sqlite3_free(errMsg);
			sqlite3_close(db->db);

			return DB_INSTALLERR;
		}
	}

	//Initialize Reservations
	contInic = 0;
	char name[5][10] = {"juan", "pedro", "luis", "jose", "ariel"};
	int flightNo[5] = {0,0,0,1,3}, seat[5] = {2, 1, 13, 25, 43};
	while(contInic<5){
		char *sql2 = sqlite3_mprintf("INSERT INTO Reservations VALUES(%d, %d, '%q', '%q', %d);",contInic++ ,flightNo[contInic], name[contInic], "Active", seat[contInic]);
		resp = sqlite3_exec(db->db, sql2, 0, 0, &errMsg);
	    
	    if (resp != SQLITE_OK ) {        
			printf("SQL error2: %s\n", errMsg);
			sqlite3_free(errMsg);
			sqlite3_close(db->db);

			return DB_INSTALLERR;
		}
	}


	return DB_OK;
}

DbCode addFlightDb(Db_t db, FlightObj * flight) {
	int resp;
	char * sql = sqlite3_mprintf("INSERT INTO Flights VALUES(%d, '%q', '%q', %d, %d, '%q');", flight->flightNo, flight->departure, flight->arrival, flight->price, flight->seats, flight->date);

	resp = sqlite3_exec(db->db, sql, 0, 0, 0);

	sqlite3_free(sql);
    if (resp != SQLITE_OK ) {    
		sqlite3_close(db->db);
		return DB_INSERTERR;
	}
	
	return DB_OK;
}


DbCode removeFlightDb(Db_t db, int flightNo) {
	int resp;
	char * sql = sqlite3_mprintf("DELETE FROM Flights WHERE FlightNo = %d;", flightNo);

	resp = sqlite3_exec(db->db, sql, 0, 0, 0);

	sqlite3_free(sql);
    
    if (resp != SQLITE_OK ) {    
		sqlite3_close(db->db);

		return DB_DELETEERR;
	}

	return DB_OK;
}



/* Check if flightNo exists */
int checkFlightNoDb(Db_t db, int flightNo) {
	sqlite3_stmt * res;
	int resp;

	char * sql = sqlite3_mprintf("SELECT * FROM Flights WHERE FlightNo = %d;", flightNo);

	resp = sqlite3_prepare_v2(db->db, sql, -1, &res, 0);

	if(resp != SQLITE_OK) {
		return 0;
	}

	resp = sqlite3_step(res);

	sqlite3_finalize(res);

	if(resp == SQLITE_ROW) {
		return 1;
	}
	return 0;
}

/* Check if the seat is available */
int checkSeatDb(Db_t db, int flightNo, int seat) {
	sqlite3_stmt * res;
	int resp;

	char * sql = sqlite3_mprintf("SELECT * FROM Reservations WHERE FlightNo = %d AND Seat = %d AND State = 'Active';", flightNo, seat);

	resp = sqlite3_prepare_v2(db->db, sql, -1, &res, 0);

	if(resp != SQLITE_OK) {
		return 0;
	}

	resp = sqlite3_step(res);

	sqlite3_finalize(res);

	if(resp == SQLITE_DONE) {
		return 1;
	}
	return 0;
}

/**/
DbCode insertReservationDb(Db_t db, ReservationObj * reserv) {
	int resp;
	char * sql = sqlite3_mprintf("INSERT INTO Reservations VALUES(NULL, %d, '%q', 'Active', %d);", reserv->flightNo, reserv->name, reserv->seat);

	resp = sqlite3_exec(db->db, sql, 0, 0, 0);

	sqlite3_free(sql);
    
    if (resp != SQLITE_OK ) {    
		sqlite3_close(db->db);

		return DB_INSERTERR;
	}

	return DB_OK;
}


DbCode bookFlightDb(Db_t db, ReservationObj * reserv) {

	/* Check flightNo exist and seat is not occupied */
	if(!checkFlightNoDb(db, reserv->flightNo)) {
		return FLIGHTNOERR;
	}
	if(!checkSeatDb(db, reserv->flightNo, reserv->seat)) {
		return SEATERR;
	}

	/* Insert reservation */
	return insertReservationDb(db, reserv);
}
DbCode cancelReservationDb(Db_t db, int reservationNo) {
	int resp;
	char * sql = sqlite3_mprintf("UPDATE Reservations SET State = 'Canceled' WHERE ReservationNo = %d;", reservationNo);

	resp = sqlite3_exec(db->db, sql, 0, 0, 0);

	sqlite3_free(sql);
    
    if (resp != SQLITE_OK ) {    
		sqlite3_close(db->db);

		return DB_UPDATEERR;
	}

	return DB_OK;
}

/*
 *
 */
ListPtr getFlightsDb(Db_t db) {

	sqlite3_stmt * res;
	int resp;
	ListPtr list;
	const char * str;

	char * sql = sqlite3_mprintf("SELECT * FROM Flights;");

	resp = sqlite3_prepare_v2(db->db, sql, -1, &res, NULL);

	if(resp != SQLITE_OK) {
		return NULL;
	}
	list = listInit(sizeof(FlightObj));

	while((resp = sqlite3_step(res)) == SQLITE_ROW) {
		FlightObj * flight = malloc(sizeof(FlightObj));

		flight->flightNo = sqlite3_column_int(res, 0);
		str = sqlite3_column_text(res,1);
		flight->departure = malloc(strlen(str)+1);
		strcpy(flight->departure, str);

		str = sqlite3_column_text(res,2);
		flight->arrival = malloc(strlen(str)+1);
		strcpy(flight->arrival, str);

		flight->price = sqlite3_column_int(res, 3);
		flight->seats = sqlite3_column_int(res, 4);

		str = sqlite3_column_text(res,5);
		flight->date = malloc(strlen(str)+1);
		strcpy(flight->date, str);		
		
		addToList(list, flight);
	}

	sqlite3_finalize(res);

	if(resp == SQLITE_DONE) {
		return list;
	}

	freeList(list);
	return NULL;
}

/*
 *
 */
ListPtr getReservationsDb(Db_t db, int flightNo) {
	sqlite3_stmt * res;
	int resp;
	ListPtr list;
	const char * str;

	char * sql = sqlite3_mprintf("SELECT * FROM Reservations WHERE FlightNo = '%d' AND State = 'Active';", flightNo);

	resp = sqlite3_prepare_v2(db->db, sql, -1, &res, 0);

	if(resp != SQLITE_OK) {
		return NULL;
	}

	list = listInit(sizeof(ReservationObj));

	while((resp = sqlite3_step(res)) == SQLITE_ROW) {
		ReservationObj * reservation = malloc(sizeof(ReservationObj));

		reservation->reservationNo = sqlite3_column_int(res, 0);
		reservation->flightNo = sqlite3_column_int(res, 1);

		str = sqlite3_column_text(res,2);
		reservation->name = malloc(strlen(str)+1);
		strcpy(reservation->name, str);

		str = sqlite3_column_text(res,3);
		reservation->state = malloc(strlen(str)+1);
		strcpy(reservation->state, str);

		reservation->seat = sqlite3_column_int(res, 4);		
		
		addToList(list, reservation);
	}

	sqlite3_finalize(res);

	if(resp == SQLITE_DONE) {
		return list;
	}

	freeList(list);
	return NULL;
}


/*
 *
 */
ListPtr getReservationsCancelledDb(Db_t db, int flightNo) {
	sqlite3_stmt * res;
	int resp;
	ListPtr list;
	const char * str;

	char * sql = sqlite3_mprintf("SELECT * FROM Reservations WHERE FlightNo = '%d';", flightNo);

	resp = sqlite3_prepare_v2(db->db, sql, -1, &res, 0);

	if(resp != SQLITE_OK) {
		return NULL;
	}

	list = listInit(sizeof(ReservationObj));

	while((resp = sqlite3_step(res)) == SQLITE_ROW) {
		ReservationObj * reservation = malloc(sizeof(ReservationObj));

		reservation->reservationNo = sqlite3_column_int(res, 0);
		reservation->flightNo = sqlite3_column_int(res, 1);

		str = sqlite3_column_text(res,2);
		reservation->name = malloc(strlen(str)+1);
		strcpy(reservation->name, str);

		str = sqlite3_column_text(res,3);
		reservation->state = malloc(strlen(str)+1);
		strcpy(reservation->state, str);

		reservation->seat = sqlite3_column_int(res, 4);			
		
		addToList(list, reservation);
	}

	sqlite3_finalize(res);

	if(resp == SQLITE_DONE) {
		return list;
	}

	freeList(list);
	return NULL;
}


/*
 *
 */
ListPtr getFlightSeatsBookedDb(Db_t db, int flightNo) {
	sqlite3_stmt * res;
	int resp;
	ListPtr list;
	const char * str;

	char * sql = sqlite3_mprintf("SELECT Seat FROM Reservations WHERE FlightNo = '%d' AND State = 'Active' ORDER BY Seat ASC;", flightNo);

	resp = sqlite3_prepare_v2(db->db, sql, -1, &res, 0);

	if(resp != SQLITE_OK) {
		return NULL;
	}

	list = listInit(sizeof(FlightSeatObj));

	while((resp = sqlite3_step(res)) == SQLITE_ROW) {
		FlightSeatObj * flightSeat = malloc(sizeof(FlightSeatObj));

		flightSeat->flightNo = flightNo;

		flightSeat->seat = sqlite3_column_int(res, 0);

		addToList(list, flightSeat);
	}

	sqlite3_finalize(res);

	if(resp == SQLITE_DONE) {
		return list;
	}

	freeList(list);
	return NULL;
}