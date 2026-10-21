#ifndef lint
static char sccsid[]="@(#)xview.c	1.146 09/07/94 Copyright 1987-1992 Sun Microsystem, Inc." ;
#endif

/*  Copyright (c) 1987-1992 Sun Microsystems, Inc.
 *  All Rights Reserved.
 *
 *  Sun considers its source code as an unpublished, proprietary
 *  trade secret, and it is available only under strict license
 *  provisions.  This copyright notice is placed here only to protect
 *  Sun in the event the source is deemed a published work.  Dissassembly,
 *  decompilation, or other means of reducing the object code to human
 *  readable form is prohibited by the license agreement under which
 *  this code is provided to the user or company in possession of this
 *  copy.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the
 *  Government is subject to restrictions as set forth in subparagraph
 *  (c)(1)(ii) of the Rights in Technical Data and Computer Software
 *  clause at DFARS 52.227-7013 and in similar clauses in the FAR and
 *  NASA FAR Supplement.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
/* Linux portability definitions are in linux_port.h. */
#include <X11/Xlib.h>

#ifdef SVR4
/* Linux portability definitions are in linux_port.h. */
#endif /*SVR4*/

#include "patchlevel.h"
#include "defs.h"
#include "fm.h"
#include "xdefs.h"
#include <xview/notice.h>
#include <xview/textsw.h>
#include "ds_xlib.h"
#include "ds_popup.h"

#include "filemgr_ui.h"

unsigned short hotspot_image[] = {
#include "images/hotspot.cursor"
} ;

extern FmVars Fm ;
extern FMXlib X ;
extern char *Str[] ;

Attr_attribute INSTANCE ;

filemgr_comments_frame_objects *co  = NULL ;
filemgr_cpo_frame_objects     *cpo  = NULL ;
filemgr_find_frame_objects    *fo   = NULL ;
filemgr_fio_frame_objects     *fio  = NULL ;
filemgr_po_frame_objects      *po   = NULL ;
filemgr_rcp_frame_objects     *rcpo = NULL ;
filemgr_rename_frame_objects  *reno = NULL ;

static Notify_client client = (Notify_client) PIPE_CLIENT_NO ;

Menu fm_cmd_el_menu_proc        P((Menu, Menu_generate)) ;

static int get_win_no           P((Menu_item)) ;

static Notify_value read_from_pipe  P((Notify_client, int)) ;

static void done_popup          P((Frame)) ;
static void draw_cursor_icon    P((Drawable, int, int, int, Drawable)) ;
static void draw_selection_box  P((Drawable, int, int, int, int, int)) ;
static void draw_stencil        P((Drawable, int, int, int, int, XPixel,
                                   Drawable)) ;
static void file_stencil        P((Drawable, int, int, int, Drawable, int)) ;


/*ARGSUSED*/
void
activate_pattern(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
  set_item_int_attr(FM_FIND_FIND_BUTTON, FM_ITEM_INACTIVE, !value) ;
  set_item_int_attr(FM_FIND_PATTERNITEM, FM_ITEM_INACTIVE,
                    (value & FLDR) || (value & APP)) ;
  set_item_int_attr(FM_FIND_CASETOGGLE,  FM_ITEM_INACTIVE,
                    (value & FLDR) || (value & APP)) ;
}


void
add_com_text(buf)
char *buf ;
{
  textsw_insert(X->items[(int) FM_COM_TEXT], buf, strlen(buf)) ;
}


/*ARGSUSED*/
void
add_frame_menu_accel(wno, mtype)
int wno ;
enum menu_type mtype ;
{
#ifdef KYBDACC
  Frame frame ;
  Menu menu ;

  if (wno == WNO_TREE) frame = X->treeX.frame ;
  else                 frame = X->fileX[wno]->frame ;

  menu = X->menus[(int) mtype] ;
  XV_SET(frame, FRAME_MENU_ADD, menu, 0) ;
#endif /*KYBDACC*/
}


/*ARGSUSED*/
void
add_menu_accel(mtype, n, str)
enum menu_type mtype ;
int n ;
char *str ;
{
#ifdef KYBDACC
  Menu menu ;
  Menu_item mi ;

  menu = X->menus[(int) mtype] ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, n) ;
  XV_SET(mi, MENU_ACCELERATOR, str, 0) ;
#endif /*KYBDACC*/
}


void
add_menu_help(mtype, n, text)     /* Add help text to item n of menu menu. */
enum menu_type mtype ;
int n ;
char *text ;
{
  char mstr[MAXLINE] ;
  Menu menu ;
  Menu_item mi ;

  STRCPY(mstr, "filemgr:") ;
  STRCAT(mstr, text) ;
  menu = X->menus[(int) mtype] ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, n) ;
  XV_SET(mi, XV_HELP_DATA, strdup(mstr), 0) ;
}


void
begin_buffer_tree_segments()
{
  X->num_cachesegs = 0 ;
}


void
busy_cursor(on, wno, count)
Boolean on ;
int wno, *count ;
{
  Frame frame ;

  if (wno == WNO_TREE) frame = X->treeX.frame ;
  else                 frame = X->fileX[wno]->frame ;

  if (on)
    {
      if (*count == 0) XV_SET(frame, FRAME_BUSY, on, 0) ;
      (*count)++ ;
    }
  else
    {
      if (*count == 1) XV_SET(frame, FRAME_BUSY, on, 0) ;
      (*count)-- ;
    }
}


/*ARGSUSED*/
void
cancel_comments(item, event)
Panel_item item ;
Event *event ;
{
  do_dismiss_popup(FM_COM_FRAME) ;
}


/*ARGSUSED*/
void
cancel_rename(item, event)
Panel_item item ;
Event *event ;
{
  do_dismiss_popup(FM_REN_FRAME) ;
}


Menu_item
cc_paste_after(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fm_cc_goto_menu_proc(item, M_EL_PASTE_AFTER) ;
  return(item) ;
}


Menu_item
cc_paste_before(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fm_cc_goto_menu_proc(item, M_EL_PASTE_BEFORE) ;
  return(item) ;
}


Menu_item
cc_paste_bottom(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fm_cc_goto_menu_proc(item, M_EL_PASTE_BOTTOM) ;
  return(item) ;
}


Menu_item
cc_copy(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fm_cc_goto_menu_proc(item, M_EL_COPY) ;
  return(item) ;
}


Menu_item
cc_cut(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fm_cc_goto_menu_proc(item, M_EL_CUT) ;
  return(item) ;
}


Menu_item
cc_delete(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fm_cc_goto_menu_proc(item, M_EL_DELETE) ;
  return(item) ;
}


Menu_item
cc_paste_top(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fm_cc_goto_menu_proc(item, M_EL_PASTE_TOP) ;
  return(item) ;
}


void
change_custom_list_item(row)
int row ;
{
  Xv_opaque i = X->items[(int) FM_PO_CMD_LIST] ;
  Custom_Command *cc, *old_cc ;

  char *alias   = get_item_str_attr(FM_PO_CMD_MLABEL,  FM_ITEM_IVALUE) ;
  char *command = get_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE) ;
  char *prompt  = get_item_str_attr(FM_PO_CMD_PTEXT1,  FM_ITEM_IVALUE) ;
  int is_prompt = get_item_int_attr(FM_PO_CMD_PROMPT,  FM_ITEM_IVALUE) ;
  int is_output = get_item_int_attr(FM_PO_CMD_OUTPUT,  FM_ITEM_IVALUE) ;

  old_cc = (Custom_Command *) xv_get(i, PANEL_LIST_CLIENT_DATA, row) ;
  cc = (Custom_Command *)
       LINT_CAST(fm_malloc((unsigned) sizeof(Custom_Command))) ;
  cc->alias   = strdup(alias) ;
  cc->command = strdup(command) ;
  if (prompt != NULL) cc->prompt = strdup(prompt) ;
  cc->is_prompt = (is_prompt) ? strdup("false") : strdup("true") ;
  cc->is_output = (is_output) ? strdup("false") : strdup("true") ;

  if (strcmp(old_cc->alias,   cc->alias) ||
      strcmp(old_cc->command, cc->command)) Fm->custom_changed = TRUE ;
  if ((old_cc->is_prompt != cc->is_prompt) ||
      (old_cc->is_output != cc->is_output)) Fm->custom_changed = TRUE ;
  if (old_cc->prompt && cc->prompt && strcmp(old_cc->prompt, cc->prompt))
    Fm->custom_changed = TRUE ;

  XV_SET(i,
         PANEL_LIST_STRING,      row, cc->alias,
         PANEL_LIST_CLIENT_DATA, row, cc,
         0) ;
  FREE(old_cc->alias) ;
  FREE(old_cc->command) ;
  if (old_cc->prompt) FREE(old_cc->prompt) ;
  FREE((char *) old_cc) ;
}


void
change_goto_list_item(row, label, client_data)
int row ;
char *label, *client_data ;
{
  Xv_opaque i     = X->items[(int) FM_PO_GOTO_LIST] ;
  char *old_label = (char *) xv_get(i, PANEL_LIST_STRING,      row) ;
  char *old_data  = (char *) xv_get(i, PANEL_LIST_CLIENT_DATA, row) ;

  if (strcmp(old_label, label) || strcmp(old_data, client_data))
    Fm->goto_changed = TRUE ;
  XV_SET(i,
         PANEL_LIST_STRING,      row, label,
         PANEL_LIST_CLIENT_DATA, row, strdup(client_data),
         0) ;
}


Menu_item
change_tree_dir(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY)
    {
      Fm->tree_vertical = !Fm->tree_vertical ;
      set_item_int_attr(FM_PO_TREE_DIR, FM_ITEM_IVALUE, !Fm->tree_vertical) ;
      fm_scrollbar_scroll_to(WNO_TREE, FM_H_SBAR, 0) ;
      fm_scrollbar_scroll_to(WNO_TREE, FM_V_SBAR, 0) ;
      do_show_tree(TRUE, TRUE) ;
    }
  return(item) ;
}


Menu_item
check_floppy(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_check_floppy() ;
  return(item) ;
}


/*  Search through all the command line arguments looking for ones to do
 *  with window size, position or state (open/iconic). If found, then insert
 *  the appropriate information into the info[] array. Note that minimal
 *  checking is done to the size and positional information.
 */

int
check_ps_args(argc, argv, info)
int argc, info[MAXINFO] ;
char **argv ;
{
  int flags, i, x, y ;
  unsigned int h, w ;
  int found  = FALSE ;

  for (i = 0; i < MAXINFO; i++) info[i] = -1 ;

/*  These values taken from the devGuide generated filemgr_ui.c file gor
 *  main filemgr folder panes.
 */

  for (i = 0; i < argc; i++)
    {
           if (EQUAL(argv[i], "-Wi") || EQUAL(argv[i], "-iconic"))
        {
          found = TRUE ;
          info[(int) FM_INFO_ISTATE] = TRUE ;
        }
      else if (EQUAL(argv[i], "-Ws") || EQUAL(argv[i], "-size"))
        {
          found = TRUE ;
          if ((i+2) > argc) return(FALSE) ;
          info[(int) FM_INFO_WIDTH]  = atoi(argv[i+1]) ;
          info[(int) FM_INFO_HEIGHT] = atoi(argv[i+2]) ;
          i += 2 ;
        }
      else if (EQUAL(argv[i], "-Wp") || EQUAL(argv[i], "-position"))
        {
          found = TRUE ;
          if ((i+2) > argc) return(FALSE) ;
          info[(int) FM_INFO_XOPEN] = atoi(argv[i+1]) ;
          info[(int) FM_INFO_YOPEN] = atoi(argv[i+2]) ;
          i += 2 ;
        }
      else if (EQUAL(argv[i], "-WP") || EQUAL(argv[i], "-icon_position"))
        {
          found = TRUE ;
          if ((i+2) > argc) return(FALSE) ;
          info[(int) FM_INFO_XICON] = atoi(argv[i+1]) ;
          info[(int) FM_INFO_YICON] = atoi(argv[i+2]) ;
          i += 2 ;
        }
      else if (EQUAL(argv[i], "+Wi"))
        {
          found = TRUE ;
          info[(int) FM_INFO_ISTATE] = FALSE ;
        }
      else if (EQUAL(argv[i], "-geometry"))
        {
          found = TRUE ;
          if ((i+1) > argc) return(FALSE) ;
          flags = XParseGeometry(argv[i+1], &x, &y, &w, &h) ;
          if (WidthValue  & flags) info[(int) FM_INFO_WIDTH]  = w ;
          if (HeightValue & flags) info[(int) FM_INFO_HEIGHT] = h ;
          if (XValue      & flags) info[(int) FM_INFO_XOPEN]  = x ;
          if (YValue      & flags) info[(int) FM_INFO_YOPEN]  = y ;
          i++ ;
        }
      else if (EQUAL(argv[i], "-icon_font") || EQUAL(argv[i], "-WT"))
        {
          if ((i+1) > argc) return(FALSE) ;
          Fm->iconfont = strdup(argv[i+1]);
          i++ ;
        }
    }
  return(found) ;
}


void
check_stop_key(wno)
int wno ;
{
  Xv_opaque pw ;
  Event event ;

  if (wno == WNO_TREE) pw = X->treeX.pw ;
  else                 pw = X->fileX[wno]->pw ;

  while (xv_input_readevent(pw, &event, FALSE, FALSE, (Inputmask *) NULL))
    if (event_action(&event) == ACTION_STOP)
      {
        write_footer(wno, Str[(int) M_STOP_ABORT], TRUE) ;
        Fm->stopped = TRUE ;
        return ;
      }
}


Menu_item
cleanup_icons(item, op)     /* Cleanup icon view in the current window. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_cleanup_icons() ;
  return(item) ;
}


void
clear_area(wno, x, y, width, height)
int wno, x, y, width, height ;
{
  Canvas canvas ;
  Drawable xid ;
  Xv_Window pw_view ;

  if (wno == WNO_TREE) canvas = X->treeX.canvas ;
  else                 canvas = X->fileX[wno]->canvas ;
  FM_CANVAS_EACH_PAINT_WINDOW(canvas, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;
    XClearArea(X->display, xid, x, y, width, height, FALSE) ;
  FM_CANVAS_END_EACH
}


void
clear_list(item)
enum item_type item ;
{
  int nitems = get_item_int_attr(item, FM_ITEM_LROWS) ;

  XV_SET(X->items[(int) item], PANEL_LIST_DELETE_ROWS, 0, nitems, 0) ;
}


int
confirm_dir_delete(wno, name, is_delete)
int wno, is_delete ;
char *name ;
{
  char pathname[MAXPATHLEN] ;
  int reply ;

  SPRINTF(pathname, "%s/%s", Fm->file[wno]->path, name) ;
  if (is_delete)
    {
      if (Fm->delete_fdr_prompt)
        {
	  char buf[MAXPATHLEN];
	  SPRINTF(buf, Str[(int) M_SURE_DELFOLDER], pathname);
          reply = (int) notice_prompt(X->fileX[wno]->frame, NULL,
                                 NOTICE_MESSAGE_STRINGS,
				   buf,
                                   0,
                                 NOTICE_BUTTON, Str[(int) M_DELFOLDER], FM_YES,
                                 NOTICE_BUTTON, Str[(int) M_CANCEL], FM_CANCEL,
                                 0) ;
        }
      else reply = FM_YES ;
    }
  else
    {
      if (Fm->destroy_fdr_prompt)
        {
	  char buf[MAXPATHLEN];
	  SPRINTF(buf, Str[(int) M_SURE_DESFOLDER], pathname);
          reply = (int) notice_prompt(X->fileX[wno]->frame, NULL,
                                 NOTICE_MESSAGE_STRINGS,
				   buf,
                                   0,
                                 NOTICE_BUTTON, Str[(int) M_DESFOLDER], FM_YES,
                                 NOTICE_BUTTON, Str[(int) M_CANCEL], FM_CANCEL,
                                 0) ;
        }
      else reply = FM_YES ;
    }
  return(reply) ;
}


/*ARGSUSED*/
void
cpo_print_proc(item, event)
Panel_item item ;
Event *event ;
{
  if (do_cpo_print_proc() == FAILURE)
    XV_SET(item, PANEL_NOTIFY_STATUS, XV_ERROR, 0) ;
}


Menu_item
create_document(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) new_file(FALSE, Fm->newdoc) ;
  return(item) ;
}


Menu_item
create_folder(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) new_file(TRUE, Fm->newdir) ;
  return(item) ;
}


Menu_item
create_shell(item, op)       /* Create a tty window. */
Menu_item item ;
Menu_generate op ;
{
  char cmd[MAXPATHLEN] ;

  if (op == MENU_NOTIFY)
    {
      SPRINTF(cmd, "(cd %s;%s)", Fm->file[Fm->curr_wno]->path, Fm->shellname) ;
      if (fm_run_str(cmd, FALSE)) fm_showerr(cmd) ;
    }
  return(item) ;
}


/*  Create the custom print popup which allows the user to print all
 *  selected files with a special print method.
 */

Menu_item
custom_print_popup(item, op)
Menu_item item ;
Menu_generate op ;
{
  Menu_item reply ;

  reply = create_display_popup(item, op, X->items[(int) FM_CP_CPO_FRAME],
                               cpo_make) ;
  set_print_method() ;
  return(reply) ;
}


/* Print selected objects using bound print method or default print method. */

Menu_item
default_print_proc(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY)
    {
      fm_busy_cursor(TRUE, Fm->curr_wno) ;
      print_files(NULL, 1, Fm->curr_wno) ;  /* NULL = use default printing. */
      fm_busy_cursor(FALSE, Fm->curr_wno) ;
    }
  return(item) ;
}


Menu_item
delete(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op != MENU_NOTIFY) return(item) ;
  delete_selected(Fm->confirm_delete, Fm->curr_wno) ;
  return(item) ;
}


void
delete_text(wno, x, y, string)     /* Removes text in a folder pane. */
int wno, x, y ;
char *string ;
{
  Xv_Window pw_view ;
  int xid ;
  unsigned long color ;

  color = (Fm->color) ? Fm->Window_bg : Fm->white ;
  XSetForeground(X->display, X->GCftext, color) ;
  color = (Fm->color) ? Fm->Window_bg : Fm->white ;
  XSetBackground(X->display, X->GCftext, color) ;
  FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->canvas, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;

    XDRAWIMAGESTRING(X->display, xid, X->font_set, X->GCftext,
                x, y, string, strlen(string)) ;
  FM_CANVAS_END_EACH
  color = (Fm->color) ? Fm->Panel_fg : Fm->black ;
  XSetForeground(X->display, X->GCftext, color) ;
  color = (Fm->color) ? Fm->Window_bg : Fm->white ;
  XSetBackground(X->display, X->GCftext, color) ;
}


