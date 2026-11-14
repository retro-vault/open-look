#ifndef lint
static char sccsid[]="@(#)various.c	1.175 10/16/96 Copyright 1987-1992 Sun Microsystem, Inc." ;
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
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
/* Linux portability definitions are in linux_port.h. */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "defs.h"
#include "fm.h"

static ROUTINE Compare_fn[4] = {
  compare_name, compare_type, compare_size, compare_time
} ;

extern FmVars Fm ;
extern char *Str[] ;


void
adjust_entry(in, out)
char *in, *out ;
{
  char *c ;

  STRCPY(out, in) ;
  if (Fm->casesen == FALSE)
    for (c = out; *c != '\0'; c++)
      if (isupper(*c)) *c = tolower((unsigned char ) *c) ;
}


void
adjust_prop_sheet_size(value)
int value ;
{
  enum item_type panel, button ;
  int width ;

  switch (value)
    {
      case PROPS_STACK_GENERAL    : panel  = FM_PO_PROPS_GEN_PANEL ;
                                    button = FM_PO_GEN_APPLY ;
                                    width  = Fm->gen_pw ;
                                    break ;
      case PROPS_STACK_NEW_FOLDER : panel  = FM_PO_PROPS_NEWF_PANEL ;
                                    button = FM_PO_NEW_APPLY ;
                                    width  = Fm->new_pw ;
                                    break ;
      case PROPS_STACK_CUR_FOLDER : panel  = FM_PO_PROPS_CUR_PANEL ;
                                    button = FM_PO_CUR_APPLY ;
                                    width  = Fm->cur_pw ;
                                    break ;
      case PROPS_STACK_GOTO       : panel  = FM_PO_PROPS_GOTO_PANEL ;
                                    button = FM_PO_GOTO_APPLY ;
                                    width  = Fm->goto_pw ;
                                    break ;
      case PROPS_STACK_CUSTOM     : panel  = FM_PO_PROPS_CUSTOM_PANEL ;
                                    button = FM_PO_CUSTOM_APPLY ;
                                    width  = Fm->cus_pw ;
                                    break ;
      case PROPS_STACK_ADVANCED   : panel  = FM_PO_PROPS_ADVANCED_PANEL ;
                                    button = FM_PO_ADV_APPLY ;
                                    width  = Fm->adv_pw ;
    }
  set_item_int_attr(panel, FM_ITEM_SHOW, TRUE) ;
  resize_prop_sheet(FM_PO_PO_FRAME, panel, button, width) ;
}


void
change_view_style(opt)
int opt ;
{
  int wno      = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;

  fm_busy_cursor(TRUE, wno) ;
  switch (opt)
    {
      case VIEW_ICON_BY_NAME : f->display_mode = VIEW_BY_ICON ;
                               f->sortby       = SORT_BY_NAME ;
                               break ;

      case VIEW_ICON_BY_TYPE : f->display_mode = VIEW_BY_ICON ;
                               f->sortby       = SORT_BY_TYPE ;
                               break ;

      case VIEW_LIST_BY_NAME : f->display_mode = VIEW_BY_LIST ;
                               f->sortby       = SORT_BY_NAME ;
                               f->listopts     = LS_DATE | LS_SIZE ;
                               break ;

      case VIEW_LIST_BY_TYPE : f->display_mode = VIEW_BY_LIST ;
                               f->sortby       = SORT_BY_TYPE ;
                               f->listopts     = LS_DATE | LS_SIZE ;
                               break ;

      case VIEW_LIST_BY_SIZE : f->display_mode = VIEW_BY_LIST ;
                               f->sortby       = SORT_BY_SIZE ;
                               f->listopts     = LS_DATE | LS_SIZE ;
                               break ;

      case VIEW_LIST_BY_DATE : f->display_mode = VIEW_BY_LIST ;
                               f->sortby       = SORT_BY_TIME ;
                               f->listopts     = LS_DATE | LS_SIZE ;
    }

  set_item_int_attr(FM_PO_CURF_SORTBY, FM_ITEM_IVALUE,   f->sortby) ;
  set_item_int_attr(FM_PO_CURF_DISPLAY, FM_ITEM_IVALUE,  f->display_mode) ;
  set_item_int_attr(FM_PO_CURF_LISTOPTS, FM_ITEM_IVALUE, f->listopts) ;
  set_pos_attr(wno, FM_POS_POS,   FALSE) ;
  set_pos_attr(wno, FM_POS_MOVED, FALSE) ;
  fm_scrollbar_scroll_to(wno, FM_H_SBAR, 0) ;
  fm_scrollbar_scroll_to(wno, FM_V_SBAR, 0) ;
  SET_CANVAS_DIMS(wno, FM_STYLE_FOLDER) ;
  fm_display_folder(FM_STYLE_FOLDER, wno) ;

/*  XXX: - need to stop view preset panel items from resetting the
 *         custom view popup.  For now, set them if frame is showing.
 */

  if (is_popup_showing(FM_PO_PO_FRAME))
    {
      reset_cur_folder_panel() ;   /* Set view panel items. */
      set_list_item(FALSE) ;       /* Grayout/activate view panel items. */
    }
  fm_busy_cursor(FALSE, wno) ;
}


void
check_all_files(wno)    /* Check the status of all files in this window. */
int wno ;
{
  File_Pane_Object **f_p, **l_p ;
  struct stat fstatus ;
  char buf[256] ;

  if (!is_frame(wno)) return ;

  if (fm_stat((char *) Fm->file[wno]->path, &fstatus) == 0)
    if (fstatus.st_mtime > Fm->mtime[wno])
      {
        fm_display_folder((BYTE) FM_BUILD_FOLDER, wno) ;
        if (Fm->treeview) adjust_tree(wno) ;
        Fm->tree.current->status &= ~TEXPLORE ;
        return ;
      }

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p < l_p; f_p++)
    {
      SPRINTF(buf, "%s/%s", Fm->file[wno]->path, (*f_p)->name) ;
      if (fm_stat(buf, &fstatus) == 0)
        {
          if (fstatus.st_mtime > (*f_p)->mtime ||
              fstatus.st_ctime > (*f_p)->ctime)
            {

/* Existing file has changed, update the display. */

              fm_display_folder((BYTE) FM_BUILD_FOLDER, wno) ;
              if (Fm->treeview) adjust_tree(wno) ;
              Fm->tree.current->status &= ~TEXPLORE ;
              return ;
            }
        }
    }
}


void
clear_position_info(wno)
int wno ;
{
  MEMSET((char *) &Fm->file[wno]->c, 0, sizeof(File_Pane_Pos)) ;
  MEMSET((char *) &Fm->file[wno]->i, 0, sizeof(File_Pane_Pos)) ;
  MEMSET((char *) &Fm->file[wno]->l, 0, sizeof(File_Pane_Pos)) ;
  MEMSET((char *) &Fm->file[wno]->o, 0, sizeof(File_Pane_Pos)) ;
}


/* Remove files marked for deletion by squeezing array. */

void
condense_folder(wno)
int wno ;
{
  File_Pane_Object **f_p, **nf_p, **l_p ;

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p < l_p; )
    {
      if ((*f_p)->redisplay == FALSE)
        {
          FREE((*f_p)->name) ;
          if ((*f_p)->image) free_fp_image(f_p) ;
          nf_p = f_p ;
          Fm->file[wno]->num_objects-- ;
          l_p = PTR_LAST(wno) ;
          while (nf_p < l_p)
            {
              **nf_p = **(nf_p+1) ;
              nf_p++ ;
            }
        }
      else f_p++ ;
    }
}


void
create_fm_dirs()   /* Create $HOME/.fm and sub-directories (if not there). */
{
  char name[MAXPATHLEN] ;

  SPRINTF(name, "%s/.fm", Fm->home) ;            /* Directory .fm */
  create_fm_dir(name) ;
}


void
delete_icon(wno, off, x, y)
int wno, off, x, y ;
{
  File_Pane *f                = Fm->file[wno] ;
  File_Pane_Object **f_p      = f->object + off ;
  BYTE mode                   = f->display_mode ;
  int selected                = (*f_p)->selected ;
  unsigned long icon_fg_index = (*f_p)->icon_fg_index ;
  unsigned long icon_bg_index = (*f_p)->icon_bg_index ;

  (*f_p)->icon_fg_index = Fm->Window_bg ;
  (*f_p)->icon_bg_index = Fm->Window_bg ;
  (*f_p)->selected      = FALSE ;
  if (mode == VIEW_BY_CONTENT && is_image(wno, off))
    clear_area(wno, x - 20, y - GLYPH_HEIGHT - 4,
               CONTENT_GLYPH_WIDTH + 8, CONTENT_GLYPH_HEIGHT + 7) ;
  else if (mode == VIEW_BY_LIST)
    clear_area(wno, x-2, y-17, LIST_GLYPH_WIDTH+4, LIST_GLYPH_HEIGHT+4) ;
  else
    clear_area(wno, x-4, y-4, GLYPH_WIDTH+8, GLYPH_HEIGHT+7) ;

  if (mode == VIEW_BY_LIST)
    delete_text(wno, x + LIST_HEIGHT, y, (char *) (*f_p)->name) ;
  else
    delete_text(wno, (x + (GLYPH_WIDTH >> 1)) - ((*f_p)->width / 2),
                y + Fm->font_sizeH + GLYPH_HEIGHT, (char *) (*f_p)->name) ;
  (*f_p)->icon_fg_index = icon_fg_index ;
  (*f_p)->icon_bg_index = icon_bg_index ;
  (*f_p)->selected      = selected ;
}


void
do_delete_wastebasket()      /* Destroy all files in the waste basket. */
{
  File_Pane *f = Fm->file[WASTE] ;
  Tree_Pane *tp = &f->path_pane ;
  char buf[MAXPATHLEN], fname[MAXPATHLEN] ;

  fm_busy_cursor(TRUE, WASTE) ;
  set_timer(0) ;
  CHDIR((char *) Fm->home) ;
  SPRINTF(fname, "%s/.wastebasket", Fm->home) ;
  fm_snprintf(buf, sizeof(buf), "/bin/rm -rf %s/* %s/.*", fname, fname) ;
  if (fm_run_str(buf, TRUE)) fm_showerr(buf) ;

  free(f->path);
  f->path = (WCHAR*)fm_malloc(strlen(fname)+1);
  STRCPY((char *) f->path, fname) ;
  fm_display_folder(FM_BUILD_FOLDER, WASTE) ;
  tp->current = path_to_node(WASTE, f->path) ;
  draw_path(WASTE, TRUE) ;
  adjust_tree(WASTE) ;
  flush_delete_list() ;      /* Set delete list to null. */
  set_timer(Fm->interval) ;
  fm_busy_cursor(FALSE, WASTE) ;
}


void
do_folder_event(ec)
Event_Context ec ;
{
  int id, w ;
  int wno = ec->wno ;

/* Change to the directory if not there already. */

  if (is_visible(wno) && is_frame_busy(wno)) ;
  else CHDIR((char *) Fm->file[wno]->path) ;

  switch (ec->fm_id)
    {
      case E_ASCII   : /* User pressed an ascii key. */
                        set_event_state(ec, INCR_SEL) ;

/* Select files with input chars. */

                        incremental_selection(ec, !CLEAR_SEL, ec->ch) ;
                        break ;

      case E_FKEY    : /* User pressed a function key. */

/* If incr. searching when a func. key is pressed, stop search. */

                        if (ec->state == INCR_SEL)
                          set_event_state(ec, CLEAR) ;
                        if (ec->down) fm_check_keys() ;
                        break ;

      case E_RESIZE    : handle_resize(wno) ;
                         break ;

      case E_DRAG_COPY : /* New style dnd drop event. */
      case E_DRAG_MOVE : do_drag_copy_move(ec) ;
                         break ;

      case E_DRAG_PREVIEW : /* New style dnd preview events. */
             id = get_event_id() ;
        if (id == E_WINEXIT)
          {
            set_timer(Fm->interval) ;
            if (ec->state == DND_OVER_ITEM)
              {

/* Clear the old image. */

                set_event_state(ec, CLEAR) ;
                draw_ith(ec->old_item, wno) ;
              }
          }
        else if (id == E_DRAG)
          {
            ec = check_if_over_item(ec) ;
            if (ec->state == DND_OVER_ITEM)
              {
                if (!ec->over_item || ec->old_item != ec->item)
                  {

/* Clear the old image. */

                    set_event_state(ec, CLEAR) ;
                    draw_ith(ec->old_item, wno) ;
                  }
              }
            else
              {
                if (ec->over_item && !ec->over_name &&
                    is_dnd_target(Fm->file[wno]->object[ec->item]))
                  {

/* Invert the image to show we are over it. */

                    set_event_state(ec, DND_OVER_ITEM) ;
                    fm_msg(FALSE, Str[(int) M_OVER],
                           Fm->file[wno]->object[ec->item]->name) ;
                    reverse_ith(ec->item, wno) ;
                    ec->old_item = ec->item ;
                  }
              }
          }
        break ;

      case E_DRAG_LOAD : /* Old style dnd drop event. */
        fm_msg(TRUE, Str[(int) M_DND_OLD]) ;
        break ;

      case E_MENU : if (ec->state == MARQUIS)
                      {
                        set_event_state(ec, CLEAR) ;
                        finish_marquis_selection(ec) ;  /* Erase box. */
                      }
                    file_pane_menu(wno) ;  /* Show/hide floating menu */
                    break ;

      case E_ADJUST :
        ec = check_if_over_item(ec) ;
        if (ec->down)
          {
            if (ec->meta)
              {
                set_event_state(ec, MAYBE_PAN) ;
                ec->start_x = ec->old_x = ec->x ;
                ec->start_y = ec->old_y = ec->y ;
              }
            else if (ec->over_item)
              {

/*  If the user clicks ADJUST on an item in one folder pane window, then we
 *  must clear all selections in all other windows.
 */

                fm_treedeselect() ;
                for (w = 0; w < Fm->maxwin; w++)
                  if (w != wno && is_frame(w))
                    {
                      fm_pathdeselect(w) ;
                      fm_filedeselect(w) ;
                    }

/* Select on down stroke. */

                toggle_item(ec, EXTEND_SELECTION) ;
                ec->old_item = ec->item ;
              }
            else
              {
                set_event_state(ec, MAYBE_MARQUIS) ;
                ec->start_x = ec->old_x = ec->x ;
                ec->start_y = ec->old_y = ec->y ;
              }
            ec->old_sec = 0 ;     /* Clear the possible double-click. */
            ec->old_usec = 0 ;    /* Clear the possible double-click. */
          }
        else                      /* Key is up. */
          {
                 if (ec->state == MARQUIS) finish_marquis_selection(ec) ;
            else if (ec->state == PAN)     finish_panning(ec) ;

/* Set/clear file info popup if showing. */

            if (Fm->fprops_showing) do_file_info_popup(wno) ;
            set_event_state(ec, CLEAR) ;
          }
        break ;

      case E_SELECT :
        if (FM_TIMER_OFF) set_timer(Fm->interval) ;
        ec = check_if_over_item(ec) ;
        if (ec->down)
          {
            if (ec->meta) set_event_state(ec, MAYBE_PAN) ;
            else if (ec->over_item)
              {
                set_event_state(ec, MAYBE_DRAG) ;
                set_event_time(ec) ;
                if (!ec->over_name && ec->item == ec->old_item &&
                    fm_is_double_click(ec->old_sec, ec->old_usec,
                                       ec->sec, ec->usec,
                                       ec->old_x, ec->old_y, ec->x, ec->y))
                  {
                    set_event_state(ec, OPENING) ;

/*  Open double-clicked file in window[wno] at pos x,y, using default open
 *  method, no subframes, no editor, no args.
 */

                    open_files(wno, ec->x, ec->y, NULL, Fm->newwin, FALSE) ;
                    ec->old_sec = 0 ;     /* Clear the double-click. */
                    ec->old_usec = 0 ;
                  }
                else if (!Fm->file[wno]->object[ec->item]->selected)
                  {

/* If not selected, then make a unique selection, else maybe a drag. */

                    ec->old_sec  = ec->sec ;  /* Setup double-click. */
                    ec->old_usec = ec->usec ;
                    select_item(ec, UNIQUE_SELECTION) ;
                  }
                else
                  {
                    ec->old_sec = ec->sec ;  /* Setup double-click. */
                    ec->old_usec = ec->usec ;
                  }
                ec->old_item = ec->item ;
              }
            else
              {
                set_event_state(ec, MAYBE_MARQUIS) ;
                ec->old_sec = 0 ;                /* Clear the double-click. */
                ec->old_usec = 0 ;
                fm_pathdeselect(wno) ;       /* Deselect path item. */
                fm_filedeselect(wno) ;       /* Deselect all files. */
              }

/* Record starting x & y position. */

            ec->start_x = ec->old_x = ec->x ;
            ec->start_y = ec->old_x = ec->y ;
          }
        else                                  /* Key is up. */
          {
                 if (ec->state == MARQUIS) finish_marquis_selection(ec) ;
            else if (ec->state == PAN)     finish_panning(ec) ;
            else if (ec->state == MAYBE_DRAG)
              {
                fm_all_filedeselect() ;      /* Deselect all files. */
                fm_all_pathdeselect(WNO_TREE) ;
                fm_treedeselect() ;
                if (ec->over_item)

/* User did not drag -- make sure to deselect other files now. */

                  select_item(ec, UNIQUE_SELECTION) ;

                if (ec->over_name)
                  {
                    set_event_state(ec, RENAMING) ;
                    fm_rename(ec->wno, ec->item) ;
                    ec->old_sec = 0 ;            /* Clear the double-click. */
                    ec->old_usec = 0 ;
                  }
              }

/* Set/clear file info popup if showing. */

            if (Fm->fprops_showing) do_file_info_popup(wno) ;
            if (ec->state != RENAMING) set_event_state(ec, CLEAR) ;
          }
        break ;

      case E_DRAG :
        if (ec->state == MAYBE_PAN || ec->state == PAN)
          {
            set_event_state(ec, PAN) ;
            do_panning(ec) ;
          }
        else if (ec->state == MAYBE_MARQUIS || ec->state == MARQUIS)
          {
            set_event_state(ec, MARQUIS) ;
            do_marquis_selection(ec) ;
          }
        else if (ec->state == MAYBE_DRAG)
          {
            if (abs(ec->x - ec->start_x) > THRESHOLD ||
                abs(ec->y - ec->start_y) > THRESHOLD)
              {
                set_event_state(ec, CLEAR) ;
                Fm->started_dnd = TRUE ;
                Fm->move_wno = wno ;
                start_dnd(ec) ;
              }
          }
        break ;

      case E_SCROLL   : if (Fm->Renaming)    /* User scrolled the window. */
                          stop_rename() ;
                        break ;

      default         : break ;
    }
    CHDIR((char *) Fm->home) ;
}


