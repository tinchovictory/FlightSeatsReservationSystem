#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/serverRequestHandler.h"
#include "includes/protocol.h"
#include "includes/dbObj.h"
#include "includes/list.h"
#include "includes/stringUtils.h"

TransactionResponse processRequest(Socket_t socket, Db_t db, char * buff);
TransactionResponse processAddFlight(Db_t db, char * buff);
TransactionResponse processBookFlight(Db_t db, char * buff);
TransactionResponse processDelFlight(Db_t db, char * buff);
TransactionResponse processCanReservation(Db_t db, char * buff);
TransactionResponse processGetFlights(Socket_t socket, Db_t db);
TransactionResponse processGetReservations(Socket_t socket, Db_t db, char * buff);
TransactionResponse processGetCanReservations(Socket_t socket, Db_t db, char * buff);
TransactionResponse processGetSeats(Socket_t socket, Db_t db, char * buff);
void sendFlight(Socket_t socket, FlightObj * flight);
void sendReservation(Socket_t socket, ReservationObj * reserv);
void sendSeat(Socket_t socket, FlightSeatObj * seat);
void answerRequest(Socket_t socket, TransactionResponse resp);
int transWaitNext(Socket_t socket);


void handleRequests(Socket_t socket, Db_t db) {
	int readBytes;
	char buff[TRANSACTION_SIZE + 1] = {0};
	TransactionResponse resp;

	while(1) {
		/* Clear buff */
		memset(buff, 0, TRANSACTION_SIZE);

		/* Listen request */
		if(recvMsg(socket, buff, TRANSACTION_SIZE) <= 0) {
			return;
		}

		/* Process request */
		resp = processRequest(socket, db, buff);

		/* Answer request */
		answerRequest(socket, resp);
	}
}

TransactionResponse processRequest(Socket_t socket, Db_t db, char * buff) {
	switch(buff[0] - '0') {
		case TRANS_ADD_FLIGHT:
			return processAddFlight(db, buff + 1);
		case TRANS_BOOK_FLIGHT:
			return processBookFlight(db, buff + 1);
		case TRANS_DEL_FLIGHT:
			return processDelFlight(db, buff + 1);
		case TRANS_CAN_RESERV:
			return processCanReservation(db, buff + 1);
		case TRANS_GET_FLIGHTS:
			return processGetFlights(socket, db);
		case TRANS_GET_RESERV:
			return processGetReservations(socket, db, buff + 1);
		case TRANS_GET_CANRESERV:
			return processGetCanReservations(socket, db, buff + 1);
		case TRANS_GET_SEATS:
			return processGetSeats(socket, db, buff + 1);
		default:
			return TRANS_UNKNOWN;
	}
	return TRANS_OK;
}


void answerRequest(Socket_t socket, TransactionResponse resp) {
	char buff[TRANSACTION_SIZE] = {0};
	buff[0] = resp + '0';
	sendMsg(socket,buff,TRANSACTION_SIZE);
}


TransactionResponse processAddFlight(Db_t db, char * buff) {
	int offset = 0;
	DbCode dbCode;

	FlightObj * flight = malloc(sizeof(FlightObj));
	flight->flightNo = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;
	flight->departure = buff + offset;
	offset += strlen(buff + offset) + 1;
	flight->arrival = buff + offset;
	offset += strlen(buff + offset) + 1;
	flight->price = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;
	flight->seats = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;
	flight->date = buff + offset;


	/* Critical zone */
	dbCode = addFlightDb(db, flight);

	if(dbCode != DB_OK) {
		return TRANS_ERR;
	}

	return TRANS_OK;
}


TransactionResponse processBookFlight(Db_t db, char * buff) {
	int offset = 0;
	DbCode dbCode;

	ReservationObj * reserv = malloc(sizeof(ReservationObj));
	reserv->flightNo = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;
	reserv->name = buff + offset;
	offset += strlen(buff + offset) + 1;
	reserv->seat = strToInt(buff + offset);
	
	/* Critical zone */
	dbCode = bookFlightDb(db, reserv);

	if(dbCode != DB_OK) {
		return TRANS_ERR;
	}

	return TRANS_OK;
}


TransactionResponse processDelFlight(Db_t db, char * buff) {
	DbCode dbCode;
	int flightNo = strToInt(buff);
	
	/* Critical zone */
	dbCode = removeFlightDb(db, flightNo);

	if(dbCode != DB_OK) {
		return TRANS_ERR;
	}

	return TRANS_OK;
}


TransactionResponse processCanReservation(Db_t db, char * buff) {
	DbCode dbCode;
	int reservationNo = strToInt(buff);
	
	/* Critical zone */
	dbCode = cancelReservationDb(db, reservationNo);

	if(dbCode != DB_OK) {
		return TRANS_ERR;
	}

	return TRANS_OK;
}