Menu_item
delete_wastebasket(item, op)    /* Delete all files in the wastebasket. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_delete_wastebasket() ;
  return(item) ;
}


void
display_each_folder(wno)
int wno ;
{
  Xv_Window pw_view ;

  FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->canvas, pw_view)
    do_display_folder(X->fileX[wno]->canvas, pw_view,
                   X->display, (Xv_Window) 0, (Xv_xrectlist *) 0) ;
  FM_CANVAS_END_EACH
}


/* Repaint Proc - displays files in a given folder. */

void
display_folder(canvas, pw, display, xid, xrects)
Canvas canvas ;                      /* Canvas handle. */
Xv_Window pw ;                       /* Pixwin/Paint window. */
Display *display ;                   /* Server connection. */
Window xid ;                         /* X11 window handle. */
Xv_xrectlist *xrects ;               /* Damage rectlist. */
{
  do_display_folder(canvas, pw, display, xid, xrects) ;
}


void
do_dismiss_popup(itype)
enum item_type itype ;
{
  XV_SET(X->items[(int) itype], FRAME_CMD_PUSHPIN_IN, FALSE, 0) ;
  XV_SET(X->items[(int) itype], XV_SHOW, FALSE, 0) ;
}


static void
done_popup(frame)
Frame frame ;
{
  int i ;

  for (i = 0; i < MAXPOPUPS; i++)
    if (frame == X->items[(int) Fm->popup_item[i]]) Fm->popup_wno[i] = -9 ;

  if (frame == X->items[(int) FM_PO_PO_FRAME])
    {
      if (check_prop_values(Fm->cur_prop_sheet))
        {
          XV_SET(X->items[(int) FM_PO_PROPS_STACK],
                 PANEL_VALUE, Fm->cur_prop_sheet,
                 0) ;
          return ;
        }
    }
  XV_SET(frame, XV_SHOW, FALSE, 0) ;
  if (frame == X->items[(int) FM_FIO_FIO_FRAME])
     Fm->fprops_showing = FALSE ;
}


/* Draw an icon in the drag cursor. */

static void
draw_cursor_icon(xid, x, y, size, image)
int x, y, size ;
Drawable xid, image ;
{
  X->GCval.ts_x_origin = x ;
  X->GCval.ts_y_origin = y ;
  X->GCval.stipple     = image ;
  X->GCmask = GCTileStipXOrigin | GCTileStipYOrigin | GCStipple ;

  XChangeGC(X->display, X->igc, X->GCmask, &X->GCval) ;
  XFillRectangle(X->display, xid, X->igc, x, y, size, size) ;
}


/* Draw a folder at a tree object's x & y coordinates. */

void
draw_folder(wno, t, type, selected)
int wno, selected ;
Tree_Pane_Object *t ;
BYTE type ;
{
  BYTE ftype ;
  Canvas c ;
  Drawable xid ;
  int xoff, yoff ;
  unsigned long bg, fg ;
  Xv_Window pw_view ;

  if (!t) return ;    /* Sanity check. */

/*  The Hitchhiker's Guide to down & dirty item drawing:  right now,
 *  filemgr cannot use an icon mask to make one Xlib drawing call
 *  to render an item -- the repaint proc needs to set the clipping
 *  on the same GC so the whole window is not painted.
 *
 *  So filemgr draws each portion of the item separately.  It draws
 *  the background color with the mask image, then draws the image
 *  over that using the foreground color.  In addition, it needs to
 *  make sure it does it for each paint window, hence the use of the
 *  FM_CANVAS_EACH_PAINT_WINDOW macro.
 *
 *  Once the repaint procs can be tuned to not depend on clipping to
 *  limit the are drawn, then we can use image masks to render the
 *  image in one Xlib call.
 */

  if (wno == WNO_TREE)
    {
      c    = X->treeX.canvas ;
      xoff = (Fm->tree_vertical) ? Fm->tree_Ymin : 0 ;
      yoff = (Fm->tree_vertical) ? 0 : Fm->tree_Ymin ;
    }
  else
    {
      c = X->fileX[wno]->pathX.canvas ;
      xoff = yoff = 0 ;
    }

  bg = selected ? Fm->Folder_fg : Fm->Folder_bg ;      /* Background color. */
  fg = selected ? Fm->Folder_bg : Fm->Folder_fg ;      /* Foreground color. */
  if (wno == WNO_TREE) ftype = (t->status & TOPEN) ? FT_DIR_OPEN : FT_DIR ;
  else                 ftype = type ;
  FM_CANVAS_EACH_PAINT_WINDOW(c, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;

    XClearArea(X->display, xid, t->Xpos - xoff, t->Ypos - yoff,
               GLYPH_WIDTH, GLYPH_HEIGHT, FALSE) ;
    draw_stencil(xid, t->Xpos - xoff, t->Ypos - yoff,
                 GLYPH_WIDTH, GLYPH_HEIGHT, bg,
                 get_generic_icon_xid(ftype, TRUE)) ;
    draw_stencil(xid, t->Xpos - xoff, t->Ypos - yoff,
                 GLYPH_WIDTH, GLYPH_HEIGHT, fg,
                 get_generic_icon_xid(ftype, FALSE)) ;
  FM_CANVAS_END_EACH
}


void
draw_icon(wno, x, y, size, fpo)       /* Draw icon on the screen. */
int wno, x, y, size ;
File_Pane_Object *fpo ;
{
  XPixel foreg, backg ;
  Xv_Window pw_view ;
  int xid ;

/*  The Hitchhiker's Guide to down & dirty item drawing:  right now,
 *  filemgr cannot use an icon mask to make one Xlib drawing call
 *  to render an item -- the repaint proc needs to set the clipping
 *  on the same GC so the whole window is not painted.
 *
 *  So filemgr draws each portion of the item separately.  It draws
 *  the background color with the mask image, then draws the image
 *  over that using the foreground color.  In addition, it needs to
 *  make sure it does it for each paint window, hence the use of the
 *  FM_CANVAS_EACH_PAINT_WINDOW macro.
 *
 *  Once the repaint procs can be tuned to not depend on clipping to
 *  limit the are drawn, then we can use image masks to render the
 *  image in one Xlib call.
 */

  foreg = fpo->icon_fg_index ;
  backg = fpo->icon_bg_index ;

  FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->canvas, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;

/* Draw selection box. */

    draw_selection_box(xid, x-4, y-4, size+8, size+6, fpo->selected) ;

/* Draw background color. */

    draw_stencil(xid, x, y, size, size, backg,
                 get_icon_xid(fpo, FM_ICON_MASK)) ;

/* Stencil icon on top. */

    draw_stencil(xid, x, y, size, size, foreg,
                 get_icon_xid(fpo, FM_ICON_IMAGE)) ;
  FM_CANVAS_END_EACH
}


void
draw_list_icon(wno, x, y, fpo)
int wno, x, y ;
File_Pane_Object *fpo ;
{
  XPixel foreg, backg ;
  Xv_Window pw_view ;
  int width, xid ;

/*  XXX: need to optimise to one xlib call which uses a mask image,
 *  once the repaint proc stops using a clip mask.  The two are incompatible.
 */

  foreg = fpo->icon_fg_index ;
  backg = fpo->icon_bg_index ;

  if (Fm->file[wno]->listopts) width = list_width(wno) ;
  else                         width = LIST_GLYPH_WIDTH ;

  FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->canvas, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;

/* Draw selection box. */

    draw_selection_box(xid, x-2, y-2, width+4, LIST_GLYPH_HEIGHT+4,
                       fpo->selected) ;

/* If on color monitor, draw background color with mask image. */

    draw_stencil(xid, x, y, LIST_GLYPH_WIDTH, LIST_GLYPH_HEIGHT, backg,
                 get_generic_list_icon_xid(fpo->type, FM_ICON_MASK)) ;

/* Stencil icon on top. */

    draw_stencil(xid, x, y, LIST_GLYPH_WIDTH, LIST_GLYPH_HEIGHT, foreg,
                 get_generic_list_icon_xid(fpo->type, FM_ICON_IMAGE)) ;
  FM_CANVAS_END_EACH
}


static void
draw_selection_box(xid, x, y, width, height, selected)
Drawable xid ;
int x, y, width, height, selected ;
{
  Display *dpy = X->display ;
  GC       gc  = X->GCselect ;

  if (selected == FALSE) XClearArea(dpy, xid, x, y, width, height, FALSE) ;
  else
    {
      if (!Fm->color) XSetForeground(dpy, gc, Fm->black) ;
      else            XSetForeground(dpy, gc, X->xcolors[CMS_CONTROL_BG1]) ;

      XFillRectangle(dpy, xid, gc, x, y, width, height) ;
      if (!Fm->color) return ;

/* Lower right bevel. */

      XSetForeground(dpy, gc, X->xcolors[CMS_CONTROL_BG3]) ;
      XDrawLine(dpy, xid, gc,
                x, y + height - 1, x + width - 1, y + height - 1) ;
      XDrawLine(dpy, xid, gc,
                x + width - 1, y + height - 1, x + width - 1, y) ;

/* Upper left bevel. */

      XSetForeground(dpy, gc, X->xcolors[CMS_CONTROL_HIGHLIGHT]) ;
      XDrawLine(dpy, xid, gc, x, y + height - 1, x, y) ;
      XDrawLine(dpy, xid, gc, x, y, x + width - 1, y) ;
    }
}


/* Draw an icon on the screen. */

static void
draw_stencil(pw_xid, x, y, width, height, color_index, image)
int x, y, width, height ;
XPixel color_index ;
Drawable pw_xid, image ;
{
  X->GCval.ts_x_origin = x ;
  X->GCval.ts_y_origin = y ;
  X->GCval.stipple     = image ;
  X->GCval.foreground  = color_index ;
  X->GCmask = GCTileStipXOrigin | GCTileStipYOrigin |
                GCStipple | GCForeground ;

  XChangeGC(X->display, X->GCimage, X->GCmask, &X->GCval) ;
  XFillRectangle(X->display, pw_xid, X->GCimage, x, y, width, height) ;
}


void
draw_tree_line(x1, y1, x2, y2)
int x1, y1, x2, y2 ;
{
  XSegment *tmp ;

  if (x1 > MAX_CANVAS_SIZE || y1 > MAX_CANVAS_SIZE ||
      x2 > MAX_CANVAS_SIZE || y2 > MAX_CANVAS_SIZE) return ;
  tmp     = &(X->cached_segments[X->num_cachesegs++]) ;
  tmp->x1 = x1 ;
  tmp->y1 = y1 ;
  tmp->x2 = x2 ;
  tmp->y2 = y2 ;
  if (X->num_cachesegs == SEGCACHESIZE) flush_buffer_tree_segments() ;
}


void
draw_tree_text(t_p)
Tree_Pane_Object *t_p ;
{
  Drawable xid ;
  int xoff, yoff ;
  Xv_Window pw_view ;

  FM_CANVAS_EACH_PAINT_WINDOW(X->treeX.canvas, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;
    xoff = (Fm->tree_vertical) ? Fm->tree_Ymin : 0 ;
    yoff = (Fm->tree_vertical) ? 0 : Fm->tree_Ymin ;
    XDRAWIMAGESTRING(X->display, xid, X->font_set, X->GCttext,
                     t_p->Xpos - xoff,
                     t_p->Ypos - yoff + GLYPH_HEIGHT + Fm->ascent,
                     (char *) t_p->name, strlen((char *) t_p->name)) ;
  FM_CANVAS_END_EACH
}


Menu_item
duplicate_file(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_duplicate_file(Fm->curr_wno) ;
  return(item) ;
}


void
edit_button(item, event)                    /* Process edit button. */
Panel_item item ;
Event *event ;
{
  if (event_is_down(event))
    {
      Fm->curr_wno = (int) xv_get(item, XV_KEY_DATA, Fm->wno_key) ;
      set_menu_window_no(FM_EDIT_MENU, Fm->curr_wno) ;
      if (event_action(event) == ACTION_MENU ||
		 event_action(event) == ACTION_SELECT)
        set_menu_items(FM_EDIT_MENU, set_edit_button_menu) ;
    }

  panel_default_handle_event(item, event) ;
}


/*ARGSUSED*/
void
eject_disk_button_proc(item, event)
Panel_item item ;
Event *event ;
{
  do_eject_disk() ;
}


int
exists_prompt(wno, name)
int wno ;
char *name ;
{
  char mess[MAXPATHLEN];

  /* On DOS filesystems filename cannot have two dots. If name has one
     dot already, then do not give user option to alter name, becauase
     altered name will have a .number extension making it invalid
     for pcfs */
  if (Fm->file[wno]->mtype == FM_DOS) {
  	SPRINTF(mess,  Str[(int) M_EXISTS2],  name, name) ;
  	if (strchr(name, '.') != NULL)
  		return((int) notice_prompt(X->fileX[wno]->frame, NULL,
                             NOTICE_MESSAGE_STRINGS,
                               mess,
                               0,
                             NOTICE_BUTTON, Str[(int) M_EX_OVERWRITE], FM_YES,
                             NOTICE_BUTTON, Str[(int) M_CANCEL], FM_CANCEL,
                             0)) ;
  }
  SPRINTF(mess,  Str[(int) M_EXISTS],  name, name) ;
  return((int) notice_prompt(X->fileX[wno]->frame, NULL,
                             NOTICE_MESSAGE_STRINGS,
                               mess,
                               0,
                             NOTICE_BUTTON, Str[(int) M_EX_OVERWRITE], FM_YES,
                             NOTICE_BUTTON, Str[(int) M_EX_ALTER], FM_NO,
                             NOTICE_BUTTON, Str[(int) M_CANCEL], FM_CANCEL,
                             0)) ;
}


Menu_item
fi_basic(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fi_basic() ;
  return(item) ;
}


Menu_item
fi_extended(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_fi_extended() ;
  return(item) ;
}


void
file_button(item, event)                /* Process File button. */
Panel_item item ;
Event *event ;
{
  if (event_is_down(event))
    {
      Fm->curr_wno = (int) xv_get(item, XV_KEY_DATA, Fm->wno_key) ;
      set_menu_window_no(FM_FILE_MENU, Fm->curr_wno) ;
      if (event_action(event) == ACTION_MENU ||
		event_action(event) == ACTION_SELECT)
        set_menu_items(FM_FILE_MENU, set_file_button_menu) ;
    }

  panel_default_handle_event(item, event) ;
}


/* View by content. Rop icons' image onto the file pane. */

void
file_content(wno, x, y, size, off, image_depth)
int wno, x, y, size, off ;
BYTE image_depth ;
{
  Drawable xid ;
  Drawable image = Fm->file[wno]->object[off]->image ;
  int selected = Fm->file[wno]->object[off]->selected ;
  unsigned long fg ;
  Xv_Window pw_view ;

  FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->canvas, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;

    draw_selection_box(xid, x-4, y-4, size+8, size+6, selected) ;
    if (image_depth == 1)
      file_stencil(xid, x, y, size, image, selected) ;
    else
      {
        fg = (unsigned long) xv_get(X->cms, CMS_FOREGROUND_PIXEL) ;
        XSetForeground(X->display, X->GCimage, fg) ;
        XCopyArea(X->display, image, xid, X->GCimage,
                  0, 0, size, size, x, y) ;
      }
  FM_CANVAS_END_EACH
}


/* Stencil icon's image onto file pane. */

static void
file_stencil(xid, x, y, size, image, selected)
int x, y, size, selected ;
Drawable xid, image ;
{
  X->GCval.ts_x_origin = x ;
  X->GCval.ts_y_origin = y ;
  X->GCval.stipple     = image ;
  X->GCmask            = GCTileStipXOrigin | GCTileStipYOrigin | GCStipple ;

  if (Fm->color)
    {
      X->GCval.foreground = (unsigned long)
                            xv_get(X->cms, CMS_FOREGROUND_PIXEL) ;
      X->GCmask |= GCForeground ;
    }
  else
    {
      X->GCval.background = selected ? Fm->Folder_fg : Fm->Folder_bg ;
      X->GCval.foreground = selected ? Fm->Folder_bg : Fm->Folder_fg ;
      X->GCmask |= GCForeground | GCBackground ;
    }

  XChangeGC(X->display, X->GCimage, X->GCmask, &X->GCval) ;
  XFillRectangle(X->display, xid, X->GCimage, x, y, size, size) ;
}


Menu_item
file_info_popup(item, op)    /* Create and display the file info popup. */
Menu_item item ;
Menu_generate op ;
{
  Menu m ;
  int wno ;

  if (op == MENU_NOTIFY)
    {
      m   = (Menu) xv_get(item, MENU_PARENT) ;
      wno = (int) xv_get(m, MENU_CLIENT_DATA) ;
      do_file_info_popup(wno) ;
    }
  return(item) ;
}


void
file_text(wno, x, y, string, invert)     /* Draws text in a folder pane. */
int wno, x, y, invert ;
char *string ;
{
  File_Pane *f = Fm->file[wno] ;
  Xv_Window pw_view ;
  int xid ;
  unsigned long color ;

  if (invert)
    {
      color = Fm->white ;
      if (Fm->color && f->display_mode == VIEW_BY_LIST && f->listopts)
        color = Fm->black ;
      XSetForeground(X->display, X->GCftext, color) ;
      color = Fm->black ;
      if (Fm->color && f->display_mode == VIEW_BY_LIST && f->listopts)
        color = Fm->Panel_bg ;
      XSetBackground(X->display, X->GCftext, color) ;
    }
  FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->canvas, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;

    XDRAWIMAGESTRING(X->display, xid, X->font_set, X->GCftext,
                     x, y, string, strlen(string)) ;
  FM_CANVAS_END_EACH
  if (invert)
    {
      XSetForeground(X->display, X->GCftext, Fm->black) ;
      XSetBackground(X->display, X->GCftext, Fm->white) ;
    }
}


/*ARGSUSED*/
void
find_button(item, event)
Panel_item item ;
Event *event ;
{
  do_find_button() ;
}


void
find_do_connections()
{
  Notify_value find_child() ;

  notify_set_input_func((Notify_client) &Fm->Pid, find_stdout, Fm->pstdout[0]) ;
  notify_set_input_func((Notify_client) &Fm->Pid, find_stderr, Fm->pstderr[0]) ;
  notify_set_wait3_func(X->items[(int) FM_FIND_FIND_FRAME],
                        find_child, Fm->Pid) ;
}

Notify_value
find_interpose(frame, event, arg, type)
Frame frame ;
Event *event ;
Notify_arg arg ;
Notify_event_type type ;
{
  Notify_value rc ;

  rc = notify_next_event_func(frame, (Notify_event) event, arg, type) ;
  if (event_action(event) == WIN_RESIZE)
    {
      int w, sb_w, rows, footer_h ;

/* Resize the scrolling list. */

/* XXX: XView kludge: no way to find the footer height, so kludge it. */

      sb_w = sb_width(Fm->curr_wno) ;
      footer_h = sb_w + 3 ;                          /* Estimate. */

      w = (int) xv_get(frame, XV_WIDTH) - sb_w ;
      if (w <= 0) w = 1 ;

/*  The scrolling list height can only be set via PANEL_LIST_DISPLAY_ROWS,
 *  so estimate the number of rows based on frame height and scrolling
 *  list position.
 */

      rows = (int) xv_get(frame, XV_HEIGHT) -
             (int) get_item_int_attr(FM_FIND_FIND_LIST, FM_ITEM_Y) -
             (int) get_item_int_attr(FM_FIND_FIND_LIST, FM_ITEM_X) - footer_h ;

      rows /= (int) xv_get(X->items[(int) FM_FIND_FIND_LIST],
                           PANEL_LIST_ROW_HEIGHT) ;
      if (rows <= 0) rows = 1 ;

      XV_SET(X->items[(int) FM_FIND_FIND_LIST],
             PANEL_LIST_WIDTH,        w,
             PANEL_LIST_DISPLAY_ROWS, rows,
             0) ;
    }
  return(rc) ;
}


void
fio_activate_panel_item(item, event)
Panel_item item ;
Event *event ;
{
  do_fio_activate_panel_item(item, event) ;
}


/*ARGSUSED*/
void
fio_apply_proc(item, event)
Panel_item item ;
Event *event ;
{
  do_fio_apply_proc() ;
}


void
fio_clear_item(itype, is_text)
enum item_type itype ;
int is_text ;
{
  XV_SET(X->items[(int) itype],
         PANEL_VALUE,    (is_text ? "" : 0),
         PANEL_INACTIVE, TRUE,
         0) ;
}


void
fio_clear_toggle(n, is_text)
int n, is_text ;
{
  XV_SET(X->toggle[n],
         PANEL_VALUE,    (is_text ? "" : 0),
         PANEL_INACTIVE, TRUE,
         0) ;
}


void
fio_done_proc(frame)
Frame frame ;
{
  XV_SET(frame, XV_SHOW, FALSE, 0) ;
  Fm->fprops_showing = FALSE ;
}


/*ARGSUSED*/
void
fio_folder_by_content(item, value, event)
Panel_item item ;
unsigned int value ;
Event *event ;
{
  do_fio_folder_by_content() ;
}


void
fio_reset_proc(item, event)
Panel_item item ;
Event *event ;
{
  do_fio_reset_proc(item, event) ;
}


void
flush_buffer_tree_segments()
{
  Drawable xid ;
  Xv_Window pw_view ;

  FM_CANVAS_EACH_PAINT_WINDOW(X->treeX.canvas, pw_view)
    xid = (int) xv_get(pw_view, XV_XID) ;
    XDrawSegments(X->display, xid, X->GCline,
                  (XSegment *) &X->cached_segments, X->num_cachesegs) ;
  FM_CANVAS_END_EACH
  X->num_cachesegs = 0 ;
}


void
fm_add_help(item, str)
enum item_type item ;
char *str ;
{
  char *ptr = strdup(str) ;

  DS_ADD_HELP(X->items[(int) item], ptr) ;
}


Menu_item
fm_addparent_button(item, op)    /* Add tree root's parent. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) addparent_button() ;
  return(item) ;
}


void
fm_center_disk_buttons(panel, cancel, apply)
enum item_type panel, cancel, apply ;
{
  ds_center_items(X->items[(int) panel], -1,
                  X->items[(int) cancel], X->items[(int) apply], 0) ;
}


void
fm_center_info_buttons(panel, apply, reset)
enum item_type panel, apply, reset ;
{
  ds_center_items(X->items[(int) panel], -1,
                  X->items[(int) apply], X->items[(int) reset], 0) ;
}


void
fm_center_prop_buttons(panel, apply, def, reset)
enum item_type panel, apply, def, reset ;
{
  ds_center_items(X->items[(int) panel], -1, X->items[(int) apply],
                  X->items[(int) def], X->items[(int) reset], 0) ;
}


void
fm_center_rcp_buttons(panel, copy, cancel)
enum item_type panel, copy, cancel ;
{
  ds_center_items(X->items[(int) panel], -1, X->items[(int) copy],
                  X->items[(int) cancel], 0) ;
}


void
fm_center_rename_buttons(panel, rename, cancel)
enum item_type panel, rename, cancel ;
{
  ds_center_items(X->items[(int) panel], -1, X->items[(int) rename],
                  X->items[(int) cancel], 0) ;
}


/*ARGSUSED*/
int
fm_cmd_list_notify(item, string, client_data, op, event, row)
Panel_item item ;
char *string ;
Xv_opaque client_data ;
Panel_list_op op ;
Event *event ;
int row ;
{
  if (op == PANEL_LIST_OP_SELECT)
    do_custom_list_select((Custom_Command *) client_data) ;
  else if (op == PANEL_LIST_OP_DESELECT)
    if (!Fm->ignore_custom_desel) do_custom_list_deselect(row) ;
  return(XV_OK) ;
}


Menu_item
fm_copy_like_dnd(item, op)
Menu_item item ;
Menu_generate op ;
{
  return(do_dnd(item, op, copy_like_dnd)) ;
}


Menu_item
fm_cut_like_dnd(item, op)
Menu_item item ;
Menu_generate op ;
{
  return(do_dnd(item, op, cut_like_dnd)) ;
}


Menu_item
fm_delete_object(item, op)             /* Delete selected object. */
Menu_item item ;
Menu_generate op ;
{
  Menu m ;
  int wno ;

  if (op != MENU_NOTIFY) return(item) ;
  m   = (Menu) xv_get(item, MENU_PARENT) ;
  wno = (int)  xv_get(m, MENU_CLIENT_DATA) ;
  if (wno == WASTE) delete_selected(FALSE, WASTE) ;
  else              delete_selected(Fm->confirm_delete, wno) ;
  return(item) ;
}


void
fm_destroy_window(wno)
int wno ;
{
  XV_DESTROY(X->fileX[wno]->frame) ;
}


void
fm_draw_rect(wno, x1, y1, x2, y2)    /* Xor a rectangle onto folder pane. */
int wno, x1, y1, x2, y2 ;
{
  Drawable xid ;
  Xv_Window pw ;

  FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->canvas, pw)
    xid = (Drawable) xv_get(pw, XV_XID) ;

    XDrawLine(X->display, xid, X->GCmargc, x1, y1, x2, y1) ;
    XDrawLine(X->display, xid, X->GCmargc, x2, y1, x2, y2) ;
    XDrawLine(X->display, xid, X->GCmargc, x2, y2, x1, y2) ;
    XDrawLine(X->display, xid, X->GCmargc, x1, y2, x1, y1) ;
  FM_CANVAS_END_EACH
}


Menu_item
fm_expand_all_nodes(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) expand_all_nodes() ;
  return(item) ;
}