void
do_display_icon(wno, rx, ry, rw, rh)
int wno, rx, ry, rw, rh ;
{
  int len, off, x, y ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **l_p, **s_p ;

  Boolean show ;          /* Set FALSE, when offscreen. */
  int height ;            /* Height in pixels of the folder pane. */
  int incx = 0 ;          /* X item increment. */
  int incy ;              /* Y item increment. */
  int max_filespercol ;   /* Maximum files per column. */
  int max_filesperline ;  /* Maximum files per line. */
  int rect_x, rect_y ;    /* Saved X/Y display position. */
  int rect_height ;       /* Save height cus changing size with change. */
  int rect_width ;        /* Save width cus changing width will change. */
  int sb_wid ;            /* Scrollbar width (in current scale). */
  int startx ;            /* Where to start display (horizontally). */
  int starty ;            /* Where to start display (vertically). */
  int widest ;            /* For names widest than window. */
  int width ;             /* Width in pixels of the folder pane. */
  int win_height ;        /* Window height. */
  int win_width ;         /* Window width. */
  int xhilimit, xlolimit ;  /* Visible X display limits. */
  int yhilimit, ylolimit ;  /* Visible Y display limits. */

/*  Have the icons already been displayed for this directory in this mode?
 *  If so, then we use the existing positional information.
 */

  if (get_pos_attr(wno, FM_POS_POS))
    {
      redisplay_folder(wno, rx, ry, rw, rh) ;
      return ;
    }

  rect_x      = rx ;
  rect_y      = ry ;
  rect_height = rh ;
  rect_width  = rw ;
  sb_wid      = sb_width(wno) ;
  win_width   = get_canvas_attr(wno, FM_WIN_WIDTH)  - sb_wid ;
  win_height  = get_canvas_attr(wno, FM_WIN_HEIGHT) - sb_wid ;

  widest      = f->widest_name ;
  len         = f->num_file_chars * Fm->font_sizeW ;
  if (widest > len) widest = len ;

       if (f->display_mode == VIEW_BY_CONTENT) len = CONTENT_GLYPH_WIDTH + 4 ;
  else if (f->display_mode == VIEW_BY_ICON)    len = GLYPH_WIDTH + 4 ;
  else                                         len = LIST_GLYPH_WIDTH + 2 ;
  if (widest > len) len = widest ;

  if (f->display_mode == VIEW_BY_LIST) incx = LIST_HEIGHT ;
  incx += len + (GLYPH_WIDTH >> 3) ;

       if (f->display_mode == VIEW_BY_ICON) incy = FM_ICON_HEIGHT ;
  else if (f->display_mode == VIEW_BY_LIST) incy = LIST_HEIGHT ;
  else                                      incy = CONTENT_HEIGHT ;

  startx = win_width % incx / 2 ;
  if (f->display_mode != VIEW_BY_LIST && GLYPH_WIDTH < len)
    startx += (len >> 1) - (GLYPH_WIDTH >> 1) ;
  if (!startx) startx = MARGIN ;

  starty = TOP_MARGIN ;
       if (f->display_mode == VIEW_BY_LIST)    starty += TOP_MARGIN ;
  else if (f->display_mode == VIEW_BY_CONTENT) starty += GLYPH_HEIGHT ;

  if (f->dispdir == FM_DISP_BY_ROWS)
    {
      max_filesperline = win_width / incx ;
      if (!max_filesperline) max_filesperline = 1 ;
      max_filespercol = f->num_objects / max_filesperline ;
      if (max_filespercol * max_filesperline != f->num_objects)
        max_filespercol++ ;
      width  = win_width ;
      height = incy * max_filespercol + (2 * starty) ;
      if (height < win_height)
        {
          max_filespercol = win_height / incy ;
          if (!max_filespercol) max_filespercol = 1 ;
          height = win_height ;
        }
      else if (height % incy) height = incy * ((height / incy) + 1) ;
    }
  else
    {
      max_filespercol = win_height / incy ;
      if (!max_filespercol) max_filespercol = 1 ;
      max_filesperline = f->num_objects / max_filespercol ;
      if (max_filesperline * max_filespercol != f->num_objects)
        max_filesperline++ ;
      height = win_height ;
      width  = incx * max_filesperline + (2 * startx) ;
      if (width < win_width)
        {
          max_filesperline = win_width / incx ;
          if (!max_filesperline) max_filesperline = 1 ;
          width = win_width ;
        }
      else if (width % incx) width = incx * ((width / incx) + 1) ;
    }

/*  If the new canvas width or height would be bigger than the maximum size
 *  allowable under XView for a canvas, then we need to half that dimension
 *  and double the other, also adjusting a few variables.
 */

  if (f->dispdir == FM_DISP_BY_ROWS)
    {
      if (height > MAX_CANVAS_SIZE)
        {
          len = (height / MAX_CANVAS_SIZE) + 1 ;
          height /= len ;
          max_filesperline *= len ;
          width = max_filesperline * incx + (2 * startx) ;
          if (width < win_width) width = win_width ;
          else if (width % incx) width = incx * ((width / incx) + 1) ;
        }
    }
  else
    {
      if (width > MAX_CANVAS_SIZE)
        {
          len = (width / MAX_CANVAS_SIZE) + 1 ;
          width /= len ;
          max_filespercol *= len ;
          height = max_filespercol * incy + (2 * starty) ;
          if (height < win_height) height = win_height ;
          else if (height % incy) height = incy * ((height / incy) + 1) ;
        }
    }

  set_pos_attr(wno, FM_POS_POS,     !Fm->autosort && (f->num_objects != 0)) ;
  set_pos_attr(wno, FM_POS_STARTX,  startx) ;
  set_pos_attr(wno, FM_POS_STARTY,  starty) ;
  set_pos_attr(wno, FM_POS_MAXCOL,  max_filespercol) ;
  set_pos_attr(wno, FM_POS_MAXLINE, max_filesperline) ;
  set_pos_attr(wno, FM_POS_INCX,    incx) ;
  set_pos_attr(wno, FM_POS_INCY,    incy) ;

  FM_SET_CANVAS_WIDTH(width,   wno, incx) ;
  FM_SET_CANVAS_HEIGHT(height, wno, incy) ;

  off    = 0 ;
  xlolimit = rect_x      - incx ;
  ylolimit = rect_y      - incy ;
  xhilimit = rect_width  + rect_x + incx ;
  yhilimit = rect_height + rect_y + incy ;
  x      = startx ;
  y      = starty ;
  l_p    = PTR_LAST(wno) ;
  s_p    = PTR_FIRST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    {
      if (x < xlolimit || y < ylolimit ||
          x > xhilimit || y > yhilimit) show = FALSE ;
      else                              show = TRUE ;
      draw_folder_icon_and_text(wno, f_p, x, y, widest, show) ;
      if (f->dispdir == FM_DISP_BY_ROWS) x += incx ;
      else                               y += incy ;
      off++ ;
      if (f->dispdir == FM_DISP_BY_ROWS)
        {
          if (off >= max_filesperline)
            {
              off = 0 ;
              x   = startx ;
              y  += incy ;
            }
        }
      else
        {
          if (off >= max_filespercol)
            {
              off = 0 ;
              x  += incx ;
              y   = starty ;
            }
        }
    }
}


/*ARGSUSED*/
void
do_display_list(wno, rect_left, rect_top, rect_width, rect_height)
int wno, rect_left, rect_top, rect_width, rect_height ;
{
  int hflag = 0 ;              /* Bugid 1182272 Test flag for height limit */
  static int yyflag = 0 ;      /* Bugid 1182272 Warning message limit flag*/
  int file_limit ;	       /* Bugid 1182272 file limit constant 1636 */

  int i, j, width ;
  int incy     = LIST_HEIGHT ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **l_p, **s_p ;

  set_pos_attr(wno, FM_POS_INCX, 1) ;
  set_pos_attr(wno, FM_POS_INCY, incy) ;

  width = list_width(wno) ;
  (void) fm_set_canvas_width(width, wno, 1) ;
  file_limit = (MAX_CANVAS_SIZE / LIST_HEIGHT) ; /* Bugid 1182272 file */
                                                 /* limit  constant 1638 */

/*  Ensure that the canvas height corresponds to the number of files in this
 *  folder. Then the scrollbar will reflect the correct visible portion.
 */

/* Bugid      - The following code was added as a solution to the number of
 * 1182272      files restriction in File Manager, and to prevent core dumping
 *              when that number is met. A warning is displayed each time the
 *              directory is changed to one that is at or more than a safe
 *              amount. This warning is sent to the bottom border localized.
 *              The canvas size is reduced to the MAX_CANVAS_SIZE value (32767),
 *              minus a pad of LIST_HEIGHT (20) times 2.
 */

/* Bugid 1182272 */
  if ((f->num_objects+1) >= file_limit)
    {
    hflag = 1;
    if ( yyflag == 0 )
      {
	fm_msg(TRUE, Str[(int) M_2MANYFILES], file_limit-2 ) ;
        yyflag = 1 ;
      }
    }

  if (hflag == 0)
    {
      if ( fm_set_canvas_height((f->num_objects+1) * LIST_HEIGHT, wno, incy))
        {
     	  if ( yyflag == 1)
              yyflag = 0 ;
          return ;
        }
    }
  else if ( fm_set_canvas_height((MAX_CANVAS_SIZE - (LIST_HEIGHT * 2)), wno, incy))
    return ;
/* Bugid 1182272 */

  j   = rect_top / LIST_HEIGHT ;
  f_p = f->object + j ;
  j   = rect_height / LIST_HEIGHT ;
  j++ ;

  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (i = 0; f_p < l_p && i < j; f_p++, i++)
    draw_list_ith(f_p - s_p, wno) ;
}


void
do_eject_disk()
{
  char cmd[MAXLINE] ;
  int wno      = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;

  if (f->mtype == FM_CD) SPRINTF(cmd, "/bin/eject cdrom%01d",  f->devno) ;
  else                   SPRINTF(cmd, "/bin/eject floppy%01d", f->devno) ;
  CHDIR((char *) Fm->home) ;
  if (fm_run_str(cmd, TRUE) != 0) fm_showerr(cmd) ;
  if (is_popup_showing(FM_REN_FRAME)) do_dismiss_popup(FM_REN_FRAME) ;
}


void
do_drag_copy_move(ec)
Event_Context ec ;
{
  int item ;
  int wno = ec->wno ;

  if (!Fm->started_dnd || Fm->move_wno != wno)
    {
      if (Fm->file[Fm->move_wno]->mtype != FM_DISK) set_copy_event(ec) ;
      if (Fm->file[wno]->mtype != FM_DISK) set_copy_event(ec) ;
      do_new_dnd_drop(ec) ;
    }
  else
    {
      item = fm_over_item(ec->x, ec->y, FALSE, wno) ;
      if (item != FM_EMPTY && is_dnd_target(Fm->file[wno]->object[item]))
        {
          if (Fm->file[wno]->mtype != FM_DISK) set_copy_event(ec) ;
          do_new_dnd_drop(ec) ;
        }
      else
        {
          set_dnd_selection(ec) ;
          handle_move_copy(ec) ;
        }
    }
  if (FM_TIMER_OFF) set_timer(Fm->interval) ;
  Fm->started_dnd = FALSE ;
}


void
do_format_disk(formtype, rawpath, mntpoint)
char *formtype, *rawpath, *mntpoint ;
{
  char binname[MAXPATHLEN], cmd[MAXPATHLEN], *ff ;
  char fdrive[8], *correct_rawpath, *correct_mntpoint;
  int found = FALSE ;
  int info[MAXINFO], x, y, i ;
  int wno      = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;

  for (i=0; i < MAXFLOPPY; i++) {
     if (ff = strchr(rawpath, '0' + i))
	break;
  }
  strcpy(fdrive, "floppy");
  fdrive[6] = *ff;
  fdrive[7] = 0;

  /*  CTE# 501463; BUG# 1206761
   *  Check to make sure we are dealing with floppy0
   */
  if (strstr(rawpath, "diskette0") == NULL) {
      return ;
  }


  get_win_X_info(wno, info) ;
  x     = info[(int) FM_INFO_XOPEN] + info[(int) FM_INFO_WIDTH] + 10 ;
  y     = info[(int) FM_INFO_YOPEN] ;

  if ((ff = getenv("DESKSETHOME")) != NULL)
    {
      SPRINTF(binname, "%s/format_floppy", ff) ;
      if (access(binname, F_OK) == 0) found = TRUE ;
    }
  if (found == FALSE && (ff = getenv("OPENWINHOME")) != NULL)
    {
      SPRINTF(binname, "%s/bin/format_floppy", ff) ;
      if (access(binname, F_OK) == 0) found = TRUE ;
    }
  if (found == FALSE)
    {
      fm_msg(TRUE, Str[(int) M_FIND], "format_floppy") ;
      return ;
    }

  correct_rawpath = media_findname(fdrive);
  correct_mntpoint = strrchr(correct_rawpath, '/');
  correct_mntpoint++;
/*  SPRINTF(cmd, "%s -p %s -x %d -y %d -d %s -m %s", */
/*	Fix for 1234866 - filemgr can't format already formatted floppy */
  fm_snprintf(cmd, sizeof(cmd), "%s -p %s -x %d -y %d -d %s -m /floppy/%s",
          binname, formtype, x, y, correct_rawpath, correct_mntpoint) ;
  CHDIR((char *) Fm->home) ;
  if (fm_run_str(cmd, FALSE) != 0) fm_showerr(cmd) ;
  else
    {
      Fm->format_pid[f->devno] = Fm->system_pid ;
      for (wno = 0; wno < Fm->num_win; wno++)
        if ((Fm->file[wno]->mtype == FM_DOS ||
             Fm->file[wno]->mtype == FM_FLOPPY) &&
             Fm->file[wno]->devno == f->devno)
	  { /* make sure the busy_count = 0 then the frame will be set to
	       busy */
	    Fm->file[wno]->busy_count = 0;
            busy_cursor(TRUE, wno, &Fm->file[wno]->busy_count) ;
	  }
    }
  free(correct_rawpath);
}


void
do_rename_disk()
{
  char binname[MAXPATHLEN], cmd[MAXPATHLEN], *disk_name, *ff ;
  int found    = FALSE ;
  int wno      = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;

  /*  CTE# 501463; BUG# 1206761
   *  Check to make sure we are dealing with floppy0.  This is a sanity
   *  check, execution should never make it here if not floppy0.
   */
  if (strstr(Fm->frawpath[f->devno], "diskette0") == NULL) {
      if (is_popup_showing(FM_REN_FRAME))
          do_dismiss_popup(FM_REN_FRAME) ;

      return ;
  }

  if ((ff = getenv("DESKSETHOME")) != NULL)
    {
      SPRINTF(binname, "%s/format_floppy", ff) ;
      if (access(binname, F_OK) == 0) found = TRUE ;
    }
  if (found == FALSE && (ff = getenv("OPENWINHOME")) != NULL)
    {
      SPRINTF(binname, "%s/bin/format_floppy", ff) ;
      if (access(binname, F_OK) == 0) found = TRUE ;
    }
  if (found == FALSE)
    {
      fm_msg(TRUE, Str[(int) M_FIND], "format_floppy") ;
      return ;
    }

  disk_name  = get_item_str_attr(FM_REN_DNAME, FM_ITEM_IVALUE) ;
  if (disk_name == NULL || isempty((WCHAR *) disk_name))
    {
      write_item_footer(FM_REN_FRAME, Str[(int) M_NO_DISK_NAME], TRUE) ;
      return ;
    }
  fm_snprintf(cmd, sizeof(cmd), "%s -r -d %s -m %s -n %s",
          binname, Fm->frawpath[f->devno], Fm->fmntpt[f->devno], disk_name) ;
  CHDIR((char *) Fm->home) ;
  if (fm_run_str(cmd, FALSE) != 0) fm_showerr(cmd) ;
  else
    {
      Fm->format_pid[f->devno] = Fm->system_pid ;
      for (wno = 0; wno < Fm->num_win; wno++)
        if ((Fm->file[wno]->mtype == FM_DOS ||
             Fm->file[wno]->mtype == FM_FLOPPY) &&
             Fm->file[wno]->devno == f->devno)
          {
            Fm->file[wno]->busy_count = 0 ;
            busy_cursor(TRUE, wno, &Fm->file[wno]->busy_count) ;
          }
    }
  do_dismiss_popup(FM_REN_FRAME) ;
}


