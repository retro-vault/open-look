/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef ev_impl_DEFINED
#define ev_impl_DEFINED

/*
 * Internal structures for entity_view implementation.
 *
 */

#				ifndef suntool_entity_view_DEFINED
#include <xview_private/ev.h>
#include <xview/window.h>
#				endif

/*
 * the purpose of struct ev_impl_line_data is to have a client_data,
 * separate from position for ft_set.
 *
 * The fields in struct ev_impl_line_seq are as follows:
 * pos		common to all finger tables, position of the finger.
 * considered	the last Es_index considered, relative to pos,
 *		in deciding what character starts the next line.
 * damaged	first Es_index, relative to pos, damaged by either
 *		insertion or deletion.
 * blit_down	temporary, only found in tmp_line_table, for telling
 *		the paint routine what to blit down.
 * blit_up	same as blit_down but for blitting up.
 */
struct ev_impl_line_data {
	Es_index		considered;
	Es_index		damaged;
	int			blit_down;
	int			blit_up;
};
typedef struct ev_impl_line_seq *Ev_impl_line_seq;
struct ev_impl_line_seq {
	Es_index		pos;
	Es_index		considered;
	Es_index		damaged;
	int			blit_down;
	int			blit_up;
};
/* flag values for Ev_impl_line_seq->flags */
#define	EV_LT_BLIT_DOWN		0x0000001

typedef struct ev_index_range {
	Es_index		first, last_plus_one;
} Ev_range;


typedef struct op_bdry_datum *	Op_bdry_handle;
struct op_bdry_datum {
	Es_index		pos;
	Ev_mark_object		info;
	unsigned		flags;
	opaque_t		more_info;
};

typedef struct ev_overlay_object {
	struct pixrect		*pr;
	int			 pix_op;
	short			 offset_x, offset_y;
	unsigned		 flags;
} Ev_overlay_object;
typedef Ev_overlay_object *	Ev_overlay_handle;
#define	EV_OVERLAY_RIGHT_MARGIN	0x00000001

#define	EI_OP_EV_OVERLAY	0x01000000

	/* op_bdry flags (in addition to EV_SEL_* in entity_view.h) */
#define EV_BDRY_END		EV_SEL_CLIENT_FLAG(0x1)
				/* ... otherwise _BEGIN */
#define	EV_BDRY_OVERLAY		EV_SEL_CLIENT_FLAG(0x2)
#define EV_BDRY_TYPE_ONLY	(EV_BDRY_END|EV_SEL_BASE_TYPE(0xFFFFFFFF))
#define EV_BDRY_EXACT_MATCH	(EV_BDRY_TYPE_ONLY|EV_SEL_PENDING_DELETE)

/*   The following struct is used to cache information about the coordinate
 * of a position in a view.  The definition of when the cache is
 * valid is encapsulated in the EV_CACHED_POS_INFO_IS_VALID macro below.
 */
typedef struct ev_pos_info {
	Es_index		 index_of_first, pos;
	int			 edit_number;
	int			 lt_index;
	struct pr_pos		 pr_pos;
} Ev_pos_info;
#define	EV_NOT_IN_VIEW_LT_INDEX	-1
#define	EV_NULL_DIM		-10000

/*   The following struct is used to cache information about the physical
 * lines that are visible in a view.  The definition of when the cache is
 * valid is encapsulated in the EV_CACHED_LINE_INFO_IS_VALID macro below.
 *
 *   It is also used to cache the last sought line number from
 * ev_position_for_physical_line().  If the next sought line number
 * is greater than the cached one and the cache is valid, then the
 * search may be started from the cached position.  In this usage,
 * line_count is ignored, and cache validity is only based on
 * EV_CHAIN_PRIVATE->edit_number.
 */
typedef struct ev_physical_line_info {
	Es_index		 index_of_first;
	int			 edit_number;
	int			 first_line_number;	/* 0-origin */
	int			 line_count;
} Ev_physical_line_info;

/*   Caret_pr_pos is set to EV_NULL_DIM when the caret is not displayed.
 * This field is required because the insert_pos may move BEFORE the caret is
 * taken down.
 */
typedef struct ev_private_data_object *Ev_pd_handle;
struct ev_private_data_object {
	struct pr_pos		 caret_pr_pos;
	short			 left_margin, right_margin;
	Ev_right_break		 right_break;
	Ev_pos_info		 cached_insert_info;
	Ev_physical_line_info	 cached_line_info;
	long unsigned		 state;
};
#ifdef lint
#define EV_PRIVATE(view_formal)	(Ev_pd_handle)(view_formal ? 0 : 0)
#else
#define EV_PRIVATE(view_formal)	((Ev_pd_handle) view_formal->private_data)
#endif

