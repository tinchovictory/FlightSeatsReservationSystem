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

	/* Close connection */
	dbClose(db);

	printf("\033[1;32m[Installed]\033[0m\n");
	return 0;

}


 void checkDbError(DbCode code) {
	if(code == DB_OK) {
		return;
	}
	if(code == DB_CONNERR) {
		printf("\n\033[0;31mFatal error while connecting to the data base.\033[0m\n");
	} else {
		printf("\n\033[0;31mUnknown fatal error while connecting to the data base.\033[0m\n");
	}
	printf("\n\033[0;31mExiting...\033[0m\n");
	exit(1);
}
