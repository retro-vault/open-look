#ifndef lint
static char sccsid[]="@(#)functions.c	1.131 04/18/97 Copyright 1987-1991 Sun Microsystem, Inc." ;
#endif

/*  Copyright (c) 1987-1991 Sun Microsystems, Inc.
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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
/* Linux portability definitions are in linux_port.h. */
#include "defs.h"
#include "fm.h"

extern FmVars Fm ;
extern char *Str[] ;


void
apply_advanced_panel(redisplay, drawtree)
int *redisplay, *drawtree ;
{
  int wno = Fm->curr_wno ;
  char *s_p ;                    /* Compare old value. */

/* Properties that would make us refresh the screen. */

  s_p = get_item_str_attr(FM_PO_FILTER_I, FM_ITEM_IVALUE) ;
  if (strcmp((char *) Fm->filter, s_p))
    {
      *redisplay = FM_BUILD_FOLDER ;
      *drawtree  = TRUE ;
      STRCPY((char *) Fm->filter, s_p) ;
      STRCPY((char *) Fm->file[wno]->filter, (char *) Fm->filter) ;
      set_pos_attr(wno, FM_POS_POS, FALSE) ;
    }

/* Properties that won't affect folder display. */

  Fm->follow_links = !get_item_int_attr(FM_PO_LINK_I,     FM_ITEM_IVALUE) ;
  Fm->show_cd      = !get_item_int_attr(FM_PO_CDROM_I,    FM_ITEM_IVALUE) ;

  Fm->interval =  get_item_int_attr(FM_PO_INTERVAL_I, FM_ITEM_IVALUE) ;
  if (Fm->interval < 1)    Fm->interval = 1 ;
  if (Fm->interval > 9999) Fm->interval = 9999 ;
  set_item_int_attr(FM_PO_INTERVAL_I, FM_ITEM_IVALUE, Fm->interval) ;
  set_timer(Fm->interval) ;

  Fm->cd_content   = !get_item_int_attr(FM_PO_CD_CONTENT, FM_ITEM_IVALUE) ;
  Fm->floppy_content = !get_item_int_attr(FM_PO_FLOPPY_CONTENT,
                                          FM_ITEM_IVALUE) ;

  STRCPY((char *) Fm->print_script,
         get_item_str_attr(FM_PO_PRINT_I, FM_ITEM_IVALUE)) ;

  Fm->editor = (short) get_item_int_attr(FM_PO_EDITOR_I, FM_ITEM_IVALUE) ;
  ZFREE(Fm->other_editor) ;
  Fm->other_editor = strdup(get_item_str_attr(FM_PO_OTHER_EDITOR_I,
                                              FM_ITEM_IVALUE)) ;
}


void
apply_general_panel(drawtree)
int *drawtree ;
{
  char *ndoc ;
  int i ;

  *drawtree = FALSE ;

  Fm->newwin = get_item_int_attr(FM_PO_OPEN_FOLDER, FM_ITEM_IVALUE) ;
  i = !get_item_int_attr(FM_PO_TREE_DIR, FM_ITEM_IVALUE) ; ;
  if (i != Fm->tree_vertical)
    {
      *drawtree = TRUE ;
      Fm->tree_vertical = i ;
    }

  Fm->confirm_delete = !(Boolean)
                       get_item_int_attr(FM_PO_DELETE_I, FM_ITEM_IVALUE) ;

  if (Fm->confirm_delete) {
	fm_create_wastebasket() ;
  	set_frame_attr(WASTE, FM_FRAME_SHOW, TRUE) ;
  }
  else                    (void)quit_waste() ;

  ZFREE(Fm->newdoc) ;
  Fm->newdoc = strdup(get_item_str_attr(FM_PO_DEFDOC, FM_ITEM_IVALUE)) ;
  if (!strlen(Fm->newdoc))
    {
      read_str((char **) &Fm->newdoc, Str[(int) M_NEWDOCUMENT]) ;
      set_item_str_attr(FM_PO_DEFDOC, FM_ITEM_IVALUE, Fm->newdoc) ;
    }
  set_item_int_attr(FM_PO_DEL_MES, FM_ITEM_SHOW, TRUE) ;
  set_item_int_attr(FM_PO_DES_MES, FM_ITEM_SHOW, TRUE) ;
}


void
apply_prop_values(value)
int value ;
{
  int wno       = Fm->curr_wno ;
  int drawtree  = FALSE ;        /* Set if the tree view needs redrawing. */
  int redisplay = FALSE ;        /* Set if we need to redisplay folder. */

/* Check to see if the values are valid. */

  if (!po_valid_values(value))
    {

/* Do this so the possibly unpinned popup will not be dismissed. */

      keep_panel_up(FM_PO_PO_FRAME) ;
      return ;                  /* Error message already given. */
    }

  write_item_footer(FM_PO_PO_FRAME, "", FALSE) ;
  switch (value)
    {
      case PROPS_STACK_GENERAL    : apply_general_panel(&drawtree) ;
                                    break ;
      case PROPS_STACK_NEW_FOLDER : apply_new_folder_panel() ;
                                    break ;
      case PROPS_STACK_CUR_FOLDER : apply_cur_folder_panel(&redisplay,
                                                           &drawtree) ;
                                    break ;
      case PROPS_STACK_GOTO       : apply_goto_panel() ;
                                    break ;
      case PROPS_STACK_CUSTOM     : apply_custom_panel() ;
                                    break ;
      case PROPS_STACK_ADVANCED   : apply_advanced_panel(&redisplay,
                                                         &drawtree) ;
    }

/* Reread directory if change. */

  if (redisplay)
    {
      fm_msg(FALSE, Str[(int) M_REDRAWING]) ;
      if (is_frame(wno))
        {
          fm_busy_cursor(TRUE, wno) ;
          fm_display_folder(redisplay, wno) ;
          fm_busy_cursor(FALSE, wno) ;
        }
    }
  if (drawtree)
    {
      fm_scrollbar_scroll_to(WNO_TREE, FM_H_SBAR, 0) ;
      fm_scrollbar_scroll_to(WNO_TREE, FM_V_SBAR, 0) ;
      if (Fm->treeview) do_show_tree(FALSE, FALSE) ;
    }
  fm_clrmsg() ;
  write_cmdline() ;
}


void
apply_new_folder_panel()
{
  int i ;

  ZFREE(Fm->newdir) ;
  Fm->newdir = strdup(get_item_str_attr(FM_PO_NEWF_DEFNAME, FM_ITEM_IVALUE)) ;
  if (!strlen(Fm->newdir))
    {
      read_str((char **) &Fm->newdir, Str[(int) M_NEWFOLDER]) ;
      set_item_str_attr(FM_PO_NEWF_DEFNAME, FM_ITEM_IVALUE, Fm->newdir) ;
    }

  i = get_item_int_attr(FM_PO_NEWF_NAMELEN, FM_ITEM_IVALUE) ;
  if (i < 1)   set_item_int_attr(FM_PO_NEWF_NAMELEN, FM_ITEM_IVALUE, 1) ;
  if (i > 255) set_item_int_attr(FM_PO_NEWF_NAMELEN, FM_ITEM_IVALUE, 255) ;

  Fm->num_file_chars = get_item_int_attr(FM_PO_NEWF_NAMELEN,  FM_ITEM_IVALUE) ;
  Fm->dispdir        = !get_item_int_attr(FM_PO_NEWF_LAYOUT,  FM_ITEM_IVALUE) ;
  Fm->display_mode   = get_item_int_attr(FM_PO_NEWF_DISPLAY,  FM_ITEM_IVALUE) ;
  Fm->content_types  = get_item_int_attr(FM_PO_NEWF_CONTENT,  FM_ITEM_IVALUE) ;
  Fm->sortby         = get_item_int_attr(FM_PO_NEWF_SORTBY,   FM_ITEM_IVALUE) ;
  Fm->see_dot = (get_item_int_attr(FM_PO_NEWF_HIDDEN, FM_ITEM_IVALUE) ? 1 : 0) ;
  Fm->listopts       = get_item_int_attr(FM_PO_NEWF_LISTOPTS, FM_ITEM_IVALUE) ;
}


void
apply_cur_folder_panel(redisplay, drawtree)
int *redisplay, *drawtree ;
{
  int i ;
  int wno       = Fm->curr_wno ;
  File_Pane *f  = Fm->file[wno] ;

  *redisplay = FM_BUILD_FOLDER ;

/* Check display toggle. */

  i = get_item_int_attr(FM_PO_CURF_DISPLAY, FM_ITEM_IVALUE) ;
  if (i != f->display_mode)
    {
      f->display_mode = i ;
      *redisplay      = FM_STYLE_FOLDER ;
    }

/* Check longest filename length. */

  i = get_item_int_attr(FM_PO_CURF_NAMELEN, FM_ITEM_IVALUE) ;
  if (i < 1)   set_item_int_attr(FM_PO_CURF_NAMELEN, FM_ITEM_IVALUE, 1) ;
  if (i > 255) set_item_int_attr(FM_PO_CURF_NAMELEN, FM_ITEM_IVALUE, 255) ;

  i = get_item_int_attr(FM_PO_CURF_NAMELEN, FM_ITEM_IVALUE) ;
  if (i != f->num_file_chars)
    {
      f->num_file_chars = i ;
      *redisplay        = FM_STYLE_FOLDER ;
    }

/* Check icon layout toggle. */

  i = !get_item_int_attr(FM_PO_CURF_LAYOUT, FM_ITEM_IVALUE) ;
  if (i != f->dispdir)
    {
      f->dispdir = i ;
      *redisplay = FM_STYLE_FOLDER ;
    }

/* Check view by content choices. */

  i = get_item_int_attr(FM_PO_CURF_CONTENT, FM_ITEM_IVALUE) ;
  if (i != f->content_types)
    {
      *redisplay       = FM_BUILD_FOLDER ;
      f->content_types = i ;
    }

/* Check sort toggle. */

  i = get_item_int_attr(FM_PO_CURF_SORTBY, FM_ITEM_IVALUE) ;
  if (i != f->sortby)
    {
      f->sortby  = i ;
      *redisplay = FM_STYLE_FOLDER ;
    }

/* Check hidden toggle. */

  i = get_item_int_attr(FM_PO_CURF_HIDDEN, FM_ITEM_IVALUE) ;
  if (i != (f->see_dot ? 1 : 0))
    {
      f->see_dot = (i ? 1 : 0) ;
      if (get_pos_attr(wno, FM_POS_MOVED)) *redisplay = FM_BUILD_FOLDER ;
      else
        {
          toggle_hidden_files(wno) ;
          *redisplay = FM_STYLE_FOLDER ;
        }
      *drawtree = TRUE ;
    }

/* Check list options choices. */

  i = get_item_int_attr(FM_PO_CURF_LISTOPTS, FM_ITEM_IVALUE) ;
  if (i != f->listopts)
    {
      if (*redisplay == FM_DISPLAY_FOLDER) *redisplay = FM_STYLE_FOLDER ;
      f->listopts = i ;
    }

/* save the properties into .state file */
  if (Fm->load_state)
    {
      char fname[MAXPATHLEN] ;
      SPRINTF(fname, "%s/.fm%s/.state", Fm->home, f->path) ;
      load_dir_defs(fname) ;
      write_dir_defs(wno) ;
      save_dir_defs(fname) ;
    }

/*  If we need to redisplay, then clear the position and moved indicators for
 *  this display mode.
 */

  if (*redisplay == FM_BUILD_FOLDER || *redisplay == FM_STYLE_FOLDER)
    {
      set_pos_attr(wno, FM_POS_POS,   FALSE) ;
      set_pos_attr(wno, FM_POS_MOVED, FALSE) ;
    }

/* Set the scroll height (also used for repaint procs). */

  if (is_frame(wno))
    {
      fm_scrollbar_scroll_to(wno, FM_V_SBAR, 0) ;
      fm_scrollbar_scroll_to(wno, FM_H_SBAR, 0) ;
    }
}


/* This is a shell to regex_match to return only a true BOOLEAN value. */

int
b_match(name, pattern)
char *name, *pattern ;
{
 if (regex_match(pattern, name) == TRUE )
	return TRUE;
 if (fm_match(name, pattern))
	return TRUE;
 return FALSE;
}


/*  If this rect is different from the stored one for this window, save the
 *  new one, and use as the basis for recreating the drop sites for this
 *  window.
 */

void
check_viewable_rect(wno, x, y, width, height)
int wno, x, y, width, height ;
{
  if (x     != Fm->file[wno]->r_left  || y      != Fm->file[wno]->r_top ||
      width != Fm->file[wno]->r_width || height != Fm->file[wno]->r_height)
    {
      Fm->file[wno]->r_left   = x ;
      Fm->file[wno]->r_top    = y ;
      Fm->file[wno]->r_width  = width ;
      Fm->file[wno]->r_height = height ;
      resize_window_drop_sites(wno, FALSE) ;
    }
}

void
do_check_floppy()
{
    	set_timer(20);

	if (!volmgt_check(NULL)) {
		fm_showerr("volcheck");
		write_footer(Fm->curr_wno, Str[M_NOFLOPPY], TRUE);
    		Fm->file[Fm->curr_wno]->Message = TRUE;
		return;
	}
    	fm_clrmsg();
	if (Fm->show_floppy)
		check_media(FM_FLOPPY, MAXFLOPPY, &Fm->floppy_mask, 0);
}


void
do_goto_list_deselect(row)
int row ;
{
  char *alias    = get_item_str_attr(FM_PO_GOTO_MLABEL, FM_ITEM_IVALUE) ;
  char *pathname = get_item_str_attr(FM_PO_PATHNAME,    FM_ITEM_IVALUE) ;
  int selected   = get_list_first_selected(FM_PO_GOTO_LIST) ;

  if (selected != -1 && valid_goto_name(pathname))
    {
      if (alias == NULL || alias[0] == '\0') alias = pathname ;
      change_goto_list_item(row, alias, pathname) ;
      set_item_str_attr(FM_PO_PATHNAME,    FM_ITEM_IVALUE, "") ;
      set_item_str_attr(FM_PO_GOTO_MLABEL, FM_ITEM_IVALUE, "") ;
    }
}


