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