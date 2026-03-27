/*
 * Declares private XView interfaces, types, and macros for sb impl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef	__scrollbar_impl_h 
#define	__scrollbar_impl_h 

/*
 * Module:	scrollbar_impl.h
 *
 * Level:	private
 *
 * Description: Declarations for data structures internal to the scrollbar 
 *
 */

/*
 * Include files: <only those required by THIS file>
 */
#include <xview_private/i18n_impl.h>
#ifdef OW_I18N
#include <xview/xv_i18n.h>
#endif /* OW_I18N */
#include <olgx/olgx.h>
#include <xview/xview.h>
#include <xview/font.h>
#include <xview/scrollbar.h>
#include <xview/openmenu.h>
#include <xview/rect.h>
#include <xview/notify.h>

/*
 * Global Defines:
 */
 
/* macros to convert variable from/to public/private form */
#define SCROLLBAR_PRIVATE(scrollbar)  \
	XV_PRIVATE(Xv_scrollbar_info, Xv_scrollbar, scrollbar)
#define SCROLLBAR_PUBLIC(scrollbar)   XV_PUBLIC(scrollbar)

#define SB_VERTICAL(sb) (sb->direction == SCROLLBAR_VERTICAL)

#define Max_offset(sb)	\
	((sb)->object_length - (sb)->view_length)

/* macro used when just the normalized elevator height is needed, and 
 * sb_normalize_rect would be doing too much work.
 */
#define SB_NORMALIZED_ELEVATOR_HEIGHT(sb) \
  ((sb->direction == SCROLLBAR_VERTICAL) ? \
   sb->elevator_rect.r_height : sb->elevator_rect.r_width)

/* uninitialized lengths */
#define	SCROLLBAR_DEFAULT_LENGTH 	0
#define SCROLLBAR_CABLE_GAP		2

/* time in msec between repeats of single line scroll */
#define SCROLLBAR_REPEAT_DELAY 			100
#define SCROLLBAR_REPEAT_PAGE_INTERVAL	        100
#define SCROLLBAR_REPEAT_LINE_INTERVAL	        1

#define SCROLLBAR_POSITION_UNCHANGED 		-1

#define SB_ELEVATOR_INNER_TOP 			 1
#define SB_ELEVATOR_INNER_LEFT 			 1

/* dimensions for small size scrollbar */
#define SCROLLBAR_SMALL_THICKNESS		17
#define	SB_SMALL_MARGIN				 2
#define	SB_SMALL_MARKER_HEIGHT			 5
#define SB_SMALL_ELEVATOR_BOX_HEIGHT	 	13
#define SB_SMALL_FLINE_IMAGE_WIDTH		 6
#define SB_SMALL_FLINE_IMAGE_HEIGHT		 4
#define SB_SMALL_LINE_IMAGE_LEFT_MARGIN		 1
#define SB_SMALL_LINE_IMAGE_TOP_MARGIN		 3
#define SB_REDUCED_SMALL_LINE_IMAGE_TOP_MARGIN	 2

/* dimensions for regular size scrollbar */
#define SCROLLBAR_MEDIUM_THICKNESS		19
#define	SB_MEDIUM_MARGIN			 2
#define	SB_MEDIUM_MARKER_HEIGHT			 6
#define SB_MEDIUM_ELEVATOR_BOX_HEIGHT	 	15
#define SB_MEDIUM_FLINE_IMAGE_WIDTH		 8
#define SB_MEDIUM_FLINE_IMAGE_HEIGHT		 5
#define SB_MEDIUM_LINE_IMAGE_LEFT_MARGIN	 2
#define SB_MEDIUM_LINE_IMAGE_TOP_MARGIN		 4
#define SB_REDUCED_MEDIUM_LINE_IMAGE_TOP_MARGIN	 2

