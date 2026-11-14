
#ifndef lint
static const char sccsid[] = "@(#)assoc.c 1.1 93/09/23";
#endif


/*  Copyright (c) 1987-1992 Sun Microsystems, Inc.
 *  All Rights Reserved.
 *
 *  Sun considers its source code as an unpublished, proprietary
 *  trade secret, and it is available only under strict license
 *  provisions.  This copyright notice is placed here only to protect
 *  Sun in the event the source is deemed a published work.  Dissassembly,
 *  decompilation, or other means of reducing the object code to human
 *  readable form is prohibited by the license agreement under which
 *  this code is provided to the user or company in possession of this
 *  copy.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the
 *  Government is subject to restrictions as set forth in subparagraph
 *  (c)(1)(ii) of the Rights in Technical Data and Computer Software
 *  clause at DFARS 52.227-7013 and in similar clauses in the FAR and
 *  NASA FAR Supplement.
 */


#include <stdio.h>


/*
 * Setup for a cheap-o assoication table.
 */
struct fm_assoc {
    void *		id;
    void *		val;
    struct fm_assoc *	prev;
    struct fm_assoc *	next;
};

static struct fm_assoc *fm_assoc_list = NULL;

void
fm_associate_value( void *id, void *val )
{
    struct fm_assoc *assoc
	= (struct fm_assoc *) malloc( sizeof(struct fm_assoc) );

    assoc->id = id;
    assoc->val = val;
    assoc->prev = NULL;
    assoc->next = fm_assoc_list;
    if ( assoc->next )
	assoc->next->prev = assoc;
    fm_assoc_list = assoc;
}


void *
fm_value_from_id( void *id )
{
    struct fm_assoc *assoc = fm_assoc_list;

    while ( assoc ) {
	if ( assoc->id == id )
	    return assoc->val;
	assoc = assoc->next;
    }
    return NULL;
}



/*
 * Remove association from list.
 */
void
fm_disassociate_value( void *val )
{
    struct fm_assoc *assoc = fm_assoc_list;

    while ( assoc ) {
	if ( assoc->val == val ) {
	    if ( assoc->next )
		assoc->next->prev = assoc->prev;
	    if ( assoc->prev )
		assoc->prev->next = assoc->next;
	    if ( assoc == fm_assoc_list )
		fm_assoc_list = assoc->next;
	    free( assoc );
	    return;
	}
	assoc = assoc->next;
    }
}
