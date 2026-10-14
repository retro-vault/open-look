/*
 * Implements the ttytl routines used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ttytl_.h>
#include <xview_private/fm_geom_.h>
#include <xview_private/ttyansi_.h>
#include <xview_private/tty_main_.h>
#include <xview_private/txt_dbx_.h>
#include <xview_private/win_geom_.h>
#include <stdio.h>
#include <string.h>
#ifdef SVR4
#include <sys/types.h>
#endif
#include <sys/file.h>
#include <sys/signal.h>
#include <xview/xview.h>
#include <xview/icon.h>
#include <xview/icon_load.h>
#include <xview/wmgr.h>
#include <xview/frame.h>
#include <xview/ttysw.h>
#include <xview_private/i18n_impl.h>
#ifdef OW_I18N
#include <xview/font.h>
#endif
#include <xview_private/tty_impl.h>
#include <xview_private/term_impl.h>
#include <xview_private/charscreen.h>

static void ttytl_text_dimensions(Tty_view ttysw_view_public,
    int *columns, int *rows);


/* BUG ALERT: This entire procedure should be rewritten! */
/* BUG ALERT: No XView prefix */
Pkg_private int
ttytlsw_escape(ttysw_view_public, c, ac, av)
    Tty_view        ttysw_view_public;
    char            c;
    register int    ac, *av;
{
    Tty             ttysw_public;
    Xv_object       frame_public;
    char            buf[150];
    char           *p, *text;
    struct rect     rect, orect;
    Ttysw_folio     ttysw, ttytlsw;
#ifdef OW_I18N
    Xv_Font         font;
#endif

    ttytlsw = ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(ttysw_view_public);
    ttysw_public = TTY_PUBLIC(ttysw);
    if (c != 't')
	return (ttysw_ansi_escape(ttysw_view_public, c, ac, av));
    frame_public = xv_get(ttysw_public, WIN_FRAME);
    switch (av[0]) {
      case 1:			/* open */
	xv_set(frame_public, FRAME_CLOSED, FALSE, 0);
	break;
      case 2:			/* close */
	xv_set(frame_public, FRAME_CLOSED, TRUE, 0);
	break;
      case 3:			/* move */
	/*
	 * no more interactive moves if (ac == 1 && !xv_get(frame_public,
	 * FRAME_CLOSED)) { wmgr_move(frame_public); break; }
	 */
	(void) win_getrect(frame_public, &rect);
	orect = rect;
	if (av[1] < 0)
	    av[1] = rect.r_top;
	if (ac < 3 || av[2] < 0)
	    av[2] = rect.r_left;
	rect.r_top = av[1];
	rect.r_left = av[2];
	if (!xv_get(frame_public, FRAME_CLOSED))
	    wmgr_completechangerect(frame_public, &rect, &orect,
				    0, 0);
	else
	    (void) win_setrect(frame_public, &rect);
	break;
      case 4:			/* stretch */
	/*
	 * no more interactive stretches if (ac == 1 && !xv_get(frame_public,
	 * FRAME_CLOSED)) { wmgr_stretch(frame_public); break; }
	 */
	(void) win_getrect(frame_public, &rect);
	orect = rect;
	if (av[1] < 0)
	    av[1] = rect.r_height;
	if (ac < 3 || av[2] < 0)
	    av[2] = rect.r_width;
	rect.r_width = av[2];
	rect.r_height = av[1];
	if (!xv_get(frame_public, FRAME_CLOSED))
	    wmgr_completechangerect(frame_public, &rect, &orect,
				    0, 0);
	else
	    (void) win_setrect(frame_public, &rect);
	break;
      case 5:			/* top */
	wmgr_top(frame_public);
	break;
      case 6:			/* bottom */
	wmgr_bottom(frame_public);
	break;
      case 7:			/* refresh */
	wmgr_refreshwindow(frame_public);
	break;
      case 8:			/* stretch, size in chars */
	/*
	 * no more itneractive stretches if (ac == 1 && !xv_get(frame_public,
	 * FRAME_CLOSED)) { wmgr_stretch(frame_public); break; }
	 */
	(void) win_getrect(frame_public, &rect);
	orect = rect;
	if (av[1] <= 0)
	    av[1] = (int) xv_get(frame_public, WIN_ROWS);
	if (ac < 3 || av[2] <= 0)
	    av[2] = (int) xv_get(frame_public, WIN_COLUMNS);
#ifdef OW_I18N
        /*
         * This is actually nothing to do with the I18N issue.  Just
         * get around the problem with xv_pf_sys.
         */
        font = (Xv_Font) xv_get(ttysw_public, WIN_FONT);
        rect.r_width = (av[2] * (int) xv_get(font, FONT_DEFAULT_CHAR_WIDTH))
                                + (2 * FRAME_BORDER_WIDTH);
        rect.r_height = (av[1] * (int) xv_get(font, FONT_DEFAULT_CHAR_HEIGHT))
                                + FRAME_BORDER_WIDTH;
#else
	rect.r_width = frame_width_from_columns(av[2]);

	rect.r_height = frame_height_from_lines(av[1],
				    xv_get(frame_public, FRAME_SHOW_LABEL));
#endif
	if (!xv_get(frame_public, FRAME_CLOSED))
	    wmgr_completechangerect(frame_public, &rect, &orect,
				    0, 0);
	else
	    (void) win_setrect(frame_public, &rect);
	break;
      case 11:			/* report open or iconic */
	if (!xv_get(frame_public, FRAME_CLOSED))
	    p = "\33[1t";
	else
	    p = "\33[2t";
	(void) ttysw_input_it(ttysw, p, 4);
	break;
      case 13:			/* report position */
	(void) win_getrect(frame_public, &rect);
	(void) sprintf(buf, "\33[3;%d;%dt", rect.r_top, rect.r_left);
	(void) ttysw_input_it(ttysw, buf, strlen(buf));
	break;
      case 14:			/* report size */
	(void) win_getrect(frame_public, &rect);
	(void) sprintf(buf, "\33[4;%d;%dt", rect.r_height, rect.r_width);
	(void) ttysw_input_it(ttysw, buf, strlen(buf));
	break;
      case 18:			/* report size in chars */
	{
		int rows, columns;
		if (ttysw_getopt(ttysw, TTYOPT_TEXT)) {
			ttytl_text_dimensions(ttysw_view_public, &columns,
			    &rows);
		}
		else {
			rows = y_to_row(winheightp);
			columns = x_to_col(winwidthp);
		}
		(void) sprintf(buf, "\33[8;%d;%dt", rows, columns);
	}
	(void) ttysw_input_it(ttysw, buf, strlen(buf));
	break;
      case 20:{		/* report icon label */
	    Icon            icon = (Icon) xv_get(xv_get(TTY_PUBLIC(ttytlsw),
							WIN_FRAME),
						 FRAME_ICON);

	    (void) ttysw_input_it(ttysw, "\33]L", 3);
	    if (0 == (text = (char *) xv_get(icon, ICON_LABEL))) {
		text = (char *) xv_get(frame_public, FRAME_LABEL);
	    }
	    if (text)
		(void) ttysw_input_it(ttysw, text, strlen(text));
	    (void) ttysw_input_it(ttysw, "\33\\", 2);
	    break;
	}
      case 21:			/* report name stripe */
	(void) ttysw_input_it(ttysw, "\33]l", 3);
	if (text = (char *) xv_get(frame_public, FRAME_LABEL))
	    (void) ttysw_input_it(ttysw, text, strlen(text));
	(void) ttysw_input_it(ttysw, "\33\\", 2);
	break;
      default:
	return (TTY_OK);
    }
    return (TTY_DONE);
}