void
do_rename_make()
{
  int wno      = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;


  /*  CTE# 501463; BUG# 1206761
   *  Check to make sure we are dealing with floppy0
   */
  if (strstr(Fm->frawpath[f->devno], "diskette0") == NULL) {
      return ;
  }

  fm_popup_create(FM_REN_POPUP) ;
  link_rename_items() ;

  fm_add_help(FM_REN_FRAME,  "filemgr:Rename_Frame") ;
  fm_add_help(FM_REN_PANEL,  "filemgr:Rename_Panel") ;
  fm_add_help(FM_REN_DNAME,  "filemgr:Rename_Disk_Name") ;
  fm_add_help(FM_REN_RENAME, "filemgr:Rename_Rename") ;
  fm_add_help(FM_REN_CANCEL, "filemgr:Rename_Cancel") ;

  set_default_item(FM_REN_PANEL, FM_REN_RENAME) ;
  fm_center_rename_buttons(FM_REN_PANEL, FM_REN_RENAME, FM_REN_CANCEL) ;
  fm_position_popup(Fm->curr_wno, FM_REN_FRAME) ;
  set_item_int_attr(FM_REN_FRAME, FM_ITEM_SHOW, TRUE) ;
}


void
draw_folder_icon(wno, display_mode, f_p, off)
int wno, display_mode, off ;
File_Pane_Object **f_p ;
{
  int x = get_file_obj_x(wno, *f_p) ;
  int y = get_file_obj_y(wno, *f_p) ;

  if (display_mode == VIEW_BY_LIST)
    draw_list_icon(wno, x, y-15, *f_p) ;
  else if (display_mode == VIEW_BY_CONTENT && is_image(wno, off))
    file_content(wno, x-16, y-GLYPH_HEIGHT, 64, off, (*f_p)->image_depth) ;
  else
    draw_icon(wno, x, y, GLYPH_HEIGHT, *f_p) ;
}


void
draw_folder_icon_and_text(wno, f_p, x, y, widest, show)
int wno, x, y, widest ;
File_Pane_Object **f_p ;
Boolean show ;
{
  File_Pane *f           = Fm->file[wno] ;
  File_Pane_Object **s_p = PTR_FIRST(wno) ;
  int off                = f_p - s_p ;

  if ((*f_p)->type == FT_UNCHECKED) my_stat((char *) f->path, f_p, wno, off) ;

  set_file_obj_x(wno, *f_p, x) ;
  set_file_obj_y(wno, *f_p, y) ;
  if (show)
    {
      draw_folder_icon(wno, f->display_mode, f_p, off) ;
      draw_folder_text(wno, f->display_mode, f_p, widest) ;
    }
}

void
draw_folder_text(wno, display_mode, f_p, widest)
int wno, display_mode, widest ;
File_Pane_Object **f_p ;
{
  Boolean is2long ;                 /* Set if filename too wide. */
  char c, c1 ;
  WCHAR *p ;
  int tx ;                          /* X value of icon filename text. */
  int ty ;                          /* Y value of icon filename text. */
  int w ;                           /* Name width. */
  int x = get_file_obj_x(wno, *f_p) ;
  int y = get_file_obj_y(wno, *f_p) ;
  int num_bytes = 0 ;

  if (!(*f_p)->width) (*f_p)->width = fm_strlen((char *) (*f_p)->name) ;
  w = (*f_p)->width ;
  is2long = FALSE ;
  if (w > widest)
    {
      is2long = TRUE ;

/*  Name is too long; truncate it to max width. This is calculated in pixels
 *  for variable and fixed width fonts. Indicate truncation with '>'.
 */
      p = (*f_p)->name ;
	  num_bytes = fm_nchars((widest - Fm->font_width['>']), p) ;
	  p += num_bytes ;

      c = *p ;
      c1 = *(p+1) ;
      *p = '>' ;
      *(p+1) = 0 ;
	  w = fm_strlen((char *) (*f_p)->name) ;
    }

  if (display_mode == VIEW_BY_LIST)
    {
      tx = x + LIST_HEIGHT ;
      ty = y ;
    }
  else
    {
      tx = (x + (GLYPH_WIDTH >> 1)) - (w / 2) ;
      ty = y + Fm->font_sizeH + GLYPH_HEIGHT ;
    }
  file_text(wno, tx, ty, (char *) (*f_p)->name, FALSE) ;

  if (is2long == TRUE)
    {
      *p = c ;
      *(p+1) = c1 ;
    }
}


int
entry_found(wno, name)
int wno ;
char *name ;
{
  char *c, cur[MAXPATHLEN], namestr[MAXPATHLEN] ;
  File_Pane *f = Fm->file[wno] ;
  int low      = 0 ;
  int high     = f->num_objects ;
  int mid, n, reply ;

  adjust_entry(name, namestr) ;
  while (low < high-1)
    {
      mid = (low + high) / 2 ;
      adjust_entry((char *) f->object[mid]->name, cur) ;

      reply = strcoll(namestr, cur) ;
      if (reply == 0 &&
          (reply = strcoll(name, (char *) f->object[mid]->name)) == 0)
        return(mid) ;
      else if (reply < 0)  high = mid ;
      else                 low  = mid ;
    }
  if (strcoll(name, (char *) f->object[low]->name) == 0) n = low ;
  else                                                   n = -1 ;
  return(n) ;
}


/*  Check to see if there is a free grid point on the last used row. If not,
 *  then enlarge the canvas height by one row, and return the first spot.
 */

void
find_free_grid_point(wno, f_p)
int wno ;
File_Pane_Object *f_p ;
{
  File_Pane *f = Fm->file[wno] ;
  short dmode  = f->display_mode ;

  f->display_mode = VIEW_BY_ICON ;
  set_grid_points(wno) ;
  set_icon_pos(wno, f_p) ;

  f->display_mode = VIEW_BY_LIST ;
  set_grid_points(wno) ;
  set_icon_pos(wno, f_p) ;

  f->display_mode = VIEW_BY_CONTENT ;
  set_grid_points(wno) ;
  set_icon_pos(wno, f_p) ;

  f->display_mode = dmode ;
}


/* Select the items which are somewhere touching the bounding box. */

void
finish_marquis_selection(ec)    /* Possible marquis selection. */
Event_Context ec ;
{
  File_Pane *f = Fm->file[ec->wno] ;
  File_Pane_Object **f_p, **s_p, **l_p ;
  int BRx, BRy ;          /* Bottom right coordinates of selection box. */
  int TLx, TLy ;          /* Top left coordinates of the selection box. */
  int size, wno, x, y ;

  wno = ec->wno ;

/* Xor (erase) old bounding box. */

  fm_draw_rect(wno, ec->start_x, ec->start_y, ec->old_x, ec->old_y) ;

  Fm->marquis_started = 0 ;
  set_timer(Fm->interval) ;      /* Restart interval timer. */

  TLx = (ec->start_x < ec->x) ? ec->start_x : ec->x ;
  TLy = (ec->start_y < ec->y) ? ec->start_y : ec->y ;
  BRx = (ec->start_x < ec->x) ? ec->x       : ec->start_x ;
  BRy = (ec->start_y < ec->y) ? ec->y       : ec->start_y ;

  if (f->display_mode == VIEW_BY_LIST && f->listopts)
    {
      TLx = 0 ;
      BRx = f->r_width ;
    }

       if (f->display_mode == VIEW_BY_CONTENT) size = CONTENT_GLYPH_WIDTH ;
  else if (f->display_mode == VIEW_BY_ICON)    size = GLYPH_WIDTH ;
  else                                         size = LIST_GLYPH_WIDTH ;

  TLx -= size ;
  TLy -= size ;

  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    {
      x = get_file_obj_x(wno, *f_p) ;
      y = get_file_obj_y(wno, *f_p) ;
      if (f->display_mode == VIEW_BY_CONTENT) x -= 16 ;
      if (f->display_mode == VIEW_BY_LIST)    y -= size ;

      if (x >= TLx && x < BRx && y >= TLy && y < BRy)
        {
          (*f_p)->selected = !(*f_p)->selected ;    /* Toggle item on/off. */
          draw_ith(f_p - s_p, wno) ;
        }
    }
}


void
fm_display_folder(mode, wno)
BYTE mode ;                     /* Build, style change, display? */
int wno ;
{
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **l_p, **s_p ;
  time_t mtime ;                /* Change date on folder. */

  if (!is_frame(wno)) return ;  /* Sanity check. */

  fm_busy_cursor(TRUE, wno) ;
  if (Fm->Renaming && wno == Fm->Rename_wno) stop_rename() ;

  if (mode == FM_NEW_FOLDER)
    {
      fm_scrollbar_scroll_to(wno, FM_V_SBAR, 0) ;
      fm_scrollbar_scroll_to(wno, FM_H_SBAR, 0) ;
    }

  if (mode == FM_BUILD_FOLDER || mode == FM_NEW_FOLDER)
    {
      if (get_pos_attr(wno, FM_POS_POS)) rebuild_folder(wno) ;
      else
        {
          build_folder(&mtime, &f->widest_name, wno) ;
          set_pos_attr(wno, FM_POS_MOVED, FALSE) ;
          Fm->mtime[wno] = mtime ;
          s_p = PTR_FIRST(wno) ;
          l_p = PTR_LAST(wno) ;
          for (f_p = s_p; f_p < l_p; f_p++)
            {
              (*f_p)->selected = FALSE ;
              my_stat((char *) f->path, f_p, wno, f_p - s_p) ;
            }
          if (f->has_cache) fast_close(f->mmap_ptr) ;
          f->has_cache = FALSE ;
          qsort((char *) f->object, f->num_objects, sizeof(File_Pane_Object *),
                Compare_fn[f->sortby]) ;
          SET_CANVAS_DIMS(wno, FM_DISPLAY_FOLDER) ;
        }
    }
  else if (mode == FM_STYLE_FOLDER)
    {

/* Sort files. */

      if (!get_pos_attr(wno, FM_POS_POS))
        qsort((char *) f->object, f->num_objects,
            sizeof(File_Pane_Object *), Compare_fn[f->sortby]) ;
    }

  display_each_folder(wno) ;
  if (wno == WASTE) set_waste_icon() ;
  set_timer(Fm->interval);   	/* set the timer ON, it is put off by */
				/*	fm_busy_cursor function */
  fm_busy_cursor(FALSE, wno) ;
}


int
fm_match_files(wno, pattern, open)     /* Select files which match pattern. */
int wno ;            /* Window number to match against. */
char *pattern ;      /* Regular expression. */
Boolean open ;       /* Open matched file(s)? */
{
  File_Pane_Object **f_p, **l_p, **s_p ;
  int i, w ;
  int matched = 0 ;
  int y = FM_EMPTY ;

  fm_pathdeselect(wno) ;
  fm_treedeselect() ;       /* Turn off old selections. */

  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p, i = 0; f_p < l_p; f_p++, i++)
    if (b_match((char *) (*f_p)->name, pattern) == 1)
      {
        matched++ ;
        if (!(*f_p)->selected)
          {
            (*f_p)->selected = TRUE ;

/*  The following two lines were added, so that if a file which is to be
 *  opened is currently unchecked, then a valid file type will be available
 *  by the time open_object is entered.
 */

            if ((*f_p)->type == FT_UNCHECKED)
              my_stat((char *) Fm->file[wno]->path, f_p, wno, f_p - s_p) ;

            if (get_file_obj_x(wno, *f_p) != FM_EMPTY) draw_ith(i, wno) ;

            if (open)

/*  Open file in window[wno] at no particular position, 1-copy only,
 *  using default open method, no subframes, no editor, no args.
 */
              open_object(*f_p, (char *) Fm->file[wno]->path, wno,
                          NULL_X, NULL_Y, NULL, FALSE, FALSE, FALSE) ;
          }
        if (y == FM_EMPTY)
          {
            y = i ;
            w = wno ;
          }
      }
    else if ((*f_p)->selected)
      {

/* Turn off old selections. */

        (*f_p)->selected = FALSE ;
        if (get_file_obj_x(wno, *f_p) != FM_EMPTY) draw_ith(i, wno) ;
      }

  if (y != FM_EMPTY) fm_scroll_to(y, w) ;   /* Scroll first into view. */
  else fm_msg(TRUE, Str[(int) M_FIND], pattern) ;
  return(matched) ;
}


int
fm_over_item(x, y, name, wno)     /* Return file index if over icon/name */
int wno, x, y ;
Boolean name ;
{
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **s_p, **l_p ;
  int fx, fy, i, j, k, max, w ;

/* Validate coordinates. */

  if (x < 0 || y < 0)  return(FM_EMPTY) ;
  if (!f->num_objects) return(FM_EMPTY) ;
  if (f->display_mode == VIEW_BY_LIST && f->listopts)
    return(fm_over_listopt_item(x, y, name, wno)) ;

  max = f->num_file_chars * Fm->font_sizeW;
  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    {
      i  = f_p - s_p ;
      fx = get_file_obj_x(wno, *f_p) ;
      fy = get_file_obj_y(wno, *f_p) ;
      if (f->display_mode != VIEW_BY_LIST)
        {
          if (name)
            {
              j = fx + (GLYPH_WIDTH / 2) ;
              k = (*f_p)->width / 2 ;
              if (x >= j-k && x <= j+k &&
                  y >= fy + GLYPH_HEIGHT &&
                  y <= fy + GLYPH_HEIGHT + Fm->font_sizeH)
                return(i) ;
            }
          else
            {
              if (f->display_mode == VIEW_BY_CONTENT && is_image(wno, i))
                {
                  if (x >= fx - 16 && x <= fx + GLYPH_WIDTH + 16 &&
                      y >= fy - GLYPH_HEIGHT &&
                      y <= fy + GLYPH_HEIGHT + Fm->font_sizeH)
                    return(i) ;
                }
              else
                if (x >= fx && x <= fx + GLYPH_WIDTH &&
                    y >= fy && y <= fy + GLYPH_HEIGHT + Fm->font_sizeH)
                  return(i) ;
            }
        }
      else
        {
          if (name)
            {
	      w = ((*f_p)->width > max) ? max : (*f_p)->width ;
              if (x >= fx + LIST_HEIGHT &&
                  x <= fx + LIST_HEIGHT + w &&
                  y >= fy - LIST_GLYPH_HEIGHT && y <= fy)
                return(i) ;
            }
          else if (x >= fx && x <= fx + LIST_GLYPH_WIDTH &&
                   y >= fy - LIST_GLYPH_HEIGHT && y <= fy)
            return(i) ;
        }
    }
  return(FM_EMPTY) ;
}


int
fm_over_listopt_item(x, y, name, wno)
int x, y, wno ;
Boolean name ;
{
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object *f_p ;
  int nx ;                        /* X coordinate of where name starts. */
  int width ;

  y = (y - LIST_OFFSET) / LIST_HEIGHT ;
  if (y >= f->num_objects) return(FM_EMPTY) ;
  f_p   = f->object[y] ;
  width = list_width(wno) ;
  nx    = (MARGIN << 1) + LIST_HEIGHT ;

  if (name)
    {
      if (x >= nx && x < (nx + f_p->width)) return(y) ;
    }
  else if (x > MARGIN && x < width) return(y) ;
  return(FM_EMPTY) ;
}


void
fm_scroll_to(n, wno)     /* Scroll to given file index in given window. */
int n, wno ;
{
  int val, x, xinc, xlen, xstart, y, yinc, ylen, ystart ;

  if (Fm->file[wno]->display_mode == VIEW_BY_LIST && Fm->file[wno]->listopts)
    {
      ystart = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_ST) ;
      ylen   = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_LEN) ;
      if (n < ystart || n >= ystart + ylen)
        fm_scrollbar_scroll_to(wno, FM_V_SBAR, n) ;
    }
  else
    {
      xstart = get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_VIEW_ST) ;
      xlen   = get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_VIEW_LEN) ;
      x      = get_file_obj_x(wno, Fm->file[wno]->object[n]) ;
      xinc   = get_pos_attr(wno, FM_POS_INCX) ;
      if (xinc)
        {
          val = x / xinc ;
          if (val < xstart || val >= xstart + xlen)
            fm_scrollbar_scroll_to(wno, FM_H_SBAR, val) ;
        }

      ystart = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_ST) ;
      ylen   = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_LEN) ;
      y      = get_file_obj_y(wno, Fm->file[wno]->object[n]) ;
      yinc   = get_pos_attr(wno, FM_POS_INCY) ;
      if (yinc)
        {
          val = y / yinc ;
          if (val < ystart || val >= ystart + ylen)
            fm_scrollbar_scroll_to(wno, FM_V_SBAR, val) ;
        }
    }
}


