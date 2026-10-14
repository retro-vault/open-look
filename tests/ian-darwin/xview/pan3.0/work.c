#define _REGEX_RE_COMP
#include <regex.h>
#include <errno.h>
#include <string.h>
/*
Post A Note V3.0
Copyright (c) 1993, Jeffrey W. Bailey
All rights reserved.

Permission is granted to distribute this program in exact, complete
source form, which includes this copyright notice, as long as no fee
other than media and distribution cost is charged.

This program may not be used in whole, or in part, in any other manner
without prior written permission from the author.

This program may not be distributed in modified form without prior
written permission from the author.  In other words, patches may be
distributed, but modified source may not be distributed.

If there are any questions, comments or suggestions, the author may be
contacted at:

    jeff@rd1.interlan.com

    or

    Jeffrey Bailey
    Racal-Datacom, Inc.
    Mail Stop E-110
    1601 N. Harrison Parkway
    Sunrise, FL  33323-2899
*/
#include <unistd.h>

#include "pan.h"



extern char *re_comp();

extern FILE *errfp;

/*
    Build a list of notes with subjects matching the provided RE.
    If notvisible is set, only finds hidden or veiled matches.

    Return number of matches.
    Return -1 for bad RE.
    Return -2 for memory alloc failure.
*/
int LLM_delete(struct LLM_root * root, char * node);
int LLM_init(struct LLM_root * root, int size);
int buildnote(struct Note * np, int display, int adjust);
int makename(char * fname, struct Note * np);
int put_win(struct Note * np);
int refresh_popups(void);
int set_frame_title(struct Note * np, char * title);
int trim(char * start);
int update(struct Note * np);
int updateinfo(struct Note * np, int force);
int w_makefolder(char * name);
int w_validfolder(char * name, int display);

int w_matchingnotes(struct LLM_root * root, char * re, int notvisible)
    {
    int  count = 0;
    struct SubDir *sp;
    struct Note *np;
    struct Note **npp;
    char tbuf [MAXSEARCHLEN + 1];

    LLM_init(root, sizeof(struct Note *));
    if(re_comp(re) != 0)
        {
        return(-1);
        }
    sp = (struct SubDir *) LLM_first(&subdir_rt);
    while(sp != 0)
        {
        np = (struct Note *) LLM_first(&sp->note_rt);
        while(np != 0)
            {
            strcpy(tbuf, np->ntitle);
            if(strlen(tbuf) == 0) strcpy(tbuf, NOTITLE);
            if(re_exec(tbuf) == 1)
                {
                if((!notvisible) ||
                   (notvisible && np->state != Visible))
                    {
                    npp = (struct Note **) LLM_add(root);
                    if(npp == 0)
                        {
                        return(-2);
                        }
                    count++;
                    *npp = np;
                    }
                }
            np = (struct Note *) LLM_next(&sp->note_rt);
            }
        sp = (struct SubDir *) LLM_next(&subdir_rt);
        }
    return(count);
    }

int w_exposenote(struct Note * np, int display)
    {
    buildnote(np, display, FALSE);
    xv_set(np->title, PANEL_VALUE, np->ntitle, 0);
    set_frame_title(np, np->ntitle);
    reseticon(np);
    np->state = Visible;
    update(np);
    updateinfo(np, FORCE);
    }

int w_hidenote(struct Note * np)
    {
    np->state = Hidden;
    update(np);
    updateinfo(np, FORCE);
    textsw_reset(np->textsw, 0, 0);
    put_win(np);
    np->mapped = 0;
    }

int w_veilnote(struct Note * np)
    {
    np->state = Veiled;
    update(np);
    updateinfo(np, NOFORCE);
    textsw_reset(np->textsw, 0, 0);
    put_win(np);
    np->mapped = 0;
    }

int w_popupxy(int * x, int *y, int width, int height, int spacing)
    {
    Display *dpy;
    Xv_Screen screen;
    int  screen_num;
    int  screen_height;
    int  screen_width;
    Rect rect;

    dpy = (Display *) xv_get(main_frame, XV_DISPLAY);
    screen = (Xv_Screen) xv_get(main_frame, XV_SCREEN);
    screen_num = xv_get(screen, SCREEN_NUMBER);
    screen_height = DisplayHeight(dpy, screen_num);
    screen_width = DisplayWidth(dpy, screen_num);
    frame_get_rect(main_frame, &rect);
    rect.r_left += spacing;
    if((rect.r_left + width) > (screen_width - spacing))
        rect.r_left = screen_width - width - spacing;
    if((rect.r_top + rect.r_height + 2 * spacing + height) <
       screen_height)
        rect.r_top += (rect.r_height + spacing);
    else
        rect.r_top -= (height + 2 * spacing);
    *x = rect.r_left;
    *y = rect.r_top;
    }

