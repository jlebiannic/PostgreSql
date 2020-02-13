/*
 ============================================================================
 Name        : PostgreSql.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"
#include "h/commons.h"

int main(int argc, char **argv) {

	const char *conninfo = NULL;
	PGconn *conn;
	PGresult *res;
	int nFields;
	int i, j;

	if (argc > 1)
		conninfo = argv[1];

	conn = connect(conninfo);

	beginTransaction(conn);

	/*
	 * Récupère les lignes de la table "data"
	 */
	res = PQexec(conn, "DECLARE myportal CURSOR FOR select * from data");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(conn));
		PQclear(res);
		closeAndExit(conn);
	}
	PQclear(res);

	res = PQexec(conn, "FETCH ALL in myportal");
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(conn));
		PQclear(res);
		closeAndExit(conn);
	}

	/* affiche d'abord les noms des attributs */
	nFields = PQnfields(res);
	for (i = 0; i < nFields; i++)
		printf("%-15s", PQfname(res, i));
	printf("\n\n");

	/* puis affiche les lignes */
	for (i = 0; i < PQntuples(res); i++) {
		for (j = 0; j < nFields; j++)
			printf("%-15s", PQgetvalue(res, i, j));
		printf("\n");
	}

	PQclear(res);

	/* ferme le portail... nous ne cherchons pas s'il y a des erreurs... */
	res = PQexec(conn, "CLOSE myportal");
	PQclear(res);

	endTransaction(conn);

	/* ferme la connexion à la base et nettoie */
	PQfinish(conn);

	return 0;
}
