/*
 * Declares public XView interfaces, types, and attributes for bitmask.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef bitmask_h
#define bitmask_h

typedef struct bm_ {
    unsigned int *bm_mask;
    int bm_max_bits;
    int bm_mask_size;
} Bitmask;

#ifndef BITSPERBYTE
#define BITSPERBYTE 8
#endif

#endif /* bitmask_h */
