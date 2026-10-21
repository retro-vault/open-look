#ifndef lint
static char sccsid[]="@(#)display.c	1.111 06/11/96 Copyright 1987-1992 Sun Microsystem, Inc." ;
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
#include <errno.h>
#include <string.h>
#include "defs.h"
#include "fm.h"

#ifdef SVR4
/* Linux portability definitions are in linux_port.h. */
#endif /*SVR4*/

/* Tree and file records share a name field but require distinct comparators. */
static int compare_folder_names(const void *a, const void *b)
{
    const Tree_Pane_Object *const *left = a, *const *right = b;
    return strcmp((char *)(*left)->name, (char *)(*right)->name);
}
static int compare_tree_order(const void *a, const void *b)
{
    return compare_tree_node((Tree_Pane_Object **)a, (Tree_Pane_Object **)b);
}
extern FmVars Fm ;
extern char *Str[] ;

/* Variable and fixed widths for List Options. */

static short Permission_width[2] = { 8, 11 } ;
static short Link_width[2]       = { 3,  5 } ;
static short Owner_width[2]      = { 5,  9 } ;
static short Group_width[2]      = { 5,  9 } ;
static short Size_width[2]       = { 5,  9 } ;
static short Date_width[2]       = { 9, 14 } ;


void
addparent_button()             /* Add tree root's parent. */
{

/* Restore current head. */

  if (Fm->tree.sibling && Fm->tree.sibling->parent == Fm->tree.head->parent)
    Fm->tree.head->sibling = Fm->tree.sibling ;
  add_parent(Fm->tree.head->parent) ;
}


void
add_parent(f_p)               /* Add parent to folder. */
Tree_Pane_Object *f_p ;
{
  Fm->tree.head = f_p ;
  fm_drawtree(TRUE) ;
  if (Fm->tree.selected)
    fm_visiblefolder(Fm->tree.selected) ;    /* Track selected. */
}


void
add_tree_entry(node, name)   /* Add a node entry beneath this node. */
Tree_Pane_Object *node ;
WCHAR *name ;
{
  Tree_Pane_Object *f_p ;
  Tree_Pane_Object *l_p = NULL ;   /* Pointer to previous tree node entry. */
  Tree_Pane_Object *nf_p ;         /* Pointer to new folder tree entry. */
  Boolean end = TRUE ;      /* Set false if entry is in middle of list. */

  f_p = node->child ;
  while (f_p != NULL)
    {
      if (!strcmp((char *) f_p->name, (char *) name)) return ;
      if (strcmp((char *) f_p->name, (char *) name) > 0)
        {
          end = FALSE ;
          break ;
        }
      l_p = f_p ;
      f_p = f_p->sibling ;
    }

  if ((nf_p = make_tree_node(name)) == NULL) return ;

  nf_p->parent = (f_p == NULL) ? node : f_p->parent ;
  nf_p->child = NULL ;
  STRCPY((char *) nf_p->name, (char *) name) ;
  nf_p->mtime = 0 ;
  nf_p->status = TEXPLORE ;

/*  Three possible situations could have occured.
 *
 *  (1) The new entry goes at the start of the sibling chain:
 *      (a) New entries sibling pointer will point to the current value of
 *          the parents child pointer.
 *      (b) The parents child pointer will then point to the new entry.
 *
 *  (2) The new entry goes in the middle of the sibling chain:
 *      (a) New entries sibling pointer gets set to the current value of
 *          the previous entries sibling pointer.
 *      (b) The previous entries sibling pointer will point to this new entry.
 *
 *  (3) The new entry goes at the end of the chain:
 *      (a) Previous entries sibling pointer gets points to the new entry.
 *      (b) The new entries sibling pointer points to NULL.
 */

  if (l_p == NULL)                                     /* Start of chain. */
    {
      nf_p->sibling = nf_p->parent->child ;
      nf_p->parent->child = nf_p ;
    }
  else if (end == FALSE)                               /* Middle of chain. */
    {
      nf_p->sibling = l_p->sibling ;
      l_p->sibling = nf_p ;
    }
  else                                                 /* End of chain. */
    {
      l_p->sibling = nf_p ;
      nf_p->sibling = NULL ;
    }
  Fm->redraw_tree = TRUE ;
}


int
alloc_more_files(wno)
register int wno ;
{
  File_Pane *f = Fm->file[wno] ;
  int firsttime, new, old ;

/*  Allocate enough memory for file structures.
 *  Allocating memory dynamically instead of statically allows
 *  us to swap pointers instead of copying structures whilst
 *  sorting, etc.
 */

  old       = f->max_objects ;
  new       = f->max_objects + FILE_ALLOC_INC ;
  firsttime = (f->max_objects == 0) ? TRUE : FALSE ;
  f->object = (File_Pane_Object **) LINT_CAST(make_mem((char *) f->object,
                               sizeof(File_Pane_Object *) * old,
                               sizeof(File_Pane_Object *) * new, firsttime)) ;

/*  Allocate another chunk of memory for these file structures.
 *  (We never free this memory).
 */

  f->max_objects += FILE_ALLOC_INC ;
  for (; old < f->max_objects; old++)
    {
      if ((f->object[old] = (File_Pane_Object *)
         LINT_CAST(fm_malloc((unsigned) (sizeof(File_Pane_Object))))) == NULL)
        return(FALSE) ;
      f->object[old]->name = NULL ;
      f->object[old]->image_depth = 1 ;
      f->object[old]->cx = -1 ;            /* Signifies no position yet. */
      f->object[old]->cy = -1 ;
      f->object[old]->ix = -1 ;
      f->object[old]->iy = -1 ;
      f->object[old]->lx = -1 ;
      f->object[old]->ly = -1 ;
      f->object[old]->ox = -1 ;
      f->object[old]->oy = -1 ;
    }
  return(TRUE) ;
}


void
calc_tree(f_p)
Tree_Pane_Object *f_p ;            /* Current node. */
{
  Fm->ntreelines = 0 ;
  tree_layout(f_p) ;
  Fm->tree_width = Fm->tree_height = 0 ;
  Fm->tree_Ymin  = Fm->tree_Ymax   = 0 ;
  f_p->Xpos = (Fm->tree_vertical) ? 0 : MARGIN ;
  f_p->Ypos = (Fm->tree_vertical) ? MARGIN : 0 ;

  tree_calculate_dims(f_p) ;
  if (Fm->tree_vertical) Fm->tree_width  = Fm->tree_Ymax - Fm->tree_Ymin ;
  else                   Fm->tree_height = Fm->tree_Ymax - Fm->tree_Ymin ;
#ifdef TREE_SPEEDUP
  sort_tree(f_p) ;
#endif /*TREE_SPEEDUP*/
}


void
check_del_dir(path)
char *path ;
{
  char newpath[MAXPATHLEN], *ptr ;
  int len, wno ;

  for (wno = 0; wno < Fm->maxwin; wno++)
    if (is_frame(wno) && EQUAL(path, (char *) Fm->file[wno]->path))
      {
        STRCPY(newpath, path) ;
        if ((ptr = strrchr(newpath, '/')) != newpath) *ptr = '\0' ;
        else
          {
            len = strlen(newpath) ;
            if (len > 1) *(ptr+1) = '\0' ;
          }
        fm_openfile(newpath, (char *) 0, TRUE, wno) ;
        set_timer(Fm->interval) ;
      }
}


/*  Check to see if over an item and its filename.  Sets the
 *  appropiate fields of the Event Context.
 *
 *  XXX: fm_over_item() should be rewritten to do this this, rather than
 *       call it twice.
 */

Event_Context
check_if_over_item(ec)
Event_Context ec ;
{
  ec->over_item = ec->over_name = FALSE ;
  ec->item = fm_over_item(ec->x, ec->y, OVER_NAME, ec->wno) ;
  if (ec->item != FM_EMPTY)
    {
      ec->over_item = ec->over_name = TRUE ;
    }
  else
    {
      ec->item = fm_over_item(ec->x, ec->y, !OVER_NAME, ec->wno) ;
      if (ec->item != FM_EMPTY) ec->over_item = TRUE ;
    }
  return(ec) ;
}


void
compare_child(ff_p, fc_p, new)
Tree_Pane_Object *ff_p, *fc_p ;
Boolean *new ;
{
  register Tree_Pane_Object *f_p, *c_p, *t_p ;
  long delete = 1 ;            /* Unlikely time (delete marker). */

  c_p = fc_p ;
  while (c_p)
    {

/* Test current children against previous. */

      for (f_p = ff_p; f_p; f_p = f_p->sibling)
        if (strcmp((char *) c_p->name, (char *) f_p->name) == 0)
          break ;

/* Exists, mark for delete. */

      if (f_p) c_p->mtime = delete ;
      c_p = c_p->sibling ;
    }

  f_p = ff_p ;
  while (f_p)
    {

/* Test previous against current. */

      for (c_p = fc_p; c_p; c_p = c_p->sibling)
        if (strcmp((char *) c_p->name, (char *) f_p->name) == 0)
          break ;

/* No longer exists, mark for delete. */

      if (!c_p) f_p->mtime = delete ;
      f_p = f_p->sibling ;
    }

/* Remove deleted/moved folders. */

  for (f_p = ff_p, c_p = NULL; f_p;)
    if (f_p->mtime == delete)
      {
        if (c_p) c_p->sibling = f_p->sibling ;
        else     f_p->parent->child = f_p->sibling ;
        t_p = f_p->sibling ;
        f_p->sibling = NULL ;
        fm_destroy_tree(f_p) ;
        *new = TRUE ;
        f_p = t_p ;
      }
    else
      {
        c_p = f_p ;
        f_p = f_p->sibling ;
      }
    f_p = c_p ;

/* Add new folders. */

  for (c_p = fc_p; c_p; c_p = t_p)
    if (c_p->mtime != delete)
      {

/* Tack it on the end. */

        if (f_p) f_p->sibling = c_p ;
        else     ff_p->child = c_p ;
        f_p = c_p ;
        *new = TRUE ;
        t_p = c_p->sibling ;
      }
    else
      {
        t_p = c_p->sibling ;

/* Delete it. */

        FREE(c_p->name) ;
        FREE((char *) c_p) ;
      }

  if (f_p) f_p->sibling = NULL ;
}


/* Return -1, 0, 1 if less than, equal to, or greater than. */

int
compare_dirpos(f1, f2)
File_Pane_Object **f1, **f2 ;
{
       if ((*f1)->dirpos < (*f2)->dirpos) return(-1) ;
  else if ((*f1)->dirpos > (*f2)->dirpos) return(1) ;
  else                                    return(0) ;
}


int
compare_name(f1, f2)
File_Pane_Object **f1, **f2 ;
{
  char *c, ch1, ch2, s1[MAXPATHLEN], s2[MAXPATHLEN] ;
  int reply ;

/*  If this is a single byte locale, initially just compare the first character in
 *  each string for performance.  This should deal with most cases.
 */

  if (!Fm->multi_byte)
    {
      ch1 = (*f1)->name[0] ;
      ch2 = (*f2)->name[0] ;
      if (Fm->casesen == FALSE)
        {
          if (isupper(ch1)) ch1 = tolower(ch1) ;
          if (isupper(ch2)) ch2 = tolower(ch2) ;
        }
           if (ch1 < ch2) return(-1) ;
      else if (ch1 > ch2) return(1) ;
    }

  STRCPY(s1, (char *) (*f1)->name) ;
  STRCPY(s2, (char *) (*f2)->name) ;
  if (Fm->casesen == FALSE)
    {
      for (c = s1; *c != '\0'; c++)
        if (isupper(*c)) *c = tolower((unsigned char ) *c) ;
      for (c = s2; *c != '\0'; c++)
        if (isupper(*c)) *c = tolower((unsigned char ) *c) ;
    }
  reply = strcoll(s1, s2) ;
  if (reply == 0) reply = strcoll((char *) (*f1)->name,
                                  (char *) (*f2)->name) ;
  return(reply) ;
}


int
compare_time(f1, f2)
register File_Pane_Object **f1, **f2 ;
{
       if ((*f1)->mtime > (*f2)->mtime) return(-1) ;
  else if ((*f1)->mtime < (*f2)->mtime) return(1) ;
  else                                  return(0) ;
}


int
compare_size(f1, f2)
register File_Pane_Object **f1, **f2 ;
{
       if ((*f1)->size > (*f2)->size) return(-1) ;
  else if ((*f1)->size < (*f2)->size) return(1) ;
  else                                return(0) ;
}


int
compare_tree_node(f1, f2)
Tree_Pane_Object **f1, **f2 ;
{
  if (Fm->tree_vertical)
    {
           if ((*f1)->Xpos < (*f2)->Xpos) return(-1) ;
      else if ((*f1)->Xpos > (*f2)->Xpos) return(1) ;
      else if ((*f1)->Ypos < (*f2)->Ypos) return(-1) ;
      else if ((*f1)->Ypos > (*f2)->Ypos) return(1) ;
      else                                return(0) ;
    }
  else
    {
           if ((*f1)->Ypos < (*f2)->Ypos) return(-1) ;
      else if ((*f1)->Ypos > (*f2)->Ypos) return(1) ;
      else if ((*f1)->Xpos < (*f2)->Xpos) return(-1) ;
      else if ((*f1)->Xpos > (*f2)->Xpos) return(1) ;
      else                                return(0) ;
    }
}


