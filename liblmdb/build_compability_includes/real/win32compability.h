#pragma once

#ifdef _WIN32
#	include <stdint.h>
#	include <basetsd.h>

#	define LITTLE_ENDIAN 1234
#	define BIG_ENDIAN 4321
#	define BYTE_ORDER LITTLE_ENDIAN

	typedef int pid_t;
	typedef SSIZE_T ssize_t;
	typedef int mode_t;

#	define SEM_FAILED NULL

#	define srandom srand
#	define random rand

#	if MDB_DEBUG
		static int mdb_debug;
		static size_t mdb_debug_start;
#	endif
#endif