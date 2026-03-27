/*
 * Declares public XView interfaces, types, and attributes for gettext.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(GETTEXT__H)
#define GETTEXT__H


char *bindtextdomain(char *domain_name, unsigned char *binding);
char *textdomain(char *domain_name);
char *gettext(char *msg_id);
char *dgettext(char *domain_name, char *msg_id);

#endif
