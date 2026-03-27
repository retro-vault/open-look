/*
 * Declares private XView interfaces, types, and macros for sel attrs.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef	xview_selection_attributes_DEFINED
#define	xview_selection_attributes_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/attr.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PRIVATE #defines 
 */

/*
 *	Common requests a client may send to a selection-holder
 */
#define ATTR_PKG_SELN	ATTR_PKG_SELN_BASE

#define SELN_ATTR(type, n)	ATTR(ATTR_PKG_SELN, type, n)

#define SELN_ATTR_LIST(list_type, type, n)	\
	ATTR(ATTR_PKG_SELN, ATTR_LIST_INLINE(list_type, type), n)

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

/*
 * Public Enumerations 
 */

/*
 *	Attributes of selections
 * 	The numbering scheme has to match the scheme of Sunview selection_attributes.h
 */
#if 0
typedef enum	{
	/*
	 * Public Attributes 
	 */
	SELN_REQ_BYTESIZE	= (Attr_attribute)SELN_ATTR(ATTR_INT,		         1),
	SELN_REQ_COMMIT_PENDING_DELETE	
				= (Attr_attribute)SELN_ATTR(ATTR_NO_VALUE,	         65),
	SELN_REQ_CONTENTS_ASCII	= (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR,  2),

#ifdef OW_I18N
	SELN_REQ_CONTENTS_WCS   = (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_WCHAR, 202),
        SELN_REQ_CHARSIZE       = (Attr_attribute)SELN_ATTR(ATTR_INT,                   204),
	SELN_REQ_FIRST_WC	= (Attr_attribute)SELN_ATTR(ATTR_INT,		        205),
	SELN_REQ_LAST_WC	= (Attr_attribute)SELN_ATTR(ATTR_INT,		        206),
#endif /*OW_I18N*/

	SELN_REQ_CONTENTS_PIECES= (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR,  3),
	SELN_REQ_DELETE		= (Attr_attribute)SELN_ATTR(ATTR_NO_VALUE,	        66),
	SELN_REQ_END_REQUEST	= (Attr_attribute)SELN_ATTR(ATTR_NO_VALUE,	        253),
	SELN_REQ_FAILED		= (Attr_attribute)SELN_ATTR(ATTR_INT,		        255),
	SELN_REQ_FAKE_LEVEL	= (Attr_attribute)SELN_ATTR(ATTR_INT,		        98),
	SELN_REQ_FILE_NAME	= (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR,  9),
	SELN_REQ_FIRST		= (Attr_attribute)SELN_ATTR(ATTR_INT,		        4),
	SELN_REQ_FIRST_UNIT	= (Attr_attribute)SELN_ATTR(ATTR_INT,		        5),
	SELN_REQ_LAST		= (Attr_attribute)SELN_ATTR(ATTR_INT,		        6),
	SELN_REQ_LAST_UNIT	= (Attr_attribute)SELN_ATTR(ATTR_INT,		        7),
	SELN_REQ_LEVEL		= (Attr_attribute)SELN_ATTR(ATTR_INT,		        8),
	SELN_REQ_RESTORE	= (Attr_attribute)SELN_ATTR(ATTR_NO_VALUE,	        67),
	SELN_REQ_SET_LEVEL	= (Attr_attribute)SELN_ATTR(ATTR_INT,		        99),
	SELN_REQ_UNKNOWN	= (Attr_attribute)SELN_ATTR(ATTR_INT,		        254),
	SELN_REQ_YIELD		= (Attr_attribute)SELN_ATTR(ATTR_ENUM,		        97),
	/*
	 * Private Attributes 
	 */
#ifdef OW_I18N
        SELN_REQ_CONTENTS_CT    = (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR,  203),
#endif /*OW_I18N*/

	SELN_AGENT_INFO		= (Attr_attribute)SELN_ATTR(ATTR_OPAQUE,                100),
	SELN_REQ_FUNC_KEY_STATE	= (Attr_attribute)SELN_ATTR(ATTR_INT,		 	101),
	SELN_REQ_SELECTED_WINDOWS= (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_INT, 	102),
	SELN_REQ_CONTENTS_OBJECT= (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR, 	103),
	SELN_REQ_OBJECT_SIZE	= (Attr_attribute)SELN_ATTR(ATTR_INT, 			104),
	SELN_REQ_IS_READONLY	= (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       105),
	SELN_TRACE_ACQUIRE	= (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       193),
	SELN_TRACE_DONE		= (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       194),
	SELN_TRACE_DUMP		= (Attr_attribute)SELN_ATTR(ATTR_ENUM,		       200),
	SELN_TRACE_HOLD_FILE	= (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       195),
	SELN_TRACE_INFORM	= (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       196),
	SELN_TRACE_INQUIRE	= (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       197),
	SELN_TRACE_STOP		= (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       199),
	SELN_TRACE_YIELD	= (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       198)
}	Seln_attribute;
#else
typedef Attr_attribute Seln_attribute;
	/*
	 * Public Attributes 
	 */