/*
    Creates a new folder, with error reporting to display OR logfile.
    Calls routines to check name validity and to make the directory.
*/
int w_newfolder(char * name, int display)
    {
    struct SubDir *sp;
    Menu_item mi;
    int  result;

    if(w_validfolder(name, display) < 0) return 0;

    if((result = w_makefolder(name)) != 0)
        {
        if(display)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "Cannot create the new folder",
                    name,
                    strerror(result),
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            }
        else
            {
            fprintf(errfp, "%s:  Cannot create new folder {%s}:  %s\n",
                   myname, name, strerror(result));
            }
        return 0;
        }
    sp = (struct SubDir *) LLM_add(&subdir_rt);
    if(sp == 0)
        {
        int  t_errno;

        t_errno = errno;
        if(display)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "Cannot create the new folder",
                    name,
                    strerror(t_errno),
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            }
        else
            {
            fprintf(errfp,
                "%s:  Cannot create new folder {%s}, memory problem %s\n",
                myname, name, strerror(t_errno));
            }
        return 0;
        }
    memset((char *)sp, 0, sizeof(struct SubDir));
    strcpy(sp->subdir, name);
    LLM_init(&sp->note_rt, sizeof(struct Note));
    mi = xv_create(0, MENUITEM,
        MENU_STRING, sp->subdir,
        MENU_NOTIFY_PROC, newnote,
        0);
    xv_set(main_newnote, MENU_APPEND_ITEM, mi, 0);
    mi = xv_create(0, MENUITEM,
        MENU_STRING, sp->subdir,
        MENU_GEN_PULLRIGHT, gen_exposemenu,
        0);
    xv_set(main_expose, MENU_APPEND_ITEM, mi, 0);
    mi = xv_create(0, MENUITEM,
        MENU_STRING, sp->subdir,
        MENU_GEN_PULLRIGHT, gen_prtmenu,
        0);
    xv_set(main_print, MENU_APPEND_ITEM, mi, 0);

    refresh_popups();
    }

/*
    Determine if a folder name is valid.
    Returns  0 - ok
            -1 - failed
*/
int w_validfolder(char * name, int display)
    {
    char *cp;
    int  result;

    result = 0;
    trim(name);
    if(*name == 0) result = -1;
    cp = name;
    if(*cp == '.') result = -2;
    while(*cp)
        {
        if(*cp == '/') result = -3;
        if(*cp == ' ' || *cp == '\t') result = -4;
        cp++;
        }

    switch(result)
        {
        case 0 : /* ok */
            break;
        case -1 : /* no name */
            if(display)
                {
                notice_prompt(main_frame, 0,
                    NOTICE_MESSAGE_STRINGS,
                        "No folder name entered",
                        0,
                    NOTICE_BUTTON_YES, "Acknowledge",
                    NOTICE_NO_BEEPING, noticenobeep,
                    0);
                }
            else
                {
                fprintf(errfp, "%s:  No folder name supplied\n", myname);
                }
            return(-1);
            break;
        case -2 : /* begins with a . */
            if(display)
                {
                notice_prompt(main_frame, 0,
                    NOTICE_MESSAGE_STRINGS,
                        "Folder name cannot begin with a '.'",
                        0,
                    NOTICE_BUTTON_YES, "Acknowledge",
                    NOTICE_NO_BEEPING, noticenobeep,
                    0);
                }
            else
                {
                fprintf(errfp, "%s:  Folder name cannot begin with a '.'\n",
                        myname);
                }
            return(-1);
            break;
        case -3 : /* contains a / */
            if(display)
                {
                notice_prompt(main_frame, 0,
                    NOTICE_MESSAGE_STRINGS,
                        "Folder name cannot contain a '/'",
                        0,
                    NOTICE_BUTTON_YES, "Acknowledge",
                    NOTICE_NO_BEEPING, noticenobeep,
                    0);
                }
            else
                {
                fprintf(errfp, "%s:  Folder name cannot contain a '/'\n",
                        myname);
                }
            return(-1);
            break;
        case -4 : /* contains whitespace */
            if(display)
                {
                notice_prompt(main_frame, 0,
                    NOTICE_MESSAGE_STRINGS,
                        "Folder name cannot contain spaces or tabs",
                        0,
                    NOTICE_BUTTON_YES, "Acknowledge",
                    NOTICE_NO_BEEPING, noticenobeep,
                    0);
                }
            else
                {
                fprintf(errfp,
                        "%s:  Folder name cannot contain spaces or tabs\n",
                        myname);
                }
            return(-1);
            break;
        }
    return(0);
    }