int
fm_set_canvas_height(new_height, wno, increment)
int new_height, wno, increment ;
{
  static int old_display_mode ;
  int sb_wid         = sb_width(wno) ;
  int visible_height = get_canvas_attr(wno, FM_WIN_HEIGHT) - sb_wid ;

  if (new_height < visible_height) new_height = visible_height ;

  if (new_height != get_canvas_attr(wno, FM_CAN_HEIGHT))
    {
      Fm->dont_paint = TRUE ;
      set_scrollbar_attr(wno, FM_V_SBAR, FM_SB_LINE_HT, increment) ;
      set_canvas_attr(wno, FM_CAN_HEIGHT, new_height) ;
      set_pos_attr(wno, FM_POS_CHEIGHT, new_height) ;
      Fm->dont_paint = FALSE ;
      old_display_mode = Fm->file[wno]->display_mode ;
      fm_file_scroll_height(wno) ;
      if (new_height < visible_height) return(0) ;
      else                             return(1) ;
    }
  set_pos_attr(wno, FM_POS_CHEIGHT, new_height) ;
  return(0) ;
}


int
fm_set_canvas_width(new_width, wno, increment)
int new_width, wno, increment ;
{
  int sb_wid        = sb_width(wno) ;
  int visible_width = get_canvas_attr(wno, FM_WIN_WIDTH) - sb_wid ;

  if (new_width < visible_width) new_width = visible_width ;

  if (new_width != get_canvas_attr(wno, FM_CAN_WIDTH))
    {
      Fm->dont_paint = TRUE ;
      set_canvas_attr(wno, FM_CAN_WIDTH, new_width) ;
      set_pos_attr(wno, FM_POS_CWIDTH, new_width) ;
      set_scrollbar_attr(wno, FM_H_SBAR, FM_SB_LINE_HT, increment) ;
      Fm->dont_paint = FALSE ;
      fm_file_scroll_width(wno) ;
      return(1) ;
    }
  set_pos_attr(wno, FM_POS_CWIDTH, new_width) ;
  return(0) ;
}


void
fm_visiblefolder(f_p)             /* Make folder visible in tree. */
register Tree_Pane_Object *f_p ;
{
  int newpos, x, y ;

/* Try and center folder in window. */

  x = (Fm->tree_vertical) ? f_p->Xpos - Fm->tree_Ymin : f_p->Xpos ;
  if (x < Fm->tree.r_left ||
      x + Fm->Hscroll_unit > Fm->tree.r_left + Fm->tree.r_width)
    {
      newpos = x - (Fm->tree.r_width >> 1) ;
      if (newpos < 0) newpos = 0 ;
      newpos /= Fm->Hscroll_unit ;
      fm_scrollbar_scroll_to(WNO_TREE, FM_H_SBAR, newpos) ;
    }

  y = (Fm->tree_vertical) ? f_p->Ypos : f_p->Ypos - Fm->tree_Ymin ;
  if (y < Fm->tree.r_top ||
      y + Fm->Vscroll_unit > Fm->tree.r_top + Fm->tree.r_height)
    {
      newpos = y - (Fm->tree.r_height >> 1) ;
      if (newpos < 0) newpos = 0 ;
      newpos /= Fm->Vscroll_unit ;
      fm_scrollbar_scroll_to(WNO_TREE, FM_V_SBAR, newpos) ;
    }
}


void
garbage_collect()
{
  char binname[MAXPATHLEN], cmd[MAXPATHLEN],*gc ;
  int found = FALSE ;

  if ((gc = getenv("DESKSETHOME")) != NULL)
    {
      SPRINTF(binname, "%s/fmgc", gc) ;
      if (access(binname, F_OK) == 0) found = TRUE ;
    }
  if (found == FALSE && (gc = getenv("OPENWINHOME")) != NULL)
    {
      SPRINTF(binname, "%s/bin/fmgc", gc) ;
      if (access(binname, F_OK) == 0) found = TRUE ;
    }
  if (found == FALSE) return ;

  fm_snprintf(cmd, sizeof(cmd), "/usr/bin/nice %s -l %d", binname, Fm->gclimit) ;
  if (fm_run_str(cmd, FALSE) != 0) fm_showerr(cmd) ;
}


enum menu_type
get_file_pane_menu(wno)
int wno ;
{
  Boolean empty         = TRUE  ;    /* Empty folder? */
  Boolean selected      = FALSE ;    /* Something selected? */
  Boolean print         = FALSE ;    /* Something selected and printable? */
  File_Pane *f          = Fm->file[wno] ;
  File_Pane_Object *f_p = get_first_selection(wno) ;
  enum menu_type m      = FM_FP_MENU ;
  WCHAR *str ;

  if (is_menu(m) == FALSE)
    {
      fm_menu_create(FM_FP_MENU) ;
      add_menu_help(m, FILE_MENU_SEL_ALL, "Edit_Select_All") ;
      add_menu_help(m, FILE_MENU_CUT,     "Edit_Cut") ;
      add_menu_help(m, FILE_MENU_COPY,    "Edit_Copy") ;
      add_menu_help(m, FILE_MENU_LINK,    "Edit_Link") ;
      add_menu_help(m, FILE_MENU_PASTE,   "Edit_Paste") ;
      add_menu_help(m, FILE_MENU_DELETE,  "Edit_Delete") ;
      add_menu_help(m, FILE_MENU_PRINT1,  "File_Default_Print") ;
      add_menu_help(m, FILE_MENU_PRINT,   "File_Custom_Print") ;
      add_menu_help(m, FILE_MENU_CLEAN,   "View_Cleanup") ;
    }

  if (is_menu(m) == TRUE)
    {

/* Only highlight those options which apply. */

      empty    = (Boolean) !Fm->file[wno]->num_objects ;

      if (f_p)
        {
          selected = TRUE ;
          if (f_p->type == FT_DOC || f_p->type == FT_APP) print = TRUE ;
        }

      mitem_inactive(m, FILE_MENU_SEL_ALL, empty) ;
      mitem_inactive(m, FILE_MENU_CUT,     (Fm->file[wno]->mtype == FM_DOS) ||
					!selected || !Fm->writable) ;
      mitem_inactive(m, FILE_MENU_COPY,    !selected) ;
      mitem_inactive(m, FILE_MENU_LINK,    !selected) ;
      mitem_inactive(m, FILE_MENU_PASTE,   !shelf_held() || !Fm->writable) ;
      mitem_inactive(m, FILE_MENU_DELETE,  !selected) ;
      mitem_inactive(m, FILE_MENU_PRINT1,  !print) ;
      mitem_inactive(m, FILE_MENU_PRINT,   !print) ;

      str = (WCHAR *) Str[(int) M_EDIT_DELETE] ;
      if (is_item(FM_PO_PO_FRAME) && !Fm->confirm_delete)
        str = (WCHAR *) Str[(int) M_EDIT_DESTROY] ;
      mitem_label(m, FILE_MENU_DELETE, str) ;

      if (selected) str = (WCHAR *) Str[(int) M_CLEANUP_SEL] ;
      else          str = (WCHAR *) Str[(int) M_CLEANUP_ICONS] ;
      mitem_label(m, FILE_MENU_CLEAN, str) ;
      mitem_inactive(m, FILE_MENU_CLEAN, f->num_objects == 0 ||
                     (f->display_mode == VIEW_BY_LIST && f->listopts)) ;
    }
  return(m) ;
}


enum menu_type
get_path_pane_menu(wno)
int wno ;
{
  enum menu_type m = FM_PP_MENU ;

  if (is_menu(m) == FALSE)
    {
      fm_menu_create(FM_PP_MENU) ;
      add_menu_help(FM_PP_MENU, PATH_MENU_OPEN, "File_Path_Open") ;
    }

  if (is_menu(m) == TRUE)
    {

/* "Show tree" item always on. */

      mitem_inactive(m, PATH_MENU_OPEN, !Fm->file[wno]->path_pane.selected) ;
    }
  return(m) ;
}


int
get_pos_attr(wno, ptype)
int wno ;
enum pos_type ptype ;
{
  File_Pane_Pos p ;
  File_Pane *f = Fm->file[wno] ;

       if (f->display_mode == VIEW_BY_CONTENT) p = f->c ;
  else if (f->display_mode == VIEW_BY_ICON)    p = f->i ;
  else if (f->listopts)                        p = f->o ;
  else                                         p = f->l ;

  switch (ptype)
    {
      case FM_POS_CHEIGHT : return(p.canvas_height) ;
      case FM_POS_CWIDTH  : return(p.canvas_width) ;
      case FM_POS_INCX    : return(p.incx) ;
      case FM_POS_INCY    : return(p.incy) ;
      case FM_POS_MAXCOL  : return(p.max_filespercol) ;
      case FM_POS_MAXLINE : return(p.max_filesperline) ;
      case FM_POS_STARTX  : return(p.startx) ;
      case FM_POS_STARTY  : return(p.starty) ;
      case FM_POS_POS     : return(p.pos) ;
      case FM_POS_MOVED   : return(p.moved) ;
      case FM_POS_HSBAR   : return(p.hsbar_pos) ;
      case FM_POS_VSBAR   : return(p.vsbar_pos) ;
    }
/*NOTREACHED*/
}


enum menu_type
get_tree_pane_menu()
{
  char *str ;
  Boolean hide     = FALSE ;         /* Hide folders inactive? */
  enum menu_type m = FM_TP_MENU ;

  if (is_menu(m) == FALSE)
    {
      fm_menu_create(FM_TP_MENU) ;
      add_menu_help(m, TREE_MENU_OPEN,  "File_Open") ;
      add_menu_help(m, TREE_MENU_OPENF, "File_Open_Folder") ;
      add_menu_help(m, TREE_MENU_HIDE,  "Tree_Pane_Hide_Subfolders") ;
      add_menu_help(m, TREE_MENU_DIR,   "Tree_Pane_Direction") ;
      add_menu_help(m, TREE_MENU_SHOW,  "Tree_Pane_Show_All_Subfolders") ;
      add_menu_help(m, TREE_MENU_START, "Tree_Pane_Start_Tree_Here") ;
      add_menu_help(m, TREE_MENU_ADD,   "Tree_Pane_Add_Trees_Parent") ;
    }

  if (is_menu(m) == TRUE)
    {

      if (Fm->tree_vertical) str = Str[(int) M_TREE_HORZ] ;
      else                   str = Str[(int) M_TREE_VERT] ;
      mitem_label(m, TREE_MENU_DIR, (WCHAR *) str) ;

/* Only highlight those options which apply. */

      hide = (Boolean) Fm->treeview && Fm->tree.selected &&
                       Fm->tree.selected->child ;

      mitem_inactive(m, TREE_MENU_OPEN,  !Fm->treeview || !Fm->tree.selected) ;
      mitem_inactive(m, TREE_MENU_OPENF, !Fm->treeview || !Fm->tree.selected) ;
      mitem_inactive(m, TREE_MENU_SHOW,  !Fm->treeview || !Fm->tree.selected) ;
      mitem_inactive(m, TREE_MENU_HIDE,  !hide) ;
      mitem_inactive(m, TREE_MENU_START, !Fm->treeview || !Fm->tree.selected) ;
      mitem_inactive(m, TREE_MENU_ADD,   Fm->tree.root == Fm->tree.head) ;
    }
  return(m) ;
}


/* Returns wastebasket pane menu with menu items activated appropriately. */

enum menu_type
get_waste_pane_menu()
{
  enum menu_type m = FM_WPANE_MENU ;

  if (is_menu(m) == FALSE)
    {
      fm_menu_create(FM_WPANE_MENU) ;
      add_menu_help(m, WASTE_PANE_EMPTY,    "Waste_Empty_Wastebasket") ;
      add_menu_help(m, WASTE_PANE_SEL_ALL,  "Waste_Select_All") ;
      add_menu_help(m, WASTE_PANE_CUT,      "Edit_Cut") ;
      add_menu_help(m, WASTE_PANE_COPY,     "Edit_Copy") ;
      add_menu_help(m, WASTE_PANE_PASTE,    "Edit_Paste") ;
      add_menu_help(m, WASTE_PANE_UNDELETE, "Waste_Undelete") ;
      add_menu_help(m, WASTE_PANE_DESTROY,  "Waste_Destroy") ;
    }

  if (is_menu(m) == TRUE) set_menu_items(FM_WPANE_MENU, set_waste_pane_menu) ;
  return(m) ;
}


int
handle_floppy_rename(devname, n, from_run_child)
char *devname ;
int n ;
int from_run_child ;
{
  FILE *fp ;
  File_Pane *f ;
  char newpath[MAXPATHLEN], oldpath[MAXPATHLEN] ;
  char mpath[MAXPATHLEN], path[MAXPATHLEN], *ptr, rawpath[MAXPATHLEN] ;
  int wno ;
  Tree_Pane_Object *tpo ;

  if ((fp = fopen(devname, "r")) == NULL)
    {
      fm_msg(TRUE, Str[(int) M_NO_READ], devname) ;
      return -1;
    }
  FSCANF(fp, "%s %s", mpath, rawpath) ;
  FCLOSE(fp) ;
  STRCPY(oldpath, Fm->fmntpt[n]) ;
  if (mpath[0] != '/')
	fm_snprintf(path, sizeof(path), "/floppy/%s", mpath);
  else strcpy(path, mpath);

  if(!from_run_child)
  	if (EQUAL(oldpath, path)) return 0;


/* Path has changed, therefore assume a rename of the floppy. */

  read_str((char **) &Fm->fmntpt[n],   path) ;
  read_str((char **) &Fm->frawpath[n], rawpath) ;
  for (wno = 0; wno < Fm->maxwin; wno++)
    if (is_frame(wno))
      {
        f = Fm->file[wno] ;
        if ((f->mtype == FM_DOS || f->mtype == FM_FLOPPY) && n == f->devno)
          {
            tpo = path_to_node(WNO_TREE, (WCHAR *) oldpath) ;
            set_tree_icon(tpo, FALSE) ;
            fm_destroy_tree(tpo->child) ;
            tpo->child = NULL ;
            fm_drawtree(TRUE) ;
            if (Fm->tree.selected) fm_visiblefolder(Fm->tree.selected) ;

            clear_position_info(wno) ;

/*  It's possible this window is looking at a sub-directory of the floppy
 *  filesystem. Need to adjust the new pathname to be the appropriate
 *  subdirectory of the new mount point.
 */

            if (EQUAL((char *) f->path, oldpath)) STRCPY(newpath, path) ;
            else
              {
                ptr = (char *) f->path + strlen(oldpath) ;
                SPRINTF(newpath, "%s%s", path, ptr) ;
		if(access(newpath, F_OK) != 0)
                       SPRINTF(newpath, "%s", path) ;
              }
            fm_openfile(newpath, (char *) NULL, TRUE, wno) ;
          }
      }
      return 1;
}


void
handle_media_removal(mtype, devno)
enum media_type mtype ;
int devno ;
{
  enum media_type wmtype ;
  int reply, wno ;
  Tree_Pane_Object *tpo ;

  for (wno = 0; wno < Fm->maxwin; wno++)
    if (is_frame(wno))
      {
        wmtype = Fm->file[wno]->mtype ;
        if (wmtype == FM_DOS) wmtype = FM_FLOPPY ;
        if (mtype == wmtype && devno == Fm->file[wno]->devno)
          {
            tpo = path_to_node(WNO_TREE, Fm->file[wno]->path) ;
            set_tree_icon(tpo, FALSE) ;
            fm_destroy_tree(tpo->child) ;
            tpo->child = NULL ;
            fm_drawtree(TRUE) ;
            if (Fm->tree.selected) fm_visiblefolder(Fm->tree.selected) ;
            if (mtype == FM_FLOPPY)
              if (Fm->confirm_delete &&
                  Fm->file[wno]->have_deleted && Fm->file[WASTE]->num_objects)
                {
                  reply = prompt_with_message(wno, Str[(int) M_DO_DELETE],
                                            Str[(int) M_YES], Str[(int) M_NO],
                                            Str[(int) M_CANCEL]) ;
                  if (reply == FM_YES) do_delete_wastebasket() ;
                }
            fm_destroy_window(wno) ;
          }
      }
}


void
handle_resize(wno)
int wno ;
{
  int h, w ;

  w = get_pw_attr(wno, FM_WIN_WIDTH) ;
  h = get_pw_attr(wno, FM_WIN_HEIGHT) ;

  if (is_resize_event() &&
     (Fm->file[wno]->pw_width != w || Fm->file[wno]->pw_height != h))
    {
      resize_window_drop_sites(wno, FALSE) ;
      reposition_status(wno) ;
      resize_path_text_item(wno) ;

      if (Fm->autosort)
        {
          set_pos_attr(wno, FM_POS_POS,   FALSE) ;
          set_pos_attr(wno, FM_POS_MOVED, FALSE) ;
          Fm->resize_dispmode = FM_STYLE_FOLDER ;
        }
      fm_display_folder(Fm->resize_dispmode, wno) ;
      adjust_pos_info(wno) ;
      Fm->file[wno]->pw_width  = w ;
      Fm->file[wno]->pw_height = h ;
    }
}


