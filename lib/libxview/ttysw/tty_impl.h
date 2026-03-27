/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef _xview_private_ttysw_impl_h_already_included
#define _xview_private_ttysw_impl_h_already_included

/*
 * A tty subwindow is a subwindow type that is used to provide a
 * terminal emulation for teletype based programs.
 */

#include <xview_private/portable.h>	/* tty and pty configuration info */

#ifdef	XV_USE_TERMIOS
#include <termios.h>		/* for POSIX-style tty state structure */
#else
#include <sys/ioctl.h>		/* for BSD-style tty state structures */
#endif

#include <xview/tty.h>
#include <xview/sel_svc.h>
#include <xview_private/i18n_impl.h>

#define TTY_PRIVATE(_t)      XV_PRIVATE(Ttysw, Xv_tty, _t)
#define TTY_PUBLIC(_tty_folio)     XV_PUBLIC(_tty_folio)

#define TTY_VIEW_PRIVATE(_t)     	 XV_PRIVATE(Ttysw_view_object, Xv_tty_view, _t)
#define TTY_VIEW_PUBLIC(_tty_view)	 XV_PUBLIC(_tty_view)

#define IS_TTY(_t) \
	(((Xv_base *)(_t))->pkg == TTY)
	
#define IS_TTY_VIEW(_t) \
	(((Xv_base *)(_t))->pkg == TTY_VIEW)
		
/* BUG: This could be made cleaner.  See 
 * TERMSW_FOLIO_FROM_TERMSW_VIEW_HANDLE in termsw_impl.h 
 */
#define TTY_VIEW_HANDLE_FROM_TTY_FOLIO(_tty_folio_private) \
	 ((Ttysw_view_handle)(((Ttysw_folio)_tty_folio_private)->view)) 
 	     
#define TTY_FOLIO_FROM_TTY_VIEW_HANDLE(_tty_view_private) \
	 ((Ttysw_folio)(((Ttysw_view_handle)_tty_view_private)->folio))
	 
#define TTY_FOLIO_FROM_TTY_VIEW(_tty_view_public) 	\
	 ((Ttysw_folio)					\
	  (((Ttysw_view_handle) TTY_VIEW_PRIVATE(_tty_view_public))->folio))

#define TTY_FROM_TTY_VIEW(_tty_view_public) 		\
	((Tty) TTY_PUBLIC(TTY_FOLIO_FROM_TTY_VIEW(_tty_view_public)))

/*
 * These are the data structures internal to the tty subwindow
 * implementation.  They are considered private to the implementation.
 */

struct cbuf {
    CHAR               *cb_rbp;    /* read pointer */
    CHAR               *cb_wbp;    /* write pointer */
    CHAR               *cb_ebp;    /* end of buffer */
    CHAR                cb_buf[2048];
};

struct input_cbuf {
    CHAR               *cb_rbp;    /* read pointer */
    CHAR               *cb_wbp;    /* write pointer */
    CHAR               *cb_ebp;    /* end of buffer */
    CHAR                cb_buf[8192];
};

struct keymaptab {
    int                 kmt_key;
    int                 kmt_output;
    char               *kmt_to;
};

struct textselpos {
    int			tsp_row;
    int			tsp_col;
#ifdef  OW_I18N
    int			tsp_charpos;
#endif
};

struct ttyselection {
    int                 sel_made;  /* a selection has been made */
    int                 sel_null;  /* the selection is null */
    int                 sel_level; /* see below */
    int                 sel_anchor;/* -1 = left, 0 = none, 1 = right */
    struct textselpos   sel_begin; /* beginning of selection */
    struct textselpos   sel_end;   /* end of selection */
    struct timeval      sel_time;  /* time selection was made */
    Seln_rank		sel_rank;  /* type of selection. primary or secondary */
    int			dehilite_op;  /* Operation for taking down selection */
};

/* selection levels */
#define	SEL_CHAR	0
#define	SEL_WORD	1
#define	SEL_LINE	2
#define	SEL_PARA	3
#define	SEL_MAX		3

extern struct ttyselection	null_ttyselection;

enum ttysw_hdrstate { HS_BEGIN, HS_HEADER, HS_ICON, HS_ICONFILE, HS_FLUSH };