Menu_item
fm_find_object(item, op)
Menu_item item ;
Menu_generate op ;
{
  Menu m;
  if (op == MENU_NOTIFY)
    {
      find_object() ;
      m = (Menu) xv_get(item, MENU_PARENT) ;
      if ((int) xv_get(m, MENU_CLIENT_DATA) == WNO_TREE)
	Fm->popup_wno[(int) FM_FIND_POPUP] = WNO_TREE ;
      else
        Fm->popup_wno[(int) FM_FIND_POPUP] = Fm->curr_wno ;
    }
  return(item) ;
}


Menu_item
fm_hide_node(item, op)     /* Don't show selected folder's descendents. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) hide_node() ;
  return(item) ;
}


Menu_item
fm_link_like_dnd(item, op)
Menu_item item ;
Menu_generate op ;
{
  return(do_dnd(item, op, link_like_dnd)) ;
}


void
fm_menu_create(m)
enum menu_type m ;
{
  caddr_t i = (caddr_t) INSTANCE ;
  Menu menu ;
  Xv_opaque o = 0 ;

       if (m == FM_FILE_MENU)  menu = filemgr_file_button_menu_create(i, o) ;
  else if (m == FM_VIEW_MENU)  menu = filemgr_view_button_menu_create(i, o) ;
  else if (m == FM_EDIT_MENU)  menu = filemgr_edit_button_menu_create(i, o) ;
  else if (m == FM_GOTO_MENU)  menu = filemgr_goto_button_menu_create(i, o) ;
  else if (m == FM_PP_MENU)    menu = filemgr_pp_menu_create(i, o) ;
  else if (m == FM_TFILE_MENU) menu = filemgr_tree_file_menu_create(i, o) ;
  else if (m == FM_TVIEW_MENU) menu = filemgr_tree_view_menu_create(i, o) ;
  else if (m == FM_TP_MENU)    menu = filemgr_tp_menu_create(i, o) ;
  else if (m == FM_FP_MENU)    menu = filemgr_fp_menu_create(i, o) ;
  else if (m == FM_WEDIT_MENU) menu = filemgr_waste_edit_menu_create(i, o) ;
  else if (m == FM_WPANE_MENU) menu = filemgr_waste_pane_menu_create(i, o) ;
  X->menus[(int) m] = menu ;
  set_menu_default(m, Fm->menu_defs[(int) m]) ;
}


void
fm_popup_create(p)
enum popup_type p ;
{
       if (p == FM_FIND_POPUP)
    fo   = filemgr_find_frame_objects_initialize(NULL, X->base_frame) ;
  else if (p == FM_FIO_POPUP)
    fio  = filemgr_fio_frame_objects_initialize(NULL, X->base_frame) ;
  else if (p == FM_PO_POPUP)
    po   = filemgr_po_frame_objects_initialize(NULL,  X->base_frame) ;
  else if (p == FM_RCP_POPUP)
    rcpo = filemgr_rcp_frame_objects_initialize(NULL, X->base_frame) ;
  else if (p == FM_CP_POPUP)
    cpo  = filemgr_cpo_frame_objects_initialize(NULL, X->base_frame) ;
  else if (p == FM_COMMENTS_POPUP)
    co = filemgr_comments_frame_objects_initialize(NULL, X->base_frame) ;
  else if (p == FM_REN_POPUP)
    reno = filemgr_rename_frame_objects_initialize(NULL, X->base_frame) ;
  Fm->popup_wno[(int) p] = Fm->curr_wno ;
}


void
fm_position_popup(wno, itype)
int wno ;
enum item_type itype ;
{
  ds_position_popup(X->fileX[wno]->frame, X->items[(int) itype],
                    DS_POPUP_RIGHT) ;
}


Menu_item
fm_props(item, op)
Menu_item item ;
Menu_generate op ;
{
  Menu_item reply ;

  reply = create_display_popup(item, op, X->items[(int) FM_PO_PO_FRAME],
                               po_make) ;
  set_cur_folder_panel() ;
  return(reply) ;
}


void
fm_resize_text_item(panel, titem)
enum item_type panel, titem ;
{
  ds_resize_text_item(X->items[(int) panel], X->items[(int) titem]) ;
}


Menu_item
fm_remote_transfer(item, op)
Menu_item item ;
Menu_generate op ;
{
  File_Pane_Object **f_p, **l_p ;
  char *buffer ;
  int bufsize ;
  int len = 0 ;
  int sel = 0 ;
  int wno = Fm->curr_wno ;
  Menu_item mi ;

  mi = create_display_popup(item, op, X->items[(int) FM_RCP_RCP_FRAME],
			    rcp_make) ;

/*  Determine the buffer length. Each filename included extra space to
 *  correctly quote it, and the overall buffer length has a sloop factor
 *  of MAXLINE to allow for the possibility of filenames containing single
 *  quotes.
 */

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p != l_p; f_p++)
    if ((*f_p)->selected)
      {
        sel++ ;
        len += (*f_p)->flen + 12 ;
      }
  if (sel)
    bufsize = (strlen((char *) Fm->file[wno]->path) * sel) + len + MAXLINE ;
  else
    bufsize = MAXLINE ;
  buffer = fm_malloc((unsigned) bufsize) ;
  get_selections(wno, buffer, FALSE) ;

  set_item_str_attr(FM_RCP_SOURCE_MC, FM_ITEM_IVALUE,
                     (*buffer ? (char *) Fm->hostname : "")) ;
  set_item_str_attr(FM_RCP_SOURCE, FM_ITEM_IVALUE, buffer) ;
  FREE(buffer) ;

  return(mi) ;
}


void
fm_scrollbar_scroll_to(wno, stype, line)
int wno, line ;
enum fm_sb_type stype ;
{
  int attr ;
  Canvas c ;
  Scrollbar sb ;
  Xv_Window pw_view ;

  if (wno == WNO_TREE) c = X->treeX.canvas ;
  else                 c = X->fileX[wno]->canvas ;

  if (c == 0) return ;
  if ((pw_view = (Xv_Window) xv_get(c, OPENWIN_NTH_VIEW, 0)) != 0)
    {
      if (stype == FM_H_SBAR) attr = OPENWIN_HORIZONTAL_SCROLLBAR ;
      else                    attr = OPENWIN_VERTICAL_SCROLLBAR ;
      sb = (Scrollbar) xv_get(c, attr, pw_view) ;
      if (sb != 0) scrollbar_scroll_to(sb, line) ;
    }
}


Menu_item
fm_select_all(item, op)
Menu_item item ;
Menu_generate op ;
{
  register File_Pane_Object **f_p, **l_p ;   /* Files array pointers. */
  register int i ;                           /* Index. */
  int wno ;

  if (op != MENU_NOTIFY) return(item) ;
  wno = get_win_no(item) ;

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno), i = 0; f_p < l_p; f_p++, i++)
    if (!(*f_p)->selected)
      {
        (*f_p)->selected = TRUE ;
        draw_ith(i, wno) ;
      }
  if (Fm->fprops_showing) do_file_info_popup(wno) ;

  if (Fm->tree.selected)                 fm_treedeselect() ;
  if (Fm->file[wno]->path_pane.selected) fm_pathdeselect(wno) ;
  return(item) ;
}


void
fm_set_pan_cursor(wno, state)
int wno, state ;
{
  Canvas canvas ;
  Xv_Window pw_view ;

  if (wno == WNO_TREE) canvas = X->treeX.canvas ;
  else                 canvas = X->fileX[wno]->canvas ;

  FM_CANVAS_EACH_PAINT_WINDOW(canvas, pw_view)
    if (state == TRUE) XV_SET(pw_view, WIN_CURSOR, X->pan_cursor, 0) ;
    else               XV_SET(pw_view, WIN_CURSOR, X->def_cursor, 0) ;
  FM_CANVAS_END_EACH
}


Menu_item
fm_set_root(item, op)      /* Set tree root to current folder. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) set_root() ;
  return(item) ;
}


Menu_item
fm_show_tree(item, op)     /* Display tree view in separate window. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_show_tree(TRUE, TRUE) ;
  return(item) ;
}


void
fm_window_create(w, wno)
enum window_type w ;
int wno ;
{
  filemgr_mframe_objects *mo ;
  filemgr_tframe_objects *to ;
  filemgr_wframe_objects *wo ;

       if (w == FM_MAIN_FRAME)
    {
      mo = filemgr_mframe_objects_initialize(NULL, 0) ;
      X->fileX[wno]->frame        = mo->mframe ;
      X->fileX[wno]->panel        = mo->mcontrol_panel ;
      X->fileX[wno]->status_panel = mo->mstatus_panel ;
      X->fileX[wno]->canvas       = mo->mcanvas ;
      X->fileX[wno]->pathX.canvas = mo->mpcanvas ;
      X->fileX[wno]->Path_item    = mo->mgoto_line ;
      X->fileX[wno]->status_glyph = mo->mstatus_glyph ;
      X->fileX[wno]->leftmess     = mo->mstatus_left ;
      X->fileX[wno]->rightmess    = mo->mstatus_right ;
      X->fileX[wno]->file_button  = mo->mfile_button ;
      X->fileX[wno]->view_button  = mo->mview_button ;
      X->fileX[wno]->edit_button  = mo->medit_button ;
      X->fileX[wno]->goto_button  = mo->mgoto_button ;
      X->fileX[wno]->eject_button = mo->eject_button ;
    }
  else if (w == FM_TREE_FRAME)
    {
      to = filemgr_tframe_objects_initialize(NULL, 0) ;
      X->treeX.frame              = to->tframe ;
      X->treeX.panel              = to->tcontrol_panel ;
      X->treeX.canvas             = to->tcanvas ;
      X->treeX.file_button        = to->tfile_button ;
      X->treeX.view_button        = to->tview_button ;
      X->treeX.goto_button        = to->tgoto_button ;
    }
  else if (w == FM_WASTE_FRAME)
    {
      wo = filemgr_wframe_objects_initialize(NULL, 0) ;
      X->fileX[wno]->frame        = wo->wframe ;
      X->fileX[wno]->panel        = wo->wcontrol_panel ;
      X->fileX[wno]->status_panel = wo->wstatus_panel ;
      X->fileX[wno]->canvas       = wo->wcanvas ;
      X->fileX[wno]->pathX.canvas = wo->wpcanvas ;
      X->fileX[wno]->leftmess     = wo->wstatus_left ;
      X->fileX[wno]->rightmess    = wo->wstatus_right ;
      X->fileX[wno]->view_button  = wo->wview_button ;
      X->fileX[wno]->edit_button  = wo->wedit_button ;
      XV_SET(wo->wempty_button,
             XV_HELP_DATA, "filemgr:Waste_Empty_Wastebasket",
             0) ;
    }
}


/* Event procedure for all folder panes (main, waste and sub). */

Notify_value
folder_event(canvas, event, arg, type)
Xv_window canvas ;
Event *event ;
Notify_arg arg ;
Notify_event_type type ;
{
  Event_Context ec ;

  ec = setup_event_context(canvas, event, FALSE) ;
  if (X->fileX[ec->wno]->pw)
  	do_folder_event(ec) ;
  return(notify_next_event_func(canvas, (Notify_event) event, arg, type)) ;
}


