/*
 * Implements the xv casecmp routines used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/xv_casecmp_.h>
#include <stdio.h>
#include <ctype.h>
#include <xview_private/i18n_impl.h>

int
xv_strcasecmp( str1, str2 )
     char *str1;
     char *str2;
{
    char low1, low2;

    if ( str1 == str2 )
	return 0;

    while ( (low1 = tolower(*str1)) == (low2 = tolower(*str2)) ) {
	if ( !low1 )
	    return 0;
	str1++; str2++;
    }

    return low1 - low2;
}

int
xv_strncasecmp( str1, str2, n)
     char *str1;
     char *str2;
     int   n;
{
    char low1, low2;

    if ( str1 == str2 )
	return 0;

    n++;

    while ( (--n != 0) && ((low1 = tolower(*str1)) == (low2 = tolower(*str2))) ) {
	if ( !low1 )
	    return 0;
	str1++; str2++;
    }

    return ( (n == 0) ? 0 : (low1 - low2) );
}

#ifdef OW_I18N

int
xv_wscasecmp( str1, str2 )
     CHAR *str1;
     CHAR *str2;
{
    CHAR low1, low2;

    if ( str1 == str2 )
	return 0;

    while ( (low1 = towlower(*str1)) == (low2 = towlower(*str2)) ) {
	if ( !low1 )
	    return 0;
	str1++; str2++;
    }

    return low1 - low2;
}

int
xv_wsncasecmp( str1, str2, n)
     CHAR *str1;
     CHAR *str2;
     int   n;
{
    CHAR low1, low2;

    if ( str1 == str2 )
	return 0;

    n++;

    while ( (--n != 0) && ((low1 = towlower(*str1)) == (low2 = towlower(*str2))) ) {
	if ( !low1 )
	    return 0;
	str1++; str2++;
    }

    return ( (n == 0) ? 0 : (low1 - low2) );
}

#endif /* OW_I18N */
