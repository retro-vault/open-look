/*
 * Declares internal interfaces used by the XView base module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef _gen_impl_h_already_included
#define _gen_impl_h_already_included

#include <xview/generic.h>

#define GEN_PUBLIC(obj)		XV_PUBLIC(obj)
#define GEN_PRIVATE(obj)	XV_PRIVATE(Generic_info, Xv_generic_struct, obj)
#define HEAD(obj)		(GEN_PRIVATE(obj))->key_data

typedef struct _generic_node {
    struct _generic_node *next;
    Attr_attribute  key;
    Xv_opaque       data;
    void            (*copy_proc) ();
    void            (*remove_proc) ();
}Generic_node;

typedef	struct	{
    Xv_object		public_self;	/* back pointer to object */
    Xv_object		owner;		/* owner of object */

    Generic_node	*key_data;
    Xv_opaque		instance_qlist;
    char		*instance_name;
} Generic_info;

#endif /* _gen_impl_h_already_included */