Menu_item
format_disk(item, op)
Menu_item item ;
Menu_generate op ;
{
  int wno = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;

  if (op == MENU_NOTIFY)
    do_format_disk("format", Fm->frawpath[f->devno], Fm->fmntpt[f->devno]) ;
  return(item) ;
}


void
free_fp_image(f_p)
File_Pane_Object **f_p ;
{
  XFreePixmap(X->display, (Pixmap) (*f_p)->image) ;
  (*f_p)->image = 0 ;
}


void
free_panpix_image()
{
  XFreePixmap(X->display, X->panpix) ;
  X->panpix = 0 ;
}


int
get_canvas_attr(wno, ctype)
int wno ;
enum fm_can_type ctype ;
{
  Canvas c ;

  if (wno == WNO_TREE) c = X->treeX.canvas ;
  else                 c = X->fileX[wno]->canvas ;

  switch (ctype)
    {
      case FM_CAN_HEIGHT : return((int) xv_get(c, CANVAS_HEIGHT)) ;
      case FM_CAN_WIDTH  : return((int) xv_get(c, CANVAS_WIDTH)) ;
      case FM_WIN_HEIGHT : return((int) xv_get(c, XV_HEIGHT)) ;
      case FM_WIN_WIDTH  : return((int) xv_get(c, XV_WIDTH)) ;
      case FM_WIN_X      : return((int) xv_get(c, WIN_X)) ;
      case FM_WIN_Y      : return((int) xv_get(c, WIN_Y)) ;
    }
/*NOTREACHED*/
}


int
get_event_id()
{
       if (event_id(X->event) == LOC_WINENTER) return(E_WINENTER) ;
  else if (event_id(X->event) == LOC_WINEXIT)  return(E_WINEXIT) ;
  else if (event_id(X->event) == LOC_DRAG)     return(E_DRAG) ;
  else                                         return(E_OTHER) ;
}


int
get_item_int_attr(itype, atype)
enum item_type itype ;
enum item_attr_type atype ;
{
  Xv_opaque i = X->items[(int) itype] ;

  switch (atype)
    {
      case FM_ITEM_HEIGHT   : return((int) xv_get(i, XV_HEIGHT)) ;
      case FM_ITEM_INACTIVE : return((int) xv_get(i, PANEL_INACTIVE)) ;
      case FM_ITEM_IVALUE   : return((int) xv_get(i, PANEL_VALUE)) ;
      case FM_ITEM_LROWS    : return((int) xv_get(i, PANEL_LIST_NROWS)) ;
      case FM_ITEM_VALX     : return((int) xv_get(i, PANEL_VALUE_X)) ;
      case FM_ITEM_VALY     : return((int) xv_get(i, PANEL_VALUE_Y)) ;
      case FM_ITEM_WIDTH    : return((int) xv_get(i, XV_WIDTH)) ;
      case FM_ITEM_X        : return((int) xv_get(i, XV_X)) ;
      case FM_ITEM_X_GAP    : return((int) xv_get(i, PANEL_ITEM_X_GAP)) ;
      case FM_ITEM_Y        : return((int) xv_get(i, XV_Y)) ;
    }
/*NOTREACHED*/
}


char *
get_item_str_attr(itype, atype)
enum item_type itype ;
enum item_attr_type atype ;
{
  Xv_opaque i = X->items[(int) itype] ;

  switch (atype)
    {
      case FM_ITEM_IVALUE : return((char *) xv_get(i, PANEL_VALUE)) ;
    }
}


char *
get_list_client_data(itype, row)
enum item_type itype ;
int row ;
{
  return((char *) xv_get(X->items[(int) itype], PANEL_LIST_CLIENT_DATA, row)) ;
}


int
get_list_first_selected(itype)
enum item_type itype ;
{
  Xv_opaque list = X->items[(int) itype] ;

  return((int) xv_get(list, PANEL_LIST_FIRST_SELECTED)) ;
}


char *
get_list_str(itype, row)
enum item_type itype ;
int row ;
{
  return((char *) xv_get(X->items[(int) itype], PANEL_LIST_STRING, row)) ;
}


int
get_menu_default(mtype)
enum menu_type mtype ;
{
  Menu m = X->menus[(int) mtype] ;
  int val = -1 ;

  if (m) val = (int) xv_get(m, MENU_DEFAULT) ;
  return(val) ;
}


char *
get_path_item_value(wno)
int wno ;
{
  return((char *) xv_get(X->fileX[wno]->Path_item, PANEL_VALUE)) ;
}


int
get_pw_attr(wno, ctype)
int wno ;
enum fm_can_type ctype ;
{
  Xv_Window pw ;

  if (wno == WNO_TREE) pw = X->treeX.pw ;
  else                 pw = X->fileX[wno]->pw ;

  switch (ctype)
    {
      case FM_WIN_HEIGHT : return((int) xv_get(pw, XV_HEIGHT)) ;
      case FM_WIN_WIDTH  : return((int) xv_get(pw, XV_WIDTH)) ;
    }
/*NOTREACHED*/
}


char *
get_rename_str(wno)
int wno ;
{
  char *s ;

  if (!X->fileX[wno]->Rename_panel) return(NULL) ;
  s = (char *) xv_get(X->fileX[wno]->Rename_item, PANEL_VALUE) ;
  XV_SET(X->fileX[wno]->Rename_panel, XV_SHOW, FALSE, 0) ;
  return(s) ;
}


char *
get_resource(db, str)   /* Get filemgr resource from X resources database. */
enum db_type db ;
char *str ;
{
  XrmDatabase rDB ;

       if (db == FM_DESKSET_DB) rDB = X->desksetDB ;
  else if (db == FM_MERGE_DB)   rDB = X->rDB ;
  else if (db == FM_DIR_DB)     rDB = X->dirDB ;
  else                          rDB = X->old_ccmdDB ;
  return(ds_get_resource(rDB, Fm->appname, str)) ;
}


int
get_scrollbar_attr(wno, stype, sattr)
int wno ;
enum fm_sb_type stype ;
enum fm_sb_attr_type sattr ;
{
  int attr ;
  Canvas c ;
  Scrollbar sb ;
  Xv_Window pw_view ;

  if (wno == WNO_TREE) c = X->treeX.canvas ;
  else                 c = X->fileX[wno]->canvas ;

  pw_view = xv_get(c, OPENWIN_NTH_VIEW, 0) ;
  if (pw_view == 0) return(0) ;
  if (stype == FM_H_SBAR) attr = OPENWIN_HORIZONTAL_SCROLLBAR ;
  else                    attr = OPENWIN_VERTICAL_SCROLLBAR ;
  sb = (Scrollbar) xv_get(c, attr, pw_view) ;

  switch (sattr)
    {
      case FM_SB_LINE_HT  : return((int) xv_get(sb, SCROLL_LINE_HEIGHT)) ;
      case FM_SB_OBJ_LEN  : return((int) xv_get(sb, SCROLLBAR_OBJECT_LENGTH)) ;
      case FM_SB_VIEW_LEN : return((int) xv_get(sb, SCROLLBAR_VIEW_LENGTH)) ;
      case FM_SB_VIEW_ST  : return((int) xv_get(sb, SCROLLBAR_VIEW_START)) ;
    }
/*NOTREACHED*/
}


static int
get_win_no(item)
Menu_item item ;
{
  Event *event ;
  Menu menu ;

  menu  = (Menu)    xv_get(item, MENU_PARENT) ;
  event = (Event *) xv_get(menu, MENU_FIRST_EVENT) ;
  if (event && event_action(event) == ACTION_ACCELERATOR)
    return(Fm->curr_wno) ;
  else return((int) xv_get(menu, MENU_CLIENT_DATA)) ;
}


void
get_win_X_info(wno, info)     /* Return array of window pos. and dims. */
int wno, info[MAXINFO] ;
{
  Frame frame ;
  Rect *orect, wrect ;
  int ilocx, ilocy ;
  Xv_Window child ;

  if (wno == WNO_TREE) frame = X->treeX.frame ;
  else                 frame = X->fileX[wno]->frame ;

  orect = (Rect *) xv_get(frame, FRAME_OPEN_RECT) ;
  frame_get_rect(frame, &wrect) ;
  XTranslateCoordinates(X->display,
    xv_get(xv_get(frame, FRAME_ICON), XV_XID),
    xv_get(xv_get(xv_get(frame, FRAME_ICON), XV_ROOT), XV_XID),
    0, 0, &ilocx, &ilocy, &child) ;

  info[(int) FM_INFO_ISTATE] = (int) xv_get(frame, FRAME_CLOSED) ;
  info[(int) FM_INFO_XICON]  = ilocx ;
  info[(int) FM_INFO_YICON]  = ilocy ;
  info[(int) FM_INFO_XOPEN]  = wrect.r_left ;
  info[(int) FM_INFO_YOPEN]  = wrect.r_top ;
  info[(int) FM_INFO_WIDTH]  = orect->r_width ;
  info[(int) FM_INFO_HEIGHT] = orect->r_height ;
}


void
goto_button(item, event)    /* Generate most recent visited folders menu. */
Panel_item item ;
Event *event ;
{
  int wno ;

  if (event_is_down(event))
    {
      set_menu_items(FM_GOTO_MENU, set_goto_button_menu) ;
      wno = (int) xv_get(item, XV_KEY_DATA, Fm->wno_key) ;
      set_menu_window_no(FM_GOTO_MENU, wno) ;
    }

/* If user has clicked SELECT then set flag for testing in goto_menu_proc. */

  if (event_action(event) == ACTION_SELECT && event_is_up(event))
    Fm->isgoto_select = TRUE ;
  else
    Fm->isgoto_select = FALSE ;

  panel_default_handle_event(item, event) ;
}


Panel_setting
goto_line_proc(item, event)
Panel_item item ;
Event *event ;
{
  goto_object() ;
  return(panel_text_notify(item, event)) ;
}


/*ARGSUSED*/
int
goto_list_notify(item, string, client_data, op, event, row)
Panel_item item ;
char *string ;
Xv_opaque client_data ;
Panel_list_op op ;
Event *event ;
int row ;
{
  if (op == PANEL_LIST_OP_SELECT)
    do_goto_list_select(row, string, (char *) client_data) ;
  else if (op == PANEL_LIST_OP_DESELECT)
    if (!Fm->ignore_goto_desel) do_goto_list_deselect(row) ;
  return(XV_OK) ;
}


Menu_item
goto_menu_proc(item, op)
Menu_item item ;
Menu_generate op ;
{
  Menu m ;
  char *dir ;
  int i, info[MAXINFO], j, val, wno ;

  if (op == MENU_NOTIFY)
    {
      m   = (Menu) xv_get(item, MENU_PARENT) ;
      wno = (int)  xv_get(m, MENU_CLIENT_DATA) ;
      val = (int)  xv_get(item, XV_KEY_DATA, Fm->goto_key) ;
           if (val == WNO_TREE) do_show_tree(TRUE, TRUE) ;
      else if (val == WASTE)
        {
          if (fm_create_wastebasket() == FM_SUCCESS)
            {
              set_frame_attr(WASTE, FM_FRAME_CLOSED, FALSE) ;
              set_frame_attr(WASTE, FM_FRAME_SHOW,   TRUE) ;
              Fm->confirm_delete = TRUE ;
              set_item_int_attr(FM_PO_DELETE_I, FM_ITEM_IVALUE,
                                !Fm->confirm_delete) ;
            }
        }
      else if (val == MAIN_WIN)
        do_goto_menu(wno,
                     (char *) xv_get(item, XV_KEY_DATA, Fm->goto_path_key)) ;
      else if (val == MEDIA_WIN)
        {
          dir = (char *) xv_get(item, XV_KEY_DATA, Fm->goto_path_key) ;
          for (i = 0; i < MAXCD; i++)
            if (Fm->cmntpt[i] != NULL && EQUAL(Fm->cmntpt[i], dir))
              {
                for (j = 0; j < MAXINFO; j++) info[j] = -1 ;
                FM_NEW_FOLDER_WINDOW("", dir, Fm->crawpath[i],
                                     FM_CD, i, FALSE, info) ;
                return(item) ;
              }
          for (i = 0; i < MAXFLOPPY; i++)
            if (Fm->fmntpt[i] != NULL && EQUAL(Fm->fmntpt[i], dir))
              {
                for (j = 0; j < MAXINFO; j++) info[j] = -1 ;
                FM_NEW_FOLDER_WINDOW("", dir, Fm->frawpath[i],
                                     FM_FLOPPY, i, FALSE, info) ;
                return(item) ;
              }
        }
    }
  return(item) ;
}


/*  Init the GC's needed to draw icon images, text for both the tree and
 *  file and stenciling.
 */

void
init_gcs()
{
  Drawable xid ;
  XPixel fg, bg ;

  fg = (XPixel) xv_get((Cms) xv_get(X->base_frame, WIN_CMS),
                                    CMS_FOREGROUND_PIXEL) ;
  bg = (XPixel) xv_get((Cms) xv_get(X->base_frame, WIN_CMS),
                                    CMS_BACKGROUND_PIXEL) ;
  xid = (Drawable) xv_get(X->base_frame, XV_XID) ;

/* Init and create image gc. */

  X->GCval.foreground = fg ;
  X->GCval.background = bg ;
  X->GCval.fill_style = FillStippled ;

  X->GCmask   = GCForeground | GCBackground ;
  X->GCselect = (GC) XCreateGC(X->display, xid, X->GCmask, &X->GCval) ;

  X->GCmask = GCForeground | GCBackground | GCFillStyle ;
  X->GCimage = (GC) XCreateGC(X->display, xid, X->GCmask, &X->GCval) ;

/* Init and create tree's text gc. */

  X->GCttext = (GC) XCreateGC(X->display, xid, X->GCmask, &X->GCval) ;

/* Init and create file's text gc. */

  X->GCftext = (GC) XCreateGC(X->display, xid, X->GCmask, &X->GCval) ;

/* Init and create cursor gc. */

  X->GCcursor = (GC) XCreateGC(X->display, xid, X->GCmask, &X->GCval) ;

/* Init and create line gc. */

  X->GCline = (GC) XCreateGC(X->display, xid, X->GCmask, &X->GCval) ;

/* Init and create panning gc's. */

  X->GCmask = GCFunction | GCForeground | GCSubwindowMode ;
  X->GCval.foreground =
             ~((~0L) << DisplayPlanes(X->display,DefaultScreen(X->display))) ;
  X->GCval.function = GXxor ;
  X->GCval.subwindow_mode = IncludeInferiors ;
  X->GCmargc = XCreateGC(X->display, xid, X->GCmask, &X->GCval) ;

/*  DefaultGC macro can give back the wrong GCs multivisual displays.
 *  Create one instead.  Assuming XDefaultGC is the same as DefaultGC.
 */

  X->GCval.foreground = fg ;
  X->GCval.background = bg ;
  X->GCmask  = GCForeground | GCBackground ;
  X->GCpangc = XCreateGC(X->display, xid, X->GCmask, &X->GCval) ;

  if (!X->GCselect || !X->GCimage || !X->GCttext || !X->GCftext ||
      !X->GCline   || !X->GCmargc || !X->GCpangc || !X->GCcursor)
    ERR_EXIT(Str[(int) M_CREATE_GC]) ;
}


Boolean
is_frame(wno)
int wno ;
{
  Frame frame ;

  if (wno == WNO_TREE) frame = X->treeX.frame ;
  else                 frame = X->fileX[wno]->frame ;
  return(frame != 0) ;
}


Boolean
is_frame_closed(wno)
int wno ;
{
  Frame frame ;

  if (wno == WNO_TREE) frame = X->treeX.frame ;
  else                 frame = X->fileX[wno]->frame ;
  return((int) xv_get(frame, FRAME_CLOSED)) ;
}


Boolean
is_input_region(itype)
enum item_type itype ;
{
  return FALSE; /* This XView build has no input-method regions. */
}


Boolean
is_item(itype)
enum item_type itype ;
{
  return(X->items[(int) itype] != 0) ;
}


Boolean
is_menu(mtype)
enum menu_type mtype ;
{
  return(X->menus[(int) mtype] != 0) ;
}


Boolean
is_panpix()
{
  return(X->panpix != 0) ;
}


Boolean
is_popup_showing(itype)
enum item_type itype ;
{
  return(X->items[(int) itype] &&
         (int) xv_get(X->items[(int) itype], XV_SHOW)) ;
}


Boolean
is_resize_event()
{
  return(event_xevent(X->event)->xconfigure.send_event == 0) ;
}


Boolean
is_visible(wno)       /* Determine if the filemgr window is visible. */
int wno ;
{
  XWindowAttributes win_attr ;

  XGetWindowAttributes(X->display, X->fileX[wno]->xid, &win_attr) ;
  return(win_attr.map_state == IsViewable) ;
}


int
item_prompt(itype, str)
enum item_type itype ;
char *str ;
{
  return((int) notice_prompt(X->items[(int) itype], NULL,
                                      NOTICE_MESSAGE_STRINGS, str, 0,
                                      NOTICE_BUTTON, Str[(int) M_YES], FM_YES,
                                      NOTICE_BUTTON, Str[(int) M_NO],  FM_NO,
                                      0)) ;
}


void
keep_panel_up(itype)
enum item_type itype ;
{
  XV_SET(X->items[(int) itype], PANEL_NOTIFY_STATUS, XV_ERROR, 0) ;
}


void
link_comment_items()
{
  X->items[(int) FM_COM_FRAME]  = co->comments_frame ;
  X->items[(int) FM_COM_TEXT]   = co->comments_text ;
  Fm->popup_item[(int) FM_COMMENTS_POPUP] = FM_COM_FRAME ;
  XV_SET(X->items[(int) FM_COM_FRAME], FRAME_DONE_PROC, done_popup, 0) ;
}


void
link_cp_items()
{
  X->items[(int) FM_CP_CPO_FRAME]    = cpo->cpo_frame ;
  X->items[(int) FM_CP_CP_PANEL]     = cpo->cp_panel ;
  X->items[(int) FM_CP_METHOD_ITEM]  = cpo->method_item ;
  X->items[(int) FM_CP_COPIES_ITEM]  = cpo->copies_item ;
  X->items[(int) FM_CP_PRINT_BUTTON] = cpo->print_button ;
  Fm->popup_item[(int) FM_CP_POPUP] = FM_CP_CPO_FRAME ;
  XV_SET(X->items[(int) FM_CP_CPO_FRAME], FRAME_DONE_PROC, done_popup, 0) ;
}


