#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/* C routines called by the (mainly) FORTRAN plotting
 * package 'ARK'
 * ARK are Alan Smale, Robin Corbet, Koji Mukai ( and Timn)
 * This version had diverged quite a lot from other versions.
 * The 'C' routines here are all new but the FORTRAN
 * companions are much closer to the originals.
 */

#include <math.h>
#include <ctype.h>
#include <stdio.h>

#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/cursor.h>
#include <xview/panel.h>
#include <xview/canvas.h>
#include <xview/xv_xrect.h>
#include <xview/cms.h>
#include <xview/textsw.h>
#include <xview/server.h>
#include <xview/seln.h>
#include <xview/notice.h>
#include <xview/notify.h>
#include <xview/font.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>

/* system routines */
#define	LPR "/usr/ucb/lpr"
/* #define RM "/bin/rm" */
#define CREATOR		"Robot"

#define KANJI_IN_JIS "$B"
#define KANJI_IN_OLD_JIS "$@"
#define KANJI_OUT_ASCII "(B"
#define KANJI_OUT_JIS "(J"

#define	FONT_FAMILY_PALATINO		"FONT_STYLE_PALATINO" /* semi-kludges */
#define	FONT_FAMILY_AVANTGARDE		"FONT_STYLE_AVANTGARDE" 
#define	FONT_FAMILY_BOOKMAN		"FONT_STYLE_BOOKMAN" 
#define	FONT_FAMILY_HELVETICAN		"FONT_STYLE_HELVETICAN" 
#define	FONT_FAMILY_NEWCENTURY		"FONT_STYLE_NEWCENTURY" 
#define	FONT_FAMILY_CHANCERY		"FONT_STYLE_CHANCERY" 
#define	FONT_FAMILY_DINGBATS		"FONT_STYLE_DINGBATS" 
#define	FONT_FAMILY_SYMBOL		"FONT_STYLE_SYMBOL" 
#define	FONT_FAMILY_KANJI		"FONT_STYLE_KANJI" 

/* flags to tell angle_text whether it has to deal with a one or two
 * byte character string */
#define	TEXT8	8
#define	TEXT16	16 

#define	PS_FAST		500
#define	PST_FAST	100
#define	PS_SLOW		500
#define	PST_SLOW	500
#define TEXT_LENGTH	81
#define	DTR	0.0174532 /* conversion between degress and radians */
#define TOLERANCE	8.73e-3 /* angle change before we let makefont
				 * make a new font */
#define	WIDTH_DEFAULT	0.75 /* default width of lines in postscript
				output */
#define	MAX_PATH	100 /* maximum number of points for drawing
				a filled shape */
#define BIG_NUMBER	10000.

#define CENTRE_TEXT	0
#define RIGHT_TEXT	1
#define TEXT_CONTINUATION 2
#define RAW_TEXT	3

#define	SUPER_SCRIPT	0
#define SUB_SCRIPT	1
#define BACK_SPACE	2

/* font styles */
#define NORMAL_TEXT	0
#define	BOLD_TEXT	1
#define	ITALIC_TEXT	2
#define	BOLD_ITALIC_TEXT 3
/* set up no. of styles - four for regular fonts; this is used later
 * in the PostScript output routine */
#define NO_OF_STYLES	4


/* used as an ID in the PostScript output as a flag for these fonts */
#define	KANJI_ID	-500
#define	SYMBOL_ID	-600
#define	DINGBATS_ID	-700
#define	CHANCERY_ID	-800

/* font families */
#define TIMES		0
#define HELVETICA	1
#define COURIER		2
#define	PALATINO	3
#define	AVANTGARDE	4
#define	BOOKMAN		5
#define	HELVETICAN	6
#define	NEWCENTURY	7

#define	CHANCERY	99

#define	END_REGULAR_FONTS 100 /* marker */

#define	SYMBOL		400
#define	KANJI		500
#define	DINGBATS	600

/* three line styles */
#define SOLID	0
#define	DASH	1
#define	DOTDASH 2

/* The symbols that can be drawn */
#define STAR 1
#define BOX 2
#define CROSS 3
#define CIRCLE 4
#define FILL_CIRCLE 5
#define FILL_BOX 6
#define DIAMOND 7
#define FILL_DIAMOND 8
#define TRIANGLE 9
#define FILL_TRIANGLE 10

/* and the default size of them */
#define	SYMBOL_SIZE	2
/* default arrow head size */
#define ARROW_HEAD_SIZE 15
/* default arrow head angle */
#define ARROW_HEAD_ANGLE 30.
/* different style for arrow heads */
#define	ARROW_DEFAULT	0
#define	ARROW_FILL	1
#define ARROW_HOLLOW	2
/* set whether we have one or two arrow heads */
#define ARROW_SINGLE	1
#define ARROW_DOUBLE	2

/* These are for the post-script "meta-file" held in memory
 * Numbers < PS_ROTATE_TEXT should be used if the x,y values in the
 * meta file are to be converted to coordinates on the page 
 * numbers higher than PS_TEXT are if no conversion is required 
 * numbers below PS_LINE are "move" type instructions */
#define PS_MOVE		100	/* move */
#define PS_TEXT_RIGHT	180	/* text right justified on position*/
#define PS_TEXT_CENTRE	190	/* text centred on position*/
#define	PS_RAW_TEXT	195	/* straight text including control sequences */
#define PS_TEXT		200	/* text */
#define PS_ARROW_START	250	/* start of arrow */
/* ones which draw lines too */
#define PS_LINE		300	/* line */
#define	PS_FILL 	310	/* points to be filled */
#define PS_END_FILL 	320	/* last point of fill */
#define PS_CIRCLE_CENTRE 350	/* centre of a circle */
#define PS_CIRCLE_END	360	/* boundary of circle */
#define PS_ELLIPSE_CENTRE 370	/* centre of an ellipse */
#define PS_ELLIPSE_END	380	/* boundary of ellipse */
#define PS_ARROW_END	392	/* end of arrow */
#define PS_ARC_CENTRE	400	/* centre of arc */
#define PS_ARC_RADIUS	410	/* to help calculate radius */
#define PS_POLY_START	440	/* start of a polygon drawing sequence */
#define PS_POLY_POINT	450	/* a point in a polygon sequence */
/* functions for which conversion is not required 
 * (PS_ROTATE_TEXT must be lowest number) */
#define	PS_ROTATE_TEXT	1300	/* rotate text */
#define PS_RAW_NCHAR	1350	/* number of characters in raw text */
#define	PS_ARROW_SIZE	1400	/* change in arrow head size */
#define	PS_ARROW_ANGLE	1405	/* change in arrow head angle */
#define	PS_ARROW_STYLE	1410	/* change in arrow style */
#define PS_TEXT_CONTINUE 1450 /* text with position already defined */
#define	PS_COLOUR	1500	/* encoded colours */
#define	PS_SCALE_TEXT	1600	/* scale text */
#define	PS_TEXT_FONT	1700	/* font type */
#define	PS_TEXT_FAMILY	1750	/* font family */
#define	PS_TEXT_STYLE	1770	/* font style */
#define	PS_LINE_WIDTH	1800	/* line width */
#define	PS_LINE_DASH_TOGGLE	1850	/* dash/solid line */
#define	PS_DOT_DASH	1860	/* dot dash line */
#define	PS_PLOT_SYMBOL	1900	/* plot symbol */
#define	PS_SYMBOL_SIZE	1950	/* change symbol size */
#define	PS_SUPER_SCRIPT	2000	/* offset from present position */
#define	PS_SUB_SCRIPT	2100	/* offset from present position */
#define	PS_BACK_SPACE	2200	/* offset from present position */
#define PS_ARC_ANGLES	2300	/* start and end angles for an arc */
#define PS_POLY_END	3100	/* end of a polygon drawing sequence */

	

#define PORTRAIT	0
#define	LANDSCAPE	1

#ifndef MIN
#define MIN(x,y)       (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y)       (((x) < (y)) ? (x) : (y))
#endif

#ifndef streq
#define streq(a, b) ( strcmp((a), (b)) == 0)
#endif

#ifndef strne
#define strne(a, b) ( strcmp((a), (b)) != 0)
#endif

#ifndef streqn
#define streqn(a, b) ( strncmp((a), (b), strlen(b)) == 0)
#endif


#define EIGHT_BIT_SIZE	256   /* clear old colors on 8 bit displays */
static	Display        *dpy;
static	GC              gc;
static	unsigned long GC_ALL = 0x3FFFFF;
static	XGCValues       gcvalues, gctempvalues;
unsigned long		foreground, background;
unsigned long		valuemask;
static	Colormap	colormap;
static	int		display_depth;
static XColor	old_colors[EIGHT_BIT_SIZE];
static int	n_colors = 0;
static	int		xplot0, yplot0;
#define NO_XPOINTS	100
XPoint	xpoints[NO_XPOINTS];	/* general purpose array of xpoints */
int	no_poly_points;		/* keep track of no. of points in polygon */

static	Window		xwin;
/* used in cursor routines */
Window	root;
Window	child;
unsigned int	keys_buttons;

static char ISO[] = "ISO";

/* static	int		canvas_width, canvas_height; */
static	Canvas		canvas;
static	float		xb = 0, yb = 0, xpap, ypap;
static  float		fx, fy;
static	float		xt = 0, yt = 0;
static	Frame		frame;
static	Cursor		cursor, text_cursor, my_cursor, default_cursor;
static	Cursor		blank_cursor;
static	Server_image	svr_image, blank_image;
static	float		xcursor, ycursor;
static  int		cursor_done;
static unsigned int	mask;
static	int	ipress3 = 0;
XEvent report;

#if XV_VERSION_NUMBER < 3000
#define CURSOR_STRING 0
#endif

/* change cursor when it becomes active */

short cursor_bits[] = {

/* Format_version=1, Width=16, Height=16, Depth=1, Valid_bits_per_item=16
 */
	0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0xFFFE,
	0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0000
};