typedef struct ttysubwindow {
    Tty			public_self;		/* Back pointer to the object*/
    struct ttysw_view_object			/* View window */
    			*view;			/* (Pure tty has only one view) */
    Tty_view		current_view_public;	/* This keep trace of the view become ttysw */			
    unsigned		ttysw_flags;
    /* common */
    int                 ttysw_opt;		/* option mask; see ttysw.h */
    struct input_cbuf   ttysw_ibuf;		/* input buffer */
    struct cbuf         ttysw_obuf;		/* output buffer */
    /* pty and subprocess */
    int                 ttysw_pty;	/* master (pty) file descriptor */
    int                 ttysw_tty;	/* slave (tty) file descriptor */
    char		tty_name[20];	/* slave (tty) file name */
    int                 ttysw_ttyslot;		/* ttyslot in utmp for tty */
    /* saved tty mode information: see access functions below */
#   ifdef XV_USE_TERMIOS
    struct termios	termios;
#   else /* XV_USE_TERMIOS */
    struct sgttyb	sgttyb;
    struct tchars	tchars;
    struct ltchars	ltchars;
#   endif /* XV_USE_TERMIOS */
    /*
     * The next two fields record the current pty remote mode state and the
     * remote mode state to which it should be set before processing the next
     * input character.
     */
    int			remote;
    int			pending_remote;
    /* page mode */
    int                 ttysw_lpp;		/* page mode: lines per page */
    /* subprocess */
    int                 ttysw_pidchild;		/* pid of the child */
    /* Caps Lock */
    int                 ttysw_capslocked;
#define TTYSW_CAPSLOCKED	0x01	/* capslocked on mask bit */
#define TTYSW_CAPSSAWESC	0x02	/* saw escape while caps locked */
    /* stuff from old ttytlsw */
    enum ttysw_hdrstate	hdrstate;		/* string trying to load */
    CHAR		*nameptr;               /* namebuf ptr */
    CHAR		namebuf[256];           /* accumulates esc string */
    /* selection */
    int                 ttysw_butdown;		/* which button is down */
    struct ttyselection	ttysw_caret;
    struct ttyselection	ttysw_primary;
    struct ttyselection	ttysw_secondary;
    struct ttyselection	ttysw_shelf;
    caddr_t             ttysw_seln_client;
    /* replaceable ops (return TTY_OK or TTY_DONE) */
    int                 (*ttysw_escapeop) (Tty_view ttysw_view_public, register CHAR c, register int ac, register int *av);	/* handle escape sequences */
    int                 (*ttysw_stringop) (caddr_t data, char type, char c);	/* handle accumulated string */
    int                 (*ttysw_eventop) ();	/* handle input event */
    /* kbd translation */
    struct keymaptab    ttysw_kmt[3 * 16 + 2];	/* Key map list */
    struct keymaptab   *ttysw_kmtp;		/* next empty ttysw_kmt slot */
    int		  	(*layout_proc)(); /* interposed window layout proc */
#ifdef  OW_I18N
    int                 im_first_col;
    int                 im_first_row;
    int                 im_len;
    wchar_t             *im_store;
    XIMFeedback         *im_attr;
    Bool                preedit_state;
    XIC                 ic;
    int			implicit_commit;

    XIMCallback     	start_pecb_struct;
    XIMCallback     	draw_pecb_struct;
    XIMCallback     	done_pecb_struct;
#ifdef FULL_R5
    XIMStyle	xim_style;
#endif /* FULL_R5 */
#endif
    int			pass_thru_modifiers;  /* Modifiers we don't interpret */
    int			eight_bit_output; /* Print eight bit characters? */
}   Ttysw;

typedef Ttysw		*Ttysw_folio;

typedef struct ttysw_view_object {
    Tty_view		public_self;
    Ttysw_folio		folio;
} Ttysw_view_object;

typedef Ttysw_view_object* 	Ttysw_view_handle;

/* Values for ttysw_flags */
#define TTYSW_FL_FROZEN			0x1
#define TTYSW_FL_IS_TERMSW		0x2
#define TTYSW_FL_IN_PRIORITIZER		0x4

/*
 * Functions, macros, and typedefs for abstracting away differences between
 * termios and old BSD-style tty mode representations.
 */
/*
 * Access functions for tty characteristics.
 */