int
compare_type(f1, f2)      /* Document type within name. */
register File_Pane_Object **f1, **f2 ;
{
       if ((*f1)->type > (*f2)->type) return(1) ;
  else if ((*f1)->type < (*f2)->type) return(-1) ;

/* Check icon type. */

       if ((*f1)->tns_entry > (*f2)->tns_entry) return(1) ;
  else if ((*f1)->tns_entry < (*f2)->tns_entry) return(-1) ;

  return(compare_name(f1, f2)) ;
}


void
cpo_make()
{
  fm_popup_create(FM_CP_POPUP) ;
  link_cp_items() ;

  fm_add_help(FM_CP_CP_PANEL,     "filemgr:Custom_Print_Panel") ;
  fm_add_help(FM_CP_METHOD_ITEM,  "filemgr:Custom_Print_Print_Method") ;
  fm_add_help(FM_CP_COPIES_ITEM,  "filemgr:Custom_Print_Copies") ;
  fm_add_help(FM_CP_PRINT_BUTTON, "filemgr:Custom_Print_Print") ;
  set_default_item(FM_CP_CP_PANEL, FM_CP_PRINT_BUTTON) ;

  fm_position_popup(Fm->curr_wno, FM_CP_CPO_FRAME) ;
  set_item_int_attr(FM_CP_CPO_FRAME, FM_ITEM_SHOW, TRUE) ;
}


void
descendant(f_p)
register Tree_Pane_Object *f_p ;
{
  if (Fm->Found_child) return ;

  for (; f_p; f_p = f_p->sibling)
    if (f_p == Fm->Child)
      {
        Fm->Found_child = TRUE ;
        break ;
      }
    else if (f_p->child)
      descendant(f_p->child) ;
}


/* Redisplay all folders which are viewing a given path. */

void
display_all_folders(mode, path)
BYTE mode ;                       /* Build, style change, display? */
char *path ;
{
  int wno ;

  for (wno = 0; wno < Fm->num_win; wno++)
    {

/*  The wastebasket, diskette, etc. may not be around when this is
 *  called, so make sure path exists before checking.
 */

      if (is_frame(wno) &&
          Fm->file[wno]->path && EQUAL((char *) Fm->file[wno]->path, path))
        fm_display_folder(mode, wno) ;
    }
}


int
do_cpo_print_proc()
{
  char *method = get_item_str_attr(FM_CP_METHOD_ITEM, FM_ITEM_IVALUE) ;
  int  copies  = get_item_int_attr(FM_CP_COPIES_ITEM, FM_ITEM_IVALUE) ;

  set_item_int_attr(FM_CP_COPIES_ITEM, FM_ITEM_IVALUE, copies) ;
  if (!valid_print_cmd()) return(FAILURE) ;
  print_files(method, copies, Fm->curr_wno) ;
  return(FM_SUCCESS) ;
}


void
do_list_notify(string, selected)
char *string ;
int selected ;
{
  Fm->Found_chosen = strdup(string) ;
  fm_all_filedeselect() ;               /* Deselect all files. */
  fm_all_pathdeselect(WNO_TREE) ;
  fm_treedeselect() ;
  set_item_int_attr(FM_FIND_OPEN_BUTTON, FM_ITEM_INACTIVE, !selected) ;
}


/* Procedure to handle marquis selection (bounding box) in file pane. */

void
do_marquis_selection(ec)
Event_Context ec ;
{
  if (!Fm->marquis_started)
    {
      Fm->marquis_started++ ;
      set_timer(0) ;          /* Turn off interval timer. */
    }

/* Xor (erase) old line. */

  fm_draw_rect(ec->wno, ec->start_x, ec->start_y, ec->old_x, ec->old_y) ;

/* Draw new line. */

  fm_draw_rect(ec->wno, ec->start_x, ec->start_y, ec->x, ec->y) ;
}


void
do_proceed_button()
{
  char *source, *source_mc, *target, *target_mc ;
  char buffer[MAXPATHLEN*2+20], sname[MAXPATHLEN], tname[MAXPATHLEN] ;
  FILE *fp ;                       /* XXX: Remove when rcp bug fixed. */

  if ((source = get_item_str_attr(FM_RCP_SOURCE,      FM_ITEM_IVALUE)) &&
      (target = get_item_str_attr(FM_RCP_DESTINATION, FM_ITEM_IVALUE)))
    {
      source_mc = get_item_str_attr(FM_RCP_SOURCE_MC, FM_ITEM_IVALUE) ;
      if (EQUAL(source_mc, (char *) Fm->hostname) ||
          isempty((WCHAR *) source_mc))
        source_mc = NULL ;
      if (source_mc) SPRINTF(sname, "%s:%s", source_mc, source) ;
      else           STRCPY(sname, source) ;

      target_mc = get_item_str_attr(FM_RCP_DESTINATION_MC, FM_ITEM_IVALUE) ;
      if (EQUAL(target_mc, (char *) Fm->hostname) ||
          isempty((WCHAR *) target_mc))
        target_mc = NULL ;
      if (target_mc) SPRINTF(tname, "%s:%s", target_mc, target) ;
      else           STRCPY(tname, target) ;

      SPRINTF(buffer, "rcp %s %s 2>&1", sname, tname) ;
      if ((fp = popen(buffer, "r")) != NULL)
        {
          buffer[0] = 0 ;
          while (fgets(buffer, MAXPATHLEN, fp)) fm_msg(TRUE, buffer) ;
          PCLOSE(fp) ;
        }
    }
}


void
draw_ith(off, wno)
int off, wno ;
{
  File_Pane *f           = Fm->file[wno] ;
  File_Pane_Object **f_p = f->object + off ;
  int len, widest ;

  if (f->display_mode == VIEW_BY_LIST && f->listopts)
    {
      draw_list_ith(off, wno) ;
      return ;
    }

  draw_folder_icon(wno, f->display_mode, f_p, off) ;
  widest      = f->widest_name ;
  len         = f->num_file_chars * Fm->font_sizeW ;
  if (widest > len) widest = len ;
  draw_folder_text(wno, f->display_mode, f_p, widest) ;
}


void
draw_list_ith(off, wno)    /* Draw list (with options) entry. */
int off, wno ;
{
  File_Pane *f           = Fm->file[wno] ;
  File_Pane_Object **f_p = f->object + off ;
  int invert             = (*f_p)->selected ;
  int x                  = get_file_obj_x(wno, *f_p) ;
  int y                  = get_file_obj_y(wno, *f_p) ;
  char *b_p, buffer[256] ;
  time_t now, sixmonthsago, onehourfromnow ;

  if (LIST_SHOW(LS_DATE))
    {

/* Allow for formatting the date sensibly... */

      now            = time((time_t *) 0) ;
      sixmonthsago   = now - SIX_MONTHS_IN_SECS ;
      onehourfromnow = now + HOUR_IN_SECS ;
    }

  if ((*f_p)->nlink == 0 || (*f_p)->type == FT_UNCHECKED)
    my_stat((char *) f->path, f_p, wno, off) ;
  x = MARGIN ;
  y = (off + 1) * LIST_HEIGHT - 2 ;

  (*f_p)->ox = x ;
  (*f_p)->oy = y ;
  draw_list_icon(wno, x, y-15, *f_p) ;

  x += LIST_HEIGHT ;
  file_text(wno, x, y, (char *) (*f_p)->name, invert) ;
  x += f->widest_name + MARGIN ;

  if (LIST_SHOW(LS_DATE))
    {
      char *year, tmp[TIMEBUFLEN], tmp_date[TIMEBUFLEN];

      b_p = make_list_time(&((*f_p)->mtime)) ;

      year = (char*)strrchr(b_p, ' '); /* get year */
      STRCPY(tmp, b_p);
      tmp[year - b_p] = '\0';	       /* date, hr and min */
      b_p = strrchr(tmp, ' ');         /* get hr:min */
      STRCPY(tmp_date, tmp);
      tmp_date[b_p - tmp] = '\0';      /* get date */
      if (year[0] == ' ')
	 *year++;

      if ((*f_p)->mtime < sixmonthsago ||
          (*f_p)->mtime > onehourfromnow)
        SPRINTF((char *) buffer,"%-8.8s%-4.4s", tmp_date, year) ;
      else
        SPRINTF((char *) buffer,"%-13.13s", tmp) ;

      file_text(wno, x, y, buffer, invert) ;
      x += Fm->font_sizeW * Date_width[Fm->fixed_font] ;
    }

  if (LIST_SHOW(LS_SIZE))
    {
      SPRINTF((char *) buffer, "%-8ld", (*f_p)->size) ;
      file_text(wno, x, y, buffer, invert) ;
      x += Fm->font_sizeW * Size_width[Fm->fixed_font] ;
    }

  if (LIST_SHOW(LS_OWNER))
    {
      if ((b_p = fio_get_name((*f_p)->uid)) == 0)
        {
          SPRINTF((char *) buffer, "%d", (*f_p)->uid) ;
          b_p = buffer ;
        }
      file_text(wno, x, y, b_p, invert) ;
      x += Fm->font_sizeW * Owner_width[Fm->fixed_font] ;
    }

  if (LIST_SHOW(LS_GROUP))
    {
      if ((b_p = fio_get_group((*f_p)->gid)) == 0)
        {
          SPRINTF((char *) buffer, "%d", (*f_p)->gid) ;
          b_p = buffer ;
        }
      file_text(wno, x, y, b_p, invert) ;
      x += Fm->font_sizeW * Group_width[Fm->fixed_font] ;
    }

  if (LIST_SHOW(LS_LINKS))
    {
      SPRINTF((char *) buffer, "%3d", (*f_p)->nlink) ;
      file_text(wno, x, y, buffer, invert) ;
      x += Fm->font_sizeW * Link_width[Fm->fixed_font] ;
    }

  if (LIST_SHOW(LS_PERMS))
    {
      b_p = buffer ;
      *b_p++ = (*f_p)->type == FT_DIR ? 'd' : '-' ;
      b_p = (char *) fmtmode((WCHAR *) b_p, (int) (*f_p)->mode) ;
      *b_p = '\0' ;
      file_text(wno, x, y, buffer, invert) ;
      x += Fm->font_sizeW * Permission_width[Fm->fixed_font] ;
    }
}


int
expand_all(f_p)
register Tree_Pane_Object *f_p ;
{
  Boolean new ;
  char path[MAXPATHLEN] ;

  check_stop_key(WNO_TREE) ;
  while (f_p && !Fm->stopped)
    {

/* Unprune branches. */

      if (f_p->status & PRUNE) f_p->status &= ~PRUNE ;

      if (f_p->mtime == 0 || !(f_p->status & TEXPLORE))
        {
          FM_GETPATH(f_p, (WCHAR *) path) ;
          if (add_children(f_p, path, &new) == -1)
            return(-1) ;
        }
      if (f_p->child)
        {
          if (expand_all(f_p->child) == -1) return(-1) ;
        }
      f_p = f_p->sibling;
    }
  return(FM_SUCCESS) ;
}


void
expand_all_nodes()
{
  WCHAR *sdir, tdir[MAXPATHLEN];     /* Save initial contents of path field. */
  Tree_Pane_Object *sibling ;
  int wno      = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;

/* NOTE: Can't do fm_busy_cursor() here if we want the Stop key to work. */

  set_busy_cursor(WNO_TREE, TRUE) ;

  sibling = Fm->tree.selected->sibling ;
  Fm->tree.selected->sibling = NULL ;

  sdir = (WCHAR *)strdup((char*)f->path);
  /* fm_getpath may change tdir so pass in temp buffer and then copy results
     back to f->path so we dont overflow f->path buffer  */
  strcpy ((char*)tdir, (char*)f->path);
  if (fm_getpath(Fm->tree.selected, (WCHAR*)tdir) || chdir((char *) tdir) == -1) {
      free(f->path);
      f->path = (WCHAR*)strdup((char*)tdir);
      free(sdir);
      write_footer(WNO_TREE, strerror(errno), TRUE) ;
  }
  else
    {
      free(f->path);
      f->path = (WCHAR*)strdup((char*)tdir);
      write_footer(WNO_TREE, Str[(int) M_EXPANDALL_1], FALSE) ;
      Fm->stopped = FALSE ;
      if (expand_all(Fm->tree.selected) == -1)
        {
          write_footer(WNO_TREE, Str[(int) M_OUT_PATH], TRUE) ;
          fm_destroy_tree(Fm->tree.selected) ;
          Fm->tree.selected->child = NULL ;
          Fm->tree.selected->mtime = 0 ;
          set_frame_attr(WNO_TREE, FM_FRAME_SHOW, FALSE) ;
          set_busy_cursor(WNO_TREE, FALSE) ;
          free(sdir);
          return ;
        }
      Fm->tree.selected->sibling = sibling ;
      fm_drawtree(TRUE) ;
      fm_visiblefolder(Fm->tree.selected) ;
      /* set back path to what it was */
      free(f->path);
      f->path = sdir;
      CHDIR((char *) f->path) ;
      if (!Fm->stopped) write_footer(WNO_TREE, "", FALSE) ;
    }
  set_busy_cursor(WNO_TREE, FALSE) ;
}


