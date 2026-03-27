/*
 * Declares private XView interfaces, types, and macros for ntfy ctbl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
typedef struct ntfy_cndtbl {
	NTFY_CLIENT		*client;
	NTFY_CONDITION		*condition;
	struct ntfy_cndtbl	*next;
} NTFY_CNDTBL;

#define NTFY_LAST_CND		9