#ifdef	XV_USE_TERMIOS
#define	tty_gettabs(t)		((t)->termios.c_oflag & XTABS)
#if !defined(__linux__) || defined(VDSUSP)
#define	tty_getdsuspc(t)	((int) ((t)->termios.c_cc[VDSUSP]))
#else
#define	tty_getdsuspc(t)	((int) -1)
#endif
#define	tty_geteofc(t)		((int) ((t)->termios.c_cc[VEOF]))
#define	tty_geteolc(t)		((int) ((t)->termios.c_cc[VEOL]))
#define	tty_geteol2c(t)		((int) ((t)->termios.c_cc[VEOL2]))
#define	tty_getintrc(t)		((int) ((t)->termios.c_cc[VINTR]))
#define	tty_getlnextc(t)	((int) ((t)->termios.c_cc[VLNEXT]))
#define	tty_getquitc(t)		((int) ((t)->termios.c_cc[VQUIT]))
#define	tty_getrprntc(t)	((int) ((t)->termios.c_cc[VREPRINT]))
#define	tty_getstartc(t)	((int) ((t)->termios.c_cc[VSTART]))
#define	tty_getstopc(t)		((int) ((t)->termios.c_cc[VSTOP]))
#define	tty_getsuspc(t)		((int) ((t)->termios.c_cc[VSUSP]))
#else	/* XV_USE_TERMIOS */
#define	tty_gettabs(t)		((t)->sgttyb.sg_flags & XTABS)
#define	tty_getdsuspc(t)	((int) ((t)->ltchars.t_dsuspc))
#define	tty_geteofc(t)		((int) ((t)->tchars.t_eofc))
#define	tty_geteolc(t)		((int) ((t)->tchars.t_brkc))
#define	tty_geteol2c(t)		((int) ((t)->tchars.t_brkc))
#define	tty_getintrc(t)		((int) ((t)->tchars.t_intrc))
#define	tty_getlnextc(t)	((int) ((t)->ltchars.t_lnextc))
#define	tty_getquitc(t)		((int) ((t)->tchars.t_quitc))
#define	tty_getrprntc(t)	((int) ((t)->ltchars.t_rprntc))
#define	tty_getstartc(t)	((int) ((t)->tchars.t_startc))
#define	tty_getstopc(t)		((int) ((t)->tchars.t_stopc))
#define	tty_getsuspc(t)		((int) ((t)->ltchars.t_suspc))
#endif	/* XV_USE_TERMIOS */
/*
 * Predicates for tty characteristics.
 */
#ifdef	XV_USE_TERMIOS
#define	tty_iscanon(t)		(((t)->termios.c_lflag & ICANON) != 0)
#define	tty_isecho(t)		(((t)->termios.c_lflag & ECHO  ) != 0)
#define tty_issig(t)		(((t)->termios.c_lflag & ISIG  ) != 0)
#else	/* XV_USE_TERMIOS */
#define	tty_iscanon(t)		(((t)->sgttyb.sg_flags & (RAW|CBREAK)) == 0)
#define	tty_isecho(t)		(((t)->sgttyb.sg_flags & ECHO) != 0)
#define tty_issig(t)		(((t)->sgttyb.sg_flags & RAW) == 0)
#endif	/* XV_USE_TERMIOS */
/*
 * Capture fd's current tty modes and store them in *mode.
 */
#ifdef	XV_USE_TERMIOS
#define	tty_mode	termios		/* Ttysw field alias (ugh!) */
typedef struct termios	tty_mode_t;
#else	/* XV_USE_TERMIOS */
#define	tty_mode	sgttyb		/* Ttysw field alias (ugh!) */
typedef struct sgttyb	tty_mode_t;
#endif	/* XV_USE_TERMIOS */

/*
 * Determine where to store tty characteristics in the environment.  To avoid
 * possible misinterpretation, we use different locations depending on whether
 * or not XV_USE_TERMIOS is set.
 */
#ifdef	XV_USE_TERMIOS
#define	WE_TTYPARMS	"WINDOW_TERMIOS"
#define	WE_TTYPARMS_E	"WINDOW_TERMIOS="
#else	/* XV_USE_TERMIOS */
#define	WE_TTYPARMS	"WINDOW_TTYPARMS"
#define	WE_TTYPARMS_E	"WINDOW_TTYPARMS="
#endif	/* XV_USE_TERMIOS */


#define TTYSW_NULL      ((Ttysw *)0)

/*
 * Possible return codes from replaceable ops. 
 */
#define	TTY_OK		(0)	   /* args should be handled as normal */
#define	TTY_DONE	(1)	   /* args have been fully handled */

#define	ttysw_handleevent(ttysw, ie) \
	(*(ttysw)->ttysw_eventop)(TTY_PUBLIC(ttysw), (ie))
#define	ttysw_handleescape(_ttysw_view, c, ac, av) \
	(*(ttysw)->ttysw_escapeop)(TTY_VIEW_PUBLIC(_ttysw_view), (c), (ac), (av))
#define	ttysw_handlestring(ttysw, strtype, c) \
	(*(ttysw)->ttysw_stringop)((caddr_t)TTY_PUBLIC(ttysw), (strtype), (c))


#define MAX_LINES 128
typedef struct {
	int	caret_line_exposed:1;
	int	caret_line;
	int	leftmost;
	char	line_exposed[MAX_LINES];
} Tty_exposed_lines;

Xv_private int ttysw_view_obscured;

#endif /* _xview_private_ttysw_impl_h_already_included */