/* a blank cursor */
short blank_cursor_bits[] = {
/* Format_version=1, Width=16, Height=16, Depth=1, Valid_bits_per_item=16
 */
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


/* display an error message if appropriate */
void
toarkt_(char	* buffin)
{
		notice_prompt(frame, 0,
			NOTICE_MESSAGE_STRINGS,
			buffin, 0,
			NOTICE_BUTTON,		"OK", 0,
			0);
}

void store_ps();
void kanji_char();
void angle_text();
void jis_char();
void redraw(), redraw_last();
void incvals();
void rotate(), rotateb(), rotate2(), rotate3();

/* Font information */
void	font_set();
Xv_Font	font;
XFontStruct	*cur_font;
int	the_font_size = 14;
int	old_font_size = 14;
char	*the_font_style = FONT_STYLE_NORMAL;
char	*the_font_family = FONT_FAMILY_ROMAN;

/* font information used in postscript output routine */
static	int	start_font_size = 14;
static	int	current_style_id = 0;
static	int	old_style_id = 0;
static	int	current_family_id = 0;
static	int	old_family_id = 0;
static	int	current_dash_style = SOLID;
static	int	start_dash_style = SOLID;
static	int	start_font_id = 0;
static	int	start_style_id = NORMAL_TEXT;
static	int	start_family_id = TIMES;
static	Bool	used_times = True;
static	Bool	used_helvetica = False;	/* helvetica not used */
static	Bool	used_courier = False;
static	Bool	used_palatino = False;
static	Bool	used_bookman = False;
static	Bool	used_newcentury = False;
static	Bool	used_helvetican = False;
static	Bool	used_avantgarde = False;
static	Bool	used_chancery = False;
static	Bool	used_symbols = False;	/* this refers to special
					symbols for data points, not
					the symbol font */

static	Bool	jis_mode = False;

static	float	current_red = 0;
static	float	current_blue = 0;
static	float	current_green = 0;
static	float	start_red = 0;
static	float	start_blue = 0;
static	float	start_green = 0;
static	float	current_angle = 0.0;
static	float	cos_angle, sin_angle;
static	float	start_angle = 0.0;
static	int	current_width = 0;
static	float	start_width = WIDTH_DEFAULT;
static	float	current_dash[2] = {0., 0.};
static	float	start_dash[2] = {0., 0.};
/* default line properties */
static	int	line_style = LineSolid;
static	int	cap_style = CapButt;
static	int	join_style = JoinRound;
/* size of symbols for post-script and X stuff */
static	int	symbol_size = SYMBOL_SIZE; 
static	int	start_symbol_size = SYMBOL_SIZE;
static	int	arrow_size = ARROW_HEAD_SIZE; /* size of arrow */
static	int	start_arrow_size = ARROW_HEAD_SIZE;
static	int	arrow_style = ARROW_DEFAULT;
static	int	start_arrow_style = ARROW_DEFAULT;
static	int	arrow_head = ARROW_SINGLE;
static	int	start_arrow_head = ARROW_SINGLE;
static	float	arrow_angle = ARROW_HEAD_ANGLE * DTR;
static	float	start_arrow_angle = ARROW_HEAD_ANGLE * DTR;
/* For postscript output */
static	FILE		*fp; /* file for output */
static	char		ps_name[120] = {"temp.ps"};
static	char		ps_title[120] = {""};
static	char		ps_printer[20] = "";
static	int		ps_ncopies = 1;
static	Bool		ps_maintain_scale = TRUE; /* plot will look like
							display */
static	int		ps_direction = PORTRAIT; 
static	int		ps_printwhen = 1; /* 0 = print now, 1= just write file*/
static	float		fx_ps, fy_ps;
static	Bool		do_ps = True;
static	Bool		print_ps = False;
static	Bool		Store_PS = True;
static	Bool		eps = False; /* Don't try to alter output
					file to make EPS use easier */
static	float		xpaper_ps = 612.; /* US Paper size */
static	float		ypaper_ps = 792.; /* US Paper size */
static	float		angle_now = 0.0; /* current angle for text */
static	int		ips_fast = 0;
static	int		ipst_fast = 0;
static	int		ips_slow = 0;
static	int		ipst_slow = 0;
/* names of fonts to be used */
static	char	*font_names[] = {"Times-Roman", "Times-Bold",
		"Times-Italic", "Times-BoldItalic",

		"Helvetica", "Helvetica-Bold",
		"Helvetica-Oblique", "Helvetica-BoldOblique",

		"Courier", "Courier-Bold",
		"Courier-Oblique", "Courier-BoldOblique",

		"Palatino-Roman", "Palatino-Bold",
		"Palatino-Italic", "Palatino-BoldItalic",

		"AvantGarde-Book", "AvantGarde-Demi",
		"AvantGarde-BookOblique", "AvantGarde-DemiOblique",

		"Bookman-Light", "Bookman-Demi",
		"Bookman-LightItalic", "Bookman-DemiItalic",

		"Helvetica-Narrow", "Helvetica-Narrow-Bold",
		"Helvetica-Narrow-Oblique",
			"Helvetica-Narrow-BoldOblique",

		"NewCenturySchlbk-Roman", "NewCenturySchlbk-Bold",
		"NewCenturySchlbk-Italic", "NewCenturySchlbk-BoldItalic",

};

static	char	Symbol[] = "Symbol";
static	char	Kanji[] = "Ryumin-Light-EUC-H";
/* static	char	Kanji[] = "Kanji"; */
static	char	Dingbats[] = "ZapfDingbats";
static	char	Chancery[] = "ZapfChanceryISO";
static	char	font_name[25];
static	int	font_size;
float	ct, st;
static	int	style_id;
static	int	family_id;



/* store maximum plotted values for use in bounding box */
static	float		ps_max_x = 0.0;
static	float		ps_max_y = 0.0;
static	float		ps_min_x = BIG_NUMBER;
static	float		ps_min_y = BIG_NUMBER;
struct	postscript
{
	float		x[PS_FAST], y[PS_FAST];
	short		instruction[PS_FAST];
};

struct	text_for_ps
{
	char		text[TEXT_LENGTH];
};

struct ps_text
{
	struct		text_for_ps text_ps[PST_FAST];

};

struct super_ps
{
	Bool	GoodPlot;

	int	number_ps;
	int	ips_slow;
	int	ipst_slow;
	int	ips_fast;
	int	ipst_fast;
	float	fx_ps;
	float	fy_ps;
	float	fx;
	float	fy;
	float	xb;
	float	yb;
	float	xt;
	float	yt;
	float	ypap;

	GC	gc;
	unsigned long	foreground; /* (shouldn't really need this I think! */
	Canvas	canvas;
	/* XFontStruct	*cur_font; */

	XColor	old_colors[EIGHT_BIT_SIZE];
	int	n_colors;
	
	Bool	used_times;
	Bool	used_helvetica;
	Bool	used_courier;
	Bool	used_palatino;
	Bool	used_bookman;
	Bool	used_newcentury;
	Bool	used_helvetican;
	Bool	used_avantgarde;
	Bool	used_chancery;
	Bool	used_symbols;

	char	*the_font_style;
	char	*the_font_family;

 	int	current_style_id;
	int	current_family_id;
	int	current_dash_style;
	int	current_red;
	int	current_green;
	int	current_blue;
	int	the_font_size;
	int	current_width;
	float	current_angle;
	int	symbol_size;
	int	arrow_size;
	float	arrow_angle;
	int	arrow_style;
	int	arrow_head;



	float	start_angle;
	int	start_font_size;
	int	start_dash_style;
	int	start_font_id;
	int	start_style_id;
	int	start_family_id;
	float	start_red;
	float	start_blue;
	float	start_green;
	float	start_width;
	float	start_dash[2];
	int	start_symbol_size;
	int	start_arrow_size;
	int	start_arrow_style;
	int	start_arrow_head;
	float	start_arrow_angle;
	struct postscript	*ps_ptr[PS_SLOW];
	struct ps_text		*pst_ptr[PST_SLOW];
};


/* static	int		number_ps = 0; */
static	int		total_plot_number = -1; /* total no. of plots done */
static	int		plot_number; /* index to present plot */

/* static	struct	postscript	*ps_ptr[PS_SLOW];
static	struct	ps_text		*pst_ptr[PST_SLOW]; */
#define	MAX_PLOTS	256
static	struct	super_ps	*sps_ptr[MAX_PLOTS];

static	char	old_text[TEXT_LENGTH]; /* cursor position for footer */

/* various routines for allocating, freeing, setting and
 * reading sturctures which hold plotting ("PostScript" information)
 */



void
save_super_ps(void)
{
int	j;
/* is plot_number valid? */
	if(plot_number < 0) return;
/* is the pointer valid? */
	if(sps_ptr[plot_number] == 0) return;

	if(sps_ptr[plot_number]->GoodPlot != TRUE) return;


	sps_ptr[plot_number]->ips_slow = ips_slow;
	sps_ptr[plot_number]->ipst_slow = ipst_slow;
	sps_ptr[plot_number]->ips_fast = ips_fast;
	sps_ptr[plot_number]->ipst_fast = ipst_fast;

	sps_ptr[plot_number]->fx_ps = fx_ps;
	sps_ptr[plot_number]->fy_ps = fy_ps;
	sps_ptr[plot_number]->fx = fx;
	sps_ptr[plot_number]->fy = fy;
	sps_ptr[plot_number]->xb = xb;
	sps_ptr[plot_number]->yb = yb;
	sps_ptr[plot_number]->xt = xt;
	sps_ptr[plot_number]->yt = yt;
	sps_ptr[plot_number]->ypap = ypap;

	XCopyGC(dpy, gc, GC_ALL, sps_ptr[plot_number]->gc);

	sps_ptr[plot_number]->foreground = foreground;


	sps_ptr[plot_number]->canvas = canvas;

	/* sps_ptr[plot_number]->cur_font = cur_font; */


	/*sps_ptr[plot_number]->n_colors = n_colors;

	for(j = 0; j < n_colors; j++)
		sps_ptr[plot_number]->old_colors[j] = old_colors[j]; */

	sps_ptr[plot_number]->used_times = used_times;
	sps_ptr[plot_number]->used_helvetica = used_helvetica;
	sps_ptr[plot_number]->used_courier = used_courier;
	sps_ptr[plot_number]->used_palatino = used_palatino;
	sps_ptr[plot_number]->used_bookman = used_bookman;
	sps_ptr[plot_number]->used_newcentury = used_newcentury;
	sps_ptr[plot_number]->used_helvetican = used_helvetican;
	sps_ptr[plot_number]->used_avantgarde = used_avantgarde;
	sps_ptr[plot_number]->used_chancery = used_chancery;
	sps_ptr[plot_number]->used_symbols = used_symbols;


	sps_ptr[plot_number]->the_font_style = the_font_style;
	sps_ptr[plot_number]->the_font_family = the_font_family;

	sps_ptr[plot_number]->current_style_id = current_style_id;
	sps_ptr[plot_number]->current_family_id = current_family_id;
	sps_ptr[plot_number]->current_dash_style = current_dash_style;
	sps_ptr[plot_number]->current_red = current_red;
	sps_ptr[plot_number]->current_green = current_green;
	sps_ptr[plot_number]->current_blue = current_blue;
	sps_ptr[plot_number]->the_font_size = the_font_size;
	sps_ptr[plot_number]->current_width = current_width;
	sps_ptr[plot_number]->current_angle = current_angle;
	sps_ptr[plot_number]->symbol_size = symbol_size;
	sps_ptr[plot_number]->arrow_size = arrow_size;
	sps_ptr[plot_number]->arrow_angle = arrow_angle;
	sps_ptr[plot_number]->arrow_style = arrow_style;
	sps_ptr[plot_number]->arrow_head = arrow_head;




	sps_ptr[plot_number]->start_font_size = start_font_size;
	sps_ptr[plot_number]->start_dash_style = start_dash_style;
	sps_ptr[plot_number]->start_font_id = start_font_id;
	sps_ptr[plot_number]->start_style_id = start_style_id;
	sps_ptr[plot_number]->start_family_id = start_family_id;
	sps_ptr[plot_number]->start_red = start_red;
	sps_ptr[plot_number]->start_green = start_green;
	sps_ptr[plot_number]->start_blue = start_blue;
	sps_ptr[plot_number]->start_width = start_width;
	sps_ptr[plot_number]->start_dash[0] = start_dash[0];
	sps_ptr[plot_number]->start_dash[1] = start_dash[1];
	sps_ptr[plot_number]->start_symbol_size = start_symbol_size;
	sps_ptr[plot_number]->start_arrow_size = start_arrow_size;
	sps_ptr[plot_number]->start_arrow_style = start_arrow_style;
	sps_ptr[plot_number]->start_arrow_angle = start_arrow_angle;
	sps_ptr[plot_number]->start_arrow_size = start_arrow_size;
}
void
load_super_ps(int i)
{	
int	j;


	ips_slow = sps_ptr[plot_number]->ips_slow;
	ipst_slow = sps_ptr[plot_number]->ipst_slow;
	ips_fast = sps_ptr[plot_number]->ips_fast;
	ipst_fast = sps_ptr[plot_number]->ipst_fast;


	fx_ps = sps_ptr[i]->fx_ps;
	fy_ps = sps_ptr[i]->fy_ps;
	fx = sps_ptr[i]->fx;
	fy = sps_ptr[i]->fy;
	xb = sps_ptr[i]->xb;
	yb = sps_ptr[i]->yb;
	xt = sps_ptr[i]->xt;
	yt = sps_ptr[i]->yt;
	ypap = sps_ptr[i]->ypap;

	XCopyGC(dpy, sps_ptr[i]->gc, GC_ALL, gc);
	foreground = sps_ptr[i]->foreground;
	canvas = sps_ptr[i]->canvas;

	xwin = xv_get(canvas_paint_window(canvas), XV_XID);
	frame = xv_get(canvas, XV_OWNER);

	/*n_colors = sps_ptr[i]->n_colors;

	for(j = 0; j < n_colors; j++)
		old_colors[j] = sps_ptr[i]->old_colors[j];*/

	used_times = sps_ptr[i]->used_times;
	used_helvetica = sps_ptr[i]->used_helvetica;
	used_courier = sps_ptr[i]->used_courier;
	used_palatino = sps_ptr[i]->used_palatino;
	used_bookman = sps_ptr[i]->used_bookman;
	used_newcentury = sps_ptr[i]->used_newcentury;
	used_helvetican = sps_ptr[i]->used_helvetican;
	used_avantgarde = sps_ptr[i]->used_avantgarde;
	used_chancery = sps_ptr[i]->used_chancery;
	used_symbols = sps_ptr[i]->used_symbols;


	current_style_id = sps_ptr[i]->current_style_id;
	current_family_id = sps_ptr[i]->current_family_id;
	current_dash_style = sps_ptr[i]->current_dash_style;
	current_red = sps_ptr[i]->current_red;
	current_green = sps_ptr[i]->current_green;
	current_blue = sps_ptr[i]->current_blue;
	the_font_size = sps_ptr[i]->the_font_size;
	current_width = sps_ptr[i]->current_width;
	current_angle = sps_ptr[i]->current_angle;
	symbol_size = sps_ptr[i]->symbol_size;
	arrow_size = sps_ptr[i]->arrow_size;
	arrow_angle = sps_ptr[i]->arrow_angle;
	arrow_style = sps_ptr[i]->arrow_style;
	arrow_head = sps_ptr[i]->arrow_head;

	the_font_style = sps_ptr[i]->the_font_style;
	the_font_family = sps_ptr[i]->the_font_family;

	start_font_size = sps_ptr[i]->start_font_size;
	start_dash_style = sps_ptr[i]->start_dash_style;
	start_font_id = sps_ptr[i]->start_font_id;
	start_style_id = sps_ptr[i]->start_style_id;
	start_family_id = sps_ptr[i]->start_family_id;
	start_red = sps_ptr[i]->start_red;
	start_green = sps_ptr[i]->start_green;
	start_blue = sps_ptr[i]->start_blue;
	start_width = sps_ptr[i]->start_width;
	start_dash[0] = sps_ptr[i]->start_dash[0];
	start_dash[1] = sps_ptr[i]->start_dash[1];
	start_symbol_size = sps_ptr[i]->start_symbol_size;
	start_arrow_size = sps_ptr[i]->start_arrow_size;
	start_arrow_style = sps_ptr[i]->start_arrow_style;
	start_arrow_angle = sps_ptr[i]->start_arrow_angle;
	start_arrow_size = sps_ptr[i]->start_arrow_size;
}

Bool
ps_alloc(int i)
{
	sps_ptr[plot_number]->ps_ptr[i] = (struct postscript *)
			calloc(1, sizeof(struct postscript));
	if(sps_ptr[plot_number]->ps_ptr[i] == 0)
		return(FALSE);
	else
		return(TRUE);
}

Bool
ps_text_alloc(int i)
{
	sps_ptr[plot_number]->pst_ptr[i] = (struct ps_text *)
			calloc(1, sizeof(struct ps_text));
	if(sps_ptr[plot_number]->pst_ptr[i] == 0)
		return(FALSE);
	else
		return(TRUE);
}

Bool
ps_free(int i)
{
	free((char *) sps_ptr[plot_number]->ps_ptr[i]);
}

Bool
ps_text_free(int i)
{
	free((char *) sps_ptr[plot_number]->pst_ptr[i]);
}


int
get_ps_number(void)
{
	return(sps_ptr[plot_number]->number_ps);
}

void
reset_ps_number(void)
{
	sps_ptr[plot_number]->number_ps = 0;
	return;
}

void
increase_ps_number(void)
{
	sps_ptr[plot_number]->number_ps++;
	return;
}


Bool
get_ps_values(int islow, int ifast, float	* xout, float	 *yout, short	* instruction)
{
	*xout = sps_ptr[plot_number]->ps_ptr[islow]->x[ifast]; 
	*yout = sps_ptr[plot_number]->ps_ptr[islow]->y[ifast];
	*instruction = sps_ptr[plot_number]->ps_ptr[islow]->instruction[ifast];
	return (TRUE);
}

Bool
set_ps_values(int islow, int ifast, float xout, float yout, short instruction)
{
	sps_ptr[plot_number]->ps_ptr[islow]->x[ifast] = xout; 
	sps_ptr[plot_number]->ps_ptr[islow]->y[ifast] = yout;
	sps_ptr[plot_number]->ps_ptr[islow]->instruction[ifast] = instruction;
	return (TRUE);
}

Bool
set_ps_text_value(int islow, int ifast, char	* text)
{
	strcpy(sps_ptr[plot_number]->pst_ptr[islow]->text_ps[ifast].text, text);
	return (TRUE);
}

char *
get_ps_text_value(int islow, int ifast)
{
	return(sps_ptr[plot_number]->pst_ptr[islow]->text_ps[ifast].text);
}

Bool
set_ps_text_value_n(int islow, int ifast, char	* text, int n)
{
	strncpy(sps_ptr[plot_number]->pst_ptr[islow]->text_ps[ifast].text, text, n);
	return (TRUE);
}
/* for later use */
/* 
void
resize_proc(Canvas canvas_draw, int the_width, int the_height)
{
	return;
}


*/


/* Write out changes in Fonts which take place inside a "RAW_TEXT" string */
void
ps_tfont(int id)
{

			if(id == KANJI_ID)
				strcpy(font_name, Kanji);
			else if(id == SYMBOL_ID)
				strcpy(font_name, Symbol);
			else if(id == DINGBATS_ID)
				strcpy(font_name, Dingbats);
			else if(id == CHANCERY_ID)
				strcpy(font_name, Chancery);
			else{
				if(id >= 0)
				  sprintf(font_name, "%s%s",
					font_names[id], ISO);
				else{
				  fprintf(stderr, "Silly font id: %d\n",
						id);
				  return;
				}
			}
			fprintf(fp, "/%s findfont\n%d scalefont\n",
					font_name, font_size);
/* and set to the correct angle */			
			ct = cos(angle_now);
			st = sin(angle_now);			
			  fprintf(fp, "[ %.1f %.1f %.1f %.1f 0 0] makefont\n",
				  ct, st, 
				  -st, ct);
			fprintf(fp,"setfont\n");
}

void
ps_font_family(int family)
{
int	id;
	if(family == family_id) return;

	family_id = family;

	if(family_id == KANJI)
		id = KANJI_ID;
	else if(family_id == SYMBOL)
		id = SYMBOL_ID;
	else if(family_id == DINGBATS)
		id = DINGBATS_ID;
	else if(family_id == CHANCERY)
		id = CHANCERY_ID;
	else
		id = family_id * NO_OF_STYLES
			+ style_id;
	ps_tfont(id);
}

void
ps_font_style(int style)
{
int	id;
	if(style == style_id) return;

	style_id = style;

	if(family_id == KANJI)
		id = KANJI_ID;
	else if(family_id == SYMBOL)
		id = SYMBOL_ID;
	else if(family_id == DINGBATS)
		id = DINGBATS_ID;
	else if(family_id == CHANCERY)
		id = CHANCERY_ID;
	else
		id = family_id * NO_OF_STYLES
			+ style_id;
	ps_tfont(id);
}


/* PostScript change in colour */
void
ps_cout(int red, int green, int blue)
{
	fprintf(fp, "%.2f %.2f %.2f setrgbcolor\n",
	(float) red/256., (float) green/256., (float) blue/256.);
}


/* replacement for PS_TEXT_CONTINUE */
void
ps_tout(char           * text, int length)
{
	int             j;
	char            buf[TEXT_LENGTH];
	if (!print_ps || length <= 0)
		return;
	strncpy(buf, text, length);
	fprintf(fp, "(");
	for (j = 0; j < length; j++) {
		if (buf[j] == '(' || buf[j] == ')')
			fprintf(fp, "\\%c", buf[j]);
		else
			fprintf(fp, "%c", buf[j]);
	}
	fprintf(fp, ") s\n");
}




void
psout(int instruction)
{
		if(instruction == SUPER_SCRIPT){
			fprintf(fp, "%.1f %.1f rmoveto\n",
				-0.5*font_size*sin(angle_now),
				0.5*font_size*cos(angle_now));
		}

		else if(instruction == SUB_SCRIPT){
			fprintf(fp, "%.1f %.1f rmoveto\n",
				0.5*font_size*sin(angle_now),
				-0.5*font_size*cos(angle_now));
		}

		else if(instruction == BACK_SPACE){
			fprintf(fp, "%.1f %.1f rmoveto\n",
				-0.5*font_size*cos(angle_now),
				-0.5*font_size*sin(angle_now));
			/* fprintf(fp, "\\b\n"); */
		}
		else
			fprintf(stderr, 
				"Error: unknown instruction %d in psout\n",
				instruction);
}


void
find_text(ix, iy, found_text, text, xfound, yfound, isf, iff, itf, itff)
int	ix, iy;
int	*isf, *iff, *itf, *itff; /* array indices of any text found */
Bool	*found_text;
char	*text;
float	*xfound, *yfound;
{
	float           xout, yout;
	float		xlow;
	static float	scale = 0.5;
	short           instruction;
	int		len;
	float           min_dist, dist;
	float		xf, yf, x_value, y_value;
	int             i, ifast, islow, itext, itext_fast;
	float		test;
	int		number_ps;




	if(fx != 0.0 && fy != 0.0){
		xf = (float) ix / fx;
		yf = (ypap - (float) iy) / fy ;
		x_value = xf  * fx_ps;
		y_value = yf  * fy_ps;


/* initial values of xfound and yfound are just input coordinates
 * in plot coordinates */

	*xfound = xf + xb;
	*yfound = yf + yb;

	}
	else
		return;


	min_dist = 100000.;
	i = ifast = islow = 0;
	itext = itext_fast = 0;
	*found_text = False;
	test = (float) start_font_size;
	number_ps = get_ps_number();
	while (i < number_ps) {
		get_ps_values(islow, ifast, &xout, &yout, &instruction);
		if(instruction == PS_SCALE_TEXT){
			test = xout;
		}
		if (instruction == PS_RAW_TEXT ||
			instruction == PS_TEXT_RIGHT ||
			instruction == PS_TEXT_CENTRE) {
			len = strlen(get_ps_text_value(itext, itext_fast));
			if(instruction == PS_TEXT_CENTRE)
				xlow = xout - len*test*scale*0.5;
			else if(instruction == PS_TEXT_RIGHT)
				xlow = xout - len*test*scale;
			else
				xlow = xout;
			if ((x_value > xlow) &&
				fabs(xlow - x_value) < len*test*scale &&
				(y_value > yout) 
				&&( fabs(yout - y_value) < test) ){
				dist = ((xlow - x_value) *
					(xlow - x_value)) +
					((yout - y_value) *
					(yout - y_value));
				if(dist < min_dist){
				   *found_text = True;
				   *xfound = xout/fx_ps + xb;
				   *yfound = yout/fy_ps + yb;
				   min_dist = dist;
				   strcpy(text,
				     get_ps_text_value(itext,itext_fast));
				   *isf = islow;
				   *iff = ifast;
				   *itf = itext;
				   *itff = itext_fast;
				}
			}
		}
		if (instruction == PS_TEXT ||
		    instruction == PS_TEXT_CONTINUE ||
		    instruction == PS_TEXT_RIGHT ||
		    instruction == PS_TEXT_CENTRE ||
		    instruction == PS_RAW_TEXT)
			incvals(&itext_fast, &itext);
		ifast++;
		if (ifast >= PS_FAST) {
			ifast = ifast - PS_FAST;
			islow++;
		}
		i++;
	}
}

/* ADD ERROR CHECKING!!! */
/* replace an exisiting string with a new one */
void
replace_text(int is, int ifst, int it, int itf, float xpos, float ypos, char	* text)
{
short	instruction;
int	ilen;
float	xval, yval;



	if(ifst >= PS_FAST){
		fprintf(stderr, "Error - fast index too large\n");
		return;
	}
	if(is > ips_slow){
		fprintf(stderr, "Error - slow index too large\n");
		return;
	}
	get_ps_values(is, ifst, &xval, &yval, &instruction);

	if(instruction != PS_TEXT &&
		instruction != PS_TEXT_CONTINUE &&
		instruction != PS_TEXT_RIGHT &&
		instruction != PS_TEXT_CENTRE &&
		instruction != PS_RAW_TEXT){
		 fprintf(stderr, "Error - not a text instruction\n");
		 return;
	}

	/* update position of text */
	xval = (xpos - xb) * fx_ps;
	yval = (ypos - yb) * fy_ps;
	set_ps_values(is, ifst, xval, yval, instruction);


	/* update text string */
	set_ps_text_value(it, itf, text);

/* modify stored length for raw text */
	if(instruction != PS_RAW_TEXT) return;
	ifst = ifst - 1;
	if(ifst < 0){
		is = is - 1;
		ifst = PS_FAST - 1;
	}
	if(ifst > PS_FAST ||
	   is > PS_SLOW ||
	   is < 0 ) {
		fprintf(stderr, "Error: incorrect indicies in replace_text\n");
		return;
	}
	else
		get_ps_values(is, ifst, &xval, &yval, &instruction);
		if(instruction != PS_RAW_NCHAR){
		   fprintf(stderr,
			"Error: previous instruction not PS_RAW_NCHAR\n");
		   return;
		}
		else {
		   xval = (float) strlen(text);
		   set_ps_values(is, ifst, xval, xval, instruction);
		}


}


/* dummy to eventually save existing plot instructions */
void
ark_save_data(int index)
{

}

/* dummy to eventually plot saved plot instructions */
void
ark_printn(int index)
{
int	plot_number_save;
void	psprint_();
	

	save_super_ps();

	plot_number_save = plot_number;
	plot_number = index - 1;

	load_super_ps(plot_number);


	psprint_();

	plot_number = plot_number_save;
	load_super_ps(plot_number);
}

Bool
ark_kill_plot(int index)
{
int	i, j;

	i = index - 1;
	if(i < 0 || i > MAX_PLOTS){
		fprintf(stderr, "ERROR: Invalid plot number: %d\n", index);
		return(FALSE);
	}

	if(sps_ptr[i] == 0){
		fprintf(stderr, "ERROR: Plot not allocated\n");
		return(FALSE);
	}

	if(sps_ptr[i]->GoodPlot != TRUE){
		fprintf(stderr, "ERROR: Plot no good\n");
		return(FALSE);
	}

	sps_ptr[i]->GoodPlot = FALSE;

	j = 0;
	while(sps_ptr[i]->ps_ptr[j] != 0 && j < PS_SLOW){
		free((char *) sps_ptr[i]->ps_ptr[j]);
		j++;
	}

	j = 0;
	while(sps_ptr[i]->pst_ptr[j] != 0 && j < PST_SLOW){
		free((char *) sps_ptr[i]->pst_ptr[j]);
		j++;
	}

	xv_destroy_safe(xv_get(sps_ptr[i]->canvas, XV_OWNER));

	free((char *) sps_ptr[i]);
	/* printf("should have freed plot no. %d\n", i);
	if(sps_ptr[i] != (struct super_ps *) 0){
		printf("but not null!!!\n");
	} */


	return(TRUE);
}



void
pltopn(Frame frame_in, Canvas canvas_in)
{
XWMHints	wmhints;
	void xnewpage_(), ZeroPixmap();

	/* save various attributes of previous plot for later reference */
	save_super_ps();
	total_plot_number++;
	if(total_plot_number >= MAX_PLOTS){
		printf("ERROR: Too many plots created!\n");
		printf("Only %d allowed in total\n", MAX_PLOTS);
		return;
	}
	plot_number = total_plot_number;
	sps_ptr[plot_number] = (struct super_ps *)
			calloc(1, sizeof(struct super_ps));
	if(sps_ptr[plot_number] == 0)
		printf("ERROR ALLOCATING SUPER_PS\n");
	sps_ptr[plot_number]->number_ps = 0;
	sps_ptr[plot_number]->GoodPlot = TRUE;


	canvas = canvas_in;
	frame = frame_in;

	dpy = (Display *) xv_get(frame, XV_DISPLAY);
	gcvalues.foreground = BlackPixel(dpy, DefaultScreen(dpy));
	gcvalues.background = WhitePixel(dpy, DefaultScreen(dpy));
	foreground = gcvalues.foreground;
	background = gcvalues.background;
	gc = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		       (GCForeground | GCBackground), &gcvalues);
	sps_ptr[plot_number]->gc = 
		XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		       (GCForeground | GCBackground), &gcvalues);
/* does this speed up XCopyArea? */
	XSetGraphicsExposures(dpy, gc, False);
	if(total_plot_number == 0)
		colormap = DefaultColormap(dpy, DefaultScreen(dpy));

	xwin = xv_get(canvas_paint_window(canvas), XV_XID);


/* Will this help with focus? */
	wmhints.flags = InputHint;
	wmhints.input = True;
	XSetWMHints(dpy, xwin, &wmhints);

/* what depth is the display */
	display_depth = DefaultDepth(dpy, 0);


/* default is to use the left button for the cursor */
	mask = Button1Mask;


/* allocate the cursor shape after first storing the initial 
 * default cursor */
	default_cursor = (Xv_cursor) xv_get(xv_get(canvas,
		CANVAS_NTH_PAINT_WINDOW, 0), WIN_CURSOR);
	svr_image = (Server_image)xv_create (XV_NULL, SERVER_IMAGE,
		XV_WIDTH, 	16,
		XV_HEIGHT, 	16,
		SERVER_IMAGE_BITS,	cursor_bits,
		0);

	cursor = (Xv_Cursor)xv_create(XV_NULL, CURSOR,
			CURSOR_IMAGE,	svr_image,
			CURSOR_XHOT,	8,
			CURSOR_YHOT,	8,
			0);

	blank_image = (Server_image)xv_create (XV_NULL, SERVER_IMAGE,
		XV_WIDTH, 	16,
		XV_HEIGHT, 	16,
		SERVER_IMAGE_BITS,	blank_cursor_bits,
		0);

	blank_cursor = (Xv_Cursor)xv_create(XV_NULL, CURSOR,
			CURSOR_IMAGE,	blank_image,
			CURSOR_XHOT,	8,
			CURSOR_YHOT,	8,
			0);

/* default cursor to use is the one defined above */
	my_cursor = cursor;

/* install a resize procedure */
	xv_set(canvas, 
		CANVAS_REPAINT_PROC, redraw_last,
		CANVAS_X_PAINT_WINDOW, TRUE,
		0);

	xv_set(canvas_paint_window(canvas), 
		WIN_CONSUME_EVENTS, WIN_MOUSE_BUTTONS, 0, 
		WIN_IGNORE_X_EVENT_MASK, ButtonReleaseMask,
		0);


