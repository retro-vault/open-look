/* $Id: objutils.c,v 1.4 92/06/14 07:34:26 pturner Exp Locker: pturner $
 *
 * operations on objects (strings, lines, and boxes)
 *
 */
#include <string.h>

#include <stdio.h>
#include <math.h>
#include "globals.h"

double xconv(), yconv();

/*
 * find the nearest object to (x,y)
 */
int isactive_box(int boxno);
int isactive_line(int lineno);
int isactive_string(int strno);
void errwin(char * msg);
void get_graph_box(int i, boxtype * b);
void get_graph_line(int i, linetype * l);
void get_graph_string(int i, plotstr * s);
void set_action(int act);

void find_item(int gno, double x, double y, int * type, int *numb)
{
    int i;
    double tmp, xtmp1, ytmp1, xtmp2, ytmp2, m = 1e307;
    double dx, dy;
    boxtype box;
    linetype line;
    plotstr str;

    x = xconv(x);
    y = yconv(y);
    *type = -1;
    for (i = 0; i < MAXBOXES; i++) {
	get_graph_box(i, &box);
	if (isactive_box(i)) {
	    if (box.loctype == VIEW) {
		xtmp1 = box.x1;
		ytmp1 = box.y1;
		xtmp2 = box.x2;
		ytmp2 = box.y2;
	    } else {
		if (gno == box.gno) {
		    xtmp1 = xconv(box.x1);
		    ytmp1 = yconv(box.y1);
		    xtmp2 = xconv(box.x2);
		    ytmp2 = yconv(box.y2);
		} else {
		    continue;
		}
	    }
	    tmp = hypot((x - xtmp1), (y - ytmp1));
	    if (m > tmp) {
		*type = BOX;
		*numb = i;
		m = tmp;
	    }
	    tmp = hypot((x - xtmp1), (y - ytmp2));
	    if (m > tmp) {
		*type = BOX;
		*numb = i;
		m = tmp;
	    }
	    tmp = hypot((x - xtmp2), (y - ytmp1));
	    if (m > tmp) {
		*type = BOX;
		*numb = i;
		m = tmp;
	    }
	    tmp = hypot((x - xtmp2), (y - ytmp2));
	    if (m > tmp) {
		*type = BOX;
		*numb = i;
		m = tmp;
	    }
	}
    }
    for (i = 0; i < MAXLINES; i++) {
	get_graph_line(i, &line);
	if (isactive_line(i)) {
	    if (line.loctype == VIEW) {
		xtmp1 = line.x1;
		ytmp1 = line.y1;
		xtmp2 = line.x2;
		ytmp2 = line.y2;
	    } else {
		if (gno == line.gno) {
		    xtmp1 = xconv(line.x1);
		    ytmp1 = yconv(line.y1);
		    xtmp2 = xconv(line.x2);
		    ytmp2 = yconv(line.y2);
		} else {
		    continue;
		}
	    }
	    tmp = hypot((x - xtmp1), (y - ytmp1));
	    if (m > tmp) {
		*type = LINE;
		*numb = i;
		m = tmp;
	    }
	    tmp = hypot((x - xtmp2), (y - ytmp2));
	    if (m > tmp) {
		*type = LINE;
		*numb = i;
		m = tmp;
	    }
	}
    }
    for (i = 0; i < MAXSTR; i++) {
	get_graph_string(i, &str);
	if (isactive_string(i)) {
	    if (str.loctype == VIEW) {
		xtmp1 = str.x;
		ytmp1 = str.y;
	    } else {
		if (gno == str.gno) {
		    xtmp1 = xconv(str.x);
		    ytmp1 = yconv(str.y);
		} else {
		    continue;
		}
	    }
	    tmp = hypot((x - xtmp1), (y - ytmp1));
	    if (m > tmp) {
		*type = STRING;
		*numb = i;
		m = tmp;
	    }
	}
    }
}

int isactive_line(int lineno)
{
    if (0 <= lineno && lineno < MAXLINES)
	return (lines[lineno].active == ON);
    return (0);
}

