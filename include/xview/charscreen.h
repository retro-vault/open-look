/*
 * Declares public XView interfaces, types, and attributes for
 * charscreen.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#define row_to_y(row)	((row)*chrheight)
#define col_to_x(col)	(((col)*chrwidth) + chrleftmargin)
#define y_to_row(y)	((y)/chrheight)
#define x_to_col(x)	((((x) >= chrleftmargin) ? \
			  ((x) - chrleftmargin) : 0)/chrwidth)

/*
 * Character dimensions (fixed width fonts only!)
 * and of screen in pixels.
 */
#if !defined(__linux__) || defined(__DEFINE_CHARSCREEN_VARS)
int	chrheight, chrwidth, chrbase;
int	winheightp, winwidthp;
int	chrleftmargin;

struct	pixfont *pixfont;

/*
 * If delaypainting, delay painting.  Set when clear screen.
 * When input will block then paint characters (! white space) of entire image
 * and turn delaypainting off.
 */
int	delaypainting;
#else /* __linux__ && !__DEFINE_CHARSCREEN_VARS */
extern int	chrheight, chrwidth, chrbase;
extern int	winheightp, winwidthp;
extern int	chrleftmargin;
extern struct	pixfont *pixfont;
extern int	delaypainting;
#endif

#if defined(cplus)
void	ttysw_pstring(CHAR *s, char mode, int col, int row, int op);
void	ttysw_pclearline(int fromcol, int tocol, int row);
void	ttysw_pcopyline(int fromcol, int tocol, int count, int row);
void	ttysw_pclearscreen(int fromrow, int torow);
#endif