/*
    Makes a new folder (sub-directory).
    Returns  0 - ok
             x = errno
*/
int w_makefolder(char * name)
    {
    if(mkdir(name, 0700) < 0)
        return(errno);
    else
        return(0);
    }

/*
    Moves a note from one folder to another
    Returns  0 - ok
            -1 = error
*/
int w_movenote(struct SubDir * src_sp, struct SubDir *dst_sp, struct Note * np, int display)
    {
    struct Note *tn;
    char src[MAXSUBDIR + 1];
    char dst[MAXSUBDIR + 1];
    char buf[MAXBUFLEN];
    char buf2[MAXBUFLEN];

    strcpy(src, src_sp->subdir);
    strcpy(dst, dst_sp->subdir);
    trim(src);
    trim(dst);
    if(strcmp(src, dst) == 0)
        {
        if(display)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "Source and destination folders cannot be the same",
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            }
        else
            {
            fprintf(errfp, "%s:  Move note failed:  Source and destination folders cannot be the same: %s\n",
                    myname, src);
            }
        return(-1);
        }

    sprintf(buf, "%s/%s", src, np->basename);
    sprintf(buf2, "%s/%s", dst, np->basename);
    if(rename(buf, buf2) < 0)
        {
        if(display)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "Note move failed",
                    strerror(errno),
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            }
        else
            {
            fprintf(errfp, "%s:  Note move failed: {%s:%s} to %s\n", myname,
                    src, np->title, dst);
            }
        return(-1);
        }
    sprintf(buf, "%s/%s.info", src, np->basename);
    sprintf(buf2, "%s/%s.info", dst, np->basename);
    if(rename(buf, buf2) < 0)
        {
        if(display)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "Note move failed",
                    strerror(errno),
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            }
        else
            {
            fprintf(errfp, "%s:  Note move failed: {%s:%s} to %s\n", myname,
                    src, np->title, dst);
            }
        return(-1);
        }
    tn = (struct Note *) add_sorted(&dst_sp->note_rt, np->ntitle);
    if(tn == 0)
        {
        if(display)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "Note move failed - Memory allocation failure",
                    strerror(errno),
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            }
        else
            {
            fprintf(errfp, "%s:  Note move failed (memory allocation failure %s): {%s:%s} to %s\n",
                    myname, strerror(errno), src, np->title, dst);
            }
        /* Best attempt to fix things */
        (void) rename(buf2, buf);
        sprintf(buf, "%s/%s", src, np->basename);
        sprintf(buf2, "%s/%s", dst, np->basename);
        (void) rename(buf2, buf);
        return(-1);
        }
    memcpy((char *)tn, (char *)np, sizeof(struct Note));
    LLM_delete(&(np->sp->note_rt), (char *)np);
    tn->sp = dst_sp;
    if(tn->mapped)
        {
        xv_set(tn->title, PANEL_CLIENT_DATA, tn, 0);
        xv_set(tn->hide, PANEL_CLIENT_DATA, tn, 0);
        xv_set(tn->actionmenu, MENU_CLIENT_DATA, tn, 0);
        }
    return(0);
    }