void
init_filemgr()         /* Initialize the Fm_Object struct variables. */
{
  int i ;

  Fm->DelList      = NULL ;
  Fm->dirpath      = NULL ;
  Fm->other_editor = NULL ;
  init_text() ;

  Fm->newwin       = FALSE ;
  read_str((char **) &Fm->newdoc, Str[(int) M_NEWDOCUMENT]) ;
  read_str((char **) &Fm->newdir, Str[(int) M_NEWFOLDER]) ;

  Fm->ce_bufsize   = 512 ;
  Fm->dispdir      = FM_DISP_BY_ROWS ;
  Fm->maxgoto      = FM_MAXHISTORY ;
  Fm->folder_color = 2 ;
  Fm->interval     = UPDATE_INTERVAL ;
  Fm->load_icons   = TRUE ;
  Fm->load_state   = TRUE ;
  Fm->gclimit      = GCDEFSIZE ;

  Fm->tree.border  = MARGIN ;
/*  Fm->tree.height  = GLYPH_HEIGHT + (2 * MARGIN) + Fm->defchar_height ; */
  Fm->tree_gap     = (GLYPH_WIDTH >> 1) + (GLYPH_WIDTH >> 3) ;

  Fm->Show_dnd_error  = TRUE ;
  Fm->Catch_key       = -1 ;
  Fm->Pitch_key       = -1 ;
  Fm->Drop_site_key   = -1 ;

  Fm->marquis_started  = 0 ;
  Fm->resize_dispmode  = FM_DISPLAY_FOLDER ;
  Fm->tree_vertical    = TRUE ;

  Fm->DND_BY_BITS    = FALSE ;
  Fm->add_old        = TRUE ;
  Fm->autosort       = FALSE ;
  Fm->casesen        = FALSE ;
  Fm->cd_content     = TRUE ;
  Fm->error_state    = FALSE ;
  Fm->floppy_content = TRUE ;
  Fm->fprops_showing = FALSE ;
  Fm->follow_links   = FALSE ;
  Fm->no_start_win   = FALSE ;     /* Assume saved window positions. */
  Fm->use_cache      = TRUE ;
  Fm->use_ce         = FALSE ;
  Fm->sdir_given     = FALSE ;
  Fm->see_dot        = TRUE ;
  Fm->show_cd        = TRUE ;
  Fm->show_floppy    = TRUE ;

  Fm->delete_fdr_prompt   = TRUE ;
  Fm->destroy_fdr_prompt  = TRUE ;
  Fm->ignore_custom_desel = FALSE ;
  Fm->ignore_goto_desel   = FALSE ;
  Fm->show_quit_prompt    = TRUE ;

  for (i = 0; i < 7;        i++) Fm->waste_info[i] = Fm->tree_info[i] = -1 ;
  for (i = 0; i < MAXMENUS; i++) Fm->menu_defs[i]  = -1 ;
  for (i = 0; i < MAXPOPUPS; i++) Fm->popup_wno[i] = -9 ;

  Fm->panning = FALSE ;          /* Setup panning variables. */
  Fm->panx1   = Fm->pany1 = Fm->panx2 = Fm->pany2 = 0 ;

/* Initialise X resource strings. */

  Fm->acolor_res = NULL ;
  Fm->dcolor_res = NULL ;
  Fm->fcolor_res = NULL ;

/* Custom view properties. */

  Fm->fm_res[(int) R_DISPMODE] = DGET("viewType") ;
  Fm->fm_res[(int) R_SORTBY]   = DGET("sortType") ;
  Fm->fm_res[(int) R_SEEDOT]   = DGET("showHidden") ;
  Fm->fm_res[(int) R_STYLE]    = DGET("iconListStyle") ;
  Fm->fm_res[(int) R_DISPDIR]  = DGET("iconDirectionVertical") ;

/* Tool properties. */

  Fm->fm_res[(int) R_PRINT]    = DGET("printScript") ;
  Fm->fm_res[(int) R_FILTER]   = DGET("filterScript") ;
  Fm->fm_res[(int) R_NAMEWID]  = DGET("filenameWidth") ;
  Fm->fm_res[(int) R_DELETE]   = DGET("confirmDelete") ;
  Fm->fm_res[(int) R_DELFDR]   = DGET("confirmDeleteFolder") ;
  Fm->fm_res[(int) R_DESFDR]   = DGET("confirmDestroyFolder") ;
  Fm->fm_res[(int) R_QUIT]     = DGET("confirmQuit") ;
  Fm->fm_res[(int) R_EDITOR]   = DGET("useTextedit") ;
  Fm->fm_res[(int) R_OTHERED]  = DGET("otherEditor") ;
  Fm->fm_res[(int) R_INTERVAL] = DGET("fileCheckInterval") ;
  Fm->fm_res[(int) R_NOCHARS]  = DGET("filenameNoChars") ;

/* Misc. properties. */

  Fm->fm_res[(int) R_DIRDC]    = DGET("newWindowOnDirectoryOpen") ;
  Fm->fm_res[(int) R_CEBUFN]   = DGET("classingEngineBufferSize") ;
  Fm->fm_res[(int) R_CENGINE]  = DGET("useClassingEngine") ;
  Fm->fm_res[(int) R_NEWDOC]   = DGET("newDocumentName") ;
  Fm->fm_res[(int) R_NEWDIR]   = DGET("newFolderName") ;
  Fm->fm_res[(int) R_NOSWINS]  = DGET("noSavedWindows") ;
  Fm->fm_res[(int) R_TREEVIEW] = DGET("treeView") ;
  Fm->fm_res[(int) R_DCOLOR]   = DGET("documentColor") ;
  Fm->fm_res[(int) R_FCOLOR]   = DGET("folderColor") ;
  Fm->fm_res[(int) R_ACOLOR]   = DGET("applicationColor") ;
  Fm->fm_res[(int) R_SHTYPE]   = DGET("shellToolName") ;
  Fm->fm_res[(int) R_MAXGOTO]  = DGET("maxGotoMenuEntries") ;
  Fm->fm_res[(int) R_SYMLINKS] = DGET("followSymbolicLinks") ;
  Fm->fm_res[(int) R_NOUGOTO]  = DGET("noUserGotoEntries") ;
  Fm->fm_res[(int) R_NOCCMDS]  = DGET("noCustomCommands") ;
  Fm->fm_res[(int) R_DOCD]     = DGET("autoShowCD") ;
  Fm->fm_res[(int) R_LSTATE]   = DGET("loadDirectoryState") ;
  Fm->fm_res[(int) R_LICONS]   = DGET("loadIconPositions") ;
  Fm->fm_res[(int) R_TREEGAP]  = DGET("treePaneGap") ;
  Fm->fm_res[(int) R_TREEDIR]  = DGET("treeDirectionVertical") ;
  Fm->fm_res[(int) R_OLDCCMDS] = DGET("useOldCustomCommands") ;
  Fm->fm_res[(int) R_FL_CON]   = DGET("floppyContentMatch") ;
  Fm->fm_res[(int) R_CD_CON]   = DGET("cdromContentMatch") ;
  Fm->fm_res[(int) R_SORTCASE] = DGET("sortCaseSensitive") ;
  Fm->fm_res[(int) R_AUTOSORT] = DGET("autoSortOnUpdate") ;
  Fm->fm_res[(int) R_CACHE]    = DGET("useCache") ;
  Fm->fm_res[(int) R_GCSIZE]   = DGET("cacheSize") ;

/* Directory info properties not included above. */

  Fm->fm_res[(int) R_ICONX]    = DGET("iconXPosition") ;
  Fm->fm_res[(int) R_ICONY]    = DGET("iconYPosition") ;
  Fm->fm_res[(int) R_WINX]     = DGET("windowXPosition") ;
  Fm->fm_res[(int) R_WINY]     = DGET("windowYPosition") ;
  Fm->fm_res[(int) R_WINW]     = DGET("windowWidth") ;
  Fm->fm_res[(int) R_WINH]     = DGET("windowHeight") ;
  Fm->fm_res[(int) R_CWIDTH]   = DGET("CanvasWidth") ;
  Fm->fm_res[(int) R_CHEIGHT]  = DGET("CanvasHeight") ;
  Fm->fm_res[(int) R_INCX]     = DGET("XIncrement") ;
  Fm->fm_res[(int) R_INCY]     = DGET("YIncrement") ;
  Fm->fm_res[(int) R_MAXCOL]   = DGET("MaxFilesPerColumn") ;
  Fm->fm_res[(int) R_MAXLINE]  = DGET("MaxFilesPerLine") ;
  Fm->fm_res[(int) R_STARTX]   = DGET("XStartPosition") ;
  Fm->fm_res[(int) R_STARTY]   = DGET("YStartPosition") ;
  Fm->fm_res[(int) R_MOVED]    = DGET("IconsMoved") ;
  Fm->fm_res[(int) R_HSBAR]    = DGET("HorizontalScrollbarPosition") ;
  Fm->fm_res[(int) R_VSBAR]    = DGET("VerticalScrollbarPosition") ;

/* Default menu entries. */

  Fm->fm_res[(int) R_FILE_MENU]  = DGET("fileMenuDefault") ;
  Fm->fm_res[(int) R_VIEW_MENU]  = DGET("viewMenuDefault") ;
  Fm->fm_res[(int) R_EDIT_MENU]  = DGET("editMenuDefault") ;
  Fm->fm_res[(int) R_GOTO_MENU]  = DGET("gotoMenuDefault") ;
  Fm->fm_res[(int) R_PP_MENU]    = DGET("pathFloatingMenuDefault") ;
  Fm->fm_res[(int) R_TFILE_MENU] = DGET("folderViewFileMenuDefault") ;
  Fm->fm_res[(int) R_TVIEW_MENU] = DGET("folderViewViewMenuDefault") ;
  Fm->fm_res[(int) R_TP_MENU]    = DGET("folderViewFloatingMenuDefault") ;
  Fm->fm_res[(int) R_FP_MENU]    = DGET("iconFloatingMenuDefault") ;
  Fm->fm_res[(int) R_WEDIT_MENU] = DGET("wasteEditMenuDefault") ;
  Fm->fm_res[(int) R_WPANE_MENU] = DGET("wasteFloatingMenuDefault") ;
  Fm->fm_res[(int) R_CC_MENU]          = DGET("customCommandsMenuDefault") ;
  Fm->fm_res[(int) R_CC_GOTO_EL_MENU]  = DGET("propsEditMenuDefault") ;
  Fm->fm_res[(int) R_CC_GOTO_ELP_MENU] = DGET("propsPasteMenuDefault") ;
}


Boolean
is_image(wno, off)
int wno, off ;
{
  return(Fm->file[wno]->object[off]->image != 0) ;
}


void
load_positional_info(wno, str)
int wno ;
char *str ;
{
  int intval ;
  File_Pane_Pos *p ;

       if (EQUAL(str, "content")) p = &Fm->file[wno]->c ;
  else if (EQUAL(str, "icon"))    p = &Fm->file[wno]->i ;
  else if (EQUAL(str, "list"))    p = &Fm->file[wno]->l ;

  if (get_pos_resource(str, R_CWIDTH,  &intval)) p->canvas_width    = intval ;
  if (get_pos_resource(str, R_CHEIGHT, &intval)) p->canvas_height   = intval ;
  if (get_pos_resource(str, R_INCX,    &intval)) p->incx            = intval ;
  if (get_pos_resource(str, R_INCY,    &intval)) p->incy            = intval ;
  if (get_pos_resource(str, R_MAXCOL,  &intval)) p->max_filespercol = intval ;
  if (get_pos_resource(str, R_MAXLINE, &intval)) p->max_filesperline = intval ;
  if (get_pos_resource(str, R_STARTX,  &intval)) p->startx          = intval ;
  if (get_pos_resource(str, R_STARTY,  &intval)) p->starty          = intval ;
  if (get_pos_resource(str, R_MOVED,   &intval)) p->moved           = intval ;
  p->pos = p->moved ;
  if (get_pos_resource(str, R_HSBAR,   &intval)) p->hsbar_pos       = intval ;
  if (get_pos_resource(str, R_VSBAR,   &intval)) p->vsbar_pos       = intval ;
}


int
make_pos_dir(name, mode)
char *name ;
int mode ;
{
  int err ;
  char *slash ;

  if (mkdir(name, mode) == 0 || errno == EEXIST) return(0) ;
  if (errno != ENOENT)                           return(-1) ;
  if ((slash = strrchr(name, '/')) == NULL)      return(-1) ;
  *slash = '\0' ;
  err = make_pos_dir(name, 0777) ;
  *slash++ = '/' ;
  if (err || !*slash) return(err) ;
  return(mkdir(name, mode)) ;
}


void
my_stat(path, f_p, wno, off)
char *path ;
File_Pane_Object **f_p ;
int wno, off ;
{
  File_Pane *f = Fm->file[wno] ;
  struct stat fstatus ;
  int bufsize ;             /* Amount of data to read from file for CE. */
  int cache_read = FALSE ;
  int reply = 0 ;
  char *newpath;

/* Use full path name; we may not be in this directory! */

  newpath = fm_malloc(strlen((char *) (*f_p)->name) + strlen(path) + 2);
  sprintf(newpath, "%s/%s", path, (*f_p)->name);

  if (f->has_cache)
    {
      if (f->cptr >= (Cache_Object *) ((f->mmap_ptr->addr+4) + f->ctext))
        f->has_cache = FALSE ;
      else if (!strncmp((char *) (*f_p)->name,
                   (f->mmap_ptr->addr+4) + f->cptr->foff, f->cptr->flen))
        {
          if ((f->cptr->st_mode & S_IFMT) == S_IFDIR)
            {
              (*f_p)->type  = FT_DIR ;
              (*f_p)->color = Fm->folder_color ;
            }
          else if ((f->cptr->st_mode & S_IFMT) == S_IFREG)
            {
              if (f->cptr->st_mode & S_IEXEC)
                {
                  (*f_p)->type  = FT_APP ;
                  (*f_p)->color = FM_APP_COLOR ;
                }
              else if (f->cptr->st_mode & S_IFREG)
                {
                  (*f_p)->type  = FT_DOC ;
                  (*f_p)->color = FM_DOC_COLOR ;
                }
            }
          else
            {
              (*f_p)->type  = FT_SYS ;
              (*f_p)->color = FM_APP_COLOR ;
            }
          (*f_p)->mode  = f->cptr->st_mode ;
          (*f_p)->nlink = f->cptr->st_nlink ;
          (*f_p)->uid   = f->cptr->st_uid ;
          (*f_p)->gid   = f->cptr->st_gid ;
          (*f_p)->mtime = f->cptr->mtime ;
          (*f_p)->ctime = f->cptr->ctime ;
          (*f_p)->size  = f->cptr->st_size ;
          (*f_p)->tlen  = f->cptr->tlen ;
          if ((*f_p)->tlen)
            {
              (*f_p)->type_name = fm_malloc((unsigned)((*f_p)->tlen + 1)) ;
              STRNCPY((*f_p)->type_name,
                      (f->mmap_ptr->addr+4) + f->cptr->toff, (*f_p)->tlen) ;
            }
          f->cptr++ ;
          cache_read = TRUE ;
        }
      else f->has_cache = FALSE ;               /* Invalidate cache. */
    }

  if (cache_read == FALSE)
    if (fm_stat(newpath, &fstatus) == 0)
      {
        if ((fstatus.st_mode & S_IFMT) == S_IFDIR)
          {
            (*f_p)->type  = FT_DIR ;
            (*f_p)->color = Fm->folder_color ;
          }
        else if ((fstatus.st_mode & S_IFMT) == S_IFREG)
          {
            if (fstatus.st_mode & S_IEXEC)
              {
                (*f_p)->type  = FT_APP ;
                (*f_p)->color = FM_APP_COLOR ;
              }
            else if (fstatus.st_mode & S_IFREG)
              {
                (*f_p)->type  = FT_DOC ;
                (*f_p)->color = FM_DOC_COLOR ;
              }
          }
        else
          {
            (*f_p)->type  = FT_SYS ;
            (*f_p)->color = FM_APP_COLOR ;
          }
        (*f_p)->mode  = fstatus.st_mode ;
        (*f_p)->nlink = fstatus.st_nlink ;
        (*f_p)->uid   = fstatus.st_uid ;
        (*f_p)->gid   = fstatus.st_gid ;
        (*f_p)->mtime = fstatus.st_mtime ;
        (*f_p)->ctime = fstatus.st_ctime ;
        (*f_p)->size  = fstatus.st_size ;
      }
  else
    {
      (*f_p)->type  = FT_BAD_LINK ;
      (*f_p)->color = FM_APP_COLOR ;
    }

/* Get the type namespace entry handle and icon colors. */

  (*f_p)->tns_entry = NULL ;
  if ((*f_p)->type == FT_DIR)
    {
      set_fp_color(*f_p, FM_FP_FGCOLOR, FM_FG_DEF_COLOR) ;
      set_fp_color(*f_p, FM_FP_BGCOLOR, FM_BG_DEF_COLOR) ;
    }

/* else fg/bg will be gotten later. */

  (*f_p)->color = 0 ;

  if ((*f_p)->type != FT_DIR)
    {
      bufsize = Fm->ce_bufsize ;
      if (Fm->file[wno]->mtype == FM_CD && Fm->cd_content == FALSE)
        bufsize = 0 ;
      if ((Fm->file[wno]->mtype == FM_FLOPPY ||
           Fm->file[wno]->mtype == FM_DOS) && Fm->floppy_content == FALSE)
        bufsize = 0 ;
      if (cache_read)
        {
          (*f_p)->tns_entry = NULL ;
          if ((*f_p)->tlen)
            (*f_p)->tns_entry = ce_get_entry(Fm->ceo->type_ns, 1, (*f_p)->type_name) ;
        }
      else
        {
          (*f_p)->tns_entry = get_tns_entry(*f_p, newpath, bufsize) ;
          (*f_p)->type_name = get_tns_attr((*f_p)->tns_entry, Fm->ceo->type_name) ;
          (*f_p)->tlen      = 0 ;
          if ((*f_p)->type_name) (*f_p)->tlen = strlen((*f_p)->type_name) ;
        }

      set_fp_color(*f_p, FM_FP_FGCOLOR, FM_FGCOLOR) ;
      set_fp_color(*f_p, FM_FP_BGCOLOR, FM_BGCOLOR) ;
      (*f_p)->color = 0 ;

/*  If we are viewing by content, then check for a variety of formats:
 *  The following formats are currently supported:
 *
 *  Sun rasterfile format - 1 bit depth.
 *  Sun icon format       - 1 bit depth.
 *  X bitmap file format  - 1 bit depth.
 *  XPM (X pixmap) format - 8 bit depth.
 *
 *  If one of these is found, the image is read in and converted to a 64x64
 *  server image, reducing the image if needed.
 */
      if (Fm->file[wno]->display_mode == VIEW_BY_CONTENT)
        (*f_p)->image_depth = set_fp_image(newpath, wno, off) ;
    }
    free(newpath);
}