void
do_goto_list_select(row, label, pathname)
int row ;
char *label, *pathname ;
{

/* Can't change or remove the "Home" entry. */

  if (!row)
    {
      write_item_footer(FM_PO_PO_FRAME, Str[(int) M_NO_HOME_CHANGE], TRUE) ;
      select_list_item(FM_PO_GOTO_LIST, row, FALSE) ;
      Fm->ignore_goto_desel = TRUE ;
    }
  else
    {
      Fm->goto_chosen = row ;
      set_item_str_attr(FM_PO_PATHNAME,    FM_ITEM_IVALUE, strdup(pathname)) ;
      set_item_str_attr(FM_PO_GOTO_MLABEL, FM_ITEM_IVALUE, label) ;
      Fm->ignore_goto_desel = FALSE ;
    }
}


void
do_goto_menu(wno, s)
int wno ;
char *s ;
{
  char dir[MAXPATHLEN], *str_p ;

/*  If the user has clicked SELECT on the Goto button, and if there's
 *  something on the path text item, try to go there.
 */

  if (Fm->isgoto_select == TRUE && wno != WNO_TREE)
    {
      Fm->isgoto_select = FALSE ;
      if ((str_p = get_path_item_value(wno)) && *str_p)
        {
          goto_object() ;
          return ;
        }
    }

  if (strcmp(s, Str[(int) M_HOME]) == 0)
    ds_expand_pathname("$HOME", dir) ;     /* Get path of user's home. */
  else
    STRCPY(dir, s) ;                       /* Goto previous dir. */

  if (wno == WNO_TREE)
    {
      Fm->tree.selected = path_to_node(WNO_TREE, (WCHAR *) dir) ;
      fm_open_folder(TRUE) ;
    }
  else open_goto_file(wno, dir) ;
}


void
do_undelete()      /* Undelete selected files in the wastebasket. */
{
  File_Pane_Object **cp, **lp ;             /* Files array pointers. */
  char buf[MAXPATHLEN+32] ;
  int i ;
  Delete_item di ;
  WCHAR *file[MAXBIND] ;
  int cnt = 0 ;                             /* Number of selected files. */

  fm_busy_cursor(TRUE, WASTE) ;
  fm_clrmsg() ;                             /* Clear status line. */

/*  Get a list of selected files to undelete.  Need to do this up
 *  front since move_by_filename will unselect files.
 */

  lp = PTR_LAST(WASTE) ;
  for (cp = PTR_FIRST(WASTE); cp < lp; cp++)
    {
      if ((*cp)->selected)
          file[cnt++] = (WCHAR *) strdup((char*) (*cp)->name) ;
      if (cnt >= MAXBIND)
        {
          SPRINTF(buf, Str[(int) M_MAX_UNDELETE], MAXBIND) ;
          (void) notice(buf, Str[(int) M_OK], NULL) ;
          break ;
        }
    }

  for (i = 0; i < cnt; i++)
    {
      di = get_delete_path(file[i]) ;
      if (!di || (access((char *) di->path, F_OK) != 0))
        {
          SPRINTF(buf, Str[(int) M_NO_ORIG], file[i]) ;
          (void) notice(buf, Str[(int) M_OK], NULL) ;
        }
      else
        {
          SPRINTF(buf, "%s/%s", Fm->file[WASTE]->path, file[i]) ;
          fm_msg(FALSE, Str[(int) M_UNDELETING], file[i]) ;
          if (move_by_filename(buf, (char *) di->path, NULL) == FALSE)

/* Print error to user and move on. */

            FPRINTF(stderr, "filemgr: unable to undelete %s\n", file[i]) ;
          else
            fm_clrmsg() ;    /* Clear footer. */
        }
      FREE(file[i]) ;
    }
  itimer_system_check() ;          /* Redisplay any updated folders. */
  fm_busy_cursor(FALSE, WASTE) ;
}


void
env_insert(item, head)
struct env_list *item, **head ;
{
  struct env_list *temp ;

  if (*head != (struct env_list *) 0)
    {
      temp = *head ;
      while (temp->next != (struct env_list *) 0) temp = temp->next ;
      temp->next = item ;
    }
  else *head = item ;
  item->next = (struct env_list *) 0 ;
}


/*  Routine to convert character strings into format for passing to
 *  applications via the shell so as to avoid having the shell interpret
 *  any spaces or other metachars. The following characters are quoted by
 *  preceding them with a backslash:
 *
 *  ;  &  (  )  |  ^  <  >  newline  space  tab  \  '  "  `  *  ?  [
 *
 *  Thanks to Brian Katzung (katzung@lisle.swissbank.com) for this routine.
 */

int
escape_arg(out, in)
char *out, *in ;
{
  char *begin = out ;

  while (*in)
    {
      switch (*in)
        {
          case '\n' : *out++ = '"' ;       /* \n -> "\n" */
                      *out++ = '\n' ;
                      *out++ = '"' ;
                      break ;

          case '\'' : *out++ = '\\' ;      /* ' -> \'\'' */
                      *out++ = '\'' ;
                      *out++ = '\\' ;
                      *out++ = '\'' ;
                      *out++ = '\'' ;
                      break ;

/* Metacharacter can be quoted with just a \ */

          default   : if (strchr(" \t;&|^<>()?*[\\`\"", *in) != (char *) 0)
                        *out++ = '\\' ;
                      *out++ = *in ;
        }
      *in++ ;
    }
  *out = 0 ;
  return(out - begin) ;
}


/*  Check for $FILE in string and expand it.  If not there, append
 *  filename to end of string.
 */

char *
expand_filename(str, files)
char *str ;                   /* String to check. */
char *files ;                 /* File(s) to add in. */
{
  Boolean found = FALSE ;
  char buf[MAXPATHLEN+32] ;
  char *r_p, *s_p ;

  r_p = str ;
  for (s_p = str; *s_p; s_p++)
    if (*s_p == '$' && strncmp(s_p+1, "FILE", 4) == 0)
    {
      found = TRUE ;
      STRNCPY(buf, r_p, s_p - r_p) ;
      buf[s_p - r_p] = '\0' ;
      STRCAT(buf, files) ;
      if (s_p + 1 && s_p + 5) STRCAT(buf, s_p + 5) ;
    }

  if (found == FALSE) SPRINTF(buf, "%s %s", str, files) ;
  STRCPY(str, buf) ;
  return(str) ;
}


/*  Expand all $FILE and $ARG references within the method
 *  The parm "method" needs enough space to hold the expanded method.
 */

char *
expand_keywords(method, path, file, args)
char *method, *path, *file, *args ;
{
  register char *cp, *bp ;         /* First ptr, current ptr, buffer ptr. */
  char buf[MAXPATHLEN+1] ;

  if (!method || !file) return(NULL) ;

  cp = method ;
  bp = buf ;
  for (; *cp; cp++, bp++)
    {
      *bp = *cp ;                  /* Copy char-by-char. */
      if (*cp == '$')              /* Possible keyword found. */
        {
          *bp = '\0' ;

          if (strncmp(cp+1, "FILE", 4) == 0)   /* $FILE found */
            {
              STRCAT(buf, "\"") ;              /* Start quoting the file. */

/* Only add the path if the file is not an absolute pathname */

              if (*file != '/' && path && *path == '/')
                {
                  STRCAT(buf, path) ;          /* Add pathname. */
                  STRCAT(buf, "/") ;
                }
              STRCAT(buf, file) ;              /* Add filename. */
              STRCAT(buf, "\"") ;              /* Finish quoting the file. */
              bp = buf + strlen(buf)-1 ;       /* Advance ptr past filename. */
              if (cp+1 && cp+4) cp = cp+4 ; /* Advance current ptr past $FILE */
            }
          else if (strncmp(cp+1, "ARG", 3) == 0)   /* $ARG found. */
            {
              if (args)
                STRCAT(buf, args) ;                /* Add args. */

              bp = buf + strlen(buf)-1 ;     /* Advance buffer ptr past args. */
              if (cp+1 && cp+3) cp = cp+3 ;  /* Advance current ptr past $ARG */
            }
        }
    }
  *bp = '\0' ;                          /* Null terminate the buffer. */
  STRCPY(method, buf) ;
  return(method) ;
}


Boolean
find_pattern(s, pat)
char *s ;                /* String to search. */
char *pat ;              /* Pattern to search for. */
{
  char *cp ;             /* Current pointer. */

  for (cp = s ; *cp ; cp++)
    if (*cp == *pat && EQUALN(cp, pat))
      return(TRUE) ;     /* Found. */

  return(FALSE) ;        /* Not found. */
}


char *
fio_add_commas(k)        /* Add thousands commas to number. */
off_t k ;
{
  static char buf[20], *p, *q, tmp ;
  int i, j ;

  j = i = 0 ;
  if (k < 10000)
    SPRINTF(buf, "%lld", (long long) k) ;
  else
    {
      while (k > 0)
        {
          j++ ;
          buf[i++] = '0' + (k % 10) ;
          k = k / 10 ;
          if (k > 0 && j == 3)
            {
              buf[i++] = ',' ;
              j = 0 ;
            }
        }

      buf[i] = 0 ;           /* Found string, now reverse in place. */
      p = buf ;
      q = buf + i - 1 ;
      while (p < q)
        {
          tmp = *p ;
          *p = *q ;
          *q = tmp ;
          p++ ;
          q-- ;
        }
    }
  return(buf) ;
}


void
fio_clear_panel()      /* Clear all values from the info popup. */
{
  int i ;

  set_item_int_attr(FM_FIO_ICON_ITEM, FM_ITEM_SHOW, FALSE) ;
  set_item_int_attr(FM_FIO_CONTENTS,  FM_ITEM_SHOW, FALSE) ;

  fio_clear_item(FM_FIO_FILE_NAME,    TRUE) ;
  fio_clear_item(FM_FIO_OWNER,        TRUE) ;
  fio_clear_item(FM_FIO_GROUP,        TRUE) ;
  fio_clear_item(FM_FIO_BITE_SIZE,    TRUE) ;
  fio_clear_item(FM_FIO_MOD_TIME,     TRUE) ;
  fio_clear_item(FM_FIO_ACCESS_TIME,  TRUE) ;
  fio_clear_item(FM_FIO_FILE_TYPE,    TRUE) ;
  fio_clear_item(FM_FIO_PERMISSIONS,  TRUE) ;
  fio_clear_item(FM_FIO_PERM_READ,    TRUE) ;
  fio_clear_item(FM_FIO_PERM_WRITE,   TRUE) ;
  fio_clear_item(FM_FIO_PERM_EXE,     TRUE) ;
  fio_clear_item(FM_FIO_FREE_SPACE,   TRUE) ;
  fio_clear_item(FM_FIO_OPEN_METHOD,  TRUE) ;
  fio_clear_item(FM_FIO_PRINT_METHOD, TRUE) ;
  fio_clear_item(FM_FIO_MOUNT_POINT,  TRUE) ;
  fio_clear_item(FM_FIO_MOUNT_FROM,   TRUE) ;

  fio_clear_item(FM_FIO_OWNER_PERM, FALSE) ;
  fio_clear_item(FM_FIO_GROUP_PERM, FALSE) ;
  fio_clear_item(FM_FIO_WORLD_PERM, FALSE) ;

  for (i = 0; i < NUM_TOGGLES; i++) fio_clear_toggle(i, FALSE) ;
}


void
fio_file_free()
{
  ZFREE(Fm->fullname) ;
  ZFREE(Fm->f_owner) ;
  ZFREE(Fm->f_group) ;
}


int
fio_prefix(s1, s2)
register char *s1, *s2 ;
{
  while (s1 && s2 && *s1 == *s2++)
    if (*s1++ == 0) return(1) ;

  return(!*s1) ;
}


void
fio_update_panel(wno)
int wno ;
{
  File_Pane_Object *fpo ;
  Boolean first_file = TRUE ;

  fio_file_free() ;    /* Free file specific fields. */

  if (wno == WNO_TREE)
    {
      if (Fm->tree.selected)
        FIO_COMPARE_AND_UPDATE_PANEL(NULL, wno, first_file) ;
      else
        fio_clear_panel() ;
    }
  else if (Fm->file[wno]->path_pane.selected)
    FIO_COMPARE_AND_UPDATE_PANEL(NULL, wno, first_file) ;
  else
    {
      File_Pane_Object **curr, **last ;

      Fm->nseln = 0 ;
      last      = PTR_LAST(wno) ;
      for (curr = PTR_FIRST(wno); curr != last; curr++)
        if ((*curr)->selected)
          {
            fpo = (*curr) ;
            if (fio_compare_and_update_panel(fpo, wno, first_file))
              {
                Fm->nseln++ ;
                first_file = FALSE ;
              }
          }

      if (Fm->nseln == 0)
        fio_clear_panel() ;    /* Clear the values on the panel. */
    }
}


void
flush_delete_list()
{
  Delete_item cp, dp ;    /* Current ptr, delete ptr. */

  for (cp = Fm->DelList; cp;)
    {
      dp = cp ;
      ZFREE(dp->path) ;
      ZFREE(dp->file) ;
      cp = cp->next ;    /* Jump to next item before getting deleted. */
      FREE((Delete_item) dp) ;
    }
  Fm->DelList = NULL ;
}


void
fm_clrmsg()        /* Clear footer message. */
{
  if (Fm->file[Fm->curr_wno]->Message)
    {
      write_footer(Fm->curr_wno, "", FALSE) ;
      Fm->file[Fm->curr_wno]->Message = FALSE ;
    }
}