/*
    Best attempt to create a new note.
        sp       - SubDir for new note
        state    - Visible, Hidden, or Veiled
        rect     - Location & size (optional, may be 0)
        title    - Note title (optional, may be 0)
        textfile - Path of file for initial contents (optional, may be 0)
        display  - ERRONDISPLAY or ERRINLOG
*/
int w_newnote(struct SubDir * sp, NoteState state, Rect * rect, char * title, char * textfile, int display)
    {
    int  adjust;
    int  x, y, w, h;
    struct Note *np;
    FILE *fp;
    char fname[MAXBUFLEN];
 
    /* get good pos for new note */
    if(rect == 0)
        {
        if(notewidth == -1 || noteheight == -1)
            {
            w_popupxy(&x, &y, DEFWIDTH, DEFHEIGHT, DEFSPACING);
            adjust = TRUE; /* width will be adjusted */
            h = DEFHEIGHT;
            w = DEFWIDTH; /* set for safety's sake */
            }
        else
            {
            adjust = FALSE; /* width will NOT be adjusted */
            h = noteheight;
            w = notewidth;
            w_popupxy(&x, &y, w, h, DEFSPACING);
            }
        }
    else
        {
        adjust = FALSE;
        x = rect->r_left;
        y = rect->r_top;
        w = rect->r_width;
        h = rect->r_height;
        }
 
    np = (struct Note *)add_sorted(&sp->note_rt,
        title == 0 ? NOTITLE : title);
    if(np == 0)
        {
        if(display)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "Internal failure - memory allocation failure",
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            }
        else
            {
            fprintf(errfp,
                "%s:  Internal failure - memory allocation failure\n", myname);
            }
        return 0;
        }
 
    memset((char *)np, 0, sizeof(struct Note));
 
    sprintf(np->basename, NOTENAM, time(0), notecount++);
    np->sp = sp;
    np->crttime = time(0);
    np->state = state;
    np->rect.r_top = y;
    np->rect.r_left = x;
    np->rect.r_width = w;
    np->rect.r_height = h;
    if(title == 0)
        strcpy(np->ntitle, NOTITLE);
    else
        strcpy(np->ntitle, title);
 
    /* Make the file with 0 length */
    makename(fname, np);
    fp = fopen(fname, "w");
    if(fp != 0) fclose(fp);
 
    if(state == Visible)
        {
        if(!buildnote(np, display, adjust)) return 0;
        xv_set(np->title, PANEL_VALUE, np->ntitle, 0);
        set_frame_title(np, np->ntitle);
        reseticon(np);
        }
 
    if(textfile != 0)
        {
        Textsw_status status;

        xv_set(np->textsw,
            TEXTSW_STATUS, &status,
            TEXTSW_INSERT_FROM_FILE, textfile,
            TEXTSW_FIRST, 0,
            0);
        if(status != TEXTSW_STATUS_OKAY)
            {
            if(display)
                {
                notice_prompt(main_frame, 0,
                    NOTICE_MESSAGE_STRINGS,
                        "Text subwindow insert from file failed",
                        textfile,
                        0,
                    NOTICE_BUTTON_YES, "Acknowledge",
                    NOTICE_NO_BEEPING, noticenobeep,
                    0);
                }
            else
                {
                fprintf(errfp,
                    "%s:  Text subwindow insert from file %s failed\n",
                    myname, textfile);
                }
            }
        }

    update(np);
    updateinfo(np, FORCE);
    refresh_popups();
    }

int w_printnote(struct Note * np, int display)
    {
    int  pid;
    struct stat st;
    char *cp;
    char fname[MAXBUFLEN];
    char envtitle[MAXBUFLEN];
    char envfname[MAXBUFLEN];

    makename(fname, np);
    if(stat(fname, &st) < 0)
        {
        if(display)
            {
            notice_prompt(main_frame, 0,
                NOTICE_MESSAGE_STRINGS,
                    "The selected note is not accessible (no contents)",
                    fname,
                    strerror(errno),
                    0,
                NOTICE_BUTTON_YES, "Acknowledge",
                NOTICE_NO_BEEPING, noticenobeep,
                0);
            }
        else
            {
            fprintf(errfp,
              "%s:  The selected note is not accessible (no contents)\n\t%s\n",
              myname, strerror(errno));
            }
        return 0;
        }
    switch(pid = fork())
        {
        case -1 :
            if(display)
                {
                notice_prompt(main_frame, 0,
                    NOTICE_MESSAGE_STRINGS,
                        "Couldn't fork a new process (internal error)",
                        strerror(errno),
                        0,
                    NOTICE_BUTTON_YES, "Acknowledge",
                    NOTICE_NO_BEEPING, noticenobeep,
                    0);
                }
            else
                {
                fprintf(errfp,
                  "%s:  Couldn't fork a new process (internal error): %s\n",
                  myname, strerror(errno));
                }
            return 0;
            break;
        case  0 :
            cp = defaults_get_string(resources[RES_PCOMMAND].a,
                 resources[RES_PCOMMAND].b, RESDEF_PCOMMAND);
            sprintf(envtitle, "NOTETITLE=%s", np->ntitle);
            sprintf(envfname, "FILE=%s", fname);
            putenv(envtitle);
            putenv(envfname);
            execl("/bin/sh", "sh", "-c", cp, 0);
            _exit(0);
            break;
        default :
            (void) notify_set_wait3_func(main_frame, 
                child_death, pid);
            break;
        }
    }