/* Event interpose procedure for the path and tree canvases. */

void
path_tree_event(ec)
Event_Context ec ;
{
  char message[MAXLINE] ;
  int id, h, w ;
  int wno = ec->wno ;

  switch (ec->fm_id)
    {
      case E_FKEY : if (ec->down)
                      fm_check_keys() ;    /* User pressed a function key. */
                    break ;

      case E_RESIZE :             /* Window resized. */
        if (is_resize_event()) resize_window_drop_sites(wno, TRUE) ;
        if (wno == WNO_TREE)
          {
            w = get_pw_attr(WNO_TREE, FM_WIN_WIDTH) ;
            h = get_pw_attr(WNO_TREE, FM_WIN_HEIGHT) ;
            if (Fm->tree.pw_width != w || Fm->tree.pw_height != h)
              {
                fm_drawtree(TRUE) ;
                Fm->tree.pw_width  = w ;
                Fm->tree.pw_height = h ;
              }
          }
        break ;

      case E_DRAG_COPY :       /* New style dnd drop event. */
      case E_DRAG_MOVE :       /* New style dnd drop event. */
        ec = tree_check_if_over_item(ec) ;
        if (ec->over_item) do_new_dnd_drop(ec) ;
        else
          {
            Fm->Show_dnd_error = FALSE ;    /* XXX: kludge */
            fm_msg(TRUE, Str[(int) M_NOT_FDR]) ;
          }
        break ;

      case E_DRAG_PREVIEW :    /* New style dnd preview events. */
        id = get_event_id() ;
        if (id == E_WINEXIT)
          {
            if (ec->state == DND_OVER_ITEM)
              {

/* Clear the old image. */

                set_event_state(ec, CLEAR) ;
                over_path_tree_folder(wno, ec->old_tree_item, FALSE) ;
              }
          }
        else if (id == E_DRAG)
          {
            ec = tree_check_if_over_item(ec) ;
            if (ec->state == DND_OVER_ITEM)
              {
                if (!ec->over_item || ec->old_tree_item != ec->tree_item)
                  {

/* Clear the old image. */

                    set_event_state(ec, CLEAR) ;
                    over_path_tree_folder(wno, ec->old_tree_item, FALSE) ;
                  }
              }
            else
              {
                if (ec->over_item)
                  {

/* Invert the image to show we are over it. */

                    set_event_state(ec, DND_OVER_ITEM) ;
                    SPRINTF(message, Str[(int) M_OVER], ec->tree_item->name) ;
                    write_footer(wno, message, FALSE) ;
                    over_path_tree_folder(wno, ec->tree_item, TRUE) ;
                    ec->old_tree_item = ec->tree_item ;
                  }
              }
          }
        break ;

      case E_DRAG_LOAD :       /* Old style dnd drop event. */
        fm_msg(TRUE, Str[(int) M_DND_OLD]) ;
        break ;

      case E_MENU : tree_path_menu(wno) ;  /* Show/hide floating menu. */
                    break ;

      case E_SELECT :
      case E_ADJUST :
        ec = tree_check_if_over_item(ec) ;
        if (ec->down)
          {
            if (ec->meta)
              set_event_state(ec, MAYBE_PAN) ;
            else if (ec->over_item)
              {
                set_event_state(ec, MAYBE_DRAG) ;
                fm_all_filedeselect() ;             /* Deselect all files. */
                fm_all_pathdeselect(wno) ;
                if (wno != WNO_TREE) fm_treedeselect() ;
                set_event_time(ec) ;
                if (ec->tree_item == ec->old_tree_item &&
                    fm_is_double_click(ec->old_sec, ec->old_usec,
                                       ec->sec, ec->usec,
                                       ec->old_x, ec->old_y, ec->x, ec->y))
                  {
                    set_event_state(ec, OPENING) ;
                    if (wno == WNO_TREE) fm_open_folder(FALSE) ;
                    else
                      {
                        fm_open_path(wno, Fm->newwin) ;
                        Fm->tree.current = path_to_node(WNO_TREE,
                                                         Fm->file[wno]->path) ;
                        adjust_tree(wno) ;
                        fm_showopen() ;
                        fm_treedeselect() ;
                      }
                    ec->old_sec = 0 ;     /* Clear the double-click. */
                    ec->old_usec = 0 ;
                  }
                else
                  {
                    path_tree_select_item(ec) ;
                    if (wno == WNO_TREE)
                      {
                        SPRINTF(message, Str[(int) M_TREESEL],
                                      (char *) ec->tree_item->name) ;
                        write_footer(WNO_TREE, message, FALSE) ;
                      }
                    ec->old_sec = ec->sec ;
                    ec->old_usec = ec->usec ;
                  }
                ec->old_tree_item = ec->tree_item ;
              }
            else
              {
                ec->old_sec = 0 ;               /* Clear the double-click. */
                ec->old_usec = 0 ;
                fm_all_filedeselect() ;         /* Deselect all files. */
                fm_all_pathdeselect(WNO_TREE) ;
                fm_treedeselect() ;
              }

            ec->start_x = ec->old_x = ec->x ;   /* Save starting x & y pos. */
            ec->start_y = ec->old_x = ec->y ;
          }
        else                                    /* Key is up. */
          {
            if (ec->state == PAN) finish_panning(ec) ;
            else if (ec->state == MAYBE_DRAG)
              {
                if (ec->over_item)

/* User did not drag -- make sure to deselect other files now. */

                  path_tree_select_item(ec) ;
              }

/*  If the file properties window is currently visible, and there is a
 *  path/tree folder selected, then show file property information.
 */
            if (Fm->fprops_showing) do_file_info_popup(wno) ;
            if (ec->state != RENAMING)
              set_event_state(ec, CLEAR) ;
          }
        break ;

      case E_DRAG :
        if (ec->state == MAYBE_PAN || ec->state == PAN)
          {
            set_event_state(ec, PAN) ;
            do_panning(ec) ;
          }
        else if (ec->state == MAYBE_DRAG)
          {
            if (abs(ec->x - ec->start_x) > THRESHOLD ||
                abs(ec->y - ec->start_y) > THRESHOLD)
              {
                set_event_state(ec, CLEAR) ;
                start_dnd(ec) ;
              }
          }
        break ;

      default : break ;
    }
}


void
read_menu_defaults()
{
  int intval ;

  if (get_int_resource(FM_MERGE_DB, R_FILE_MENU,        &intval))
    Fm->menu_defs[(int) FM_FILE_MENU]        = intval ;
  if (get_int_resource(FM_MERGE_DB, R_VIEW_MENU,        &intval))
    Fm->menu_defs[(int) FM_VIEW_MENU]        = intval ;
  if (get_int_resource(FM_MERGE_DB, R_EDIT_MENU,        &intval))
    Fm->menu_defs[(int) FM_EDIT_MENU]        = intval ;
  if (get_int_resource(FM_MERGE_DB, R_GOTO_MENU,        &intval))
    Fm->menu_defs[(int) FM_GOTO_MENU]        = intval ;
  if (get_int_resource(FM_MERGE_DB, R_PP_MENU,          &intval))
    Fm->menu_defs[(int) FM_PP_MENU]          = intval ;
  if (get_int_resource(FM_MERGE_DB, R_TFILE_MENU,       &intval))
    Fm->menu_defs[(int) FM_TFILE_MENU]       = intval ;
  if (get_int_resource(FM_MERGE_DB, R_TVIEW_MENU,       &intval))
    Fm->menu_defs[(int) FM_TVIEW_MENU]       = intval ;
  if (get_int_resource(FM_MERGE_DB, R_TP_MENU,          &intval))
    Fm->menu_defs[(int) FM_TP_MENU]          = intval ;
  if (get_int_resource(FM_MERGE_DB, R_FP_MENU,          &intval))
    Fm->menu_defs[(int) FM_FP_MENU]          = intval ;
  if (get_int_resource(FM_MERGE_DB, R_WEDIT_MENU,       &intval))
    Fm->menu_defs[(int) FM_WEDIT_MENU]       = intval ;
  if (get_int_resource(FM_MERGE_DB, R_WPANE_MENU,       &intval))
    Fm->menu_defs[(int) FM_WPANE_MENU]       = intval ;
  if (get_int_resource(FM_MERGE_DB, R_CC_MENU,          &intval))
    Fm->menu_defs[(int) FM_CC_MENU]          = intval ;
}


void
read_resources()    /* Read all possible resources from the database. */
{
  int boolval, intval ;
  char str[MAXLINE] ;

/* Note the resources should not be localized. */

/* Get custom view properties. */

  if (get_str_resource(FM_MERGE_DB, R_DISPMODE, str))
    {
           if (EQUAL(str, "icon"))    Fm->display_mode = VIEW_BY_ICON ;
      else if (EQUAL(str, "list"))    Fm->display_mode = VIEW_BY_LIST  ;
      else if (EQUAL(str, "content")) Fm->display_mode = VIEW_BY_CONTENT ;
    }
  if (get_str_resource(FM_MERGE_DB, R_SORTBY, str))
    {
           if (EQUAL(str, "name")) Fm->sortby = SORT_BY_NAME ;
      else if (EQUAL(str, "type")) Fm->sortby = SORT_BY_TYPE ;
      else if (EQUAL(str, "size")) Fm->sortby = SORT_BY_SIZE ;
      else if (EQUAL(str, "date")) Fm->sortby = SORT_BY_TIME ;
    }
  if (get_bool_resource(FM_MERGE_DB, R_SEEDOT, &boolval))
    Fm->see_dot = boolval ;
  if (get_int_resource(FM_MERGE_DB, R_STYLE, &intval))
    Fm->listopts = intval ;

/*  We are only interested in the last 6 bits for 6 list options.
 *  Clear all other bits just in case (bug 1061766).
 */

  Fm->listopts = Fm->listopts & 077 ;
  if (get_bool_resource(FM_MERGE_DB, R_DISPDIR, &boolval))
    Fm->dispdir = boolval ;

/* Get tool properties. */

  if (get_str_resource(FM_MERGE_DB, R_PRINT, str))
    STRCPY((char *) Fm->print_script, str) ;
  if (get_str_resource(FM_MERGE_DB, R_FILTER, str))
    STRCPY((char *) Fm->filter, str) ;

  if (get_int_resource(FM_MERGE_DB, R_CEBUFN, &intval))
    if (intval >= 0 && intval <= 32768)
      Fm->ce_bufsize = intval ;

  if (get_str_resource(FM_MERGE_DB, R_NEWDOC, str))
    read_str((char **) &Fm->newdoc, str) ;
  if (get_str_resource(FM_MERGE_DB, R_NEWDIR, str))
    read_str((char **) &Fm->newdir, str) ;

  if (get_int_resource(FM_MERGE_DB, R_NOCHARS, &intval))
    Fm->num_file_chars = intval ;
  if (get_bool_resource(FM_MERGE_DB, R_DELETE, &boolval))
    Fm->confirm_delete = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_DELFDR, &boolval))
    Fm->delete_fdr_prompt = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_DESFDR, &boolval))
    Fm->destroy_fdr_prompt = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_QUIT, &boolval))
    Fm->show_quit_prompt = boolval ;

  if (get_bool_resource(FM_MERGE_DB, R_EDITOR, &boolval))
    Fm->editor = !boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_SYMLINKS, &boolval))
    Fm->follow_links = boolval ;
  if (get_str_resource(FM_MERGE_DB, R_OTHERED, str))
    read_str((char **) &Fm->other_editor, str) ;
  if (get_int_resource(FM_MERGE_DB, R_INTERVAL, &intval))
    Fm->interval = intval ;

  if (Fm->other_editor && Fm->other_editor[0] == 0) set_default(DEF_EDITOR) ;

/* Misc. values. */

  if (get_bool_resource(FM_MERGE_DB, R_DIRDC,    &boolval))
    Fm->newwin = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_CENGINE,  &boolval))
    Fm->use_ce = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_TREEVIEW, &boolval))
    Fm->treeview = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_DOCD,     &boolval))
    Fm->show_cd = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_FL_CON,   &boolval))
    Fm->floppy_content = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_CD_CON,   &boolval))
    Fm->cd_content = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_SORTCASE, &boolval))
    Fm->casesen = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_AUTOSORT, &boolval))
    Fm->autosort = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_CACHE,    &boolval))
    Fm->use_cache = boolval ;

  if (get_int_resource(FM_MERGE_DB, R_GCSIZE, &intval)) Fm->gclimit = intval ;
  if (Fm->gclimit < 0 || Fm->gclimit > 100000000) Fm->gclimit = GCDEFSIZE ;

  if (get_bool_resource(FM_MERGE_DB, R_OLDCCMDS, &boolval))
    Fm->add_old = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_TREEDIR,  &boolval))
    Fm->tree_vertical = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_LSTATE,   &boolval))
    Fm->load_state = boolval ;
  if (get_bool_resource(FM_MERGE_DB, R_LICONS,   &boolval))
    Fm->load_icons = boolval ;
  if (Fm->load_icons == TRUE) Fm->load_state = TRUE ;

  if (get_str_resource(FM_MERGE_DB, R_DCOLOR, str))
    {
      read_str(&Fm->dcolor_res, str) ;
      set_color(str, 0) ;
    }

  if (get_str_resource(FM_MERGE_DB, R_FCOLOR, str))
    {
      read_str(&Fm->fcolor_res, str) ;
      set_color(str, 1) ;
    }

  if (get_str_resource(FM_MERGE_DB, R_ACOLOR, str))
    {
      read_str(&Fm->acolor_res, str) ;
      set_color(str, 2) ;
    }

  read_window_info(WASTE,    Fm->waste_info) ;
  read_window_info(WNO_TREE, Fm->tree_info) ;
  read_menu_defaults() ;

  if (get_str_resource(FM_MERGE_DB, R_SHTYPE, str))
    read_str(&Fm->shellname, str) ;
  if (Fm->shellname == NULL || *Fm->shellname == '\0')
    read_str(&Fm->shellname, "cmdtool") ;

  if (get_int_resource(FM_MERGE_DB, R_NOSWINS, &intval))
    Fm->no_saved_wins = intval ;
  if (get_int_resource(FM_MERGE_DB, R_NOCCMDS, &intval))
    Fm->no_ccmds = intval ;

  if (get_int_resource(FM_MERGE_DB, R_MAXGOTO, &intval))
    Fm->maxgoto = intval ;
  if (Fm->maxgoto < 1 || Fm->maxgoto > 500)
    Fm->maxgoto = FM_MAXHISTORY ;

  if (get_int_resource(FM_MERGE_DB, R_TREEGAP, &intval))
    Fm->tree_gap = intval ;
  if (Fm->tree_gap < 48 || Fm->tree_gap > 320)
    Fm->tree_gap = GLYPH_WIDTH * 3 ;
}


