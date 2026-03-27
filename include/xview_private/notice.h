/*
 * Declares private XView interfaces, types, and macros for notice.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef xview_notice_DEFINED
#define xview_notice_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/xv_c_types.h>
#include <xview/attrol.h>
#include <xview/generic.h>
#include <xview/window.h>

/*
 ***********************************************************************
 *			Definitions and Macros	
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */
#define NOTICE			&xv_notice_pkg

/*
 * the following constant, NOTICE_FAILED is returned if notice_prompt() 
 * failed for an unspecified reason.
 */
#define NOTICE_YES			 1
#define NOTICE_NO			 0
#define NOTICE_FAILED			-1
#define NOTICE_TRIGGERED		-2

/*
 * PRIVATE #defines 
 */

#define NOTICE_ATTR(type, ordinal)	ATTR(ATTR_PKG_NOTICE, type, ordinal)
#define NOTICE_ATTR_LIST(ltype, type, ordinal) \
	NOTICE_ATTR(ATTR_LIST_INLINE((ltype), (type)), (ordinal))
#if defined(__x86_64__) || defined(__ia64__) || defined(_XV_API_BROKEN_64BIT) || defined(__amd64__)
#define NOTICE_BUTTON_VALUE_PAIR	ATTR_OPAQUE_PAIR
#else
#define NOTICE_BUTTON_VALUE_PAIR	ATTR_INT_PAIR
#endif


/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */

typedef Xv_opaque	Xv_Notice;
typedef Xv_opaque	Xv_notice;

typedef enum {
	/*
	 * Public attributes 
	 */
	NOTICE_BUTTON		= NOTICE_ATTR(NOTICE_BUTTON_VALUE_PAIR,	 1),
	NOTICE_BUTTON_NO	= NOTICE_ATTR(ATTR_STRING,		 5),
	NOTICE_BUTTON_YES	= NOTICE_ATTR(ATTR_STRING,		10),
	NOTICE_FOCUS_XY		= NOTICE_ATTR(ATTR_XY,			15),
	NOTICE_FONT		= NOTICE_ATTR(ATTR_PIXFONT_PTR,		20),
	NOTICE_MESSAGE_STRINGS	
			= NOTICE_ATTR_LIST(ATTR_NULL, ATTR_STRING,	25),
	NOTICE_MESSAGE_STRINGS_ARRAY_PTR = NOTICE_ATTR(ATTR_STRING,	30),
	NOTICE_NO_BEEPING	= NOTICE_ATTR(ATTR_BOOLEAN,	 	35),
	NOTICE_TRIGGER		= NOTICE_ATTR(ATTR_INT,			40),
	NOTICE_MESSAGE_STRING   = NOTICE_ATTR(ATTR_STRING,              45),
#ifdef	OW_I18N
	NOTICE_MESSAGE_STRINGS_WCS	
			= NOTICE_ATTR_LIST(ATTR_NULL, ATTR_WSTRING,	50),
	NOTICE_MESSAGE_STRINGS_ARRAY_PTR_WCS
			= NOTICE_ATTR(ATTR_WSTRING,			55),
	NOTICE_MESSAGE_STRING_WCS
				= NOTICE_ATTR(ATTR_WSTRING,		60),
	NOTICE_BUTTON_WCS	= NOTICE_ATTR(NOTICE_BUTTON_VALUE_PAIR,	65),
	NOTICE_BUTTON_NO_WCS	= NOTICE_ATTR(ATTR_WSTRING,		70),
	NOTICE_BUTTON_YES_WCS	= NOTICE_ATTR(ATTR_WSTRING,		75),
#endif
	NOTICE_LOCK_SCREEN	= NOTICE_ATTR(ATTR_BOOLEAN,		80),
	NOTICE_TRIGGER_EVENT	= NOTICE_ATTR(ATTR_INT,			85),
	NOTICE_STATUS		= NOTICE_ATTR(ATTR_OPAQUE,		95),
	NOTICE_EVENT_PROC	= NOTICE_ATTR(ATTR_FUNCTION_PTR,	100),
	NOTICE_BUSY_FRAMES = NOTICE_ATTR_LIST(ATTR_NULL, ATTR_OPAQUE,	105),
	NOTICE_BLOCK_THREAD	= NOTICE_ATTR(ATTR_BOOLEAN,		110)
} Notice_attribute;


/*
 * Notice public struct
 */
typedef struct  {
    Xv_generic_struct	parent_data;
    Xv_opaque		private_data;
} Xv_notice_struct;

#include <xview/macros.h>

#define notice_prompt(...) \
    MACRO_DEF2(_notice_prompt, Xv_Window, Event*, __VA_ARGS__)

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * Public Functions 
 */
EXTERN_FUNCTION (int		_notice_prompt, (Xv_Window window,
					Event *return_event,
					DOTDOTDOT));

extern Xv_pkg		xv_notice_pkg;

#endif /* ~xview_notice_DEFINED */