void
link_find_items()
{
  X->items[(int) FM_FIND_FIND_FRAME]     = fo->find_frame ;
  X->items[(int) FM_FIND_FIND_PANEL]     = fo->find_panel ;
  X->items[(int) FM_FIND_FROMITEM]       = fo->Fromitem ;
  X->items[(int) FM_FIND_NAMEITEM]       = fo->Nameitem ;
  X->items[(int) FM_FIND_NAMETOGGLE]     = fo->Nametoggle ;
  X->items[(int) FM_FIND_OWNERITEM]      = fo->Owneritem ;
  X->items[(int) FM_FIND_OWNERTOGGLE]    = fo->Ownertoggle ;
  X->items[(int) FM_FIND_AFTERITEM]      = fo->Afteritem ;
  X->items[(int) FM_FIND_AFTER_DATE_PI]  = fo->after_date_pi ;
  X->items[(int) FM_FIND_BEFOREITEM]     = fo->Beforeitem ;
  X->items[(int) FM_FIND_BEFORE_DATE_PI] = fo->before_date_pi ;
  X->items[(int) FM_FIND_TYPEITEM]       = fo->Typeitem ;
  X->items[(int) FM_FIND_PATTERNITEM]    = fo->Patternitem ;
  X->items[(int) FM_FIND_CASETOGGLE]     = fo->Casetoggle ;
  X->items[(int) FM_FIND_FIND_BUTTON]    = fo->Find_button ;
  X->items[(int) FM_FIND_OPEN_BUTTON]    = fo->Open_button ;
  X->items[(int) FM_FIND_STOP_BUTTON]    = fo->Stop_button ;
  X->items[(int) FM_FIND_FIND_LIST]      = fo->Find_list ;
  Fm->popup_item[(int) FM_FIND_POPUP] = FM_FIND_FIND_FRAME ;
  XV_SET(X->items[(int) FM_FIND_FIND_FRAME], FRAME_DONE_PROC, done_popup, 0) ;
}


void
link_fio_items()
{
  X->items[(int) FM_FIO_FIO_FRAME]    = fio->fio_frame ;
  X->items[(int) FM_FIO_FI_PANEL]     = fio->fi_panel ;
  X->items[(int) FM_FIO_FILE_NAME]    = fio->file_name ;
  X->items[(int) FM_FIO_OWNER]        = fio->owner ;
  X->items[(int) FM_FIO_GROUP]        = fio->group ;
  X->items[(int) FM_FIO_BITE_SIZE]    = fio->bite_size ;
  X->items[(int) FM_FIO_MOD_TIME]     = fio->mod_time ;
  X->items[(int) FM_FIO_ACCESS_TIME]  = fio->access_time ;
  X->items[(int) FM_FIO_FILE_TYPE]    = fio->file_type ;
  X->items[(int) FM_FIO_PERMISSIONS]  = fio->permissions ;
  X->items[(int) FM_FIO_PERM_READ]    = fio->perm_read ;
  X->items[(int) FM_FIO_PERM_WRITE]   = fio->perm_write ;
  X->items[(int) FM_FIO_PERM_EXE]     = fio->perm_exe ;
  X->items[(int) FM_FIO_OWNER_PERM]   = fio->owner_perm ;
  X->items[(int) FM_FIO_GROUP_PERM]   = fio->group_perm ;
  X->items[(int) FM_FIO_WORLD_PERM]   = fio->world_perm ;
  X->items[(int) FM_FIO_OPEN_METHOD]  = fio->open_method ;
  X->items[(int) FM_FIO_PRINT_METHOD] = fio->print_method ;
  X->items[(int) FM_FIO_MOUNT_POINT]  = fio->mount_point ;
  X->items[(int) FM_FIO_MOUNT_FROM]   = fio->mount_from ;
  X->items[(int) FM_FIO_FREE_SPACE]   = fio->free_space ;
  X->items[(int) FM_FIO_APPLY_BUTTON] = fio->apply_button ;
  X->items[(int) FM_FIO_ICON_ITEM]    = fio->icon_item ;
  X->items[(int) FM_FIO_CONTENTS]     = fio->contents ;
  X->items[(int) FM_FIO_OWNER_R]      = fio->owner_r ;
  X->items[(int) FM_FIO_OWNER_W]      = fio->owner_w ;
  X->items[(int) FM_FIO_OWNER_E]      = fio->owner_e ;
  X->items[(int) FM_FIO_GROUP_R]      = fio->group_r ;
  X->items[(int) FM_FIO_GROUP_W]      = fio->group_w ;
  X->items[(int) FM_FIO_GROUP_E]      = fio->group_e ;
  X->items[(int) FM_FIO_WORLD_R]      = fio->world_r ;
  X->items[(int) FM_FIO_WORLD_W]      = fio->world_w ;
  X->items[(int) FM_FIO_WORLD_E]      = fio->world_e ;
  X->items[(int) FM_FIO_RESET_BUTTON] = fio->reset_button ;
  X->items[(int) FM_FIO_MORE_BUTTON]  = fio->more_button ;
  Fm->popup_item[(int) FM_FIO_POPUP] = FM_FIO_FIO_FRAME ;
  XV_SET(X->items[(int) FM_FIO_FIO_FRAME], FRAME_DONE_PROC, done_popup, 0) ;
}


void
link_po_items()
{
  Menu menu, smenu ;
  Menu_item mi ;

  menu = (Menu) xv_get(po->props_stack, PANEL_ITEM_MENU) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, PROPS_STACK_GENERAL+1) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Props_Category", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, PROPS_STACK_NEW_FOLDER+1) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Props_Category", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, PROPS_STACK_CUR_FOLDER+1) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Props_Category", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, PROPS_STACK_GOTO+1) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Props_Category", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, PROPS_STACK_CUSTOM+1) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Props_Category", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, PROPS_STACK_ADVANCED+1) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Props_Category", 0) ;

  X->items[(int) FM_PO_PO_FRAME]             = po->po_frame ;
  X->items[(int) FM_PO_PROPS_STACK]          = po->props_stack ;
  X->items[(int) FM_PO_PROPS_STACK_PANEL]    = po->props_stack_panel ;

  X->items[(int) FM_PO_PROPS_GEN_PANEL]      = po->props_general_panel ;
  X->items[(int) FM_PO_OPEN_FOLDER]          = po->open_folder ;
  X->items[(int) FM_PO_TREE_DIR]             = po->tree_dir ;
  X->items[(int) FM_PO_DELETE_I]             = po->delete_i ;
  X->items[(int) FM_PO_DEL_MES]              = po->eitem_del_mes ;
  X->items[(int) FM_PO_DES_MES]              = po->eitem_des_mes ;
  X->items[(int) FM_PO_DEFDOC]               = po->docname_i ;
  X->items[(int) FM_PO_GEN_APPLY]            = po->pgen_apply ;
  X->items[(int) FM_PO_GEN_DEFAULT]          = po->pgen_default ;
  X->items[(int) FM_PO_GEN_RESET]            = po->pgen_reset ;

  X->items[(int) FM_PO_PROPS_NEWF_PANEL]     = po->props_newf_panel ;
  X->items[(int) FM_PO_NEWF_DEFNAME]         = po->nf_defname ;
  X->items[(int) FM_PO_NEWF_NAMELEN]         = po->nf_namelen ;
  X->items[(int) FM_PO_NEWF_LAYOUT]          = po->nf_layout ;
  X->items[(int) FM_PO_NEWF_SORTBY]          = po->nf_sortby ;
  X->items[(int) FM_PO_NEWF_HIDDEN]          = po->nf_hidden ;
  X->items[(int) FM_PO_NEWF_DISPLAY]         = po->nf_display ;
  X->items[(int) FM_PO_NEWF_LISTOPTS]        = po->nf_listopts ;
  X->items[(int) FM_PO_NEWF_CONTENT]         = po->nf_content ;
  X->items[(int) FM_PO_NEW_APPLY]            = po->pnew_apply ;
  X->items[(int) FM_PO_NEW_DEFAULT]          = po->pnew_default ;
  X->items[(int) FM_PO_NEW_RESET]            = po->pnew_reset ;

  X->items[(int) FM_PO_PROPS_CUR_PANEL]      = po->props_current_panel ;
  X->items[(int) FM_PO_CURF_NAMELEN]         = po->cf_namelen ;
  X->items[(int) FM_PO_CURF_LAYOUT]          = po->cf_layout ;
  X->items[(int) FM_PO_CURF_DISPLAY]         = po->display_i ;
  X->items[(int) FM_PO_CURF_SORTBY]          = po->sort_i ;
  X->items[(int) FM_PO_CURF_HIDDEN]          = po->hidden_i ;
  X->items[(int) FM_PO_CURF_LISTOPTS]        = po->list_i ;
  X->items[(int) FM_PO_CURF_CONTENT]         = po->content_i ;
  X->items[(int) FM_PO_CUR_APPLY]            = po->pcur_apply ;
  X->items[(int) FM_PO_CUR_DEFAULT]          = po->pcur_default ;
  X->items[(int) FM_PO_CUR_RESET]            = po->pcur_reset ;

  X->items[(int) FM_PO_PROPS_GOTO_PANEL]     = po->props_goto_panel ;
  X->items[(int) FM_PO_GOTO_CLICK_MES]       = po->click_message ;
  X->items[(int) FM_PO_PATHNAME]             = po->goto_pathname ;
  X->items[(int) FM_PO_GOTO_ADD]             = po->goto_add ;
  X->items[(int) FM_PO_GOTO_MLABEL]          = po->goto_mlabel ;
  X->items[(int) FM_PO_GOTO_LIST]            = po->goto_list ;
  X->items[(int) FM_PO_GOTO_EDIT]            = po->goto_edit ;
  X->items[(int) FM_PO_GOTO_LAST_MES]        = po->last_message ;
  X->items[(int) FM_PO_GOTO_NUMBER]          = po->goto_number ;
  X->items[(int) FM_PO_GOTO_APPLY]           = po->pgoto_apply ;
  X->items[(int) FM_PO_GOTO_DEFAULT]         = po->pgoto_default ;
  X->items[(int) FM_PO_GOTO_RESET]           = po->pgoto_reset ;

  menu = (Menu) xv_get(po->goto_edit, PANEL_ITEM_MENU) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, M_EL_CUT) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, M_EL_COPY) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, M_EL_PASTE) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit", 0) ;
  smenu = (Menu) xv_get(mi, MENU_PULLRIGHT) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, M_EL_DELETE) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit", 0) ;

  mi   = (Menu_item) xv_get(smenu, MENU_NTH_ITEM, M_PASTE_BEFORE) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit_Paste", 0) ;
  mi   = (Menu_item) xv_get(smenu, MENU_NTH_ITEM, M_PASTE_AFTER) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit_Paste", 0) ;
  mi   = (Menu_item) xv_get(smenu, MENU_NTH_ITEM, M_PASTE_TOP) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit_Paste", 0) ;
  mi   = (Menu_item) xv_get(smenu, MENU_NTH_ITEM, M_PASTE_BOTTOM) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit_Paste", 0) ;

  X->items[(int) FM_PO_PROPS_CUSTOM_PANEL]   = po->props_custom_panel ;
  X->items[(int) FM_PO_CMD_LIST]             = po->cmd_list ;
  X->items[(int) FM_PO_NEW_COMMAND_BUTTON]   = po->new_command_button ;
  X->items[(int) FM_PO_EDIT_BUTTON]          = po->edit_button ;
  X->items[(int) FM_PO_CMD_MLABEL]           = po->cmd_mlabel ;
  X->items[(int) FM_PO_CMD_CMDLINE]          = po->cmd_cmdline ;
  X->items[(int) FM_PO_CMD_PROMPT]           = po->cmd_prompt ;
  X->items[(int) FM_PO_CMD_PTEXT1]           = po->cmd_ptext1 ;
  X->items[(int) FM_PO_CMD_OUTPUT]           = po->cmd_output ;
  X->items[(int) FM_PO_CUSTOM_APPLY]         = po->pcustom_apply ;
  X->items[(int) FM_PO_CUSTOM_DEFAULT]       = po->pcustom_default ;
  X->items[(int) FM_PO_CUSTOM_RESET]         = po->pcustom_reset ;

  menu = (Menu) xv_get(po->edit_button, PANEL_ITEM_MENU) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, M_EL_CUT) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, M_EL_COPY) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit", 0) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, M_EL_PASTE) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit", 0) ;
  smenu = (Menu) xv_get(mi, MENU_PULLRIGHT) ;
  mi   = (Menu_item) xv_get(menu, MENU_NTH_ITEM, M_EL_DELETE) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit", 0) ;

  mi   = (Menu_item) xv_get(smenu, MENU_NTH_ITEM, M_PASTE_BEFORE) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit_Paste", 0) ;
  mi   = (Menu_item) xv_get(smenu, MENU_NTH_ITEM, M_PASTE_AFTER) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit_Paste", 0) ;
  mi   = (Menu_item) xv_get(smenu, MENU_NTH_ITEM, M_PASTE_TOP) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit_Paste", 0) ;
  mi   = (Menu_item) xv_get(smenu, MENU_NTH_ITEM, M_PASTE_BOTTOM) ;
  XV_SET(mi, XV_HELP_DATA, "filemgr:Custom_Command_Goto_Edit_Paste", 0) ;

  X->items[(int) FM_PO_PROPS_ADVANCED_PANEL] = po->props_advanced_panel ;
  X->items[(int) FM_PO_PRINT_I]              = po->print_i ;
  X->items[(int) FM_PO_FILTER_I]             = po->filter_i ;
  X->items[(int) FM_PO_INTERVAL_I]           = po->interval_i ;
  X->items[(int) FM_PO_LINK_I]               = po->link_i ;
  X->items[(int) FM_PO_EDITOR_I]             = po->editor_i ;
  X->items[(int) FM_PO_OTHER_EDITOR_I]       = po->other_editor_i ;
  X->items[(int) FM_PO_MEDIA_M]              = po->media_m ;
  X->items[(int) FM_PO_CDROM_I]              = po->cdrom_i ;
  X->items[(int) FM_PO_CONTENT_M]            = po->content_mes ;
  X->items[(int) FM_PO_FLOPPY_CONTENT]       = po->floppy_content ;
  X->items[(int) FM_PO_CD_CONTENT]           = po->cdrom_content ;
  X->items[(int) FM_PO_ADV_APPLY]            = po->padvanced_apply ;
  X->items[(int) FM_PO_ADV_DEFAULT]          = po->padvanced_default ;
  X->items[(int) FM_PO_ADV_RESET]            = po->padvanced_reset ;
  Fm->popup_item[(int) FM_PO_POPUP] = FM_PO_PO_FRAME ;
  XV_SET(X->items[(int) FM_PO_PO_FRAME], FRAME_DONE_PROC, done_popup, 0) ;
}


void
link_rcp_items()
{
  X->items[(int) FM_RCP_RCP_FRAME]      = rcpo->rcp_frame ;
  X->items[(int) FM_RCP_REMOTE_PANEL]   = rcpo->remote_panel ;
  X->items[(int) FM_RCP_SOURCE_MC]      = rcpo->Source_mc ;
  X->items[(int) FM_RCP_SOURCE]         = rcpo->Source ;
  X->items[(int) FM_RCP_DESTINATION_MC] = rcpo->Destination_mc ;
  X->items[(int) FM_RCP_DESTINATION]    = rcpo->Destination ;
  X->items[(int) FM_RCP_COPY_BUTTON]    = rcpo->Copy_button ;
  X->items[(int) FM_RCP_CANCEL]         = rcpo->rcp_cancel ;
  Fm->popup_item[(int) FM_RCP_POPUP] = FM_RCP_RCP_FRAME ;
  XV_SET(X->items[(int) FM_RCP_RCP_FRAME], FRAME_DONE_PROC, done_popup, 0) ;
}


void
link_rename_items()
{
  X->items[(int) FM_REN_FRAME]  = reno->rename_frame ;
  X->items[(int) FM_REN_PANEL]  = reno->rename_panel ;
  X->items[(int) FM_REN_DNAME]  = reno->rename_dname ;
  X->items[(int) FM_REN_RENAME] = reno->rename_rename ;
  X->items[(int) FM_REN_CANCEL] = reno->rename_cancel ;
  Fm->popup_item[(int) FM_REN_POPUP] = FM_REN_FRAME ;
  XV_SET(X->items[(int) FM_REN_FRAME], FRAME_DONE_PROC, done_popup, 0) ;
}


/*ARGSUSED*/
int
list_notify(item, string, client_data, op, event, row)
Panel_item item ;
char *string ;
Xv_opaque client_data ;
Panel_list_op op ;
Event *event ;
int row ;
{
       if (op == PANEL_LIST_OP_SELECT)   do_list_notify(string, TRUE) ;
  else if (op == PANEL_LIST_OP_DESELECT) do_list_notify(string, FALSE) ;
  return(XV_OK) ;
}


void
load_deskset_defs()     /* Load current deskset resource database. */
{
  if (X->desksetDB != NULL) XrmDestroyDatabase(X->desksetDB) ;
  X->desksetDB = ds_load_deskset_defs() ;
}


void
load_dir_defs(fname)     /* Load default values for this directory. */
char *fname ;
{
  if (X->dirDB != NULL) XrmDestroyDatabase(X->dirDB) ;
  X->dirDB = XrmGetFileDatabase(fname) ;
}


void
load_resources()        /* Load combined X resources databases. */
{
  X->rDB = ds_load_resources(X->display) ;
}


void
load_old_ccmd_defs(fname)     /* Load old custom commands database. */
char *fname ;
{
  if (X->old_ccmdDB != NULL) XrmDestroyDatabase(X->old_ccmdDB) ;
  X->old_ccmdDB = XrmGetFileDatabase(fname) ;
}


Menu_item
make_comments(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY)
    {
      if (!is_item(FM_COM_FRAME))
        {
          fm_popup_create(FM_COMMENTS_POPUP) ;
          link_comment_items() ;
          write_questions() ;
          fm_position_popup(Fm->curr_wno, FM_COM_FRAME) ;
        }
      textsw_normalize_view(X->items[(int) FM_COM_TEXT], 0) ;
      XV_SET(X->items[(int) FM_COM_FRAME], FRAME_CMD_PUSHPIN_IN, TRUE, 0) ;
      set_item_int_attr(FM_COM_FRAME, FM_ITEM_SHOW, TRUE) ;
      Fm->popup_wno[(int) FM_COMMENTS_POPUP] = Fm->curr_wno ;
    }
  return(item) ;
}


