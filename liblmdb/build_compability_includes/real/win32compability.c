#if !defined(MDB_DEBUG) || !MDB_DEBUG
	char * mdb_dkey(void *key, char *buf) { return 0; }
#endif