void
redisplay_folder(wno, rx, ry, rw, rh)
int wno, rx, ry, rw, rh ;
{
  File_Pane_Object **f_p, **l_p, **s_p ;    /* Files array pointers. */
  int canvas_height, canvas_width, incx, incy, x, y ;
  int x1, y1, x2, y2 ;                      /* Clipping limits. */

  if (is_visible(wno) == FALSE) return ;    /* Don't draw if not visible. */

  canvas_height = get_pos_attr(wno, FM_POS_CHEIGHT) ;
  canvas_width  = get_pos_attr(wno, FM_POS_CWIDTH) ;
  incx          = get_pos_attr(wno, FM_POS_INCX) ;
  incy          = get_pos_attr(wno, FM_POS_INCY) ;

  x1 = rx - incx ;
  if (x1 < 0) x1 = 0 ;
  x2 = rx + rw + incx ;
  if (x2 > canvas_width) x2 = canvas_width ;

  y1 = ry - incy ;
  if (y1 < 0) y1 = 0 ;
  y2 = ry + rh + incy ;
  if (y2 > canvas_height) y2 = canvas_height ;

  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    {
      x = get_file_obj_x(wno, *f_p) ;
      y = get_file_obj_y(wno, *f_p) ;
      if (y < y1 || y > y2 || x < x1 || x > x2) continue ;
      draw_ith(f_p - s_p, wno) ;
    }
}


void
reset_file_pane(wno)       /* Reset various File_Pane attributes. */
int wno ;
{
  File_Pane_Object **f_p, **l_p, **s_p ;

  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++) (*f_p)->width = 0 ;

  Fm->file[wno]->num_objects = 0 ;
}


void
resize_prop_sheet(frame, panel, button, width)
enum item_type frame, panel, button ;
int width ;
{
  int bheight, by, h ;

  by      = get_item_int_attr(button, FM_ITEM_Y) ;
  bheight = get_item_int_attr(button, FM_ITEM_HEIGHT) ;
  set_item_int_attr(panel, FM_ITEM_WIDTH,  width) ;
  set_item_int_attr(frame, FM_ITEM_WIDTH,  width) ;
  h = by + bheight ;
  h += (is_input_region(FM_PO_PO_FRAME)) ? 75 : 50 ;
  set_item_int_attr(frame, FM_ITEM_HEIGHT, h) ;
}


void
save_directory_info(wno)
int wno ;
{
  File_Pane *f = Fm->file[wno] ;
  char fname[MAXPATHLEN] ;

/*  If the directory being monitored is in the ~/.fm hierarchy, don't save
 *  any information (because of recursion).
 */

  SPRINTF(fname, "%s/.fm", Fm->home) ;
  if (EQUALN((char *) f->path, fname)) return ;

  SPRINTF(fname, "%s/.fm%s", Fm->home, f->path) ;
  if (make_pos_dir(fname, 0777) != -1)
    {
      if (Fm->load_state)
        {
          SPRINTF(fname, "%s/.fm%s/.state", Fm->home, f->path) ;
          load_dir_defs(fname) ;
          write_dir_defs(wno) ;
          save_dir_defs(fname) ;
        }
      if (Fm->use_cache) write_cache(wno) ;
      if ((f->i.moved || f->l.moved) && Fm->load_icons) write_icon_positions(wno) ;
    }
}


void
save_window_info(wno)
int wno ;
{
  char intval[16] ;
  int info[MAXINFO] ;

  if (is_frame(wno))
    {
      get_win_X_info(wno, info) ;
      put_win_info(wno, DGET("Closed"), set_bool(info[0] == TRUE)) ;
      SPRINTF(intval, "%d", info[1]) ;
      put_win_info(wno, DGET("IconXPosition"), intval) ;
      SPRINTF(intval, "%d", info[2]) ;
      put_win_info(wno, DGET("IconYPosition"), intval) ;
      SPRINTF(intval, "%d", info[3]) ;
      put_win_info(wno, DGET("WindowXPosition"), intval) ;
      SPRINTF(intval, "%d", info[4]) ;
      put_win_info(wno, DGET("WindowYPosition"), intval) ;
      SPRINTF(intval, "%d", info[5]) ;
      put_win_info(wno, DGET("Width"), intval) ;
      SPRINTF(intval, "%d", info[6]) ;
      put_win_info(wno, DGET("Height"), intval) ;
    }
}