/* allocate a structure to store plot instructions 
 * (nominally for postscript output)	
 */

	xnewpage_();


/* and define font to be used */
	font_set();
/* some additional set-ups from FORTRAN */
	fsetup_();
}


/* This is for doing initial open windows set-ups
 * for those who don't want to do that themselves
 */
void
arkopn_(void)
{
static	char	*argv[1];
static	int	argc;
	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv,
				0);
	frame = (Frame) xv_create(0, FRAME,
				  0);
	canvas = (Canvas) xv_create(frame, CANVAS,
				  WIN_X,    0,	
				  XV_WIDTH, 600,
				  XV_HEIGHT, 600,
				  0);
	window_fit(frame);	

	pltopn(frame, canvas);
	xv_set(frame, XV_SHOW, TRUE, 0);
}
void
startps_(void)
{
	do_ps = True;
}

void
haltps_(void)
{
	do_ps = False;
}


/* Say what the current plot colour is */
void
what_colour(int	* red, int	 *green, int	 *blue)
{
	*red = current_red;
	*blue = current_blue;
	*green = current_green;
}


/* FORTRAN call for colour change */
void
farkc_(int * red, int *green, int *blue)
{
void	ark_colour();

	ark_colour(*red, *green, *blue);
}

void
ark_colour(int red, int green, int blue)
{
XColor		my_color;
void		ps_colour();


/* if colors haven't changed return */
	if(red == current_red &&
	   blue == current_blue &&
	   green == current_green) return;

/* Make sure colours are within allowed limits */
	

	my_color.red = red << 8;
	my_color.blue = blue << 8;
	my_color.green = green << 8;

/* change postscript colour? */
	if (do_ps && Store_PS) ps_colour(red, green, blue);
	current_red = red;
	current_green = green;
	current_blue = blue;

/* only set X colour if display depth seems to make sense
 * for this */
	if(display_depth < 2) return;
	if (XAllocColor (dpy, colormap, &my_color) == 0){
		fprintf(stderr, "color allocation failed \n");
		return;
		}

	XSetForeground(dpy, gc, my_color.pixel);
	foreground = my_color.pixel;

	old_colors[n_colors] = my_color;
	n_colors++;
	if(n_colors >= EIGHT_BIT_SIZE) n_colors = EIGHT_BIT_SIZE - 1;
}



/* A FORTRAN handle for font_resize */
void
fsize_(float	* fs)
{
int	fsi;
void	font_resize();
	fsi = (int) *fs;
	font_resize(fsi);
}


void
font_resize(int size)
{
/* exit if font size unchanged */
	if(size == the_font_size)
		return;

	the_font_size = size;
	font_set();	
	if (do_ps) store_ps((float) size, (float) size,  PS_SCALE_TEXT);
}


/* FORTRAN handle for line width */
void
fwidth_(int	* ws)
{
void	set_line_width();
	set_line_width(*ws);
}

void
set_line_width(int line_width)
{
/* return if no change from present value */
	if(current_width == line_width) return;
	XSetLineAttributes(dpy, gc,
		line_width,
		line_style,
		cap_style,
		join_style);
	current_width = line_width;
	if (do_ps) 
	    store_ps((float) line_width, (float) line_width,  PS_LINE_WIDTH);
	 
}		


void
gcxor_()
/* this used to be a straight call to XSetFunction(dpy, gc, GXxor)
 * The fg ^ bg was copied from xfig after seeing that that worked,
 * but robot didn't under LINUX */
{
	gctempvalues.function = GXxor;
	gctempvalues.foreground = foreground ^ background;
	valuemask = GCForeground | GCFunction;
	
	XChangeGC(dpy, gc, valuemask, &gctempvalues);
}

void
gccopy_(void)
{
	gctempvalues.function = GXcopy;
	gctempvalues.foreground = foreground;
	valuemask = GCForeground | GCFunction;
	XChangeGC(dpy, gc, valuemask, &gctempvalues);
}

void
gcerase_(void)
{
	gctempvalues.function = GXcopy;
	gctempvalues.foreground = background;
	valuemask = GCForeground | GCFunction;
	XChangeGC(dpy, gc, valuemask, &gctempvalues);
}





void
set_line_dotdash(void)
{
#define DOT_DASHED_LIST_LENGTH 4
static unsigned char dot_dashed[] =
{4, 4, 1, 4};
static int dash_offset = 0;

		current_dash_style = DOTDASH;
		line_style = LineOnOffDash;
		current_dash[0] = 1;
		current_dash[1] = 3;
	XSetDashes(dpy, gc, dash_offset, dot_dashed, DOT_DASHED_LIST_LENGTH);
	XSetLineAttributes(dpy, gc,
		current_width,
		line_style,
		cap_style,
		join_style);
	if (do_ps) store_ps(1.0, 3.0, PS_DOT_DASH);
	 
}		

void
dotdash_(void)
{
	set_line_dotdash();
}

void
set_line_dash(void)
{
#define DASHED_LIST_LENGTH 2
static unsigned char dashed[] =
{4, 4};
static int dash_offset = 0;
		current_dash_style = DASH;
		line_style = LineOnOffDash;
		current_dash[0] = 4;
		current_dash[1] = 4;
	XSetDashes(dpy, gc, dash_offset, dashed, DASHED_LIST_LENGTH);
	XSetLineAttributes(dpy, gc,
		current_width,
		line_style,
		cap_style,
		join_style);
	if (do_ps) store_ps((float) current_dash[0],
		(float) current_dash[1], PS_LINE_DASH_TOGGLE);
	 
}		

void
dodash_(void)
{
	set_line_dash();
}

void
set_line_dot(void)
{
#define DOTTED_LIST_LENGTH 2
static unsigned char dotted[] =
{1, 4};
static int dash_offset = 0;
		current_dash_style = DASH;
		line_style = LineOnOffDash;
		current_dash[0] = .4;
		current_dash[1] = 4;
	XSetDashes(dpy, gc, dash_offset, dotted, DOTTED_LIST_LENGTH);
	XSetLineAttributes(dpy, gc,
		current_width,
		line_style,
		cap_style,
		join_style);
	if (do_ps) store_ps((float) current_dash[0],
		(float) current_dash[1], PS_LINE_DASH_TOGGLE);
	 
}		

void
dodot_(void)
{
	set_line_dot();
}

void
set_line_nodash(void)
{
		current_dash_style = SOLID;
		line_style = LineSolid;
		current_dash[0] = 0;
		current_dash[1] = 0;
	XSetLineAttributes(dpy, gc,
		current_width,
		line_style,
		cap_style,
		join_style);
	if (do_ps) store_ps((float) current_dash[0],
		(float) current_dash[1], PS_LINE_DASH_TOGGLE);
	 
}		

void
nodash_(void)
{
	set_line_nodash();
}


void
font_style(int style)
{
/* if the current style is the same as the one requested 
 * just return */

	if(style == current_style_id) return;


	current_style_id = style;
	if(style == NORMAL_TEXT) the_font_style = FONT_STYLE_NORMAL;
	else if(style == BOLD_TEXT) the_font_style = FONT_STYLE_BOLD;
	else if(style == ITALIC_TEXT) the_font_style = FONT_STYLE_OBLIQUE;
	else if(style == BOLD_ITALIC_TEXT) 
		the_font_style = FONT_STYLE_BOLD_OBLIQUE;
	else{
		fprintf(stderr, "Unrecognised style number: %d", style);
		return;
	}

	font_set();
/* set up the Font ID for PostScript output */
	if(do_ps && Store_PS)
		store_ps((float) current_style_id,
				(float) current_style_id,
				PS_TEXT_STYLE);


}

/* A FORTRAN handle for font_style */
void
fstyle_(int	* fs)
{
	font_style(*fs);
}

void
font_family(int family)
{
/* if the current style is the same as the one requested 
 * just return */
	if(family == current_family_id) return;


	current_family_id = family;
	if(family == TIMES){
		used_times = True;
		the_font_family = FONT_FAMILY_ROMAN;
	}
	else if(family == HELVETICA){
		used_helvetica = True;
		the_font_family = FONT_FAMILY_HELVETICA;
	}
	else if(family == COURIER){
		used_courier = True;
		the_font_family = FONT_FAMILY_COUR;
	}
	else if(family == SYMBOL){
		 the_font_family = FONT_FAMILY_SYMBOL;
	}
	else if(family == KANJI){
		 the_font_family = FONT_FAMILY_KANJI;
	}
	else if(family == DINGBATS){
		 the_font_family = FONT_FAMILY_DINGBATS;
	}
	else if(family == CHANCERY){
		 used_chancery = True;
		 the_font_family = FONT_FAMILY_CHANCERY;
	}
	else if(family == PALATINO){
		 used_palatino = True;
		 the_font_family = FONT_FAMILY_PALATINO;
	}
	else if(family == AVANTGARDE){
		 used_avantgarde = True;
		 the_font_family = FONT_FAMILY_AVANTGARDE;
	}
	else if(family == BOOKMAN){
		 used_bookman = True;
		 the_font_family = FONT_FAMILY_BOOKMAN;
	}
	else if(family == HELVETICAN){
		 used_helvetican = True;
		 the_font_family = FONT_FAMILY_HELVETICAN;
	}
	else if(family == NEWCENTURY){
		 used_newcentury = True;
		 the_font_family = FONT_FAMILY_NEWCENTURY;
	}
	else{
		fprintf(stderr, "Error: No such font family ID: %d\n",
				family);
	}

	font_set();
/* set up the Font ID for PostScript output */

	if(do_ps && Store_PS)
		store_ps((float) current_family_id,
				(float) current_family_id,
				PS_TEXT_FAMILY);
}

/* A FORTRAN handle for font_family */
void
ffam_(int	* ff)
{
	font_family(*ff);
}

void
font_set(void)
{
char	style[40];
char	fn[128];
void	set_font_name();
	/* allowance for Symbol font is a kludge */
	/* change if OpenWindows symbol font becomes available */
	cur_font = 0;
	if streq(the_font_family, FONT_FAMILY_SYMBOL){
	  if(the_font_size == 10)
	   cur_font = XLoadQueryFont(dpy,
	"-adobe-symbol-medium-r-normal--10-100-75-75-p-61-adobe-fontspecific");
	  else if(the_font_size == 12)
	   cur_font = XLoadQueryFont(dpy, 
	"-adobe-symbol-medium-r-normal--12-120-75-75-p-74-adobe-fontspecific");
	  else if(the_font_size == 14)
	   cur_font = XLoadQueryFont(dpy, 
	"-adobe-symbol-medium-r-normal--14-140-75-75-p-85-adobe-fontspecific");
	  else if(the_font_size == 18)
	   cur_font = XLoadQueryFont(dpy, 
	"-adobe-symbol-medium-r-normal--18-180-75-75-p-107-adobe-fontspecific");
	  else if(the_font_size == 24)
	   cur_font = XLoadQueryFont(dpy, 
	"-adobe-symbol-medium-r-normal--24-240-75-75-p-142-adobe-fontspecific");
	  else{
	        sprintf(fn, "%s-%d", "symbol", 
					the_font_size);
		cur_font = XLoadQueryFont(dpy, fn);
	  }
		
/* if we didn't manage to set that font use a "generic" symbol font */
	  if(cur_font == 0)
		cur_font = XLoadQueryFont(dpy, "Symbol");
	}
	else if streq(the_font_family, FONT_FAMILY_KANJI){
	        sprintf(fn, "%s-%d", "kanji", 
					the_font_size);
		cur_font = XLoadQueryFont(dpy, fn);
		if(cur_font == 0){
		   fprintf(stderr, "Error in getting that size\n");
		   cur_font = XLoadQueryFont(dpy, "kanji");
		}
/* If that fails try and get something that some other machines have */
		if(cur_font == 0){
   			  cur_font = XLoadQueryFont(dpy, "k14");
		
		}
	}
	else if streq(the_font_family, FONT_FAMILY_DINGBATS){
	        sprintf(fn, "%s-%d", "zapfdingbats", 
					the_font_size);
		cur_font = XLoadQueryFont(dpy, fn);
	}
	else if (streq(the_font_family, FONT_FAMILY_PALATINO) ||
		  streq(the_font_family, FONT_FAMILY_AVANTGARDE) ||
		  streq(the_font_family, FONT_FAMILY_BOOKMAN) ||
		  streq(the_font_family, FONT_FAMILY_HELVETICAN) ||
		  streq(the_font_family, FONT_FAMILY_NEWCENTURY)) {
			set_font_name(fn, 1);
			cur_font = XLoadQueryFont(dpy, fn);
			/* a kludge - might help on a DECstation 
			 * specify name in slightly different format ! */
			if(cur_font == 0){
				set_font_name(fn, 2);
				cur_font = XLoadQueryFont(dpy, fn);
			}
	}
			
	else if streq(the_font_family, FONT_FAMILY_CHANCERY){
	        sprintf(fn, "%s-%d", "zapfchancery-mediumitalic", 
					the_font_size);
		cur_font = XLoadQueryFont(dpy, fn);
	}
	else {

/* Times-Roman uses an italic font wheras the other two use oblique,
 * kludge a way to call these */
		if(streq(the_font_family,FONT_FAMILY_ROMAN)
			&& streq(the_font_style,FONT_STYLE_OBLIQUE))
				strcpy(style, FONT_STYLE_ITALIC);
		else if(streq(the_font_family, FONT_FAMILY_ROMAN)
			&& streq(the_font_style, FONT_STYLE_BOLD_OBLIQUE))
				strcpy(style, FONT_STYLE_BOLD_ITALIC);
		else
			strcpy(style, the_font_style);
		font = (Xv_Font) xv_find(frame, FONT,
					 FONT_STYLE, style,
					 FONT_SIZE, the_font_size,
					 FONT_FAMILY, the_font_family,
					 0);


		if (!font) {
			fprintf(stderr, "Error finding font");
			font = (Xv_Font) xv_get(frame, XV_FONT);
		}
		cur_font = (XFontStruct *) xv_get(font, FONT_INFO);
	}
/* if we couldn't get the font try to fix it */
	if(cur_font == 0){
		fprintf(stderr, "Error setting font\n");
		fprintf(stderr, "(Present fn is:\n%s\n", fn);
		font = (Xv_Font) xv_get(frame, XV_FONT);
		cur_font = (XFontStruct *) xv_get(font, FONT_INFO);
	}
/* and check again */
	if(cur_font == 0)
		fprintf(stderr, "Still an error in setting the font!");
	else
		XSetFont(dpy, gc, cur_font->fid);
}


void
xmove_(float	* x, float	 *y)
{
void move_ps();
	xplot0 = (int) ((*x - xb) * fx);
	yplot0 = (int) (ypap - ((*y - yb) * fy));
	if(do_ps && Store_PS) move_ps(x, y);
}
void
xline_(float	* x, float	 *y)
{
void	line_ps();
int	xplot1, yplot1;
	xplot1 = (int) ((*x - xb) * fx);
	yplot1 = (int) (ypap - ((*y - yb) * fy));
	XDrawLine(dpy, xwin, gc, xplot0, yplot0, xplot1, yplot1);
	xplot0 = xplot1;
	yplot0 = yplot1;
	if(do_ps) line_ps(x, y);

}
void
line_ps(float	* x, float	 *y)
{
	store_ps(*x, *y,  PS_LINE);
}
void
move_ps(float	* x, float	 *y)
{
	store_ps(*x, *y,  PS_MOVE);
}

/* change default size of symbols */
void
ssize_(float * x)
{
	symbol_size = (int) *x;
	if (do_ps) store_ps(*x, *x, PS_SYMBOL_SIZE);

}


void
arc_ps(float x, float y, float radius, float ang1, float ang2)
{
	store_ps(x, y, PS_ARC_CENTRE);
	store_ps(x+radius, radius, PS_ARC_RADIUS);
	store_ps(ang1, ang2, PS_ARC_ANGLES);
}


void
arc_(xin, yin, radin, ang1, ang2, fill)
float	*xin, *yin, *radin, *ang1, *ang2;
int	*fill;
/* Draw an arc centered on xin, yin, radius defined
 * in units of X direction. ang1 and ang2 are start and end angles
 */
{
static unsigned int	width , height;
int		angle1 = 0, angle2 = 360*64;
float	x, y;

	x = *xin;
	y = *yin;

	angle1 = (int) (*ang1 *64.);
	angle2 = (int) (*ang2 * 64.) - angle1;
	
	xplot0 = (int) ((x - xb) * fx);
	yplot0 = (int) (ypap - ((y - yb) * fy));

	width = (int) (*radin *fx);

	xplot0 = xplot0 - width;
	yplot0 = yplot0 - width;

	width = width*2.;
	height = width;

	if(*fill == 1){
	  XFillArc(dpy, xwin, gc, xplot0, yplot0,
			width, height, angle1, angle2);
	}
	else{
	  XDrawArc(dpy, xwin, gc, xplot0, yplot0,
			width, height, angle1, angle2);
	}
	if (do_ps){
		arc_ps(*xin, *yin, *radin, *ang1, *ang2);
	}

}



void
circle_ps(float x, float y, float x2, float y2)
{
	store_ps(x, y, PS_CIRCLE_CENTRE);
	store_ps(x2, y2, PS_CIRCLE_END);
}


void
circle_(xin, yin, xin2, yin2)
float	*xin, *yin, *xin2, *yin2;
/* Draw a circle centred on (x,y) and
 * with a corner at (x2, y2)
 */
{
static unsigned int	width , height;
static int		angle1 = 0, angle2 = 360*64;
int		xplot1, yplot1;
float x, y, x2, y2;
	x = *xin;
	y = *yin;
	x2 = *xin2;
	y2 = *yin2;
	xplot0 = (int) ((x - xb) * fx);
	yplot0 = (int) (ypap - ((y - yb) * fy));
	xplot1 = (int) ((x2 - xb) * fx);
	yplot1 = (int) (ypap - ((y2 - yb) * fy));

	width = 2*abs(xplot0 - xplot1);

	height = width;
	xplot1 = xplot0 - width/2;
	yplot1 = yplot0 - width/2;
	XDrawArc(dpy, xwin, gc, xplot1, yplot1,
			width, height, angle1, angle2);
	if(do_ps) circle_ps(x, y, x2, y2);

}

void
arrow_store()
/* store changes in arrow style in PostScript file */
{
	if(!do_ps)return;
	store_ps((float) arrow_head, (float) arrow_style, PS_ARROW_STYLE);
}


void
arrow_angle_ps()
/* store change in arrow head angle */
{
	if(!do_ps)return;
	store_ps(arrow_angle, arrow_angle,  PS_ARROW_ANGLE);
}

void
arrow_size_ps()
/* store change in arrow head size */
{
	if(!do_ps)return;
	store_ps((float) arrow_size, (float) arrow_size,  PS_ARROW_SIZE);
}


/* change whether we have a single or double arrow heads */
void
adble_(void)
{
	if(arrow_head == ARROW_DOUBLE) return;
	arrow_head = ARROW_DOUBLE;
	arrow_store();
}

void
asingle_(void)
{
	if(arrow_head == ARROW_SINGLE) return;
	arrow_head = ARROW_SINGLE;
	arrow_store();
}


/* change the style used for drawing arrow heads */
void
astyle_(int	* style)
{
	if(arrow_style == *style) return;

	if(*style == ARROW_HOLLOW)
		arrow_style = *style;
	else if(*style == ARROW_FILL)
		arrow_style = *style;
	else
		arrow_style = ARROW_DEFAULT;
	arrow_store();
}

/* change the angle of arrow heads */
void
aangle_(float * angle)
{
	if((*angle * DTR) == arrow_angle)return;
	arrow_angle = *angle * DTR;
	arrow_angle_ps();
}

/* change the size of arrow heads */
void
asize_(float * size)
{
	if((int) *size == arrow_size)return;
	arrow_size = (int) *size;
	arrow_size_ps();
}


