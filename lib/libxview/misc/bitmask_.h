/*
 * Declares internal interfaces used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(BITMASK__H)
#define BITMASK__H

#include <xview/pkg.h>
#include <xview_private/bitmask.h>

Bitmask *xv_bitss_new_mask(int max_bits);
Bitmask *xv_bitss_copy_mask(Bitmask *m);
void xv_bitss_dispose_mask(Bitmask *m);
Bitmask *xv_bitss_unset_mask(register Bitmask *m, register int bit);
Bitmask *xv_bitss_set_mask(register Bitmask *m, register int bit);
unsigned int xv_bitss_get_mask(register Bitmask *m, register int bit);
int xv_bitss_cmp_mask(register Bitmask *m1, register Bitmask *m2);
Bitmask *xv_bitss_and_mask(Bitmask *m1, Bitmask *m2, Bitmask *m3); 
Bitmask *xv_bitss_or_mask(Bitmask *m1, Bitmask *m2, Bitmask *m3); 
Bitmask *xv_bitss_not_mask(Bitmask *m1, Bitmask *m2); 

#endif