void
fm_create_menus()            /* Create control panel menus. */
{
  char fname[MAXPATHLEN] ;
  enum menu_type m ;

  fm_menu_create(FM_FILE_MENU) ;
  m = FM_FILE_MENU ;
  add_menu_help(m, FILE_BUT_OPEN,     "File_Open") ;
  add_menu_help(m, FILE_BUT_OPEN_ED,  "File_Open_Editor") ;
  add_menu_help(m, FILE_BUT_CDOC,     "File_Document") ;
  add_menu_help(m, FILE_BUT_CFOLD,    "File_Folder") ;
  add_menu_help(m, FILE_BUT_DUP,      "File_Duplicate") ;
  add_menu_help(m, FILE_BUT_PRINT1,   "File_Default_Print") ;
  add_menu_help(m, FILE_BUT_PRINT,    "File_Custom_Print") ;
  add_menu_help(m, FILE_BUT_FIND,     "File_Find") ;
  add_menu_help(m, FILE_BUT_FILEI,    "File_File_Info") ;
  add_menu_help(m, FILE_BUT_RCOPY,    "File_Remote_Copy") ;
  add_menu_help(m, FILE_BUT_CCMDS,    "File_Custom_Commands") ;
  add_menu_help(m, FILE_BUT_FCHECK,   "File_Check_Floppy") ;
  add_menu_help(m, FILE_BUT_FORMATD,  "File_Format_Disk") ;
  add_menu_help(m, FILE_BUT_RENAMED,  "File_Rename_Disk") ;
  add_menu_help(m, FILE_BUT_COMMENTS, "File_Comments") ;
  add_menu_help(m, FILE_BUT_QUIT,     "File_Really_Quit") ;

  add_menu_accel(m, FILE_BUT_OPEN,   "coreset Open") ;
  add_menu_accel(m, FILE_BUT_CFOLD,  "coreset New") ;
  add_menu_accel(m, FILE_BUT_PRINT1, "coreset Print") ;
  add_menu_accel(m, FILE_BUT_FIND,   "coreset Find") ;

  set_custom_menu(m) ;
  SPRINTF(fname, "%s/%s", Fm->home, OLD_CMD_FILE) ;
  if (Fm->add_old) fm_include_old(fname) ;
  fm_process_cmdfile() ;
  set_menu_default(FM_CC_MENU, Fm->menu_defs[(int) FM_CC_MENU]) ;

  fm_menu_create(FM_VIEW_MENU) ;
  m = FM_VIEW_MENU ;
  add_menu_help(m, VIEW_TREE,         "View_Tree") ;
  add_menu_help(m, VIEW_POS_ICON,     "View_Position_Icon") ;
  add_menu_help(m, VIEW_POS_LIST,     "View_Position_List") ;
  add_menu_help(m, VIEW_CLEANUP,      "View_Cleanup") ;
  add_menu_help(m, VIEW_ICON_BY_NAME, "View_Icon_Name") ;
  add_menu_help(m, VIEW_ICON_BY_TYPE, "View_Icon_Type") ;
  add_menu_help(m, VIEW_LIST_BY_NAME, "View_List_Name") ;
  add_menu_help(m, VIEW_LIST_BY_TYPE, "View_List_Type") ;
  add_menu_help(m, VIEW_LIST_BY_SIZE, "View_List_Size") ;
  add_menu_help(m, VIEW_LIST_BY_DATE, "View_List_Date") ;

  fm_menu_create(FM_EDIT_MENU) ;
  m = FM_EDIT_MENU ;
  add_menu_help(m, EDIT_BUT_SEL_ALL, "Edit_Select_All") ;
  add_menu_help(m, EDIT_BUT_CUT,     "Edit_Cut") ;
  add_menu_help(m, EDIT_BUT_COPY,    "Edit_Copy") ;
  add_menu_help(m, EDIT_BUT_LINK,    "Edit_Link") ;
  add_menu_help(m, EDIT_BUT_PASTE,   "Edit_Paste") ;
  add_menu_help(m, EDIT_BUT_DELETE,  "Edit_Delete") ;
  add_menu_help(m, EDIT_BUT_PROPS,   "Edit_Properties") ;

  add_menu_accel(m, EDIT_BUT_SEL_ALL, "coreset SelectAll") ;
  add_menu_accel(m, EDIT_BUT_CUT,     "coreset Cut") ;
  add_menu_accel(m, EDIT_BUT_COPY,    "coreset Copy") ;
  add_menu_accel(m, EDIT_BUT_PASTE,   "coreset Paste") ;
  add_menu_accel(m, EDIT_BUT_PROPS,   "coreset Props") ;

  fm_menu_create(FM_GOTO_MENU) ;
  add_menu_help(FM_GOTO_MENU, GOTO_BUT_HOME, "Goto_Home") ;
}


void
fm_free_argv(argv)    /* Free memory associated with an argv array. */
char *argv[] ;
{
  int argc = 0 ;

  while (argv[argc] != NULL) FREE(argv[argc++]) ;
}


char *
fm_get_resource(db, rtype)  /* Get resource from X resources database. */
enum db_type db ;
enum res_type rtype ;
{
  char str[MAXLINE] ;

  STRCPY(str, Fm->fm_res[(int) rtype]) ;
  return(get_resource(db, str)) ;
}


/* Put X resource into appropriate resources database. */

void
fm_put_resource(dbtype, rtype, value)
enum db_type dbtype ;
enum res_type rtype ;
char *value ;
{
  char str[MAXLINE] ;

  STRCPY(str, Fm->fm_res[(int) rtype]) ;
  put_resource(dbtype, str, value) ;
}


int
fm_regex_in_string(s_p)      /* Regular expression in string? */
register char *s_p ;
{
  while (*s_p)
    {
      if (*s_p == '*' || *s_p == '[' || *s_p == ']' ||
          *s_p == '{' || *s_p == '}' || *s_p == '?')
        break ;
      s_p++ ;
    }
  return(*s_p) ;
}


int
fm_run_argv(argv, wait_for_child)
char *argv[] ;
int wait_for_child ;
{
  char *buf ;
  unsigned int len = 0 ;
  int i = 0 ;
  int status ;

  while (argv[i]) len += strlen(argv[i++]) + 1 ;
  if ((buf = fm_malloc(len)) != NULL)
    {
      *buf = i = 0 ;
      while (argv[i])
        {
          if (i) STRCAT(buf, " ") ;
          STRCAT(buf, argv[i++]) ;
        }
      status = fm_run_str(buf, wait_for_child) ;
      FREE(buf) ;
      return(status) ;
    }
  return(-1) ;
}


void
fm_showerr(cmd)           /* Display stderr message. */
char *cmd ;
{
  FILE *fp ;
  static char buf[MAXPATHLEN] ;

  SPRINTF(buf, Str[(int) M_OPEN_ERROR], Fm->err_file) ;
  if ((fp = fopen(Fm->err_file, "r")) != NULL)
    {
      if (fgets(buf, 255, fp) == NULL)
        SPRINTF(buf, Str[(int) M_CMD], cmd) ;
      FCLOSE(fp) ;
      fm_msg(TRUE, buf) ;
      UNLINK(Fm->err_file) ;
    }
}


int
fm_stat(path, buf)
char *path ;
struct stat *buf ;
{
  int reply ;

  for (;;)
    {
      reply = stat(path, buf) ;
      if (reply == 0 || errno != EINTR) return(reply) ;
    }
}


/*  Convert a character string into an char *argv[] array, returning the
 *  number of tokens found. Quoted tokens are handled. Memory is malloc'ed
 *  to store each token in the argv[] array.
 */

int
fm_string_to_argv(str, argv)
char *str, *argv[] ;
{
  char tsep ;                /* Token separator. */
  char *r_p, *s_p, *t_p ;    /* Pointers into the character string. */
  int argc ;                 /* Number of tokens found. */
  unsigned int len ;         /* Length of this token. */

  argc = 0 ;
  s_p = str ;
  while (s_p != NULL)
    {

/* Determine what the string separator is. */

           if (*s_p == '\'') tsep = '\'' ;
      else if (*s_p == '"')  tsep = '"' ;
      else                   tsep = ' ' ;

      r_p = s_p ;
      if (tsep != ' ') s_p++, r_p++ ;
      t_p = r_p ;

/*  Advance rear index until some separator is found. If the separator is a
 *  space, then we've also got to look out for tabs.
 */

      if (tsep == ' ')
        while (t_p && *t_p != ' ' && *t_p != '\t' && *t_p != '\0') t_p++ ;
      else
        while (t_p && *t_p != tsep && *t_p != '\0') t_p++ ;

      if (t_p != r_p)
        {
          len = t_p - s_p + 1 ;
          argv[argc] = (char *) fm_malloc(len) ;
          STRNCPY((char *) argv[argc], (char *)s_p, len-1) ;
          argv[argc][len-1] = '\0' ;
          argc++ ;

          if (*t_p == '\0') s_p = NULL ;
          else              s_p = t_p + 1 ;

/*  Then advance the pointer to the first character which is not a space or
 *  a tab.
 */
          while (s_p != NULL && (*s_p == ' ' || *s_p == '\t')) s_p++ ;
        }
      else s_p = NULL ;
    }
  return(argc) ;
}


int
get_bool_resource(db, rtype, boolval)  /* Get boolean resource from database. */
enum db_type db ;
enum res_type rtype ;
int *boolval ;
{
  char *val, tempstr[MAXLINE] ;
  int len, n ;

  if ((val = fm_get_resource(db, rtype)) == NULL) return(0) ;
  STRCPY(tempstr, val) ;
  len = strlen(tempstr) ;
  for (n = 0; n < len; n++)
    if (isupper(tempstr[n])) tempstr[n] = tolower(tempstr[n]) ;
  if (EQUAL(tempstr, "true")) *boolval = TRUE ;
  else                        *boolval = FALSE ;
  return(1) ;
}


int
get_bool_win_info(wno, rstr, boolval)   /* Get bool. info. from database. */
int *boolval, wno ;
char *rstr ;
{
  char *val, tempstr[MAXLINE] ;
  int len, n ;

  if ((val = get_win_info(wno, rstr)) == NULL) return(0) ;
  STRCPY(tempstr, val) ;
  len = strlen(tempstr) ;
  for (n = 0; n < len; n++)
    if (isupper(tempstr[n])) tempstr[n] = tolower(tempstr[n]) ;
  if (EQUAL(tempstr, "true")) *boolval = TRUE ;
  else                        *boolval = FALSE ;
  return(1) ;
}


char *
get_ccmd_str_resource(n, rstr)
int n ;
char *rstr ;
{
  char str[MAXLINE] ;

  SPRINTF(str, "userCommand%1d%s", n, rstr) ;
  return(get_resource(FM_MERGE_DB, str)) ;
}


char *
get_goto_resource(gtype, n)
enum goto_type gtype ;
int n ;
{
  char str[MAXLINE] ;

  if (gtype == FM_GOTO_PATH) SPRINTF(str, "userGotoPathname%1d", n) ;
  else                       SPRINTF(str, "userGotoLabel%1d", n) ;
  return(get_resource(FM_MERGE_DB, str)) ;
}


/* Get the default open method. */

char *
get_default_open_method(fpo, useEditor)
File_Pane_Object *fpo ;
Boolean useEditor ;
{
  char *method = NULL ;
  static char buf[128] ;

  if (Fm->editor == FM_TEXTEDIT && useEditor == TRUE && fpo->type == FT_DOC)
    method = get_tns_attr(Fm->ceo->generic_doc, Fm->ceo->type_open) ;
  if ((fpo->type == FT_DOC && !method) || (fpo->type == FT_APP && useEditor))
    {
      if (Fm->editor == FM_TEXTEDIT)
        {
          SPRINTF(buf, "%s", "textedit $ARG $FILE") ;
          method = (char *) buf ;
        }
      else
        {
          method = (char *) Fm->other_editor ;
          if (method && isempty((WCHAR *) method))
            method = NULL ;
        }
    }
  else if (fpo->type == FT_APP)
    {
      SPRINTF(buf, "%s", fpo->name) ;
      method = (char *) buf ;
    }

/* XXX: need to add check to CE for default. */

  return(method) ;
}


char *
get_default_print_method()        /* Get the default print method. */
{
  char *method ;

  method = (char *) Fm->print_script ;
  if (method && isempty((WCHAR *) method)) method = NULL ;

/* XXX: need to add check to CE for default. */

  return(method) ;
}


Delete_item
get_delete_path(file)
WCHAR *file ;
{
  Delete_item di ;

  for (di = Fm->DelList; di; di = di->next)
    if (!strcmp((char *) di->file, (char *) file)) return(di) ;

  return(NULL) ;
}


/*  Read a file len bytes at a time.  It returns the bytes read -- it is up
 *  to the caller to detect the file is done by checking the bytes returned.
 */

int
get_file_contents(fp, buf, len)
FILE *fp ;
char *buf ;
int len ;
{
  int count = 0 ;

  count = fread(buf, 1, len, fp) ;
  return(count) ;
}


File_Pane_Object *
get_first_selection(wno)  /* Returns first selected item in a given window. */
int wno ;
{
  register File_Pane_Object **f_p, **l_p ;     /* Files array pointers. */

/* Stop at first selection. */

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p < l_p; f_p++)
  if ((*f_p)->selected) return((File_Pane_Object *) *f_p) ;
  return(NULL) ;
}


unsigned char
get_hex(c)
unsigned char c ;
{
       if (c >= '0' && c <= '9') return(c - '0') ;
  else if (c >= 'A' && c <= 'F') return(c - 'A' + 10) ;
  else if (c >= 'a' && c <= 'f') return(c - 'a' + 10) ;
}


int
get_int_resource(db, rtype, intval)   /* Get integer resource from database. */
enum db_type db ;
enum res_type rtype ;
int *intval ;
{
  char *val ;

  if ((val = fm_get_resource(db, rtype)) == NULL) return(0) ;
  *intval = atoi(val) ;
  return(1) ;
}


int
get_int_win_info(wno, rstr, intval)   /* Get integer info. from database. */
int *intval, wno ;
char *rstr ;
{
  char *val ;

  if ((val = get_win_info(wno, rstr)) == NULL) return(0) ;
  *intval = atoi(val) ;
  return(1) ;
}


char *
get_old_ccmd_str_resource(n, rstr)
int n ;
char *rstr ;
{
  char str[MAXLINE] ;

  SPRINTF(str, "cmd%1d.%s", n, rstr) ;
  return(get_resource(FM_OLDCCMD_DB, str)) ;
}


