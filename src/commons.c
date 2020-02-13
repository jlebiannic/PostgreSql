/*
 * commons.c
 *
 *  Created on: 13 f�vr. 2020
 *      Author: jlebiannic
 */

#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"

void closeAndExit(PGconn *conn) {
	PQfinish(conn);
	exit(1);
}

static void setDefaultSchema(PGconn *conn) {
	PGresult *res;
	res = PQexec(conn, "SELECT pg_catalog.set_config('search_path', '\"Tdx\"', false)");
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
		PQclear(res);
		closeAndExit(conn);
	}
	PQclear(res);
}

PGconn* connect(const char *conninfo) {
	PGconn *conn;
	printf("connect\n");
	if (!conninfo) {
		conninfo = "dbname=postgres user=postgres password=admin";
	}

	/* Cr�e une connexion � la base de donn�es */
	conn = PQconnectdb(conninfo);

	/* V�rifier que la connexion au backend a �t� faite avec succ�s */
	if (PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
		closeAndExit(conn);
	}

	setDefaultSchema(conn);

	return conn;
}

void beginTransaction(PGconn *conn) {
	printf("begin\n");
	PGresult *res;
	/* D�marre un bloc de transaction */
	res = PQexec(conn, "BEGIN");
	//fprintf(stderr, "BEGIN, PQresultStatus(res): %d", (int) PQresultStatus(res));
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
		PQclear(res);
		closeAndExit(conn);
	}
	PQclear(res);
}

void endTransaction(PGconn *conn) {
	printf("end\n");
	PGresult *res;
	/* termine la transaction */
	res = PQexec(conn, "END");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "END command failed: %s", PQerrorMessage(conn));
		PQclear(res);
		closeAndExit(conn);
	}
	PQclear(res);
}

