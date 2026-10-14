/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 */

#ifdef IDENT
#ident	"@(#)list.c	1.2 olvwm version 25 Nov 1992"
#endif

/*
 * Based on
#ident	"@(#)list.c	26.7	91/09/14 SMI"
 *
 */

#include <stdio.h>
#include <olgx/olgx.h>
#include "mem.h"
#include "list.h"

/***************************************************************************
* Local data
***************************************************************************/

#define NUMCELLS 511

typedef struct _cellBlock {
	struct _cellBlock *next;
	int n;
	List cells[NUMCELLS];
	} cellBlock, *pcellBlock;

static pcellBlock cellList = 0;
static List *freeList = 0;

/***************************************************************************
* Local functions
***************************************************************************/

/* local function
 *
 * pcellBlock mkCellBlock()
 * returns a new, minimally initialised cell block.  The block is not linked 
 * into the list of cell blocks.  The cells are not initialised.
 */
static pcellBlock
mkCellBlock(void)
{
	pcellBlock pcb;
	pcb =  MemNew(cellBlock);
	pcb->n = NUMCELLS;
	pcb->next = 0;
	return pcb;
}


/* local function
 *
 * void initCellBlock()
 * puts all the cells in the new cell block onto the free list
 */
static void
initCellBlock(pcellBlock pcb)
{
	int i;

	for (i=0; i<pcb->n; i++)
	{
		pcb->cells[i].next = freeList;
		pcb->cells[i].value = 0;
		freeList = &(pcb->cells[i]);
	}
}

/* local function
 *
 * List *allocCell()
 * allocates a cell from the free list; if none are available, allocates
 * a new cell block.
 */
static List *
allocCell(void)
{
	pcellBlock pcb;
	List *cell;

	if (freeList == 0)
	{
		pcb = mkCellBlock();
		initCellBlock(pcb);
		pcb->next = cellList;
		cellList = pcb;
	}
	cell = freeList;
	freeList = freeList->next;
	cell->next = 0;
	return cell;
}

/* local function
 *
 * void freeCell()
 * returns a cell from the free list
 */
static void
freeCell(List * cell)
{
	cell->next = freeList;
	cell->value = 0;
	freeList = cell;
}


#ifdef DEBUG

/* debugging function listPrint
 *
 * Intended to be called from a debugger.  Prints out the value of each list
 * item in hex.
 */
static void
listPrint(List * list)
{
    int count;

    for (count=0 ; list != NULL_LIST ; list = list->next, ++count)
	fprintf(stderr, "item %d: value = 0x%x\n", count, list->value);
}

#endif /* DEBUG */

/***************************************************************************
* Global functions
***************************************************************************/

/* global function
 *
 * void ListInit()
 * no parameters.
 *
 * Initialises the List package:  allocates a block of cells.
 */
void 
ListInit(void)
{
	cellList = mkCellBlock();
	initCellBlock(cellList);
}

/* global function
 *
 * void ListCons(void *val, List *next)
 *
 * Prepends a value to the head of a list.
 */
List *
ListCons(void * val, List * next)
{
	List *cell;

	cell = allocCell();
	cell->value = val;
	cell->next = next;
	return cell;
}

/* global function
 *
 * int ListCount(List *l)
 *
 * Returns the number of items in a list.
 */
int
ListCount(List * l)
{
	int len = 0;

	while (l != 0)
	{
		len++;
		l = l->next;
	}
	return len;
}


/* global function 
 * void ListDestroy(List *l)
 *
 * destroys all the cells in a list.  Leaves the values unaffected.
 */
void 
ListDestroy(List * l)
{
	List *last;

	while (l != 0)
	{
		last = l;
		l = l->next;
		freeCell(last);
	}
}


/* global function 
 * void ListDestroyCell(List **l)
 *
 * deletes one cell from a list; modifies the List* passed by
 * reference to point to the next cell.
 */
void 
ListDestroyCell(List ** l)
{
	List *cell;

	if ((cell = *l) != 0)
	{
		*l = cell->next;
		freeCell(cell);
	}
}

/* global function 
 * void ListApply(List *l,void *(*f)(),void*c)
 *
 * iteratively calls the function f with list values and the
 * closure c, until f returns a non-0 value 
 */
void *
ListApply(l,f,c)
List *l; 
void *(*f)();
void *c;
{
	List *cell;
	void *res;

	for (cell = l; cell != 0; cell = cell->next)
	{
		res = (*f)(cell->value,c);
		if (res != 0)
			return res;
	}
	return 0;
}

/* global function
 * void * ListEnum(List **l)
 *
 * returns the value of the cell pointed to by l; modifies l to point
 * to the next cell in the list.
 */
void *
ListEnum(List ** l)
{
	void *val;

	if (*l != 0)
	{
		val = (*l)->value;
		*l = (*l)->next;
		return val;
	}
	else
		return 0;
}

List *
ListDestroyCellByValue(void      * val, List      * l)
{
List  *cell, *last;

    for (cell = l, last = 0; cell; last = cell, cell = cell->next)
	if (cell->value == val)
	    break;
    if (!cell)
	return l;
    if (last) {
	last->next = cell->next;
	freeCell(cell);
    }
    else {
	l = l->next;
	freeCell(cell);
    }
    return l;
}

Bool
ListIsAMember(void	* val, List	* l)
{
List	*cell;

    for (cell = l; cell; cell = cell->next)
	if (cell->value == val)
	    return True;
    return False;
}

/*
 * ListReverse(List **l)
 *
 * reverses the order of the elements in a list.
 * this is done by creating a new list then destroying the old list.
 */

void 
ListReverse(List ** l)
{
    List *current;
    List *new = 0;

    /*
     * Create new list reversing order.
     */
    for (current = *l; current != NULL_LIST; current = current->next) {
        new = ListCons(current->value, new);
    }
	  
    /*
     * Destroy old list, reset new list.
     */
    ListDestroy(*l);

    *l = new;
}