void
find_object()
{

  if (!is_item(FM_FIND_FIND_FRAME))
    {
      find_make() ;
      fm_position_popup(Fm->curr_wno, FM_FIND_FIND_FRAME) ;
    }
  else if (Fm->Pid == 0)            /* Find operation already in progress? */
    {
      set_item_str_attr(FM_FIND_FROMITEM, FM_ITEM_IVALUE,
                        (char *) Fm->file[Fm->curr_wno]->path) ;
      set_item_str_attr(FM_FIND_NAMEITEM,    FM_ITEM_IVALUE, "") ;
      set_item_int_attr(FM_FIND_NAMETOGGLE,  FM_ITEM_IVALUE, 0) ;
      set_item_str_attr(FM_FIND_OWNERITEM,   FM_ITEM_IVALUE, "") ;
      set_item_int_attr(FM_FIND_OWNERTOGGLE, FM_ITEM_IVALUE, 0) ;
      set_item_str_attr(FM_FIND_AFTERITEM,   FM_ITEM_IVALUE, "") ;
      set_item_str_attr(FM_FIND_BEFOREITEM,  FM_ITEM_IVALUE, "") ;
      set_item_int_attr(FM_FIND_TYPEITEM, FM_ITEM_IVALUE, FLDR + DOC + APP) ;
      set_item_str_attr(FM_FIND_PATTERNITEM, FM_ITEM_IVALUE, "") ;
      set_item_int_attr(FM_FIND_CASETOGGLE,  FM_ITEM_IVALUE, -1) ;
    }
  set_item_int_attr(FM_FIND_FIND_FRAME, FM_ITEM_SHOW, TRUE) ;
}


void
fm_all_filedeselect()        /* Deselect all files in all windows. */
{
  int wno ;

  for (wno = 0; wno < Fm->maxwin; wno++)
    if (is_frame(wno)) fm_filedeselect(wno) ;
}


void
fm_all_pathdeselect(curr_wno)     /* Deselect all files in all path panes. */
int curr_wno ;
{
  int wno ;

  for (wno = 0; wno < Fm->maxwin; wno++)
    if (is_frame(wno))
      if (curr_wno != wno) fm_pathdeselect(wno) ;
}


void
fm_busy_cursor(on, wno)      /* Turn busy cursor on/off. */
Boolean on ;
int wno ;
{
  if (on) set_timer(0) ;
  else if (Fm->cur_interval) set_timer(Fm->interval) ;

/* XXX: Until filemgr is multi-threaded, then setting the frame busy on any
 *      frame, should also set it busy on any other open frame.
 */

  for (wno = 0; wno < Fm->maxwin; wno++)
    if (is_frame(wno) && !is_frame_closed(wno))
      busy_cursor(on, wno, &Fm->file[wno]->busy_count) ;

  if (Fm->treeview && !is_frame_closed(WNO_TREE))
    busy_cursor(on, WNO_TREE, &Fm->tree.busy_count) ;
}


int
fm_descendant(c_p, f_p)    /* Search folder tree for descendant of folder. */
Tree_Pane_Object *c_p ;
Tree_Pane_Object *f_p ;
{
  if (c_p == f_p) return(TRUE) ;
  Fm->Child = c_p ;
  Fm->Found_child = FALSE ;
  descendant(f_p) ;
  return(Fm->Found_child) ;
}


void
fm_destroy_tree(f_p)  /* Free up memory allocated to (portion of) tree */
register Tree_Pane_Object *f_p ;
{
  for ( ; f_p; f_p = f_p->sibling)
    if (f_p->child)
      fm_destroy_tree(f_p->child) ;
    else
      {
        FREE(f_p->name) ;
        FREE((char *) f_p) ;
      }
}


/* Deselect any selected objects in a given folder pane window. */

void
fm_filedeselect(wno)
int wno ;
{
  register File_Pane_Object **curr, **last ;  /* Files array pointers. */
  register int i ;

  last = PTR_LAST(wno) ;
  for (curr = PTR_FIRST(wno), i = 0; curr < last ; curr++, i++)
    if ((*curr)->selected)
      {
        (*curr)->selected = FALSE ;
        draw_ith(i, wno) ;
      }
}


int
fm_getpath(c_p, buf)      /* Get full path name from node in tree. */
register Tree_Pane_Object *c_p ;    /* Node. */
WCHAR *buf ;                        /* Place path in buffer. */
{
  Tree_Pane_Object *level[33] ;
  register int levelno ;
  register WCHAR *p_p, *s_p ;

/* Get full path name; back up tree and copy it in reverse order. */

  for (levelno = 0; c_p; c_p = c_p->parent, levelno++)
    {
      if (levelno > 32)
        {

/* Too deep; use slower system call instead. */

          if (!fm_getcwd((char *) buf, MAXPATHLEN))
            {
              fm_msg(TRUE, (char *) buf) ;
              return(FAILURE) ;
            }
        }
      level[levelno] = c_p ;
    }

  p_p = buf ;
  levelno -= 2 ;                /* Skip root. */
  while (levelno >= 0)
    {
      *p_p++ = '/' ;
      s_p = level[levelno]->name ;
      while (*s_p) *p_p++ = *s_p++ ;
      levelno--;
    }
  if (p_p == buf) *p_p++ = '/' ;
  *p_p = '\0' ;

  c_p = level[0] ;
  return(FM_SUCCESS) ;
}


void
fm_init_path_tree(wno)    /* Initialize path/tree tree structure. */
int wno ;
{
  File_Pane *f ;
  Tree_Pane *tp ;         /* Point to appropriate path/tree structure. */
  Tree_Pane_Object *f_p, *nf_p ;             /* Node pointers. */
  WCHAR *n_p, *s_p ;                         /* String pointers. */
  char path[MAXPATHLEN], portion[MAXPATHLEN] ;
  Boolean new ;                              /* Function parameter. */

  if (wno == WNO_TREE)
    {
      wno = Fm->curr_wno ;
      tp  = &Fm->tree ;
    }
  else tp = &Fm->file[wno]->path_pane ;
  f = Fm->file[wno] ;

/* Build path/tree. */

  if (wno == WNO_TREE)
    {
      if ((f_p = make_tree_node((WCHAR *) "/")) == NULL) exit(1) ;
    }
  else
    {
      f_p = (Tree_Pane_Object *)
            LINT_CAST(fm_malloc((unsigned) (sizeof(Tree_Pane_Object)))) ;
      f_p->name = (WCHAR *) fm_malloc((unsigned) 2) ;
      if (!f_p || !f_p->name) exit(1) ;
      STRCPY((char *) f_p->name, "/") ;
      f_p->width = GLYPH_WIDTH ;         /* Assumes GLYPH_WIDTH > strlen "/" */
    }

  tp->root = tp->head = f_p ;
  f_p->parent = f_p->sibling = NULL ;
  f_p->mtime = 0 ;
  f_p->status = 0 ;
  n_p = (WCHAR *) &f->path[1] ;
  while (*n_p)
    {
      s_p = (WCHAR *) portion ;
      while (*n_p && *n_p != '/') *s_p++ = *n_p++ ;
      *s_p = '\0' ;
      if (*n_p) n_p++ ;          /* Skip next / (if any). */
      if (wno == WNO_TREE)
        {
          if ((f_p = make_tree_node((WCHAR *) portion)) == NULL) exit(1) ;
        }
      else
        {
          nf_p = (Tree_Pane_Object *)
                 LINT_CAST(fm_malloc((unsigned) (sizeof(Tree_Pane_Object)))) ;
          nf_p->name = (WCHAR *) fm_malloc((unsigned) (strlen(portion)+1)) ;
          if (!nf_p || !nf_p->name) exit(1) ;
          STRCPY((char *) nf_p->name, portion) ;
          nf_p->width = fm_strlen((char *) portion) ;
          if (nf_p->width < GLYPH_WIDTH) nf_p->width = GLYPH_WIDTH ;
        }
      f_p->child    = nf_p ;
      nf_p->parent  = f_p ;
      nf_p->sibling = NULL ;
      nf_p->mtime   = 0 ;
      nf_p->status  = 0 ;
      f_p           = nf_p ;
    }
  f_p->child  = NULL ;
  tp->current = f_p ;
  tp->lastcur = f_p ;

       if (wno == WASTE)
    tp->head = f_p ;
  else if (f->mtype == FM_CD)
    tp->head = path_to_node(wno, (WCHAR *) Fm->cmntpt[f->devno]) ;
  else if (f->mtype == FM_FLOPPY || f->mtype == FM_DOS)
    tp->head = path_to_node(wno, (WCHAR *) Fm->fmntpt[f->devno]) ;

  FM_GETPATH(f_p, (WCHAR *) path) ;
}


int
fm_new_folder_window(path, name, rawpath, mtype, devno, pos_given, info)
char *path, *name, *rawpath ;
enum media_type mtype ;
int devno, pos_given, info[MAXINFO] ;
{
#ifdef SVR4
  FILE *fp ;
  struct mnttab *mp ;
#endif /*SVR4*/
  File_Pane *f ;
  char pathname[MAXPATHLEN], raw[MAXPATHLEN] ;
  int wno ;

  fm_busy_cursor(TRUE, Fm->curr_wno) ;
  SPRINTF(raw, "%s/%s", path, name) ;
  FM_REALPATH(raw, pathname) ;

  if (is_window_showing(pathname, 1))
    {
      for (wno = 0; wno < Fm->maxwin; wno++)
        if (is_frame(wno) && EQUAL(pathname, (char *) Fm->file[wno]->path))
      fm_busy_cursor(FALSE, Fm->curr_wno) ;
      return(wno) ;
    }

/* Create window in first available slot. */

  for (wno = WIN_SPECIAL+1; wno < Fm->maxwin; wno++)
    if (!is_frame(wno)) break ;

  if (wno >= Fm->maxwin)
    {
      resize_arrays(FALSE, 5) ;
      resize_X_arrays(FALSE, 5) ;
    }
  f = Fm->file[wno] ;
  free(f->path);
  f->path = (WCHAR*)strdup(pathname);
  STRCPY((char *) f->filter, (char *) Fm->filter) ;

  if (mtype == FM_CD)
    {
      f->devno = devno ;
      if (Fm->cmntpt[devno] == NULL)
        read_str((char **) &Fm->cmntpt[devno], (char *) f->path) ;
      if (Fm->crawpath[devno] == NULL)
        read_str((char **) &Fm->crawpath[devno], (char *) rawpath) ;
    }
  else if (mtype == FM_FLOPPY)
    {
      f->devno = devno ;
      if (Fm->fmntpt[devno] == NULL)
        read_str((char **) &Fm->fmntpt[devno], (char *) f->path) ;
      if (Fm->frawpath[devno] == NULL)
        read_str((char **) &Fm->frawpath[devno], (char *) rawpath) ;
#ifdef SVR4
      if ((fp = fopen(MNTTAB, "r")) != NULL)
        {
          mp = (struct mnttab *) malloc(sizeof(struct mnttab)) ;
          while (fm_getmntent(fp, mp) != -1)
            if (EQUAL(mp->mnt_mountp, Fm->fmntpt[f->devno]))
              if (EQUAL(mp->mnt_fstype, "pcfs")) mtype = FM_DOS ;
          FCLOSE(fp) ;
          FREE(mp) ;
        }
#endif /*SVR4*/
    }
  f->mtype = mtype ;

  clear_position_info(wno) ;
  set_window_params(wno) ;
  reset_file_pane(wno) ;
  if (Fm->load_state)
    load_directory_info(wno, pos_given, info) ;
  make_new_frame(wno, mtype) ;
  Fm->num_win++ ;
  set_window_dims(wno, info) ;
  set_status_glyph(wno, mtype) ;

  if (Fm->load_icons) load_icon_positions(wno) ;
  if (Fm->use_cache)  load_cache(wno) ;
  fm_display_folder((BYTE) FM_NEW_FOLDER, Fm->curr_wno) ;
  if (get_pos_attr(wno, FM_POS_MOVED))
    {
      fm_scrollbar_scroll_to(wno, FM_V_SBAR, get_pos_attr(wno, FM_POS_VSBAR)) ;
      fm_scrollbar_scroll_to(wno, FM_H_SBAR, get_pos_attr(wno, FM_POS_HSBAR)) ;
    }
  adjust_tree(wno) ;

  if (Fm->treeview)
    fm_visiblefolder(path_to_node(WNO_TREE, Fm->file[Fm->curr_wno]->path)) ;

  fm_busy_cursor(FALSE, Fm->curr_wno) ;
  sync_server() ;
  set_frame_attr(wno, FM_FRAME_SHOW, TRUE) ;
  sync_server() ;
  return(wno) ;
}


