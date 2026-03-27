/*
 * Declares private XView interfaces, types, and macros for hashfn.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(HASHFN__H)
#define HASHFN__H

#include <xview_private/hashfn.h>


HashTable *hashfn_new_table(int size, int (*hash_fn)(caddr_t), int (*cmp_fn)(caddr_t, caddr_t));
void hashfn_dispose_table(HashTable *h);
caddr_t hashfn_lookup(HashTable *h, caddr_t key);
caddr_t hashfn_install(HashTable *h, caddr_t key, caddr_t payload);
caddr_t hashfn_delete(HashTable *h, caddr_t key);
caddr_t hashfn_first_key(HashTable *h, caddr_t *payload);
caddr_t hashfn_next_key(HashTable *h, caddr_t *payload);

#endif