void
arrow_ps(float x, float y, float x2, float y2)
{
	store_ps(x, y, PS_ARROW_START);
	store_ps(x2, y2, PS_ARROW_END);
}
void
arrow_(xin, yin, xin2, yin2)
float	*xin, *yin, *xin2, *yin2;
/* draw an arrow with the tail at xin, yin and point
 * at xin2, yin2 */
{
float x, y, x2, y2;
int xplot1, yplot1;
int xstart, ystart, xend, yend;
float	ca, sa;
int	xsiz, ysiz;
int	hyp;
int	caa, saa;

	x = *xin;
	y = *yin;
	x2 = *xin2;
	y2 = *yin2;
	xplot0 = (int) ((x - xb) * fx);
	yplot0 = (int) (ypap - ((y - yb) * fy));
	xplot1 = (int) ((x2 - xb) * fx);
	yplot1 = (int) (ypap - ((y2 - yb) * fy));

	hyp = (int)sqrt((float) (xplot1-xplot0)*(xplot1-xplot0) +
			(yplot1-yplot0)*(yplot1-yplot0));

/* for zero length arrows just return */
	if(hyp == 0)return;
	
	ca = (float) (xplot1-xplot0)/hyp;
	sa = (float) (yplot0-yplot1)/hyp;

	caa = (int) (cos(arrow_angle) * arrow_size);
	saa = (int) (sin(arrow_angle) * arrow_size);

/* for filled and hollow heads don't want to draw the shaft all the way */
	rotate2(caa, 0, &xsiz, &ysiz, ca, sa);
		xend = xplot1;
		yend = yplot1;
		xstart = xplot0;
		ystart = yplot0;
	if(arrow_style == ARROW_HOLLOW || arrow_style == ARROW_FILL){
		xend = xplot1 - xsiz;
		yend = yplot1 + ysiz;
		if(arrow_head == ARROW_DOUBLE){
			xstart = xplot0 + xsiz;
			ystart = yplot0 - ysiz;
		}
	}
	
		
	      XDrawLine(dpy, xwin, gc, xstart, ystart, xend, yend);



	rotate2(caa, saa, &xsiz, &ysiz, ca, sa);
	xpoints[0].x = xplot1 - xsiz;
	xpoints[0].y = yplot1 + ysiz;
/* in case we need to fill the arrow head */
	xpoints[3].x = xpoints[0].x;
	xpoints[3].y = xpoints[0].y;
	xpoints[1].x = xplot1;
	xpoints[1].y = yplot1;
	rotate2(caa, -saa, &xsiz, &ysiz, ca, sa);
	/* rotate2(arrow_size, -arrow_size, &xsiz, &ysiz, ca, sa); */
	xpoints[2].x = xplot1 - xsiz;
	xpoints[2].y = yplot1 + ysiz;

	if(arrow_style == ARROW_HOLLOW)
	  XDrawLines(dpy, xwin, gc, xpoints, 4, CoordModeOrigin);
	else if(arrow_style == ARROW_FILL)
	 XFillPolygon(dpy, xwin, gc, xpoints, 3, Convex, CoordModeOrigin);
	else
	  XDrawLines(dpy, xwin, gc, xpoints, 3, CoordModeOrigin);

/* double arrow head? */
	if(arrow_head == ARROW_DOUBLE){
		rotate2(caa, saa, &xsiz, &ysiz, -ca, -sa);
		xpoints[0].x = xplot0 - xsiz;
		xpoints[0].y = yplot0 + ysiz;
	/* in case we need to fill the arrow head */
		xpoints[3].x = xpoints[0].x;
		xpoints[3].y = xpoints[0].y;
		xpoints[1].x = xplot0;
		xpoints[1].y = yplot0;
		rotate2(caa, -saa, &xsiz, &ysiz, -ca, -sa);
		xpoints[2].x = xplot0 - xsiz;
		xpoints[2].y = yplot0 + ysiz;
	
		if(arrow_style == ARROW_HOLLOW)
		  XDrawLines(dpy, xwin, gc, xpoints, 4, 
			CoordModeOrigin);
		else if(arrow_style == ARROW_FILL)
		 XFillPolygon(dpy, xwin, gc, xpoints, 3, 
			Convex, CoordModeOrigin);
		else
		  XDrawLines(dpy, xwin, gc, xpoints, 3, CoordModeOrigin);
	}
	

/* save arrow point as current position */
	xplot0 = xplot1;
	yplot0 = yplot1;

	if(do_ps) arrow_ps(x, y, x2, y2);
}

void
ellipse_ps(float x, float y, float x2, float y2)
{
	store_ps(x, y, PS_ELLIPSE_CENTRE);
	store_ps(x2, y2, PS_ELLIPSE_END);
}


void
elipse_(xin, yin, xin2, yin2)
float	*xin, *yin, *xin2, *yin2;
/* Draw an ellipse centred on (x,y) and
 * with a corner at (x2, y2)
 */
{
static unsigned int	width , height;
static int		angle1 = 0, angle2 = 360*64;
int		xplot1, yplot1;
float x, y, x2, y2;
	x = *xin;
	y = *yin;
	x2 = *xin2;
	y2 = *yin2;
	xplot0 = (int) ((x - xb) * fx);
	yplot0 = (int) (ypap - ((y - yb) * fy));
	xplot1 = (int) ((x2 - xb) * fx);
	yplot1 = (int) (ypap - ((y2 - yb) * fy));

	width = 2*abs(xplot0 - xplot1);
	
	height = 2*abs(yplot0 - yplot1);
	xplot1 = xplot0 - width/2;
	yplot1 = yplot0 - height/2;
	XDrawArc(dpy, xwin, gc, xplot1, yplot1,
			width, height, angle1, angle2);
	if(do_ps) ellipse_ps(x, y, x2, y2);

}



void
x_star(void)
{
int	xplot1, yplot1, xplot2, yplot2;
	xplot1 = xplot0 + symbol_size;	yplot1 = yplot0 + symbol_size;
	xplot2 = xplot0 - symbol_size;	yplot2 = yplot0 - symbol_size;
	XDrawLine(dpy, xwin, gc, xplot1, yplot1, xplot2, yplot2);
	xplot1 = xplot0 + symbol_size;	yplot1 = yplot0 - symbol_size;
	xplot2 = xplot0 - symbol_size;	yplot2 = yplot0 + symbol_size;
	XDrawLine(dpy, xwin, gc, xplot1, yplot1, xplot2, yplot2);
	
}

void
x_box(void)
{
int	xplot1, yplot1, xplot2, yplot2;
	xplot1 = xplot0 + symbol_size;	yplot1 = yplot0 + symbol_size;
	xplot2 = xplot0 - symbol_size;	yplot2 = yplot0 + symbol_size;
	XDrawLine(dpy, xwin, gc, xplot1, yplot1, xplot2, yplot2);
	xplot1 = xplot0 - symbol_size;	yplot1 = yplot0 - symbol_size;
	XDrawLine(dpy, xwin, gc, xplot1, yplot1, xplot2, yplot2);
	xplot2 = xplot0 + symbol_size;	yplot2 = yplot0 - symbol_size;
	XDrawLine(dpy, xwin, gc, xplot1, yplot1, xplot2, yplot2);
	xplot1 = xplot0 + symbol_size;	yplot1 = yplot0 + symbol_size;
	XDrawLine(dpy, xwin, gc, xplot1, yplot1, xplot2, yplot2);

}

void
x_cross(void)
{
int	xplot1, yplot1, xplot2, yplot2;
	xplot1 = xplot0 + symbol_size;	xplot2 = xplot0 - symbol_size;
	XDrawLine(dpy, xwin, gc, xplot1, yplot0, xplot2, yplot0);
	yplot1 = yplot0 + symbol_size;	yplot2 = yplot0 - symbol_size;
	XDrawLine(dpy, xwin, gc, xplot0, yplot1, xplot0, yplot2);

}


void
x_diamond(void)
{
	xpoints[0].x = xplot0 - symbol_size;
	xpoints[0].y = yplot0;
	xpoints[1].x = xplot0;
	xpoints[1].y = yplot0 + symbol_size;
	xpoints[2].x = xplot0 + symbol_size;
	xpoints[2].y = yplot0;
	xpoints[3].x = xplot0;
	xpoints[3].y = yplot0 - symbol_size;
	xpoints[4].x = xpoints[0].x;
	xpoints[4].y = xpoints[0].y;
	XFillPolygon(dpy, xwin, gc, xpoints, 5, Convex, CoordModeOrigin);
}

void
x_fill_diamond(void)
{
	xpoints[0].x = xplot0 - symbol_size;
	xpoints[0].y = yplot0;
	xpoints[1].x = xplot0;
	xpoints[1].y = yplot0 + symbol_size;
	xpoints[2].x = xplot0 + symbol_size;
	xpoints[2].y = yplot0;
	xpoints[3].x = xplot0;
	xpoints[3].y = yplot0 - symbol_size;
	XFillPolygon(dpy, xwin, gc, xpoints, 4, Convex, CoordModeOrigin);
}

void
x_triangle(void)
{
	xpoints[0].x = xplot0 - symbol_size;
	xpoints[0].y = yplot0 + symbol_size;
	xpoints[1].x = xplot0;
	xpoints[1].y = yplot0 - symbol_size;
	xpoints[2].x = xplot0 + symbol_size;
	xpoints[2].y = yplot0 + symbol_size;
	xpoints[3].x = xpoints[0].x;
	xpoints[3].y = xpoints[0].y;
	XDrawLines(dpy, xwin, gc, xpoints, 4, CoordModeOrigin);

}

void
x_fill_triangle(void)
{
	xpoints[0].x = xplot0 - symbol_size;
	xpoints[0].y = yplot0 + symbol_size;
	xpoints[1].x = xplot0;
	xpoints[1].y = yplot0 - symbol_size;
	xpoints[2].x = xplot0 + symbol_size;
	xpoints[2].y = yplot0 + symbol_size;
	XFillPolygon(dpy, xwin, gc, xpoints, 3, Convex, CoordModeOrigin);
}
	



void
x_circle(void)
{
static unsigned int	width , height;
static int		angle1 = 0, angle2 = 360*64;
int			xplot1, yplot1;
	width = 2 * symbol_size;
	height = width;
	xplot1 = xplot0 - symbol_size;
	yplot1 = yplot0 - symbol_size;
	XDrawArc(dpy, xwin, gc, xplot1, yplot1,
			width, height, angle1, angle2);
}

void
x_fill_circle(void)
{
static unsigned int	width , height;
static int		angle1 = 0, angle2 = 360*64;
int			xplot1, yplot1;
	width = 2 * symbol_size;
	height = width;
	xplot1 = xplot0 - symbol_size;
	yplot1 = yplot0 - symbol_size;
	XFillArc(dpy, xwin, gc, xplot1, yplot1,
			width, height, angle1, angle2);
}

void
x_fill_box(void)
{
static unsigned int	width , height;
int			xplot1, yplot1;
	width = 2 * symbol_size;
	height = width;
	xplot1 = xplot0 - symbol_size;
	yplot1 = yplot0 - symbol_size;
	XFillRectangle(dpy, xwin, gc, xplot1, yplot1, width, height);
}


/* draw a symbol (e.g. box, cross) at
 * specified point
 */
void
symbol_(float	* x, float	 *y, int	* i)
{
float	store_symbol;
	xplot0 = (int) ((*x - xb) * fx);
	yplot0 = (int) (ypap - ((*y - yb) * fy));
	if(do_ps){
		used_symbols = True;
		move_ps(x, y);
		store_symbol = (float) *i;
		store_ps(store_symbol, store_symbol, PS_PLOT_SYMBOL);
	}
	if (*i == STAR) x_star();
	else if(*i == BOX) x_box();
	else if(*i == CROSS) x_cross();
	else if(*i == CIRCLE) x_circle();
	else if(*i == FILL_CIRCLE) x_fill_circle();
	else if(*i == FILL_BOX) x_fill_box();
	else if(*i == DIAMOND) x_diamond();
	else if(*i == FILL_DIAMOND) x_fill_diamond();
	else if(*i == TRIANGLE) x_triangle();
	else if(*i == FILL_TRIANGLE) x_fill_triangle();
	else
	   fprintf(stderr, "unknown symbol number %d\n", *i);
}

/* start of polygon drawing routines */
void
arkpoly1_(float	* x, float	 *y)
{
	xplot0 = (int) ((*x - xb) * fx);
	yplot0 = (int) (ypap - ((*y - yb) * fy));
	if(do_ps && Store_PS){
		store_ps(*x, *y, PS_POLY_START);
	}
	no_poly_points = 0;
	xpoints[no_poly_points].x = xplot0;
	xpoints[no_poly_points].y = yplot0;       
}

/* add a point to the polygon list */
void
arkpoly_(float	* x, float	 *y)
{
	xplot0 = (int) ((*x - xb) * fx);
	yplot0 = (int) (ypap - ((*y - yb) * fy));
	if(do_ps && Store_PS){
		store_ps(*x, *y, PS_POLY_POINT);
	}
	no_poly_points++;
	if(no_poly_points <= NO_XPOINTS){
		xpoints[no_poly_points].x = xplot0;
		xpoints[no_poly_points].y = yplot0;
	}      
}

/* finish polygon drawing routines */
void
arkpoly2_(void)
{
	if(do_ps && Store_PS){
		store_ps(0.0, 0.0, PS_POLY_END);
	}
	no_poly_points++;
	XFillPolygon(dpy, xwin, gc, xpoints, no_poly_points,
		Complex, CoordModeOrigin);
}

/* Store the text angle instruction as
 * the x/y coordinate
 */
void
psangle_(float * angle)
{
	current_angle = *angle;
	cos_angle = cos(current_angle * DTR);
	sin_angle = sin(current_angle * DTR);
	if(do_ps) store_ps(current_angle, current_angle,  PS_ROTATE_TEXT);
}

/* encode the rgb colour in the x/y coordinates
 * to confuse things the x coordinate stores both the
 * red and the green colours
 */
void
ps_colour(int red, int green, int blue)
{
float	xcode, ycode;
	xcode = red * 1000. + green;
	ycode = blue;
	store_ps(xcode, ycode,  PS_COLOUR);
}




void
ps_fill_box(float x1, float y1, float x2, float y2)
{
/* store as a four parts */
	store_ps(x1, y1, PS_FILL);
	store_ps(x1, y2, PS_FILL);
	store_ps(x2, y2, PS_FILL);
	store_ps(x2, y1, PS_FILL);
	store_ps(x1, y1, PS_END_FILL);
}
		
void
fill_box(float x1, float y1, float x2, float y2)
{
int	xplot1, xplot2, yplot1, yplot2;
static unsigned int	width , height;

	xplot1 = (int) ((x1 - xb) * fx);
	yplot1 = (int) (ypap - ((y1 - yb) * fy));
	xplot2 = (int) ((x2 - xb) * fx);
	yplot2 = (int) (ypap - ((y2 - yb) * fy));

	width = abs(xplot1 - xplot2) + 1;
	height = abs(yplot1 - yplot2) + 1;

	xplot1 = MIN(xplot1, xplot2);
	yplot1 = MIN(yplot1, yplot2);

	XFillRectangle(dpy, xwin, gc, xplot1, yplot1, width, height);

	if(do_ps) ps_fill_box(x1, y1, x2, y2);
}

void
fbox_(float	* x1, float	 *y1, float	 *x2, float	 *y2)
{
	fill_box(*x1, *y1, *x2, *y2);
}

float
whatx_()
/* NB, if called from FORTRAN these (whatx and whaty) must be declared
 * as double precision!!!	*/
{
	return fx;
}

float
whaty_(void)
{
	return fy;
}


void
xlimitm_(float	* xlow, float	 *ylow, float	 *xhi, float	 *yhi)
{
int	width, height;

	width = (int) xv_get(canvas_paint_window(canvas), XV_WIDTH);
	height = (int) xv_get(canvas_paint_window(canvas), XV_HEIGHT);
	xpap = (float) width;
	ypap = (float) height;
	/* x conversion factor    */
         fx = xpap/(*xhi-*xlow);
         xb = *xlow;
	 xt = *xhi;
	/* y conversion factor */
         fy = ypap/(*yhi-*ylow);
         yb = *ylow;
	 yt = *yhi;
/* conversion factors for postscripts */
	fx_ps = xpaper_ps/(*xhi-*xlow);
	fy_ps = ypaper_ps/(*yhi-*ylow);
	
}

/* as xlimitm but use a square plotting area */
void
xlimits_(float	* xlow, float	 *ylow, float	 *xhi, float	 *yhi)
{
int	width, height;

	width = (int) xv_get(canvas_paint_window(canvas), XV_WIDTH);
	height = (int) xv_get(canvas_paint_window(canvas), XV_HEIGHT);
	width = MIN(width, height);
	height = width;
	xpap = (float) width;
	ypap = (float) height;
/*       x conversion factor    */
         fx=xpap/(*xhi-*xlow);
         xb = *xlow;
/*       y conversion factor */
         fy=ypap/(*yhi-*ylow);
         yb = *ylow;
/* conversion factors for postscripts */
	fx_ps = xpaper_ps/(*xhi-*xlow);
	fy_ps = ypaper_ps/(*yhi-*ylow);
	
}


int
non_z_length(buffin)
/* the returns the length of the buffer that contains non
 * blanks
 */
char	*buffin;
{
int	i;
	for (i = strlen(buffin)-1; i> 0; i--)
		if(!isspace(buffin[i]))
			break;
	i++;
	return (i);
}

int
first_nz(buffin)
/* returns the first element of the buffer that 
 * isn't a blank
 */
char	*buffin;
{
int	i;
	for (i = 0; i < strlen(buffin); i++)
		if(!isspace(buffin[i]))
			break;
	return (i);
}
	

void
pstsize(int new_size)
{
/* Make a new font of specified size */
			font_size = new_size;
			fprintf(fp, "/%s findfont\n%d scalefont\n",
					font_name, font_size);
/* and set to the correct angle */
			ct = cos(angle_now);
			st = sin(angle_now);			
			  fprintf(fp, "[ %.1f %.1f %.1f %.1f 0 0] makefont\n",
				  ct, st, 
				  -st, ct);
			fprintf(fp,"setfont\n");
}


void
decrease_font()
/* shrink font size down to the next nice easily available
 * if possible or else just decrease */
{
int new_size;
	if(the_font_size <= 2)
		new_size = 1;
	else
		new_size = the_font_size - 2;

	font_resize(new_size);
}	

void
increase_font()
/* shrink font size down to the next nice easily available
 * if possible or else just decrease */
{
int new_size;
	new_size = the_font_size + 2;

	font_resize(new_size);
}	

void
decrease_ps_font()
/* shrink font size down to the next nice easily available
 * if possible or else just decrease */
{
int new_size;
	if(font_size <= 2)
		new_size = 1;
	else
		new_size = the_font_size - 2;

	pstsize(new_size);
}	

void
increase_ps_font()
/* shrink font size down to the next nice easily available
 * if possible or else just decrease */
{
int new_size;
	new_size = font_size + 2;

	pstsize(new_size);
}	



void
xtxtm_(float	* x, float	 *y, char textin[], int * n)
{
void	ps_txtm();
int	xplot1, yplot1;
int	x_end, y_end;
int	i;
int	length;
void	s_script(), special_char();
char	buff[3];
int	value;
int	increment;
char	text[TEXT_LENGTH];

int	first, length2;
int	old_red, old_green, old_blue;


	sprintf(text, "%s", textin);
/* the font storage routines shouldn't write to the arrays -
 * everything should be handled here */
	Store_PS = False;


/* we'll let the program plot less than all the string but there's
 * no point in plotting blanks */
	length = MIN(*n, non_z_length(text));
/* return if we have no text to plot */
	if(length <= 0){
		Store_PS = True;
		return;
	}



/* store the raw input string in a "PostScript" array to make
 * it easier to send straight back to this routine in a redraw */
		ps_txtm(x, y, text, &length, RAW_TEXT);

/* store old colours in case changed inside string */
		old_red = current_red;
		old_green = current_green;
		old_blue = current_blue;


		xplot1 = (int) ((*x - xb) * fx);
		yplot1 = (int) (ypap - ((*y - yb) * fy));
		first = 0;
		xmove_(x, y);
		for (i = 0; i < length; i++){
		  if(streqn(text+i, KANJI_IN_JIS) ||
			streqn(text+i, KANJI_IN_OLD_JIS)){
		        length2 = i - first;
			angle_text(xplot1, yplot1, text+first, text+first,
				 length2,
				&x_end, &y_end, TEXT8);
		        xplot1 = x_end;
		        yplot1 = y_end;
			jis_mode = True;
		        i = i+3;
			first = i;
		  }
		  else if(streqn(text+i, KANJI_OUT_JIS) ||
			  streqn(text+i, KANJI_OUT_ASCII)) {
		        length2 = i - first;
			if(jis_mode)
				jis_char(&xplot1, &yplot1,
				   text+first, length2, &x_end, &y_end);
			else
				fprintf(stderr,
				   "Error: kanji exit without enter");
		        xplot1 = x_end;
		        yplot1 = y_end;
			jis_mode = False;
		        i = i+3;
			first = i;
		  }
		   
	
		  if(streqn(text+i, "\\") && !jis_mode){
		     length2 = i - first;
/* only plot (rotate) one character at a time */
			angle_text(xplot1, yplot1, text+first, text+first,
				 length2,
				&x_end, &y_end, TEXT8);
		        xplot1 = x_end;
		        yplot1 = y_end;
		     increment = 2; /* default increment */
		     if(streqn(text+i+1, "nf") || streqn(text+i+1, "rf"))
			font_style(NORMAL_TEXT);
		     else if(streqn(text+i+1, "if")) 
			font_style(ITALIC_TEXT);
		     else if(streqn(text+i+1, "bf")){
			font_style(BOLD_TEXT);}
		     else if(streqn(text+i+1, "bi")) 
			font_style(BOLD_ITALIC_TEXT);
		     else if(streqn(text+i+1, "sf")) 
			font_family(SYMBOL);
		     else if(streqn(text+i+1, "hf")) 
			font_family(HELVETICA);
		     else if(streqn(text+i+1, "tf")) 
			font_family(TIMES);
		     else if(streqn(text+i+1, "cf")) 
			font_family(COURIER);
		     /* change colour */
		     else if(streqn(text+i+1, "red")){ 
			ark_colour(255, 0, 0);
			increment = 3;
		     }
		     else if(streqn(text+i+1, "green")){ 
			ark_colour(0, 255, 0);
			increment = 5;
		     }
		     else if(streqn(text+i+1, "blue")){ 
			ark_colour(0, 0, 255);
			increment = 4;
		     }
		     else if(streqn(text+i+1, "white")){ 
			ark_colour(255, 255, 255);
			increment = 5;
		     }
		     else if(streqn(text+i+1, "black")){ 
			ark_colour(0, 0, 0);
			increment = 5;
		     }
		     else if(streqn(text+i+1, "up")) 
			s_script(&xplot1, &yplot1, SUPER_SCRIPT);
		     else if(streqn(text+i+1, "dn")) 
			s_script(&xplot1, &yplot1, SUB_SCRIPT);
		     else if(streqn(text+i+1, "bs")) 
			s_script(&xplot1, &yplot1, BACK_SPACE);
		     else if(streqn(text+i+1, "-")){
		 	decrease_font();
			increment = 1; }
		     else if(streqn(text+i+1, "+")){
		 	increase_font();
			increment = 1;}
		     else if(streqn(text+i+1, "[")){
			kanji_char(&xplot1, &yplot1,
				text+i+2, &increment, &x_end, &y_end);
		     }
		     else if(streqn(text+i+1, "{")) {
			special_char(&xplot1, &yplot1,
				text+i+2, &increment, &x_end, &y_end);
			}
		     else if(isdigit(text[i+1]) && isdigit(text[i+2])){
			strncpy(buff, text+i+1, 2);
			buff[2] = ' ';
			sscanf(buff, "%d", &value);
			font_resize(value);
		     }
		     else{
			fprintf(stderr, "Unrecognised control:\n %c%c\n",
				text[i+1], text[i+2]);
		     }
		     i = i+increment;
		     first = i+1;
		   } 

		}
		length2 = i - first;
			if(jis_mode){
				jis_char(&xplot1, &yplot1,
				   text+first, length2, &x_end, &y_end);
			}
			else{
				angle_text(xplot1, yplot1, text+first,
					text+first, length2,
					&x_end, &y_end, TEXT8);
			}
		        xplot1 = x_end;
		        yplot1 = y_end;
	Store_PS = True;
/* restore colours if changed */
	ark_colour(old_red, old_green, old_blue);
/* make sure we're out of kanji mode */
	jis_mode = False;

}