void
get_options(argc, argv)
int argc ;
char *argv[] ;
{
  char dir[MAXPATHLEN] ;        /* -d option directory pathname. */

  Fm->Progname = *argv ;
  argc-- ;
  argv++ ;
  while (argc > 0)
    {
      if (argv[0][0] == '-')
        switch(argv[0][1])
          {
            case 'a' : Fm->check_all = TRUE ;
                       break ;

            case 'C' : Fm->use_ce = FALSE ;
                       break ;

            case 'c' : Fm->dispdir = FM_DISP_BY_COLS ; /* Display by columns. */
                       Fm->sdir_given = TRUE ;
                       break ;

            case 'd' : argc-- ;
                       argv++ ;
                       if (*argv != NULL && argv[0][0] != '-')
                         {
                           ds_expand_pathname(*argv, dir) ;
                           if (chdir(dir) == -1)

/* Unable to change directory. */

                             fm_msg(TRUE, Str[(int) M_BAD_START_DIR], dir) ;
                           else
                             {
                               Fm->dirpath = (char *)
                                 malloc((unsigned int) (strlen(dir) + 1)) ;
                               STRCPY(Fm->dirpath, dir) ;
                             }
                         }
                       else

/* Bad starting directory. */

                         fm_msg(TRUE, Str[(int) M_BAD_START_DIR], dir) ;
                       break ;

            case 'D' : fm_msg(FALSE, Str[(int) M_DEBUGGING_ON]) ;
                       Fm->debug = TRUE ;    /* Turn on debugging. */
                       break ;

/*  XXX: This case has been added to debug dnd.  It forces filemgr to
 *  dnd by bits only, and not use mv or cp operations.  This should
 *  be removed for V3.0.1 fcs.
 */

            case 'b' : if (EQUAL((char *) *argv, "-bybits"))
                         Fm->DND_BY_BITS = TRUE ;  /* Allow dnd by bits only. */
                       break ;

            case 'i' : argc-- ;
                       argv++ ;
                       Fm->interval = UPDATE_INTERVAL ;
                       if (*argv != NULL && argv[0][0] != '-')
                         {
                           Fm->interval = atoi(*argv) ;  /* New interval. */
                           if (Fm->interval <= 0 ||
                               Fm->interval > MAX_INTERVAL)
                             {

/* Invalid interval found. */

                               fm_msg(TRUE, Str[(int) M_BAD_INTERVAL],
                                      MAX_INTERVAL, UPDATE_INTERVAL) ;
                               Fm->interval = UPDATE_INTERVAL ;
                             }
                         }
                       else

/* No interval found. */

                         fm_msg(TRUE, Str[(int) M_NO_INTERVAL],
                                MAX_INTERVAL, Fm->interval) ;
                       break ;

            case 'M' : Fm->color = FALSE ;   /* Always display in monochrome. */
                       break ;

            case 'r' : Fm->dispdir = FM_DISP_BY_ROWS ;   /* Display by rows. */
                       Fm->sdir_given = TRUE ;
                       break ;

            case 'v' : version(Fm->Progname) ; /* Display version information. */
            case 'n' : if (EQUAL((char *) *argv, "-nomedia")) {
				Fm->show_cd=FALSE;
                		Fm->show_floppy=FALSE;
			}
			break;
            case '?' : usage(Fm->appname) ;    /* Display usage message. */
            default  : usage(Fm->appname) ;    /* Display usage message. */
          }
      argc-- ;
      argv++ ;
    }
}


char *
get_pos_resource(pstr, rtype, intval)
char *pstr ;
enum res_type rtype ;
int *intval ;
{
  char str[MAXLINE], *val ;

  SPRINTF(str, "%s%s", pstr, Fm->fm_res[(int) rtype]) ;
  if ((val = get_resource(FM_DIR_DB, str)) == NULL) return(NULL) ;
  *intval = atoi(val) ;
  return(val) ;
}


/*  Collect all selected files in a window into a char buffer and return
 *  the number of files in the buffer.
 */

int
get_selections(wno, buf, no_folders)
int wno ;
char *buf ;
Boolean no_folders ;                          /* Include folders? */
{
  char in[MAXPATHLEN], out[MAXPATHLEN] ;
  int cnt = 0 ;
  int len = 0 ;
  File_Pane_Object **f_p, **l_p ;             /* Files array pointers */

  STRCPY(buf, "") ;                           /* Initialize string. */
  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p != l_p; f_p++)
    {
      if ((*f_p)->selected)
        {
          if (no_folders && (*f_p)->type != FT_DOC && (*f_p)->type != FT_APP)
            continue ;
          len += strlen((char *) Fm->file[wno]->path) + 1 ;
          len += (*f_p)->flen + 1 ;
          if (cnt) STRCAT(buf, " ") ;
          STRCPY(in, (char *) Fm->file[wno]->path) ;
          STRCAT(in, "/") ;
          STRCAT(in, (char *) (*f_p)->name) ;
          STRCAT(buf, in) ;
          cnt++ ;
        }
    }
  return(cnt) ;
}


int
get_str_resource(db, rtype, strval)  /* Get a string resource from database. */
enum db_type db ;
enum res_type rtype ;
char *strval ;
{
  char *val ;

  if ((val = fm_get_resource(db, rtype)) == NULL) return(0) ;
  STRCPY(strval, val) ;
  return(1) ;
}


int
get_str_win_info(wno, rstr, strval)  /* Get string win. info. from database. */
int wno ;
char *rstr, *strval ;
{
  char *val ;

  if ((val = get_win_info(wno, rstr)) == NULL) return(0) ;
  STRCPY(strval, val) ;
  return(1) ;
}


char *
get_win_info(wno, rstr)  /* Get filemgr window info. from merged database. */
int wno ;
char *rstr ;
{
  char str[MAXLINE] ;

       if (wno == WNO_TREE) STRCPY(str,  "tree") ;
  else if (wno == WASTE)    STRCPY(str,  "wastebasket") ;
  else                      SPRINTF(str, "window%1d", wno) ;
  STRCAT(str, rstr) ;
  return(get_resource(FM_MERGE_DB, str)) ;
}


void
goto_object()                        /* Either match or goto. */
{
  File_Pane_Object *f_p ;
  char dir[MAXPATHLEN], *str_p ;
  int wno = Fm->curr_wno ;

  if ((str_p = get_path_item_value(wno)) && *str_p)
    {

/* Regular expression?  If so, match and select those files. */

      if (fm_regex_in_string(str_p))
        {
          if (fm_match_files(wno, str_p, FALSE) == 1)
            {
              f_p = get_first_selection(wno) ;
              if (f_p->type == FT_DIR)
                {
                  CHDIR((char *) Fm->file[wno]->path) ;
                  fm_msg(FALSE, Str[(int) M_OPENING], f_p->name) ;
                  open_dir(f_p, wno) ;
                }
            }
        }
      else
        {

/*  If the contents of the goto line starts with a slash, dollar or tilde,
 *  then expand the pathname, otherwise append to the end of the current path.
 */
          if (*str_p == '/' || *str_p == '~' || *str_p == '$')
            ds_expand_pathname(str_p, dir) ;
          else
            {
              STRCPY(dir, (char *) Fm->file[wno]->path) ;
              STRCAT(dir, "/") ;
              STRCAT(dir, str_p) ;
            }
          str_p = dir ;

/* Must be a directory. Try and change directories there */

          open_goto_file(wno, dir) ;
        }
    }
  set_menu_items(FM_GOTO_MENU, set_goto_button_menu) ;
}


void
init_advanced_panel()
{
  fm_add_help(FM_PO_PROPS_ADVANCED_PANEL, "filemgr:Props_Icon_Panel") ;
  fm_add_help(FM_PO_PRINT_I,     "filemgr:Props_Icon_Def_Print_Script") ;
  fm_add_help(FM_PO_FILTER_I,    "filemgr:Props_Icon_Filter_Pattern") ;
  fm_add_help(FM_PO_LINK_I,      "filemgr:Props_Follow_Links") ;
  fm_add_help(FM_PO_INTERVAL_I,  "filemgr:Props_Interval_Time") ;
  fm_add_help(FM_PO_EDITOR_I,       "filemgr:Props_Icon_Def_Doc_Editor") ;
  fm_add_help(FM_PO_OTHER_EDITOR_I, "filemgr:Props_Icon_Def_Doc_Editor") ;
  fm_add_help(FM_PO_MEDIA_M,     "filemgr:Props_Auto_Media") ;
  fm_add_help(FM_PO_CDROM_I,     "filemgr:Props_Auto_Media") ;

  fm_add_help(FM_PO_CONTENT_M,      "filemgr:Props_Classing_Engine_Content") ;
  fm_add_help(FM_PO_FLOPPY_CONTENT, "filemgr:Props_Classing_Engine_Content") ;
  fm_add_help(FM_PO_CD_CONTENT,     "filemgr:Props_Classing_Engine_Content") ;
  fm_add_help(FM_PO_ADV_APPLY,   "filemgr:apply") ;
  fm_add_help(FM_PO_ADV_DEFAULT, "filemgr:set_default") ;
  fm_add_help(FM_PO_ADV_RESET,   "filemgr:reset") ;

  set_item_int_attr(FM_PO_EDITOR_I,    FM_ITEM_IVALUE, (int) Fm->editor) ;
  set_item_str_attr(FM_PO_OTHER_EDITOR_I, FM_ITEM_IVALUE, Fm->other_editor) ;
  set_item_int_attr(FM_PO_OTHER_EDITOR_I, FM_ITEM_SHOW,
                    Fm->editor == FM_OTHER_EDITOR) ;

  set_default_item(FM_PO_PROPS_ADVANCED_PANEL, FM_PO_ADV_APPLY) ;
  set_item_str_attr(FM_PO_PRINT_I, FM_ITEM_IVALUE, (char *) Fm->print_script) ;
  set_item_str_attr(FM_PO_FILTER_I,    FM_ITEM_IVALUE, (char *) Fm->filter) ;
  set_item_int_attr(FM_PO_INTERVAL_I,  FM_ITEM_IVALUE, Fm->interval) ;
  set_item_int_attr(FM_PO_LINK_I,      FM_ITEM_IVALUE, !Fm->follow_links) ;
  set_item_int_attr(FM_PO_CDROM_I,     FM_ITEM_IVALUE, !Fm->show_cd) ;
  set_item_int_attr(FM_PO_CD_CONTENT,     FM_ITEM_IVALUE, !Fm->cd_content) ;
  set_item_int_attr(FM_PO_FLOPPY_CONTENT, FM_ITEM_IVALUE, !Fm->floppy_content) ;

  fm_center_prop_buttons(FM_PO_PROPS_ADVANCED_PANEL, FM_PO_ADV_APPLY,
                         FM_PO_ADV_DEFAULT, FM_PO_ADV_RESET) ;

  Fm->adv_pw = get_item_int_attr(FM_PO_PROPS_ADVANCED_PANEL, FM_ITEM_WIDTH) ;
}


void
init_cur_folder_panel()
{
  fm_add_help(FM_PO_PROPS_CUR_PANEL, "filemgr:Props_CurFolder_Panel") ;
  fm_add_help(FM_PO_CURF_NAMELEN, "filemgr:Props_CurFolder_Longest_Filename") ;
  fm_add_help(FM_PO_CURF_LAYOUT,     "filemgr:Props_CurFolder_Layout") ;
  fm_add_help(FM_PO_CURF_DISPLAY,    "filemgr:Props_CurFolder_Display_Mode") ;
  fm_add_help(FM_PO_CURF_CONTENT,    "filemgr:Props_CurFolder_Content_Mode") ;
  fm_add_help(FM_PO_CURF_SORTBY,     "filemgr:Props_CurFolder_Sort") ;
  fm_add_help(FM_PO_CURF_HIDDEN,     "filemgr:Props_CurFolder_Hidden_Items") ;
  fm_add_help(FM_PO_CURF_LISTOPTS,   "filemgr:Props_CurFolder_List_Options") ;
  fm_add_help(FM_PO_CUR_APPLY,       "filemgr:apply") ;
  fm_add_help(FM_PO_CUR_DEFAULT,     "filemgr:set_default") ;
  fm_add_help(FM_PO_CUR_RESET,       "filemgr:reset") ;

  set_default_item(FM_PO_PROPS_CUR_PANEL, FM_PO_CUR_APPLY) ;
  set_cur_folder_panel() ;
  fm_center_prop_buttons(FM_PO_PROPS_CUR_PANEL, FM_PO_CUR_APPLY,
                         FM_PO_CUR_DEFAULT, FM_PO_CUR_RESET) ;

  Fm->cur_pw = get_item_int_attr(FM_PO_PROPS_CUR_PANEL, FM_ITEM_WIDTH) ;
}


void
init_custom_panel()
{
  int w, x, y ;

  set_item_int_attr(FM_PO_CMD_PROMPT, FM_ITEM_IVALUE, TRUE) ;
  set_item_int_attr(FM_PO_CMD_OUTPUT, FM_ITEM_IVALUE, TRUE) ;

  fm_add_help(FM_PO_PROPS_CUSTOM_PANEL, "filemgr:Props_Custom_Panel") ;
  fm_add_help(FM_PO_CMD_LIST,       "filemgr:Props_Custom_Custom_Commands") ;
  fm_add_help(FM_PO_NEW_COMMAND_BUTTON, "filemgr:Props_Custom_New_Command") ;
  fm_add_help(FM_PO_EDIT_BUTTON,        "filemgr:Props_Custom_Edit_Menu") ;
  fm_add_help(FM_PO_CMD_MLABEL,         "filemgr:Props_Custom_Menu_Label") ;
  fm_add_help(FM_PO_CMD_CMDLINE,        "filemgr:Props_Custom_UNIX_Command") ;
  fm_add_help(FM_PO_CMD_PROMPT,         "filemgr:Props_Custom_Prompt_Window") ;
  fm_add_help(FM_PO_CMD_PTEXT1,         "filemgr:Props_Custom_Prompt") ;
  fm_add_help(FM_PO_CMD_OUTPUT,         "filemgr:Props_Custom_Output_Window") ;
  fm_add_help(FM_PO_CUSTOM_APPLY,       "filemgr:apply") ;
  fm_add_help(FM_PO_CUSTOM_DEFAULT,     "filemgr:set_default") ;
  fm_add_help(FM_PO_CUSTOM_RESET,       "filemgr:reset") ;

  set_default_item(FM_PO_PROPS_CUSTOM_PANEL, FM_PO_CUSTOM_APPLY) ;

  x = get_item_int_attr(FM_PO_CMD_CMDLINE, FM_ITEM_VALX) ;
  set_item_int_attr(FM_PO_CMD_LIST, FM_ITEM_X, x) ;

  w = get_item_int_attr(FM_PO_CMD_CMDLINE, FM_ITEM_WIDTH) ;
  x = get_item_int_attr(FM_PO_CMD_LIST, FM_ITEM_X) ;
  y = get_item_int_attr(FM_PO_CMD_LIST, FM_ITEM_VALY) ;
  set_item_int_attr(FM_PO_NEW_COMMAND_BUTTON, FM_ITEM_X, x + w + 20) ;
  set_item_int_attr(FM_PO_EDIT_BUTTON,        FM_ITEM_X, x + w + 20) ;
  set_item_int_attr(FM_PO_EDIT_BUTTON,        FM_ITEM_Y, y) ;
  set_item_int_attr(FM_PO_CMD_LIST, FM_ITEM_LWIDTH, w - 20) ;

  x = get_item_int_attr(FM_PO_NEW_COMMAND_BUTTON, FM_ITEM_X) ;
  w = get_item_int_attr(FM_PO_NEW_COMMAND_BUTTON, FM_ITEM_WIDTH) ;
  set_item_int_attr(FM_PO_PROPS_CUSTOM_PANEL, FM_ITEM_WIDTH, x + w + 20) ;

  fm_center_prop_buttons(FM_PO_PROPS_CUSTOM_PANEL, FM_PO_CUSTOM_APPLY,
                         FM_PO_CUSTOM_DEFAULT, FM_PO_CUSTOM_RESET) ;

  Fm->cus_pw = get_item_int_attr(FM_PO_PROPS_CUSTOM_PANEL, FM_ITEM_WIDTH) ;
  build_cmd_panel_list(FM_PO_CMD_LIST) ;
}


