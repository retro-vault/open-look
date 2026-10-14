/* $Id: compute.c,v 1.4 91/12/11 20:30:30 pturner Exp Locker: pturner $
 *
 * perform math between sets
 *
 */
#include <stdio.h>
#include <string.h>

#include "globals.h"

static double *xtmp, *ytmp;

void errwin(char * msg);
void fixupstr(char * val);
void scanner(char s[], double * x, double *y, int len, double *a, double *b, double *c, double *d, int lenscr, int i, int setno, int *errpos);
void update_set_status(int gno, int setno);
void updatesetminmax(int gno, int i);

void loadset(int gno, int selset, int toval, double startno, double stepno)
{
    int i, lenset;
    double *ltmp;

    if ((lenset = getsetlength(gno, selset)) <= 0) {
	char stmp[60];

	sprintf(stmp, "Length of set %d <= 0", selset);
	errwin(stmp);
	return;
    }
    xtmp = getx(gno, selset);
    ytmp = gety(gno, selset);
    switch (toval) {
    case 1:
	ltmp = xtmp;
	break;
    case 2:
	ltmp = ytmp;
	break;
    case 3:
	ltmp = ax;
	break;
    case 4:
	ltmp = bx;
	break;
    case 5:
	ltmp = cx;
	break;
    case 6:
	ltmp = dx;
	break;
    default:
	return;
    }
    for (i = 0; i < lenset; i++) {
	*ltmp++ = startno + i * stepno;
    }
    updatesetminmax(gno, selset);
    update_set_status(gno, selset);
}

/*
 * evaluate the expression in sscanstr and place the result in selset
 */
int formula(int gno, int selset, char sscanstr[])
{
    char stmp[64], tmpstr[512];
    int i = 0, errpos, lenset;

    if ((lenset = getsetlength(gno, selset)) <= 0) {
	sprintf(stmp, "Length of set %d = 0", selset);
	errwin(stmp);
	return 0;
    }
    xtmp = getx(gno, selset);
    ytmp = gety(gno, selset);
    strcpy(tmpstr, sscanstr);
    fixupstr(tmpstr);
    scanner(tmpstr, xtmp, ytmp, lenset, ax, bx, cx, dx, MAXARR, i, selset, &errpos);
    updatesetminmax(gno, selset);
    update_set_status(gno, selset);
    return (errpos);
}