unsigned long
make_compressed(wno, width, height, buf)
int wno, width, height ;
unsigned char *buf ;
{
  unsigned long gmask ;
  Pixmap pixmap ;
  XGCValues gval ;
  XImage *image ;

  pixmap = XCreatePixmap(X->display, X->fileX[wno]->xid, width, height, 1) ;
  if (X->igc == NULL)
    {
      gmask = GCForeground | GCBackground | GCGraphicsExposures ;
      gval.foreground = BlackPixel(X->display, DefaultScreen(X->display)) ;
      gval.background = WhitePixel(X->display, DefaultScreen(X->display)) ;
      gval.graphics_exposures = False ;
      X->igc = XCreateGC(X->display, pixmap, gmask, &gval) ;
    }
  XSetFillStyle(X->display, X->igc, FillSolid) ;
  image = XCreateImage(X->display, X->visual, 1, XYPixmap, 0, (char *) buf,
                       width, height, BitmapPad(X->display), 0) ;
  XPutImage(X->display, pixmap, X->igc, image, 0, 0, 0, 0, width, height) ;
  return((unsigned long) pixmap) ;
}


void
make_drag_cursor(ec)
Event_Context ec ;
{
  static Server_image hotspot_sv = 0 ;

  unsigned long gmask ;
  XGCValues gval ;
  Drawable xid ;
  char message[MAXLINE] ;
  int hotx, hoty, isize, x, y ;
  int wminx, wminy, wmaxx, wmaxy ;
  int wno = ec->wno ;
  int sel = 0 ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **l_p, **s_p ;
  Server_image sv ;

  if (hotspot_sv == 0)
    {
      hotspot_sv = xv_create(XV_NULL,            SERVER_IMAGE,
                             XV_WIDTH,           16,
                             XV_HEIGHT,          16,
                             SERVER_IMAGE_BITS,  hotspot_image,
                             SERVER_IMAGE_DEPTH, 1,
                             0) ;
    }

       if (f->display_mode == VIEW_BY_ICON) isize = GLYPH_HEIGHT ;
  else if (f->display_mode == VIEW_BY_LIST) isize = LIST_GLYPH_HEIGHT ;
  else                                      isize = CONTENT_GLYPH_HEIGHT ;

  wminx = Fm->drag_maxx = get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_LINE_HT) *
                 get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_VIEW_ST) ;
  wminy = Fm->drag_maxy = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_LINE_HT) *
                 get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_ST) ;
  wmaxx = Fm->drag_minx = wminx + get_canvas_attr(wno, FM_WIN_WIDTH) ;
  wmaxy = Fm->drag_miny = wminy + get_canvas_attr(wno, FM_WIN_HEIGHT) ;
  s_p  = PTR_FIRST(wno) ;
  l_p  = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    if ((*f_p)->selected)
      {
        sel++ ;
        x = get_file_obj_x(wno, *f_p) ;
        y = get_file_obj_y(wno, *f_p) ;
        if (x > wminx && x < Fm->drag_minx) Fm->drag_minx = x ;
        if (y > wminy && y < Fm->drag_miny) Fm->drag_miny = y ;
        if (x < wmaxx && x > Fm->drag_maxx) Fm->drag_maxx = x ;
        if (y < wmaxy && y > Fm->drag_maxy) Fm->drag_maxy = y ;
      }
  Fm->drag_maxx += isize ;
  Fm->drag_maxy += isize ;

  sv = xv_create(XV_NULL,            SERVER_IMAGE,
                 XV_WIDTH,           Fm->drag_maxx - Fm->drag_minx,
                 XV_HEIGHT,          Fm->drag_maxy - Fm->drag_miny,
                 SERVER_IMAGE_DEPTH, 1,
                 0) ;

  xid = (Drawable) xv_get(sv, XV_XID) ;
  if (X->igc == NULL)
    {
      gmask = GCForeground | GCBackground | GCGraphicsExposures ;
      gval.foreground = BlackPixel(X->display, DefaultScreen(X->display)) ;
      gval.background = WhitePixel(X->display, DefaultScreen(X->display)) ;
      gval.graphics_exposures = False ;
      X->igc = XCreateGC(X->display, xid, gmask, &gval) ;
    }
  XSetFunction(X->display, X->igc, GXclear) ;
  XCopyArea(X->display, xid, xid, X->igc,
            0, 0, Fm->drag_maxx - Fm->drag_minx, Fm->drag_maxy - Fm->drag_miny, 0, 0) ;
  XSetFunction(X->display, X->igc, GXset) ;
  XSetFillStyle(X->display, X->igc, FillStippled) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    if ((*f_p)->selected)
      {
        x = get_file_obj_x(wno, *f_p) ;
        y = get_file_obj_y(wno, *f_p) ;
        if (x < Fm->drag_minx || x > Fm->drag_maxx || y < Fm->drag_miny || y > Fm->drag_maxy) continue ;
        if (f->display_mode == VIEW_BY_ICON)
          draw_cursor_icon(xid, x - Fm->drag_minx, y - Fm->drag_miny, isize,
                           get_icon_xid(*f_p, FM_ICON_IMAGE)) ;
        else if (f->display_mode == VIEW_BY_LIST)
          draw_cursor_icon(xid, x - Fm->drag_minx, y - Fm->drag_miny, isize,
                     get_generic_list_icon_xid((*f_p)->type, FM_ICON_IMAGE)) ;
        else
          {
            if (is_image(wno, f_p - s_p) && (*f_p)->image_depth == 1)
              draw_cursor_icon(xid, x - Fm->drag_minx, y - Fm->drag_miny, isize, (*f_p)->image) ;
            else
              draw_cursor_icon(xid, x - Fm->drag_minx, y - Fm->drag_miny, GLYPH_HEIGHT,
                               get_icon_xid(*f_p, FM_ICON_IMAGE)) ;
          }
      }
  if (X->drag_cursor) XV_DESTROY_SAFE(X->drag_cursor) ;

  hotx = ec->start_x - Fm->drag_minx ;
  if (hotx < 0)    hotx = 0 ;
  if (hotx > Fm->drag_maxx) hotx = Fm->drag_maxx ;
  if (f->display_mode == VIEW_BY_LIST) Fm->drag_miny -= 15 ;
  hoty = ec->start_y - Fm->drag_miny ;
  if (hoty < 0)    hoty = 0 ;
  if (hoty > Fm->drag_maxy) hoty = Fm->drag_maxy ;
  draw_cursor_icon(xid, hotx, hoty, 16,
                   (Drawable) xv_get(hotspot_sv, XV_XID)) ;

  X->drag_cursor = xv_create(XV_NULL,      CURSOR,
                             CURSOR_IMAGE, sv,
                             CURSOR_XHOT,  hotx,
                             CURSOR_YHOT,  hoty,
                             CURSOR_OP,    PIX_SRC ^ PIX_DST,
                             0) ;
  SPRINTF(message, Str[(int) M_NSELECTED], sel) ;
  write_footer(wno, message, FALSE) ;
}


int
make_key()
{
  INSTANCE = xv_unique_key() ;
  return(INSTANCE) ;
}


void
mitem_inactive(mtype, n, state)        /* Set menu items activeness. */
enum menu_type mtype ;
int n, state ;
{
  Menu m = X->menus[(int) mtype] ;
  Menu_item mi ;

  mi = (Menu_item) xv_get(m, MENU_NTH_ITEM, n) ;
  XV_SET(mi, MENU_INACTIVE, state, 0) ;
}


void
mitem_label(mtype, n, str)
enum menu_type mtype ;
int n ;
WCHAR *str ;
{
  Menu m = X->menus[(int) mtype] ;
  Menu_item mi ;

  mi = (Menu_item) xv_get(m, MENU_NTH_ITEM, n) ;
  XV_SET(mi, MENU_STRING, str, 0) ;
}


void
mitem_set(mtype, n, value)
enum menu_type mtype ;
int n, value ;
{
  Menu m = X->menus[(int) mtype] ;
  Menu_item mi ;

  mi = (Menu_item) xv_get(m, MENU_NTH_ITEM, n) ;
  XV_SET(mi, MENU_VALUE, value, 0) ;
}


/*ARGSUSED*/
void
more_button_proc(item, event)
Panel_item item ;
Event *event ;
{
  if (Fm->info_extended) do_fi_basic() ;
  else                   do_fi_extended() ;
}


Menu_item
om_open_file(item, op)   /* File button - Open menu - file item. */
Menu_item item ;
Menu_generate op ;
{
  int wno ;

  if (op == MENU_NOTIFY)
    {
      wno = get_win_no(item) ;
      if (Fm->file[wno]->path_pane.selected)
        {
          fm_open_path(wno, Fm->newwin) ;
          Fm->tree.current = path_to_node(WNO_TREE, Fm->file[wno]->path) ;
          adjust_tree(wno) ;
          fm_showopen() ;
        }
      else open_files(wno, NULL_X, NULL_Y, NULL, Fm->newwin, FALSE) ;
    }
  return(item) ;
}


/*ARGSUSED*/
void
open_button(item, event)
Panel_item item ;
Event *event ;
{

  if (Fm->Found_chosen != NULL)
    fm_openfile(Fm->Found_chosen,
                get_item_str_attr(FM_FIND_PATTERNITEM, FM_ITEM_IVALUE),
                FALSE, Fm->curr_wno) ;
  else error(TRUE, Str[(int) M_SELFIRST], (char *) 0) ;
}


Menu_item
open_in_editor(item, op)
Menu_item item ;
Menu_generate op ;
{
  Menu m ;
  int wno ;

  if (op == MENU_NOTIFY)
    {
      m   = (Menu) xv_get(item, MENU_PARENT) ;
      wno = (int)  xv_get(m, MENU_CLIENT_DATA) ;
      open_files(wno, NULL_X, NULL_Y, NULL, TRUE, TRUE) ;
    }
  return(item) ;
}


Menu_item
open_tree_folder(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) fm_open_folder(FALSE) ;
  return(item) ;
}


Menu_item
paste_like_dnd_from_menu(item, op)
Menu_item item ;
Menu_generate op ;
{
  return(do_dnd(item, op, paste_like_dnd)) ;
}


/* Event interpose procedure for the path canvases. */

Notify_value
path_event(canvas, event, arg, typ)    /* Catch SELECT, MENU buttons. */
Canvas canvas ;
register Event *event ;
Notify_arg arg ;
Notify_event_type typ ;
{
  Event_Context ec ;

  ec = setup_event_context(canvas, event, TRUE) ;
  path_tree_event(ec) ;
  return(notify_next_event_func(canvas, (Notify_event) event, arg, typ)) ;
}


/*ARGSUSED*/
void
path_repaint(canvas, pw, display, xid, xrects)
Canvas canvas ;
Xv_Window pw ;
Display *display ;
Window xid ;
Xv_xrectlist *xrects ;
{
  draw_path((int) xv_get(pw, XV_KEY_DATA, Fm->wno_key), FALSE) ;
}


/*ARGSUSED*/
void
po_apply_proc(item, event)
Panel_item item ;
Event *event ;
{

  enum item_type def_itype,  res_itype;
  switch (Fm->cur_prop_sheet)
    {
      case PROPS_STACK_GENERAL    : def_itype = FM_PO_GEN_DEFAULT;
				    res_itype = FM_PO_GEN_RESET;
                                    break ;
      case PROPS_STACK_NEW_FOLDER : def_itype = FM_PO_NEW_DEFAULT;
				    res_itype = FM_PO_NEW_RESET;
                                    break ;
      case PROPS_STACK_CUR_FOLDER : def_itype = FM_PO_CUR_DEFAULT;
				    res_itype = FM_PO_CUR_RESET;
                                    break ;
      case PROPS_STACK_GOTO       : def_itype = FM_PO_GOTO_DEFAULT;
				    res_itype = FM_PO_GOTO_RESET;
                                    break ;
      case PROPS_STACK_CUSTOM     : def_itype = FM_PO_CUSTOM_DEFAULT;
				    res_itype = FM_PO_CUSTOM_RESET;
                                    break ;
      case PROPS_STACK_ADVANCED   : def_itype = FM_PO_ADV_DEFAULT;
				    res_itype = FM_PO_ADV_RESET;
                                    break ;
    }
  set_item_int_attr(def_itype, FM_ITEM_INACTIVE, TRUE);
  set_item_int_attr(res_itype, FM_ITEM_INACTIVE, TRUE);
  apply_prop_values(Fm->cur_prop_sheet) ;
  set_item_int_attr(def_itype, FM_ITEM_INACTIVE, FALSE);
  set_item_int_attr(res_itype, FM_ITEM_INACTIVE, FALSE);
}


/*ARGSUSED*/
void
po_cur_display_item_proc(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
  set_list_item(FALSE) ;
}


/*ARGSUSED*/
void
po_cur_list_item_proc(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
  set_layout_item(FALSE) ;
}


/*ARGSUSED*/
void
po_custom_add(item, event)
Panel_item item ;
Event *event ;
{
  do_po_custom_add() ;
}


/*ARGSUSED*/
void
po_default_proc(item, event)
Panel_item item ;
Event *event ;
{
  enum item_type app_itype,  res_itype;
  switch (Fm->cur_prop_sheet)
    {
      case PROPS_STACK_GENERAL    : app_itype = FM_PO_GEN_APPLY;
				    res_itype = FM_PO_GEN_RESET;
                                    break ;
      case PROPS_STACK_NEW_FOLDER : app_itype = FM_PO_NEW_APPLY;
				    res_itype = FM_PO_NEW_RESET;
                                    break ;
      case PROPS_STACK_CUR_FOLDER : app_itype = FM_PO_CUR_APPLY;
				    res_itype = FM_PO_CUR_RESET;
                                    break ;
      case PROPS_STACK_GOTO       : app_itype = FM_PO_GOTO_APPLY;
				    res_itype = FM_PO_GOTO_RESET;
                                    break ;
      case PROPS_STACK_CUSTOM     : app_itype = FM_PO_CUSTOM_APPLY;
				    res_itype = FM_PO_CUSTOM_RESET;
                                    break ;
      case PROPS_STACK_ADVANCED   : app_itype = FM_PO_ADV_APPLY;
				    res_itype = FM_PO_ADV_RESET;
                                    break ;
    }
  set_item_int_attr(app_itype, FM_ITEM_INACTIVE, TRUE);
  set_item_int_attr(res_itype, FM_ITEM_INACTIVE, TRUE);

  apply_prop_values(Fm->cur_prop_sheet) ;
       if (Fm->cur_prop_sheet == PROPS_STACK_CUSTOM) write_custom_commands() ;
  else if (Fm->cur_prop_sheet == PROPS_STACK_GOTO)   write_user_goto_menu() ;
  write_resources() ;

  set_item_int_attr(app_itype, FM_ITEM_INACTIVE, FALSE);
  set_item_int_attr(res_itype, FM_ITEM_INACTIVE, FALSE);
}


/*ARGSUSED*/
void
po_goto_add(item, event)
Panel_item item ;
Event *event ;
{
  do_po_goto_add() ;
}


/*ARGSUSED*/
void
po_new_display_item_proc(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
  set_list_item(TRUE) ;
}


/*ARGSUSED*/
void
po_new_list_item_proc(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
  set_layout_item(TRUE) ;
}


/*ARGSUSED*/
void
po_reset_proc(item, event)
Panel_item item ;
Event *event ;
{
  reset_prop_values(Fm->cur_prop_sheet) ;
}


/*ARGSUSED*/
void
po_show_other_editor(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
  set_item_int_attr(FM_PO_OTHER_EDITOR_I, FM_ITEM_SHOW,
    get_item_int_attr(FM_PO_EDITOR_I, FM_ITEM_IVALUE) == FM_OTHER_EDITOR) ;
}


void
pos_eitem_messages()
{
  Rect *r ;

  r = (Rect *) xv_get(X->items[(int) FM_PO_DELETE_I], PANEL_CHOICE_RECT, 0) ;
  set_item_int_attr(FM_PO_DEL_MES, FM_ITEM_X, r->r_left + r->r_width + 20) ;
  set_item_int_attr(FM_PO_DEL_MES, FM_ITEM_Y, r->r_top + 5) ;

  r = (Rect *) xv_get(X->items[(int) FM_PO_DELETE_I], PANEL_CHOICE_RECT, 1) ;
  set_item_int_attr(FM_PO_DES_MES, FM_ITEM_X, r->r_left + r->r_width + 20) ;
  set_item_int_attr(FM_PO_DES_MES, FM_ITEM_Y, r->r_top + 5) ;
}


Menu_item
pp_tp_open_dir(item, op)   /* File button - "Open Folder Window" menu item. */
Menu_item item ;
Menu_generate op ;
{
  Menu menu ;
  int wno ;

  if (op == MENU_NOTIFY)
    {
      menu = (Menu) xv_get(item, MENU_PARENT) ;
      wno  = (int)  xv_get(menu, MENU_CLIENT_DATA) ;
      if (wno == WNO_TREE) fm_open_folder(TRUE) ;
      else
        {
          fm_open_path(wno, Fm->newwin) ;
          Fm->tree.current = path_to_node(WNO_TREE, Fm->file[wno]->path) ;
          adjust_tree(wno) ;
          fm_showopen() ;
        }
    }
  return(item) ;
}


/*ARGSUSED*/
void
proceed_button(item, event)
Panel_item item ;
Event *event ;
{
  do_proceed_button() ;
}


int
prompt_with_message(wno, str, ystr, nstr, cstr)
int wno ;
char *str, *ystr, *nstr, *cstr ;
{
  return((int) notice_prompt(X->fileX[wno]->frame, NULL,
                                      NOTICE_MESSAGE_STRINGS, str, 0,
                                      NOTICE_BUTTON, ystr, FM_YES,
                                      NOTICE_BUTTON, nstr, FM_NO,
                                      NOTICE_BUTTON, cstr, FM_CANCEL,
                                      0)) ;
}


/*ARGSUSED*/
void
props_stack_menu(item, value, event)
Panel_item item ;
int value ;
Event *event ;
{
  if (check_prop_values(Fm->cur_prop_sheet))
    {
      XV_SET(item, PANEL_VALUE, Fm->cur_prop_sheet, 0) ;
      return ;
    }
  write_item_footer(FM_PO_PO_FRAME, "", FALSE) ;
  Fm->cur_prop_sheet = value ;
  adjust_prop_sheet_size(Fm->cur_prop_sheet) ;
}


/* Put X resource into appropriate resources database. */

