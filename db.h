#ifndef __DB_H__
#define __DB_H__

typedef struct Db * Db_t;

typedef enum {DB_OK, DB_CONNERR} DbCode;

DbCode dbInit(Db_t * db);

void dbClose(Db_t db);

#endif