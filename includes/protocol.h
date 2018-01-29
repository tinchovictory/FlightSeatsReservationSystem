#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__


#define TRANSACTION_SIZE 1000

#define MAX_NUM_SIZE 5


/*
 * The first byte of the transaction will have the TransactionCode, the next bytes will have the rest of the data.
 */

typedef enum { TRANS_ADD_FLIGHT, TRANS_BOOK_FLIGHT, TRANS_DEL_FLIGHT, TRANS_CAN_RESERV, TRANS_GET_FLIGHTS, TRANS_GET_RESERV, TRANS_GET_CANRESERV, TRANS_GET_SEATS } TransactionCode;

typedef enum { TRANS_OK, TRANS_ERR, TRANS_UNKNOWN, TRANS_BEGIN, TRANS_NEXT, TRANS_RESP } TransactionResponse;

#endif