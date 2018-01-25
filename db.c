#include <stdio.h>
#include <stdlib.h>
#include "sqlite/sqlite3.h"

#include "includes/db.h"


typedef struct Db {
	sqlite3 * db;
} Db;


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
                "CREATE TABLE Flights(FlightNo INT PRIMARY KEY, Departure TEXT, Arrival TEXT, Price INT, Seats INT, Date TEXT);"
                "CREATE TABLE Reservations(ReservationNo INT PRIMARY KEY, FlightNo INT, Name TEXT, State TEXT, Seat TEXT,"
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