#include <stdio.h>
#include <stdlib.h>
#include "sqlite/sqlite3.h"

#include "includes/db.h"



typedef struct Db {
	sqlite3 * db;
} Db;


int printFlightsCallback(void *NotUsed, int argc, char **argv, char **azColName);//TESTING
int printReservationsCallback(void *NotUsed, int argc, char **argv, char **azColName);//TESTING

int checkFlightNo(Db_t db, int flightNo);
int checkSeat(Db_t db, int flightNo, char * seat);
DbCode insertReservation(Db_t db, int flightNo, char * name, char * seat);



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
                "CREATE TABLE Reservations(ReservationNo INTEGER PRIMARY KEY, FlightNo INTEGER, Name TEXT, State TEXT, Seat TEXT,"
                "FOREIGN KEY (FlightNo) REFERENCES Flights (FlightNo) ON DELETE CASCADE ON UPDATE NO ACTION);";

	resp = sqlite3_exec(db->db, sql, 0, 0, &errMsg);
    
    if (resp != SQLITE_OK ) {        
		printf("SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(db->db);

		return DB_INSTALLERR;
	}

	return DB_OK;
}


DbCode addFlight(Db_t db, int flightNo, char * departure, char * arrival, int price, int seats, char * date) {
	int resp;
	char * sql = sqlite3_mprintf("INSERT INTO Flights VALUES(%d, '%q', '%q', %d, %d, '%q');", flightNo, departure, arrival, price, seats, date);

	resp = sqlite3_exec(db->db, sql, 0, 0, 0);

	sqlite3_free(sql);
    
    if (resp != SQLITE_OK ) {    
		sqlite3_close(db->db);

		return DB_INSERTERR;
	}

	//printFlights(db);//TESTING

	return DB_OK;
}


DbCode removeFlight(Db_t db, int flightNo) {
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


DbCode bookFlight(Db_t db, int flightNo, char * name, char * seat) {

	/* Check flightNo exist and seat is not occupied */
	if(!checkFlightNo(db, flightNo)) {
		return FLIGHTNOERR;
	}
	if(!checkSeat(db, flightNo, seat)) {
		return SEATERR;
	}

	/* Insert reservation */
	return insertReservation(db, flightNo, name, seat);
}

/* Check if flightNo exists */
int checkFlightNo(Db_t db, int flightNo) {
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
int checkSeat(Db_t db, int flightNo, char * seat) {
	sqlite3_stmt * res;
	int resp;

	char * sql = sqlite3_mprintf("SELECT * FROM Reservations WHERE FlightNo = %d AND Seat = '%q';", flightNo, seat);

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
DbCode insertReservation(Db_t db, int flightNo, char * name, char * seat) {
	int resp;
	char * sql = sqlite3_mprintf("INSERT INTO Reservations VALUES(NULL, %d, '%q', 'Active', '%q');", flightNo, name, seat);

	resp = sqlite3_exec(db->db, sql, 0, 0, 0);

	sqlite3_free(sql);
    
    if (resp != SQLITE_OK ) {    
		sqlite3_close(db->db);

		return DB_INSERTERR;
	}

	return DB_OK;
}


DbCode cancelReservation(Db_t db, int reservationNo) {
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






/* TESTING */
DbCode printFlights(Db_t db) {
	int resp;
	char * errMsg;

	char * sql = "SELECT * FROM Flights;";

	resp = sqlite3_exec(db->db, sql, printFlightsCallback, 0, &errMsg);
    
    if (resp != SQLITE_OK ) {  
    	printf("SQL error: %s\n", errMsg);      
		sqlite3_close(db->db);
		sqlite3_free(errMsg);

		return DB_SELECTERR;
	}
}

int printFlightsCallback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;

	NotUsed = 0;
    
    for (i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");

    return 0;
}

DbCode printReservations(Db_t db) {
	int resp;
	char * errMsg;

	char * sql = "SELECT * FROM Reservations;";

	resp = sqlite3_exec(db->db, sql, printReservationsCallback, 0, &errMsg);
    
    if (resp != SQLITE_OK ) {  
    	printf("SQL error: %s\n", errMsg);      
		sqlite3_close(db->db);
		sqlite3_free(errMsg);

		return DB_SELECTERR;
	}
}

int printReservationsCallback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;

	NotUsed = 0;
    
    for (i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");

    return 0;
}