void
init_general_panel()
{
  fm_add_help(FM_PO_PROPS_GEN_PANEL, "filemgr:Props_General_Panel") ;
  fm_add_help(FM_PO_OPEN_FOLDER,     "filemgr:Props_General_Open_Folder") ;
  fm_add_help(FM_PO_TREE_DIR,        "filemgr:Props_General_Tree_Direction") ;
  fm_add_help(FM_PO_DELETE_I,        "filemgr:Props_General_Delete") ;
  fm_add_help(FM_PO_DEL_MES,         "filemgr:Props_General_Delete") ;
  fm_add_help(FM_PO_DES_MES,         "filemgr:Props_General_Delete") ;
  fm_add_help(FM_PO_DEFDOC,        "filemgr:Props_General_Default_Document") ;
  fm_add_help(FM_PO_GEN_APPLY,       "filemgr:apply") ;
  fm_add_help(FM_PO_GEN_DEFAULT,     "filemgr:set_default") ;
  fm_add_help(FM_PO_GEN_RESET,       "filemgr:reset") ;

  set_default_item(FM_PO_PROPS_GEN_PANEL, FM_PO_GEN_APPLY) ;
  set_item_int_attr(FM_PO_OPEN_FOLDER, FM_ITEM_IVALUE, Fm->newwin) ;
  set_item_int_attr(FM_PO_TREE_DIR,    FM_ITEM_IVALUE, !Fm->tree_vertical) ;
  set_item_int_attr(FM_PO_DELETE_I,    FM_ITEM_IVALUE, !Fm->confirm_delete) ;
  set_item_str_attr(FM_PO_DEFDOC,      FM_ITEM_IVALUE, Fm->newdoc) ;

  pos_eitem_messages() ;
  fm_center_prop_buttons(FM_PO_PROPS_GEN_PANEL, FM_PO_GEN_APPLY,
                         FM_PO_GEN_DEFAULT, FM_PO_GEN_RESET) ;
  Fm->gen_pw = get_item_int_attr(FM_PO_PROPS_GEN_PANEL, FM_ITEM_WIDTH) ;
}


/*  Setup Gen_icon_image array with generic icons from statically
 *  included icons.
 */

void
init_generic_icons()
{
  static unsigned short folder_bits[] = {
#include "images/Folder_closed_glyph.icon"
  } ;
  static unsigned short folder_mask_bits[] = {
#include "images/Folder_closed_glyph_mask.icon"
  } ;

  static unsigned short document_bits[] = {
#include "images/document.icon"
  } ;
  static unsigned short document_mask_bits[] = {
#include "images/documentI.icon"
  } ;

  static unsigned short application_bits[] = {
#include "images/application.icon"
  } ;
  static unsigned short application_mask_bits[] = {
#include "images/applicationI.icon"
  } ;

  static unsigned short system_bits[] = {
#include "images/system.icon"
  } ;
  static unsigned short system_mask_bits[] = {
#include "images/systemI.icon"
  } ;

  static unsigned short broken_link_bits[] = {
#include "images/brokenlink.icon"
  } ;
  static unsigned short broken_link_mask_bits[] = {
#include "images/applicationI.icon"
  } ;

  static unsigned short folder_unexpl_bits[] = {
#include "images/Folder_closed_glyph.icon"
  } ;
  static unsigned short folder_unexpl_mask_bits[] = {
#include "images/Folder_closed_glyph_mask.icon"
  } ;

  static unsigned short folder_open_bits[] = {
#include "images/open_folder.icon"
  } ;
  static unsigned short folder_open_mask_bits[] = {
#include "images/open_folder.mask.icon"
  } ;

  static unsigned short waste_bits[] = {
#include "images/small_waste.icon"
  } ;
  static unsigned short waste_mask_bits[] = {
#include "images/small_waste.mask.icon"
  } ;

  static unsigned short cd_bits[] = {
#include "images/small_cd.icon"
  } ;
  static unsigned short cd_mask_bits[] = {
#include "images/small_cd.mask.icon"
  } ;

  static unsigned short floppy_bits[] = {
#include "images/small_floppy.icon"
  } ;
  static unsigned short floppy_mask_bits[] = {
#include "images/small_floppy.mask.icon"
  } ;

  static unsigned short dos_bits[] = {
#include "images/small_dos.icon"
  } ;
  static unsigned short dos_mask_bits[] = {
#include "images/small_dos.mask.icon"
  } ;

  create_generic_icon_entry(FT_DIR, folder_bits, folder_mask_bits) ;
  create_generic_icon_entry(FT_DOC, document_bits, document_mask_bits) ;
  create_generic_icon_entry(FT_APP, application_bits, application_mask_bits) ;
  create_generic_icon_entry(FT_SYS, system_bits, system_mask_bits) ;
  create_generic_icon_entry(FT_BAD_LINK,
                            broken_link_bits, broken_link_mask_bits) ;
  create_generic_icon_entry(FT_DIR_UNEXPLORED,
                            folder_unexpl_bits, folder_unexpl_mask_bits) ;
  create_generic_icon_entry(FT_DIR_OPEN,
                            folder_open_bits, folder_open_mask_bits) ;
  create_generic_icon_entry(FT_WASTE,  waste_bits,  waste_mask_bits) ;
  create_generic_icon_entry(FT_CD,     cd_bits,     cd_mask_bits) ;
  create_generic_icon_entry(FT_FLOPPY, floppy_bits, floppy_mask_bits) ;
  create_generic_icon_entry(FT_DOS,    dos_bits,    dos_mask_bits) ;
}


/*  Setup Generic_list_icon array with generic icons from statically
 *  included icons.
 */

void
init_generic_list_icons()
{
  static unsigned short folder_bits[] = {
#include "images/folder_list.icon"
  } ;
  static unsigned short folder_mask_bits[] = {
#include "images/folder_listI.icon"
  } ;

  static unsigned short document_bits[] = {
#include "images/doc_list.icon"
  } ;
  static unsigned short document_mask_bits[] = {
#include "images/doc_listI.icon"
  } ;

  static unsigned short application_bits[] = {
#include "images/app_list.icon"
  } ;
  static unsigned short application_mask_bits[] = {
#include "images/app_listI.icon"
  } ;

  static unsigned short system_bits[] = {
#include "images/sys_list.icon"
  } ;
  static unsigned short system_mask_bits[] = {
#include "images/sys_listI.icon"
  } ;

  static unsigned short broken_link_bits[] = {
#include "images/broken_list.icon"
  } ;
  static unsigned short broken_link_mask_bits[] = {
#include "images/app_listI.icon"                  /* XXX create mask */
  } ;

  static unsigned short folder_unexpl_bits[] = {
#include "images/folder_list.icon"                /* XXX create new one */
  } ;
  static unsigned short folder_unexpl_mask_bits[] = {
#include "images/folder_listI.icon"               /* XXX create mask */
  } ;

  static unsigned short folder_open_bits[] = {
#include "images/folder_list.icon"                /* XXX create new one */
  } ;
  static unsigned short folder_open_mask_bits[] = {
#include "images/folder_listI.icon"               /* XXX create new one */
  } ;

  create_generic_list_icon_entry(FT_DIR, folder_bits,   folder_mask_bits) ;
  create_generic_list_icon_entry(FT_DOC, document_bits, document_mask_bits) ;
  create_generic_list_icon_entry(FT_APP,
                                 application_bits, application_mask_bits) ;
  create_generic_list_icon_entry(FT_SYS,
                                 system_bits, system_mask_bits) ;
  create_generic_list_icon_entry(FT_BAD_LINK,
                                 broken_link_bits, broken_link_mask_bits) ;
  create_generic_list_icon_entry(FT_DIR_UNEXPLORED,
                                 folder_unexpl_bits, folder_unexpl_mask_bits) ;
  create_generic_list_icon_entry(FT_DIR_OPEN,
                                 folder_open_bits, folder_open_mask_bits) ;
}


void
init_new_folder_panel()
{
  fm_add_help(FM_PO_PROPS_NEWF_PANEL, "filemgr:Props_NewFolder_Panel") ;
  fm_add_help(FM_PO_NEWF_DEFNAME,     "filemgr:Props_NewFolder_New_Folder") ;
  fm_add_help(FM_PO_NEWF_NAMELEN, "filemgr:Props_NewFolder_Longest_Filename") ;
  fm_add_help(FM_PO_NEWF_LAYOUT,      "filemgr:Props_NewFolder_Layout") ;
  fm_add_help(FM_PO_NEWF_DISPLAY,     "filemgr:Props_NewFolder_Display_Mode") ;
  fm_add_help(FM_PO_NEWF_CONTENT,     "filemgr:Props_NewFolder_Content_Mode") ;
  fm_add_help(FM_PO_NEWF_SORTBY,      "filemgr:Props_NewFolder_Sort") ;
  fm_add_help(FM_PO_NEWF_HIDDEN,      "filemgr:Props_NewFolder_Hidden_Items") ;
  fm_add_help(FM_PO_NEWF_LISTOPTS,    "filemgr:Props_NewFolder_List_Options") ;
  fm_add_help(FM_PO_NEW_APPLY,        "filemgr:apply") ;
  fm_add_help(FM_PO_NEW_DEFAULT,      "filemgr:set_default") ;
  fm_add_help(FM_PO_NEW_RESET,        "filemgr:reset") ;

  set_default_item(FM_PO_PROPS_NEWF_PANEL, FM_PO_NEW_APPLY) ;
  set_item_str_attr(FM_PO_NEWF_DEFNAME, FM_ITEM_IVALUE, Fm->newdir) ;
  set_item_int_attr(FM_PO_NEWF_NAMELEN, FM_ITEM_IVALUE, Fm->num_file_chars) ;
  set_item_int_attr(FM_PO_NEWF_DISPLAY, FM_ITEM_IVALUE,
                    (int) Fm->display_mode)
;
  set_item_int_attr(FM_PO_NEWF_CONTENT, FM_ITEM_IVALUE, Fm->content_types) ;
  set_item_int_attr(FM_PO_NEWF_HIDDEN,  FM_ITEM_IVALUE, (Fm->see_dot ? 1 : 0)) ;
  set_item_int_attr(FM_PO_NEWF_LISTOPTS, FM_ITEM_IVALUE, Fm->listopts) ;
  set_item_int_attr(FM_PO_NEWF_LAYOUT,   FM_ITEM_IVALUE, !Fm->dispdir) ;
  set_item_int_attr(FM_PO_NEWF_SORTBY,   FM_ITEM_IVALUE, Fm->sortby) ;

  fm_center_prop_buttons(FM_PO_PROPS_NEWF_PANEL, FM_PO_NEW_APPLY,
                         FM_PO_NEW_DEFAULT, FM_PO_NEW_RESET) ;
  Fm->new_pw = get_item_int_attr(FM_PO_PROPS_NEWF_PANEL, FM_ITEM_WIDTH) ;
}


int
isempty(s)   /* Check to see if string is empty (except for white space). */
WCHAR *s ;
{
  int len = strlen((char *) s), i ;

  for (i = 0; i < len; i++)
    if (!isspace(s[i])) return(0) ;
  return(1) ;
}


/* if vold changed status, update file menu's accordingly */
int
fm_verify_volmgt()
{
    int wno;
    int volmgt_present = volmgt_running();

    if ( Fm->volmgt == volmgt_present )
	return Fm->volmgt;

    for(wno=0; wno < Fm->num_win; wno++) {
	if ( is_frame(wno) && (wno != WASTE) )
	    mitem_inactive(FM_FILE_MENU, FILE_BUT_FCHECK, Fm->volmgt);
    }

    return Fm->volmgt = volmgt_present;
}



/* Itimer notifier proc to check for changed system information. */

