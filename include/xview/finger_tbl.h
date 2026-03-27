/*
 * Declares public XView interfaces, types, and attributes for finger
 * tbl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef finger_table_DEFINED
#define finger_table_DEFINED

/*
 * This is the programmatic interface to the finger table abstraction.
 *
 *     A finger_table is an array of records: .seq[ 0 .. .last_plus_one-1 ].
 *     Each record has an Es_index as its first field, and is
 * .size_of_element bytes long.
 *     The array is ordered by the values of the Es_index field, with the
 * smallest value occurring in the first record of the array.  The
 * implementation supports and requires the ordering.
 *     .last_bounding_index records the last result from ft_bounding_index;
 * it is a cache private to ft_bounding_index used to speed up the common case
 * of a succession of searches for similar positions.
 *     .first_infinity records the first occurrence of an ES_INFINITY;
 * it is a cache private to ft_shift_{up,out} used to speed up a common case
 * of shifts in large tables with many ES_INFINITY's at the end.
 */

#					ifndef suntool_entity_stream_DEFINED
#include <xview_private/es.h>
#					endif

typedef struct ft_table {
	int	 	last_plus_one;
	unsigned	sizeof_element;
	int	 	last_bounding_index;
	int	 	first_infinity;
	Es_index	*seq;
} Ft_table_object;
typedef Ft_table_object	*Ft_table;
/* The following two typedef's are for 3.0 compatibility. */
typedef struct ft_table		ft_object;
typedef struct ft_table *	ft_handle;
#define FT_NULL			((ft_handle)0)

#define FT_CLIENT_CREATE(num_elements, client_type)			\
	ft_create((num_elements), sizeof(client_type)-sizeof(Es_index))
#define FT_SET_ALL(finger_table, to, client_data)			\
	ft_set((finger_table), 0, (finger_table).last_plus_one,		\
		(to), (client_data))
#define FT_CLEAR_ALL(finger_table)					\
	FT_SET_ALL((finger_table), ES_INFINITY, (char *)0)

#ifdef lint
#define	FT_ADDR(_handle, _index, _addr_delta)				\
	(Es_index *)( (_handle) && (_index) && (_addr_delta) ? 0 : 0 )
#define	FT_NEXT_ADDR(_addr, _addr_delta)				\
	(Es_index *)( (_addr) && (_addr_delta) ? 0 : 0 )
#define	FT_PREV_ADDR(_addr, _addr_delta)				\
	(Es_index *)( (_addr) && (_addr_delta) ? 0 : 0 )
#else
#define	FT_ADDR(_handle, _index, _addr_delta)				\
	(Es_index *)(							\
	    ((char *)((_handle)->seq)) + ((_index)*(_addr_delta)) )
#define	FT_NEXT_ADDR(_addr, _addr_delta)				\
	(Es_index *)( (char *)(_addr) + (_addr_delta) )
#define	FT_PREV_ADDR(_addr, _addr_delta)				\
	(Es_index *)( (char *)(_addr) - (_addr_delta) )
#endif

#define	FT_ADDRESS(_handle, _index)					\
	FT_ADDR((_handle), (_index), (_handle)->sizeof_element)
#define	FT_NEXT_ADDRESS(_handle, _addr)					\
	FT_NEXT_ADDR((_addr), (_handle)->sizeof_element )
#define	FT_PREV_ADDRESS(_handle, _addr)					\
	FT_PREV_ADDR((_addr), (_handle)->sizeof_element )

#endif
