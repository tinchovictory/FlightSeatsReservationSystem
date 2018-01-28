#ifndef __DBOOBJ_H__
#define __DBOOBJ_H__

typedef struct FlightObj {
	int flightNo;
	char * departure;
	char * arrival;
	int price;
	int seats;
	char * date;
} FlightObj;

typedef struct ReservationObj {
	int reservationNo;
	int flightNo;
	char * name;
	char * state;
	char * seat;
} ReservationObj;

typedef struct FlightSeatObj {
	int flightNo;
	char * seat;
} FlightSeatObj;

#endif