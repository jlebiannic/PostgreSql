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

static void exit_nicely(PGconn *conn) {
	PQfinish(conn);
	exit(1);
}

int test(int argc, char **argv) {

	const char *conninfo;
	PGconn *conn;
	PGresult *res;
	int nFields;
	int i, j;

	if (argc > 1)
		conninfo = argv[1];
	else
		conninfo = "dbname=postgres user=postgres password=admin";

	/* Cr�e une connexion � la base de donn�es */
	conn = PQconnectdb(conninfo);

	/* V�rifier que la connexion au backend a �t� faite avec succ�s */
	if (PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
		exit_nicely(conn);
	}

	res = PQexec(conn, "SELECT pg_catalog.set_config('search_path', '\"Tdx\"', false)");
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);
	}

	/*
	 * Il faut lib�rer PGresult avec PQclear d�s que l'on en a plus besoin pour
	 * �viter les fuites de m�moire.
	 */
	PQclear(res);

	/* D�marre un bloc de transaction */
	res = PQexec(conn, "BEGIN");
	//fprintf(stderr, "BEGIN, PQresultStatus(res): %d", (int) PQresultStatus(res));
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);

	/*
	 * R�cup�re les lignes de pg_database, catalogue syst�me des bases de
	 * donn�es
	 */
	res = PQexec(conn, "DECLARE myportal CURSOR FOR select * from data");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);

	res = PQexec(conn, "FETCH ALL in myportal");
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);
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

	/* termine la transaction */
	res = PQexec(conn, "END");
	PQclear(res);

	/* ferme la connexion � la base et nettoie */
	PQfinish(conn);

	return 0;
}
