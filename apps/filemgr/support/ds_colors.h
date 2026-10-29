/*
 * "@(#)ds_colors.h	3.1 04/03/92"
 * Copyright (c) 1990 Sun Microsystems, Inc.
 */

#ifndef	_ds_colors_h
#define	_ds_colors_h

#include <limits.h>
#include <xview/xview.h>
#include <xview/cms.h>

/*
 * Max colors of the user can allocate with the color text file.
 */
#define MAX_DS_COLORS     240

/*
 * Define null cms index entry as unsigned max long, since we cannot use
 * zero as an index.
 */
#define DS_NULL_CMS_INDEX	ULONG_MAX

/*
 * Property name in the server.
 */
#define COLOR_PROP_NAME   "_SUN_DESKSET_COLORS"

/*
 * Function Declarations
 */
extern int		ds_colors_init();
extern Cms 		ds_cms_create();
extern int 		ds_alloc_colors();
extern unsigned long 	ds_cms_index();
extern unsigned long	ds_x_index();
extern unsigned long	ds_x_pixel();
extern int 		ds_set_colormap();

#endif
