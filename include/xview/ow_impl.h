/*
 * Declares public XView interfaces, types, and attributes for ow impl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef	__openwin_impl_h 
#define	__openwin_impl_h 

/*
 * Package:     openwin
 * 
 * Module:	ow_impl.h
 * 
 * Description: defines internal data structures for managing openlook windows
 *
 */

#include <xview/xview.h>
#include <xview/openwin.h>
#include <xview/scrollbar.h>
#include <xview/sel_svc.h>
#include <xview/rect.h>

/*
 * Global Defines:
 */
 
/* macros to convert variable from/to public/private form */
#define OPENWIN_PRIVATE(win)  \
	XV_PRIVATE(Xv_openwin_info, Xv_openwin, win)
#define OPENWIN_PUBLIC(win)   XV_PUBLIC(win)

#define OPENWIN_REGULAR_VIEW_MARGIN	2
#define OPENWIN_VIEW_BORDER_WIDTH 2

#define OPENWIN_SPLIT_VERTICAL_MINIMUM  50
#define OPENWIN_SPLIT_HORIZONTAL_MINIMUM  50

#define OPENWIN_SCROLLBAR_LEFT 0
#define OPENWIN_SCROLLBAR_RIGHT 1

#define openwin_sb(view, direction)    \
   ((view)->sb[((direction) == SCROLLBAR_VERTICAL) ? 0 : 1])
   
#define openwin_set_sb(view, direction, sb) \
   openwin_sb((view), (direction)) = sb
   

/*
 * Typedefs:
 */
 
typedef struct	openwin_view_struct		Openwin_view_info;
typedef struct	openwin_info_struct		Xv_openwin_info;

struct openwin_view_struct {
	Xv_opaque	view;
	Scrollbar	sb[2]; /* 0 -> vertical 1 -> horizontal */
	Rect		enclosing_rect; /* full area the view takes up --
	               includes margins, borders and scrollabrs */
	
	int			right_edge; /* view against openwin's right edge */
	int			bottom_edge; /* view against bottom edge */
	Openwin_view_info	*next_view;
    Xv_openwin_info       *owin;
};

#define STATUS(ow, field)           ((ow)->status_bits.field)
#define STATUS_SET(ow, field)       STATUS(ow, field) = TRUE
#define STATUS_RESET(ow, field)     STATUS(ow, field) = FALSE
#define BOOLEAN_FIELD(field)        unsigned field : 1

struct openwin_info_struct {
   	Openwin		public_self;		/* Back pointer */
   	
   	Xv_pkg		*view_class;
	Openwin_view_info	*views;
	int		margin;
	Rect		cached_rect;
	Scrollbar	vsb_on_create;	/* cached scrollbar until view is */
	Scrollbar	hsb_on_create;	/* created */
	Attr_avlist	view_avlist; 	/* cached view avlist on create */
	Attr_avlist	view_end_avlist;
#ifdef SELECTABLE_VIEWS
	Seln_client	seln_client;		/* selection svc client id */
	Openwin_view_info	*seln_view;	/* selected view, if any */
#endif /* SELECTABLE_VIEWS */
	struct {
	    BOOLEAN_FIELD(auto_clear);
	    BOOLEAN_FIELD(adjust_vertical);
	    BOOLEAN_FIELD(adjust_horizontal);
	    BOOLEAN_FIELD(no_margin);
	    BOOLEAN_FIELD(created);
	    BOOLEAN_FIELD(show_borders);
	    BOOLEAN_FIELD(removing_scrollbars);
	    BOOLEAN_FIELD(mapped);
	    BOOLEAN_FIELD(left_scrollbars);
#ifndef NO_OPENWIN_PAINT_BG
	    BOOLEAN_FIELD(paint_bg);
#endif /* NO_OPENWIN_PAINT_BG */
	} status_bits;
	int		nbr_cols;		/* WIN_COLUMNS specified by client */
	int		nbr_rows;		/* WIN_ROWS specified by client */
	int		(*layout_proc)();
	void		(*split_init_proc)();
	void		(*split_destroy_proc)();
#ifndef NO_OPENWIN_PAINT_BG
	XColor		background;
#endif /* NO_OPENWIN_PAINT_BG */
};

/*
 * Global Variable Declarations:
 */
extern	Attr_attribute openwin_view_context_key;

#endif	/* __openwin_impl_h */
