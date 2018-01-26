#include <stdio.h>
#include <stdlib.h>
#include "includes/db.h"

void checkDbError(DbCode code);

int main(void) {
	Db_t db;
	DbCode dbCode;

	/* Create a db connection */
	dbCode = dbInit(&db);
	checkDbError(dbCode);

	/* Install Db */
	dbCode = installDb(db);
	checkDbError(dbCode);

	/*** TESTING ****/

	dbCode = addFlight(db, 2, "Buenos Airesss", "JFK", 100, 50, "26/01/2018");
	checkDbError(dbCode);

	dbCode = addFlight(db, 105, "JFK", "Buenos Aires", 105, 50, "29/01/2018");
	checkDbError(dbCode);

	dbCode = printFlights(db);
	checkDbError(dbCode);

	dbCode = bookFlight(db, 105, "Martin Victory", "1A");
	checkDbError(dbCode);

	dbCode = bookFlight(db, 105, "Martin Victory", "1B");
	checkDbError(dbCode);

	dbCode = printReservations(db);
	checkDbError(dbCode);

	dbCode = cancelReservation(db, 2);
	checkDbError(dbCode);

	dbCode = printReservations(db);
	checkDbError(dbCode);


	/*** END TESTING *****/

	/* Close connection */
	dbClose(db);


	return 0;

}


 void checkDbError(DbCode code) {
	if(code == DB_OK) {
		return;
	}

	if(code == FLIGHTNOERR) {
		printf("The flight number is incorrect\n");
		return;
	}

	if(code == SEATERR) {
		printf("The seat is alrready taken\n");
		return;
	}

	printf("Db fail\n");
	exit(1);
}