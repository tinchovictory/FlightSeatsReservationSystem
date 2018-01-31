#include <stdio.h>
#include <stdlib.h>
#include "includes/db.h"

//TESTING
#include "includes/list.h"
#include "includes/dbObj.h"
void printReservations(Db_t db, int flightNo);
void printSeats(Db_t db, int flightNo);

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

	FlightObj * flight = malloc(sizeof(FlightObj));
	flight->flightNo = 1010;
	flight->departure = "New York";
	flight->arrival = "London";
	flight->price = 1000;
	flight->seats = 50;
	flight->date = "31/01/2018";

	addFlightDb(db, flight);

	/*

	dbCode = addFlight(db, 2, "Buenos Airesss", "JFK", 100, 50, "26/01/2018");
	checkDbError(dbCode);

	dbCode = addFlight(db, 105, "JFK", "Buenos Aires", 105, 50, "29/01/2018");
	checkDbError(dbCode);

	/*dbCode = printFlights(db);
	checkDbError(dbCode);
*//*
	ListPtr flightsList = getFlights(db);
	ListIteratorPtr flightIterator = listIterator(flightsList);
	FlightObj * flight = malloc(sizeof(FlightObj));
	while(iteratorHasNext(flightIterator)) {
		iteratorGetNext(flightIterator, flight);
		printf("Flight: %d, from: %s, to: %s\n",flight->flightNo, flight->departure, flight->arrival);
	}
	freeIterator(flightIterator);
	freeList(flightsList);


	dbCode = bookFlight(db, 105, "Martin Victory", "1A");
	checkDbError(dbCode);

	dbCode = bookFlight(db, 105, "Martin Victory", "1B");
	checkDbError(dbCode);

printf("\n\n");
printReservations(db, 105);


printf("\n\n");
printSeats(db, 105);


	dbCode = cancelReservation(db, 2);
	checkDbError(dbCode);

printf("\n\n");

	printReservations(db, 105);


	/*** END TESTING *****/

	/* Close connection */
	dbClose(db);

	printf("\033[1;32m[Installed]\n");
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
/*
void printReservations(Db_t db, int flightNo) {
	ListPtr reservationList = getReservationsDb(db, flightNo);
	ListIteratorPtr reservationIterator = listIterator(reservationList);
	ReservationObj * reservation = malloc(sizeof(ReservationObj));
	while(iteratorHasNext(reservationIterator)) {
		iteratorGetNext(reservationIterator, reservation);
		printf("Reservation: %d, Flight: %d, Name: %s, Seat: %s\n", reservation->reservationNo, reservation->flightNo, reservation->name, reservation->seat);
	}
	free(reservation);
	freeIterator(reservationIterator);
	freeList(reservationList);
}

void printSeats(Db_t db, int flightNo) {
	ListPtr seatList = getFlightSeatsBookedDb(db, flightNo);
	ListIteratorPtr seatIterator = listIterator(seatList);
	FlightSeatObj * seat = malloc(sizeof(FlightSeatObj));
	while(iteratorHasNext(seatIterator)) {
		iteratorGetNext(seatIterator, seat);
		printf("Flight: %d, Seat: %s\n", seat->flightNo, seat->seat);
	}
	free(seat);
	freeIterator(seatIterator);
	freeList(seatList);
}*/