int
fm_openfile(name, pattern, folder, wno) /* Open a file. */
char *name ;                            /* File name (incl path). */
char *pattern ;                         /* Pattern it matched. */
Boolean folder ;                        /* Goto folder? */
int wno ;
{
  register char *n_p ;                  /* Name pointer. */
  char *fname ;                         /* File name (excl path). */
  char path[MAXPATHLEN] ;
  Tree_Pane_Object *f_p, *parent ;      /* Tree pointers. */
  int i ;
  int info[MAXINFO] ;                   /* Info values ignored below. */
  int more ;                            /* More of path name to come? */
  Boolean make_folder = FALSE ;         /* Did we make a folder? */
  Tree_Pane *tp ;
  Tree_Pane_Object *child ;             /* Temp folder child. */

  if (wno == WNO_TREE) tp = &Fm->tree ;
  else                 tp = &Fm->file[wno]->path_pane ;

  if (folder) fname = NULL ;
  else
    {

/* Remove last component of path. */

      n_p = name ;
      while (*n_p) n_p++ ;
      while (*n_p != '/' && n_p != name) n_p-- ;

/* It's a filename, match it. */

      if (n_p == name)
        {
          fname = n_p ;
          goto match ;
        }

      fname = n_p+1 ;
      *n_p = '\0' ;
    }

  more = TRUE ;

  if (*name == '/')
    {

/* Begin at root... */

      name++ ;
      if (*name == '\0')
        {
          more = FALSE ;
          f_p  = tp->root ;
        }
      else
        {
          f_p    = tp->root->child ;
          parent = tp->root ;
        }
    }
  else
    {

/* Begin at selected or current folder... */

      f_p    = tp->selected ? tp->selected : tp->current ;
      parent = f_p ;
      f_p    = f_p->child ;
      if (f_p) f_p->status &= ~PRUNE ;
    }

  while (more)
    {

/* Get next folder name... */

      while (*name == '/') name++ ;         /* Skip extra /'s. */
      n_p = name ;
      while (*n_p && *n_p != '/') n_p++ ;

      if ((more = (*n_p == '/')) != '\0') *n_p = '\0' ;
      else if (n_p == name)                 /* / at end. */
        break ;

      for (; f_p; f_p = f_p->sibling)
        if (strcmp((char *) f_p->name, name) == 0)
          break ;

      if (f_p)                              /* Found folder? */
        {
          if (more)
            {
              parent = f_p ;
              f_p = f_p->child ;
              if (f_p) f_p->status &= ~PRUNE ;
            }
          else break ;                      /* Last folder in path. */
        }
      else
        {

/* We have to make folder. */

          make_folder = TRUE ;
          if ((f_p = make_tree_node((WCHAR *) name)) == NULL) return(0) ;

          if (parent && parent->mtime == 0 && parent->child)
            {

/*  Assign parent some bogus time so it thinks we've seen this folder,
 *  (revisiting folder will expand it, but preserve existing children).
 */

              parent->mtime = 10 ;
            }
          STRCPY((char *) f_p->name, name) ;
          f_p->parent   = parent ;
          f_p->sibling  = parent->child ;
          f_p->child    = NULL ;
          f_p->status   = 0 ;
          f_p->mtime    = 0 ;
          parent->child = f_p ;
          if (more)
            {
              parent = f_p ;
              f_p = f_p->child ;
              if (f_p) f_p->status &= ~PRUNE ;
            }
          else break ;                      /* Last folder in path. */
        }
      if (more)
        {
          *n_p = '/' ;
          name = n_p+1 ;
        }
    }

/* We've arrived at the correct folder. */

  if (fname) *(fname-1) = '/' ;       /* Restore name. */

  if (f_p != (tp->selected ? tp->selected : tp->current))
    {
      if (wno == WNO_TREE)
        {
          FM_GETPATH(f_p, (WCHAR *) path) ;
          for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
          wno = fm_new_folder_window("", path, NULL, FM_DISK, 0, FALSE, info) ;
        }
      else
        {

/* File isn't in current directory... */

	  strcpy(path, (char*)Fm->file[wno]->path);
          FM_GETPATH(f_p, (WCHAR*)path) ;
	  free(Fm->file[wno]->path);
	  Fm->file[wno]->path = (WCHAR*)strdup(path);
          if (chdir((char *) Fm->file[wno]->path) == -1)
            {
              fm_msg(TRUE, strerror(errno)) ;
              return(0) ;
            }
          load_and_draw_folder(wno) ;
        }

      tp->current = f_p ;

/* Reset the head of the tree if this is a floppy or a CD. */

  if (wno != WNO_TREE)
    {
           if (Fm->file[wno]->mtype == FM_CD)
        tp->head = path_to_node(wno,
                                (WCHAR *) Fm->cmntpt[Fm->file[wno]->devno]) ;
      else if (Fm->file[wno]->mtype == FM_FLOPPY ||
               Fm->file[wno]->mtype == FM_DOS)
        tp->head = path_to_node(wno,
                                (WCHAR *) Fm->fmntpt[Fm->file[wno]->devno]) ;
    }

/* Did folder repaint discover new folders? */

      child = f_p->child ;
      if (child != f_p->child) make_folder = TRUE ;

      if (tp->head != tp->root && !fm_descendant(f_p, tp->head))
        {

/* Folder not in current subtree; back to real root. */

          tp->head->sibling = tp->sibling ;
          tp->head          = tp->root ;
          make_folder       = TRUE ;
        }

      if (make_folder || wno != WNO_TREE)
        {
          if (wno == WNO_TREE) fm_drawtree(TRUE) ;
          else                 draw_path(wno, TRUE) ;
        }
      else fm_showopen() ;

      if (wno == WNO_TREE)
        {
          set_tree_icon(tp->current, TRUE) ;
          if (Fm->treeview)
            {
              fm_visiblefolder(tp->current) ;
              draw_folder(wno, tp->current, FT_DIR_OPEN, TRUE) ;
            }
          tp->selected = tp->lastcur = tp->current ;
        }
      else tp->selected = tp->lastcur = NULL ;

      Fm->tree.current = path_to_node(WNO_TREE, Fm->file[wno]->path) ;
      set_tree_icon(Fm->tree.current, TRUE) ;
      adjust_tree(wno) ;
      if (Fm->treeview)
        {
          fm_drawtree(TRUE) ;
          fm_visiblefolder(Fm->tree.current) ;
        }
    }

/* If we were passed a pattern, then try & edit it, otherwise match it. */

  if (fname)
    {

match:

      if (*fname == '.' && !Fm->file[wno]->see_dot)
        fm_msg(TRUE, Str[(int) M_HIDDEN]) ;
      else
        fm_match_files(wno, fname, (pattern ? TRUE : FALSE)) ;
    }
  return(1) ;
}


void
fm_open_folder(newwin)      /* Open selected folder. */
int newwin ;
{
  char buf[MAXPATHLEN], path[MAXPATHLEN], resolved_name[MAXPATHLEN] ;
  int i, info[MAXINFO] ;
  int wno = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;
  Boolean children ;
  char *b_p = buf ;
  char *s_p ;
  struct stat fstatus ;

  if (!Fm->treeview) return ;

  if (Fm->tree.selected == NULL)
    Fm->tree.selected = path_to_node(WNO_TREE, f->path) ;

  if (fm_getpath(Fm->tree.selected, (WCHAR *) path))
    {
      fm_msg(TRUE, strerror(errno)) ;
      return ;
    }

  if (newwin)
    {
      SPRINTF(buf, Str[(int) M_OPENING], Fm->tree.selected->name) ;
      write_footer(WNO_TREE, buf, FALSE) ;
      for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
      FM_NEW_FOLDER_WINDOW("", path, NULL, FM_DISK, 0, FALSE, info) ;
    }

  fm_busy_cursor(TRUE, WNO_TREE) ;

/* Open reveals folder's children, if not currently visible. */

  if (Fm->tree.selected->mtime == 0 || !(Fm->tree.selected->status & TEXPLORE))
    {
      if (add_children(Fm->tree.selected, path, &children) == 0)
        if (children) fm_drawtree(TRUE) ;
    }
  else if ((fm_stat((char *) f->path, &fstatus) == 0) &&
           Fm->tree.selected->mtime < fstatus.st_mtime)
    {

/* Folder has been modified; check subfolders */

      if (add_children(Fm->tree.selected, path, &children) == 0)
        if (children) fm_drawtree(TRUE) ;
    }
  Fm->tree.selected->status |= TEXPLORE ;

/* Open reveals hidden folder. */

  if (Fm->tree.selected->status & PRUNE)
    {
      Fm->tree.selected->status &= ~PRUNE ;
      fm_drawtree(TRUE) ;
    }

/* Open a symbolically linked folder displays the link contents. */

  if (Fm->tree.selected->status & SYMLINK)
    {

/*  Symbolic link to directory; get reference's real name.
 *  These names may be relative pathnames...
 */

      b_p = buf ;
      if (fm_realpath(path, buf) == NULL)
        {
          fm_msg(TRUE, strerror(errno)) ;
          fm_busy_cursor(FALSE, WNO_TREE) ;
          return ;
        }

/* Relative pathname? */

      if (*b_p != '/')
        {

/* Translate to absolute. */

          STRCPY(resolved_name, path) ;
          if ((s_p = (char *) strrchr(resolved_name, '/')) != NULL)
            *(s_p+1) = '\0' ;
          STRCAT(resolved_name, b_p) ;
          STRCPY(buf, resolved_name) ;
          FM_REALPATH(b_p, resolved_name) ;
          b_p = resolved_name ;
        }
      fm_msg(FALSE, Str[(int) M_LINKEDTO], Fm->tree.selected->name, b_p) ;
      draw_folder(WNO_TREE, Fm->tree.selected, FT_DIR_OPEN, TRUE) ;
      Fm->tree.selected = NULL ;
      if (!newwin) fm_openfile(b_p, (char *) 0, TRUE, wno) ;
    }
  else
    {
      Fm->tree.current = Fm->tree.selected ;         /* Selected is now open. */
      draw_folder(WNO_TREE, Fm->tree.selected, FT_DIR_OPEN, FALSE) ;
      fm_showopen() ;                              /* Show open folder. */

/* Turn reverse back on. */

      draw_folder(WNO_TREE, Fm->tree.selected, FT_DIR_OPEN, TRUE) ;
    }

  set_menu_items(FM_GOTO_MENU, set_goto_button_menu) ;
  fm_busy_cursor(FALSE, WNO_TREE) ;
}


void
fm_open_path(wno, newwin)
int wno, newwin ;
{
  Tree_Pane *tp ;
  char last_path[MAXPATHLEN], path[MAXPATHLEN] ;
  int i, info[MAXINFO] ;
  File_Pane *f = Fm->file[wno] ;

  fm_busy_cursor(TRUE, wno) ;
  Fm->isgoto_select = TRUE;
  tp = &f->path_pane ;
  if (tp->selected == NULL) tp->selected = path_to_node(wno, f->path) ;
  STRCPY(last_path, (char *) f->path) ;
  FM_GETPATH(tp->selected, (WCHAR *) path) ;

  if (wno == WASTE) newwin = FALSE ;
  if (newwin)
    {
      for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
      FM_NEW_FOLDER_WINDOW("", path, NULL, f->mtype, f->devno, FALSE, info) ;
    }
  else
    {
      if (is_window_showing(path, 1))
        {
          fm_busy_cursor(FALSE, wno) ;
          return ;
        }
      set_tree_icon(path_to_node(WNO_TREE, (WCHAR *) last_path), FALSE) ;
      save_directory_info(wno) ;
      clear_position_info(wno) ;
      free(f->path);
      f->path = (WCHAR*)strdup(path);
      set_tree_icon(path_to_node(WNO_TREE, (WCHAR *) path), TRUE) ;
      tp->current  = tp->selected ;
      if (chdir((char *) f->path) == -1) fm_msg(TRUE, (char*)strerror(errno)) ;
      if (!EQUAL(last_path, (char *) f->path)) load_and_draw_folder(wno) ;
    }

  draw_path(wno, TRUE) ;
  set_menu_items(FM_GOTO_MENU, set_goto_button_menu) ;
  Fm->isgoto_select = FALSE;
  fm_busy_cursor(FALSE, wno) ;
}


void
fm_pathdeselect(wno)    /* Deselect folder in path pane for this window. */
int wno ;
{
  File_Pane *f  = Fm->file[wno] ;
  Tree_Pane *tp = &f->path_pane ;
  BYTE ftype ;

  if (tp->selected)
    {
      if (wno == WASTE && tp->selected == tp->head)
        ftype = FT_WASTE ;
      else if (f->mtype == FM_CD &&
        (tp->selected == path_to_node(wno, (WCHAR *) Fm->cmntpt[f->devno])))
        ftype = FT_CD ;
      else if (f->mtype == FM_FLOPPY &&
        (tp->selected == path_to_node(wno, (WCHAR *) Fm->fmntpt[f->devno])))
        ftype = FT_FLOPPY ;
      else if (f->mtype == FM_DOS &&
        (tp->selected == path_to_node(wno, (WCHAR *) Fm->fmntpt[f->devno])))
        ftype = FT_DOS ;
      else
        ftype = (tp->selected == tp->current) ? FT_DIR_OPEN : FT_DIR ;
      draw_folder(wno, tp->selected, ftype, FALSE) ;
      tp->selected = NULL ;
    }
}