void
put_resource(dbtype, str, value)
enum db_type dbtype ;
char *str, *value ;
{
  XrmDatabase *rDB = (dbtype == FM_DESKSET_DB) ? &X->desksetDB : &X->dirDB ;

  ds_put_resource(rDB, Fm->appname, str, value) ;
}


Notify_value
pw_folder_event(pw, event, arg, type)
Xv_window pw ;
Event *event ;
Notify_arg arg ;
Notify_event_type type ;
{
  int wno = (int) xv_get(pw, XV_KEY_DATA, Fm->wno_key) ;

  X->fileX[wno]->pw  = pw ;
  X->fileX[wno]->xid = (Drawable) xv_get(pw, XV_XID) ;
  return(folder_event(pw, event, arg, type)) ;
}


Notify_value
pw_tree_event(pw, event, arg, type)
Xv_window pw ;
Event *event ;
Notify_arg arg ;
Notify_event_type type ;
{
  int wno = (int) xv_get(pw, XV_KEY_DATA, Fm->wno_key) ;

  if (wno == WNO_TREE)
    {
      X->treeX.pw  = pw ;
      X->treeX.xid = (Drawable) xv_get(pw, XV_XID) ;
    }
  else
    {
      X->fileX[wno]->pathX.pw  = pw ;
      X->fileX[wno]->pathX.xid = (Drawable) xv_get(pw, XV_XID) ;
    }
  return(tree_event(pw, event, arg, type)) ;
}


int
quit_prompt(wno, str, ystr, cstr)
int wno ;
char *str, *ystr, *cstr ;
{
  int reply ;

  if (Fm->show_quit_prompt == FALSE) reply = FM_YES ;
  else
    reply = (int) notice_prompt(X->fileX[wno]->frame, NULL,
                                NOTICE_MESSAGE_STRINGS, str, 0,
                                NOTICE_BUTTON, ystr, FM_YES,
                                NOTICE_BUTTON, cstr, FM_CANCEL,
                                0) ;
  return(reply) ;
}


/*ARGSUSED*/
static Notify_value
read_from_pipe(client, fd)   /* Read from named pipe connection to vold. */
Notify_client client ;
int fd ;
{
  char buf[512] ;
  int bytes, n ;

  if (ioctl(fd, FIONREAD, &bytes) == 0)
    if ((n = read(fd, buf, sizeof(buf))) > 0)
      {
        if (Fm->show_cd)
          	check_media(FM_CD, MAXCD, &Fm->cd_mask, 0) ;

	if (Fm->show_floppy)
        	check_media(FM_FLOPPY, MAXFLOPPY, &Fm->floppy_mask, 0) ;
      }
  return(NOTIFY_DONE) ;
}


/*ARGSUSED*/
void
rcp_cancel_proc(item, event)
Panel_item item ;
Event *event ;
{
  XV_SET(X->items[(int) FM_RCP_RCP_FRAME], FRAME_CMD_PUSHPIN_IN, FALSE, 0) ;
  XV_SET(X->items[(int) FM_RCP_RCP_FRAME], XV_SHOW, FALSE, 0) ;
}


void
rcp_prompt(source_mc, target_mc)
char *source_mc, *target_mc ;
{
  char mess[MAXPATHLEN] ;

  SPRINTF(mess, Str[(int) M_RCP_FAIL], source_mc, target_mc) ;
  NOTICE_PROMPT(X->fileX[Fm->curr_wno]->frame, NULL,
                NOTICE_MESSAGE_STRINGS,
                  mess,
                  0,
                NOTICE_BUTTON, Str[(int) M_CANCEL], FM_CANCEL,
                0) ;
}


Menu_item
really_quit(item, op)
Menu_item item ;
Menu_generate op ;
{
  Menu m ;
  int reply, wno ;

  if (op == MENU_NOTIFY)
    {
      m   = (Menu) xv_get(item, MENU_PARENT) ;
      wno = (int)  xv_get(m, MENU_CLIENT_DATA) ;
      reply = quit_prompt(wno, Str[(int) M_QUIT_MES],
                               Str[(int) M_QUIT_FM], Str[(int) M_CANCEL]) ;
      if (reply == FM_CANCEL) return(item) ;
      if (Fm->confirm_delete && Fm->file[WASTE]->num_objects)
        {
          reply = prompt_with_message(wno, Str[(int) M_DO_DELETE],
                                      Str[(int) M_YES], Str[(int) M_NO],
                                      Str[(int) M_CANCEL]) ;
               if (reply == FM_CANCEL) return(item) ;
          else if (reply == FM_YES)    do_delete_wastebasket() ;
        }
      for (wno = 0; wno < Fm->num_win; wno++)
        if (is_frame(wno))
          save_directory_info(wno) ;
      UNLINK(Fm->err_file) ;     /* Erase the temporary error file. */
      yield_shelf() ;
      save_window_positions() ;
      UNLINK(Fm->pipe_name) ;
      exit(0) ;
    }
  return(item) ;
}


/*ARGSUSED*/
void
rename_disk(item, event)
Panel_item item ;
Event *event ;
{
  do_rename_disk() ;
}


Menu_item
rename_make(item, op)
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY)
    {
      do_rename_make() ;
      Fm->popup_wno[(int) FM_REN_POPUP] = Fm->curr_wno ;
    }
  return(item) ;
}


void
reposition_status(wno)   /* Reposition Eject and right status message. */
int wno ;
{
  int bw, bx, lw, lx, rx ;
  int swidth = (int) xv_get(X->fileX[wno]->rightmess, XV_WIDTH) ;
  int wwidth = get_canvas_attr(wno, FM_WIN_WIDTH) ;

  XV_SET(X->fileX[wno]->rightmess, XV_X, wwidth - swidth - 20, 0) ;

  if (X->fileX[wno]->eject_button)
    {
      lx = (int) xv_get(X->fileX[wno]->leftmess,     XV_X) ;
      rx = (int) xv_get(X->fileX[wno]->rightmess,    XV_X) ;
      lw = (int) xv_get(X->fileX[wno]->leftmess,     XV_WIDTH) ;
      bw = (int) xv_get(X->fileX[wno]->eject_button, XV_WIDTH) ;
      bx = lx + lw + ((rx - (lx + lw) - bw) / 2) ;
      XV_SET(X->fileX[wno]->eject_button, XV_X, bx, 0) ;
    }
}


void
resize_path_text_item(wno)
int wno ;
{
  ds_resize_text_item(X->fileX[wno]->panel, X->fileX[wno]->Path_item) ;
}


void
save_cmdline(argc, argv)
int argc ;
char *argv[] ;
{
  DS_SAVE_CMDLINE(X->base_frame, argc, argv) ;
}


void
save_dir_defs(fname)     /* Load default values for this directory. */
char *fname ;
{
  XrmPutFileDatabase(X->dirDB, fname) ;
}


void
save_resources()
{
  Xv_opaque reply = ds_save_resources(X->desksetDB) ;

  if (reply == XV_ERROR) fm_msg(TRUE, Str[(int) M_NORESSAVE]) ;
}


int
sb_width(wno)        /* Returns the width of a canvas scrollbar. */
int wno ;
{
  Xv_Window view, win ;
  Scrollbar sb ;
  int width ;

  if (wno == WNO_TREE) win = X->treeX.canvas ;
  else                 win = X->fileX[wno]->canvas ;

  view = (Xv_Window) xv_get(win, OPENWIN_NTH_VIEW, 0) ;
  if (!view) return(0) ;

/* Get a scrollbar from the view. */

  sb = (Scrollbar) xv_get(win, OPENWIN_VERTICAL_SCROLLBAR, view) ;
  if (!sb)
    sb = (Scrollbar) xv_get(win, OPENWIN_HORIZONTAL_SCROLLBAR, view) ;
  if (!sb) return(0) ;

  width = (int) xv_get(sb, XV_WIDTH) ;
  return(width) ;
}


Notify_value
scroll_event(client, event, sbar, type)
Notify_client client ;
Event *event ;
Scrollbar sbar ;
Notify_event_type type ;
{
  Event_Context ec ;
  Xv_opaque canvas = xv_get(client, XV_OWNER) ;

  ec = setup_event_context(canvas, event, FALSE) ;
  do_folder_event(ec) ;
  return(notify_next_event_func(client, (Notify_event) event, sbar, type)) ;
}


void
scrunch_window(wno, bheight)
int wno, bheight ;
{
  Canvas c ;
  Panel cp ;
  int diff, h, y ;

/* Force the panel to be the shortest height possible. */

  c  = (wno == WNO_TREE) ? X->treeX.canvas : X->fileX[wno]->canvas ;
  cp = (wno == WNO_TREE) ? X->treeX.panel  : X->fileX[wno]->panel ;
  h  = (int) xv_get(cp, XV_HEIGHT) ;

  XV_SET(cp, XV_HEIGHT, bheight + 6, 0) ;
  diff = h - bheight - 6 ;

/*  If not the tree pane, then move path pane and status panel up by adjusted
 *  amount.
 */

  if (wno != WNO_TREE)
    {
      y = (int) xv_get(X->fileX[wno]->pathX.canvas, XV_Y) ;
      XV_SET(X->fileX[wno]->pathX.canvas, XV_Y, y - diff, 0) ;

      y = (int) xv_get(X->fileX[wno]->status_panel, XV_Y) ;
      XV_SET(X->fileX[wno]->status_panel, XV_Y, y - diff, 0) ;
      XV_SET(X->fileX[wno]->status_panel, XV_HEIGHT, bheight + 6, 0) ;
      XV_SET(c, XV_Y, y - diff + bheight + 6, 0) ;
    }
  else
    {
      y = (int) xv_get(cp, XV_Y) ;
      XV_SET(c, XV_Y, y + bheight + 6, 0) ;
    }
}


void
select_list_item(list, row, state)
enum item_type list ;
int row, state ;
{
  XV_SET(X->items[(int) list], PANEL_LIST_SELECT, row, state, 0) ;
}


/*ARGSUSED*/
void
send_comments(item, event)
Panel_item item ;
Event *event ;
{
  char *c, cmd[MAXPATHLEN], filename[MAXPATHLEN], nextline[MAXPATHLEN] ;
  char progname[MAXLINE] ;
  int len, reply, total, x, y ;
  Textsw_index start, end ;
  FILE *ip, *op ;
  struct passwd *pp ;

#ifdef SVR4
  STRCPY(progname, "/bin/mailx") ;
#else
  STRCPY(progname, "/usr/ucb/Mail") ;
#endif /*SVR4*/

  len = (int) xv_get(X->items[(int) FM_COM_TEXT], TEXTSW_LENGTH) ;
  if (len)
    {
      x = (int) xv_get(X->fileX[Fm->curr_wno]->frame, XV_X) ;
      y = (int) xv_get(X->fileX[Fm->curr_wno]->frame, XV_Y) ;
      SPRINTF(filename, "/tmp/.FMcom%d", getpid()) ;

      textsw_store_file(X->items[(int) FM_COM_TEXT], filename, x, y) ;
      c = getlogin() ;
      if (c == NULL)
        {
          pp = getpwuid(geteuid()) ;
          if (pp == NULL) c = "unknown" ;
          else c = pp->pw_name ;
        }
      SPRINTF(cmd,
  "%s -s \"Filemgr 3.2 (rev %d) comments from %s@%s\" fm-comments@beerbust.Eng.Sun.COM",
              progname, PATCHLEVEL, c, Fm->hostname) ;
      if ((op = popen(cmd, "w")) != NULL)
        {
          if ((ip = fopen(filename, "r")) != NULL)
            {
              while (fgets(nextline, MAXPATHLEN, ip) != NULL)
                FPRINTF(op, "%s", nextline) ;
            }
          else FPRINTF(stderr, "%s: couldn't read %s.\n", Fm->appname, filename) ;
        }
      else FPRINTF(stderr, "%s: couldn't connect to Mail.\n", Fm->appname) ;
      if (ip) FCLOSE(ip) ;
      if (op) PCLOSE(op) ;
      UNLINK(filename) ;

/* Remove everything below the ====== line, ready for next comment. */

      start = 0 ;
      reply = textsw_find_bytes(X->items[(int) FM_COM_TEXT], &start, &end,
                                Str[(int) M_COM_DIVIDE],
                                strlen(Str[(int) M_COM_DIVIDE]), FALSE) ;
      if (reply != -1)
        {
          total = (int) xv_get(X->items[(int) FM_COM_TEXT], TEXTSW_LENGTH) ;
          textsw_delete(X->items[(int) FM_COM_TEXT], end, total) ;
        }
      else
        {
          textsw_reset(X->items[(int) FM_COM_TEXT], x, y) ;
          write_questions() ;
        }
    }
  XV_SET(X->items[(int) FM_COM_FRAME], FRAME_CMD_PUSHPIN_IN, FALSE, 0) ;
  XV_SET(X->items[(int) FM_COM_FRAME], XV_SHOW, FALSE, 0) ;
}


void
set_busy_cursor(wno, state)
int wno, state ;
{
  Canvas canvas ;
  Xv_Window pw_view ;

  if (wno == WNO_TREE) canvas = X->treeX.canvas ;
  else                 canvas = X->fileX[wno]->canvas ;

  FM_CANVAS_EACH_PAINT_WINDOW(canvas, pw_view)
    if (state == TRUE) XV_SET(pw_view, WIN_CURSOR, X->busy_cursor, 0) ;
    else               XV_SET(pw_view, WIN_CURSOR, X->def_cursor, 0) ;
  FM_CANVAS_END_EACH
}


void
set_canvas_attr(wno, ctype, value)
int wno, value ;
enum fm_can_type ctype ;
{
  Canvas c = X->fileX[wno]->canvas ;

  switch (ctype)
    {
      case FM_CAN_WIDTH  : XV_SET(c, CANVAS_WIDTH, value, 0) ;
                           break ;
      case FM_CAN_HEIGHT : XV_SET(c, CANVAS_HEIGHT, value, 0) ;
    }
}


void
set_color(str, n)
char *str ;
int n ;
{
  Colormap cmap ;
  XColor ccol ;

/*  The DefaultColormap macro can give the wrong value back.
 *  Used CMS_CMAP_ID instead for the information.
 */

  cmap = (Colormap)
         xv_get((Cms) xv_get(X->base_frame, WIN_CMS), CMS_CMAP_ID) ;
  if (XParseColor(X->display, cmap, str, &ccol) != 0)
    {
      Fm->red  [Fm->folder_color + n] = (ccol.red   >> 8) ;
      Fm->green[Fm->folder_color + n] = (ccol.green >> 8) ;
      Fm->blue [Fm->folder_color + n] = (ccol.blue  >> 8) ;
    }
}


void
set_copy_event(ec)
Event_Context ec ;
{
  ec->id = ACTION_DRAG_COPY ;
}


void
set_custom_menu(m)
enum menu_type m ;
{
  Menu_item mi ;

  mi = (Menu_item) xv_get(X->menus[(int) m], MENU_NTH_ITEM, FILE_BUT_CCMDS) ;
  X->menus[(int) FM_CC_MENU] = (Menu) xv_get(mi, MENU_PULLRIGHT) ;
  xv_set(X->menus[(int) FM_CC_MENU], XV_HELP_DATA,
		 "filemgr:File_Custom_Commands", 0);

}


void
set_default_item(panel, item)
enum item_type panel, item ;
{
  XV_SET(X->items[(int) panel], PANEL_DEFAULT_ITEM, X->items[(int) item], 0) ;
}


void
set_dnd_selection(ec)
Event_Context ec ;
{
  X->sel = create_seln_requestor(ec) ;
  dnd_decode_drop(X->sel, X->event) ;
}


void
set_event_time(ec)
Event_Context ec ;
{
  ec->sec  = X->event->ie_time.tv_sec ;
  ec->usec = X->event->ie_time.tv_usec ;
}


void
set_frame_attr(wno, ftype, value)
int wno, value ;
enum fm_frame_type ftype ;
{
  Frame f ;

  if (wno == WNO_TREE) f = X->treeX.frame ;
  else                 f = X->fileX[wno]->frame ;

  switch (ftype)
    {
      case FM_FRAME_BELL   : window_bell(f) ;
                             break ;
      case FM_FRAME_CLOSED : XV_SET(f, FRAME_CLOSED, value, 0) ;
                             break ;
      case FM_FRAME_SHOW   : XV_SET(f, XV_SHOW, value, 0) ;
    }
}


void
set_frame_icon(wno, mtype)
int wno ;
enum media_type mtype ;
{
  Icon icon ;
  Server_image icon_sv, mask_sv ;
  static Server_image CD_icon_sv      = 0, CD_mask_sv      = 0 ;
  static Server_image DOS_icon_sv     = 0, DOS_mask_sv     = 0 ;
  static Server_image Floppy_icon_sv  = 0, Floppy_mask_sv  = 0 ;
  static Server_image Filemgr_icon_sv = 0, Filemgr_mask_sv = 0 ;

  static unsigned short C_image[] = {
#include "images/cd.icon"
  } ;
  static unsigned short C_mask[] = {
#include "images/cd.mask.icon"
  } ;

  static unsigned short D_image[] = {
#include "images/diskette.icon"
  } ;

  static unsigned short D_mask[] = {
#include "images/diskette.mask.icon"
  } ;

  static unsigned short DOS_image[] = {
#include "images/dos.icon"
  } ;

  static unsigned short DOS_mask[] = {
#include "images/dos.mask.icon"
  } ;

  static unsigned short F_image[] = {
#include "images/FileMgr.icon"
  } ;
  static unsigned short F_mask[] = {
#include "images/FileMgr_mask.icon"
  } ;