/* as above but allows text to be either right justified or
 * centred depending on the value of istyle */
void xsptxt_(float	* x, float	 *y, char	* text, int * n, int *istyle)
{

void	ps_txtm();
int	xplot1, yplot1;
int	length;

int	direction;
int	ascent, descent;
XCharStruct	overall;

/* Don't include blanks in length determination */
	text = text + first_nz(text);


/* we'll let the program plot less than all the string but there's
 * no point in plotting blanks */

	length = MIN( *n, non_z_length(text));
	text[length] = 0;
/* find length of string */
	XQueryTextExtents(dpy, cur_font->fid, text, length,
		&direction, &ascent, &descent, &overall);

		xplot1 = (int) ((*x - xb) * fx);
		yplot1 = (int) (ypap - ((*y - yb) * fy));
		if(*istyle == RIGHT_TEXT)
			xplot1 = xplot1 - overall.width;
		else if(*istyle == CENTRE_TEXT)
			xplot1 = xplot1 - (overall.width/2);
		else
			fprintf(stderr, "ERROR - unknown style %d",
					*istyle);
		XDrawString(dpy, xwin, gc, xplot1, yplot1, text, length);
	ps_txtm(x, y, text, &length, *istyle);
}



void
ps_txtm(float	* x, float	 *y, char	* text, int	* n, int	 istyle)
{

/* is PostScript output enabled ? */
	if(!do_ps) return;

/* Is there any text to plot? */
	if(*n == 0){
		return;
	}


	set_ps_text_value_n(ipst_slow, ipst_fast, text,
		MIN(*n, TEXT_LENGTH-1));


	ipst_fast++;
	if(ipst_fast >= PST_FAST){
		ipst_fast = ipst_fast - PST_FAST;
		ipst_slow++;
		if(!ps_text_alloc(ipst_slow))
			toarkt_("Memory Allocation Failed!!!");
	}
	if(istyle < 0){
		store_ps(*x, *y,  PS_TEXT);
	}
	else if(istyle == CENTRE_TEXT)
		store_ps(*x, *y, PS_TEXT_CENTRE);
	else if(istyle == RIGHT_TEXT)
		store_ps(*x, *y, PS_TEXT_RIGHT);
	else if(istyle == TEXT_CONTINUATION)
		store_ps(*x, *y, PS_TEXT_CONTINUE);
	else if(istyle == RAW_TEXT){
		store_ps((float) (*n), (float) (*n), PS_RAW_NCHAR);
		store_ps(*x, *y, PS_RAW_TEXT);
	}
	else
		fprintf(stderr, "WARNING: unknown style!\n");


}




void
store_ps(float x, float y, short instruction)
{
float x_value, y_value;

	if(instruction <  PS_ROTATE_TEXT){
		x_value = (x - xb) * fx_ps;
		y_value = (y - yb) * fy_ps;
		set_ps_values(ips_slow, ips_fast, x_value, y_value, 
					instruction);
		/* check if max or min changed */
		ps_max_x = MAX(ps_max_x, x_value);
		ps_max_y = MAX(ps_max_y, y_value);
		ps_min_x = MIN(ps_min_x, x_value);
		ps_min_y = MIN(ps_min_y, y_value); 
		
	}
	else {
		set_ps_values(ips_slow, ips_fast, x, y, instruction);
	}


	ips_fast++;
	if(ips_fast >= PS_FAST){
		ips_fast = ips_fast - PS_FAST;
		ips_slow++;
		if(!ps_alloc(ips_slow))
			toarkt_("Memory Allocation Failed!!!");
	}
	/* number_ps++; */
	increase_ps_number();
}

void
xnewpage_(void)
{
int	i;
	XClearWindow(dpy, xwin);

	/* if(n_colors > 1)
		XSetForeground(dpy, gc, old_colors[n_colors-1].pixel); */

	old_colors[0] = old_colors[n_colors];


	/* free previous color entries */
	for (i = 1; i < (n_colors); i++){
		XFreeColors(dpy, colormap, (unsigned long *)
 			&old_colors[i].pixel, 1, 0);
	}

	n_colors = 1;


	/* number_ps = 0; */	 /*postscript reset*/
	reset_ps_number();
	for(i = 0; i<= ips_slow; i++) /* destroy all to reinitalise */
		ps_free(i);
	for(i = 0; i<= ipst_slow; i++) /* destroy all to reinitalise */
		ps_text_free(i);
/* and make new allocations */
	ps_alloc(0);
	ps_text_alloc(0);

	ips_fast = 0;
	ipst_fast = 0;
	ips_slow = 0;
	ipst_slow = 0;
/* set initial font/colour properties based on last settings */
	start_font_size = the_font_size;
	start_style_id = current_style_id;
	start_family_id = current_family_id;
	start_red = current_red/256.;
	start_blue = current_blue/256.;
	start_green = current_green/256.;
	start_width = WIDTH_DEFAULT * (float) (current_width) ;
	start_angle = current_angle;
	start_symbol_size = symbol_size;
	*start_dash = *current_dash;
	start_dash_style = current_dash_style;
/* arrow stuff */
	start_arrow_size = arrow_size;
	start_arrow_angle = arrow_angle;
	start_arrow_style = arrow_style;
	start_arrow_head = arrow_head;
/* are we using a "non-standard" font? */
	if(current_family_id != TIMES) used_times = False;
	if(current_family_id != HELVETICA) used_helvetica = False;
	if(current_family_id != PALATINO) used_palatino = False;
	if(current_family_id != AVANTGARDE) used_avantgarde = False;
	if(current_family_id != BOOKMAN) used_bookman = False;
	if(current_family_id != HELVETICAN) used_helvetican = False;
	if(current_family_id != NEWCENTURY) used_newcentury = False;
	if(current_family_id != COURIER) used_courier = False;
	if(current_family_id != CHANCERY) used_chancery = False;
/* not used any symbols yet */
	used_symbols = False;
/* reset bounding box */
	ps_max_x = 0.0;
	ps_max_y = 0.0;
	ps_min_x = BIG_NUMBER;
	ps_min_y = BIG_NUMBER;

}


void
showit(void)
{
	XFlush(dpy);
}



/* or for FORTRAN calls */
void
gshow_(void)
{
	XFlush(dpy);
}




/* kludge !!! */
Bool
check_press(void)
{
	if(ipress3 == 0){
		return True;
	}
	else{
		ipress3--;
		return False;
	}
}


void
xcurse_()
/* Cursor routine */
{
int	root_x, root_y;
int	win_x, win_y;





	/* xv_set(canvas_paint_window(canvas), 
		WIN_IGNORE_X_EVENT_MASK, ButtonPressMask | ButtonReleaseMask,
		0); */



	if(mask == Button1Mask)
	   xv_set(frame, 
		FRAME_LEFT_FOOTER, "Use left mouse button",
		0);
	else if(mask == (Button1Mask | Button3Mask))
	   xv_set(frame, 
		FRAME_LEFT_FOOTER, "Left button draws, right starts over",
		0);
	else if(mask == (Button1Mask | Button2Mask))
	   xv_set(frame, 
		FRAME_LEFT_FOOTER, "Left button draws, middle terminates",
		0);
	else;
	
	cursor_done = 0;	/* show that cursor calls not finished */
/* and change the cursor shape to show that we can use it */
	if(my_cursor != 0)
	xv_set(xv_get(canvas, CANVAS_NTH_PAINT_WINDOW, 0), 
			WIN_CURSOR,	my_cursor,
			0);  


/* make sure mouse button 1 is not depressed right now 
 * --unless we're in the poly mode! (good choice?)
 */
	

	if (mask != (Button1Mask | Button2Mask)) {
		while (1) {
			XQueryPointer(dpy, xwin, &root, &child, &root_x,
				&root_y, &win_x, &win_y, &keys_buttons);
			if (!(keys_buttons & Button1Mask))
					break;
		}
	}


}

int
gotc_(void)
{
	return cursor_done;
}

/* set up the text cursor as the one to use next and
 * assign the string to it
 */
void
tcurs_(char	* string)
{
		text_cursor = (Xv_Cursor)xv_create(XV_NULL, CURSOR,
			CURSOR_STRING,	string,
			0);
		my_cursor = text_cursor;
}

/* set up an invisible cursor - it's assumed that
 * something else is done by the program to make the cursor
 * location visible */
void
bcurs_(void)
{
		my_cursor = blank_cursor;
}




/* routines to return the cursor position */
void
getc_(float	* value1, float	 *value2)
{
	*value1 = xcursor;
	*value2 = ycursor;


/* if we've got the cursor position reset back to the original
 * default cursor */
	if(cursor_done >= 1){
		xv_set(xv_get(canvas, CANVAS_NTH_PAINT_WINDOW, 0), 
			WIN_CURSOR,	default_cursor,
			0);
/* and say that the default text_cursor is the cross-hair one */
		my_cursor = cursor;

		/* xv_set(canvas_paint_window(canvas), 
			WIN_CONSUME_EVENTS, WIN_MOUSE_BUTTONS, 0, 
			WIN_IGNORE_X_EVENT_MASK, ButtonReleaseMask,
			0); */
	}

}


void
setmask_(i)
int	*i;
/* sets the button mask for accepting mouse buttons when using
 * the cursor. The parameter passed should work like a binary
 * mask. (1 = left button, 2 = middle, 4 = right.) 
 */
{
	mask = 0;
	if(*i < 1 || *i > 7){
		fprintf(stderr, "WARNING - value of %d sent to setmask\n\
Using default value of 1", *i);
		mask = Button1Mask;
		return;
	}
	if(*i & 1){
		mask = mask | Button1Mask;
	}
	if(*i & 2){
		mask = mask | Button2Mask;
	}
	if(*i & 4){
		mask = mask | Button3Mask;
	}
}

void
wherec_(void)
{
char	text[60];
int	root_x, root_y;
int	win_x, win_y;
Bool	good;
	good = XQueryPointer(dpy, xwin, &root, &child, &root_x,
			&root_y, &win_x, &win_y, &keys_buttons);


	if (good){

			xcursor = (float) win_x / fx +xb;
			ycursor = (ypap - (float) win_y) / fy +yb;
			sprintf(text, "x = %g y = %g",
				xcursor, ycursor);
			if(strne(text, old_text)){
				xv_set(frame, 
					FRAME_RIGHT_FOOTER, text, 0);
					strcpy(old_text, text);
				}

			if(keys_buttons & mask){
				if(keys_buttons & Button2Mask)
				 	cursor_done = 2;
				else if(keys_buttons & Button3Mask){
					ipress3++;
				 	cursor_done = 3;
				}
				else
					cursor_done = 1;
				 xv_set(frame, FRAME_LEFT_FOOTER, "", 0);
				 xv_set(frame, FRAME_RIGHT_FOOTER, "", 0);
				 /* XFlush(dpy); */
			        
			}
	}

}





void
lscape_(void)
{
	ps_direction = LANDSCAPE;
}
	
void
portrt_(void)
{
	ps_direction = PORTRAIT;
}
	

/* specify name of output file, orientation, whether output is
 * a printer of a file */
void
ps_print_set(name, direction, printwhen)
char	name[];
int	direction, printwhen; /* 0 = portrait, 1 = l.scape,
			       * 0 = print now, 1= just write file*/

{
	strcpy(ps_name, name);
	ps_direction = direction; 
	ps_printwhen = printwhen; 
}

void
ps_title_set(char title[])
{
	sprintf(ps_title, "%s", title);
}

/* specify the number of copies to be made */
void
ps_number_set(int ncopies_in)
{
	ps_ncopies = ncopies_in;
}


/* specify name of printer to send output to */
void
ps_printer_set(char	* in_printer)
{
	sprintf(ps_printer, "%s", in_printer);
}

void
ps_scale_set(scale)
/* specify whether the PostScript output file will maintain
 * the same scaling as the displayed plot 
 */
Bool scale;
{
	ps_maintain_scale = scale;
}

void
eps_set(void)
{
	eps = TRUE;
}

void
eps_unset(void)
{
	eps = FALSE;
}

void
write_time()
/* write time to PostScript header */
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
{
  char buf[TEXT_LENGTH];
  struct tm* tm;		/* date/time structure */
  time_t clock;			/* current time in seconds  */

  clock = time(0);
  tm = localtime(&clock);
  strftime(buf, TEXT_LENGTH, "%C", tm);
  fprintf(fp, "%%%%CreationDate: %s\n", buf);
} 