#define	EV_VIEW_FIRST(view_formal)	ft_position_for_index( \
		(view_formal)->line_table, 0)

	/* Bit flags for EV_PRIVATE(view)->state */
#define	EV_VS_INSERT_WAS_IN_VIEW	0x00000001
#define	EV_VS_INSERT_WAS_IN_VIEW_RECT	0x00000002
#define	EV_VS_DAMAGED_LT		0x00000004
#define	EV_VS_DELAY_UPDATE		0x00000008
#define	EV_VS_NO_REPAINT_TIL_EVENT	0x00000010
#define	EV_VS_SET_CLIPPING		0x00000020
#define EV_VS_BUFFERED_OUTPUT           0x00000040
#define	EV_VS_UNUSED			0xfffffff0

typedef struct ev_chain_private_data_object *Ev_chain_pd_handle;
struct ev_chain_private_data_object {
	Es_index		  insert_pos;
	Ev_mark_object		  selection[2];
	Ev_mark_object		  secondary[2];
	Ev_line_table		  op_bdry;
	int			  auto_scroll_by,
				  lower_context, upper_context;
	int			(*notify_proc)();
	int			  notify_level;
	int			  edit_number;
	int			  caret_is_ghost;
	int			  glyph_count;
	struct pixrect		 *caret_pr;
	struct pr_pos		  caret_hotpoint;
	struct pixrect		 *ghost_pr;
	struct pr_pos		  ghost_hotpoint;
	Ev_physical_line_info	  cache_pos_for_file_line;
#ifdef OW_I18N
	int			  updated;	/* contents update flag */
#endif
};
#ifdef lint
#define EV_CHAIN_PRIVATE(chain_formal)					\
	((Ev_chain_pd_handle)((chain_formal) ? 0 : 0 ))
#define	EV_CACHED_POS_INFO_IS_VALID(view_formal, pos_formal, cache_formal) \
	((view_formal) && (pos_formal) && (cache_formal))
#define	EV_CACHED_LINE_INFO_IS_VALID(view_formal)			\
	((view_formal))
#else
#define EV_CHAIN_PRIVATE(chain_formal)					\
	((Ev_chain_pd_handle) (chain_formal)->private_data)
#define	EV_CACHED_POS_INFO_IS_VALID(view_formal, pos_formal, cache_formal) \
    (	((cache_formal)->pos == (pos_formal)) &&			   \
	((cache_formal)->index_of_first == EV_VIEW_FIRST(view_formal)) &&  \
	((cache_formal)->edit_number ==	     				   \
	 EV_CHAIN_PRIVATE((view_formal)->view_chain)->edit_number)  )
#define	EV_CACHED_LINE_INFO_IS_VALID(view_formal)			\
    (	(EV_PRIVATE(view_formal)->cached_line_info.index_of_first ==	\
	 EV_VIEW_FIRST(view_formal)) &&					\
	(EV_PRIVATE(view_formal)->cached_line_info.edit_number ==	\
	 EV_CHAIN_PRIVATE((view_formal)->view_chain)->edit_number)  )
#endif

	/* break_action bits for ev_display_internal */
#define EV_QUIT			0x00000001
#define EV_Q_DORBREAK		0x00000002
#define EV_Q_LTMATCH		0x00000004

#define EV_BUFSIZE 200

struct range {
	int		ei_op;
	int		last_plus_one;
	int		next_i;
	unsigned	op_bdry_state;
};

/*
 * Struct for use by ev_process and friends.
 */
typedef struct _ev_process_object {
	Ev_handle		 view;
	Rect			 rect;
	struct ei_process_result result;
	struct pr_pos		 ei_xy;
	Es_buf_object		 esbuf;
	Es_buf_object		 whole_buf;
	int			 esbuf_status;
	Es_index		 last_plus_one;
	Es_index		 pos_for_line;
	unsigned		 flags;
}	Ev_process_object;
#define	EV_PROCESS_PROCESSED_BUF	0x0000001

typedef Ev_process_object *Ev_process_handle;

#define EV_INSERT_VISIBLE_IN_VIEW(_view)\
    ev_check_cached_pos_info(_view, EV_CHAIN_PRIVATE(_view->view_chain)->insert_pos, &EV_PRIVATE(_view)->cached_insert_info)

#endif