void
itimer_system_check()   /* ARGS IGNORED */
{
  char curdir[MAXPATHLEN], *fptr, oldcurdir[MAXPATHLEN] ;
  Tree_Pane_Object *tnode ;
  struct stat fstatus ;
  int wno ;


  (void) fm_verify_volmgt();

/*  Loop thru windows and check for changed stat() times.  Note the itimer
 *  should be running always if a Wastebasket exists, in order to change
 *  the icon.
 */

  for (wno = 0; wno < Fm->num_win; wno++)
    {
      if (is_frame(wno) && (wno == WASTE || !is_frame_closed(wno)))
        {
          if (Fm->check_all)
            check_all_files(wno) ;
          else if (fm_stat((char *) Fm->file[wno]->path, &fstatus) == 0)
            {
              if (fstatus.st_mtime > Fm->mtime[wno])
                {
                  fm_display_folder((BYTE) FM_BUILD_FOLDER, wno) ;

/*  If there is a tree view showing, then check to see if the change was the
 *  creation or deletion of any folders, and adjust the tree accordingly.
 */
                  if (Fm->treeview) adjust_tree(wno) ;

/*  Tree might be out of date; check for new subfolders when you re-open
 *  current folder in tree.
 */
                  Fm->tree.current->status &= ~TEXPLORE ;
                }
            }
          else
            {

/*  The fstat for this directory has failed. Three possible things could have
 *  happened:
 *
 *  1/ The user has moved this directory somewhere else, by some other
 *     process than filemgr. If so, then adjust tree/path view and update
 *     window to point to home directory.
 *
 *  2/ The user has deleted this directory, by some other process than
 *     filemgr. If so, then adjust tree/path view and update
 *     window to point to home directory.
 *
 *  3/ The window had been monitoring a CDROM or a floppy which has now been
 *     ejected. In this case, this window is just ignored. The next time that
 *     check_media() is called, the window should just disappear.
 */

              *curdir = '\0' ;
              if (Fm->file[wno]->mtype == FM_DISK)
                if (wno != WASTE)
                  {
		    int curr_wno = Fm->curr_wno;
		    Fm->curr_wno = wno;
                    fm_msg(TRUE, Str[(int) M_DIR_GONE], Fm->file[wno]->path) ;
		    Fm->curr_wno = curr_wno;
		    Fm->error_state = TRUE;
                    if (fm_getcwd(curdir, MAXPATHLEN) == NULL ||
                        *curdir == '\0')
                      STRCPY(curdir, (char *) Fm->home) ;
                    STRCPY(oldcurdir, (char *) Fm->file[wno]->path) ;
                    fm_busy_cursor(TRUE, wno) ;
                    fm_pathdeselect(wno) ;
		    Fm->isgoto_select = TRUE;
                    if (fm_openfile(curdir, (char *) 0, TRUE, wno))
                      {
                        if ((fptr = strrchr(oldcurdir, '/')) != NULL)
                          *fptr++ = '\0' ;
                        tnode = path_to_node(WNO_TREE, (WCHAR *) oldcurdir) ;
                        remove_tree_entry(tnode, (WCHAR *) fptr) ;
                        fm_drawtree(TRUE) ;
                      }
		    Fm->isgoto_select = FALSE;
		    Fm->error_state = FALSE;
                    fm_busy_cursor(FALSE, wno) ;
                  }
                else
                  {
                    if (access((char *) Fm->file[WASTE]->path, F_OK) == -1)
                      if (mkdir((char *) Fm->file[WASTE]->path, 0777) == -1)
			{
		          int curr_wno = Fm->curr_wno;
		          Fm->curr_wno = wno;
                          fm_msg(TRUE, Str[(int) M_NO_CREATE_WASTE],
                               Fm->file[WASTE]->path, strerror(errno)) ;
		          Fm->curr_wno = curr_wno;
			}
                  }
            }
        }
    }
  CHDIR((char *) Fm->home) ;
}


void
make_pipe()     /* Open named pipe for vold CD notification. */
{
  const char *dname = "/tmp/.removable" ;
  char pipename[MAXPATHLEN] ;
  int n ;

/* Make sure /tmp/.removable exists first, making it if it doesn't. */

  if (access(dname, F_OK) == -1)
      if (mkdir(dname, (mode_t) 0777) == -1) {
	  perror("mkdir") ;
	  exit(1) ;
      }

  /* make *sure* other users can access */
  (void) chmod(dname, 0777);


/* Generate a unique new /tmp/.removable/notify# name. */

  n = 0 ;
  do
    {
      n++ ;
      SPRINTF(pipename, "%s/notify%1d", dname, n) ;
    }
  while (access(pipename, F_OK) == 0) ;

/* Create the named pipe. */

  if (mknod(pipename, S_IFIFO | 0600, 0) < 0)
    {
      perror("mknod") ;
      exit(1) ;
    }

/*  Note we open r/w so open won't hang, and closes by vold won't screw us
 *  up with EOFs.
 */

  if ((Fm->pipe_fd = open(pipename, O_RDWR, 0)) < 0)
    {
      perror("open") ;
      exit(1) ;
    }

/*  Save name so the pipe can be removed when we quit. This isn't too
 *  important, because volume management also cleans up bogus named pipes.
 */

  read_str(&Fm->pipe_name, pipename) ;
}


/* Return the number of selected items in a window. */

int
number_selected_items(wno)
int wno ;
{
  register File_Pane_Object **curr, **end ;    /* File pane object ptrs. */
  register int cnt = 0 ;

  end = PTR_LAST(wno) ;
  for (curr = PTR_FIRST(wno); curr != end; curr++)
    if ((*curr)->selected)
      cnt++ ;

  return(cnt) ;
}


int
open_above_mount_point(wno, pathname)
int wno ;
char *pathname ;
{
  char resolved[MAXPATHLEN] ;
  File_Pane *f = Fm->file[wno] ;

  REALPATH(pathname, resolved) ;
  if (f->mtype == FM_CD && !EQUALN(resolved, Fm->cmntpt[f->devno]))
    return(TRUE) ;
  if ((f->mtype == FM_FLOPPY || f->mtype == FM_DOS) &&
       !EQUALN(resolved, Fm->fmntpt[f->devno]))
    return(TRUE) ;
  return(FALSE) ;
}


void
open_goto_file(wno, name)
int wno ;
char *name ;
{
  char expanded[MAXPATHLEN] ;         /* Resolve regular expression. */
  char resolved_name[MAXPATHLEN] ;    /* Fixup references to "." and ".." */
  int i, info[MAXINFO] ;
  struct stat fstatus ;               /* Check for folder. */
  File_Pane *f = Fm->file[wno] ;

  Fm->isgoto_select = TRUE ;/* flag for no update message in goto operation */
  fm_busy_cursor(TRUE, wno) ;

  ds_expand_pathname(name, expanded) ;

/* Resolve references to "." and ".." */

  if (fm_realpath(expanded, resolved_name) == 0)
    fm_msg(TRUE, strerror(errno)) ;
  else if (fm_stat(resolved_name, &fstatus))
    fm_msg(TRUE, strerror(errno)) ;
  else
    {
      fm_msg(FALSE, Str[(int) M_OPENING], resolved_name) ;
      if (f->path_pane.selected) fm_pathdeselect(wno) ;
      if (open_above_mount_point(wno, resolved_name))
        {
          for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
          FM_NEW_FOLDER_WINDOW("", resolved_name, NULL, FM_DISK, 0,
                               FALSE, info) ;
          set_path_item_value(wno, "") ;
        }
      else if (Fm->newwin)
        {
          for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
          FM_NEW_FOLDER_WINDOW("", resolved_name, NULL,
                               f->mtype, f->devno, FALSE, info) ;
          set_path_item_value(wno, "") ;
        }
      else
        {
          if (is_window_showing(resolved_name, 1))
            {
              fm_busy_cursor(FALSE, wno) ;
              Fm->isgoto_select = FALSE ;
              return ;
            }
          save_directory_info(wno) ;
          if ((fstatus.st_mode & S_IFMT) == S_IFDIR) clear_position_info(wno) ;
          set_tree_icon(path_to_node(WNO_TREE, f->path), FALSE) ;
          if (fm_openfile(resolved_name, (char *) NULL,
               ((fstatus.st_mode & S_IFMT) == S_IFDIR), wno))

/* Successfully moved so clear goto line. */

            set_path_item_value(wno, "") ;
        }
    }
  Fm->isgoto_select = FALSE ; /* reset back */
  fm_busy_cursor(FALSE, wno) ;
}


void
po_make()
{
  fm_popup_create(FM_PO_POPUP) ;
  link_po_items() ;

  fm_add_help(FM_PO_PROPS_STACK,       "filemgr:Props_Category") ;
  fm_add_help(FM_PO_PROPS_STACK_PANEL, "filemgr:Props_Category") ;

  init_general_panel() ;
  init_new_folder_panel() ;
  init_cur_folder_panel() ;
  init_goto_panel() ;
  init_custom_panel() ;
  init_advanced_panel() ;

  set_list_item(TRUE) ;        /* New Folder settings. */
  set_list_item(FALSE) ;       /* Current Folder settings. */

/* Width numeric text item. */

  if (Fm->num_file_chars <= 0 || Fm->num_file_chars > 255)
    Fm->num_file_chars = 15 ;     /* Default number of chars wide. */

  fm_position_popup(Fm->curr_wno, FM_PO_PO_FRAME) ;
  Fm->cur_prop_sheet = PROPS_STACK_GENERAL ;
  adjust_prop_sheet_size(Fm->cur_prop_sheet) ;
  set_item_int_attr(FM_PO_PROPS_GEN_PANEL, FM_ITEM_SHOW, TRUE) ;
  set_item_int_attr(FM_PO_PO_FRAME,        FM_ITEM_SHOW, TRUE) ;
  Fm->props_created = TRUE ;
}


Boolean
po_valid_values(value)
int value ;
{
  int reply = TRUE ;

  if (value == PROPS_STACK_ADVANCED) reply = valid_advanced_panel() ;
  return(reply) ;
}


/*  print_files() inits the print job, including getting the print
 *  method, and finding selected files. It is used by both default
 *  and custom print routines.
 *
 *  Default Printing:
 *
 *  Passing a NULL method will force the routine to look first at the
 *  print method associated with the filetype of the file.  If this
 *  does not exist, then it will default to the print method defined
 *  on the tools property sheet.
 *
 *  Custom Printing:
 *
 *  Passing a method will force the routine to try and execute the
 *  the print command copies times.
 *
 *  wno is used for both default & custom printing. Only the selected files
 *  in window wno will be printed.
 */

void
print_files(method, copies, wno)
char *method ;
int copies ;
int wno ;
{
  register File_Pane_Object **f_p, **l_p ;     /* Files array pointers. */
  int error, j ;
  Boolean found = FALSE ;
  File_Pane *f  = Fm->file[wno] ;
  char cmd[MAX_CMD_LEN+1] ;

  cmd[0] = '\0' ;

/* Loop thru selected items in window. */

  CHDIR((char *) f->path) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p != l_p; f_p++)
    if ((*f_p)->selected)
      {
        cmd[0] = '\0' ;
        found = TRUE ;
        if (!method)         /* Use default print. */
          {
            char *pm ;       /* Print method ptr. */

            pm = get_print_method(*f_p) ;
            if (pm) STRCPY(cmd, (char *) pm) ;
          }
        else STRCPY(cmd, method) ;     /* Use custom print. */

        if (!cmd || isempty((WCHAR *) cmd))
          {
            SPRINTF(cmd, Str[(int) M_NOPRINT_METHOD], (*f_p)->name) ;
            (void) notice(cmd, MGET("Ok"), NULL) ;
            return ;  /* Stop printing all files if error detected. */
          }

/* Expand $FILE & $ARG */

      EXPAND_KEYWORDS(cmd, (char *) f->path,
                      (char *) (*f_p)->name, (char *) NULL) ;

      fm_msg(FALSE, Str[(int) M_PRINTING], (*f_p)->name) ;
      for (j = 0; j < copies; j++)
        {
          fm_busy_cursor(TRUE, wno) ;
          error = fm_run_str(cmd, FALSE) ;
          fm_busy_cursor(FALSE, wno) ;
          if (error)
            {
              fm_msg(TRUE, Str[(int) M_UNABLE_PRINT], (*f_p)->name) ;
              return ;     /* Stop printing all files if error detected. */
            }
        }
    }

  if (found == FALSE) fm_msg(TRUE, Str[(int) M_NOSEL]) ;
}


void
put_pos_resource(pstr, rtype, value)
char *pstr, *value ;
enum res_type rtype ;
{
  char str[MAXLINE] ;

  SPRINTF(str, "%s%s", pstr, Fm->fm_res[(int) rtype]) ;
  put_resource(FM_DIR_DB, str, value) ;
}


void
put_win_info(wno, rstr, value)  /* Save window info. in deskset database. */
int wno ;
char *rstr, *value ;
{
  char str[MAXLINE] ;

       if (wno == WNO_TREE) STRCPY(str,  "tree") ;
  else if (wno == WASTE)    STRCPY(str,  "wastebasket") ;
  else                      SPRINTF(str, "window%1d", wno) ;
  STRCAT(str, rstr) ;
  put_resource(FM_DESKSET_DB, str, value) ;
}


char *
quote_file_sel(wno)
int wno ;
{
  int len ;
  int nents = 0 ;
  int tsize = 0 ;
  char *ptr ;
  char *buf = NULL ;
  char in[MAXPATHLEN], out[MAXPATHLEN] ;
  File_Pane_Object **f_p, **l_p ;             /* Files array pointers */
  struct env_list *head = (struct env_list *) 0 ;
  struct env_list *temp ;

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p != l_p; f_p++)
    if ((*f_p)->selected)
      if ((temp = (struct env_list *) malloc(sizeof(struct env_list))) != 0)
        {
          if (strlen((char *) (*f_p)->name))
            {
              STRCPY(in, (char *) Fm->file[wno]->path) ;
              STRCAT(in, "/") ;
              STRCAT(in, (char *) (*f_p)->name) ;
              escape_arg(out, in) ;

              len = strlen(out) + 1 ;
              if ((temp->buffer = (char *) malloc(sizeof(char) * len)) != 0)
                {
                  STRCPY(temp->buffer, out) ;
                  temp->size = len ;
                  tsize += len ;
                  env_insert(temp, &head) ;
                  nents++ ;
                }
            }
        }

  tsize += nents ;     /* Allow room separating spaces. */
  if (nents > 0)
    if ((buf = (char *) malloc(tsize)) != 0)
      {
        ptr = buf ;
        temp = head ;
        while (temp)
          {
            if (temp != head) *ptr++ = ' ' ;
            STRCPY((char *) ptr, temp->buffer) ;
            while (*ptr) ptr++ ;
            temp = temp->next ;
          }
        *ptr = '\0' ;
      }

  temp = head ;
  while (temp)
    {
      FREE(temp->buffer) ;
      FREE((char *) temp) ;
      temp = temp->next ;
    }
  return(buf) ;
}


/*  Decode functions for RT_BYTE_ENCODED images (taken from pr/pr_io.c):
 *
 *  The "run-length encoding" is of the form
 *
 *       <byte><byte>...<ESC><0>...<byte><ESC><count><byte>...
 *
 *  where the counts are in the range 0..255 and the actual number of
 *  instances of <byte> is <count>+1 (i.e. actual is 1..256). One- or
 *  two-character sequences are left unencoded; three-or-more character
 *  sequences are encoded as <ESC><count><byte>.  <ESC> is the character
 *  code 128.  Each single <ESC> in the input data stream is encoded as
 *  <ESC><0>, because the <count> in this scheme can never be 0 (actual
 *  count can never be 1).  <ESC><ESC> is encoded as <ESC><1><ESC>.
 *
 *  This algorithm will fail (make the "compressed" data bigger than the
 *  original data) only if the input stream contains an excessive number of
 *  one- and two-character sequences of the <ESC> character.
 */

