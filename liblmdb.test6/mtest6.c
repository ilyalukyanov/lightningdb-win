/* mtest6.c - memory-mapped database tester/toy */
/*
 * Copyright 2011 Howard Chu, Symas Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

/* Tests for DB splits and merges */
#define _XOPEN_SOURCE 500		/* srandom(), random() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#  include "win32compability.h"
#endif
#include "lmdb.h"

char dkbuf[1024];

int main(int argc,char * argv[])
{
	int i = 0, j = 0, rc;
	MDB_env *env;
	MDB_dbi dbi;
	MDB_val key, data;
	MDB_txn *txn;
	MDB_stat mst;
	MDB_cursor *cursor;
	int count;
	int *values;
	long kval;
	char *sval;

	srandom(time(NULL));

	rc = mdb_env_create(&env);
	rc = mdb_env_set_mapsize(env, 10485760);
	rc = mdb_env_set_maxdbs(env, 4);
	rc = mdb_env_open(env, "testdb", MDB_FIXEDMAP|MDB_NOSYNC, 0664);
	rc = mdb_txn_begin(env, NULL, 0, &txn);
	rc = mdb_open(txn, "id2", MDB_CREATE|MDB_INTEGERKEY, &dbi);
	rc = mdb_cursor_open(txn, dbi, &cursor);
	rc = mdb_stat(txn, dbi, &mst);

	sval = calloc(1, mst.ms_psize / 4);
	key.mv_size = sizeof(long);
	key.mv_data = &kval;
	data.mv_size = mst.ms_psize / 4 - 30;
	data.mv_data = sval;

	printf("Adding 12 values, should yield 3 splits\n");
	for (i=0;i<12;i++) {
		kval = i*5;
		sprintf(sval, "%08x", kval);
		rc = mdb_cursor_put(cursor, &key, &data, MDB_NOOVERWRITE);
	}
	printf("Adding 12 more values, should yield 3 splits\n");
	for (i=0;i<12;i++) {
		kval = i*5+4;
		sprintf(sval, "%08x", kval);
		rc = mdb_cursor_put(cursor, &key, &data, MDB_NOOVERWRITE);
	}
	printf("Adding 12 more values, should yield 3 splits\n");
	for (i=0;i<12;i++) {
		kval = i*5+1;
		sprintf(sval, "%08x", kval);
		rc = mdb_cursor_put(cursor, &key, &data, MDB_NOOVERWRITE);
	}
	rc = mdb_cursor_get(cursor, &key, &data, MDB_FIRST);

	do {
		printf("key: %p %s, data: %p %.*s\n",
			key.mv_data,  mdb_dkey(&key, dkbuf),
			data.mv_data, (int) data.mv_size, (char *) data.mv_data);
	} while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0);
	mdb_cursor_close(cursor);
	mdb_txn_commit(txn);

#if 0
	j=0;

	for (i= count - 1; i > -1; i-= (random()%5)) {
		j++;
		txn=NULL;
		rc = mdb_txn_begin(env, NULL, 0, &txn);
		sprintf(kval, "%03x", values[i & ~0x0f]);
		sprintf(sval, "%03x %d foo bar", values[i], values[i]);
		key.mv_size = sizeof(int);
		key.mv_data = kval;
		data.mv_size = sizeof(sval);
		data.mv_data = sval;
		rc = mdb_del(txn, dbi, &key, &data);
		if (rc) {
			j--;
			mdb_txn_abort(txn);
		} else {
			rc = mdb_txn_commit(txn);
		}
	}
	free(values);
	printf("Deleted %d values\n", j);

	rc = mdb_env_stat(env, &mst);
	rc = mdb_txn_begin(env, NULL, 1, &txn);
	rc = mdb_cursor_open(txn, dbi, &cursor);
	printf("Cursor next\n");
	while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
		printf("key: %.*s, data: %.*s\n",
			(int) key.mv_size,  (char *) key.mv_data,
			(int) data.mv_size, (char *) data.mv_data);
	}
	printf("Cursor prev\n");
	while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_PREV)) == 0) {
		printf("key: %.*s, data: %.*s\n",
			(int) key.mv_size,  (char *) key.mv_data,
			(int) data.mv_size, (char *) data.mv_data);
	}
	mdb_cursor_close(cursor);
	mdb_close(txn, dbi);

	mdb_txn_abort(txn);
#endif
	mdb_env_close(env);

	return 0;
}
