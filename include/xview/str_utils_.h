/*
 * Declares public XView interfaces, types, and attributes for str
 * utils.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(STR_UTILS__H)
#define STR_UTILS__H

#include <xview/pkg.h>
#include <xview/str_utils.h>

Bool xv_substring(char *s, int start, int n, char *dest);
Bool xv_substrequal(char *s1, int start1, char *s2, int start2, int n, Bool case_matters);
Bool string_equal(char *s1, char *s2, Bool case_matters);
int string_find(char *s, char *target, Bool case_matters);
char *string_get_token(char *s, int *index, char *dest, enum CharClass (*charproc)(char));
char *string_get_sequence(char *s, int *index, char *dest, struct CharAction (*charproc)());

#endif
