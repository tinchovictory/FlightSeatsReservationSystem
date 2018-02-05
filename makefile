makefile:

clean:
	@-rm client installDb server
	@echo "[Done.]"
clean_client:
	-rm client

all:
	@echo "[Compiling...]"
	@gcc -o installDb installDb.c list.c db.c sqlite/sqlite3.c -lpthread -ldl
	@gcc -o server server.c socket.c db.c list.c stringUtils.c serverRequestHandler.c dbSincronization.c serverUi.c sqlite/sqlite3.c -lpthread -ldl
	@gcc -o client client.c socket.c stringUtils.c list.c clientRequestBuilder.c gui.c guiUtils.c
	@echo "[Installing DataBase...]"
	@./installDb
	@echo "[Done.]"
client:
	@gcc -o client client.c socket.c stringUtils.c list.c clientRequestBuilder.c gui.c guiUtils.c