/* Code shamelessly stolen from .../lib/libc/gen/common/realpath.c */

char *
fm_realpath(raw, canon)
char *raw, *canon ;
{
  char *d, *modcanon, *s ;
  char *limit = canon + MAXPATHLEN ;

  if (Fm->follow_links) return((char *) realpath(raw, canon)) ;

/* Do a bit of sanity checking.  */

  if (raw == NULL || canon == NULL)
    {
      errno = EINVAL ;
      return(NULL) ;
    }

/*  If the path in raw is not already absolute, convert it to that form.
 *  In any case, initialize canon with the absolute form of raw.  Make
 *  sure that none of the operations overflow the corresponding buffers.
 *  The code below does the copy operations by hand so that it can easily
 *  keep track of whether overflow is about to occur.
 */

  s = raw ;
  d = canon ;
  modcanon = canon ;
  while (d < limit && *s) *d++ = *s++ ;

/* Add a trailing slash to simplify the code below. */

  s = "/" ;
  while (d < limit && (*d++ = *s++)) continue ;

/*  Canonicalize the path.  The strategy is to update in place, with
 *  d pointing to the end of the canonicalized portion and s to the
 *  current spot from which we're copying.  This works because
 *  canonicalization doesn't increase path length, except as discussed
 *  below.  Note also that the path has had a slash added at its end.
 *  This greatly simplifies the treatment of boundary conditions.
 */

  d = s = modcanon ;
  while (d < limit && *s)
    {
      if ((*d++ = *s++) == '/' && d > canon + 1)
        {
          char *t = d - 2 ;

          switch (*t)
            {
              case '/' : d-- ;                   /* Found // in the name. */
                         continue ;
              case '.' : switch (*--t)
                           {
                             case '/' : d -= 2 ; /* Found /./ in the name. */
                                        continue ;
                             case '.' : if (*--t == '/')
                                          {
                                                 /* Found /../ in the name. */
                                            while (t > canon && *--t != '/')
                                              continue ;
                                            d = t + 1 ;
                                          }
                                        continue ;
                             default  : break ;
                           }
                         break ;
              default  : break ;
            }
        }
    }

/* Remove the trailing slash that was added above. */

  if (*(d - 1) == '/' && d > canon + 1) d-- ;
  *d = '\0' ;
  return(canon) ;
}


void
fm_showopen()      /* Avoid repainting tree to show open folder */
{
  if (!Fm->treeview) return ;

/*  We've seen this folder already. Just exchange the open folder icons. */

  if (Fm->tree.lastcur)
    draw_folder(WNO_TREE, Fm->tree.lastcur, FT_DIR, FALSE) ;

  draw_folder(WNO_TREE, Fm->tree.current, FT_DIR_OPEN, FALSE) ;

/* Ensure that this folder is visible... */

  fm_visiblefolder(Fm->tree.current) ;
  Fm->tree.lastcur = Fm->tree.current ;
}


void
fm_sort_children(t_p)      /* Sort node's children by name. */
Tree_Pane_Object *t_p ;
{
  register Tree_Pane_Object *c_p ;
  Tree_Pane_Object *child[256] ;
  register int cno ;

  if (t_p->child == NULL || t_p->child->sibling == NULL) return ;

  for (c_p = t_p->child, cno = 0; c_p && cno < 256; c_p = c_p->sibling)
    child[cno++] = c_p ;

  qsort((char *) child, cno, sizeof(Tree_Pane_Object *), compare_folder_names) ;

/* Fix sibling pointers. */

  child[cno] = NULL ;
  while (cno)
    {
      child[cno-1]->sibling = child[cno] ;
      cno-- ;
    }
  t_p->child = child[0] ;
}


void
fm_treedeselect()    /* Deselect folder */
{
  BYTE ftype ;

  if (Fm->treeview && Fm->tree.selected)
    {
      ftype = (Fm->tree.selected == Fm->tree.current) ? FT_DIR_OPEN : FT_DIR ;
      draw_folder(WNO_TREE, Fm->tree.selected, ftype, FALSE) ;
      Fm->tree.selected = NULL ;
      write_footer(WNO_TREE, "", FALSE) ;
    }
}


WCHAR *
fmtmode(lp, flags)    /* Display list mode permissions. */
WCHAR *lp ;
int flags ;
{
  static int m1[] = { 1, S_IREAD  >> 0, 'r', '-' } ;
  static int m2[] = { 1, S_IWRITE >> 0, 'w', '-' } ;
  static int m3[] = { 3, S_ISUID | (S_IEXEC >> 0), 's', S_IEXEC >> 0, 'x',
                         S_ISUID, 'S', '-' } ;
  static int m4[] = { 1, S_IREAD >> 3, 'r', '-' } ;
  static int m5[] = { 1, S_IWRITE >> 3, 'w', '-' } ;
  static int m6[] = { 3, S_ISGID | (S_IEXEC >> 3), 's', S_IEXEC >> 3, 'x',
                         S_ISGID, 'S', '-' } ;
  static int m7[] = { 1, S_IREAD >> 6, 'r', '-' } ;
  static int m8[] = { 1, S_IWRITE >> 6, 'w', '-' } ;
  static int m9[] = { 2, S_ISVTX, 't', S_IEXEC >> 6, 'x', '-' } ;
  static int *m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9} ;
  int **mp ;

  for (mp = &m[0]; mp < &m[sizeof(m) / sizeof(m[0])]; )
    {
      register int *pairp = *mp++ ;
      register int n = *pairp++ ;

      while (n-- > 0)
        {
          if ((flags & *pairp) == *pairp)
            {
              pairp++ ;
              break ;
            }
          else pairp += 2 ;
        }
      *lp++ = *pairp ;
    }
  return(lp) ;
}


/* Return the X coordinate of the file pane object depending upon mode. */

int
get_file_obj_x(wno, fpo)
int wno ;
File_Pane_Object *fpo ;
{
  short dmode = Fm->file[wno]->display_mode ;

       if (dmode == VIEW_BY_ICON)    return(fpo->ix) ;
  else if (dmode == VIEW_BY_CONTENT) return(fpo->cx) ;
  else if (Fm->file[wno]->listopts)  return(fpo->ox) ;
  else                               return(fpo->lx) ;
}


/* Return the Y coordinate of the file pane object depending upon mode. */

int
get_file_obj_y(wno, fpo)
int wno ;
File_Pane_Object *fpo ;
{
  short dmode = Fm->file[wno]->display_mode ;

       if (dmode == VIEW_BY_ICON)    return(fpo->iy) ;
  else if (dmode == VIEW_BY_CONTENT) return(fpo->cy) ;
  else if (Fm->file[wno]->listopts)  return(fpo->oy) ;
  else                               return(fpo->ly) ;
}


void
hide_node()    /* Don't show selected folder's descendents. */
{

/* Bug fix 1041681: stop "hide subfolders" option from toggling. */

  if (!(Fm->tree.selected->status & PRUNE))
    {
      if (Fm->tree.selected != Fm->tree.current)
        {
          fm_destroy_tree(Fm->tree.selected->child) ;
          Fm->tree.selected->child = NULL ;
          Fm->tree.selected->mtime = 0 ;
        }
      else Fm->tree.selected->status |= PRUNE ;
    }

  fm_drawtree(TRUE) ;
  fm_visiblefolder(Fm->tree.selected) ;    /* Track selected. */
}


/*  This routine is called when an ascii char is typed in the file pane.
 *  It will try to select files matching the user's input chars.
 */

void
incremental_selection(ec, clear, c)
Event_Context ec ;
Boolean clear ;
int c ;
{
  static int i, building ;            /* Make static to keep around. */
  static char buf[32] ;           /* From call to call.         */
  char eoe[16] ;                  /* End of expression chars. */
  char reg[16] ;                  /* Regular expression chars. */

  if (clear)                      /* Clear the search pattern. */
    {
      i = 0 ;
      buf[0] = 0 ;
      return ;
    }

/*  Only search for ascii keys which were released.  This will filter
 *  out keys pressed down and repeating.
 */

  if (!ec->down)
    {

/*  In general, we try to match files on every char typed.  Only when
 *  the user is building a regular expression do we wait to match files.
 */

      SPRINTF(eoe, "%c%c%c", 27, 13, 0) ;       /* ESC, RETURN */
      SPRINTF(reg, "%c%c%c", '*', '?', 0) ;

      if (strchr(eoe, (int) c))        /* End of expression char found. */
        {
          if (building) fm_msg(FALSE, Str[(int) M_MATCHING], buf) ;
          else
            fm_clrmsg() ;              /* Clear old message. */

          if (!building || !fm_match_files(ec->wno, buf, FALSE))
            {
              i = 0 ;                  /* Clear out. */
              buf[0] = 0 ;
            }
          building = FALSE ;
        }
      else if (strchr(reg, (int) c) ||    /* Regular expression char found. */
               (c == '.' && buf[i-1] == '*'))   /* Period after star found. */
        {
          buf[i++] = c ;                  /* Add char to end. */
          buf[i]   = '\0' ;               /* Print without "*" on end. */
          fm_msg(FALSE, Str[(int) M_BUILDING], buf) ;
          building = TRUE ;
        }
      else
        {
          buf[i++] = c ;         /* Add char to end. */
          buf[i++] = '*' ;       /* Add '*' to complete regular expression. */
          buf[i--] = '\0' ;      /* Decr. to overwrite the '*' char later. */
          fm_msg(FALSE, Str[(int) M_MATCHING], buf) ;
          if (!fm_match_files(ec->wno, buf, FALSE))
            {
              i = 0 ;            /* Clear out. */
              buf[0] = 0 ;
            }
          building = FALSE ;
        }
    }
}


/* Return if a valid dnd target (for highlighting). */

Boolean
is_dnd_target(fpo)
File_Pane_Object *fpo ;
{

/*  This is a flawed approach to handle dropping an item on another
 *  item.  We would like to allow the dropped object to be coverted
 *  to something the recipient will understand.  However, this means
 *  we have to identify what is being dropped, which seems almost
 *  impossible with the current dnd solution.  So this is used to
 *  implement only half the solution, that is we will setup and get
 *  a drop method filter from the CE, but leave the dropped object
 *  identification for later.
 */

/*  A valid dnd target can be a directory or anything with a
 *  drop method defined in the CE.
 */

       if (fpo->type == FT_DIR)          return(TRUE) ;
  else if (get_drop_method(fpo) != NULL) return(TRUE) ;
  else                                   return(FALSE) ;
}


/* Check to see if there is a window already monitoring this directory. */

int
is_window_showing(pathname, show_fm_msg)
char *pathname ;
int show_fm_msg;
{
  int wno ;

  for (wno = 0; wno < Fm->maxwin; wno++)
    if (is_frame(wno) && EQUAL(pathname, (char *) Fm->file[wno]->path))
      {
	if (show_fm_msg)
        	fm_msg(TRUE, Str[(int) M_IS_VIEWED], pathname) ;
        if (is_frame_closed(wno)) set_frame_attr(wno, FM_FRAME_CLOSED, FALSE) ;
        set_frame_attr(wno, FM_FRAME_SHOW, TRUE) ;
        return(TRUE) ;
      }
  return(FALSE) ;
}
int
is_window_showing_with_path(pathname)
char *pathname ;
{
  int wno ;

  for (wno = 0; wno < Fm->maxwin; wno++)
    if (is_frame(wno) && strncmp(pathname, (char *)Fm->file[wno]->path, strlen(pathname)) == 0)
        return(TRUE) ;
  return(FALSE) ;
}

int
list_width(wno)
int wno ;
{
  Boolean f = Fm->fixed_font ;
  int     w = MARGIN + LIST_HEIGHT ;

  w += Fm->file[wno]->widest_name + MARGIN ;
  if (LIST_SHOW(LS_PERMS)) w += Fm->font_sizeW * Permission_width[f] ;
  if (LIST_SHOW(LS_LINKS)) w += Fm->font_sizeW * Link_width[f] ;
  if (LIST_SHOW(LS_OWNER)) w += Fm->font_sizeW * Group_width[f] ;
  if (LIST_SHOW(LS_GROUP)) w += Fm->font_sizeW * Size_width[f] ;
  if (LIST_SHOW(LS_SIZE))  w += Fm->font_sizeW * Size_width[f] ;
  if (LIST_SHOW(LS_DATE))  w += Fm->font_sizeW * Date_width[f] ;
  w += MARGIN ;
  return(w) ;
}


