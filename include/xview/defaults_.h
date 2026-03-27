/*
 * Declares public XView interfaces, types, and attributes for
 * defaults.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DEFAULTS__H)
#define DEFAULTS__H

#include <xview/pkg.h>
#include <xview/defaults.h>

Bool defaults_exists(char *name, char *class);
Bool defaults_get_boolean(char *name, char *class, Bool default_bool);
char defaults_get_character(char *name, char *class, int default_char);
int defaults_get_enum(char *name, char *class, Defaults_pairs *pairs);
int defaults_get_integer(char *name, char *class, int default_integer);
int defaults_get_integer_check(char *name, char *class, int default_int, int minimum, int maximum);
char *defaults_get_string(char *instance, char *class, char *default_string);
void defaults_set_character(char *resource, int value);
void defaults_set_boolean(char *resource, Bool value);
void defaults_set_integer(char *resource, int value);
void defaults_set_string(char *resource, char *value);
void defaults_init_db(void);
void defaults_load_db(char *filename);
void defaults_store_db(char *filename);
int defaults_lookup(register char *name, register Defaults_pairs *pairs);
#ifdef OW_I18N
void defaults_set_locale(char *locale, Xv_generic_attr locale_attr);
char * defaults_get_locale(void);
#endif

#endif