int
read_encoded(fp, icnt, src, ocnt)
register FILE *fp ;
register int icnt ;
register u_char *src ;
register int ocnt ;
{
  register u_char c ;
  register int repeat ;

  while (1)
    {
      while (--icnt >= 0 && --ocnt >= 0 && (c = getc(fp)) != ESCAPE)
        *src++ = c ;

      if (ocnt < 0 || --icnt < 0) break ;

      if ((repeat = getc(fp)) == 0) *src++ = c ;
      else
        {
          if ((ocnt -= repeat) < 0 || --icnt < 0) break ;
          c = getc(fp) ;
          do
            {
              *src++ = c ;
            }
          while (--repeat != -1) ;
        }
    }

       if (ocnt < 0)  icnt-- ;
  else if (icnt < -1) ocnt-- ;
  if ((icnt += 2) > 0) return(icnt) ;
  return(-(++ocnt)) ;
}


void
read_str(str, value)
char **str, *value ;
{
  ZFREE(*str) ;
  if (value != NULL && strlen(value))
    {
      *str = (char *) malloc((unsigned) (strlen(value) + 1)) ;
      STRCPY(*str, value) ;
    }
  else *str = NULL ;
}


void
read_window_info(wno, info)
int wno, info[MAXINFO] ;
{
  int b, i ;

  for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
  if (get_bool_win_info(wno, DGET("Closed"), &b)) info[0] = (b ? 1 : 0) ;
  if (get_int_win_info(wno,  DGET("IconXPosition"), &i))   info[1] = i ;
  if (get_int_win_info(wno,  DGET("IconYPosition"), &i))   info[2] = i ;
  if (get_int_win_info(wno,  DGET("WindowXPosition"), &i)) info[3] = i ;
  if (get_int_win_info(wno,  DGET("WindowYPosition"), &i)) info[4] = i ;
  if (get_int_win_info(wno,  DGET("Width"),  &i))          info[5] = i ;
  if (get_int_win_info(wno,  DGET("Height"), &i))          info[6] = i ;
}


void
read_window_positions(noswins)   /* Read window information and display. */
int noswins ;
{
  char *path, str[MAXPATHLEN] ;
  int info[MAXINFO], n, wno ;

  for (wno = 0; wno < noswins; wno++)
    {
      n = WIN_SPECIAL + wno + 1 ;
      read_window_info(n, info) ;
           if (get_str_win_info(n, DGET("Path"), str) &&
               access(str, F_OK) == 0) path = str ;
      else if ((path = getenv("PWD")) == NULL) path = (char *) Fm->home ;
      FM_NEW_FOLDER_WINDOW("", path, NULL, FM_DISK, 0, FALSE, info) ;
    }
}


void
rcp_make()
{
  int x ;

  fm_popup_create(FM_RCP_POPUP) ;
  link_rcp_items() ;

  fm_add_help(FM_RCP_REMOTE_PANEL,   "filemgr:Rcp_Panel") ;
  fm_add_help(FM_RCP_SOURCE_MC,      "filemgr:Rcp_Source") ;
  fm_add_help(FM_RCP_SOURCE,         "filemgr:Rcp_Source") ;
  fm_add_help(FM_RCP_DESTINATION_MC, "filemgr:Rcp_Destination") ;
  fm_add_help(FM_RCP_DESTINATION,    "filemgr:Rcp_Destination") ;
  fm_add_help(FM_RCP_COPY_BUTTON,    "filemgr:Rcp_Copy") ;
  fm_add_help(FM_RCP_CANCEL,         "filemgr:Rcp_Cancel") ;

  set_default_item(FM_RCP_REMOTE_PANEL, FM_RCP_COPY_BUTTON) ;

  x = get_item_int_attr(FM_RCP_SOURCE_MC, FM_ITEM_VALX) ;
  set_item_int_attr(FM_RCP_SOURCE, FM_ITEM_VALX, x) ;
  x = get_item_int_attr(FM_RCP_DESTINATION_MC, FM_ITEM_VALX) ;
  set_item_int_attr(FM_RCP_DESTINATION, FM_ITEM_VALX, x) ;

  fm_center_rcp_buttons(FM_RCP_REMOTE_PANEL, FM_RCP_COPY_BUTTON,
                        FM_RCP_CANCEL) ;
  fm_position_popup(Fm->curr_wno, FM_RCP_RCP_FRAME) ;
  set_item_int_attr(FM_RCP_RCP_FRAME, FM_ITEM_SHOW, TRUE) ;
}


/*  J. Kercheval  Wed, 02/20/1991  22:29:01  Released to Public Domain.
 *
 *  Match the pattern PATTERN against the string TEXT;
 *  return TRUE if it matches, FALSE otherwise.
 *
 *  A match means the entire string TEXT is used up in matching.
 *
 *  In the pattern string:
 *       `*' matches any sequence of characters
 *       `?' matches any character
 *       [SET] matches any character in the specified set,
 *       [!SET] or [^SET] matches any character not in the specified set.
 *
 *  Note: the standard regex character '+' (one or more) should by
 *        simulated by using "?*" which is equivelant here.
 *
 *  A set is composed of characters or ranges; a range looks like
 *  character hyphen character (as in 0-9 or A-Z).
 *  [0-9a-zA-Z_] is the set of characters allowed in C identifiers.
 *  Any other character in the pattern must be matched exactly.
 *
 *  To suppress the special syntactic significance of any of `[]*?!^-\',
 *  and match the character exactly, precede it with a `\'.
 */

int
regex_match(p, t)
char *p, *t ;
{
  char range_start, range_end ;  /* Start and end in range */
  int invert ;                   /* Is this [..] or [!..] */
  int member_match ;             /* Have I matched the [..] construct? */
  int loop ;                     /* Should I terminate? */

  for ( ; *p; p++, t++)
    {

/* If this is the end of the text then this is the end of the match. */

      if (!*t) return(( *p == '*' && *++p == '\0') ? TRUE : REGEX_ABORT) ;

/* Determine and react to pattern type. */

      switch (*p)
        {

/* Single any character match. */

          case '?' : break ;

/* Multiple any character match. */

          case '*' : return(regex_match_after_star(p, t)) ;

/* [..] construct, single member/exclusion character match */

          case '[' : {

                       p++ ;   /* Move to beginning of range. */

/* Check if this is a member match or exclusion match. */

                       invert = FALSE ;
                       if (*p == '!' || *p == '^')
                         {
                           invert = TRUE ;
                           p++ ;
                         }

/* If closing bracket here or at range start then it's a malformed pattern. */

                       if (*p == ']') return(REGEX_ABORT) ;

                       member_match = FALSE ;
                       loop = TRUE ;

                       while (loop)
                         {

/* If end of construct then loop is done. */

                           if (*p == ']')
                             {
                               loop = FALSE ;
                               continue ;
                             }

/* Matching a '!', '^', '-', '\' or a ']' */

                           if (*p == '\\') range_start = range_end = *++p ;
                           else            range_start = range_end = *p ;

/* If end of pattern then bad pattern (Missing ']'). */

                           if (!range_start) return(REGEX_ABORT) ;

/* Move to next pattern char. */

                           p++ ;

/* Check for range bar. */

                           if (*p == '-')
                             {
                               range_end = *++p ;   /* Get the range end. */

/* Special character range end. */

                               if (range_end == '\\') range_end = *++p ;

/* If end of pattern or construct then bad pattern. */

                               if (range_end == '\0' || range_end == ']')
                                 return(REGEX_ABORT) ;
                             }

/*  If the text character is in range then match found.  Make sure the range
 *  letters have the proper relationship to one another before comparison.
 */
                          if (range_start < range_end)
                            {
                              if (*t >= range_start && *t <= range_end)
                                {
                                  member_match = TRUE ;
                                  loop = FALSE ;
                                }
                            }
                          else
                            {
                              if (*t >= range_end && *t <= range_start)
                                {
                                  member_match = TRUE ;
                                  loop = FALSE ;
                                }
                            }
                        }

/*  If there was a match in an exclusion set then no match.
 *  If there was no match in a member set then no match.
 */
                      if ((invert && member_match) ||
                         !(invert || member_match)) return(FALSE) ;

/*  If this is not an exclusion then skip the rest of the [...] construct that
 *  already matched.
 */
                      if (member_match)
                        {
                          while (*p != ']')
                            {

/* Bad pattern (Missing ']'). */

                              if (!*p) return(REGEX_ABORT) ;

/* Skip exact match */
                              if (*p == '\\') p++ ;

/* Move to next pattern char. */

                              p++ ;
                            }
                        }
                      break ;
                    }

/*  Next character is quoted and must match exactly.
 *  Move pattern pointer to quoted char and fall through.
 */

          case '\\' : p++ ;

/* Must match this character exactly. */

          default   : if (*p != *t) return(FALSE) ;
        }
    }

/* If end of text not reached then the pattern fails. */

  return(!*t);
}


/* recursively call b_match with final segment of PATTERN and of TEXT. */

int
regex_match_after_star(p, t)
char *p, *t ;
{
  int match, nextp ;

  while (*p == '?' || *p == '*')   /* Pass over existing ? and * in pattern. */
    {
      if (*p == '?')               /* Take one char for each ? */
        if (!*t++)
          return(REGEX_ABORT) ;    /* If end of text then no match. */

      p++ ;                        /* Move to next char in pattern. */
    }

/* If end of pattern we have matched regardless of text left. */

  if (!*p) return(TRUE) ;

/* Get the next character to match which must be a literal or '['. */

  nextp = *p ;
  if (nextp == '\\') nextp = p[1] ;

/* Continue until we run out of text or definite result seen. */

  match = FALSE ;
  while (match == FALSE)
    {

/*  A precondition for matching is that the next character in the pattern
 *  match the next character in the text or that the next pattern is the
 *  beginning of a range.  Increment text pointer as we go here.
 */

      if ( *p == *t || nextp == '[' ) match = regex_match(p, t) ;

/* If the end of text is reached then no match. */

      if ( !*t++ ) match = REGEX_ABORT ;
    }
  return(match) ;
}


void
remove_delete_path(file)
WCHAR *file ;                  /* File to delete. */
{
  Delete_item cp, pp = NULL ;  /* Current ptr, previous ptr. */

  if (!file)
    {
      FPRINTF(stderr, "%s", Str[(int) M_EMPTY_FILE]);
      return ;
    }

/* Loop thru until we find the item to delete. */

  for (cp = Fm->DelList; cp; cp = cp->next)
    if (!EQUAL((char *) cp->file, (char *) file))
      pp = cp ;          /* Set previous ptr and go to next node. */
    else
      break ;            /* File found. */

  if (!cp)
    {
      if (Fm->debug) FPRINTF(stderr, Str[(int) M_NO_DELETE], file) ;
      return ;
    }

  if (!pp)          /* At start of list. */
    {
      pp = cp ;
      Fm->DelList = cp->next ;
    }

/* Remove the item from the list, and free its memory. */

  pp->next = cp->next ;
  FREE((char *) cp->path) ;
  FREE((char *) cp->file) ;
  FREE((Delete_item) cp) ;
}


void
reset_advanced_panel()
{
  set_item_int_attr(FM_PO_LINK_I,     FM_ITEM_IVALUE, !Fm->follow_links) ;
  set_item_int_attr(FM_PO_CDROM_I,    FM_ITEM_IVALUE, !Fm->show_cd) ;
  set_item_int_attr(FM_PO_INTERVAL_I, FM_ITEM_IVALUE, Fm->interval) ;
  set_item_int_attr(FM_PO_CD_CONTENT,     FM_ITEM_IVALUE, !Fm->cd_content) ;
  set_item_int_attr(FM_PO_FLOPPY_CONTENT, FM_ITEM_IVALUE, !Fm->floppy_content) ;
  set_item_str_attr(FM_PO_FILTER_I,   FM_ITEM_IVALUE, (char *) Fm->filter) ;
  set_item_str_attr(FM_PO_PRINT_I, FM_ITEM_IVALUE, (char *) Fm->print_script) ;

  set_item_int_attr(FM_PO_EDITOR_I,    FM_ITEM_IVALUE,  (int) Fm->editor) ;
  set_item_str_attr(FM_PO_OTHER_EDITOR_I, FM_ITEM_IVALUE, Fm->other_editor) ;
  set_item_int_attr(FM_PO_OTHER_EDITOR_I, FM_ITEM_SHOW,
       get_item_int_attr(FM_PO_EDITOR_I, FM_ITEM_IVALUE) == FM_OTHER_EDITOR) ;
}


void
reset_cur_folder_panel()
{
  int wno = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;

  set_item_int_attr(FM_PO_CURF_NAMELEN,  FM_ITEM_IVALUE, f->num_file_chars) ;
  set_item_int_attr(FM_PO_CURF_LAYOUT,   FM_ITEM_IVALUE, !f->dispdir) ;
  set_item_int_attr(FM_PO_CURF_DISPLAY,  FM_ITEM_IVALUE, f->display_mode) ;
  set_item_int_attr(FM_PO_CURF_CONTENT,  FM_ITEM_IVALUE, f->content_types) ;
  set_item_int_attr(FM_PO_CURF_SORTBY,   FM_ITEM_IVALUE, f->sortby) ;
  set_item_int_attr(FM_PO_CURF_HIDDEN,   FM_ITEM_IVALUE, (f->see_dot ? 1 : 0)) ;
  set_item_int_attr(FM_PO_CURF_LISTOPTS, FM_ITEM_IVALUE, f->listopts) ;

  if (f->display_mode == VIEW_BY_ICON)
    {
       set_item_int_attr(FM_PO_CURF_LAYOUT, FM_ITEM_INACTIVE, FALSE);
       set_item_int_attr(FM_PO_CURF_LISTOPTS, FM_ITEM_INACTIVE, TRUE);
       set_item_int_attr(FM_PO_CURF_CONTENT, FM_ITEM_INACTIVE, TRUE);
    }
  else if (f->display_mode == VIEW_BY_LIST)
    {
       if (f->listopts)
         set_item_int_attr(FM_PO_CURF_LAYOUT, FM_ITEM_INACTIVE, TRUE);
       else
         set_item_int_attr(FM_PO_CURF_LAYOUT, FM_ITEM_INACTIVE, FALSE);
       set_item_int_attr(FM_PO_CURF_LISTOPTS, FM_ITEM_INACTIVE, FALSE);
       set_item_int_attr(FM_PO_CURF_CONTENT, FM_ITEM_INACTIVE, TRUE);
    }
  else
    {
       set_item_int_attr(FM_PO_CURF_LAYOUT, FM_ITEM_INACTIVE, FALSE);
       set_item_int_attr(FM_PO_CURF_LISTOPTS, FM_ITEM_INACTIVE, TRUE);
       set_item_int_attr(FM_PO_CURF_CONTENT, FM_ITEM_INACTIVE, FALSE);
    }
}