/* dimensions for large size scrollbar */
#define SCROLLBAR_LARGE_THICKNESS		23
#define	SB_LARGE_MARGIN				 3
#define	SB_LARGE_MARKER_HEIGHT			 7
#define SB_LARGE_ELEVATOR_BOX_HEIGHT	 	17
#define SB_LARGE_FLINE_IMAGE_WIDTH		 8
#define SB_LARGE_FLINE_IMAGE_HEIGHT		 5
#define SB_LARGE_LINE_IMAGE_LEFT_MARGIN		 2
#define SB_LARGE_LINE_IMAGE_TOP_MARGIN		 5
#define SB_REDUCED_LARGE_LINE_IMAGE_TOP_MARGIN	 3

/* dimensions for extralarge size scrollbar */
#ifndef SVR4
#define SCROLLBAR_XLARGE_THICKNESS		29
#else /* SVR4 */
#define SCROLLBAR_XLARGE_THICKNESS		27
#endif /* SVR4 */
#define	SB_XLARGE_MARGIN			 4
#define	SB_XLARGE_MARKER_HEIGHT			 9
#define SB_XLARGE_ELEVATOR_BOX_HEIGHT	 	21
#define SB_XLARGE_FLINE_IMAGE_WIDTH		12
#define SB_XLARGE_FLINE_IMAGE_HEIGHT		 7
#define SB_XLARGE_LINE_IMAGE_LEFT_MARGIN	 3
#define SB_XLARGE_LINE_IMAGE_TOP_MARGIN		 7
#define SB_REDUCED_XLARGE_LINE_IMAGE_TOP_MARGIN	 5

typedef enum {
    SCROLLBAR_FULL_SIZE,
    SCROLLBAR_ABBREVIATED,
    SCROLLBAR_MINIMUM
} Scrollbar_size;

/*
 * Typedefs:
 */
 
typedef struct scrollbar			Xv_scrollbar_info;
 
struct scrollbar {
    Scrollbar		public_self;		  /* Back pointer */
    int			creating;

    Scrollbar_setting	direction;		/* Scrollbar's orientation */
    int			can_split;		
    int			inactive;		
    unsigned long	last_view_start;
    Menu		menu;
    Xv_opaque		managee;
    unsigned int	page_length;
    unsigned long	object_length;
    unsigned int	pixels_per_unit;
    unsigned int	view_length;
    unsigned long	view_start;
    void		(*compute_scroll_proc)(); /* Application supplied compute proc */
    int			(*normalize_proc)();	  /* Application supplied normalize proc */

    int		        jump_pointer;		/* boolean, adjust pointer? */
    int			multiclick_timeout;
    int                 drag_repaint_percent;   /* percent of dragging events*/
						/* to be passed on to */ 
						/* application as repaint */
						/* events */

    Event		transit_event;
    Scroll_motion	transit_motion;
    int			transit_occurred;
    struct timeval	last_select_time;  	/* timestamp of last ACTION_SELECT */

    Scrollbar_size	size;			/* full, abbreviated, or minimum */
    Frame_rescale_state	scale;
    Graphics_info       *ginfo;			/* OLGX graphics information */
    XID			window;                 /* the XID to paint into */
    Rect		elevator_rect;          /* Rectangle describing elevator location */
    int			elevator_state;		/* OLGX state */
    Rect		top_anchor_rect;	/* Geometry of the top anchor */
    int			top_anchor_inverted;    /* boolean */
    Rect		bottom_anchor_rect;	/* Geometry of the bottom anchor */
    int			bottom_anchor_inverted; /* boolean */
    int			cable_start;
    int			cable_height;
    int			painted;
    int			length;			

    int			last_pos;		/* Used for cutting down on excess */
    int			last_prop_pos;		/* painting of the scrollbar */
    int			last_prop_length;
    int			last_state;
    Scroll_motion	last_motion;		/* keep track of scrollbar motion */
};

/*
 * Global Variable Declarations:
 */
extern	Attr_attribute	sb_context_key;
extern	Attr_attribute	sb_split_view_menu_item_key;
extern	Attr_attribute	sb_join_view_menu_item_key;


#endif	/* __scrollbar_impl_h */