#define STROKE_TEST 1024	/* how often to do a "stroke" */
void
psprint_()
/* "Prints" to a postscript file or sends output directly to
 * an actual printer
 */
{
void	set_symbol_size(), fill_path(), draw_path(), draw_ps_arrow();
void	ps_dot_dash(), ISO_encode();
void	pstext();
int	i, j;
int	islow, ifast, itext_fast;
int	itext;
int	icycle;
float	xout, yout, ratio, inv_ratio, theta;
float	xnow, ynow;
float	fudge_ratio;
float	xcircle, ycircle;
float	arc_radius;
Bool	shrink_x;	/* if we maintain scaling do we need
				 * to make the x or y values smaller? */
Bool	UseTemp;	/* if we couldn't open the file write to /tmp */
float	xsub, ysub;
float	xpath[MAX_PATH], ypath[MAX_PATH]; /* for a filled shape */
int	ipath;
float	red, green, blue;
float	line_width;
int	ired, length;
/* int	style_id; */
/* parameters of arrows - style, single or double head, head size */
int	a_style, a_head;
float	a_size;
float	a_angle;
short	instruction;
static	char	buf[40];
int	ilen;
int	number_ps;


/* Ratios in case the plot is rotated */

	print_ps = True;

	ratio = xpaper_ps/ypaper_ps;
	inv_ratio = ypaper_ps/xpaper_ps;

	if(ps_printwhen == 0)
		UseTemp = True;
	else
		UseTemp = False;

	if(UseTemp){
			strcpy(ps_name, "/tmp/");
			sprintf(buf, "rob%-d.ps", getpid());
			strcat(ps_name, buf);
			fp = fopen(ps_name, "w");
	}

	else 
		fp = fopen(ps_name, "w");
	if(fp == 0){
			toarkt_("Couldn't write to specified file");
			fprintf(stderr, "Couldn't open output file");
			fprintf(stderr, "%s", ps_name);
			print_ps = False;
			return;
		}

/* Write a standard header with defaults */
fprintf(fp, "%%!PS-Adobe-3.0 EPSF-3.0\n");
fprintf(fp, "%%%%BoundingBox: ");
/* Bounding box is max and min if no eps support */
if(!eps){
	fprintf(fp, "0 0 %.0f %.0f\n", xpaper_ps, ypaper_ps);
	}
else{
/* fudge the borders a little */
	xsub = ps_min_x - 5.;
	ysub = ps_min_y - 5.;
 	fprintf(fp, "0 0 %.0f %.0f\n", 10. + ps_max_x - ps_min_x,
				     10. + ps_max_y - ps_min_y);
	}
fprintf(fp, "%%%%Title: %s\n", ps_title);
/* in principle other programs might use this library... */
fprintf(fp, "%%%%Creator: %s\n", CREATOR);

/* write current date */
write_time();
/* specify language level 2 is needed - the 
ISOLatin1Encoding command is the "culprit */

fprintf(fp, "%%%% LanguageLevel: 2\n");

fprintf(fp, "%% Postscript output from >aRk<\n");
fprintf(fp, "%% User: %s\n", (char *) getenv("USER"));
fprintf(fp, "%% Author: Robin Corbet, PSU.\n");
fprintf(fp, "%% corbet@astro.psu.edu\n");
fprintf(fp, "/l {lineto} bind def\n/m {moveto} bind def\n/s {show} bind def\n");
/* use ISO encoding of fonts */
fprintf(fp, "%%Use ISO encoding because X uses that too\n");

/* Only if we used Times-Roman; this is the default so
 * we almost certainly did! */
if(used_times) ISO_encode(TIMES);
/* similarly for the rest */
if(used_palatino) ISO_encode(PALATINO);
if(used_helvetica) ISO_encode(HELVETICA);
if(used_courier) ISO_encode(COURIER);
if(used_avantgarde) ISO_encode(AVANTGARDE);
if(used_bookman) ISO_encode(BOOKMAN);
if(used_helvetican) ISO_encode(HELVETICAN);
if(used_newcentury) ISO_encode(NEWCENTURY);

/* we probably don't have the full set of the ZapfChancery fonts so
 * just do one here */
if(used_chancery){
	fprintf(fp, "/ZapfChancery-MediumItalic findfont\n\
	dup length dict begin\n\
	{1 index /FID ne {def} {pop pop} ifelse} forall\n\
	/Encoding ISOLatin1Encoding def\n\
	currentdict\n\
	end\n\
	/ZapfChanceryISO exch definefont pop\n");
}

fprintf(fp, "0 setlinecap\n1 setlinejoin\n");
fprintf(fp, "%f setlinewidth\n", start_width);
/* dash properties of the line */
if(start_dash_style == SOLID)
	fprintf(fp, "[] 0 setdash\n");
else if(start_dash_style == DASH){
	fprintf(fp, "[%.1f %.1f] 0 setdash\n", start_dash[0], start_dash[1]);
	}
else if(start_dash_style == DOTDASH)
	ps_dot_dash();
else
	fprintf(stderr, "ERROR - INITIAL START DASH STYLE UNKNOWN\n");


/* right justified text */
fprintf(fp, "/rshow {dup stringwidth pop\n\
-1 mul 0 rmoveto\n\
show} bind def\n");

/* centred text */
fprintf(fp, "/cshow {dup stringwidth pop\n\
-1 mul 2 div 0 rmoveto show}  bind def\n");

/* define routines for drawing symbols - only do this if we used symbols */
/* first variables for the symbol size */
if(used_symbols){

set_symbol_size(start_symbol_size);

fprintf(fp, "/box\n {\
symbol_size symbol_size rmoveto\n\
neg_symbol_size2 0 rlineto\n\
0 neg_symbol_size2 rlineto\n\
symbol_size2 0 rlineto\n\
0 symbol_size2 rlineto\n\
stroke\n} bind def\n");

fprintf(fp, "/fill_box\n {gsave\n\
currentpoint translate\n\
newpath\n\
symbol_size symbol_size moveto\n\
neg_symbol_size2 0 rlineto\n\
0 neg_symbol_size2 rlineto\n\
symbol_size2 0 rlineto\n\
0 symbol_size2 rlineto\n\
closepath\n\
fill grestore} bind def\n");

fprintf(fp, "/star\n { symbol_size symbol_size rmoveto\n\
neg_symbol_size2 neg_symbol_size2 rlineto\n\
0 symbol_size2 rmoveto\n\
symbol_size2 neg_symbol_size2 rlineto\n\
stroke\n} bind def\n");

fprintf(fp, "/cross\n { symbol_size 0 rmoveto\n\
neg_symbol_size2 0 rlineto\n\
symbol_size symbol_size rmoveto\n\
0 neg_symbol_size2 rlineto\n\
stroke\n} bind def\n");

fprintf(fp, "/circle\n {gsave \ncurrentpoint translate \nnewpath\n\
0 0 symbol_size 0 360 arc\n closepath\n stroke\n grestore} bind def\n");

fprintf(fp, 
	"/fill_circle\n {gsave\n currentpoint translate \nnewpath\n\
0 0 symbol_size 0 360 arc\n closepath\n fill\n grestore} bind def\n");

fprintf(fp,
	"/diamond\n {\n\
neg_symbol_size neg_symbol_size rmoveto\n\
symbol_size symbol_size rlineto\n\
symbol_size neg_symbol_size rlineto\n\
neg_symbol_size neg_symbol_size rlineto\n\
neg_symbol_size symbol_size rlineto\n\
stroke\n} bind def\n");

fprintf(fp,
	"/fill_diamond\n {gsave\n currentpoint translate \nnewpath\n\
neg_symbol_size neg_symbol_size moveto\n\
symbol_size symbol_size rlineto\n\
symbol_size neg_symbol_size rlineto\n\
neg_symbol_size neg_symbol_size rlineto\n\
neg_symbol_size symbol_size rlineto\n\
closepath\n\
fill grestore} bind def\n");


fprintf(fp,
	"/triangle\n {\n\
neg_symbol_size 0 rmoveto\n\
symbol_size symbol_size2 rlineto\n\
symbol_size neg_symbol_size2 rlineto\n\
neg_symbol_size2 0 rlineto\n\
stroke\n} bind def\n");

fprintf(fp,
	"/fill_triangle\n {gsave\n currentpoint translate \nnewpath\n\
neg_symbol_size 0 moveto\n\
symbol_size symbol_size2 rlineto\n\
symbol_size neg_symbol_size2 rlineto\n\
neg_symbol_size2 0 rlineto\n\
closepath\n\
fill grestore} bind def\n");

}

fprintf(fp, "%%\n%% end of definitions\n%%\n");

/* current colour */
fprintf(fp, "%.2f %.2f %.2f setrgbcolor\n",
			start_red, start_green, start_blue);
/* font info */
start_font_id = start_family_id * NO_OF_STYLES
			+ start_style_id;


if(start_family_id == KANJI)
	strcpy(font_name, Kanji);
else if(start_family_id == SYMBOL)
	strcpy(font_name, Symbol);
else if(start_family_id == DINGBATS)
	strcpy(font_name, Dingbats);
else if(start_family_id == CHANCERY)
	strcpy(font_name, Chancery);
else{
	if( start_font_id >= 0)
	 sprintf(font_name, "%s%s", font_names[start_font_id], ISO);
	else{
         fprintf(stderr, "Silly font id = %d\n", start_font_id);
	 sprintf(font_name, "%s", font_names[0]);
	}
}

style_id = start_style_id;
family_id = start_family_id;

fprintf(fp, "/%s findfont\n%d scalefont\n", 
			font_name, start_font_size);
angle_now = start_angle*DTR;
ct = cos(angle_now);
st = sin(angle_now);
/* Write the matrix */
fprintf(fp, "[ %.1f %.1f %.1f %.1f 0 0] makefont\n",
ct, st, 
-st, ct);
fprintf(fp,"setfont\n");

/* plot will be landscape if required*/
if(ps_direction == LANDSCAPE) fprintf(fp, "90 rotate\n");
fprintf(fp, "%%\n%% start of plot\n%%\n");

fprintf(fp, "newpath\n");

	font_size = start_font_size;

	a_style = start_arrow_style;
	a_head = start_arrow_head;
	a_size = start_arrow_size;
	a_angle = start_arrow_angle;

	itext = 0;
	icycle = 0;
	i = 0;
	islow = 0;
	ifast = 0;
	itext = 0;
	itext_fast = 0;
	ipath = 0;

/* fudge ratio in case we want to keep the scaling */
	if(ps_direction == PORTRAIT)
		fudge_ratio = (xpap/ypap)/(xpaper_ps/ypaper_ps);
	else{
		fudge_ratio = (xpap/ypap)/(ypaper_ps/xpaper_ps);
	}


	if(fudge_ratio < 1.0  ){
		shrink_x = TRUE;
		fudge_ratio = 1./fudge_ratio;
	}
	else
		shrink_x = FALSE;


/* start of main output loop */
	number_ps = get_ps_number();
	while (i < number_ps){
		get_ps_values(islow, ifast, &xout, &yout, &instruction);



		if (instruction < PS_ROTATE_TEXT) {
			if (ps_maintain_scale) {
				if (shrink_x)
					xout = xout / fudge_ratio;
				else
					yout = yout / fudge_ratio;
			}
/* is it a landscape plot? */
			if (ps_direction == LANDSCAPE) {
				yout = yout * ratio - xpaper_ps;
				xout = xout * inv_ratio;
			}
			/* are we going to maintain scaling? */
		}


		if(instruction < PS_LINE) {
			if(eps){ xout = xout - xsub;
				 yout = yout - ysub;}
			xnow = xout; ynow = yout;
			  fprintf(fp,"%.1f %.1f m\n", xout, yout);}

		else if(instruction ==  PS_LINE){
			if(eps){ xout = xout - xsub;
				 yout = yout - ysub;}
			xnow = xout; ynow = yout;
			fprintf(fp,"%.1f %.1f l\n", xout, yout);}
		else if(instruction ==  PS_ARROW_END){
			if(eps){ xout = xout - xsub;
				 yout = yout - ysub;}
			/* fprintf(fp,"%.1f %.1f l\n", xout, yout); */
			draw_ps_arrow(xnow, ynow, xout, yout, 
				xpath, ypath, a_style, a_head, a_size, a_angle);
			xnow = xout; ynow = yout;}
		else if(instruction ==  PS_COLOUR) {

/* extract red and green */
			ired = (xout / 1000.);
			green = xout - (1000 * ired);
			red = ired / 256.;
			green = green / 256.;
			blue = yout / 256.;
			fprintf(fp, "stroke\n");
			fprintf(fp, "%.2f %.2f %.2f setrgbcolor\n",
				red, green, blue);
		}

		else if(instruction ==  PS_ROTATE_TEXT){
/* Make a new font, the new angle is the (original x coordinate) */
			theta = xout * DTR;
/* check whether the angle has changed significantly
 * to avoid needlessly making fonts.
 * Arbitrarily limit angle changes to 1/2 degree */
			if(fabs((angle_now - theta)) > TOLERANCE){
			  angle_now = theta;
			  ct = cos(theta);
			  st = sin(theta);
			  fprintf(fp, "/%s findfont\n%d scalefont\n",
					font_name, font_size);
/* Write the matrix */
			  fprintf(fp, "[ %.1f %.1f %.1f %.1f 0 0] makefont\n",
				  ct, st, 
				  -st, ct);
			  fprintf(fp,"setfont\n");
			}
		}

		else if(instruction ==  PS_SCALE_TEXT){
/* Make a new font of specified size */
			font_size = xout;
			fprintf(fp, "/%s findfont\n%d scalefont\n",
					font_name, font_size);
/* and set to the correct angle */
			ct = cos(angle_now);
			st = sin(angle_now);			
			  fprintf(fp, "[ %.1f %.1f %.1f %.1f 0 0] makefont\n",
				  ct, st, 
				  -st, ct);
			fprintf(fp,"setfont\n");

		}

		else if(instruction == PS_TEXT_FAMILY)
			ps_font_family((int) xout);

		else if(instruction == PS_TEXT_STYLE)
			ps_font_style((int) xout);

		else if(instruction ==  PS_TEXT_FONT){
			fprintf(stderr,"got a PS_TEXT_FONT - Oops!\n");
/* New font of specified style */
			/* style_id = (int) xout;
			if(style_id == KANJI_ID)
				strcpy(font_name, Kanji);
			else if(style_id == SYMBOL_ID)
				strcpy(font_name, Symbol);
			else if(style_id == DINGBATS_ID)
				strcpy(font_name, Dingbats);
			else if(style_id == CHANCERY_ID)
				strcpy(font_name, Chancery);
			else{
				if(style_id >= 0)
				  sprintf(font_name, "%s%s",
					font_names[style_id], ISO);
				else
				  fprintf(stderr, "Silly font id: %d\n",
						style_id);
			}
			fprintf(fp, "/%s findfont\n%d scalefont\n",
					font_name, font_size); */
/* and set to the correct angle */			
			ct = cos(angle_now);
			st = sin(angle_now);			
			  fprintf(fp, "[ %.1f %.1f %.1f %.1f 0 0] makefont\n",
				  ct, st, 
				  -st, ct);
			fprintf(fp,"setfont\n");
		}

		else if(instruction == PS_SUPER_SCRIPT){
			fprintf(fp, "%.1f %.1f rmoveto\n",
				-0.5*font_size*sin(angle_now),
				0.5*font_size*cos(angle_now));
		}

		else if(instruction == PS_SUB_SCRIPT){
			fprintf(fp, "%.1f %.1f rmoveto\n",
				0.5*font_size*sin(angle_now),
				-0.5*font_size*cos(angle_now));
		}

		else if(instruction == PS_BACK_SPACE){
			fprintf(fp, "%.1f %.1f rmoveto\n",
				-0.5*font_size*cos(angle_now),
				-0.5*font_size*sin(angle_now));
		}
				

		else if(instruction ==  PS_LINE_WIDTH){
/* new line width */
			line_width = WIDTH_DEFAULT * (xout);
			fprintf(fp, "stroke\n%.1f setlinewidth\n", 
				line_width);
		}

		else if(instruction == PS_ARROW_STYLE){
			a_head = (int) xout;
			a_style = (int) yout;

		}

		else if(instruction == PS_ARROW_SIZE){
			a_size = xout;
		}
		else if(instruction == PS_ARROW_ANGLE){
			a_angle = xout;
		}

		else if(instruction == PS_LINE_DASH_TOGGLE){
/* new dash pattern */
			fprintf(fp, "stroke\n");
			if( xout < 0.01)
				fprintf(fp, "[] 0 setdash\n");
			else
				fprintf(fp, "[%.1f %.1f] 0 setdash\n",
					xout, yout);
		}

		else if(instruction == PS_DOT_DASH){
/* dot dash pattern */
			ps_dot_dash();
		}

		else if(instruction == PS_PLOT_SYMBOL){
/* plot specified symbol */
			switch ((int) xout) {
			  case STAR: fprintf(fp, "star\n"); break;
			  case BOX: fprintf(fp, "box\n"); break;
			  case CROSS: fprintf(fp, "cross\n"); break;
			  case CIRCLE: fprintf(fp, "circle\n"); break;
			  case FILL_CIRCLE: fprintf(fp, "fill_circle\n"); break;
			  case FILL_BOX: fprintf(fp, "fill_box\n"); break;
			  case DIAMOND: fprintf(fp, "diamond\n"); break;
			  case FILL_DIAMOND: 
				fprintf(fp, "fill_diamond\n"); break;
			  case TRIANGLE: 
				fprintf(fp, "triangle\n"); break;
			  case FILL_TRIANGLE: 
				fprintf(fp, "fill_triangle\n"); break;
			  default: fprintf(stderr, "unknown symbol"); break;
			}
		}
/* start and end of polygon drawing sequences */
		else if(instruction == PS_POLY_START) {
			fprintf(fp, "gsave\nnewpath\n%.1f %.1f m\n",
					xout, yout);
		}
		else if(instruction == PS_POLY_POINT) {
			fprintf(fp, "%.1f %.1f l\n", xout, yout);
		}
		else if(instruction == PS_POLY_END) {
			fprintf(fp, "closepath\neofill\ngrestore\n");
		}
		else if(instruction == PS_CIRCLE_CENTRE) {
			xcircle = xout;
			ycircle = yout;
		}
		else if(instruction == PS_CIRCLE_END) {
			fprintf(fp, "stroke\n%.2f %.2f %.2f 0 360 arc\n",
				xcircle, ycircle, xout - xcircle);
			
		}
/* recycle variables - presumably we're not going to mix some
 * arc instructions in the middle of a circle instruction
 */
		else if(instruction == PS_ARC_CENTRE) {
			xcircle = xout;
			ycircle = yout;
		}
		else if(instruction == PS_ARC_RADIUS) {
			arc_radius = xout - xcircle;
		}
		else if(instruction == PS_ARC_ANGLES) {
/* last part of the arc instructions */
			fprintf(fp, "stroke\n%.1f %.1f %.1f %.1f %.1f arc\n",
				xcircle, ycircle, arc_radius,
				xout, yout);
		}
		else if(instruction == PS_ELLIPSE_CENTRE) {
			xcircle = xout; /* recycle variables ! */
			ycircle = yout;
		}
		else if(instruction == PS_ELLIPSE_END) {
			fprintf(fp, "%.1f %.1f m\n", xcircle, ycircle);
			fprintf(fp, "gsave\n");
			fprintf(fp, "currentpoint translate\n");
			fprintf(fp, "1 %.3f scale\n",
				fabs((yout-ycircle)/(xout - xcircle)));
			fprintf(fp, "newpath\n");
			fprintf(fp, "0 0 %.2f 0 360 arc\n stroke\n",
				xout - xcircle);
			fprintf(fp, "closepath\n");
			fprintf(fp, "grestore\n");
			
		}
		else if(instruction == PS_SYMBOL_SIZE){
/* change size of symbols */
			set_symbol_size((int) xout);
		}
		else if(instruction == PS_FILL){
			if(ipath < MAX_PATH){
				if(eps){ xout = xout - xsub;
				 	yout = yout - ysub;}
				xpath[ipath] = xout;
				ypath[ipath] = yout;
			}
			ipath++;
		}
		else if(instruction == PS_END_FILL){
/* fill the shape */
			if(ipath < MAX_PATH){
				if(eps){ xout = xout - xsub;
				 	yout = yout - ysub;}
				xpath[ipath] = xout;
				ypath[ipath] = yout;
			}
			fill_path(xpath, ypath, ipath);
			ipath = 0;
		}


		if (instruction == PS_TEXT ||
		    instruction == PS_TEXT_CONTINUE) {
			sprintf(buf,
			 "%s",get_ps_text_value(itext, itext_fast));
			fprintf(fp, "(");
			length = strlen(buf);
			for (j = 0; j < length; j++) {
				if (buf[j] == '(' || buf[j] == ')')
					fprintf(fp, "\\%c", buf[j]);
				else
					fprintf(fp, "%c", buf[j]);
			}
			fprintf(fp, ") s\n");

			incvals(&itext_fast, &itext);
		}
		if(instruction == PS_RAW_NCHAR){
			ilen = (int) xout;
		}
		if (instruction == PS_RAW_TEXT) {
/* increment indices but otherwise do nothing */
			pstext(get_ps_text_value(itext, itext_fast), ilen);
			incvals(&itext_fast, &itext);
		}
		if (instruction == PS_TEXT_CENTRE) {
			fprintf(fp, "(%s) cshow\n",
			   get_ps_text_value(itext, itext_fast));
			incvals(&itext_fast, &itext);
		}
		if (instruction == PS_TEXT_RIGHT) {
			fprintf(fp, "(%s) rshow\n", 
			   get_ps_text_value(itext, itext_fast));
			incvals(&itext_fast, &itext);
		}
/* a kludge which may very well not be correct! */
	if(instruction == PS_MOVE || 
		instruction == PS_LINE){
			icycle++;
	}
	if(icycle == STROKE_TEST) {
		fprintf(fp, "stroke\n%.1f %.1f m\n", xnow, ynow);
		icycle = icycle - STROKE_TEST;
	}

	ifast++;
	if(ifast >= PS_FAST){
		ifast = ifast - PS_FAST;
		islow++;
	}
	i++;


}
	fprintf(fp, "stroke\n");
	if(!eps)fprintf(fp, "showpage\n");
	fclose(fp);
	print_ps = False;
	if (ps_printwhen == 0) {
		if(streq(ps_printer, "")){
			/* just use the default printer */
			sprintf(buf, "%s -#%d %s", LPR, ps_ncopies, ps_name);
		}
		else {
			/* a printer is specified */
			sprintf(buf, "%s -P%s -#%d %s", LPR, ps_printer, ps_ncopies,
				ps_name);
		}
		system(buf);
		/* if we're using a temporary file delete it right now */
		if (UseTemp) {
			/* sprintf(buf, "%s %s", RM, ps_name);
			system(buf); */
			if(unlink(ps_name))
			  fprintf(stderr, 
			    "ERROR: deleting temporary PostScript file\n");
		}
	}
}


/* set line style to dot dash */
void
ps_dot_dash(void)
{
	fprintf(fp, "stroke\n");
	fprintf(fp, "[4 4 .3 4] 0 setdash\n");
}

void
set_symbol_size(int symbol_size_now)
{
fprintf(fp, "/symbol_size %d def\n", symbol_size_now);
fprintf(fp, "/symbol_size2 %d def\n", 2 * symbol_size_now);
fprintf(fp, "/neg_symbol_size %d def\n", -symbol_size_now);
fprintf(fp, "/neg_symbol_size2 %d def\n", -2 * symbol_size_now);
}




/* draw a set of points and fill the define area */
void
fill_path(float xpath[], float ypath[], int ipath)
{
int	i, iloop;
	if(ipath < 1) return; /* need at least two points */
	iloop = MIN(ipath, MAX_PATH);
fprintf(fp, "gsave\n\
newpath\n");
	fprintf(fp, "%.1f %.1f m\n", xpath[0], ypath[0]);
	for(i =1; i <= iloop; i++)
		fprintf(fp, "%.1f %.1f l\n", xpath[i], ypath[i]);
	fprintf(fp, "closepath\nfill\ngrestore\n");
}

/* draw a series of lines */
void
draw_path(float xpath[], float ypath[], int ipath)
{
int	i, iloop;
	if(ipath < 1) return; /* need at least two points */
	iloop = MIN(ipath, MAX_PATH);
	fprintf(fp, "%.1f %.1f m\n", xpath[0], ypath[0]);
	for(i =1; i <= iloop; i++)
		fprintf(fp, "%.1f %.1f l\n", xpath[i], ypath[i]);
}
	
void
draw_ps_arrow(xnow, ynow, xout, yout, xpath, ypath, a_style, a_head,
a_size, a_angle)
float	xnow, ynow, xout, yout;
float	xpath[], ypath[];
int	a_style, a_head;
float	a_size, a_angle;
{
float	hyp, ca, sa;
float	caa, saa;
float	xsiz, ysiz;
float	xstart, ystart, xend, yend;

	hyp = sqrt((xout-xnow)*(xout-xnow) +
			(yout-ynow)*(yout-ynow));

/* for zero length arrows just return */
	if(hyp == 0)return;
	
	ca = (float) (xout-xnow)/hyp;
	sa = (float) (ynow-yout)/hyp;
/* cos and sin of arrow head angles */
	caa = (float) (cos(a_angle) * a_size);
	saa = (float) (sin(a_angle) * a_size);

/* draw the shaft */
	rotate3(caa, 0.0, &xsiz, &ysiz, ca, sa);
	xstart = xnow;
	ystart = ynow;
	xend = xout;
	yend = yout;
	if(a_style == ARROW_HOLLOW || a_style == ARROW_FILL){
		xend = xout - xsiz;
		yend = yout + ysiz;
		if(a_head == ARROW_DOUBLE){
			xstart = xnow + xsiz;
			ystart = ynow - ysiz;
		}
	}
	fprintf(fp,"%.1f %.1f m\n", xstart, ystart);
	fprintf(fp,"%.1f %.1f l\n", xend, yend);

	rotate3(caa,
		saa, &xsiz, &ysiz, ca, sa);
	xpath[0] = xout - xsiz;
	ypath[0] = yout + ysiz;
/* in case we need to fill the arrow head */
	xpath[3] = xpath[0];
	ypath[3] = ypath[0];
	xpath[1] = xout;
	ypath[1] = yout;
	rotate3(caa, -saa, &xsiz, &ysiz, ca, sa);
	xpath[2] = xout - xsiz;
	ypath[2] = yout + ysiz;
	if(a_style == ARROW_FILL)
		fill_path(xpath, ypath, 3);
	else if(a_style == ARROW_HOLLOW)
		draw_path(xpath, ypath, 3);
	else
		draw_path(xpath, ypath, 2);
/* double arrow head? */
	if(a_head == ARROW_DOUBLE){
		ca = -ca;
		sa = -sa;
		rotate3(caa, saa, &xsiz, &ysiz, ca, sa);
		xpath[0] = xnow - xsiz;
		ypath[0] = ynow + ysiz;
	/* in case we need to fill the arrow head */
		xpath[3] = xpath[0];
		ypath[3] = ypath[0];
		xpath[1] = xnow;
		ypath[1] = ynow;
		rotate3(caa, -saa, &xsiz, &ysiz, ca, sa);
		xpath[2] = xnow - xsiz;
		ypath[2] = ynow + ysiz;

	if(a_style == ARROW_FILL)
		fill_path(xpath, ypath, 3);
	else if(a_style == ARROW_HOLLOW)
		draw_path(xpath, ypath, 3);
	else
		draw_path(xpath, ypath, 2);
	
	}
}

void
ark_image(an_image, cw, ch, xplot1, yplot1)
/* put a sub-image, such as rotated text, onto the display */
XImage *an_image;
int	cw, ch;
int	xplot1, yplot1;
{
int	xstart, ystart, xend, yend;

	xstart = MAX(xplot1, 0);
	ystart = MAX((yplot1 - ch), 0);
	xend = MIN((xplot1 + cw), 
			(int)xv_get(canvas_paint_window(canvas), XV_WIDTH));
	yend = MIN(yplot1, 
		(int)xv_get(canvas_paint_window(canvas), XV_HEIGHT));
	
	XPutImage(dpy, xwin, gc, an_image, 0, 0, 
		xstart, ystart,
		xend-xstart, yend-ystart);
}

