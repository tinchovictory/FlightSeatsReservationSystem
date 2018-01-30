#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/clientRequestBuilder.h"
#include "includes/protocol.h"
#include "includes/stringUtils.h"

FlightObj * strToFlight(char * buff);
ReservationObj * strToReservation(char * buff);
FlightSeatObj * strToSeat(char * buff);


int addFlight(Socket_t socket, FlightObj * flight) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};

	ptr += addToStr(ptr, intToStr(TRANS_ADD_FLIGHT, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(flight->flightNo, intBuff));
	ptr += 1 + addToStr(ptr, flight->departure);
	ptr += 1 + addToStr(ptr, flight->arrival);
	ptr += 1 + addToStr(ptr, intToStr(flight->price, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(flight->seats, intBuff));
	ptr += 1 + addToStr(ptr, flight->date);

	sendMsg(socket,buff,TRANSACTION_SIZE);

	memset(buff, 0, TRANSACTION_SIZE);

	recvMsg(socket, buff, TRANSACTION_SIZE);

	if(buff[0] - '0' != TRANS_OK) {
		return 0;
	}

	return 1;
}



int bookFlight(Socket_t socket, ReservationObj * reserv) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};

	ptr += addToStr(ptr, intToStr(TRANS_BOOK_FLIGHT, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(reserv->flightNo, intBuff));
	ptr += 1 + addToStr(ptr, reserv->name);
	ptr += 1 + addToStr(ptr, reserv->seat);

	sendMsg(socket,buff,TRANSACTION_SIZE);

	memset(buff, 0, TRANSACTION_SIZE);

	recvMsg(socket, buff, TRANSACTION_SIZE);

	if(buff[0] - '0' != TRANS_OK) {
		return 0;
	}

	return 1;
}




int deleteFlight(Socket_t socket, int flightNo) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};

	ptr += addToStr(ptr, intToStr(TRANS_DEL_FLIGHT, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(flightNo, intBuff));

	sendMsg(socket,buff,TRANSACTION_SIZE);

	memset(buff, 0, TRANSACTION_SIZE);

	recvMsg(socket, buff, TRANSACTION_SIZE);

	if(buff[0] - '0' != TRANS_OK) {
		return 0;
	}

	return 1;
}



int cancelReservation(Socket_t socket, int reservNo) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};

	ptr += addToStr(ptr, intToStr(TRANS_CAN_RESERV, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(reservNo, intBuff));
	
	sendMsg(socket,buff,TRANSACTION_SIZE);

	memset(buff, 0, TRANSACTION_SIZE);

	recvMsg(socket, buff, TRANSACTION_SIZE);

	if(buff[0] - '0' != TRANS_OK) {
		return 0;
	}

	return 1;
}



ListPtr getFlights(Socket_t socket) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};
	ListPtr list = listInit(sizeof(FlightObj));

	/* Ask for flights */
	ptr += addToStr(ptr, intToStr(TRANS_GET_FLIGHTS, intBuff));
	sendMsg(socket,buff,TRANSACTION_SIZE);
	/* Wait confirmation */
	memset(buff, 0, TRANSACTION_SIZE);
	recvMsg(socket, buff, TRANSACTION_SIZE);
	
	if(buff[0] - '0' != TRANS_BEGIN) {
		return NULL;
	}
	
/* Transaction begin, save flights to the list */
	while(buff[0] - '0' == TRANS_BEGIN || buff[0] - '0' == TRANS_RESP) {
		/* Ask for next */
		memset(buff, 0, TRANSACTION_SIZE);
		buff[0] = TRANS_NEXT + '0';
		sendMsg(socket,buff,TRANSACTION_SIZE);

		/* Get response */
		memset(buff, 0, TRANSACTION_SIZE);
		recvMsg(socket, buff, TRANSACTION_SIZE);
		if(buff[0] - '0' == TRANS_RESP) {
			FlightObj * flight = strToFlight(buff + 1);
			addToList(list, strToFlight(buff + 1));
		}
	}

	if(buff[0] - '0' != TRANS_OK) {
		freeList(list);
		return NULL;
	}
	return list;
}




ListPtr getReservations(Socket_t socket, int flighNo) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};
	ListPtr list = listInit(sizeof(ReservationObj));

	/* Ask for reservations */
	ptr += addToStr(ptr, intToStr(TRANS_GET_RESERV, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(flighNo, intBuff));
	sendMsg(socket,buff,TRANSACTION_SIZE);

	/* Wait confirmation */
	memset(buff, 0, TRANSACTION_SIZE);
	recvMsg(socket, buff, TRANSACTION_SIZE);

	if(buff[0] - '0' != TRANS_BEGIN) {
		return NULL;
	}

	/* Transaction begin, save reservations to the list */
	while(buff[0] - '0' == TRANS_BEGIN || buff[0] - '0' == TRANS_RESP) {
		/* Ask for next */
		memset(buff, 0, TRANSACTION_SIZE);
		buff[0] = TRANS_NEXT + '0';
		sendMsg(socket,buff,TRANSACTION_SIZE);

		/* Get response */
		memset(buff, 0, TRANSACTION_SIZE);
		recvMsg(socket, buff, TRANSACTION_SIZE);

		if(buff[0] - '0' == TRANS_RESP) {
			addToList(list, strToReservation(buff + 1));
		}
	}

	if(buff[0] - '0' != TRANS_OK) {
		freeList(list);
		return NULL;
	}

	return list;
}