void
save_window_positions()   /* Write window info. to ~/.desksetdefaults. */
{
  char intval[16] ;
  int i, wno ;

  load_deskset_defs() ;

  wno = 0 ;
  for (i = WIN_SPECIAL+1; i < Fm->maxwin; i++)
    if (is_frame(i) && Fm->file[i]->mtype == FM_DISK)
      {
        wno++ ;
        save_window_info(wno) ;
        put_win_info(wno, DGET("Path"), (char *) Fm->file[i]->path) ;
      }

  SPRINTF(intval, "%d", wno) ;
  fm_put_resource(FM_DESKSET_DB, R_NOSWINS, intval) ;

  save_window_info(WASTE) ;
  save_window_info(WNO_TREE) ;
  fm_put_resource(FM_DESKSET_DB, R_TREEDIR,
                  set_bool(Fm->tree_vertical == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_TREEVIEW, set_bool(Fm->treeview == TRUE)) ;
  write_menu_defaults() ;
  save_resources() ;
}


char *
set_bool(value)
int value ;
{
  return((value) ? "true" : "false") ;
}


void
set_edit_button_menu()
{
  WCHAR *str ;
  int wno          = Fm->curr_wno ;
  Boolean empty    = TRUE ;         /* Empty folder? */
  Boolean selected = FALSE ;        /* Anything selected. */
  enum menu_type m = FM_EDIT_MENU ;

/* Only highlight those options which apply. */

  empty    = (Boolean) !Fm->file[wno]->num_objects ;
  selected = (Boolean) (Fm->file[wno]->path_pane.selected ||
                       (get_first_selection(wno) != NULL)) ;

  str = (WCHAR *) Str[(int) M_EDIT_DELETE] ;
  if (is_item(FM_PO_PO_FRAME) && !Fm->confirm_delete)
    str = (WCHAR *) Str[(int) M_EDIT_DESTROY] ;
  mitem_label(m, EDIT_BUT_DELETE, str) ;

  mitem_inactive(m, EDIT_BUT_SEL_ALL, empty) ;
  mitem_inactive(m, EDIT_BUT_CUT,     Fm->file[wno]->mtype == FM_DOS || !selected || !Fm->writable) ;
  mitem_inactive(m, EDIT_BUT_COPY,    !selected) ;
  mitem_inactive(m, EDIT_BUT_LINK,    !selected) ;
  mitem_inactive(m, EDIT_BUT_PASTE,   !shelf_held() || !Fm->writable) ;
  mitem_inactive(m, EDIT_BUT_DELETE,  !selected) ;
}


void
set_file_button_menu()
{
  Boolean eraseable     = FALSE ;   /* Set true for floppy disks. */
  Boolean fselected     = FALSE ;   /* Something selected in file pane? */
  Boolean pselected     = FALSE ;   /* Something selected in path pane? */
  Boolean print         = FALSE ;   /* Something selected and printable? */
  Boolean floppy0       = FALSE ;   /* Window for floppy0? */
  int wno               = Fm->curr_wno ;
  File_Pane *f          = Fm->file[wno] ;
  File_Pane_Object *f_p = get_first_selection(wno) ;
  enum menu_type m      = FM_FILE_MENU ;
  int fd;
  char 			path[256];

/* Only highlight those options which apply. */

  if (f_p)
    {
      fselected = TRUE ;
      if (f_p->type == FT_DOC || f_p->type == FT_APP) print = TRUE ;
    }
  if (f->path_pane.selected) pselected = TRUE ;

  if (f->mtype == FM_FLOPPY || f->mtype == FM_DOS) {
	sprintf(path, "/vol/dev/aliases/floppy%d", f->devno);
	if ((fd = open(path, O_RDWR)) == -1) {
		if (errno != EACCES && errno != EROFS) {
			eraseable = TRUE;
		}
	}
	else {
		eraseable = TRUE;
		close(fd);
	}


        /*  CTE# 501463; BUG# 1206761
         *  Check to make sure we are dealing with floppy0
         */
        if (Fm->frawpath[f->devno] &&
          (strstr(Fm->frawpath[f->devno], "diskette0") != NULL)) {
            floppy0 = TRUE ;
        }
  }

  mitem_inactive(m, FILE_BUT_OPEN,    !(fselected || pselected)) ;
  mitem_inactive(m, FILE_BUT_OPEN_ED, !(fselected || pselected)) ;
  mitem_inactive(m, FILE_BUT_PRINT1, !print) ;
  mitem_inactive(m, FILE_BUT_PRINT,  !print) ;
  mitem_inactive(m, FILE_BUT_CDOC,   !Fm->writable) ;
  mitem_inactive(m, FILE_BUT_CFOLD,  !Fm->writable) ;
  mitem_inactive(m, FILE_BUT_DUP,    !(Fm->writable && fselected)) ;
  mitem_inactive(m, FILE_BUT_FILEI,  !(fselected || pselected)) ;
  mitem_inactive(m, FILE_BUT_FORMATD, !(eraseable && floppy0)) ;
  mitem_inactive(m, FILE_BUT_RENAMED, !(eraseable && floppy0)) ;
}


void
set_fp_color(f_p, ftype, ctype)
File_Pane_Object *f_p ;
enum fm_fp_type ftype ;
enum fm_color_type ctype ;
{
  unsigned long color ;

  switch (ctype)
    {
      case FM_FGCOLOR      : color = get_fg_index(f_p) ;
                             break ;
      case FM_BGCOLOR      : color = get_bg_index(f_p) ;
                             break ;
      case FM_FG_DEF_COLOR : color = get_default_fg_index(f_p->type) ;
                             break ;
      case FM_BG_DEF_COLOR : color = get_default_bg_index(f_p->type) ;
    }

  switch (ftype)
    {
      case FM_FP_FGCOLOR : f_p->icon_fg_index = color ;
                           break ;
      case FM_FP_BGCOLOR : f_p->icon_bg_index = color ;
    }
}


void
set_icon_pos(wno, f_p)
int wno ;
File_Pane_Object *f_p ;
{
  File_Pane_Pos *p ;
  File_Pane *f         = Fm->file[wno] ;
  int canvas_width     = get_pos_attr(wno, FM_POS_CWIDTH) ;
  int canvas_height    = get_pos_attr(wno, FM_POS_CHEIGHT) ;
  int incx             = get_pos_attr(wno, FM_POS_INCX) ;
  int incy             = get_pos_attr(wno, FM_POS_INCY) ;
  int max_filespercol  = get_pos_attr(wno, FM_POS_MAXCOL) ;
  int max_filesperline = get_pos_attr(wno, FM_POS_MAXLINE) ;
  int startx           = get_pos_attr(wno, FM_POS_STARTX) ;
  int starty           = get_pos_attr(wno, FM_POS_STARTY) ;
  int col, row ;

       if (f->display_mode == VIEW_BY_CONTENT) p = &f->c ;
  else if (f->display_mode == VIEW_BY_ICON)    p = &f->i ;
  else if (f->display_mode == VIEW_BY_LIST)    p = &f->l ;

  if (p->pos == FALSE) return ;
  if (!max_filespercol || !max_filesperline) return ;

  if (p->grid[(max_filespercol * max_filesperline)-1])
    {
      if (f->dispdir == FM_DISP_BY_ROWS)
        {
          FM_SET_CANVAS_HEIGHT(canvas_height + incy, wno, incy) ;
          set_file_obj_x(wno, f_p, startx) ;
          set_file_obj_y(wno, f_p, (max_filespercol * incy) + starty) ;
        }
      else
        {
          FM_SET_CANVAS_WIDTH(canvas_width + incx, wno, incx) ;
          set_file_obj_x(wno, f_p, (max_filesperline * incx) + startx) ;
          set_file_obj_y(wno, f_p, starty) ;
        }
    }
  else
    {
      if (f->dispdir == FM_DISP_BY_ROWS)
        {
          for (row = max_filespercol - 1; row >= 0; row--)
            for (col = max_filesperline - 1; col >= 0; col--)
              if (p->grid[(row * max_filesperline) + col])
                {
                  col++ ;
                  if (col == max_filesperline)
                    {
                      row++ ;
                      col = 0 ;
                    }
                  set_file_obj_x(wno, f_p, (col * incx) + startx) ;
                  set_file_obj_y(wno, f_p, (row * incy) + starty) ;
                  return ;
                }
        }
      else
        {
          for (col = max_filesperline - 1; col >= 0; col--)
            for (row = max_filespercol - 1; row >= 0; row--)
              if (p->grid[(col * max_filespercol) + row])
                {
                  row++ ;
                  if (row == max_filespercol)
                    {
                      col++ ;
                      row = 0 ;
                    }
                  set_file_obj_x(wno, f_p, (col * incx) + startx) ;
                  set_file_obj_y(wno, f_p, (row * incy) + starty) ;
                  return ;
                }
        }
    }
}


void
set_layout_item(is_newfolder)
Boolean is_newfolder ;
{
  enum item_type layout, listopts ;
  int opt ;

  layout   = (is_newfolder) ? FM_PO_NEWF_LAYOUT   : FM_PO_CURF_LAYOUT ;
  listopts = (is_newfolder) ? FM_PO_NEWF_LISTOPTS : FM_PO_CURF_LISTOPTS ;
  opt      = get_item_int_attr(listopts, FM_ITEM_IVALUE) ;
  set_item_int_attr(layout, FM_ITEM_INACTIVE, opt) ;
}


void
set_list_item(is_newfolder)
Boolean is_newfolder ;
{
  enum item_type content, display, layout, listopts, sortby ;
  int opt ;

  content  = (is_newfolder) ? FM_PO_NEWF_CONTENT  : FM_PO_CURF_CONTENT ;
  display  = (is_newfolder) ? FM_PO_NEWF_DISPLAY  : FM_PO_CURF_DISPLAY ;
  layout   = (is_newfolder) ? FM_PO_NEWF_LAYOUT   : FM_PO_CURF_LAYOUT ;
  listopts = (is_newfolder) ? FM_PO_NEWF_LISTOPTS : FM_PO_CURF_LISTOPTS ;
  sortby   = (is_newfolder) ? FM_PO_NEWF_SORTBY   : FM_PO_CURF_SORTBY ;
  opt      = get_item_int_attr(display, FM_ITEM_IVALUE) ;

  if (opt == VIEW_BY_CONTENT)
    {
      set_item_int_attr(content,  FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(sortby,   FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(listopts, FM_ITEM_INACTIVE, TRUE) ;
      set_item_int_attr(layout,   FM_ITEM_INACTIVE, FALSE) ;
    }
  else if (opt == VIEW_BY_LIST)
    {
      set_item_int_attr(content,  FM_ITEM_INACTIVE, TRUE) ;
      set_item_int_attr(sortby,   FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(listopts, FM_ITEM_INACTIVE, FALSE) ;
      set_layout_item(is_newfolder) ;
    }
  else
    {
      set_item_int_attr(content,  FM_ITEM_INACTIVE, TRUE) ;
      set_item_int_attr(sortby,   FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(listopts, FM_ITEM_INACTIVE, TRUE) ;
      set_item_int_attr(layout,   FM_ITEM_INACTIVE, FALSE) ;
    }
}


void
set_pos_attr(wno, ptype, value)
int wno, value ;
enum pos_type ptype ;
{
  File_Pane_Pos *p ;
  File_Pane *f = Fm->file[wno] ;

       if (f->display_mode == VIEW_BY_CONTENT) p = &f->c ;
  else if (f->display_mode == VIEW_BY_ICON)    p = &f->i ;
  else if (f->listopts)                        p = &f->o ;
  else                                         p = &f->l ;

  switch (ptype)
    {
      case FM_POS_CHEIGHT : p->canvas_height    = value ; break ;
      case FM_POS_CWIDTH  : p->canvas_width     = value ; break ;
      case FM_POS_INCX    : p->incx             = value ; break ;
      case FM_POS_INCY    : p->incy             = value ; break ;
      case FM_POS_MAXCOL  : p->max_filespercol  = value ; break ;
      case FM_POS_MAXLINE : p->max_filesperline = value ; break ;
      case FM_POS_STARTX  : p->startx           = value ; break ;
      case FM_POS_STARTY  : p->starty           = value ; break ;
      case FM_POS_POS     : p->pos              = value ; break ;
      case FM_POS_MOVED   : p->moved            = value ; break ;
      case FM_POS_HSBAR   : p->hsbar_pos        = value ; break ;
      case FM_POS_VSBAR   : p->vsbar_pos        = value ; break ;
    }
}


void
set_root()           /* Set tree root to current folder. */
{
  if (Fm->tree.selected != Fm->tree.head)
    {
      fm_scrollbar_scroll_to(WNO_TREE, FM_H_SBAR, 0) ;
      fm_scrollbar_scroll_to(WNO_TREE, FM_V_SBAR, 0) ;
      Fm->tree.sibling = Fm->tree.selected->sibling ;
      Fm->tree.selected->sibling = NULL ;
      add_parent(Fm->tree.selected) ;
      Fm->tree.lastcur = NULL ;
      fm_drawtree(TRUE) ;
      fm_visiblefolder(Fm->tree.selected) ;
    }
}


void
set_tree_file_button_menu()
{
  enum menu_type m = FM_TFILE_MENU ;

/* Only highlight those options which apply. */

  mitem_inactive(m, TFILE_BUT_OPEN,   !Fm->tree.selected) ;
  mitem_inactive(m, TFILE_BUT_OPENF,  !Fm->tree.selected) ;
  mitem_inactive(m, TFILE_BUT_FILEI,  !Fm->tree.selected) ;
}


void
set_tree_view_button_menu()
{
  char *str ;
  Boolean hide     = FALSE ;            /* Hide folders inactive? */
  enum menu_type m = FM_TVIEW_MENU ;

  hide = (Boolean) Fm->treeview && Fm->tree.selected &&
		   Fm->tree.selected->child ;

  if (Fm->tree_vertical) str = Str[(int) M_TREE_HORZ] ;
  else                   str = Str[(int) M_TREE_VERT] ;
  mitem_label(m, TVIEW_DIR, (WCHAR *) str) ;

  mitem_inactive(m, TVIEW_SHOW,  !Fm->treeview || !Fm->tree.selected) ;
  mitem_inactive(m, TVIEW_HIDE,  !hide) ;
  mitem_inactive(m, TVIEW_START, !Fm->treeview || !Fm->tree.selected) ;
  mitem_inactive(m, TVIEW_ADD,   Fm->tree.root == Fm->tree.head) ;
}


void
set_view_button_menu()
{
  int wno               = Fm->curr_wno ;
  enum menu_type      m = FM_VIEW_MENU ;
  File_Pane *f          = Fm->file[wno] ;
  File_Pane_Object *f_p = get_first_selection(wno) ;
  WCHAR *str ;

  if (f_p) str = (WCHAR *) Str[(int) M_CLEANUP_SEL] ;
  else     str = (WCHAR *) Str[(int) M_CLEANUP_ICONS] ;
  mitem_label(m, VIEW_CLEANUP, str) ;
  mitem_inactive(m, VIEW_CLEANUP, f->num_objects == 0 ||
                 (f->display_mode == VIEW_BY_LIST && f->listopts)) ;
}


void
set_waste_edit_menu()
{
  Boolean empty    = TRUE ;                       /* Empty folder? */
  Boolean selected = FALSE ;                      /* Anything selected? */
  enum menu_type m = FM_WEDIT_MENU ;

/* Only highlight those options which apply. */

  empty    = (Boolean) (Fm->file[WASTE]->num_objects == 0) ;
  selected = (Boolean) (get_first_selection(WASTE) != NULL) ;

  mitem_inactive(m, WASTE_EDIT_SEL_ALL,  empty) ;
  mitem_inactive(m, WASTE_EDIT_CUT,      !selected || !Fm->writable) ;
  mitem_inactive(m, WASTE_EDIT_COPY,     !selected) ;
  mitem_inactive(m, WASTE_EDIT_PASTE,    !shelf_held() || !Fm->writable) ;
  mitem_inactive(m, WASTE_EDIT_UNDELETE, !selected) ;
  mitem_inactive(m, WASTE_EDIT_DESTROY,  !selected) ;
}


void
set_waste_pane_menu()
{
  Boolean empty    = TRUE ;                       /* Empty folder? */
  Boolean selected = FALSE ;                      /* Anything selected? */
  enum menu_type m = FM_WPANE_MENU ;

/* Only highlight those options which apply. */

  empty    = (Boolean) (Fm->file[WASTE]->num_objects == 0) ;
  selected = (Boolean) (get_first_selection(WASTE) != NULL) ;

  mitem_inactive(m, WASTE_PANE_EMPTY,    empty) ;
  mitem_inactive(m, WASTE_PANE_SEL_ALL,  empty) ;
  mitem_inactive(m, WASTE_PANE_CUT,      !selected || !Fm->writable) ;
  mitem_inactive(m, WASTE_PANE_COPY,     !selected) ;
  mitem_inactive(m, WASTE_PANE_PASTE,    !shelf_held() || !Fm->writable) ;
  mitem_inactive(m, WASTE_PANE_UNDELETE, !selected) ;
  mitem_inactive(m, WASTE_PANE_DESTROY,  !selected) ;
}


void
set_window_params(wno)
int wno ;
{
  Fm->file[wno]->num_file_chars = Fm->num_file_chars ;
  Fm->file[wno]->dispdir        = Fm->dispdir ;

  Fm->file[wno]->display_mode   = Fm->display_mode ;
  Fm->file[wno]->sortby         = Fm->sortby ;
  Fm->file[wno]->see_dot        = Fm->see_dot ;
  Fm->file[wno]->listopts       = Fm->listopts ;
  Fm->file[wno]->content_types  = Fm->content_types ;
}


void
write_dir_defs(wno)
int wno ;
{
  File_Pane *f = Fm->file[wno] ;
  char intval[16], *value ;
  int info[MAXINFO] ;

       if (f->display_mode == VIEW_BY_ICON)    value = "icon" ;
  else if (f->display_mode == VIEW_BY_LIST)    value = "list" ;
  else if (f->display_mode == VIEW_BY_CONTENT) value = "content" ;
  fm_put_resource(FM_DIR_DB, R_DISPMODE, value) ;
       if (f->sortby == SORT_BY_NAME) value = "name" ;
  else if (f->sortby == SORT_BY_TYPE) value = "type" ;
  else if (f->sortby == SORT_BY_SIZE) value = "size" ;
  else if (f->sortby == SORT_BY_TIME) value = "date" ;
  fm_put_resource(FM_DIR_DB, R_SORTBY, value) ;
  fm_put_resource(FM_DIR_DB, R_SEEDOT, set_bool(f->see_dot == TRUE)) ;
  SPRINTF(intval, "%d", f->listopts) ;
  fm_put_resource(FM_DIR_DB, R_STYLE, intval) ;
  fm_put_resource(FM_DIR_DB, R_DISPDIR,
                  set_bool(f->dispdir == FM_DISP_BY_COLS)) ;
  SPRINTF(intval, "%d", f->num_file_chars) ;
  fm_put_resource(FM_DIR_DB, R_NOCHARS, intval) ;
  SPRINTF(intval, "%d", f->widest_name) ;
  fm_put_resource(FM_DIR_DB, R_NAMEWID, intval) ;

  get_win_X_info(wno, info) ;
  SPRINTF(intval, "%d", info[1]) ;
  fm_put_resource(FM_DIR_DB, R_ICONX, intval) ;
  SPRINTF(intval, "%d", info[2]) ;
  fm_put_resource(FM_DIR_DB, R_ICONY, intval) ;
  SPRINTF(intval, "%d", info[3]) ;
  fm_put_resource(FM_DIR_DB, R_WINX,  intval) ;
  SPRINTF(intval, "%d", info[4]) ;
  fm_put_resource(FM_DIR_DB, R_WINY,  intval) ;
  SPRINTF(intval, "%d", info[5]) ;
  fm_put_resource(FM_DIR_DB, R_WINW,  intval) ;
  SPRINTF(intval, "%d", info[6]) ;
  fm_put_resource(FM_DIR_DB, R_WINH,  intval) ;

  write_positional_info(wno, "content") ;
  write_positional_info(wno, "icon") ;
  write_positional_info(wno, "list") ;
}


void
write_icon_positions(wno)    /* Save X/Y values for this window. */
int wno ;
{
  FILE *fp ;
  File_Pane_Object **f_p, **l_p ;
  char fname[MAXPATHLEN] ;

  SPRINTF(fname, "%s/.fm%s/.icons", Fm->home, Fm->file[wno]->path) ;
  if ((fp = fopen(fname, "w")) != NULL)
    {
      l_p = PTR_LAST(wno) ;
      for (f_p = PTR_FIRST(wno); f_p < l_p; f_p++)
        FPRINTF(fp, "%d %d %d %d %s\n", (*f_p)->ix, (*f_p)->iy,
                (*f_p)->lx, (*f_p)->ly, (*f_p)->name) ;
    }
  FCLOSE(fp) ;
}


void
write_menu_defaults()
{
  write_menu_def_val(FM_FILE_MENU,        R_FILE_MENU) ;
  write_menu_def_val(FM_VIEW_MENU,        R_VIEW_MENU) ;
  write_menu_def_val(FM_EDIT_MENU,        R_EDIT_MENU) ;
  write_menu_def_val(FM_GOTO_MENU,        R_GOTO_MENU) ;
  write_menu_def_val(FM_PP_MENU,          R_PP_MENU) ;
  write_menu_def_val(FM_FP_MENU,          R_FP_MENU) ;
  write_menu_def_val(FM_CC_MENU,          R_CC_MENU) ;

  if (is_frame(WNO_TREE))
    {
      write_menu_def_val(FM_TFILE_MENU,   R_TFILE_MENU) ;
      write_menu_def_val(FM_TVIEW_MENU,   R_TVIEW_MENU) ;
      write_menu_def_val(FM_TP_MENU,      R_TP_MENU) ;
    }

  if (is_frame(WASTE))
    {
      write_menu_def_val(FM_WEDIT_MENU,   R_WEDIT_MENU) ;
      write_menu_def_val(FM_WPANE_MENU,   R_WPANE_MENU) ;
    }
}


void
write_menu_def_val(mtype, rtype)
enum menu_type mtype ;
enum res_type rtype ;
{
  char intval[16] ;
  int value ;

  value = get_menu_default(mtype) ;
  if (value != -1)
    {
      SPRINTF(intval, "%d", value) ;
      fm_put_resource(FM_DESKSET_DB, rtype, intval) ;
    }
}


void
write_positional_info(wno, str)
int wno ;
char *str ;
{
  char intval[16] ;
  File_Pane_Pos p ;

       if (EQUAL(str, "content")) p = Fm->file[wno]->c ;
  else if (EQUAL(str, "icon"))    p = Fm->file[wno]->i ;
  else if (EQUAL(str, "list"))    p = Fm->file[wno]->l ;

  SPRINTF(intval, "%d", p.canvas_width) ;
  put_pos_resource(str, R_CWIDTH, intval) ;
  SPRINTF(intval, "%d", p.canvas_height) ;
  put_pos_resource(str, R_CHEIGHT, intval) ;
  SPRINTF(intval, "%d", p.incx) ;
  put_pos_resource(str, R_INCX, intval) ;
  SPRINTF(intval, "%d", p.incy) ;
  put_pos_resource(str, R_INCY, intval) ;
  SPRINTF(intval, "%d", p.max_filespercol) ;
  put_pos_resource(str, R_MAXCOL, intval) ;
  SPRINTF(intval, "%d", p.max_filesperline) ;
  put_pos_resource(str, R_MAXLINE, intval) ;
  SPRINTF(intval, "%d", p.startx) ;
  put_pos_resource(str, R_STARTX, intval) ;
  SPRINTF(intval, "%d", p.starty) ;
  put_pos_resource(str, R_STARTY, intval) ;
  SPRINTF(intval, "%d", p.moved) ;
  put_pos_resource(str, R_MOVED, intval) ;

  SPRINTF(intval, "%d", get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_VIEW_ST)) ;
  put_pos_resource(str, R_HSBAR, intval) ;
  SPRINTF(intval, "%d", get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_ST)) ;
  put_pos_resource(str, R_VSBAR, intval) ;
}


void
write_resources()     /* Write various X resources to ~/.desksetdefaults. */
{
  char intval[16], *value ;

/* Note: resource properties should not be localized. */

  load_deskset_defs() ;

/* Save custom view properties. */

       if (Fm->display_mode == VIEW_BY_ICON)    value = "icon" ;
  else if (Fm->display_mode == VIEW_BY_LIST)    value = "list" ;
  else if (Fm->display_mode == VIEW_BY_CONTENT) value = "content" ;
  fm_put_resource(FM_DESKSET_DB, R_DISPMODE, value) ;
       if (Fm->sortby == SORT_BY_NAME) value = "name" ;
  else if (Fm->sortby == SORT_BY_TYPE) value = "type" ;
  else if (Fm->sortby == SORT_BY_SIZE) value = "size" ;
  fm_put_resource(FM_DESKSET_DB, R_SORTBY, value) ;
  fm_put_resource(FM_DESKSET_DB, R_SEEDOT, set_bool(Fm->see_dot == TRUE)) ;
  SPRINTF(intval, "%d", Fm->listopts) ;
  fm_put_resource(FM_DESKSET_DB, R_STYLE, intval) ;
  fm_put_resource(FM_DESKSET_DB, R_DISPDIR,
                  set_bool(Fm->dispdir == FM_DISP_BY_COLS)) ;

  SPRINTF(intval, "%d", Fm->maxgoto) ;
  fm_put_resource(FM_DESKSET_DB, R_MAXGOTO, intval) ;

/* Save tool properties. */

  if (Fm->print_script != NULL)
    fm_put_resource(FM_DESKSET_DB, R_PRINT, (char *) Fm->print_script) ;
  if (Fm->filter != NULL)
    fm_put_resource(FM_DESKSET_DB, R_FILTER, (char *) Fm->filter) ;

  if (Fm->newdoc != NULL)
    fm_put_resource(FM_DESKSET_DB, R_NEWDOC, (char *) Fm->newdoc) ;
  if (Fm->newdir != NULL)
    fm_put_resource(FM_DESKSET_DB, R_NEWDIR, (char *) Fm->newdir) ;

  SPRINTF(intval, "%d", Fm->no_ccmds) ;
  fm_put_resource(FM_DESKSET_DB, R_NOCCMDS, intval) ;
  SPRINTF(intval, "%d", Fm->tree_gap) ;
  fm_put_resource(FM_DESKSET_DB, R_TREEGAP, intval) ;
  SPRINTF(intval, "%d", Fm->num_file_chars) ;
  fm_put_resource(FM_DESKSET_DB, R_NOCHARS, intval) ;
  SPRINTF(intval, "%d", Fm->interval) ;
  fm_put_resource(FM_DESKSET_DB, R_INTERVAL, intval) ;
  SPRINTF(intval, "%d", Fm->ce_bufsize) ;
  fm_put_resource(FM_DESKSET_DB, R_CEBUFN, intval) ;
  SPRINTF(intval, "%d", Fm->gclimit) ;
  fm_put_resource(FM_DESKSET_DB, R_GCSIZE, intval) ;

  fm_put_resource(FM_DESKSET_DB, R_DELETE,
                  set_bool(Fm->confirm_delete == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_DELFDR,
                  set_bool(Fm->delete_fdr_prompt == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_DESFDR,
                  set_bool(Fm->destroy_fdr_prompt == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_QUIT,
                  set_bool(Fm->show_quit_prompt == TRUE)) ;

  fm_put_resource(FM_DESKSET_DB, R_EDITOR, set_bool(Fm->editor == FALSE)) ;
  if (Fm->other_editor != NULL)
    {
      if (Fm->other_editor[0] == '\0') set_default(DEF_EDITOR) ;
      fm_put_resource(FM_DESKSET_DB, R_OTHERED, (char *) Fm->other_editor) ;
    }

/* Save misc. properties. */

  fm_put_resource(FM_DESKSET_DB, R_OLDCCMDS, set_bool(Fm->add_old == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_DIRDC,    set_bool(Fm->newwin == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_CENGINE,  set_bool(Fm->use_ce == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_CACHE,    set_bool(Fm->use_cache == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_TREEVIEW, set_bool(Fm->treeview == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_AUTOSORT, set_bool(Fm->autosort == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_SYMLINKS,
                  set_bool(Fm->follow_links == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_DOCD,     set_bool(Fm->show_cd == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_FL_CON,
                  set_bool(Fm->floppy_content == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_CD_CON, set_bool(Fm->cd_content == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_SORTCASE, set_bool(Fm->casesen == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_LSTATE, set_bool(Fm->load_state == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_LICONS, set_bool(Fm->load_icons == TRUE)) ;
  fm_put_resource(FM_DESKSET_DB, R_TREEDIR,
                  set_bool(Fm->tree_vertical == TRUE)) ;

  if (Fm->acolor_res != NULL)
    fm_put_resource(FM_DESKSET_DB, R_ACOLOR, Fm->acolor_res) ;
  if (Fm->dcolor_res != NULL)
    fm_put_resource(FM_DESKSET_DB, R_DCOLOR, Fm->dcolor_res) ;
  if (Fm->fcolor_res != NULL)
    fm_put_resource(FM_DESKSET_DB, R_FCOLOR, Fm->fcolor_res) ;

  if (Fm->shellname != NULL)
    fm_put_resource(FM_DESKSET_DB, R_SHTYPE, Fm->shellname) ;

  write_menu_defaults() ;
  save_window_info(WASTE) ;
  save_window_info(WNO_TREE) ;
  save_resources() ;
}


void
write_user_goto_menu()    /* Write goto menu entries to ~/.desksetdefaults. */
{
  char intval[16] ;
  int i ;

  load_deskset_defs() ;
  SPRINTF(intval, "%d", Fm->no_user_goto) ;
  fm_put_resource(FM_DESKSET_DB, R_NOUGOTO, intval) ;
  for (i = 0; i < Fm->no_user_goto; i++)
    {
      set_goto_resource(FM_GOTO_LABEL, i, Fm->user_goto_label[i]) ;
      set_goto_resource(FM_GOTO_PATH,  i, Fm->user_goto[i]) ;
    }
  save_resources() ;
}
