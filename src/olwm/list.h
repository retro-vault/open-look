/*
 * list.h: declarations and shared types for the list module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_LIST_H
#define _OLWM_LIST_H

typedef struct _List {
	void *value;
	struct _List *next;
	} List;

#define NULL_LIST ((List *)0)

/* ListInit: Initialize list. */
extern void ListInit();
	/* initialises the List package; should be called only once 
	 * at startup.
	 */

/* ListCons: Process list cons. */
extern List *ListCons();	/* void *val, List *next */

/* ListCount: Process list count. */
extern int ListCount();		/* List *l */
	/* returns number of items in a list 
	 */

/* ListDestroy: Destroy list. */
extern void ListDestroy();	/* List *l */
	/* destroys all list cells in a list; does not affect list values 
	 */

/* ListDestroyCell: Destroy list cell. */
extern void ListDestroyCell();	/* List **l */
	/* deletes one cell from a list; modifies the List* passed by
	 * reference to point to the next cell.
	 */

/* ListApply: Execute list. */
extern void *ListApply();	/* List *l, (void *)(*f)(), void *c */
	/* iteratively calls the function f with list values and the
	 * closure, until it returns a non-NULL value.  Returns this non-Null
	 * value, or NULL if hte end of the list is reached.
	 */

/* ListEnum: Process list enum. */
extern void *ListEnum();	/* List **l */
	/* used for enumerating a list in a loop.  Returns the value of the
	 * cell, and modifies the cell pointer to point to the next cell.
	 */

#endif /* _OLWM_LIST_H */
