/*
 * Implements the seln routines used by the XView sel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/seln_.h>
#include <xview_private/gettext_.h>
#include <xview_private/getlogindr_.h>
#include <xview_private/win_global_.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#ifndef XVIEW_USE_INSECURE_TMPFILES
/* martin.buck@bigfoot.com */
#include <dirent.h>
#endif
#include <xview_private/i18n_impl.h>
#include <xview/xview_xvin.h>
#include <xview/selection.h>
#include <unistd.h>			/* unlink() */

struct selection selnull;

#define	SEL_FILEMODE	0X0777

/* ARGSUSED */
void
selection_set(sel, sel_write, sel_clear, window)
    struct selection *sel;
    int             (*sel_write) (), (*sel_clear) ();
    Xv_object       window;
/* Note: sel_clear not used yet */
{
    FILE           *file;
    int             firsttime = 1;

    (void) win_lockdata(window);
Open:
    if ((file = fopen(selection_filename(), "w+")) == NULL) {
	if (firsttime) {
	    firsttime = 0;
	    if (unlink(selection_filename()) == 0)
		goto Open;
	}
	(void) win_unlockdata(window);
	(void) fprintf(stderr, 
		XV_MSG("%s would not open\n"), 
		selection_filename());
#ifdef SUNVIEW1			/******* BUG ************/
	(void) werror(-1, window);
#endif
	return;
    }
    /* Make so everyone can access */
    (void) fchmod(fileno(file), 0666);
    (void) fprintf(file, "TYPE=%ld, ITEMS=%ld, ITEMBYTES=%ld, PUBFLAGS=%lx, PRIVDATA=%lx\n",
		   (long)sel->sel_type, (long)sel->sel_items, (long)sel->sel_itembytes,
		   (long)sel->sel_pubflags, (long)sel->sel_privdata);
    (*sel_write) (sel, file);
    (void) fclose(file);
    (void) win_unlockdata(window);
}

void
selection_get(sel_read, window)
    int             (*sel_read) ();
    Xv_object       window;
{
    struct selection selection, *sel = &selection;
    FILE           *file;
    char             c;
    int             n;
    /*extern          errno;*/

    *sel = selnull;
    (void) win_lockdata(window);
    if ((file = fopen(selection_filename(), "r")) == NULL) {
	(void) win_unlockdata(window);
	if (errno == ENOENT)
	    return;		/* No selection available */
	(void) fprintf(stderr, 
		XV_MSG("%s would not open\n"), 
		selection_filename());
#ifdef SUNVIEW1			/******* BUG ************/
	(void) werror(-1, window);
#endif
	return;
    }
    if ((c = getc(file)) == EOF) {
	goto Cleanup;		/* Selection has been cleared */
    } else
	(void) ungetc(c, file);
    if ((n = fscanf(file,
	 "TYPE=%d, ITEMS=%d, ITEMBYTES=%d, PUBFLAGS=%x, PRIVDATA=%p%c",
		    &sel->sel_type, &sel->sel_items, &sel->sel_itembytes,
		    &sel->sel_pubflags, &sel->sel_privdata, &c)) != 6) {
	(void) win_unlockdata(window);
	(void) fprintf(stderr, 
		XV_MSG("%s not in correct format\n"),
		       selection_filename());
	(void) fprintf(stderr, "TYPE=%d, ITEMS=%d, ITEMBYTES=%d, PUBFLAGS=%x, PRIVDATA=%lx c=%c, n=%d\n",
		       sel->sel_type, sel->sel_items, sel->sel_itembytes,
		       sel->sel_pubflags, (long)sel->sel_privdata, c, n);
	goto Cleanup;
    }
    (*sel_read) (sel, file);
Cleanup:
    (void) fclose(file);
    (void) win_unlockdata(window);
}

void
selection_clear(window)
    Xv_object       window;
{
    FILE           *file;

    (void) win_lockdata(window);
    if ((file = fopen(selection_filename(), "w+")) == NULL) {
	(void) win_unlockdata(window);
	(void) fprintf(stderr, 
		XV_MSG("%s would not open\n"), 
		selection_filename());
#ifdef SUNVIEW1			/******* BUG ************/
	(void) werror(-1, window);
#endif
	return;
    }
    (void) fclose(file);
    (void) win_unlockdata(window);
}

char           *
selection_filename()
{
    char           *name;
#ifndef XVIEW_USE_INSECURE_TMPFILES
    /* martin.buck@bigfoot.com */
    static char homename[MAXNAMLEN];
#endif

    if ((name = getenv("SELECTION_FILE")) == NULL)
#ifdef XVIEW_USE_INSECURE_TMPFILES
	name = "/tmp/winselection";
#else
	if (!homename[0]) {
	    sprintf(homename, "%s/.winselection", xv_getlogindir());
	}
	name = homename;
#endif
    return (name);
}
