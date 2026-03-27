/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_INIT__H)
#define TTY_INIT__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h> 

struct ttysw_createoptions {
    int             becomeconsole;	/* be the console */
    char          **argv;	/* args to be used in exec */
    char           *args[4];	/* scratch array if need to build argv */
};

Pkg_private int ttysw_lookup_boldstyle(char *str);
Pkg_private int ttysw_print_bold_options(void);
Pkg_private Xv_opaque ttysw_init_view_internal(Tty parent, Tty_view tty_view_public);
Pkg_private Xv_opaque ttysw_init_folio_internal(Tty tty_public);
int ttysw_add_FNDELAY(int fd);
Pkg_private int ttysw_fork_it(caddr_t ttysw0, char **argv, int wfd);
void ttysw_parseargs(struct ttysw_createoptions *opts, int *argcptr, char **argv_base);
Pkg_private void ttysw_done(Ttysw_folio ttysw_folio_private);
int updateutmp(char *username, int ttyslotuse, int ttyfd);

#endif