void
load_and_draw_folder(wno)
int wno ;
{
  int info[MAXINFO] ;                   /* Info values ignored below. */

  reset_file_pane(wno) ;
  if (Fm->load_state) load_directory_info(wno, FALSE, info) ;
  if (Fm->load_icons) load_icon_positions(wno) ;
  if (Fm->use_cache)  load_cache(wno) ;

  fm_display_folder((BYTE) FM_NEW_FOLDER, wno) ;
  if (get_pos_attr(wno, FM_POS_MOVED))
    {
      fm_scrollbar_scroll_to(wno, FM_V_SBAR, get_pos_attr(wno, FM_POS_VSBAR)) ;
      fm_scrollbar_scroll_to(wno, FM_H_SBAR, get_pos_attr(wno, FM_POS_HSBAR)) ;
    }
  if (is_popup_showing(FM_PO_PO_FRAME))
    {
      reset_cur_folder_panel() ;         /* Set view panel items. */
      set_list_item(FALSE) ;             /* [Un]grayout view panel items. */
    }
}


Tree_Pane_Object *
make_tree_node(name)
WCHAR *name ;
{
  int i ;
  Tree_Pane_Object *t_p ;

  if (Fm->ntpo >= Fm->maxtpo)
    {
      Fm->tpo = (Tree_Pane_Object **)
        LINT_CAST(make_mem((char *) Fm->tpo,
              sizeof(Tree_Pane_Object **) *  Fm->maxtpo,
              sizeof(Tree_Pane_Object **) * (Fm->maxtpo + TPANE_ALLOC_INC),
              (Fm->maxtpo == 0))) ;
      for (i = 0; i < TPANE_ALLOC_INC; i++)
        Fm->tpo[Fm->maxtpo + i] = (Tree_Pane_Object *)
          fm_malloc((unsigned int) sizeof(Tree_Pane_Object)) ;

      Fm->maxtpo += TPANE_ALLOC_INC ;
    }

  t_p = Fm->tpo[Fm->ntpo++] ;
  if (t_p == NULL) return(NULL) ;
  t_p->name = (WCHAR *) fm_malloc((unsigned) strlen((char *) name) + 1) ;
  if (t_p->name == NULL) return(NULL) ;

  t_p->width = fm_strlen((char *) name) ;
  if (t_p->width < GLYPH_WIDTH) t_p->width = GLYPH_WIDTH ;
  return(t_p) ;
}


Tree_Pane_Object *
mouse(x, y, f_p)                 /* Are we over a folder? */
int x, y ;
Tree_Pane_Object *f_p ;
{
  Fm->Chosen = NULL ;
  mouse_recursive(x, y, f_p) ;
  return(Fm->Chosen) ;
}


void
mouse_recursive(x, y, f_p)
register int x, y ;
register Tree_Pane_Object *f_p ;
{
  int xoff, yoff ;

  if (Fm->Chosen) return ;

  for ( ; f_p; f_p = f_p->sibling)
    {
      if (!Fm->file[Fm->curr_wno]->see_dot && f_p->name[0] == '.')
        if (strcmp((char *) f_p->name, ".wastebasket")) continue ;

      xoff = (Fm->tree_vertical) ? Fm->tree_Ymin : 0 ;
      yoff = (Fm->tree_vertical) ? 0 : Fm->tree_Ymin ;

      if ((x >= f_p->Xpos - xoff &&
           x <= f_p->Xpos - xoff + GLYPH_WIDTH) &&
          (y >= f_p->Ypos - yoff &&
           y <= f_p->Ypos - yoff + GLYPH_HEIGHT))
        {
          Fm->Chosen = f_p ;
          return ;
        }
      if (f_p->child && !(f_p->status & PRUNE))
        mouse_recursive(x, y, f_p->child) ;
    }
}


void
open_dir(fpo, wno)
File_Pane_Object *fpo ;
int wno ;
{
  char *cp, pathname[MAXPATHLEN] ;
  int info[MAXINFO], len ;
  File_Pane *f = Fm->file[wno] ;
  Tree_Pane *tp ;

  fm_busy_cursor(TRUE, wno) ;
  cp = (char *) fpo->name ;
  if (chdir((char *) cp) == -1)
    {
      fm_msg(TRUE, strerror(errno)) ;
      fm_busy_cursor(FALSE, wno) ;
      return ;
    }

  if (!Fm->follow_links)
    {
      STRCPY(pathname, (char *) f->path) ;
      len = strlen(pathname) ;
      if (len > 1) STRCAT(pathname, "/") ;
      STRCAT(pathname, (char *) cp) ;
    }
  if (fm_getcwd(pathname, MAXPATHLEN+1) == 0)
    fm_msg(TRUE, pathname) ;              /* Error message in pathname. */

  if (is_window_showing(pathname, 1))
    {
      fm_busy_cursor(FALSE, wno) ;
      return ;
    }

  save_directory_info(wno) ;
  clear_position_info(wno) ;

  set_tree_icon(path_to_node(WNO_TREE, f->path), FALSE) ;
  free(f->path);
  f->path = (WCHAR*)strdup(pathname);

  set_window_params(wno) ;
  load_and_draw_folder(wno) ;

  tp = &f->path_pane ;
  tp->current = path_to_node(wno, f->path) ;
  draw_path(wno, TRUE) ;
  adjust_tree(wno) ;
  set_tree_icon(path_to_node(WNO_TREE, f->path), TRUE) ;
  Fm->tree.current = path_to_node(WNO_TREE, f->path) ;
  fm_drawtree(TRUE) ;
  fm_visiblefolder(Fm->tree.current) ;
  fm_busy_cursor(FALSE, wno) ;
}


/* Generic function to open selected files in all or some windows. */

int
open_files(wno, x, y, method, use_subframe, use_editor)
int wno ;                   /* Wno to check for selected files. */
int x, y ;                  /* X & y coord to open object. */
char *method ;              /* Custom open method to use. */
Boolean use_subframe ;      /* Use subframe to display dir? */
Boolean use_editor ;        /* Force use of editor? */
{
  char lastpath[MAXPATHLEN] ;     /* Check for opening directory. */
  File_Pane_Object **f_p, **l_p ;
  File_Pane *f  = Fm->file[wno] ;
  Boolean found = FALSE ;

  CHDIR((char *) f->path) ;
  STRCPY(lastpath, (char *) f->path) ;

/* Loop thru selected items in window. */

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p != l_p; f_p++)
    {
      if (!use_subframe && strcmp(lastpath, (char *) f->path)) break ;
      if ((*f_p)->selected)
        {
          found = TRUE ;
          open_object(*f_p, (char *) f->path, wno, x, y, method,
                      FALSE, use_subframe, use_editor) ;
        }
    }

  if (found == FALSE) fm_msg(TRUE, Str[(int) M_NOSEL]) ;
  return(1) ;
}


/* Generic function to open a given object. */

int
open_object(fpo, path, wno, x, y, method, pos_given, use_subframe, use_editor)
File_Pane_Object *fpo ;      /* Object to open. */
char *path ;                 /* Path where object is located. */
int wno ;                    /* Window number. */
int x, y ;                   /* X & y coord to open object. */
char *method ;               /* Custom open method to use. */
Boolean pos_given ;          /* Directory position given? */
Boolean use_subframe ;       /* Use subframe to display dir? */
Boolean use_editor ;         /* Force use of editor? */
{
  int error, i, info[MAXINFO] ;
  char cmd[MAX_CMD_LEN+1], expanded_cmd[MAX_CMD_LEN+1], fname[MAXPATHLEN] ;
  File_Pane *f = Fm->file[wno] ;

  if (!fpo)
    {
      fm_msg(TRUE, Str[(int) M_NOOPEN_INV_OBJ]) ;
      return(FALSE) ;
    }

  cmd[0] = '\0' ;

  if (!method)       /* Use default open method. */
    {
      char *om ;       /* Open method ptr. */

      if (use_editor) om = get_default_open_method(fpo, use_editor) ;
      else            om = get_open_method(fpo) ;
      if (om)
        STRCPY(cmd, (char *) om) ;
    }
  else STRCPY(cmd, (char *) method) ;

  if (cmd && !isempty((WCHAR *) cmd))
    {
      char *args = NULL ;

/* Expand $FILE & $ARG */

      EXPAND_KEYWORDS(cmd, path, (char *) fpo->name, args) ;
      ds_expand_pathname(cmd, expanded_cmd) ;

      fm_msg(FALSE, Str[(int) M_OPEN_ERRORS], fpo->name) ;

      if (!use_editor && fm_get_app_type((char *) fpo->name) == FM_SHELL_APP)
        {
          char temp[MAX_CMD_LEN+1] ;
          int reply ;

/* Does it look like it should be run in a shell?  If so, prompt user. */

          reply = prompt_with_message(wno, Str[(int) M_APPINSHELLTOOL],
                                      Str[(int) M_APP_SHELLYES],
                                      Str[(int) M_APP_SHELLNO],
                                      Str[(int) M_CANCEL]) ;
          if (reply == FM_CANCEL) return(FALSE) ;
          if (reply == FM_YES)
            {
              fm_snprintf(temp, sizeof(temp),
                      "%s -Wl \"Press return to quit.\" sh -c \"%s;read ans\"",
                      Fm->shellname, expanded_cmd) ;
              STRCPY(expanded_cmd, temp) ;
              if (fm_run_str(expanded_cmd, FALSE)) fm_showerr(expanded_cmd) ;
              return(TRUE) ;
            }
        }

      fm_busy_cursor(TRUE, wno) ;
      error = fm_run_str(cmd, FALSE) ;
      fm_busy_cursor(FALSE, wno) ;
      if (error)
        {
          fm_msg(TRUE, Str[(int) M_UNABLE_OPEN], fpo->name) ;
          return(FALSE) ;        /* Stop if error detected. */
        }
    }
  else if (fpo->type == FT_DIR)
    {
      fm_msg(FALSE, Str[(int) M_OPENING], fpo->name) ;
      Fm->isgoto_select = TRUE;
      if (wno == WASTE) use_subframe = FALSE ;
      SPRINTF(fname, "%s/%s", path, (char *) fpo->name) ;
      if (open_above_mount_point(wno, fname))
        {
          for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
          FM_NEW_FOLDER_WINDOW(path, (char *) fpo->name, NULL,
                               FM_DISK, 0, pos_given, info) ;
        }
      else if (use_subframe)
        {
          info[0] = info[1] = info[2] = info[5] = info[6] = -1 ;
          info[3] = x ;
          info[4] = y ;
          FM_NEW_FOLDER_WINDOW(path, (char *) fpo->name, NULL,
                               f->mtype, f->devno, pos_given, info) ;
        }
      else
        open_dir(fpo, wno) ;
      Fm->isgoto_select = FALSE;
      set_menu_items(FM_GOTO_MENU, set_goto_button_menu) ;
    }
  else
    {
      SPRINTF(cmd, Str[(int) M_NOOPEN_METHOD], fpo->name) ;
      (void) notice(cmd, Str[(int) M_OK], NULL) ;
      return(FALSE) ;      /* stop if error detected */
    }
  return(TRUE) ;
}


Tree_Pane_Object *
path_chosen(wno, x, y)            /* What path item was chosen? */
int wno, x, y ;
{
  Tree_Pane_Object *t_p ;

  if (y < MARGIN || y > (MARGIN + GLYPH_HEIGHT)) return(NULL) ;
  t_p = Fm->file[wno]->path_pane.current ;
  while (t_p)
    {
      if (x > t_p->Xpos && x < (t_p->Xpos + GLYPH_WIDTH)) break ;
      if (t_p == Fm->file[wno]->path_pane.head) return(NULL) ;
      t_p = t_p->parent ;
    }
  return(t_p) ;
}


Tree_Pane_Object *
path_to_node(wno, name)   /* Find the tree node for the given pathname. */
int wno ;
WCHAR *name ;
{
  WCHAR *n_p ;                            /* Name pointer. */
  Tree_Pane *tp ;
  Tree_Pane_Object *f_p, *parent ;        /* Tree pointers. */
  int more ;                              /* More of path name to come? */

  if (wno == WNO_TREE) tp = &Fm->tree ;
  else                 tp = &Fm->file[wno]->path_pane ;

  more = TRUE ;

  if (*name == '/')                       /* Begin at root? */
    {
      name++ ;
      if (*name == '\0')
        {
          more = FALSE ;
          f_p  = tp->root ;
        }
      else
        {
          f_p    = tp->root->child ;
          parent = tp->root ;
        }
    }
  else                     /* Begin at selected or current folder... */
    {
      f_p    = tp->selected ? tp->selected : tp->current ;
      parent = f_p ;
      f_p    = f_p->child ;
      if (f_p) f_p->status &= ~PRUNE ;
    }

  while (more)                                 /* Get next folder name... */
    {
      while (*name == '/') name++ ;            /* Skip extra /'s */
      n_p = name ;
      while (*n_p && *n_p != '/') n_p++ ;
      if ((more = (*n_p == '/')) != '\0') *n_p = '\0' ;
      else if (n_p == name) break ;            /* / at end. */

      for (; f_p; f_p = f_p->sibling)
        if (strcmp((char *) f_p->name, (char *) name) == 0) break ;

      if (f_p)                                 /* Found folder? */
        {
          if (more)
            {
              parent = f_p ;
              f_p = f_p->child ;
              if (f_p) f_p->status &= ~PRUNE ;
            }
          else break ;                         /* Last folder in path */
        }
      else                                     /* We have to make folder */
        {
          if ((f_p = make_tree_node(name)) ==  NULL) return(NULL) ;

/*  Assign parent some bogus time so it thinks we've seen this folder,
 *  (revisiting folder will expand it, but preserve existing children).
 */
          if (parent && parent->mtime == 0 && parent->child)
             parent->mtime = 10 ;

          STRCPY((char *) f_p->name, (char *) name) ;
          f_p->parent   = parent ;
          f_p->sibling  = parent->child ;
          f_p->child    = NULL ;
          f_p->status   = 0 ;
          f_p->mtime    = 0 ;
          parent->child = f_p ;
          parent        = f_p ;
        }
      if (more)
        {
          *n_p = '/' ;
          name = n_p + 1 ;
        }
    }
  return(f_p) ;
}


