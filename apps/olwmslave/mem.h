/*
 * Declares the mem interfaces used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

extern void *MemAlloc();	/* malloc frontend */
extern void *MemCalloc();	/* calloc frontend */
extern void MemFree();		/* free frontend */

#define MemNew(t) ((t *)MemAlloc((unsigned int)sizeof(t)))
#define MemNewString(s) (strcpy(MemAlloc(strlen(s)+1),s))
#ifdef OW_I18N_L4
#define MemNewWString(s) (wscpy(MemAlloc((wslen(s)+1) * sizeof(wchar_t)),s))
#endif /* OW_I18N_L4 */