void
ZeroPixmap(Pixmap the_pixmap, unsigned int width, unsigned int height)
{
	gcerase_();
	XFillRectangle(dpy, the_pixmap, gc, 0, 0, width,
			height);
	gccopy_();
}
		
void
convert_code(short int in, short int *out1, short int *out2)
{
#define ZERO_POINT	15489


	in = in + ZERO_POINT;


	*out1 = in/96;
	*out2 = in - (*out1 * 96);

	*out1 = *out1 - 1;
	*out2 = *out2 + 96;


}

/* plot a two byte encoded Japanese character */
void
jis_char(int            * xplot1, int *yplot1, char           * text, int length, int            * x_end, int *y_end)
{
	short int       value, value1, value2;
	XChar2b         buffer16[2];
	char            small_buffer[9];
	int             i, old_family;

	if(print_ps){
			old_family = family_id;
			ps_font_family(KANJI);
	}
	else{
			old_family = current_family_id;
			font_family(KANJI);
	}



	for (i = 0; i < length; i = i + 2) {
		value1 = text[i] - 32;
		value2 = text[i + 1] - 32;

		value = value1 * 96 + value2;
		value = value + 31;
		if (print_ps) {
			convert_code(value, &value1, &value2);
			sprintf(small_buffer, "\\%3.3o\\%3.3o",
				value1, value2);
			ps_tout(small_buffer, 8);
		}
		else {

			buffer16[0].byte2 = value & 255;
			buffer16[0].byte1 = value >> 8;

			angle_text(*xplot1, *yplot1, buffer16, buffer16,
				   1, x_end, y_end, TEXT16);
			*xplot1 = *x_end;
			*yplot1 = *y_end;
		}
	}
	if(print_ps){
			ps_font_family(old_family);
	}
	else{
			font_family(old_family);
	}

}

void
kanji_char(int * xplot1, int *yplot1, char	* text, int * increment, int * x_end, int *y_end)
{
/* structure for character names and PS/ISO values */
struct symbol_values
{
	char name[4];
	short int value;
};
static struct symbol_values kana[] =
{
#include "kana.h" 
};
/* structure for character names and PS/ISO values - syllables
 * which are combinations of two characters */
struct symbol_values2
{
	char name[4];
	short int value1, value2;
};
static struct symbol_values2 kana2[] =
{
#include "kana2.h" 
};
short	int value1, value2;
int	valuea, valueb;
XChar2b	buffer16[2];
char	small_buffer[9];
int i, old_family, slen;


/* replace "]" with a 0 */
	slen = strlen(text);
	for (i = 0; i < slen; i++){
		if(text[i] == ']'){
			text[i] = 0;
			break;
		}
	}

/* is it a JIS code? */
	if(i == 4){
		sscanf(text, "%2x", &valuea);
		sscanf(text+2, "%2x", &valueb);
		small_buffer[0] = (char) valuea;
		small_buffer[1] = (char) valueb;
		jis_char(xplot1, yplot1, small_buffer, 2, x_end, y_end);
		*increment = 6;
		return;
	}
		

/* look for kana characters */
	for(i = 0; i != -1; i++){
		if(streq(kana[i].name, "last")) break;
		if(streq(text, kana[i].name)){
		  *increment = strlen(kana[i].name) + 2;
		  if(print_ps){
			old_family = family_id;
			ps_font_family(KANJI);
			convert_code(kana[i].value, &value1, &value2);
		  	sprintf(small_buffer, "\\%3.3o\\%3.3o",
				value1, value2);
		  	ps_tout(small_buffer, 8);
			ps_font_family(old_family);
		  }
		  else{
		  	old_family = current_family_id;
		  	font_family(KANJI);

		        buffer16[0].byte2 = kana[i].value & 255;
		        buffer16[0].byte1 = kana[i].value >> 8;

			angle_text(*xplot1, *yplot1, buffer16, buffer16,
					1, x_end, y_end, TEXT16);
		  	font_family(old_family);
		  	*xplot1 = *x_end;
		  	*yplot1 = *y_end;
		  }
		  return;
		}
	}

/* look for kana double characters */
	for(i = 0; i != -1; i++){
		if(streq(kana2[i].name, "last")) break;
		if(streq(text, kana2[i].name)){
		  *increment = strlen(kana2[i].name) + 2;
		  if(print_ps){
			old_family = family_id;
			ps_font_family(KANJI);
			convert_code(kana2[i].value1, &value1, &value2);
		  	sprintf(small_buffer, "\\%3.3o\\%3.3o",
				value1, value2);
		  	ps_tout(small_buffer, 8);
			ps_font_family(old_family);
			convert_code(kana2[i].value2, &value1, &value2);
		  	sprintf(small_buffer, "\\%3.3o\\%3.3o",
				value1, value2);
		  	ps_tout(small_buffer, 8);
			ps_font_family(old_family);
		  }
		  else{
		  	old_family = current_family_id;
		  	font_family(KANJI);

		        buffer16[0].byte2 = kana2[i].value1 & 255;
		        buffer16[0].byte1 = kana2[i].value1 >> 8;

			angle_text(*xplot1, *yplot1, buffer16, buffer16,
					1, x_end, y_end, TEXT16);
		  	*xplot1 = *x_end;
		  	*yplot1 = *y_end;

		        buffer16[0].byte2 = kana2[i].value2 & 255;
		        buffer16[0].byte1 = kana2[i].value2 >> 8;

			angle_text(*xplot1, *yplot1, buffer16, buffer16,
					1, x_end, y_end, TEXT16);
		  	font_family(old_family);
		  	*xplot1 = *x_end;
		  	*yplot1 = *y_end;
		  }
		  return;
		}
	}
}

static	char *greek[] = {
"alpha", "beta", "chi", "delta", "epsilon", "phi",
"gamma", "eta", "iota", "varphi", "kappa", "lambda", "mu",
"nu", "omicron", "pi", "theta", "rho", "sigma",
"tau", "upsilon", "varomega", "omega", "xi", "psi", "zeta",

"Alpha", "Beta", "Chi", "Delta", "Epsilon", "Phi",
"Gamma", "Eta", "Iota", "Phi1", "Kappa", "Lambda", "Mu",
"Nu", "Omicron", "Pi", "Theta", "Rho", "Sigma",
"Tau", "Upsilon", "Omega1", "Omega", "Xi", "Psi", "Zeta",

"last",};

static char *alphabet[] = {
"a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
"k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
"v", "w", "x", "y", "z",

"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U",
"V", "W", "X", "Y", "Z", };


/* structure for character names and PS/ISO values */
struct symbol_value
{
	char name[15];
	short int value;
};


static struct symbol_value symbols[] =
{
{"-", 0055},
{"aleph", 0300},
{"angle", 0320},
{"approx", 0273},
{"ast", 0052},
{"asterisk", 0052},
{"bot", 0136},
{"bullet", 0267},
{"cap", 0307},
{"ldots", 0274},
{"circlemultiply", 0304},
{"circleplus", 0305},
{"club", 0247},
{"clubsuit", 0247},
{"copyright", 0343},
{"congruent", 0100},
{"cong", 0100},
{"cup", 0310},
{"degree", 0260},
{"diamond", 0340},
{"diamondsuit", 0250},
{"divide", 0270},
{"div", 0270},
{"downarrow", 0257},
{"Downarrow", 0337},
{"dotmath", 0327},
{"element", 0316},
{"emptyset", 0306},
{"equiv", 0272},
{"equivalent", 0272},
{"exists", 0044},
{"forall", 0042},
{"geq", 0263},
{"gradient", 0321},
{"greaterequal", 0263},
{"heart", 0251},
{"heartsuit", 0251},
{"Im", 0301},
{"in", 0316},
{"infinity", 0245},
{"infty", 0245},
{"integral", 0362},
{"intersection", 0307},
{"land", 0331},
{"langle", 0341},
{"lceil", 0351},
{"lfloor", 0353},
{"leftarrow", 0254},
{"Leftarrow", 0334},
{"leftrightarrow", 0253},
{"Leftrightarrow", 0333},
{"leq", 0243},
{"lessequal", 0243},
{"lnot", 0330},
{"lor", 0332},
{"minus", 0055},
{"nabla", 0321},
{"ne", 0271},
{"neq", 0271},
{"notequal", 0271},
{"notsubset", 0313},
{"notin", 0317},
{"not=", 0271},
{"partial", 0266},
{"partialdiff", 0266},
{"perpendicular", 0136},
{"plusminus", 0261},
{"pm", 0261},
{"product", 0325},
{"prod", 0325},
{"proportional", 0265},
{"propto", 0265},
{"radicalex", 0140},
{"rangle", 0361},
{"rceil", 0371},
{"Re", 0302},
{"registered", 0322},
{"return", 0277},
{"rfloor", 0373},
{"rightarrow", 0256},
{"Rightarrow", 0336},
{"spade", 0252},
{"spadesuit", 0252},
{"subset", 0314},
{"subseteq", 0315},
{"suchthat", 0047},
{"summation", 0345},
{"sum", 0345},
{"superset", 0311},
{"supset", 0311},
{"supseteq", 0312},
{"surd", 0326},
{"therefore", 0134},
{"times", 0264},
{"trademark", 0324},
{"union", 0310},
{"universal", 0042},
{"uparrow", 0255},
{"Uparrow", 0335},
{"weierstrass", 0303},
{"wp", 0303},
{"last", 0000},
} ;

/* post-script font will be ISO encoded too (as is the X font) */

static struct symbol_value romans[] =
{
{"!", 0241},
{"?", 0277},
{"aa", 0345},
{"AA", 0305},
{"aacute", 0341},
{"Aacute", 0301},
{"'A", 0301},
{"acircumflex", 0342},
{"^a", 0342},
{"Acircumflex", 0302},
{"^A", 0302},
{"adieresis", 0344},
{"\"a", 0344},
{"Adieresis", 0304},
{"ae", 0346},
{"AE", 0306},
{"agrave", 0340},
{"`a", 0340},
{"Agrave", 0300},
{"`A", 0300},
{"aring", 0345},
{"Aring", 0305},
{"Angstrom", 0305},
{"atilde", 0343},
{"~a", 0343},
{"Atilde", 0303},
{"~A", 0303},
{"backslash", 0134},
{"cc", 0347},
{"ccedilla", 0347},
{"Ccedilla", 0307},
{"cedilla", 0270},
{"cent", 0242},
{"currency", 0244},
{"eacute", 0351},
{"'e", 0351},
{"Eacute", 0311},
{"'E", 0311},
{"ecircumflex", 0352},
{"^e", 0352},
{"Ecircumflex", 0312},
{"^E", 0312},
{"edieresis", 0353},
{"\"e", 0353},
{"Edieresis", 0313},
{"\"E", 0313},
{"egrave", 0350},
{"`e", 0350},
{"Egrave", 0310},
{"`E", 0310},
{"eth", 0360},
{"Eth", 0320},
{"guillemotleft", 0253},
{"guillemotright", 0273},
{"half", 0275},
{"hyphen", 0255},
{"iacute", 0355},
{"'i", 0355},
{"Iacute", 0315},
{"'I", 0315},
{"icircumflex", 0356},
{"^i", 0356},
{"Icircumflex", 0316},
{"^I", 0316},
{"idieresis", 0357},
{"\"i", 0357},
{"Idieresis", 0317},
{"\"I", 0317},
{"igrave", 0354},
{"`i", 0354},
{"Igrave", 0314},
{"`I", 0314},
{"ntilde", 0361},
{"~n", 0361},
{"Ntilde", 0321},
{"~N", 0321},
{"o", 0370},
{"O", 0330},
{"oacute", 0363},
{"'o", 0363},
{"Oacute", 0323},
{"'O", 0323},
{"ocircumflex", 0364},
{"^o", 0364},
{"Ocircumflex", 0324},
{"^O", 0324},
{"odieresis", 0366},
{"\"o", 0366},
{"Odieresis", 0326},
{"\"O", 0326},
{"ogonek", 0236},
{"ograve", 0362},
{"`o", 0362},
{"Ograve", 0322},
{"`O", 0322},
{"ordfem", 0252},
{"ordmasc", 0272},
{"otilde", 0365},
{"~o", 0365},
{"Otilde", 0325},
{"~O", 0325},
{"paragraph", 0266},
{"pipe", 0246},
{"pounds", 0243},
{"quarter", 0274},
{"ring", 0232},
{"section", 0247},
{"ss", 0337},
{"sterling", 0243},
{"thorn", 0376},
{"Thorn", 0336},
{"threequarters", 0276},
{"uacute", 0372},
{"'u", 0372},
{"Uacute", 0332},
{"'U", 0332},
{"ucircumflex", 0373},
{"^u", 0373},
{"Ucircumflex", 0333},
{"^U", 0333},
{"udieresis", 0374},
{"\"u", 0374},
{"Udieresis", 0334},
{"\"U", 0334},
{"ugrave", 0371},
{"`u", 0371},
{"Ugrave", 0331},
{"`U", 0331},
{"yacute", 0375},
{"'y", 0375},
{"Yacute", 0335},
{"'Y", 0335},
{"ydieresis", 0377},
{"\"y", 0377},
{"yen", 0245},
{"last", 0000},
};

void 
get_symbol(int i, char string[])
{
	strcpy(string, symbols[i].name);
}

void 
get_special(int i, char string[])
{
	strcpy(string, romans[i].name);
}

void 
get_greek(int i, char string[])
{
	strcpy(string, greek[i]);
}


int
ark_symbol_code(char string[], char small_buffer[])
{
int	i;

	for(i = 0; i != -1; i++){
		if(streq(symbols[i].name, "last")) break;
		if(streq(string, symbols[i].name)){
		        small_buffer[0] = symbols[i].value;
		  return (1);
		}
	}
	return(0);
}

int
ark_specials_code(char string[], char small_buffer[])
{
int	i;

	for(i = 0; i != -1; i++){
		if(streq(romans[i].name, "last")) break;
		if(streq(string, romans[i].name)){
		        small_buffer[0] = romans[i].value;
		  return (1);
		}
	}
	return(0);
}

int
ark_greek_code(char string[], char small_buffer[])
{
int	i;

	for(i = 0; i != -1; i++){
		if(streq(greek[i], "last")) break;
		if(streq(string, greek[i])){
		/*
		        sprintf(small_buffer, "%s", alphabet[i]); */
			small_buffer[0] = *alphabet[i];
		  return (1);
		}
	}
	return(0);
}

void
special_char(int * xplot1, int *yplot1, char	* text, int * increment, int * x_end, int *y_end)
{

void angle_text();


char	small_buffer[5];
int	i, slen, old_family;

/* replace "}" with a 0 */
	slen = strlen(text);
	for (i = 0; i < slen; i++){
		if(text[i] == '}'){
			text[i] = 0;
			break;
		}
	}


/* first check if it's a Greek symbol */

 for(i = 0; i < 52; i++){
   if(streq(text, greek[i])){
	*increment = strlen(greek[i]) + 2;
	if(print_ps){
		old_family = family_id;
		ps_font_family(SYMBOL);
		ps_tout(alphabet[i], 1);
		ps_font_family(old_family);
	}
	else{
		old_family = current_family_id;
		font_family(SYMBOL);
		angle_text(*xplot1, *yplot1, alphabet[i], alphabet[i], 
				1, x_end, y_end, TEXT8);
		font_family(old_family);
		*xplot1 = *x_end;
		*yplot1 = *y_end;
	}
	return;
	}
  }
/* look for other special symbols in symbol font if it wasn't Greek */
	for(i = 0; i != -1; i++){
		if(streq(symbols[i].name, "last")) break;
		if(streq(text, symbols[i].name)){
		  *increment = strlen(symbols[i].name) + 2;
		  if(print_ps){
			old_family = family_id;
			ps_font_family(SYMBOL);
		  	sprintf(small_buffer, "\\%3.3o", symbols[i].value);
		  	ps_tout(small_buffer, 4);
			ps_font_family(old_family);
		  }
		  else{
		  	old_family = current_family_id;
		  	font_family(SYMBOL);
		        small_buffer[0] = symbols[i].value;
		  	angle_text(*xplot1, *yplot1, small_buffer, 
			       small_buffer, 1, x_end, y_end, TEXT8);
		  	font_family(old_family);
		  	*xplot1 = *x_end;
		  	*yplot1 = *y_end;
		  }
		  return;
		}
	}
/* look for other special symbols in standard fonts if it wasn't a symbol */
	for(i = 0; i != -1; i++){
		if(streq(romans[i].name, "last")) break;
		if(streq(text, romans[i].name)){
/* change to "normal" font if the current style isn't a Roman one */
		  *increment = strlen(romans[i].name) + 2;
		  if(print_ps){
			old_family = family_id;
			if(family_id > END_REGULAR_FONTS)
				ps_font_family(TIMES);
		  	sprintf(small_buffer, "\\%3.3o", romans[i].value);
		  	ps_tout(small_buffer, 4);
			ps_font_family(old_family);
		  }
		  else{
		  	old_family = current_family_id;
		  	if(current_family_id > END_REGULAR_FONTS){
		     		font_family(TIMES);
		     	}
		        small_buffer[0] = romans[i].value;
		  	angle_text(*xplot1, *yplot1, 
				small_buffer, small_buffer, 
				1, x_end, y_end, TEXT8);
		  	font_family(old_family);
		  	*xplot1 = *x_end;
		  	*yplot1 = *y_end; 
		  }
		  return;
		}
	}
/* didn't find it */
	fprintf(stderr, "Couldn't find symbol:\n%s\n", text);
	
}


/* calculate "typical" character size for use externally (FORTRAN)*/
void
txtsiz_(float	* xsiz, float	 *ysiz)
{
int	direction;
int	ascent, descent;
XCharStruct	overall;

	if(cur_font == 0 || cur_font->fid == 0){
		fprintf(stderr, "0 font in txtsiz\n");
		*xsiz = 1.0;
		*ysiz = 1.0;
		return;
	}

	XQueryTextExtents(dpy, cur_font->fid, "a", 1,
		&direction, &ascent, &descent, &overall);

	*ysiz = (float) (ascent + descent) / fy;
	*xsiz = (float) overall.width /fx;

}

/* calculate X length of specified character */
void
chchk_(char	* ch, float	* xsiz)
{
int	direction;
int	ascent, descent;
XCharStruct	overall;

	XQueryTextExtents(dpy, cur_font->fid, ch, 1,
		&direction, &ascent, &descent, &overall);

	*xsiz = (float) overall.width /fx;
}




/* Do superscripts, subscripts and back spaces depending
 * on the value of "id" */
void
s_script(int * xplot1, int *yplot1, int id)
{
int	direction;
int	ascent, descent;
int	i1, i2, a, b;
XCharStruct	overall;

	XQueryTextExtents(dpy, cur_font->fid, "a", 1,
		&direction, &ascent, &descent, &overall);

	i1 = 0;
	if(id == SUPER_SCRIPT){
		i2 = (ascent+descent)/2;
	}
	else if(id == SUB_SCRIPT){
		i2 = -(ascent+descent)/2;
	}
	else if(id == BACK_SPACE){
		i1 = -overall.width;
		i2 = 0;
	}
	else
		fprintf(stderr, "s_script called with unknown ID\n");


	rotate(i1, i2, &a, &b);

	*xplot1 = *xplot1 + a;
	*yplot1 = *yplot1 - b;
}

		