/* BUG ALERT: No XView prefix */
Pkg_private int
ttytlsw_string(ttysw_public, type, c)
    caddr_t             ttysw_public;
    CHAR            type, c;
{
    Ttysw_folio     ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(ttysw_public);
    Ttysw          *ttytlsw = ttysw;
    CHAR            namestripe[150];
    CHAR            iconlabel[33];

    if (type != ']')
	return (ttysw_ansi_string((caddr_t)ttysw_public, type, c));
    switch (ttytlsw->hdrstate) {
      case HS_BEGIN:
	switch (c) {
	  case 'l':
	    ttytlsw->nameptr = ttytlsw->namebuf;
	    ttytlsw->hdrstate = HS_HEADER;
	    break;
	  case 'I':
	    ttytlsw->nameptr = ttytlsw->namebuf;
	    ttytlsw->hdrstate = HS_ICONFILE;
	    break;
	  case 'L':
	    ttytlsw->nameptr = ttytlsw->namebuf;
	    ttytlsw->hdrstate = HS_ICON;
	    break;
	  case '\0':
	    break;
	  default:
	    ttytlsw->hdrstate = HS_FLUSH;
	    break;
	}
	break;
      case HS_HEADER:
      case HS_ICONFILE:
      case HS_ICON:
#ifdef OW_I18N
	/*
	 * wchar is used for the internal processing.
	 */
	if ( iswprint(c) ) {
#else
	/* jcb 6/1/90 -- change to let (all) eight bit charactes in */
	if ((c >= ' ' && c <= '~') || ((c & 0x80) != 0) ) {
#endif
	    if (ttytlsw->nameptr <
		&ttytlsw->namebuf[sizeof(ttytlsw->namebuf) - 1]) 
		*ttytlsw->nameptr++ = c;
	} else if (c == '\0') {
	    *ttytlsw->nameptr = '\0';
	    switch (ttytlsw->hdrstate) {
	      case HS_HEADER:
#ifdef OW_I18N
                (void) STRNCPY(namestripe, ttytlsw->namebuf,
                               sizeof(namestripe) / sizeof(CHAR));
                (void) xv_set(xv_get(TTY_PUBLIC(ttysw), WIN_FRAME),
                              XV_LABEL_WCS, namestripe, 0);
#else
		(void) strncpy(namestripe, ttytlsw->namebuf,
			       sizeof(namestripe));
		(void) xv_set(xv_get(TTY_PUBLIC(ttysw), WIN_FRAME),
			      FRAME_LABEL, namestripe, 0);
#endif
		break;
	      case HS_ICONFILE:{
		    char            err[IL_ERRORMSG_SIZE];
		    struct pixrect *mpr;
		    Icon            icon;
#ifdef OW_I18N
                    char            *mb_filename;

                    mb_filename = _xv_wcstombsdup(ttytlsw->namebuf);
                    if ((mpr = icon_load_mpr(mb_filename, err)) ==
#else
		    if ((mpr = icon_load_mpr(ttytlsw->namebuf, err)) ==
#endif
			(struct pixrect *) 0) {
			xv_error((Xv_opaque)mpr,
				 ERROR_STRING, err,
				 ERROR_PKG, TTY,
				 0);
		    } else {
			Frame	frame = xv_get(TTY_PUBLIC(ttysw), WIN_FRAME);
			Icon	current_icon = xv_get(frame, FRAME_ICON);

			if (current_icon)  {
			    xv_set(current_icon, ICON_IMAGE, mpr, NULL);
			}
			else  {
			    icon = icon_create( ICON_IMAGE, mpr, 0 );
			    (void) xv_set(xv_get(TTY_PUBLIC(ttysw), WIN_FRAME),
				          FRAME_ICON, icon, 0);
			}
		    }
#ifdef OW_I18N
                    free(mb_filename);
#endif
		    break;
		}
	      case HS_ICON: {
		Frame		frame = 
				xv_get(TTY_PUBLIC(ttysw), WIN_FRAME);
#ifdef OW_I18N
                (void) STRNCPY(iconlabel, ttytlsw->namebuf,
                               sizeof(iconlabel) / sizeof(CHAR));
                (void) xv_set(xv_get(frame, FRAME_ICON),
                              XV_LABEL_WCS, iconlabel, 0);
#else
		(void) strncpy(iconlabel, ttytlsw->namebuf,
			       sizeof(iconlabel));
		(void) xv_set(xv_get(frame, FRAME_ICON),
			      ICON_LABEL, iconlabel, 0);
#endif
		}
		break;
	      default:{
		}
	    }
	    ttytlsw->hdrstate = HS_BEGIN;
	}
	break;
      case HS_FLUSH:
	if (c == '\0')
	    ttytlsw->hdrstate = HS_BEGIN;
	break;
      default:
	return (ttysw_ansi_string((caddr_t)ttysw_public, type, c));
    }
    return (TTY_DONE);
}
static void
ttytl_text_dimensions(ttysw_view_public, columns, rows)
    Tty_view ttysw_view_public;
    int     *columns;
    int     *rows;
{
    Xv_termsw_view *view = (Xv_termsw_view *) ttysw_view_public;
    Xv_opaque       saved_private = view->parent_data.private_data;
    Textsw          text_view = (Textsw) view;

    view->parent_data.private_data = view->private_text;
    *columns = textsw_screen_column_count(text_view);
    *rows = textsw_screen_line_count(text_view);
    view->parent_data.private_data = saved_private;
}