TransactionResponse processGetFlights(Socket_t socket, Db_t db) {
	ListPtr flightsList;
	ListIteratorPtr flightsIter;
	FlightObj * flight;
	/* Critical zone */
	flightsList = getFlightsDb(db);
	flightsIter = listIterator(flightsList);
	flight = malloc(sizeof(FlightObj));
	iteratorGetNext(flightsIter, flight);

	answerRequest(socket, TRANS_BEGIN);
	while(transWaitNext(socket) && iteratorHasNext(flightsIter)) {
		iteratorGetNext(flightsIter, flight);
		sendFlight(socket, flight);
	}
	free(flight);
	freeIterator(flightsIter);
	freeList(flightsList);
	return TRANS_OK;
}



TransactionResponse processGetReservations(Socket_t socket, Db_t db, char * buff) {
	ListPtr reservList;
	ListIteratorPtr reservIter;
	ReservationObj * reserv;

	/* Critical zone */
	reservList = getReservationsDb(db, strToInt(buff));

	reservIter = listIterator(reservList);
	reserv = malloc(sizeof(ReservationObj));

	answerRequest(socket, TRANS_BEGIN);

	while(transWaitNext(socket) && iteratorHasNext(reservIter)) {
		iteratorGetNext(reservIter, reserv);
		sendReservation(socket, reserv);
	}
	
	free(reserv);
	freeIterator(reservIter);
	freeList(reservList);

	return TRANS_OK;
}


TransactionResponse processGetCanReservations(Socket_t socket, Db_t db, char * buff) {
	ListPtr reservList;
	ListIteratorPtr reservIter;
	ReservationObj * reserv;

	/* Critical zone */
	reservList = getReservationsCancelledDb(db, strToInt(buff));

	reservIter = listIterator(reservList);
	reserv = malloc(sizeof(ReservationObj));

	answerRequest(socket, TRANS_BEGIN);

	while(transWaitNext(socket) && iteratorHasNext(reservIter)) {

		iteratorGetNext(reservIter, reserv);
		sendReservation(socket, reserv);

	}
	free(reserv);
	freeIterator(reservIter);
	freeList(reservList);

	return TRANS_OK;
}



TransactionResponse processGetSeats(Socket_t socket, Db_t db, char * buff) {
	ListPtr seatsList;
	ListIteratorPtr seatsIter;
	FlightSeatObj * seat;

	/* Critical zone */
	seatsList = getFlightSeatsBookedDb(db, strToInt(buff));

	seatsIter = listIterator(seatsList);
	seat = malloc(sizeof(FlightSeatObj));

	answerRequest(socket, TRANS_BEGIN);

	while(transWaitNext(socket) && iteratorHasNext(seatsIter)) {

		iteratorGetNext(seatsIter, seat);
		sendSeat(socket, seat);

	}
	free(seat);
	freeIterator(seatsIter);
	freeList(seatsList);

	return TRANS_OK;
}



int transWaitNext(Socket_t socket) {
	char buff[TRANSACTION_SIZE] = {0};
	if(recvMsg(socket, buff, TRANSACTION_SIZE) <= 0) {
		return 0;
	}
	if(buff[0] - '0' != TRANS_NEXT) {
		return 0;
	}
	return 1;
}



void sendFlight(Socket_t socket, FlightObj * flight) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};

	ptr +=  addToStr(ptr, intToStr(TRANS_RESP, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(flight->flightNo, intBuff));
	ptr += 1 + addToStr(ptr, flight->departure);
	ptr += 1 + addToStr(ptr, flight->arrival);
	ptr += 1 + addToStr(ptr, intToStr(flight->price, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(flight->seats, intBuff));
	ptr += 1 + addToStr(ptr, flight->date);

	sendMsg(socket,buff,TRANSACTION_SIZE);
}

void sendReservation(Socket_t socket, ReservationObj * reserv) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};

	ptr += addToStr(ptr, intToStr(TRANS_RESP, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(reserv->reservationNo, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(reserv->flightNo, intBuff));
	ptr += 1 + addToStr(ptr, reserv->name);
	ptr += 1 + addToStr(ptr, reserv->state);
	ptr += 1 + addToStr(ptr, intToStr(reserv->seat, intBuff));

	sendMsg(socket,buff,TRANSACTION_SIZE);
}

void sendSeat(Socket_t socket, FlightSeatObj * seat) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[MAX_NUM_SIZE] = {0};

	ptr += addToStr(ptr, intToStr(TRANS_RESP, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(seat->flightNo, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(seat->seat, intBuff));

	sendMsg(socket,buff,TRANSACTION_SIZE);
}