/*ARGSUSED*/
void
path_tree_select_item(ec)     /* Select an item in a path/tree pane. */
Event_Context ec ;
{
  BYTE dtype, stype ;
  Tree_Pane *tp ;
  Tree_Pane_Object *tpo ;
  int wno = ec->wno ;
  File_Pane *f ;

  if (wno == WNO_TREE)
    {
      tp = &Fm->tree ;
      dtype = (tp->lastsel   == tp->current) ? FT_DIR_OPEN : FT_DIR ;
      stype = (ec->tree_item == tp->current) ? FT_DIR_OPEN : FT_DIR ;
    }
  else
    {
      f  = Fm->file[wno] ;
      tp = &f->path_pane ;
      if (wno == WASTE)
        {
          if (tp->lastsel == tp->head)   dtype = FT_WASTE ;
          else dtype = (tp->lastsel == tp->current)   ? FT_DIR_OPEN : FT_DIR ;
          if (ec->tree_item == tp->head) stype = FT_WASTE ;
          else stype = (ec->tree_item == tp->current) ? FT_DIR_OPEN : FT_DIR ;
        }
      else if (f->mtype == FM_CD)
        {
          tpo = path_to_node(wno, (WCHAR *) Fm->cmntpt[f->devno]) ;
          if (tpo == tp->lastsel)   dtype = FT_CD ;
          else dtype = (tp->lastsel   == tp->current) ? FT_DIR_OPEN : FT_DIR ;
          if (tpo == ec->tree_item) stype = FT_CD ;
          else stype = (ec->tree_item == tp->current) ? FT_DIR_OPEN : FT_DIR ;
        }
      else if (f->mtype == FM_FLOPPY)
        {
          tpo = path_to_node(wno, (WCHAR *) Fm->fmntpt[f->devno]) ;
          if (tpo == tp->lastsel)   dtype = FT_FLOPPY ;
          else dtype = (tp->lastsel   == tp->current) ? FT_DIR_OPEN : FT_DIR ;
          if (tpo == ec->tree_item) stype = FT_FLOPPY ;
          else stype = (ec->tree_item == tp->current) ? FT_DIR_OPEN : FT_DIR ;
        }
      else if (f->mtype == FM_DOS)
        {
          tpo = path_to_node(wno, (WCHAR *) Fm->fmntpt[f->devno]) ;
          if (tpo == tp->lastsel)   dtype = FT_DOS ;
          else dtype = (tp->lastsel   == tp->current) ? FT_DIR_OPEN : FT_DIR ;
          if (tpo == ec->tree_item) stype = FT_DOS ;
          else stype = (ec->tree_item == tp->current) ? FT_DIR_OPEN : FT_DIR ;
        }
      else
        {
          dtype = (tp->lastsel   == tp->current) ? FT_DIR_OPEN : FT_DIR ;
          stype = (ec->tree_item == tp->current) ? FT_DIR_OPEN : FT_DIR ;
        }
    }
  draw_folder(wno, tp->lastsel,   dtype, FALSE) ;
  draw_folder(wno, ec->tree_item, stype, TRUE) ;
}


void
remove_tree_entry(node, name)   /* Remove tree entry from below this node. */
Tree_Pane_Object *node ;
WCHAR *name ;
{
  Tree_Pane_Object *f_p ;               /* Pointer to current tree node. */
  Tree_Pane_Object *l_p = NULL ;        /* Pointer to previous tree node. */

  f_p = node->child ;
  while (f_p != NULL)
    {
      if (!strcmp((char *) f_p->name, (char *) name))
        {

/*  We've found the entry that needs removing. If the previous tree node
 *  pointer is NULL, then this node is the first in the sibling chain, and
 *  we have to set the found nodes parents child node pointer to point to
 *  the found nodes sibling pointer.
 *
 *  If the previous tree node pointer isn't NULL, then we are part way
 *  down the chain, and so we need to set the previous nodes sibling pointer
 *  to the found nodes sibling pointer.
 *
 *  Confused?  You won't be after this weeks episode of ...
 */
          if (l_p == NULL) f_p->parent->child = f_p->sibling ;
          else l_p->sibling = f_p->sibling ;

          FREE(f_p->name) ;             /* Remove node. */
          FREE((char *) f_p) ;
	  f_p = NULL;
          Fm->redraw_tree = TRUE ;
          return ;
        }
      l_p = f_p ;
      f_p = f_p->sibling ;
    }
}


void
resize_history(f, n)   /* Resize directory history cache. */
int f ;                /* True if first time. Malloc, else realloc. */
int n ;                /* Initial size or increment. */
{
  int old ;            /* Old size of the array. */

  if (f)
    {
      old = 0 ;
      Fm->maxhistory = n ;
    }
  else
    {
      old = Fm->maxhistory ;
      Fm->maxhistory += n ;
    }

  Fm->history = (WCHAR **) LINT_CAST(make_mem((char *) Fm->history,
                           sizeof(WCHAR **) * old, sizeof(WCHAR **) * n, f)) ;
}


void
reverse_ith(off, wno)            /* Reverse file pane object on canvas. */
int off, wno ;
{
  File_Pane_Object **f_p = Fm->file[wno]->object + off ;
  int selected           = (*f_p)->selected ;

  (*f_p)->selected = !(*f_p)->selected ;
  draw_ith(off, wno) ;
  (*f_p)->selected = selected ;
}


void
select_item(ec, unique)       /* Select an item on the file pane. */
Event_Context ec ;
enum select_type unique ;
{
  int item     = ec->item ;
  int wno      = ec->wno ;
  File_Pane *f = Fm->file[wno] ;

  fm_pathdeselect(wno) ;
  if (unique == UNIQUE_SELECTION)
    {
      File_Pane_Object **curr, **last ;       /* Files array pointers. */
      int i ;

/* Deselect all files first, except for item we are over. */

      last = PTR_LAST(wno) ;
      for (curr = PTR_FIRST(wno), i = 0; curr < last; curr++, i++)
        if ((*curr)->selected && *curr != f->object[item])
          {
            (*curr)->selected = FALSE ;
            draw_ith(i, wno) ;
          }
    }

/* Make sure the item we are over is selected. */

  if (!f->object[item]->selected)
    {
      f->object[item]->selected = TRUE ;
      draw_ith(item, wno) ;
    }
}


void
set_cur_folder_panel()
{
  File_Pane *f = Fm->file[Fm->curr_wno] ;

  set_item_int_attr(FM_PO_CURF_NAMELEN, FM_ITEM_IVALUE, f->num_file_chars) ;
  set_item_int_attr(FM_PO_CURF_DISPLAY, FM_ITEM_IVALUE,
                    (int) f->display_mode) ;
  set_item_int_attr(FM_PO_CURF_CONTENT, FM_ITEM_IVALUE, f->content_types) ;
  set_item_int_attr(FM_PO_CURF_HIDDEN,  FM_ITEM_IVALUE, (f->see_dot ? 1 : 0)) ;
  set_item_int_attr(FM_PO_CURF_LISTOPTS, FM_ITEM_IVALUE, f->listopts) ;
  set_item_int_attr(FM_PO_CURF_LAYOUT,   FM_ITEM_IVALUE, !f->dispdir) ;
  set_item_int_attr(FM_PO_CURF_SORTBY,   FM_ITEM_IVALUE, f->sortby) ;
}


/*  Sets the current state of the event, and cleans up any previous
 *  states which were in the middle of doing something.
 */

void
set_event_state(ec, newstate)
Event_Context ec ;
enum state_type newstate ;
{

/*  A folder repaint may cause the rename op to stop -- correct the
 *  event context in this case.
 */

  if (!Fm->Renaming && ec->state == RENAMING) ec->state = CLEAR ;

  if (newstate == ec->state) return ;      /* No change. */

#ifdef DEBUG_EVENT
  switch (newstate)
    {
      case MAYBE_DRAG    : PUTS("MAYBE_DRAG state") ;    break ;
      case MAYBE_PAN     : PUTS("MAYBE_PAN state") ;     break ;
      case MAYBE_MARQUIS : PUTS("MAYBE_MARQUIS state") ; break ;
      case PAN           : PUTS("PAN state") ;           break ;
      case MARQUIS       : PUTS("MARQUIS state") ;       break ;
      case CLEAR         : PUTS("CLEAR state") ;         break ;
      case INCR_SEL      : PUTS("INCR_SEL state") ;      break ;
      case RENAMING      : PUTS("RENAMING state") ;      break ;
      case RESIZING      : PUTS("RESIZING state") ;      break ;
      case DND_OVER_ITEM : PUTS("DND_OVER_ITEM state") ; break ;
      case OPENING       : PUTS("OPENING state") ;       break ;
    }
#endif /*DEBUG_EVENT*/

  switch (ec->state)
    {
      case PAN           :
      case CLEAR         :
      case MAYBE_DRAG    :
      case MAYBE_PAN     :
      case MAYBE_MARQUIS :
      case DND_OVER_ITEM : fm_clrmsg() ;
                           break ;

      case MARQUIS :
        if (newstate == MAYBE_MARQUIS)
          {
            finish_marquis_selection(ec) ;      /* Erase box. */
            ec->start_x = ec->old_x = ec->x ;   /* Record starting x. */
            ec->start_y = ec->old_x = ec->y ;   /* and y position. */
          }
        break ;

      case INCR_SEL :                           /* Clear incr search. */
        incremental_selection(ec, CLEAR_SEL, ec->ch) ;
        break ;

      case RENAMING      : stop_rename() ;
                           break ;

      case OPENING       : /* Do not clear the opening message. */
                           break ;
    }

/*  The state machine does not know if the user is renaming a NewFolder
 *  or NewDocument, so check the global boolean Fm->Renaming. If
 *  Fm->Renaming, then stop it.
 */

  if (Fm->Renaming && ec->state != RENAMING) stop_rename() ;
  ec->state = newstate ;
}


/* Set the X coordinate of the file pane object depending upon mode. */

void
set_file_obj_x(wno, fpo, value)
int wno, value ;
File_Pane_Object *fpo ;
{
  short dmode = Fm->file[wno]->display_mode ;

       if (dmode == VIEW_BY_ICON)    fpo->ix = value ;
  else if (dmode == VIEW_BY_CONTENT) fpo->cx = value ;
  else if (Fm->file[wno]->listopts)  fpo->ox = value ;
  else                               fpo->lx = value ;
}


/* Set the Y coordinate of the file pane object depending upon mode. */

void
set_file_obj_y(wno, fpo, value)
int wno, value ;
File_Pane_Object *fpo ;
{
  short dmode = Fm->file[wno]->display_mode ;

       if (dmode == VIEW_BY_ICON)    fpo->iy = value ;
  else if (dmode == VIEW_BY_CONTENT) fpo->cy = value ;
  else if (Fm->file[wno]->listopts)  fpo->oy = value ;
  else                               fpo->ly = value ;
}


void
set_print_method()     /* Set print method for selected item(s) in popup. */
{
  File_Pane_Object **f_p, **l_p ;    /* Files array pointers. */
  char *pm         = NULL ;          /* Pointer to print method. */
  Boolean found    = FALSE ;
  Boolean multiple = FALSE ;         /* Set true for multiple selections. */

  l_p = PTR_LAST(Fm->curr_wno) ;
  for (f_p = PTR_FIRST(Fm->curr_wno); f_p != l_p; f_p++)
    if ((*f_p)->selected)
      {
        pm = get_print_method(*f_p) ;
        if (found == TRUE)
          {
            multiple = TRUE ;
            break ;
          }
        else found = TRUE ;
      }
  if (pm && multiple == FALSE)
    set_item_str_attr(FM_CP_METHOD_ITEM, FM_ITEM_IVALUE, pm) ;
  else
    set_item_str_attr(FM_CP_METHOD_ITEM, FM_ITEM_IVALUE, "") ;
}


void
sort_tree(f_p)
Tree_Pane_Object *f_p ;
{
  Fm->sort_tp = f_p ;
  qsort((char *) Fm->tpo, Fm->ntpo,
        sizeof(Tree_Pane_Object *), compare_tree_order) ;
}


