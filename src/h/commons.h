/*
 * commons.h
 *
 *  Created on: 13 févr. 2020
 *      Author: jlebiannic
 */

#ifndef H_COMMONS_H_
#define H_COMMONS_H_

void closeAndExit(PGconn *conn);
PGconn* connect(const char *conninfo);
void beginTransaction(PGconn *conn);
void endTransaction(PGconn *conn);

#endif /* H_COMMONS_H_ */
