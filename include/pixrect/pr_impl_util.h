/*
 * Internal pixrect implementation macros for loops, pointers, and ops.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef	pr_impl_util_DEFINED
#define	pr_impl_util_DEFINED

/* Reiser cpp concatenation macros */
#ifndef _CAT
# ifdef __STDC__
#  define _CAT(a,b) a ## b
# else /* __STDC__ */
#  undef  _IDENT
#  define _IDENT(x)	x
#  define _CAT(a,b)	_IDENT(a)b
# endif /* __STDC__ */
#endif /* _CAT */

/*
 * code selection macros
 */
#define	IFTRUE(a,b) a
#define IFFALSE(a,b) b

/* and together multiple options */
#define	IFAND IFAND2
#define IFAND2(opt1,opt2,a,b) opt1(opt2(a,b),b)
#define IFAND3(opt1,opt2,opt3,a,b) opt1(opt2(opt3(a,b),b),b)
#define IFAND4(opt1,opt2,opt3,opt4,a,b) opt1(opt2(opt3(opt4(a,b),b),b),b)
#define IFAND5(opt1,opt2,opt3,opt4,opt5,a,b) \
		opt1(opt2(opt3(opt4(opt5(a,b),b),b),b),b)
#define IFAND6(opt1,opt2,opt3,opt4,opt5,opt6,a,b) \
		opt1(opt2(opt3(opt4(opt5(opt6(a,b),b),b),b),b),b)

#define	IFKERNEL IFFALSE

/*
 * lint garbage
 */
#define	IFLINT	IFFALSE

#define	_ZERO_ 0
#define	_ONE_ 1

/*
 * portability aids
 */
#define IF68000	IFFALSE

typedef	int	INT_T;
typedef caddr_t	PTR_T;
typedef	int	LOOP_T;

#define	LOOP_DECR(var)	(--(var) >= 0)

#define	IFSPARC	IFFALSE

/* true if we can make 32 bit accesses on 16 bit boundaries */
#ifndef	SHORT_ALIGN
#define	SHORT_ALIGN	1
#endif

#if SHORT_ALIGN
#define	IFSHORT_ALIGN	IFTRUE
#else
#define	IFSHORT_ALIGN	IFFALSE
#endif


#ifndef LITTLE_ENDIAN
#define	LITTLE_ENDIAN	1
#endif

#if LITTLE_ENDIAN
#define	ENDIAN	IFTRUE
#else
#define	ENDIAN	IFFALSE
#endif

#define	IFSUNOS4	IFFALSE
#define	IFSUNOS41	IFFALSE


/*
 * misc. macros
 */

/* statement macro */
#define	_STMT(op)	do { op } while (_ZERO_)

/* loop macros */
#define	PR_LOOPVP(var, op) 	do { op; } while (LOOP_DECR(var))
#define	PR_LOOPV(var, op)	_STMT(while (LOOP_DECR(var)) { op; })
#define	PR_LOOPP(count, op)	_STMT( \
	IFLINT(, register LOOP_T) _loop = (count); PR_LOOPVP(_loop, op);)
#define	PR_LOOP(count, op)	_STMT( \
	IFLINT(, register LOOP_T) _loop = (count); PR_LOOPV(_loop, op);)

/* pointer manipulation */
#define	PTR_ADD(p, incr)	((caddr_t) (p) + (incr))
#define	PTR_INCR(type, p, incr)	(p = (type) ((caddr_t) (p) + (incr)))

/* unshifted pixrect op codes */
#define	PIX_OPSRC	(12)
#define	PIX_OPDST	(10)
#define	PIX_OPNOT(op)	((op) ^ 15)
#define	PIX_OPCLR	(0)
#define	PIX_OPSET	(15)

/* extract color and op code fields */
#define	PIXOP_COLOR(op)	((op) >> 5)
#define	PIXOP_OP(op)	((op) >> 1 & 15)

/* reverse video src or dst */
#define	PIX_OP_REVERSESRC(op)	(((op) & 3) << 2 | (op) >> 2 & 3)
#define	PIX_OP_REVERSEDST(op)	((~(op) & 5) << 1 | ~(op) >> 1 & 0x5)

/* determine if op needs src or dst */
#define	PIX_OP_NEEDS_SRC(op)	(((op) >> 2 ^ (op)) & 3)
#define	PIX_OP_NEEDS_DST(op)	(((op) >> 1 ^ (op)) & 5)

/* misc. data types */
typedef	short	MPR_T;		/* type used for memory pixrect data */
typedef	u_short	UMPR_T;		/* unsigned equivalent of MPR_T */

#endif /* 	pr_impl_util_DEFINED */