void
reset_general_panel()
{
  set_item_int_attr(FM_PO_OPEN_FOLDER, FM_ITEM_IVALUE,  Fm->newwin) ;
  set_item_int_attr(FM_PO_TREE_DIR,    FM_ITEM_IVALUE,  !Fm->tree_vertical) ;
  set_item_int_attr(FM_PO_DELETE_I,    FM_ITEM_IVALUE,  !Fm->confirm_delete) ;
  set_item_str_attr(FM_PO_DEFDOC, FM_ITEM_IVALUE, Str[(int) M_NEWDOCUMENT]) ;
  set_item_int_attr(FM_PO_DEL_MES, FM_ITEM_SHOW, TRUE) ;
  set_item_int_attr(FM_PO_DES_MES, FM_ITEM_SHOW, TRUE) ;
}


void
reset_goto_panel()
{
  load_goto_list() ;           /* Load scrolling list with menu values. */
  Fm->goto_changed = FALSE ;
  set_item_str_attr(FM_PO_PATHNAME,    FM_ITEM_IVALUE, "") ;
  set_item_str_attr(FM_PO_GOTO_MLABEL, FM_ITEM_IVALUE, "") ;
  set_item_int_attr(FM_PO_GOTO_NUMBER, FM_ITEM_IVALUE, Fm->maxgoto) ;
}


void
reset_new_folder_panel()
{
  set_item_str_attr(FM_PO_NEWF_DEFNAME,  FM_ITEM_IVALUE,
                    Str[(int) M_NEWFOLDER]) ;
  set_item_int_attr(FM_PO_NEWF_NAMELEN,  FM_ITEM_IVALUE, Fm->num_file_chars) ;
  set_item_int_attr(FM_PO_NEWF_LAYOUT,   FM_ITEM_IVALUE, !Fm->dispdir) ;
  set_item_int_attr(FM_PO_NEWF_DISPLAY,  FM_ITEM_IVALUE, Fm->display_mode) ;
  set_item_int_attr(FM_PO_NEWF_CONTENT,  FM_ITEM_IVALUE, Fm->content_types) ;
  set_item_int_attr(FM_PO_NEWF_SORTBY,   FM_ITEM_IVALUE, Fm->sortby) ;
  set_item_int_attr(FM_PO_NEWF_HIDDEN, FM_ITEM_IVALUE, (Fm->see_dot ? 1 : 0)) ;
  set_item_int_attr(FM_PO_NEWF_LISTOPTS, FM_ITEM_IVALUE, Fm->listopts) ;

  if (Fm->display_mode == VIEW_BY_ICON)
    {
       set_item_int_attr(FM_PO_NEWF_LAYOUT, FM_ITEM_INACTIVE, FALSE);
       set_item_int_attr(FM_PO_NEWF_LISTOPTS, FM_ITEM_INACTIVE, TRUE);
       set_item_int_attr(FM_PO_NEWF_CONTENT, FM_ITEM_INACTIVE, TRUE);
    }
  else if (Fm->display_mode == VIEW_BY_LIST)
    {
       if (Fm->listopts)
         set_item_int_attr(FM_PO_NEWF_LAYOUT, FM_ITEM_INACTIVE, TRUE);
       else
         set_item_int_attr(FM_PO_NEWF_LAYOUT, FM_ITEM_INACTIVE, FALSE);
       set_item_int_attr(FM_PO_NEWF_LISTOPTS, FM_ITEM_INACTIVE, FALSE);
       set_item_int_attr(FM_PO_NEWF_CONTENT, FM_ITEM_INACTIVE, TRUE);
    }
  else
    {
       set_item_int_attr(FM_PO_NEWF_LAYOUT, FM_ITEM_INACTIVE, FALSE);
       set_item_int_attr(FM_PO_NEWF_LISTOPTS, FM_ITEM_INACTIVE, TRUE);
       set_item_int_attr(FM_PO_NEWF_CONTENT, FM_ITEM_INACTIVE, FALSE);
    }
}


void
reset_prop_values(value)
int value ;
{
  write_item_footer(FM_PO_PO_FRAME, "", FALSE) ;
  switch (value)
    {
      case PROPS_STACK_GENERAL    : reset_general_panel() ;
                                    break ;
      case PROPS_STACK_NEW_FOLDER : reset_new_folder_panel() ;
                                    break ;
      case PROPS_STACK_CUR_FOLDER : reset_cur_folder_panel() ;
                                    break ;
      case PROPS_STACK_GOTO       : reset_goto_panel() ;
                                    break ;
      case PROPS_STACK_CUSTOM     : reset_custom_panel() ;
                                    break ;
      case PROPS_STACK_ADVANCED   : reset_advanced_panel() ;
    }
  keep_panel_up(FM_PO_PO_FRAME) ;
}


void
save_delete_path(path, file)
WCHAR *path, *file ;
{
  Delete_item di ;

  di = (Delete_item) LINT_CAST(fm_malloc(sizeof(Delete_Object))) ;

  di->path = (WCHAR *) strdup((char *) path) ;
  di->file = (WCHAR *) strdup((char *) file) ;

/* Link us at the start of the list. */

  di->next = Fm->DelList ;
  Fm->DelList = di ;
}


void
scroll_to_file_and_select(dir, file)
char *dir, *file ;
{
  File_Pane_Object **curr, **end ;
  int n, w ;

  for (w = 0; w < Fm->num_win; w++)
    if ((char *) Fm->file[w]->path != NULL)
      if (EQUAL(dir, (char *) Fm->file[w]->path))
        {
          n = 0 ;
          end = PTR_LAST(w) ;
          for (curr = PTR_FIRST(w); curr != end; curr++, n++)
            {
              if (EQUAL((char *) (*curr)->name, file))
                {
                  Fm->file[w]->object[n]->selected = TRUE ;
                  draw_ith(n, w) ;
                  fm_scroll_to(n, w) ;
                }
            }
        }
}


void
set_ccmd_str_resource(n, rstr, value)
int n ;
char *rstr, *value ;
{
  char str[MAXLINE] ;

  if (rstr != NULL && !isempty((WCHAR *) rstr))
    {
      SPRINTF(str, "userCommand%1d%s", n, rstr) ;
      put_resource(FM_DESKSET_DB, str, value) ;
    }
}


void
set_default(def)    /* Nothing or obsolete; set defaults... */
int def ;
{
  register int i ;
  static u_char r[3] = { 171, 161, 214 } ;
  static u_char g[3] = { 160, 214, 161 } ;
  static u_char b[3] = { 255, 214, 205 } ;

  switch (def)
    {
      case DEF_VARIOUS : Fm->display_mode = VIEW_BY_ICON ;
                         Fm->content_types = CON_MONO_ICON | CON_MONO_IMAGE ;
                         if (Fm->color) Fm->content_types |= CON_COLOR_ICON ;
                         Fm->sortby = SORT_BY_NAME ;
                         Fm->see_dot  = FALSE ;
                         Fm->listopts = 0 ;   /* Set list options to nothing. */
                         Fm->dispdir = FM_DISP_BY_ROWS ;
                         Fm->editor = FM_TEXTEDIT ;
                         Fm->confirm_delete = TRUE ;
                         Fm->num_file_chars = 15 ;
                         Fm->treeview = FALSE ;
                         break ;

      case DEF_PSCRIPT :

#ifdef SVR4
                         STRCPY((char *) Fm->print_script,
                                "cat $FILE | mp -l -o | lp -s") ;
#else
                         STRCPY((char *) Fm->print_script,
                                "cat $FILE | mp -l -o | lpr") ;
#endif /*SVR4*/
                         break ;

      case DEF_FILTER  : Fm->filter[0] = 0 ;
                         break ;

      case DEF_EDITOR  : read_str((char **) &Fm->other_editor, OTHER_EDITOR) ;
                         break ;

      case DEF_COLOR   : for (i = 0; i < 3; i++)
                           {
                             Fm->red[Fm->folder_color + i]   = r[i] ;
                             Fm->green[Fm->folder_color + i] = g[i] ;
                             Fm->blue[Fm->folder_color + i]  = b[i] ;
                           }
                         break ;

      case DEF_SHTYPE  : read_str(&Fm->shellname, "cmdtool") ;
                         break ;

      case DEF_WASTE   : Fm->waste_info[0] = 1 ;     /* Iconic WB. */
                         for (i = 1; i < 7; i++)
                           Fm->waste_info[i] = -1 ;
    }
}


int
set_fp_image(path, wno, off)
char *path ;
int wno, off ;
{
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object *f_p ;
  int depth = 1 ;
  int idepth ;

  f_p = f->object[off] ;
  if (path == NULL)
    {
      f_p->image = 0 ;
      return(depth) ;
    }
  if (f->content_types & CON_MONO_IMAGE)
    f_p->image = get_raster_content(path) ;
  if (f_p->image == 0 && f->content_types & CON_MONO_ICON)
    f_p->image = get_icon_content(path) ;
  if (f_p->image == 0 && f->content_types & CON_MONO_IMAGE)
    f_p->image = get_bitmap_content(path) ;
  if (f_p->image == 0 && f->content_types & CON_COLOR_ICON)
    {
      f_p->image = get_xpm_content(path, &idepth) ;
      if (f_p->image != 0) depth = idepth ;
    }
  return(depth) ;
}


void
set_goto_resource(gtype, n, value)
enum goto_type gtype ;
int n ;
char *value ;
{
  char str[MAXLINE] ;

  if (gtype == FM_GOTO_PATH) SPRINTF(str, "userGotoPathname%1d", n) ;
  else                       SPRINTF(str, "userGotoLabel%1d", n) ;
  put_resource(FM_DESKSET_DB, str, value) ;
}


void
usage(progname)
char *progname ;
{

/* Unrecognised arguments were found; give user a usage statement. */

  fm_msg(TRUE, Str[(int) M_USAGE], progname) ;
  exit(0) ;
}


int
valid_advanced_panel()
{
  char *s, buf[256] ;

/* Check other editor. */

  s = get_item_str_attr(FM_PO_OTHER_EDITOR_I, FM_ITEM_IVALUE) ;

  if (!strlen(Fm->other_editor) ||
      !EQUAL((char *) Fm->other_editor, s))   /* Other editor changed? */
    {
      if (get_item_int_attr(FM_PO_EDITOR_I,
                            FM_ITEM_IVALUE) == FM_OTHER_EDITOR &&
          !find_pattern(s, "$FILE"))          /* Check for $FILE */
        {
          int choice ;

/* Give warning and get user's choice. */

          choice = give_file_warning() ;

          if (choice == FILE_CANCEL) return(FALSE) ;  /* User cancelled. */
          else if (choice == FILE_ADDFILE)
            {
              SPRINTF(buf, "%s $FILE", s) ;    /* User wants $FILE added */
              set_item_str_attr(FM_PO_OTHER_EDITOR_I, FM_ITEM_IVALUE, buf) ;
            }
          else if (choice != FILE_CONTINUE)           /* User continued? */
            ERR_EXIT(Str[(int) M_INV_NOTICE]) ;
        }
    }

/* Check printer. */

  s = get_item_str_attr(FM_PO_PRINT_I, FM_ITEM_IVALUE) ;

  if (!EQUAL((char *) Fm->print_script, s))    /* Printer changed? */
    {
      if (!find_pattern(s, "$FILE"))           /* Check for $FILE */
        {
          int choice ;

/* Give warning and get user's choice. */

          choice = give_file_warning() ;

          if (choice == FILE_CANCEL) return(FALSE) ;   /* User cancelled */
          else if (choice == FILE_ADDFILE)
            {
              SPRINTF(buf, "%s $FILE", s) ;    /* User wants $FILE added */
              set_item_str_attr(FM_PO_PRINT_I, FM_ITEM_IVALUE, buf) ;
            }
          else if (choice != FILE_CONTINUE)            /* User continued? */
            ERR_EXIT(Str[(int) M_INV_NOTICE]) ;
        }
    }
  return(TRUE) ;
}


void
write_cmdline()        /* Write out user supplied command line options. */
{
  int argc ;
  char *argv[9], buf[MAXLINE] ;

  argc = 0 ;
  if (Fm->check_all == TRUE)            argv[argc++] = "-a" ;
  if (Fm->use_ce    == FALSE)           argv[argc++] = "-C" ;
  if (Fm->dispdir   == FM_DISP_BY_COLS) argv[argc++] = "-c" ;
  if (Fm->dispdir   == FM_DISP_BY_ROWS) argv[argc++] = "-r" ;
  if (Fm->debug     == TRUE)            argv[argc++] = "-D" ;

  if (Fm->dirpath != NULL)
    {
      argv[argc++] = "-d" ;
      argv[argc++] = Fm->dirpath ;
    }

  argv[argc++] = "-i" ;
  SPRINTF(buf, "%1d", Fm->interval) ;
  argv[argc++] = buf ;

  save_cmdline(argc, argv) ;
}


void
write_questions()
{
  add_com_text(Str[(int) M_COM_TITLE]) ;
  add_com_text(Str[(int) M_COM_LONG]) ;
  add_com_text(Str[(int) M_COM_TIME]) ;
  add_com_text(Str[(int) M_COM_PRIMARY]) ;
  add_com_text(Str[(int) M_COM_OTHER]) ;
  add_com_text(Str[(int) M_COM_APPS]) ;
  add_com_text(Str[(int) M_COM_USAGE]) ;
  add_com_text(Str[(int) M_COM_USAGE1]) ;
  add_com_text(Str[(int) M_COM_USAGE2]) ;
  add_com_text(Str[(int) M_COM_COM]) ;
  add_com_text(Str[(int) M_COM_DIVIDE]) ;
}
