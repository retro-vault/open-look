/*
 * Declares internal interfaces used by the XView menu module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef xview_image_DEFINED
#define xview_image_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/base.h>
#include <xview/attrol.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines
 */

#define	IMAGE_NULL			((Image)0)

/*
 * PRIVATE #defines 
 */

#define	IMAGE_ATTR(type, ordinal)	ATTR(ATTR_PKG_IMAGE, type, ordinal)
#define	IMAGE_ATTRIBUTE_LIST		ATTR_LIST

/*
 * Fake types -- This should be resolved someday 
 */
#define ATTR_IMAGE_PTR			ATTR_OPAQUE
#define ATTR_FONT_PTR			ATTR_OPAQUE
#define ATTR_PIXRECT_INT_INT		ATTR_INT_TRIPLE

/*
 * Reserved for future use 
 */
#define	IMAGE_ATTR_UNUSED_FIRST		 0
#define	IMAGE_ATTR_UNUSED_LAST		31

/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */

typedef	Xv_opaque 		Image;

typedef enum {
	/*
	 * Private Attributes 
	 */
	IMAGE_ACTIVE		= IMAGE_ATTR(ATTR_NO_VALUE,		 32), 
	IMAGE_CENTER		= IMAGE_ATTR(ATTR_BOOLEAN, 		133),
	IMAGE_ERROR_PROC	= IMAGE_ATTR(ATTR_FUNCTION_PTR,		 49),
	IMAGE_GINFO		= IMAGE_ATTR(ATTR_OPAQUE,		 34),
	IMAGE_HEIGHT		= IMAGE_ATTR(ATTR_INT,			 35),
	IMAGE_INACTIVE		= IMAGE_ATTR(ATTR_NO_VALUE,		 36), 
	IMAGE_INVERT		= IMAGE_ATTR(ATTR_BOOLEAN,		 37), 
	IMAGE_LEFT_PIXRECT	= IMAGE_ATTR(ATTR_PIXRECT_PTR,		 38), 
	IMAGE_PIXRECT		= IMAGE_ATTR(ATTR_PIXRECT_PTR,		 41), 
	IMAGE_REGION		= IMAGE_ATTR(ATTR_PIXRECT_INT_INT,	 50), 
	IMAGE_RELEASE		= IMAGE_ATTR(ATTR_NO_VALUE,		 42), 
	IMAGE_RELEASE_STRING	= IMAGE_ATTR(ATTR_NO_VALUE,		 43), 
	IMAGE_RELEASE_PR	= IMAGE_ATTR(ATTR_NO_VALUE,		 44), 
	IMAGE_RENDER_STRING 	= IMAGE_ATTR(ATTR_BOOLEAN,		 52),
	IMAGE_TITLE		= IMAGE_ATTR(ATTR_BOOLEAN,		 54),
	IMAGE_3D		= IMAGE_ATTR(ATTR_BOOLEAN,		 39),
	IMAGE_WIDTH		= IMAGE_ATTR(ATTR_INT,			 48),
} Image_attribute;

/*
 ***********************************************************************
 *			Globals
 ***********************************************************************
 */

#endif /* ~xview_image_DEFINED */
