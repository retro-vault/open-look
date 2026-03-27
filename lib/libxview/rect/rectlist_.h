/*
 * Declares internal interfaces used by the XView rect module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(RECTLIST__H)
#define RECTLIST__H

#include <xview/pkg.h>
#include <xview/rect.h>
#include <xview/rectlist.h>

unsigned rl_includespoint(register Rectlist *rl, coord x, coord y);
void rl_intersection(register struct rectlist *rl1, register struct rectlist *rl2, register struct rectlist *rl);
void rl_sort(register struct rectlist *rl1, register struct rectlist *rl, int sortorder);
void rl_union(register struct rectlist *rl1, register struct rectlist *rl2, register struct rectlist *rl);
void rl_difference(register struct rectlist *rl1, register struct rectlist *rl2, register struct rectlist *rl);
unsigned rl_empty(register struct rectlist *rl);
unsigned rl_equal(register struct rectlist *rl1, register struct rectlist *rl2);
unsigned rl_equalrect(register struct rect *r, register struct rectlist *rl);
unsigned rl_boundintersectsrect(register struct rect *r, register struct rectlist *rl);
unsigned rl_rectintersects(register struct rect *r, register struct rectlist *rl);
void rl_rectintersection(register struct rect *r, register struct rectlist *rl1, register struct rectlist *rl);
void rl_rectunion(register struct rect *r, register struct rectlist *rl1, register struct rectlist *rl);
void rl_rectdifference(register struct rect *r, register struct rectlist *rl1, register struct rectlist *rl);
void rl_initwithrect(register struct rect *r, struct rectlist *rl);
void rl_copy(register struct rectlist *rl1, register struct rectlist *rl);
void rl_free(register struct rectlist *rl);
void rl_coalesce(register struct rectlist *rl);
void rl_normalize(register struct rectlist *rl);
void rl_print(struct rectlist *rl, char *tag);

#endif