int isactive_box(int boxno)
{
    if (0 <= boxno && boxno < MAXBOXES)
	return (boxes[boxno].active == ON);
    return (0);
}

int isactive_string(int strno)
{
    if (0 <= strno && strno < MAXSTR)
	return (pstr[strno].s[0]);
    return (0);
}

int next_line(void)
{
    int i;

    for (i = 0; i < MAXLINES; i++) {
	if (!isactive_line(i)) {
	    lines[i].active = ON;
	    return (i);
	}
    }
    errwin("Error - no lines available");
    return (-1);
}

int next_box(void)
{
    int i;

    for (i = 0; i < MAXBOXES; i++) {
	if (!isactive_box(i)) {
	    boxes[i].active = ON;
	    return (i);
	}
    }
    errwin("Error - no boxes available");
    return (-1);
}

int next_string(void)
{
    int i;

    for (i = 0; i < MAXSTR; i++) {
	if (!isactive_string(i)) {
	    return (i);
	}
    }
    errwin("Error - no strings available");
    return (-1);
}

void kill_box(int boxno)
{
    boxes[boxno].active = OFF;
}

void kill_line(int lineno)
{
    lines[lineno].active = OFF;
}

void kill_string(int stringno)
{
    pstr[stringno].active = OFF;
    pstr[stringno].s[0] = 0;
}

void do_boxes_proc(void)
{
    set_action(0);
    set_action(MAKE_BOX_1ST);
}

void do_lines_proc(void)
{
    set_action(0);
    set_action(MAKE_LINE_1ST);
}

void do_move_proc(void)
{
    set_action(0);
    set_action(MOVE_OBJECT_1ST);
}

void do_delete_object_proc(void)
{
    set_action(0);
    set_action(DEL_OBJECT);
}

void edit_objects_proc(void)
{
    set_action(0);
    set_action(EDIT_OBJECT);
}

int define_string(char * s, double wx, double wy)
{
    int i;

    i = next_string();
    if (i >= 0) {
	if (s != 0) {
	    strcpy(pstr[i].s, s);
	} else {
	    pstr[i].s[0] = 0;
	    pstr[i].active = OFF;
	}
	pstr[i].font = string_font;
	pstr[i].color = string_color;
	pstr[i].linew = string_linew;
	pstr[i].rot = string_rot;
	pstr[i].charsize = string_size;
	pstr[i].loctype = string_loctype;
	pstr[i].just = string_just;
	pstr[i].active = ON;
	if (string_loctype == VIEW) {
	    pstr[i].x = xconv(wx);
	    pstr[i].y = yconv(wy);
	    pstr[i].gno = -1;
	} else {
	    pstr[i].x = wx;
	    pstr[i].y = wy;
	    pstr[i].gno = cg;
	}
	return i;
    }
    return -1;
}

void strings_loc_proc(void)
{
    set_action(0);
    set_action(STR_LOC);
/*
    xv_set(xv_get(canvas, CANVAS_NTH_PAINT_WINDOW, 0), WIN_CURSOR, cursor_strloc, 0);
*/
}

void strings_ang_proc(void)
{
    set_action(0);
    set_action(STR_LOC1ST);
    /*
     * xv_set(xv_get(canvas, CANVAS_NTH_PAINT_WINDOW, 0),WIN_CURSOR,
     * cursor_strloc, 0);
     */
}

void strings_edit_proc(void)
{
    set_action(0);
    set_action(STR_EDIT);
/*
    xv_set(xv_get(canvas, CANVAS_NTH_PAINT_WINDOW, 0), WIN_CURSOR, cursor_strloc, 0);
*/
}

void do_clear_lines(void)
{
    int i;

    for (i = 0; i < MAXLINES; i++) {
	kill_line(i);
    }
}

void do_clear_boxes(void)
{
    int i;

    for (i = 0; i < MAXBOXES; i++) {
	kill_box(i);
    }
}

void do_clear_text(void)
{
    int i;

    for (i = 0; i < MAXSTR; i++) {
	kill_string(i);
    }
}