#define SELN_REQ_BYTESIZE (Attr_attribute)SELN_ATTR(ATTR_INT,		         1)
#define	SELN_REQ_COMMIT_PENDING_DELETE (Attr_attribute)SELN_ATTR(ATTR_NO_VALUE,	         65)
#define SELN_REQ_CONTENTS_ASCII (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR,  2)

#ifdef OW_I18N
#define SELN_REQ_CONTENTS_WCS (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_WCHAR, 202)
#define SELN_REQ_CHARSIZE (Attr_attribute)SELN_ATTR(ATTR_INT,                   204)
#define SELN_REQ_FIRST_WC (Attr_attribute)SELN_ATTR(ATTR_INT,		        205)
#define SELN_REQ_LAST_WC (Attr_attribute)SELN_ATTR(ATTR_INT,		        206)
#endif /*OW_I18N*/

#define SELN_REQ_CONTENTS_PIECES (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR,  3)
#define SELN_REQ_DELETE (Attr_attribute)SELN_ATTR(ATTR_NO_VALUE,	        66)
#define SELN_REQ_END_REQUEST (Attr_attribute)SELN_ATTR(ATTR_NO_VALUE,	        253)
#define SELN_REQ_FAILED (Attr_attribute)SELN_ATTR(ATTR_INT,		        255)
#define SELN_REQ_FAKE_LEVEL (Attr_attribute)SELN_ATTR(ATTR_INT,		        98)
#define SELN_REQ_FILE_NAME (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR,  9)
#define SELN_REQ_FIRST (Attr_attribute)SELN_ATTR(ATTR_INT,		        4)
#define SELN_REQ_FIRST_UNIT (Attr_attribute)SELN_ATTR(ATTR_INT,		        5)
#define SELN_REQ_LAST (Attr_attribute)SELN_ATTR(ATTR_INT,		        6)
#define SELN_REQ_LAST_UNIT (Attr_attribute)SELN_ATTR(ATTR_INT,		        7)
#define SELN_REQ_LEVEL (Attr_attribute)SELN_ATTR(ATTR_INT,		        8)
#define SELN_REQ_RESTORE (Attr_attribute)SELN_ATTR(ATTR_NO_VALUE,	        67)
#define SELN_REQ_SET_LEVEL (Attr_attribute)SELN_ATTR(ATTR_INT,		        99)
#define SELN_REQ_UNKNOWN (Attr_attribute)SELN_ATTR(ATTR_INT,		        254)
#define SELN_REQ_YIELD (Attr_attribute)SELN_ATTR(ATTR_ENUM,		        97)
	/*
	 * Private Attributes 
	 */
#ifdef OW_I18N
#define SELN_REQ_CONTENTS_CT (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR,  203)
#endif /*OW_I18N*/

#define SELN_AGENT_INFO (Attr_attribute)SELN_ATTR(ATTR_OPAQUE,                100)
#define SELN_REQ_FUNC_KEY_STATE (Attr_attribute)SELN_ATTR(ATTR_INT,		 	101)
#define SELN_REQ_SELECTED_WINDOWS (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_INT, 	102)
#define SELN_REQ_CONTENTS_OBJECT (Attr_attribute)SELN_ATTR_LIST(ATTR_NULL, ATTR_CHAR, 	103)
#define SELN_REQ_OBJECT_SIZE (Attr_attribute)SELN_ATTR(ATTR_INT, 			104)
#define SELN_REQ_IS_READONLY (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       105)
#define SELN_TRACE_ACQUIRE (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       193)
#define SELN_TRACE_DONE (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       194)
#define SELN_TRACE_DUMP (Attr_attribute)SELN_ATTR(ATTR_ENUM,		       200)
#define SELN_TRACE_HOLD_FILE (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       195)
#define SELN_TRACE_INFORM (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       196)
#define SELN_TRACE_INQUIRE (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       197)
#define SELN_TRACE_STOP (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       199)
#define SELN_TRACE_YIELD (Attr_attribute)SELN_ATTR(ATTR_BOOLEAN,	       198)
#endif

/* Meta-levels available for use with SELN_REQ_FAKE/SET_LEVEL.
 *	SELN_LEVEL_LINE is "text line bounded by newline characters,
 *			    including only the terminating newline"
 */
typedef enum {
	SELN_LEVEL_FIRST	= 0x40000001,
	SELN_LEVEL_LINE		= 0x40000101,
	SELN_LEVEL_ALL		= 0x40008001,
	SELN_LEVEL_NEXT		= 0x4000F001,
	SELN_LEVEL_PREVIOUS	= 0x4000F002
}	Seln_level;

#endif /* ~xview_selection_attributes_DEFINED */
