/*
 * mem.h: declarations and shared types for the mem module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_MEM_H
#define _OLWM_MEM_H

/* MemAlloc: Alloc mem. */
extern void *MemAlloc();	/* malloc frontend */
/* MemCalloc: Process mem calloc. */
extern void *MemCalloc();	/* calloc frontend */
/* MemFree: Free mem. */
extern void MemFree();		/* free frontend */
/* MemRealloc: Process mem realloc. */
extern void *MemRealloc();	/* realloc frontend */

#ifdef MEMDEBUG
/* d_MemAlloc: Alloc d mem. */
extern void *d_MemAlloc();
/* d_MemFree: Free d mem. */
extern void d_MemFree();
/* d_MemRealloc: Process d mem realloc. */
extern void *d_MemRealloc();
/* d_MemCalloc: Process d mem calloc. */
extern void *d_MemCalloc();

#define MemAlloc(s)	d_MemAlloc((s), __FILE__, __LINE__, NULL)
#define MemCalloc(n,s)	d_MemCalloc((n),(s), __FILE__, __LINE__)
#define MemFree(p)	d_MemFree(p)
#define MemRealloc(p,s)	d_MemRealloc((p),(s))

#define MemNew(X) 	d_MemAlloc(sizeof(X), __FILE__, __LINE__, #X)
#define MemNewString(s) (strcpy(d_MemAlloc(strlen(s)+1,__FILE__,__LINE__,"(string)"),s))

extern int MemAcct;
extern int AcctTag;
#else
#define MemNew(t) ((t *)MemAlloc((unsigned int)sizeof(t)))
#define MemNewString(s) (strcpy((char *)MemAlloc(strlen(s)+1),s))
#endif /* MEMDEBUG */

#ifdef OW_I18N_L4

#define MemNewText(s)	wscpy((wchar_t *)MemAlloc((wslen(s)+1)*sizeof(wchar_t)),s)

#else

#define MemNewText(s)	MemNewString((char *)s)

#endif /* OW_I18N_L4 */

#endif /* _OLWM_MEM_H */