void
angle_text(xplot1, yplot1, text, text16, length, x_end, y_end, text_type)
/* draws a text string at the required angle */
char text[];		/* "normal text to plot */
XChar2b text16[];	/* two byte text to plot */
int length;		/* length of text string */
int xplot1, yplot1;
int *x_end, *y_end;	/* returned end coordinates of plotted text */
int text_type;		/* determines if text is 8 or 16 bit */
{
Pixmap	pixmap;
Pixmap	pixmap2;
XImage	*ximage;
XImage	*ximage2;
unsigned int	c_width, c_height;
int	i1, i2, a, b, a2, b2, a_min, a_max, b_min, b_max;
int	dest_width, dest_height;
unsigned long pixel_value;
int	direction;
int	ascent, descent;
XCharStruct	overall;
int	text_height, text_width;
Bool	text_type16;

float	*cos_pointer;
float	*sin_pointer; 

int	t1, t2, t3;

/* we shouldn't be doding anything here is we're "printing" PostScript
 * output */
	if(print_ps) return;

	if(length <= 0){
		*x_end = xplot1;
		*y_end = yplot1;
		 return;
	}

	if(text_type == TEXT8)
		text_type16 = False;
	else if(text_type == TEXT16)
		text_type16 = True;
	else
		fprintf(stderr, "Error: unrecognised text_type\n");


/* find length of string */
	if(text_type16){
		XQueryTextExtents16(dpy, cur_font->fid, text16, length,
			&direction, &ascent, &descent, &overall);
	}
	else{
		XQueryTextExtents(dpy, cur_font->fid, text, length,
			&direction, &ascent, &descent, &overall);
	}


/* for zero angles have a special case and plot directly to the
 * screen without having to rotate images etc. */

	if(fabs(current_angle) < TOLERANCE){
		if(text_type16){
			XDrawString16(dpy, xwin, gc, xplot1, yplot1, 
				text16, length);
		}
		else {
			XDrawString(dpy, xwin, gc, xplot1, yplot1, 
				text, length);
		}
		*x_end = xplot1 + overall.width;
		*y_end = yplot1;
		return;
	}

	c_width = (int)xv_get(canvas_paint_window(canvas), XV_WIDTH);
	c_height = (int)xv_get(canvas_paint_window(canvas), XV_HEIGHT);


	


	text_height = ascent+ descent+2;
	text_width = overall.width + 1;




	pixmap = XCreatePixmap(dpy, xwin, text_width, 
		text_height, display_depth);
	ZeroPixmap(pixmap, text_width, text_height);
	if(text_type16) {
		XDrawString16(dpy, pixmap, gc, 1, text_height-descent-1, 
			text16, length);
	}
	else {
		XDrawString(dpy, pixmap, gc, 1, text_height-descent-1, 
			text, length);
	}


	ximage = XGetImage(dpy, pixmap, 0, 0, text_width, text_height,
		AllPlanes, XYPixmap);
/* find min and max of boundary */
	i1 = 0; i2 = 0;
/* no need to do rotation of (0,0) - stays where it is */
	a_min = 0; a_max = 0;  
	b_min = 0; b_max = 0;

	i1 = text_width;
	rotate(i1, i2, &a, &b);
	a_min = MIN(a, a_min); a_max = MAX(a, a_max);
	b_min = MIN(b, b_min); b_max = MAX(b, b_max);

	*x_end = xplot1 + a;
	*y_end = yplot1 - b;

	i2 = text_height;
	rotate(i1, i2, &a, &b);
	a_min = MIN(a, a_min); a_max = MAX(a, a_max);
	b_min = MIN(b, b_min); b_max = MAX(b, b_max);

	i1 = 0;
	rotate(i1, i2, &a, &b);
	a_min = MIN(a, a_min); a_max = MAX(a, a_max);
	b_min = MIN(b, b_min); b_max = MAX(b, b_max);


	if( (xplot1 + a_min) < 0) a_min = -xplot1;
	if( (yplot1 - b_max) < 0)
		b_max = yplot1;
	dest_width = a_max - a_min;
	dest_height = b_max - b_min;

	
/*	if((yplot1 - b_min) > c_height || (yplot1 - b_min) < 0 ||
		(xplot1 + a_min) > c_width || (xplot1 + a_min) < 0 ) */
	if((yplot1  - dest_height - b_min) < 0 || 
		(yplot1 - dest_height - b_min) > c_height)
	{
		fprintf(stderr, "Text starts outside window\n");
		return;
	}

/* put present view into ximage2 */
	/* dest_width = MIN(c_width - a_min, dest_width);
	dest_height = MIN(yplot1, dest_height); */



	if(dest_height < 0 ||
	   dest_width < 0)
		return;

	
	pixmap2 = XCreatePixmap(dpy, xwin, dest_width, 
		dest_height, display_depth);

	XCopyArea(dpy, xwin, pixmap2, gc,
		xplot1 + a_min, yplot1 - dest_height - b_min, 
		dest_width, dest_height,
		0, 0);

	ximage2 = XGetImage(dpy, pixmap2, 0, 0, dest_width, dest_height,
		AllPlanes, XYPixmap);




/* try inverse rotation - avoids holes in rotated text. (Shows
 * up for large bold fonts)
 */

	t3 = dest_height + b_min - 1;

/* precompute sines and cosines */

	cos_pointer = (float *) calloc(dest_height, sizeof(float));
	sin_pointer = (float *) calloc(dest_height, sizeof(float));

	for (b = 0; b < dest_height; b++){
		b2 = t3 - b;
		*(cos_pointer + b) = ((float) b2 * cos_angle);
		*(sin_pointer + b) = ((float) b2 * sin_angle);
	} 
		

	for (a = 0; a < dest_width ; a++) {
		a2 = a + a_min;
		t1 = a2*cos_angle;
		t2 = text_height + a2*sin_angle;
		for (b = 0; b < dest_height; b++) {
		   b2 = t3 - b;
		   /* i1 = t1 + b2 * sin_angle; */
		   i1 = t1 + *(sin_pointer + b);
		   if(i1 > -1 && i1 < text_width){
		      /* i2 = t2 - b2 * cos_angle; */
		      i2 = t2 - *(cos_pointer + b);
                      if(i2 > -1 && i2 < text_height){
			/* only copy non-zero */
		   	if((pixel_value = XGetPixel(ximage, i1, i2))
				== foreground)
		   		XPutPixel(ximage2, a, b, pixel_value);
			}
		   }
		}
	}
	free(cos_pointer);
	free(sin_pointer);

	ark_image(ximage2, dest_width, dest_height, xplot1 + a_min, 
					yplot1 - b_min);

/* destroy structures */
	XFreePixmap(dpy, pixmap);
	XFreePixmap(dpy, pixmap2);
	XDestroyImage(ximage);
	XDestroyImage(ximage2);
}

/* rotate coordinates based on current text plotting angle */
void
rotate(int i1, int i2, int	* a, int	 *b)
{
	*a = i1 * cos_angle - i2 * sin_angle;
	*b = i1 * sin_angle + i2 * cos_angle;
}

/* rotate coordinates based on current text plotting angle
 * this is the inverse direction of the one above (should avoid
 * holes in rotated text */
void
rotateb(int i1, int i2, int	* a, int	 *b)
{
	*a = i1 * cos_angle + i2 * sin_angle;
	*b = i2 * cos_angle - i1 * sin_angle;
}



/* rotate coordinates based on specified cos and sine values
 * (e.g. used in arrow-head routines */
void
rotate2(int i1, int i2, int	* a, int	 *b, float ca, float sa)
{
	
	*a = i1 * ca - i2 * sa;
	*b = i1 * sa + i2 * ca;
}

/* rotate coordinates based on specified cos and sine values
 * (e.g. used in arrow-head routines */
/* floating point version */
void
rotate3(float i1, float i2, float	* a, float	 *b, float ca, float sa)
{
	
	*a = i1 * ca - i2 * sa;
	*b = i1 * sa + i2 * ca;
}

void
ark_redrawn(int index)
{
int	plot_number_save;


	if(sps_ptr[index - 1] == 0 ||
		index < 1 || index >= MAX_PLOTS){
		fprintf(stderr, "ERROR: Invalid plot to redraw\n");
		fprintf(stderr, "Index = %d\n", index);
		return;
	}

	plot_number_save = plot_number;

/* update current plot before loading a new one */
	save_super_ps();

	plot_number = index - 1;
	load_super_ps(plot_number);
	redraw();
/* save new parameters after redraw */
	save_super_ps();

	plot_number = plot_number_save;
	load_super_ps(plot_number);


}

void
redraw_last(void)
{
	ark_redrawn(total_plot_number + 1);
}

Frame
ark_old_to_current(int index)
{
int	plot_number_save;

	save_super_ps();
	xv_set(frame, 
		FRAME_SHOW_FOOTER,	FALSE,
		0);

	plot_number = index - 1;

	load_super_ps(plot_number);

	xv_set(frame, 
		FRAME_SHOW_FOOTER,	TRUE,
		0);

	return(frame);
}

void
ark_last_to_current(void)
{
	ark_old_to_current(total_plot_number + 1);
}

/* return the current plot canvas */
Canvas
ark_current_canvas(void)
{
	return(canvas);
}

/* redraw plot taking into account changes in plot area size */
void
redraw()
/* Canvas	canvas_in;
Xv_window	paint_window;
Display		dpy_in;
Window		xwin_in;
Xv_xrectlist	*xrects; */	
{
short	instruction;
int	i, ifast, islow;
int	itext, itext_fast;
int	red, green, blue;
int	style;
float	xout, yout;
float	xnow, ynow;
float	xold, yold;
float	fxoldps;
float	fyoldps;
Bool	was_do_ps;
int	itemp, jtemp;
int	ifamily, istyle;
int ilen;
int	number_ps;
static int text_length = TEXT_LENGTH-1;


	number_ps = get_ps_number();

	/* no plot yet done! */
	if(number_ps <= 0) return;

	XClearWindow(dpy, xwin);

	/* can't scale correctly */
	if(xb == xt || yb == yt) return;

	fxoldps = fx_ps;
	fyoldps = fy_ps;

	xlimitm_(&xb, &yb, &xt, &yt);



	i = ifast = islow = 0;
	itext = itext_fast = 0;
	if(do_ps)
		was_do_ps = TRUE;
	else
		was_do_ps = FALSE;
	haltps_();



/* initialisations */
	font_resize(start_font_size);
	font_family(start_family_id);
	font_style(start_style_id);
	itemp = start_width/WIDTH_DEFAULT;
	set_line_width(itemp);
	xout = (float) start_symbol_size;
	ssize_(&xout);
	xout = (float) start_arrow_size;
	asize_(&xout);
	if(start_arrow_head == ARROW_DOUBLE)
		adble_();
	else
		asingle_();
	astyle_(&start_arrow_style);
	psangle_(&start_angle);
	ark_colour((int) (256. * start_red),
		   (int) (256. * start_green),
		   (int) (256. * start_blue));

	if(start_dash_style == DOTDASH){
		set_line_dotdash();
	}
	else if(start_dash_style == DASH){
		if(start_dash[0] < 1.0)
			set_line_dot();
		else
			set_line_dash();
	}
	else if(start_dash_style == SOLID){
		set_line_nodash();
	}
	else
		fprintf(stderr, "ERROR - INITIAL START DASH STYLE UNKNOWN\n");

/* start of loop */
	while(i < number_ps){
		get_ps_values(islow, ifast, &xout, &yout, &instruction);
		if(instruction < PS_ROTATE_TEXT){
			xout = xout/fxoldps + xb;
			yout = yout/fyoldps + yb;
		}
		itemp = (int) xout;
		jtemp = (int) yout;
		if (instruction < PS_LINE) {
			xmove_(&xout, &yout);
		}
		else if (instruction == PS_LINE) {
			xline_(&xout, &yout);
		}
		else if (instruction == PS_ARROW_END) {
			arrow_(&xnow, &ynow, &xout, &yout);
		}
		else if (instruction == PS_COLOUR) {

			/* extract red and green */
			red = (xout / 1000.);
			green = xout - (1000 * red);
			blue = yout;
			ark_colour(red, green, blue);
		}
		else if(instruction == PS_ROTATE_TEXT){
			psangle_(&xout);
		}
		else if (instruction == PS_SCALE_TEXT){
			font_resize(itemp);
		}

		else if(instruction == PS_TEXT_FAMILY){
			font_family(itemp);
		}
		else if(instruction == PS_TEXT_STYLE){
			font_style(itemp);
		}
		else if (instruction == PS_TEXT_FONT) {
			if (itemp == KANJI_ID)
				font_family(KANJI);
			else if (itemp == SYMBOL_ID)
				font_family(SYMBOL); 
			else if (itemp == DINGBATS_ID)
				font_family(DINGBATS); 
			else if (itemp == CHANCERY_ID)
				font_family(CHANCERY); 
			else {
				ifamily = itemp / NO_OF_STYLES;
				istyle = itemp - ifamily * NO_OF_STYLES;
				font_family(ifamily);
				font_style(istyle);
			}
		}
		else if (instruction == PS_SUPER_SCRIPT){
		}
		else if (instruction == PS_SUB_SCRIPT){
		}
		else if (instruction == PS_BACK_SPACE){
		}
		else if (instruction == PS_LINE_WIDTH){
			set_line_width(itemp);
		}
		else if (instruction == PS_ARROW_STYLE){
			if(itemp == ARROW_DOUBLE)
				adble_();
			else
				asingle_();
			astyle_(&jtemp);
		}
		else if (instruction == PS_ARROW_SIZE){
			asize_(&xout);
		}
		else if (instruction == PS_ARROW_ANGLE){
			xout = xout/DTR;
			aangle_(&xout);
		}
		else if (instruction == PS_LINE_DASH_TOGGLE) {
			if (xout < 0.001){
				set_line_nodash();
			}
			else if (xout >  0.001 && xout < 1.0){
				set_line_dot();
			}
			else if (xout > 1.0){
				set_line_dash();
			}
			else
				fprintf(stderr,
					"unregistered dash length %.1f\n",
					xout);
		}
		else if (instruction == PS_DOT_DASH) {
			set_line_dotdash();
		}
		else if (instruction == PS_PLOT_SYMBOL){
			symbol_(&xnow, &ynow, &itemp);
		}
		else if(instruction == PS_SYMBOL_SIZE){
			ssize_(&xout);
		}
		else if(instruction == PS_POLY_START){
			arkpoly1_(&xout, &yout);
		}
		else if(instruction == PS_POLY_POINT){
			arkpoly_(&xout, &yout);
		}
		else if(instruction == PS_POLY_END){
			arkpoly2_();
		}
		else if(instruction == PS_CIRCLE_CENTRE){
		}
		else if(instruction == PS_CIRCLE_END){
			circle_(&xnow, &ynow, &xout, &yout);
		}
		else if(instruction == PS_ARC_CENTRE){
		}
		else if(instruction == PS_ARC_RADIUS){
		}
		else if(instruction == PS_ARC_ANGLES){
			arc_(&xold, &yold, &ynow, &xout, &yout);
		}

		else if(instruction == PS_ELLIPSE_CENTRE){
		}
		else if(instruction == PS_ELLIPSE_END){
			elipse_(&xnow, &ynow, &xout, &yout);
		}
		else if(instruction == PS_FILL){
		}
/* this assumes END_FILL can only come from fill box - may change
 * in the future!
 */
		else if(instruction == PS_END_FILL){
			fill_box(xout, yout, xold, yold);
		}

		if (instruction == PS_TEXT) {
			incvals(&itext_fast, &itext);
		}
		else if (instruction == PS_TEXT_CONTINUE) {
			incvals(&itext_fast, &itext);
		}
		else if(instruction == PS_RAW_NCHAR){
			ilen = itemp;
		} else if (instruction == PS_RAW_TEXT) {
			xtxtm_(&xout, &yout,
				get_ps_text_value(itext, itext_fast), &ilen);
			incvals(&itext_fast, &itext);
		} else if (instruction == PS_TEXT_RIGHT) {
			style = RIGHT_TEXT;
			xsptxt_(&xout, &yout,
			   get_ps_text_value(itext, itext_fast), 
			   &text_length, &style);
			incvals(&itext_fast, &itext);
		} else if (instruction == PS_TEXT_CENTRE) {
			style = CENTRE_TEXT;
			xsptxt_(&xout, &yout,
 			   get_ps_text_value(itext, itext_fast), 
			   &text_length, &style);
			incvals(&itext_fast, &itext);
		} else;
	ifast++;
	if(ifast >= PS_FAST){
		ifast = ifast - PS_FAST;
		islow++;
	}
	xold = xnow;
	yold = ynow;
	xnow = xout;
	ynow = yout;
	i++;
	}
	if(was_do_ps)
		startps_();
}


void
incvals(int	* itext_fast, int	 *itext)
{

		*itext_fast = *itext_fast + 1;
		if (*itext_fast >= PST_FAST) {
			*itext_fast = *itext_fast - PST_FAST;
			*itext = *itext + 1;
		}
}








/* a lot of similarities to xtxtm - called when making postscript
 * output. */

void
pstext(char textin[], int length)
{
int	i;
char	buff[3];
int	value;
int	increment;
char	text[TEXT_LENGTH];
int	crap1, crap2, crap3, crap4;  /* special_char expects ints, but
				     * they're not used if called from here
				     */

int	first, length2;


	sprintf(text, "%s", textin);

		first = 0;

		for (i = 0; i < length; i++){
		  if(streqn(text+i, KANJI_IN_JIS) ||
			streqn(text+i, KANJI_IN_OLD_JIS)){
		        length2 = i - first;
		        ps_tout(text + first, length2);
			jis_mode = True;
		        i = i+3;
			first = i;
		  }
		  else if(streqn(text+i, KANJI_OUT_JIS) ||
			  streqn(text+i, KANJI_OUT_ASCII)) {
		        length2 = i - first;
			if(jis_mode)
				jis_char(&crap1, &crap2,
				   text+first, length2, &crap3, &crap4);
			else
				fprintf(stderr,
				   "Error: kanji exit without enter");
			jis_mode = False;
		        i = i+3;
			first = i;
		  }
		  if(streqn(text+i, "\\") && !jis_mode){
		     length2 = i - first;
		     ps_tout(text + first, length2);
/* only plot (rotate) one character at a time */
		     increment = 2; /* default increment */
		     if(streqn(text+i+1, "nf") || streqn(text+i+1, "rf"))
			ps_font_style(NORMAL_TEXT);
		     else if(streqn(text+i+1, "if")) 
			ps_font_style(ITALIC_TEXT);
		     else if(streqn(text+i+1, "bf")){
			ps_font_style(BOLD_TEXT);}
		     else if(streqn(text+i+1, "bi")) 
			ps_font_style(BOLD_ITALIC_TEXT);
		     else if(streqn(text+i+1, "sf")) 
			ps_font_family(SYMBOL);
		     else if(streqn(text+i+1, "hf")) 
			ps_font_family(HELVETICA);
		     else if(streqn(text+i+1, "tf")) 
			ps_font_family(TIMES);
		     else if(streqn(text+i+1, "cf")) 
			ps_font_family(COURIER);
		     /* change colour */
		     else if(streqn(text+i+1, "red")){ 
			ps_cout(255, 0, 0);
			increment = 3;
		     }
		     else if(streqn(text+i+1, "green")){ 
			ps_cout(0, 255, 0);
			increment = 5;
		     }
		     else if(streqn(text+i+1, "blue")){ 
			ps_cout(0, 0, 255);
			increment = 4;
		     }
		     else if(streqn(text+i+1, "white")){ 
			ps_cout(255, 255, 255);
			increment = 5;
		     }
		     else if(streqn(text+i+1, "black")){ 
			ps_cout(0, 0, 0);
			increment = 5;
		     }
		     else if(streqn(text+i+1, "up")) 
			psout(SUPER_SCRIPT);
		     else if(streqn(text+i+1, "dn")) 
			psout(SUB_SCRIPT);
		     else if(streqn(text+i+1, "bs")) 
			psout(BACK_SPACE);
		     else if(streqn(text+i+1, "-")){
		 	decrease_ps_font();
			increment = 1; }
		     else if(streqn(text+i+1, "+")){
		 	increase_ps_font();
			increment = 1;}
		     else if(streqn(text+i+1, "[")){
			kanji_char(&crap1, &crap2,
					text+i+2, &increment, &crap3, &crap4);
		     }
		     else if(streqn(text+i+1, "{")){
			special_char(&crap1, &crap2,
					text+i+2, &increment, &crap3, &crap4);

			}
		     else if(isdigit(text[i+1]) && isdigit(text[i+2])){
			strncpy(buff, text+i+1, 2);
			buff[2] = ' ';
			sscanf(buff, "%d", &value);
			pstsize(value);
		     }
		     else{
			fprintf(stderr, "Unrecognised control:\n %c%c\n",
				text[i+1], text[i+2]);
		     }
		     i = i+increment;
		     first = i+1;
		   } 

		}
		length2 = i - first;
			if(jis_mode){
				jis_char(&crap1, &crap2,
				   text+first, length2, &crap3, &crap4);
			}
			else{
				ps_tout( text + first, length2);
			}

	/* switch off kanji mode */
	jis_mode = False;

}


/* save current state of font */
void
savfont_(void)
{
	old_font_size = the_font_size;
	old_style_id = current_style_id;
	old_family_id = current_family_id;
}

/* return font to previous properties */
void
rstfont_(void)
{
	font_resize(old_font_size);
	font_family(old_family_id);
	font_style(old_style_id);
}



/* derive a font name to call XLoadQueryFont with */
void
set_font_name(char fn[], short mode)
{
short	id, i;		


		id = current_family_id * NO_OF_STYLES + current_style_id;
/* It appears that DECstations and Suns have similar fonts but
 * with names specified in slightly different ways - is there
 * a better way to do this? Why are font names apparently different? */
		if(mode == 2)
		   sprintf(fn, "%s%d", font_names[id], the_font_size);
		else
		   sprintf(fn, "%s-%d", font_names[id], the_font_size);
		/* convert name to lower case */
    		for (i = strlen(fn) - 1; i >= 0; i--){
			if (isupper(fn[i]))
	    		   fn[i] = tolower(fn[i]);
			if(fn[i] == '-' && mode == 2){
			   fn[i] = '_';
			}
		}
}


/* ISO encode a requested font family */
void
ISO_encode(int ISO_family)
{
	short           ifont, istart;

	istart = ISO_family * NO_OF_STYLES;
	for (ifont = istart; ifont < istart + NO_OF_STYLES; ifont++) {
		fprintf(fp, "/%s findfont\n\
	dup length dict begin\n\
	{1 index /FID ne {def} {pop pop} ifelse} forall\n\
	/Encoding ISOLatin1Encoding def\n\
	currentdict\n\
	end\n\
	/%s%s exch definefont pop\n",
			font_names[ifont], font_names[ifont], ISO);
	}
}