  if (!CD_icon_sv)
    CD_icon_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                           XV_WIDTH,          64,
                           XV_HEIGHT,         64,
                           SERVER_IMAGE_BITS, C_image,
                           0) ;

  if (!CD_mask_sv)
    CD_mask_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                           XV_WIDTH,          64,
                           XV_HEIGHT,         64,
                           SERVER_IMAGE_BITS, C_mask,
                           0) ;

  if (!DOS_icon_sv)
    DOS_icon_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                            XV_WIDTH,          64,
                            XV_HEIGHT,         64,
                            SERVER_IMAGE_BITS, DOS_image,
                            0) ;

  if (!DOS_mask_sv)
    DOS_mask_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                            XV_WIDTH,          64,
                            XV_HEIGHT,         64,
                            SERVER_IMAGE_BITS, DOS_mask,
                            0) ;

  if (!Floppy_icon_sv)
    Floppy_icon_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                               XV_WIDTH,          64,
                               XV_HEIGHT,         64,
                               SERVER_IMAGE_BITS, D_image,
                               0) ;

  if (!Floppy_mask_sv)
    Floppy_mask_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                               XV_WIDTH,          64,
                               XV_HEIGHT,         64,
                               SERVER_IMAGE_BITS, D_mask,
                               0) ;

  if (!Filemgr_icon_sv)
    Filemgr_icon_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                                XV_WIDTH,          64,
                                XV_HEIGHT,         64,
                                SERVER_IMAGE_BITS, F_image,
                                0) ;

  if (!Filemgr_mask_sv)
    Filemgr_mask_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                                XV_WIDTH,          64,
                                XV_HEIGHT,         64,
                                SERVER_IMAGE_BITS, F_mask,
                                0) ;

       if (mtype == FM_DISK)
    {
      icon_sv = Filemgr_icon_sv ;
      mask_sv = Filemgr_mask_sv ;
    }
  else if (mtype == FM_CD)
    {
      icon_sv = CD_icon_sv ;
      mask_sv = CD_mask_sv ;
    }
  else if (mtype == FM_DOS)
    {
      icon_sv = DOS_icon_sv ;
      mask_sv = DOS_mask_sv ;
    }
  else
    {
      icon_sv = Floppy_icon_sv ;
      mask_sv = Floppy_mask_sv ;
    }

  icon = (Icon) xv_create(XV_NULL,          ICON,
                          ICON_IMAGE,       icon_sv,
                          ICON_MASK_IMAGE,  mask_sv,
                          XV_WIDTH,         64,
                          XV_HEIGHT,        64,
                          WIN_RETAINED,     TRUE,
                          ICON_TRANSPARENT, TRUE,
                          0) ;
  XV_SET(X->fileX[wno]->frame, FRAME_ICON, icon, 0) ;
}


void
set_icon_font(frame)      /* Set icon font (if specified). */
Frame frame ;
{
  Icon icon ;
  Xv_font font ;

  if (Fm->iconfont) {
	icon = (Icon) xv_get(frame, FRAME_ICON) ;
      	font = (Xv_Font) xv_find(frame, FONT,
                               FONT_NAME, Fm->iconfont,
                               0) ;
	if (font != 0)
      		XV_SET(icon, ICON_FONT, font, 0) ;
    }
}


void
set_item_int_attr(itype, atype, value)
enum item_type itype ;
enum item_attr_type atype ;
int value ;
{
  Xv_opaque i = X->items[(int) itype] ;

  if (i == 0) return ;
  switch (atype)
    {
      case FM_ITEM_BUSY     : XV_SET(i, FRAME_BUSY, value, 0) ;
                              break ;
      case FM_ITEM_FIT      : window_fit(i) ;
                              break ;
      case FM_ITEM_HEIGHT   : XV_SET(i, XV_HEIGHT, value, 0) ;
                              break ;
      case FM_ITEM_INACTIVE : XV_SET(i, PANEL_INACTIVE, value, 0) ;
                              break ;
      case FM_ITEM_IVALUE   : XV_SET(i, PANEL_VALUE, value, 0) ;
                              break ;
      case FM_ITEM_LDELETE  : XV_SET(i, PANEL_LIST_DELETE, value, 0) ;
                              break ;
      case FM_ITEM_LWIDTH   : XV_SET(i, PANEL_LIST_WIDTH, value, 0) ;
                              break ;
      case FM_ITEM_PANELX   : XV_SET(i, PANEL_ITEM_X, value, 0) ;
                              break ;
      case FM_ITEM_PANELY   : XV_SET(i, PANEL_ITEM_Y, value, 0) ;
                              break ;
      case FM_ITEM_SHOW     : XV_SET(i, XV_SHOW, value, 0) ;
                              break ;
      case FM_ITEM_VALX     : XV_SET(i, PANEL_VALUE_X, value, 0) ;
                              break ;
      case FM_ITEM_WIDTH    : XV_SET(i, XV_WIDTH, value, 0) ;
                              break ;
      case FM_ITEM_X        : XV_SET(i, XV_X, value, 0) ;
                              break ;
      case FM_ITEM_Y        : XV_SET(i, XV_Y, value, 0) ;
    }
}


void
set_item_str_attr(itype, atype, str)
enum item_type itype ;
enum item_attr_type atype ;
char *str ;
{
  Xv_opaque i = X->items[(int) itype] ;

  if (i == 0) return ;
  switch (atype)
    {
      case FM_ITEM_IVALUE   : XV_SET(i, PANEL_VALUE, str, 0) ;
    }
}


void
set_menu_default(mtype, value)
enum menu_type mtype ;
int value ;
{
  Menu m = X->menus[(int) mtype] ;
  int  n = (int) xv_get(m, MENU_NITEMS) ;

  if (value != -1 && m && value > 0 && value <= n) XV_SET(m, MENU_DEFAULT, value, 0) ;
}


void
set_menu_items(m, actfunc)
enum menu_type m ;
void (*actfunc)() ;
{
  int pinned  = 0 ;
  Frame frame = (Frame) xv_get(X->menus[(int) m], MENU_PIN_WINDOW) ;

  if (frame)
    {
      pinned = xv_get(frame, FRAME_CMD_PUSHPIN_IN) ;
      if (pinned) XV_SET(frame, FRAME_CMD_PUSHPIN_IN, FALSE, 0) ;
    }

  actfunc() ;

  if (pinned) XV_SET(frame, FRAME_CMD_PUSHPIN_IN, TRUE, 0) ;
}


void
set_menu_window_no(m, wno)
enum menu_type m ;
int wno ;
{
  XV_SET(X->menus[(int) m], MENU_CLIENT_DATA, wno, 0) ;
}


void
set_path_item_value(wno, str)
int wno ;
char *str ;
{
  XV_SET(X->fileX[wno]->Path_item, PANEL_VALUE, str, 0) ;
}


void
set_pw_events(wno, pw)
int wno ;
Xv_Window pw ;
{
  XV_SET(pw,
         WIN_CONSUME_PICK_EVENTS,
           WIN_META_EVENTS,
           LOC_WINENTER,
           LOC_WINEXIT,
           LOC_DRAG,
           ACTION_DRAG_MOVE,
           ACTION_DRAG_COPY,
           ACTION_DRAG_PREVIEW,
           SHIFT_CTRL,
           ACTION_STOP,
           ACTION_PROPS,
           ACTION_COPY,
           ACTION_PASTE,
           ACTION_FIND_FORWARD,
           ACTION_FIND_BACKWARD,
           ACTION_CUT,
           ACTION_GO_LINE_BACKWARD,
           ACTION_GO_LINE_END,
           ACTION_GO_PAGE_FORWARD,
           ACTION_GO_PAGE_BACKWARD,
           0,
         WIN_IGNORE_EVENTS,
           LOC_WINENTER,
           LOC_MOVE,
           0,
         XV_KEY_DATA, Fm->wno_key, wno,
         WIN_BIT_GRAVITY, ForgetGravity,
         0) ;
}


void
set_scrollbar_attr(wno, stype, sattr, value)
int wno, value ;
enum fm_sb_type stype ;
enum fm_sb_attr_type sattr ;
{
  int attr ;
  Canvas c ;
  Scrollbar sb ;
  Xv_Window pw_view ;

  if (wno == WNO_TREE) c = X->treeX.canvas ;
  else                 c = X->fileX[wno]->canvas ;

  pw_view = xv_get(c, OPENWIN_NTH_VIEW, 0) ;
  if (pw_view == 0) return ;

  if (stype == FM_H_SBAR) attr = OPENWIN_HORIZONTAL_SCROLLBAR ;
  else                    attr = OPENWIN_VERTICAL_SCROLLBAR ;
  sb = (Scrollbar) xv_get(c, attr, pw_view) ;
  if (sb == 0) return ;
  switch (sattr)
    {
      case FM_SB_LINE_HT  : XV_SET(sb, SCROLLBAR_PIXELS_PER_UNIT, value, 0) ;
                            break ;
      case FM_SB_SPLIT    : XV_SET(sb, SCROLLBAR_SPLITTABLE,      value, 0) ;
                            break ;
      case FM_SB_VIEW_LEN : XV_SET(sb, SCROLLBAR_VIEW_LENGTH,     value, 0) ;
    }
}


void
set_status_glyph(wno, mtype)
int wno ;
enum media_type mtype ;
{
#ifdef SVR4
  FILE *fp ;
  struct mnttab *mp ;
#endif /*SVR4*/
  int show                        = FALSE ;
  static Server_image lock_sv     = 0 ;
  static Server_image readonly_sv = 0 ;

  static unsigned short l_image[] = {
#include "images/lock.icon"
} ;
  static unsigned short r_image[] = {
#include "images/readonly.icon"
} ;

  if (!lock_sv)
    lock_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                        XV_WIDTH,          16,
                        XV_HEIGHT,         16,
                        SERVER_IMAGE_BITS, l_image,
                        0) ;
  if (!readonly_sv)
    readonly_sv = xv_create(XV_NULL,           SERVER_IMAGE,
                            XV_WIDTH,          16,
                            XV_HEIGHT,         16,
                            SERVER_IMAGE_BITS, r_image,
                            0) ;

  if (mtype == FM_CD)
    {
      XV_SET(X->fileX[wno]->status_glyph, PANEL_LABEL_IMAGE, readonly_sv, 0) ;
      show = TRUE ;
    }
#ifdef SVR4
  if (mtype == FM_FLOPPY || mtype == FM_DOS)
    if ((fp = fopen(MNTTAB, "r")) != NULL)
      {
        mp = (struct mnttab *) malloc(sizeof(struct mnttab)) ;
        while (fm_getmntent(fp, mp) != -1)
          if (EQUAL(mp->mnt_mountp, Fm->fmntpt[Fm->file[wno]->devno]))
            if (fm_hasmntopt(mp, "ro") != 0)
              {
                show = TRUE ;
                XV_SET(X->fileX[wno]->status_glyph,
                       PANEL_LABEL_IMAGE, lock_sv,
                       0) ;
              }
        FCLOSE(fp) ;
        FREE(mp) ;
      }
#endif /*SVR4*/
  XV_SET(X->fileX[wno]->status_glyph, XV_SHOW, show, 0) ;
}


/* Sets an itimer to check for system changes every n seconds, n=0 disables. */

void
set_timer(n)
int n ;
{
  struct itimerval period ;           /* Itimer interval. */

  Fm->cur_interval = n ;
  period.it_interval.tv_sec  = n ;
  period.it_interval.tv_usec = 0 ;
  period.it_value.tv_sec     = n ;
  period.it_value.tv_usec    = 0 ;
  NOTIFY_SET_ITIMER_FUNC(X->base_frame,
                         n > 0 ? (Notify_func) itimer_system_check :
                         NOTIFY_FUNC_NULL, ITIMER_REAL, &period, 0) ;
}


void
set_tree_title(t_p)        /* Set title line for the folder view window. */
Tree_Pane_Object *t_p ;
{
  WCHAR fname[MAXPATHLEN], title[MAXPATHLEN] ;

  if (fm_getpath(t_p, fname) == FM_SUCCESS)
    {
      fm_snprintf((char *) title, sizeof(title), "%s %s", Str[(int) M_FV_TITLE], fname) ;
      XV_SET(X->treeX.frame, XV_LABEL, title, 0) ;
    }
}


int
shelf_held()
{
  Atom seln_atom = (Atom) xv_get(X->server, SERVER_ATOM, "CLIPBOARD") ;

  if (!seln_atom) return(FALSE) ;
  if (XGetSelectionOwner(X->display, seln_atom) == None) return(FALSE) ;
  else                                                   return(TRUE) ;
}


#ifdef START_POPUP
void
start_popup()
{
  char version[MAXPATHLEN] ;
  int reply ;

  SPRINTF(version, "Version: 3.2alpha1.0  Patch %d\n", PATCHLEVEL) ;
  reply = notice_prompt(X->fileX[Fm->curr_wno]->frame, NULL,
                        NOTICE_MESSAGE_STRINGS,
    "Solaris 4/93 DeskSet: File Manager",
    "",
    "This is an pre-beta version of the File Manager application",
    "Please use the Comments popup to return feedback to the DeskSet group",
    "",
    "Features in this version may or may not be in the final release",
    "",
    version,
    0,
                       NOTICE_BUTTON_YES, "Continue",
                       NOTICE_BUTTON_NO,  "Quit",
                       0) ;
  if (reply != NOTICE_YES) exit(0) ;
}
#endif /*START_POPUP*/


void
start_tool()
{
  notify_set_input_func(client, read_from_pipe, Fm->pipe_fd) ;
  notify_start() ;
}


/*ARGSUSED*/
void
stop_button(item, event)
Panel_item item ;
Event *event ;
{
  do_stop_button() ;
}


void
stop_move_operation()
{
  really_stop_dnd(X->sel, FALSE) ;
}


void
sync_server()
{
  XV_SET(X->server, SERVER_SYNC_AND_PROCESS_EVENTS, 0) ;
  XFlush(X->display) ;
}


/* Event interpose procedure for the tree canvas. */

Notify_value
tree_event(canvas, event, arg, typ)
Canvas canvas ;
Event *event ;
Notify_arg arg ;
Notify_event_type typ ;
{
  Event_Context ec ;

  ec = setup_event_context(canvas, event, TRUE) ;
  path_tree_event(ec) ;
  return(notify_next_event_func(canvas, (Notify_event) event, arg, typ)) ;
}


void
tree_file_button(item, event)          /* Process tree pane File button. */
Panel_item item ;
Event *event ;
{
  if (event_is_down(event) &&
	(event_action(event) == ACTION_MENU ||
		event_action(event) == ACTION_SELECT))
	set_menu_items(FM_TFILE_MENU, set_tree_file_button_menu) ;

  panel_default_handle_event(item, event) ;
}


void
tree_repaint(canvas, pw, display, xid, xrects)
Canvas canvas ;
Xv_Window pw ;
Display *display ;
Window xid ;
Xv_xrectlist *xrects ;
{
  do_tree_repaint(canvas, pw, display, xid, xrects) ;
}


void
tree_view_button(item, event)       /* Process tree pane View button. */
Panel_item item ;
Event *event ;
{
  if (event_is_down(event) &&
	(event_action(event) == ACTION_MENU ||
		event_action(event) == ACTION_SELECT))
	set_menu_items(FM_TVIEW_MENU, set_tree_view_button_menu) ;

  panel_default_handle_event(item, event) ;
}


Menu_item
undelete(item, op)   /* Undelete selected files in the wastebasket. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) do_undelete() ;
  return(item) ;
}


void
view_button(item, event)                    /* Process View button. */
Panel_item item ;
Event *event ;
{
  if (event_is_down(event))
    {
      Fm->curr_wno = (int) xv_get(item, XV_KEY_DATA, Fm->wno_key) ;
      set_menu_window_no(FM_VIEW_MENU, Fm->curr_wno) ;
      if (event_action(event) == ACTION_MENU ||
		event_action(event) == ACTION_SELECT)
        set_menu_items(FM_VIEW_MENU, set_view_button_menu) ;
    }

  panel_default_handle_event(item, event) ;
}


Menu_item
vm_icon_name(item, op)   /* View button menu - Icon by Name item. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) change_view_style(VIEW_ICON_BY_NAME) ;
  return(item) ;
}


Menu_item
vm_icon_type(item, op)   /* View button menu - Icon by Type item. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) change_view_style(VIEW_ICON_BY_TYPE) ;
  return(item) ;
}


Menu_item
vm_list_date(item, op)   /* View button menu - List by Date. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) change_view_style(VIEW_LIST_BY_DATE) ;
  return(item) ;
}


Menu_item
vm_list_name(item, op)   /* View button menu - List by Name item. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) change_view_style(VIEW_LIST_BY_NAME) ;
  return(item) ;
}


Menu_item
vm_list_size(item, op)   /* View button menu - List by Size item. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) change_view_style(VIEW_LIST_BY_SIZE) ;
  return(item) ;
}


Menu_item
vm_list_type(item, op)   /* View button menu - List by Type item. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) change_view_style(VIEW_LIST_BY_TYPE) ;
  return(item) ;
}


Menu_item
vm_pos_icon(item, op)        /* View button menu - "Large Icons" item. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) change_pos_view(VIEW_BY_ICON) ;
  return(item) ;
}


Menu_item
vm_pos_list(item, op)        /* View button menu - "Small Icons" item. */
Menu_item item ;
Menu_generate op ;
{
  if (op == MENU_NOTIFY) change_pos_view(VIEW_BY_LIST) ;
  return(item) ;
}


/*ARGSUSED*/
void
waste_empty_button(item, event)
Panel_item item ;
Event *event ;
{
  do_delete_wastebasket() ;
}


void
wedit_button(item, event)        /* Process Waste pane edit button. */
Panel_item item ;
Event *event ;
{
  if (event_is_down(event) &&
	(event_action(event) == ACTION_MENU ||
		event_action(event) == ACTION_SELECT))
    set_menu_items(FM_WEDIT_MENU, set_waste_edit_menu) ;

  panel_default_handle_event(item, event) ;
}


void
write_footer(wno, str, bell)
int wno, bell ;
char *str ;
{
  Frame frame ;

  if (wno == WNO_TREE) frame = X->treeX.frame ;
  else                 frame = X->fileX[wno]->frame ;

  if (frame == 0) frame = X->fileX[Fm->curr_wno]->frame ;
  if (frame == 0) return ;
  XV_SET(frame, FRAME_LEFT_FOOTER, str, 0) ;
  if (bell) set_frame_attr(Fm->curr_wno, FM_FRAME_BELL, TRUE) ;
}


void
write_item_footer(itype, str, bell)
enum item_type itype ;
char *str ;
int bell ;
{
  XV_SET(X->items[(int) itype], FRAME_LEFT_FOOTER, str, 0) ;
  if (bell) set_frame_attr(Fm->curr_wno, FM_FRAME_BELL, TRUE) ;
}


void
wview_button(item, event)        /* Process Waste pane view button. */
Panel_item item ;
Event *event ;
{
  if (event_is_down(event) &&
	(event_action(event) == ACTION_MENU ||
		event_action(event) == ACTION_SELECT))
	set_menu_items(FM_VIEW_MENU, set_view_button_menu) ;

  panel_default_handle_event(item, event) ;
}

Boolean
is_frame_busy(wno)
int wno;
{
  Frame frame ;
  if (wno == WNO_TREE) frame = X->treeX.frame ;
  else                 frame = X->fileX[wno]->frame ;
  return ((int)xv_get(frame, FRAME_BUSY));
}