ListPtr getCanReservations(Socket_t socket, int flighNo) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};
	ListPtr list = listInit(sizeof(ReservationObj));

	/* Ask for reservations */
	ptr += addToStr(ptr, intToStr(TRANS_GET_CANRESERV, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(flighNo, intBuff));
	sendMsg(socket,buff,TRANSACTION_SIZE);

	/* Wait confirmation */
	memset(buff, 0, TRANSACTION_SIZE);
	recvMsg(socket, buff, TRANSACTION_SIZE);

	if(buff[0] - '0' != TRANS_BEGIN) {
		return NULL;
	}

	/* Transaction begin, save reservations to the list */
	while(buff[0] - '0' == TRANS_BEGIN || buff[0] - '0' == TRANS_RESP) {
		/* Ask for next */
		memset(buff, 0, TRANSACTION_SIZE);
		buff[0] = TRANS_NEXT + '0';
		sendMsg(socket,buff,TRANSACTION_SIZE);

		/* Get response */
		memset(buff, 0, TRANSACTION_SIZE);
		recvMsg(socket, buff, TRANSACTION_SIZE);

		if(buff[0] - '0' == TRANS_RESP) {
			addToList(list, strToReservation(buff + 1));
		}
	}

	if(buff[0] - '0' != TRANS_OK) {
		freeList(list);
		return NULL;
	}

	return list;
}


ListPtr getSeats(Socket_t socket, int flighNo) {
	char buff[TRANSACTION_SIZE] = {0};
	char * ptr = buff;
	char intBuff[5] = {0};
	ListPtr list = listInit(sizeof(FlightSeatObj));

	/* Ask for reservations */
	ptr += addToStr(ptr, intToStr(TRANS_GET_SEATS, intBuff));
	ptr += 1 + addToStr(ptr, intToStr(flighNo, intBuff));
	sendMsg(socket,buff,TRANSACTION_SIZE);

	/* Wait confirmation */
	memset(buff, 0, TRANSACTION_SIZE);
	recvMsg(socket, buff, TRANSACTION_SIZE);

	if(buff[0] - '0' != TRANS_BEGIN) {
		return NULL;
	}

	/* Transaction begin, save reservations to the list */
	while(buff[0] - '0' == TRANS_BEGIN || buff[0] - '0' == TRANS_RESP) {
		/* Ask for next */
		memset(buff, 0, TRANSACTION_SIZE);
		buff[0] = TRANS_NEXT + '0';
		sendMsg(socket,buff,TRANSACTION_SIZE);

		/* Get response */
		memset(buff, 0, TRANSACTION_SIZE);
		recvMsg(socket, buff, TRANSACTION_SIZE);

		if(buff[0] - '0' == TRANS_RESP) {
			addToList(list, strToSeat(buff + 1));
		}
	}

	if(buff[0] - '0' != TRANS_OK) {
		freeList(list);
		return NULL;
	}

	return list;
}




FlightObj * strToFlight(char * buff) {
	int offset = 0;
	FlightObj * flight = malloc(sizeof(FlightObj));

	flight->flightNo = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;

	flight->departure = malloc(strlen(buff + offset));	
	strcpy(flight->departure, buff + offset);
	offset += strlen(buff + offset) + 1;

	flight->arrival = malloc(strlen(buff + offset));
	strcpy(flight->arrival, buff + offset);
	offset += strlen(buff + offset) + 1;

	flight->price = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;
	
	flight->seats = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;
	
	flight->date = malloc(strlen(buff + offset));
	strcpy(flight->date, buff + offset);

	return flight;
}

ReservationObj * strToReservation(char * buff) {
	int offset = 0;
	ReservationObj * reserv = malloc(sizeof(ReservationObj));

	reserv->reservationNo = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;

	reserv->flightNo = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;

	reserv->name = malloc(strlen(buff + offset));
	strcpy(reserv->name, buff + offset);
	offset += strlen(buff + offset) + 1;

	reserv->state = malloc(strlen(buff + offset));
	strcpy(reserv->state, buff + offset);
	offset += strlen(buff + offset) + 1;

	reserv->seat = malloc(strlen(buff + offset));
	strcpy(reserv->seat, buff + offset);

	return reserv;
}

FlightSeatObj * strToSeat(char * buff) {
	int offset = 0;
	FlightSeatObj * seat = malloc(sizeof(FlightSeatObj));

	seat->flightNo = strToInt(buff + offset);
	offset += strlen(buff + offset) + 1;

	seat->seat = malloc(strlen(buff + offset));
	strcpy(seat->seat, buff + offset);
	
	return seat;
}