void
toggle_item(ec, unique)      /* Select/Unselect an item on the file pane. */
Event_Context ec ;
enum select_type unique ;
{
  int item     = ec->item ;
  int wno      = ec->wno ;
  File_Pane *f = Fm->file[wno] ;

  fm_pathdeselect(wno) ;
  if (unique == UNIQUE_SELECTION)
    fm_filedeselect(wno) ;        /* Deselect all files first. */

  f->object[item]->selected = !f->object[item]->selected ;
  draw_ith(item, wno) ;
}


void
tree_attach_parent(t, h)
Tree_Pane_Object *t ;
int h ;
{
  int x, y1, y2 ;

  x  = Fm->tree.border + Fm->tree_gap ;
  if (Fm->tree_vertical)
    {
      y2 = (h - t->width) / 2 - Fm->tree.border ;
      y1 = y2 + t->width + 2 * Fm->tree.border - h ;
      t->child->Xoff = x + Fm->tree.height ;
    }
  else
    {
      y2 = (h - Fm->tree.height) / 2 - Fm->tree.border ;
      y1 = y2 + Fm->tree.height + 2 * Fm->tree.border - h ;
      t->child->Xoff = x + t->width ;
    }
  t->child->Yoff = y1 ;
  if (Fm->tree_vertical)
    {
      t->contour.upper.head =
        tree_line(Fm->tree.height, 0, tree_line(x, y1, t->contour.upper.head)) ;
      t->contour.lower.head =
        tree_line(Fm->tree.height, 0, tree_line(x, y2, t->contour.lower.head)) ;
    }
  else
    {
      t->contour.upper.head =
        tree_line(t->width, 0, tree_line(x, y1, t->contour.upper.head)) ;
      t->contour.lower.head =
        tree_line(t->width, 0, tree_line(x, y2, t->contour.lower.head)) ;
    }
}


Tree_polyline *
tree_bridge(line1, x1, y1, line2, x2, y2)
Tree_polyline *line1, *line2 ;
int x1, y1, x2, y2 ;
{
  int dx, dy, s ;
  Tree_polyline *r ;

  dx = x2 + line2->dx - x1 ;
  if (line2->dx == 0) dy = line2->dy ;
  else
    {
      s = dx * line2->dy ;
      dy = s / line2->dx ;
    }
  r = tree_line(dx, dy, line2->link) ;
  line1->link = tree_line(0, y2 + line2->dy - dy - y1, r) ;
  return(r) ;
}


void
tree_calculate_dims(t)
Tree_Pane_Object *t ;
{
  Tree_Pane_Object *c ;
  int curXpos, curYpos ;

  for (c = t->child; c; c = c->sibling)
    {
      if (c == t->child)
        {
          if (Fm->tree_vertical)
            {
              c->Xpos = curXpos = t->Xpos + c->Yoff ;
              c->Ypos = curYpos = t->Ypos + c->Xoff ;
            }
          else
            {
              c->Xpos = curXpos = t->Xpos + c->Xoff ;
              c->Ypos = curYpos = t->Ypos + c->Yoff ;
            }
        }
      else
        {
          if (Fm->tree_vertical)
            {
              curXpos = c->Xpos = curXpos + c->Yoff ;
              curYpos = c->Ypos = curYpos + c->Xoff ;
            }
          else
            {
              curXpos = c->Xpos = curXpos + c->Xoff ;
              curYpos = c->Ypos = curYpos + c->Yoff ;
            }
        }
      if (Fm->tree_vertical)
        {
          if (curYpos + Fm->tree.height > Fm->tree_height)
            Fm->tree_height = curYpos + Fm->tree.height ;
          if (curXpos < Fm->tree_Ymin) Fm->tree_Ymin = curXpos ;
          if (curXpos + c->width > Fm->tree_Ymax)
            Fm->tree_Ymax = curXpos + c->width ;
        }
      else
        {
          if (curXpos + c->width > Fm->tree_width)
            Fm->tree_width = curXpos + c->width ;
          if (curYpos < Fm->tree_Ymin) Fm->tree_Ymin = curYpos ;
          if (curYpos + Fm->tree.height > Fm->tree_Ymax)
            Fm->tree_Ymax = curYpos + Fm->tree.height ;
        }
      tree_calculate_dims(c) ;
    }
}


/*  Check to see if over an item . Sets the appropiate fields of the
 *  Event Context.
 */

Event_Context
tree_check_if_over_item(ec)
Event_Context ec ;
{
  int wno = ec->wno ;
  File_Pane *f ;

  if (wno == WNO_TREE)
    {
      ec->tree_item = mouse(ec->x, ec->y, Fm->tree.head) ;
      if (ec->tree_item && ec->down)
        {
          Fm->tree.lastsel  = Fm->tree.selected ;
          Fm->tree.selected = ec->tree_item ;
        }
    }
  else
    {
      f = Fm->file[wno] ;
      ec->tree_item = path_chosen(wno, ec->x, ec->y) ;
      if (ec->tree_item && ec->down)
        {
          f->path_pane.lastsel  = f->path_pane.selected ;
          f->path_pane.selected = ec->tree_item ;
        }
    }

  if (ec->tree_item != NULL) ec->over_item = TRUE ;
  else                       ec->over_item = FALSE ;
  return(ec) ;
}


int
tree_join(t)
Tree_Pane_Object *t ;
{
  Tree_Pane_Object *c ;
  int d, h, sum ;

  c = t->child ;
  t->contour = c->contour ;
  if (Fm->tree_vertical)
    sum = h = c->width + 2 * Fm->tree.border ;
  else
    sum = h = Fm->tree.height +  2 * Fm->tree.border ;
  c = c->sibling ;
  for ( ; c; c = c->sibling)
    {
      d = tree_merge(&t->contour, &c->contour) ;
      c->Xoff = 0 ;
      c->Yoff = d + h ;
      if (Fm->tree_vertical)
        h = c->width + 2 * Fm->tree.border ;
      else
        h = Fm->tree.height + 2 * Fm->tree.border ;
      sum += d + h ;
    }
  return(sum) ;
}


void
tree_layout(t)
Tree_Pane_Object *t ;
{
  Tree_Pane_Object *c ;

  t->Xoff = t->Yoff = 0 ;
  MEMSET((char*) &t->contour, 0, sizeof(struct Tree_polygon)) ;

  for (c = t->child; c; c = c->sibling) tree_layout(c) ;
  if (t->child) tree_attach_parent(t, tree_join(t)) ;
  else          tree_layout_leaf(t) ;
}


void
tree_layout_leaf(t)
Tree_Pane_Object *t ;
{
  if (Fm->tree_vertical)
    {
      t->contour.upper.tail =
        tree_line(Fm->tree.height + 2 * Fm->tree.border, 0, 0) ;
      t->contour.upper.head = t->contour.upper.tail ;
      t->contour.lower.tail = tree_line(0, -t->width - 2 * Fm->tree.border, 0) ;
      t->contour.lower.head =
        tree_line(Fm->tree.height + 2 * Fm->tree.border, 0,
                                       t->contour.lower.tail) ;
    }
  else
    {
      t->contour.upper.tail = tree_line(t->width + 2 * Fm->tree.border, 0, 0) ;
      t->contour.upper.head = t->contour.upper.tail ;
      t->contour.lower.tail =
        tree_line(0, -Fm->tree.height - 2 * Fm->tree.border, 0) ;
      t->contour.lower.head =
        tree_line(t->width + 2 * Fm->tree.border, 0, t->contour.lower.tail) ;
    }
}


Tree_polyline *
tree_line(dx, dy, link)
int dx, dy ;
Tree_polyline *link ;
{
  int i ;
  Tree_polyline *val ;

  if (Fm->ntreelines >= Fm->maxtreelines)
    {
      Fm->tlines = (Tree_polyline **)
        LINT_CAST(make_mem((char *) Fm->tlines,
              sizeof(Tree_polyline **) * Fm->maxtreelines,
              sizeof(Tree_polyline **) * (Fm->maxtreelines + TLINE_ALLOC_INC),
              (Fm->maxtreelines == 0))) ;
      for (i = 0; i < TLINE_ALLOC_INC; i++)
        Fm->tlines[Fm->maxtreelines + i] = (Tree_polyline *)
          fm_malloc((unsigned int) sizeof(Tree_polyline)) ;

      Fm->maxtreelines += TLINE_ALLOC_INC ;
    }

  val = Fm->tlines[Fm->ntreelines++] ;

  val->dx   = dx ;
  val->dy   = dy ;
  val->link = link ;
  return(val) ;
}


int
tree_merge(c1, c2)
struct Tree_polygon *c1, *c2 ;
{
  int x, y, total, d ;
  Tree_polyline *lower, *upper, *b ;

  x = y = total = 0 ;
  upper = c1->lower.head ;
  lower = c2->upper.head ;

  while (lower && upper)         /* Compute offset total. */
    {
      d = tree_offset(x, y, lower->dx, lower->dy, upper->dx, upper->dy) ;
      y += d ;
      total += d ;

      if (x + lower->dx <= upper->dx)
        {
          y += lower->dy ;
          x += lower->dx ;
          lower = lower->link ;
        }
      else
        {
          y -= upper->dy ;
          x -= upper->dx ;
          upper = upper->link ;
        }
    }

  if (lower)                     /* Store result in c1 */
    {
      b = tree_bridge(c1->upper.tail, 0, 0, lower, x, y) ;
      c1->upper.tail = (b->link) ? c2->upper.tail : b ;
      c1->lower.tail = c2->lower.tail ;
    }
  else                           /* Upper. */
    {
      b = tree_bridge(c2->lower.tail, x, y, upper, 0, 0) ;
      if (!b->link) c1->lower.tail = b ;
    }
  c1->lower.head = c2->lower.head ;
  return(total) ;
}


int
tree_offset(p1, p2,  a1, a2, b1, b2)
int p1, p2, a1, a2, b1, b2 ;
{
  int d, s, t ;

  if (b1 <= p1 || p1 + a1 <= 0) return(0) ;

  t = b1 * a2 - a1 * b2 ;
  if (t > 0)
    if (p1 < 0)
      {
        s = p1 * a2 ;
        d = s / a1 - p2 ;
      }
    else if (p1 > 0)
      {
        s = p1 * b2 ;
        d = s / b1 - p2 ;
      }
    else d = -p2 ;
  else
    if (b1 < p1 + a1)
      {
        s = (b1 - p1) * a2 ;
        d = b2 - (p2 + s / a1) ;
      }
    else if (b1 > p1 + a1)
      {
        s = (a1 + p1) * b2 ;
        d = s / b1 - (p2 + a2) ;
      }
    else  d = b2 - (p2 + a2) ;
  return(MAX(0, d)) ;
}


Boolean
valid_filename(file)
char *file ;
{
  register char *cp, *ip ;    /* Current ptr, invalid char ptr. */
  char invalid_chars[16] ;    /* Array of invalid filename chars. */

  if (!file || isempty((WCHAR *) file))
    {
      fm_clrmsg() ;           /* Clear "renaming..." prompt. */
      return(FALSE) ;         /* No error message for this. */
    }

/*  Check for invalid chars in the filename.  In UNIX, there seems
 *  to be only two chars which do this: NULL and slash.  The
 *  following code is overkill, but left in just in case we want
 *  to add other chars to filter.
 */

  SPRINTF(invalid_chars, "/") ;
  for (cp = file; *cp; cp++)
    {
      for (ip = invalid_chars; *ip && *ip != *cp; ip++) continue ;

      if (*ip == *cp)
        {
          if (isprint(*ip)) fm_msg(TRUE, Str[(int) M_BADCHAR], *ip) ;
          else              fm_msg(TRUE, Str[(int) M_INVCHAR]) ;

          return(FALSE) ;
        }
    }
  return(TRUE) ;
}


Boolean
valid_print_cmd()
{
  char *method = get_item_str_attr(FM_CP_METHOD_ITEM, FM_ITEM_IVALUE) ;

/* Make sure print command is not empty. */

  if (!method || isempty((WCHAR *) method))
    {
      fm_msg(TRUE, Str[(int) M_ENTER_PMETHOD]) ;
      return(FALSE) ;
  }

/* Check UNIX command for $FILE */

  if (!find_pattern(method, "$FILE"))           /* Check for $FILE */
    {
      int choice ;
      char buf[256] ;

/* Give warning and get user's choice. */

      choice = give_file_warning() ;

           if (choice == FILE_CANCEL) return(FALSE) ;      /* User cancelled */
      else if (choice == FILE_ADDFILE)
        {
          SPRINTF(buf, "%s $FILE", method) ;       /* User wants $FILE added */
          set_item_str_attr(FM_CP_METHOD_ITEM, FM_ITEM_IVALUE, buf) ;
        }
      else if (choice != FILE_CONTINUE)                   /* User continued? */
        ERR_EXIT(Str[(int) M_INV_NOTICE]) ;
    }
  return(TRUE) ;
}
