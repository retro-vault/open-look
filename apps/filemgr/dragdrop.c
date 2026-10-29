#ifndef lint
static char sccsid[]="@(#)dragdrop.c	1.85 01/11/95 Copyright 1987-1992 Sun Microsystem, Inc." ;
#endif /*   */
/*  Copyright (c) 1989-1992 Sun Microsystems, Inc.
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

#ifdef __STDC__
#include <stdarg.h>              /* For variable length declarations. */
#else
#include <varargs.h>
#endif /*__STDC__*/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/X.h>
#include <xview/defaults.h>
#include <xview/notice.h>

#include "defs.h"
#include "fm.h"
#include "xdefs.h"

extern FmVars Fm ;
extern FMXlib X ;
extern char *Str[] ;

static const unsigned short folder_drag_move_image[] = {
#include "images/fol_drag_move.icon"
} ;

static const unsigned short folders_drag_move_image[] = {
#include "images/fols_drag_move.icon"
} ;

static const unsigned short folder_drag_copy_image[] = {
#include "images/fol_drag_copy.icon"
} ;

static const unsigned short folders_drag_copy_image[] = {
#include "images/fols_drag_copy.icon"
} ;

static const unsigned short folder_accept_move_image[] = {
#include "images/fol_acc_move.icon"
} ;

static const unsigned short folders_accept_move_image[] = {
#include "images/fols_acc_move.icon"
} ;

static const unsigned short folder_accept_copy_image[] = {
#include "images/fol_acc_copy.icon"
} ;

static const unsigned short folders_accept_copy_image[] = {
#include "images/fols_acc_copy.icon"
} ;

static Boolean copy_by_filename     P((char *, char *, Catch, int)) ;

static Catch create_catch           P((Event_Context)) ;

static int atom_in_list             P((Atom, Atom *, int)) ;
static int dnd_convert              P((Selection_owner, Atom *, Xv_opaque *,
                                       long *, int *)) ;

static Notify_value clear_context_itimer_func P((Selection_requestor, int)) ;

static Pitch create_pitch           P((Event_Context, int)) ;

static void add_dlist               P((struct Dnd_Display_Struct **,
                                       char *, char *)) ;
static void add_to_plist            P((struct Pitch_List_Struct **, Pitch)) ;
static void begin_item_transfer     P((Selection_requestor)) ;
static void cleanup_pitch           P((Pitch)) ;
static void clear_context           P((Catch, int)) ;
static void close_tmp_file          P((Catch)) ;
static void count_reply_proc        P((Selection_requestor, Atom, Atom,
                                       Xv_opaque, unsigned long, int)) ;
static void delete_from_plist       P((struct Pitch_List_Struct **, Pitch)) ;
static void destroy_dlist           P((struct Dnd_Display_Struct **)) ;
static void display_dlist           P((struct Dnd_Display_Struct **)) ;
static void dnd_adjust              P((Catch)) ;
static void dnd_catch_proc          P((Selection_requestor, Atom, Atom,
                                       Xv_opaque, unsigned long, int)) ;
static void get_item_bits           P((Selection_requestor)) ;
static void init_folder_cursors     P(()) ;
static void lose_clipboard          P((Selection_owner)) ;
static void make_dnd_filename       P((Catch, char *, int, int)) ;
static void print_atom_list         P((Atom *, int)) ;
static void resize_drop_site        P((Xv_Window, Xv_Window)) ;
static void recompute_canvas_size   P((int)) ;

static Xv_Cursor init_folder_cursor P((const unsigned short *)) ;


/* For debugging. */

static void
print_atom_list(atom_list, length)
Atom *atom_list ;
int length ;
{
  int  i ;
  char *name ;

  if (!atom_list) return ;

  for (i = 0; i < length; i++)
    {
      name = (atom_list[i] > 0 ? XGetAtomName(X->display, atom_list[i])
                               : "[none]") ;
      dout("\t\tAtom %2d = %s\n", i, name) ;
    }
}


static int
atom_in_list(atom, atom_list, length)
Atom atom ;
Atom *atom_list ;
int length ;
{
  int i ;

  if (!atom_list)
    {
      DB2("atom_in_list()\t No list %d found for atom %d\n",
          atom_list, atom) ;
      return(FALSE) ;
    }

  for (i = 0; i < length; i++)
    if (atom_list[i] == atom) return(TRUE) ;

  DB2("atom_in_list()\t Atom %d not found in list %d\n", atom, atom_list) ;

  return(FALSE) ;
}


/* Clears context block in preparation for a new drag/drop transaction. */

static void
clear_context(catch, all_state)
Catch catch ;
Boolean all_state ;
{

/*  Note: save these fields for subsequent requests:
 *        destdir, over_file, drop_method
 */

  if (all_state)
    {
      catch->items_caught = 0 ;
      catch->num_items    = 0 ;
      catch->destdir[0]   = 0 ;

      ZFREE(catch->over_file) ;
      ZFREE(catch->drop_method) ;
    }

  ZFREE(catch->target_list) ;
  ZFREE(catch->transport_list) ;
  ZFREE(catch->data_label) ;
  ZFREE(catch->src_hostname) ;
  ZFREE(catch->src_filename) ;

  catch->over_folder       = FALSE ;
  catch->num_targets       = 0 ;
  catch->num_transports    = 0 ;
  catch->chosen_target     = 0 ;
  catch->chosen_transport  = 0 ;
  catch->state_mask        = 0 ;
  catch->stop_hit          = 0 ;
  catch->processing_stream = 0 ;
  catch->tmpfile[0]        = 0 ;
  catch->fp                = 0 ;
  catch->object_x          = 0 ;
  catch->object_y          = 0 ;
  catch->object_loc_good   = FALSE ;
  catch->cursor_x          = 0 ;
  catch->cursor_y          = 0 ;
  catch->cursor_loc_good   = FALSE ;
  catch->is_paste          = FALSE ;
  catch->display_list      = NULL ;
}


static Boolean
copy_by_filename(file, dest, catch, link)
char *file ;        /* Full pathname of source file. */
char *dest ;        /* Destination of where to copy/move. */
Catch catch ;
int link ;
{
  File_Pane_Object **f_p ;
  Tree_Pane_Object *dest_node ;
  char sdir[MAXPATHLEN] ;       /* Source directory of filename. */
  char *label ;                 /* Filename only. */
  struct stat fstatus ;
  Boolean isdir = FALSE ;
  char buf[MAXPATHLEN+64] ;     /* Command buffer. */
  char destfile[MAXPATHLEN] ;   /* Pathname of destination file. */
  char *s ;
  int reply ;

  STRCPY(sdir, file) ;          /* Get source directory. */
  s = strrchr(sdir, '/') ;
  if (!s) ERR_EXIT(Str[(int) M_FULLPATH]) ;

  if (!catch->data_label || !*catch->data_label)
    {
      label = ++s ;             /* Setup filename. */
      *(--s) = 0 ;              /* Separate from src dir. */
    }
  else label = catch->data_label ;

  SPRINTF(destfile, "%s/%s", dest, label) ;
  if (access(destfile, F_OK) == 0)
    {
      reply = exists_prompt(Fm->curr_wno, destfile) ;
      if (reply == FM_CANCEL) return(FALSE) ;
      if (reply == FM_NO) {
         char tmpfile[MAXPATHLEN] ;
	 unique_filename(destfile) ;
	 STRCPY(tmpfile, destfile);
         s = strrchr(tmpfile, '/') ;
         label = ++s ;             /* Setup new filename. */
         *(--s) = 0 ;              /* Separate from dest dir. */
      }
    }

/* Check for the possibility of a recursive copy; which should be aborted. */

  if (is_recursive(Fm->curr_wno, file, destfile)) return(FALSE) ;

/*  Copy the file in -- be sure to quote filenames.
 *  Use -r to copy folders.
 */

  SPRINTF(buf, "%s \"%s\" \"%s\"", link ? "ln -s" : "cp -r", file, destfile) ;
  if (fm_run_str(buf, TRUE))
    {
      fm_showerr(buf) ;
      return(FALSE) ;
    }
  fm_clrmsg() ;                        /* Clear footer. */

  fm_all_filedeselect();       /* deselect all selection */

  if (catch && !catch->is_paste && catch->dest_wno != WNO_TREE &&
      EQUAL((char *) Fm->file[catch->dest_wno]->path, dest) &&
      catch->object_loc_good && catch->cursor_loc_good &&
      !catch->over_folder && catch->x != -1 && catch->y != -1)
     f_p = make_file_obj(catch->dest_wno, label,
                        catch->x + catch->object_x - catch->cursor_x,
                        catch->y + catch->object_y - catch->cursor_y) ;

/*  If we are copying a folder, and we are displaying a tree view, then the
 *  tree node structure needs to be updated, before redisplaying the tree
 *  pane.
 */

  if (fm_stat(destfile, &fstatus) == 0)
    if ((fstatus.st_mode & S_IFMT) == S_IFDIR)
      isdir = TRUE ;

  if (isdir == TRUE)
    {
      dest_node = path_to_node(WNO_TREE, (WCHAR *) dest) ;
      if (dest_node != NULL)
        add_tree_entry(dest_node, (WCHAR *) label) ;
      fm_drawtree(TRUE) ;
    }

/* Redisplay the folders affected by the move. */

  if (catch) add_dlist(&(catch->display_list), dest, label) ;
  else       display_all_folders(FM_BUILD_FOLDER, dest) ;
  return(TRUE) ;
}


Boolean
move_by_filename(file, dest, catch)
char *file ;                     /* Full pathname of file. */
char *dest ;                     /* Destination of where to copy/move. */
Catch catch ;
{
  File_Pane_Object **f_p ;
  Tree_Pane_Object *dest_node, *src_node ;
  int reply ;
  char *s ;
  struct stat fstatus ;
  Boolean isdir   = FALSE ;
  Boolean towaste = FALSE ;
  char buf[MAXPATHLEN+64] ;      /* Command buffer. */
  char *label ;                  /* Filename only. */
  char sdir[MAXPATHLEN] ;        /* Source directory of filename. */

  STRCPY(sdir, file) ;           /* Get source directory. */
  s = strrchr(sdir, '/') ;
  if (!s) ERR_EXIT(Str[(int) M_FULLPATH]) ;

  label = ++s ;                  /* Setup filename. */
  *(--s) = 0 ;                   /* Separate from src dir. */

/* Check to see if dropped on the starting directory. */

  if (EQUAL(sdir, dest))
    {
      fm_msg(TRUE, Str[(int) M_IGNORE_DND]) ;
      return(FALSE) ;
    }

/*  Check if the source file is the users .wastebasket directory. If so,
 *  then output an error message saying that this cannot be moved.
 */

  SPRINTF(buf, "%s/%s", sdir, label) ;
  if (Fm->file[WASTE]->path && EQUAL(buf, (char *) Fm->file[WASTE]->path))
    {
      fm_msg(TRUE, Str[(int) M_NO_WASTE_MOVE]) ;
      return(FALSE) ;
    }

  SPRINTF(buf, "%s/%s", dest, label) ;
  if (access(buf, F_OK) == 0)
    {
      reply = exists_prompt(Fm->curr_wno, buf) ;
      if (reply == FM_CANCEL) return(FALSE) ;
      if (reply == FM_NO) {
	 char tmpfile[MAXPATHLEN] ;
	 unique_filename(buf) ;
	 STRCPY(tmpfile, buf);
	 s = strrchr(tmpfile, '/') ;
	 label = ++s ;             /* Setup new filename. */
	 *(--s) = 0 ;              /* Separate from dest dir. */
      }
    }

  if ((reply = do_move(file, buf)) == 0) {
     fm_clrmsg() ;    	  	  /* Clear footer. */
     fm_all_filedeselect();       /* deselect all selection */
  }

  if (reply == 0)
    {
      if (catch && !catch->is_paste && catch->dest_wno != WNO_TREE &&
          EQUAL((char *) Fm->file[catch->dest_wno]->path, dest) &&
          catch->object_loc_good && catch->cursor_loc_good &&
          !catch->over_folder && catch->x != -1 && catch->y != -1)
        f_p = make_file_obj(catch->dest_wno, label,
                            catch->x + catch->object_x - catch->cursor_x,
                            catch->y + catch->object_y - catch->cursor_y) ;

/*  If moving to/from the WB, then save/delete the old path info
 *  used for the undelete feature. If moving to the waste basket, timestamp
 *  the file with the current time.
 */

      if (X->fileX[WASTE]->frame)
        {
          if (EQUAL(dest, (char *) Fm->file[WASTE]->path))
            {
              towaste = TRUE ;
              save_delete_path((WCHAR *) sdir, (WCHAR *) label) ;
              UTIMES(buf, (struct timeval *) NULL) ;
            }
          else if (EQUAL(sdir, (char *) Fm->file[WASTE]->path))
            remove_delete_path((WCHAR *) label) ;
        }

/*  If we are moving a folder, and we are displaying a tree view, then the
 *  tree node structure needs to be updated, before redisplaying the tree
 *  pane.
 */

      SPRINTF(buf, "%s/%s", dest, label) ;
      if (fm_stat(buf, &fstatus) == 0)
        if ((fstatus.st_mode & S_IFMT) == S_IFDIR)
          isdir = TRUE ;

      if (isdir == TRUE)
        {
          src_node  = path_to_node(WNO_TREE, (WCHAR *) sdir) ;
          dest_node = path_to_node(WNO_TREE, (WCHAR *) dest) ;
          if (src_node  != NULL)
            remove_tree_entry(src_node, (WCHAR *) label) ;
          if (dest_node != NULL)
            add_tree_entry(dest_node,   (WCHAR *) label) ;
          fm_drawtree(TRUE) ;
        }

/*  If we moved a folder to the wastebasket, we need to check if there are
 *  any filemgr windows currently monitoring that directory. If so, then
 *  they need to be adjusted to monitor their parent.
 */

      if (isdir && towaste)
        {
          SPRINTF(buf, "%s/%s", sdir, label) ;
          check_del_dir(buf) ;
        }
    }
  else
    {
      char* tmpstr;
      File_Pane_Object **curr, **last;
      int i;
      last = PTR_LAST(Fm->curr_wno) ;
      tmpstr = strrchr(dest, '/');
      tmpstr++;
      for (curr = PTR_FIRST(Fm->curr_wno), i = 0; curr < last; curr++, i++)
        if (strcmp((char*)(*curr)->name , tmpstr) == 0) {
           draw_ith(i, Fm->curr_wno) ;
	   break;
	}
    }

/* Redisplay the folders affected by the move */

  if (catch)
    {
      add_dlist(&(catch->display_list), dest, label) ;
      add_dlist(&(catch->display_list), sdir, NULL) ;
    }
  else
    {
      display_all_folders(FM_BUILD_FOLDER, dest) ;
      display_all_folders(FM_BUILD_FOLDER, sdir) ;
    }

  return((reply == 0) ? TRUE : FALSE) ;
}


static void
get_item_bits(sel)
Selection_requestor sel ;
{
  int end_enum_context = -1 ;
  Catch catch = (Catch) xv_get(sel, XV_KEY_DATA, Fm->Catch_key) ;

  if (!catch)
    {
      DB("get_item_bits()\t Unable to get dnd catch") ;
      really_stop_dnd(sel, FALSE) ;
      return ;
    }

  xv_set(sel,
         SEL_REPLY_PROC, dnd_catch_proc,
         SEL_TYPES,
           X->A->enum_item,
           catch->chosen_target,
           X->A->enum_item,
           0,

         SEL_TYPE_INDEX,    0,
           SEL_PROP_TYPE,   XA_INTEGER,
           SEL_PROP_DATA,   &catch->items_caught,
           SEL_PROP_FORMAT, 32,
           SEL_PROP_LENGTH, 1,

         SEL_TYPE_INDEX,    2,
           SEL_PROP_TYPE,   XA_INTEGER,
           SEL_PROP_DATA,   &end_enum_context,
           SEL_PROP_FORMAT, 32,
           SEL_PROP_LENGTH, 1,
         0) ;

  DB2("get_item_bits()\t sel=%d, catch=%d\n", sel, catch) ;

  sel_post_req(sel) ;       /* Initiate the non-blocking request. */
}


/* Makes a unique filename for the dnd dropped file. */

static void
make_dnd_filename(c, bits, len, mode)
Catch c ;
char *bits ;                     /* Bits. */
int len ;                        /* Length of bits. */
int mode ;                       /* Mode of new file to create. */
{
  extern char *ds_expand_template() ;

  File_Pane_Object *tmp ;        /* Setup dummy for fm.bind.c routines. */
  char template[128], *file, *type ;
  char *label = NULL ;
  int fd ;

/*  If there is a data label, try that first. If there is no usable label,
 *  and there is a filename for the transfer, then try to prune the leaf
 *  off of it, and use that for the label.
 */

  if (c->data_label && *c->data_label)
    label = c->data_label ;
  else if (c->src_filename && *c->src_filename)
    {
      char *s_ptr = c->src_filename ;
      char *e_ptr = s_ptr ;

      while (*e_ptr++) continue ;
      while ((*(e_ptr - 1) != '/') && (e_ptr != s_ptr)) e_ptr-- ;

      label = e_ptr ;
    }

  if (label && *label)
    {
      if (*label == '/') STRCPY(c->tmpfile, label) ;
      else               fm_snprintf(c->tmpfile, sizeof(c->tmpfile), "%s/%s", c->destdir, label) ;

      fd = open(c->tmpfile, O_CREAT | O_EXCL, mode) ;
      if (fd >= 0)
        {
          CLOSE(fd) ;       /* Success! */
          return ;
        }

/*  Failure - the file already existed, or there was no usable template.
 *  Go try the tns template.
 */

    }

/*  Create a dummy file pane object so a tns_entry can be gotten.
 *  Get the file template associated with the tns entry.
 *  Expand the template to an acual filename.
 *  Finally, free up the dummy file pane object.
 */

  tmp = (File_Pane_Object *)
        LINT_CAST(fm_malloc((unsigned) sizeof(File_Pane_Object))) ;
  if (label && *label)
    {
      tmp->tns_entry = ce_get_entry(Fm->ceo->file_ns, 3, label, bits, len) ;
      if (tmp->tns_entry)
        {
          type = ce_get_attribute(Fm->ceo->file_ns, tmp->tns_entry,
                                  Fm->ceo->file_type) ;
          if (!type) tmp->tns_entry = NULL ;
          else tmp->tns_entry = ce_get_entry(Fm->ceo->type_ns, 1, type) ;
        }
    }
  else tmp->tns_entry = get_tns_entry_by_bits((char *)bits, len) ;
  GET_FILE_TEMPLATE(tmp, template) ;
  FREE(tmp) ;

  file = ds_expand_template(c->destdir, (char *) template, mode) ;

  if (!file) c->tmpfile[0] = '\0' ;
  else
    {
      STRCPY(c->tmpfile, file) ;
      FREE(file) ;
    }
}


/* Close temporary file where bits were written to. */

static void
close_tmp_file(catch)
Catch catch ;
{
  char *s = strrchr(catch->tmpfile, '/') ;

/*  Close open pipe or file.  catch->over_file will only be set
 *  if we are over an object (other than a folder) and this indicates
 *  a pipe was used.
 */

  if (catch->over_file) PCLOSE(catch->fp) ;   /* Close pipe. */
  else                  FCLOSE(catch->fp) ;   /* Close file. */

/* Redisplay if looking at the same folder where drop occurred. */

 if (s && !catch->over_file)
   {
     s++ ;
     add_dlist(&(catch->display_list), catch->destdir, s) ;
   }

/* We tell the source that we are all done.  */

  DB4("- object %d/%d received, data_label=%s, file=%s\n",
      catch->items_caught + 1, catch->num_items,
      catch->data_label, catch->tmpfile) ;
}


/* XXX: XView kludge: by an itimer to clean up the catch and selection. */

/*ARGSUSED*/
static Notify_value
clear_context_itimer_func(sr, which)
Selection_requestor sr ;
int which ;
{
  Catch catch = (Catch) xv_get(sr, XV_KEY_DATA, Fm->Catch_key) ;

  if (catch)
    {
      clear_context(catch, TRUE) ;
      FREE(catch) ;
    }
  XV_DESTROY_SAFE(sr) ;
  sr = '\0' ;
  return(NOTIFY_DONE) ;
}


/*  Huge monolithic routine that processes all the replies to the questions
 *  that I ask about the current drag/drop selection. These requests are
 *  made in groups, and the routine contains a state machine whose current
 *  state is stored in the dnd block. This routine updates the state machine
 *  as replies, or failed replies come in. Changes in state may require
 *  issuing new requests for data, which are processed by this same routine.
 */

/*ARGSUSED*/
static void
dnd_catch_proc(sel_req, target, type, replyBuf, len, format)
Selection_requestor sel_req ;
Atom target ;                    /* Current atom we are checking. */
Atom type ;                      /* Type of atom we are checking. */
Xv_opaque replyBuf ;             /* Ptr to converted data. */
unsigned long len ;              /* Length of replyBuf. */
int format ;                     /* Data type of returned data. */
{
  File_Pane_Object **f_p ;
  Pitch pitch    = (Pitch) xv_get(sel_req, XV_KEY_DATA, Fm->Pitch_key) ;
  Catch catch    = (Catch) xv_get(sel_req, XV_KEY_DATA, Fm->Catch_key) ;
  int  *err_ptr  = (int *) replyBuf ;
  char *char_buf = (char *) replyBuf ;
  int old_length, reply ;
  Event event ;

  if (!pitch && !catch)
    {

/*  This is a critical error -- do not call really_stop_dnd() since this
 *  will just get into an infinite loop always calling this proc.
 *  Instead, just return and let the sender time out.
 */
      if (Fm->debug)
        FPRINTF(stderr, "Unable to get context of %s",
                        !pitch ? "drop" : "drag") ;
      return ;
  }

/* Try to turn type and target atoms into some useful text for debugging. */

#ifdef DND_DEBUG
  if (1)
    {
      static Selection_requestor old_sel ;
      static Catch old_catch ;

      FPRINTF(stderr,"\ndnd_catch_proc()\t sel=%d, catch=%d",
              sel_req, catch) ;
      if (old_sel != 0 && old_sel != sel_req)
        fprintf(stderr, " (CHANGED)\n") ;
      else
        FPRINTF(stderr, "\n") ;
      FPRINTF(stderr, "- len = %d, format = %d, buf = %d state = %d\n",
              len, format, err_ptr, catch->state_mask) ;
      old_sel = sel_req ;
      old_catch = catch ;
    }
#endif /* DND_DEBUG */

  if (Fm->debug)
    {
      char *name = NULL ;

      name = (type > 0 ? XGetAtomName(X->display, type) : "[none]") ;
      dout("------- Processing\n\ttype   atom (%3d) %s\n", type, name) ;

      name = (target > 0 ? XGetAtomName(X->display, target) : "[none]") ;
      dout("\ttarget atom (%3d) %s", target, name) ;
    }

/*  Simply processing the return from the termination request. No action
 *  necessary.
 */

  if (target == X->A->enum_item)
    {

/*  KLUDGE -- interupting a request causes static data to be overwritten.
 *  Hence posting a stop dnd or get next item request will screw up the
 *  current request you are working on.  Use this small state machine
 *  to wait until its safe to ask for info.
 */

      if (catch->stop_state == TRANSFER_INTERRUPTED ||
          catch->stop_state == TRANSFER_FINISHED)
        {
          Boolean delete = FALSE ;

/*  Only tell sender to delete if transfer completed and this is a move
 *  operation.
 */

          if (catch->stop_state == TRANSFER_FINISHED && !catch->copying)
            delete = TRUE ;

          catch->stop_state = DO_NOTHING ;

          if (Fm->debug)
            dout("\n\tStopping %s dnd, transfer %s%s\n",
                 catch->copying ? "copy" : "move",
                 catch->stop_state == TRANSFER_FINISHED ? "finished"
                                                              : "interrupted",
                 delete ? ", sending DELETE back" : "") ;

          really_stop_dnd(sel_req, delete) ;
        }
      else if (catch->stop_state == READY_TO_GET_NEXT)
        {
          if (Fm->debug) dout("\tAsking for next file\n") ;

          catch->stop_state = DO_NOTHING ;
          begin_item_transfer(sel_req) ;
        }
      else if (catch->stop_state == READY_TO_GET_BITS)
        {
          if (Fm->debug) dout("\tAsking for bits\n") ;

          catch->stop_state = DO_NOTHING ;
          get_item_bits(sel_req) ;
        }
      if (Fm->debug) dout("\n") ;

      return ;
    }

  if (target == X->A->done)
    {

/* No longer receiving dnd data -- safe to destroy selection requestor. */

/*  XXX: XView kludge : xview is freeing the selection twice.  Put an
 *       itimer in here to stop the free until later.
 */

      struct itimerval itv ;

      if (Fm->debug)
        {
          dout("\n\tNo longer receiving dnd data,") ;
          dout(" destroying selection requester\n") ;
        }

/* Start an itimer to do the real cleanup */

      itv.it_interval.tv_sec = 0 ;
      itv.it_interval.tv_usec = 0 ;
      itv.it_value.tv_sec = 1 ;
      itv.it_value.tv_usec = 0 ;

      NOTIFY_SET_ITIMER_FUNC((Notify_client) sel_req,
                              (Notify_func) clear_context_itimer_func,
                              ITIMER_REAL, &itv, 0) ;
      return ;
    }

  if (len == SEL_ERROR)
    {

      if (Fm->debug) dout("\treplyBuf length error") ;

      if ((*err_ptr) == SEL_BAD_CONVERSION)
        {

/* A conversion of some type failed. Mark the state variable. */

          if (Fm->debug)
            dout(", bad conversion detected, changing state variable\n") ;

               if (target == X->A->targets)
            catch->state_mask |= TARGET_RESPONSE ;
          else if (target == X->A->host)
            catch->state_mask |= HOST_RESPONSE ;
          else if (target == X->A->atm)
            catch->state_mask |= TRANSPORT_RESPONSE ;
          else if (target == X->A->file)
            catch->state_mask |= FILE_NAME_RESPONSE ;
          else if (target == X->A->label)
            catch->state_mask |= DATA_LABEL_RESPONSE ;
          else if (target == X->A->location)
            catch->state_mask |= OBJECT_LOC_RESPONSE ;
          else if (target == X->A->cursor_loc)
            catch->state_mask |= CURSOR_LOC_RESPONSE ;
        }
      else
        {

/*  Some internal error happened as a result of an earlier posted request.
 *  Tell the user and stop receiving.
 */

          if (Fm->debug)
            dout(", internal error detected, stopping catch\n") ;

          switch (*err_ptr)
            {
              case SEL_BAD_PROPERTY :
                     fm_msg(TRUE, Str[(int) M_BAD_PROPERTY]) ;
                     break ;
              case SEL_BAD_TIME     :
                     fm_msg(TRUE,  Str[(int) M_BAD_TIME]) ;
                     break ;
              case SEL_BAD_WIN_ID   :
                     fm_msg(TRUE,  Str[(int) M_BAD_WIN_ID]) ;
                     break ;
              case SEL_TIMEDOUT     :
                     fm_msg(TRUE,  Str[(int) M_TIMEDOUT]);
                     break ;
              case SEL_PROPERTY_DELETED :
                     fm_msg(TRUE,  Str[(int) M_PROPERTY_DELETED]) ;
                     break ;
              case SEL_BAD_PROPERTY_EVENT :
                     fm_msg(TRUE, Str[(int) M_BAD_PROPERTY_EVENT]) ;
            }
          catch->stop_state = TRANSFER_INTERRUPTED ;
          return ;
        }
    }
  else if (type == X->A->incr)
    {
      if (Fm->debug) dout("\tcatch->processing_stream = TRUE\n") ;

      catch->processing_stream = TRUE ;
    }
  else if ((target == XA_STRING) || (target == X->A->text))
    {
      if (Fm->debug) dout("\tData stream coming thru\n") ;

      if ((!catch->stop_hit) && (len || (!len && !catch->processing_stream)))
        {

/* The length is non-zero, so data, and not the end of transmission. */

          if (!catch->tmpfile[0])
            {
              if (catch->over_file)
                {

/* Setup drop method for pipe. */

                  STRCPY(catch->tmpfile, catch->drop_method) ;
                  expand_filename(catch->tmpfile, catch->over_file) ;

                  if (Fm->debug)
                    dout("\tOver file %s\n", catch->tmpfile) ;

/* Open pipe. */

                  catch->fp = popen(catch->tmpfile, "w") ;
                  if (!catch->fp)
                    {
                      if (Fm->debug)
                        dout("\tUnable to open file, stopping dnd\n") ;

                      fm_msg(TRUE, Str[(int) M_NOOPEN_PIPE], catch->tmpfile) ;
                      catch->stop_state = TRANSFER_INTERRUPTED ;
                      return ;
                    }
                }
              else
                {
                  int mode ;
                  int fdes = 0 ;

/* Make sure the directory for the catch is writable... */

                  if (Fm->debug)
                    dout("\tOver folder %s\n", catch->destdir) ;

                  if (access(catch->destdir, W_OK) != 0)
                    {
                      if (Fm->debug)
                        dout("\tUnable to write to folder, stopping dnd\n") ;

                      fm_msg(TRUE, Str[(int) M_NOWRITE_DND], catch->destdir) ;
                      catch->stop_state = TRANSFER_INTERRUPTED ;
                      return ;
                    }

/* Store data into a unique file in the directory */

                  mode = catch->executable ? 0777 : 0666 ;
                  catch->executable = 0 ;
                  make_dnd_filename(catch, char_buf, len, mode) ;

                  if (!catch->tmpfile[0])
                    {
                      if (Fm->debug)
                        dout("\tUnable to create %s, stopping dnd\n",
                             catch->tmpfile) ;

                      fm_msg(TRUE, Str[(int) M_NOCREATE_DND]) ;
                      catch->stop_state = TRANSFER_INTERRUPTED ;
                      return ;
                    }

                  catch->fp = fopen(catch->tmpfile, "w") ;
                  if (!catch->fp)
                    {
                      CLOSE(fdes) ;
                      UNLINK(catch->tmpfile) ;
                      if (Fm->debug)
                        dout("\tUnable to open %s, stopping dnd\n",
                             catch->tmpfile) ;
                      fm_msg(TRUE, Str[(int) M_NOOPEN_FILE], catch->tmpfile) ;
                      catch->stop_state = TRANSFER_INTERRUPTED ;
                      return ;
                    }
                  else if (!catch->is_paste && catch->dest_wno != WNO_TREE &&
                            catch->object_loc_good &&
                            catch->data_label && catch->cursor_loc_good)
                    f_p = make_file_obj(catch->dest_wno, catch->data_label,
                            catch->x + catch->object_x - catch->cursor_x,
                            catch->y + catch->object_y - catch->cursor_y) ;
                }
            }

/* Send bits */

          reply = fwrite(char_buf, 1, len, catch->fp) ;
          if (reply != len)
            {
              fm_msg(TRUE, Str[(int) M_NOCREATE_OBJ]) ;
              really_stop_dnd(sel_req, FALSE) ;
              if (!catch->processing_stream && !catch->over_file)
                {
                  close_tmp_file(catch) ;
                  UNLINK(catch->tmpfile) ;
                }
              return ;
            }

          if (!catch->processing_stream)
            {
              close_tmp_file(catch) ;
              catch->state_mask |= BYTES_XFERRED ;
            }
        }
      else if (catch->processing_stream)
        {

/* The length was 0, so end of a data transmission. */

          if (Fm->debug)
            dout("\tData len = 0, end of data transmission\n") ;
          close_tmp_file(catch) ;
          catch->state_mask |= BYTES_XFERRED ;
        }
      else
        {
          if (Fm->debug)
            dout("\ttarget=XA_STRING or TEXT, stopping dnd\n") ;

          catch->stop_state = TRANSFER_INTERRUPTED ;
          return ;
        }
    }
  else if (target == X->A->targets)
    {
      if (len)
        {
          if (catch->target_list && !catch->processing_stream)
            ZFREE(catch->target_list) ;

          if (!catch->target_list)
            {
              catch->target_list = (Atom *) LINT_CAST(malloc(len*4)) ;
              MEMCPY((char *) catch->target_list, (char *) replyBuf, len * 4) ;
              if (!catch->processing_stream)
                catch->state_mask |= TARGET_RESPONSE ;
            }
          else
            {
              catch->target_list = (Atom *)
                LINT_CAST(realloc(catch->target_list,
                                  catch->num_targets*4 + len * 4)) ;
              MEMCPY((char *) &catch->target_list[catch->num_targets - 1],
                     (char *) replyBuf, len*4) ;
            }

          if (Fm->debug)
            {
              dout("\n") ;
              print_atom_list(catch->target_list, len) ;
            }
        }
      else
        {
          if (Fm->debug)
            dout("\tcatch->state_mask = TARGET_RESPONSE\n") ;

          catch->state_mask |= TARGET_RESPONSE ;
          catch->processing_stream = FALSE ;
        }
      catch->num_targets += len ;
    }
  else if (target == X->A->atm)
    {
      if (len)
        {
          if (catch->transport_list && !catch->processing_stream)
            ZFREE(catch->transport_list) ;

          if (!catch->transport_list)
            {
              catch->transport_list = (Atom *) LINT_CAST(malloc(len*4)) ;
              MEMCPY((char *) catch->transport_list,
                     (char *) replyBuf, len * 4) ;
              if (!catch->processing_stream)
                catch->state_mask |= TRANSPORT_RESPONSE ;
            }
          else
            {
              catch->transport_list = (Atom *)
                LINT_CAST(realloc(catch->transport_list,
                                  catch->num_transports*4 + len*4)) ;
              MEMCPY((char *) &catch->transport_list[catch->num_transports - 1],
                     (char *) replyBuf, len * 4) ;
            }
          if (Fm->debug)
            {
              dout("\n") ;
              print_atom_list(catch->transport_list, len) ;
            }
        }
      else
        {
          if (Fm->debug)
            dout("\tcatch->state_mask = TRANSPORT_RESPONSE\n") ;

          catch->state_mask |= TRANSPORT_RESPONSE ;
          catch->processing_stream = FALSE ;
        }
      catch->num_transports += len ;
    }
  else if (target == X->A->host)
    {
      if (len)
        {
          if (catch->src_hostname && !catch->processing_stream)
            ZFREE(catch->src_hostname) ;

          if (!catch->src_hostname)
            {
              catch->src_hostname = malloc(len + 1) ;
              MEMCPY((char *)catch->src_hostname, (char *)replyBuf, len) ;
              catch->src_hostname[len] = '\0' ;
              if (!catch->processing_stream)
                catch->state_mask |= HOST_RESPONSE ;
            }
          else
            {
              old_length = strlen(catch->src_hostname) ;
              catch->src_hostname = (char *)
                        realloc(catch->src_hostname, old_length + len + 1) ;
              MEMCPY((char *) &catch->src_hostname[old_length],
                     (char *) replyBuf, len) ;
              catch->src_hostname[old_length + len] = '\0' ;
            }
          if (Fm->debug) dout(" : %s\n", catch->src_hostname) ;
        }
      else
        {
          if (Fm->debug) dout("\tcatch->state_mask = HOST_RESPONSE\n") ;

          catch->state_mask |= HOST_RESPONSE ;
          catch->processing_stream = FALSE ;
        }
    }
  else if (target == X->A->file)
    {
      if (len)
        {
          if (catch->src_filename && !catch->processing_stream)
            ZFREE(catch->src_filename) ;

          if (!catch->src_filename)
            {
              catch->src_filename = malloc(len + 1) ;
              if (!catch->src_filename)
                fm_msg(TRUE, Str[(int) M_NOCREATE_CATCH]) ;
              MEMCPY((char *)catch->src_filename, (char *)replyBuf, len) ;
              catch->src_filename[len] = '\0' ;
              if (!catch->processing_stream)
                catch->state_mask |= FILE_NAME_RESPONSE ;
              DB1("- file caught %s\n", catch->src_filename) ;
            }
          else
            {
              old_length = strlen(catch->src_filename) ;
              catch->src_filename = (char *)
                realloc(catch->src_filename, old_length + len + 1) ;
              if (!catch->src_filename)
                fm_msg(TRUE, Str[(int) M_NO_REALLOCATE]) ;
              MEMCPY((char *) &catch->src_filename[old_length],
                     (char *) replyBuf, len) ;
              catch->src_filename[old_length + len] = '\0' ;
            }
          if (Fm->debug) dout("\n\t%s\n", catch->src_filename) ;
        }
      else
        {
          if (Fm->debug) dout("\tcatch->state_mask = FILE_NAME_RESPONSE\n") ;

          catch->state_mask |= FILE_NAME_RESPONSE ;
          catch->processing_stream = FALSE ;
        }
    }
  else if (target == X->A->label)
    {
      if (len)
        {
          if (catch->data_label && !catch->processing_stream)
            ZFREE(catch->data_label) ;

          if (!catch->data_label)
            {
              catch->data_label = malloc(len + 1) ;
              MEMCPY((char *) catch->data_label, (char *) replyBuf, len) ;
              catch->data_label[len] = '\0' ;
              if (!catch->processing_stream)
                catch->state_mask |= DATA_LABEL_RESPONSE ;
            }
          else
            {
              old_length = strlen(catch->data_label) ;
              catch->data_label = (char *)
                realloc(catch->data_label, old_length + len + 1) ;
              MEMCPY((char *) &catch->data_label[old_length],
                     (char *) replyBuf, len) ;
              catch->data_label[old_length + len] = '\0' ;
            }
          if (Fm->debug) dout(" : %s\n", catch->data_label) ;
        }
      else
        {
          if (Fm->debug)
            dout("\tcatch->state_mask = DATA_LABEL_RESPONSE\n") ;

          catch->state_mask |= DATA_LABEL_RESPONSE ;
          catch->processing_stream = FALSE ;
        }
    }
  else if (target == X->A->execute && len >= 1)
    {
      if (format == 32) catch->executable = *(int *) replyBuf ;
      if (Fm->debug) dout(" : %d\n", catch->executable) ;
    }
  else if (target == X->A->location )
    {
      int *fake = (int *) replyBuf ;

      if (format == 32)
      {
        catch->object_x = fake[0] ;
        catch->object_y = fake[1] ;
        catch->object_loc_good = TRUE;
      }
      catch->state_mask |= OBJECT_LOC_RESPONSE ;
      if (Fm->debug) dout(" : %d %d\n", catch->object_x, catch->object_y) ;
    }
  else if (target == X->A->cursor_loc )
    {
      int *fake = (int *) replyBuf ;

      if (format == 32)
      {
        catch->cursor_x = fake[0] ;
        catch->cursor_y = fake[1] ;
        catch->cursor_loc_good = TRUE;
      }
      catch->state_mask |= CURSOR_LOC_RESPONSE ;
      if (Fm->debug) dout(" : %d %d\n", catch->cursor_x, catch->cursor_y ) ;
    }
  else
    {
      if (Fm->debug) dout("\n\tUnknown target, ignoring\n") ;
      return ;
    }

  if (catch->state_mask == RESPONSE_1)
    {
      if (Fm->debug)
        dout("\tFirst batch of replies processed, asking for second\n") ;

      if (!atom_in_list(XA_STRING, catch->target_list, catch->num_targets) &&
          !atom_in_list(X->A->atm_file, catch->transport_list,
                                     catch->num_targets) &&
          !atom_in_list(X->A->text, catch->target_list, catch->num_targets))
        {
          if (Fm->debug)
            dout("\tAtom not in target or transport list, stopping dnd\n") ;

          notice_prompt(X->fileX[Fm->curr_wno]->frame, &event,
                        NOTICE_MESSAGE_STRINGS,
                          Str[(int) M_NO_OPERATE1],
                          0,
                        NOTICE_BUTTON_NO, Str[(int) M_CONTINUE],
                        0) ;

          catch->stop_state = TRANSFER_INTERRUPTED ;
          return ;
        }

/*  If the source selection is advertising link copy as a target, it must
 *  be holding the clipboard, and the operation is illegal if the selection
 *  holder is not on the same host as the paste target.
 */

      if (atom_in_list(X->A->link_copy, catch->target_list,
                                     catch->num_targets) &&
          (!atom_in_list(X->A->atm_file, catch->transport_list,
                                      catch->num_targets) ||
          !EQUAL((char *) Fm->hostname, catch->src_hostname)))
        {
          notice_prompt(X->fileX[Fm->curr_wno]->frame, &event,
                        NOTICE_MESSAGE_STRINGS,
                          "Links may not be pasted between",
                          "File Managers operating on different hosts.",
                          "The drop operation has been cancelled.",
                          0,
                        NOTICE_BUTTON_NO, "Continue",
                        0) ;

          catch->stop_state = TRANSFER_INTERRUPTED ;
          return ;
        }

      if (!atom_in_list(XA_STRING, catch->target_list, catch->num_targets))
        catch->chosen_target = X->A->text ;
      else
        catch->chosen_target = XA_STRING ;

/*  Determine what sort of data we have coming. Get the host name to go with
 *  the file name.
 */

/* This determines if we can transfer by filename. */

      if (!Fm->DND_BY_BITS && atom_in_list(X->A->atm_file,
                            catch->transport_list, catch->num_transports) &&
                                                   catch->src_filename)
        {
          if (EQUAL((char *) Fm->hostname, catch->src_hostname))
            {
              Boolean copy = TRUE ;
              Boolean err = FALSE ;

/* Life is hunky dory. Use the file name in place. */

              if (catch->src_filename[0] == '\0')
                {
                  if (Fm->debug)
                    dout("\tBad filename received, asking for next\n") ;

                  fm_msg(TRUE, Str[(int) M_BAD_FILENAME]) ;
                  clear_context(catch, FALSE) ;
                  catch->stop_state = READY_TO_GET_NEXT ;
                  return ;
                }

              if (Fm->debug)
                dout("\tFile\t%s %d/%d %s to\n\t\t%s\n",
                     catch->src_filename,
                     catch->items_caught, catch->num_items,
                     catch->copying ? "copied" : "moved",
                     catch->destdir) ;

              if (catch->copying)
                {
                  if (catch->over_file)
                    err = !exec_drop_method(catch->src_filename,
                                            catch->destdir, catch->over_file,
                                            catch->drop_method, copy) ;
                  else
		  {
                    err = !copy_by_filename(catch->src_filename,
                                            catch->destdir, catch,
                                            atom_in_list(X->A->link_copy,
                                                         catch->target_list,
                                                         catch->num_targets)) ;
		    if (!err) dnd_adjust(catch) ;
		  }
                }
              else
                {
                  if (catch->over_file)
                    err = !exec_drop_method(catch->src_filename,
                                            catch->destdir, catch->over_file,
                                            catch->drop_method, !copy) ;
                  else
		  {
                    err = !move_by_filename(catch->src_filename,
                                            catch->destdir, catch) ;
		    if (!err) dnd_adjust(catch) ;
		  }
                }

              catch->items_caught++ ;

              if (err)
                {
                  if (Fm->debug)
                    dout("\tError occurred while copying/moving the file\n") ;

                  catch->stop_state = TRANSFER_INTERRUPTED ;
                  return ;
                }
              else if (catch->items_caught < catch->num_items)
                {
                  if (Fm->debug)
                    dout("\tFile caught, getting next file\n") ;
                  clear_context(catch, FALSE) ;
                  catch->stop_state = READY_TO_GET_NEXT ;
                  return ;
                }
              else
                {
                  if (Fm->debug)
                    dout("\tLast item caught, stopping dnd\n") ;
                  catch->stop_state = TRANSFER_FINISHED ;

                  display_dlist(&(catch->display_list)) ;
                  destroy_dlist(&(catch->display_list)) ;
                  return ;
                }
            }
        }

/* We have to deal with a data stream. */

      if (Fm->debug) dout("\tAsking for raw bits\n") ;

      catch->stop_state = READY_TO_GET_BITS ;
      catch->state_mask |= BYTES_REQUESTED ;
      return ;
    }
  else if (catch->state_mask == RESPONSE_2)
    {
      catch->items_caught++ ;
      if (catch->items_caught < catch->num_items)
        {
          if (Fm->debug) dout("\t??? caught, getting next ???\n") ;

          clear_context(catch, FALSE) ;
          catch->stop_state = READY_TO_GET_NEXT ;
          return ;
        }
      else
        {
          if (Fm->debug) dout("\tLast ??? caught, stopping dnd\n") ;

          catch->stop_state = TRANSFER_FINISHED ;

          display_dlist(&(catch->display_list)) ;
          destroy_dlist(&(catch->display_list)) ;
          return ;
        }
    }
}


void
dnd_adjust(catch)
Catch catch ;
{
  if (catch->dest_wno != WNO_TREE && catch->num_items == catch->items_caught + 1)
    {
      recompute_canvas_size(catch->dest_wno);
      adjust_pos_info(catch->dest_wno) ;
      redisplay_folder(catch->dest_wno, 0, 0,
                       get_pos_attr(catch->dest_wno, FM_POS_CWIDTH),
                       get_pos_attr(catch->dest_wno, FM_POS_CHEIGHT)) ;
      set_status_info(catch->dest_wno) ;
    }
}


/*  Stop the drag and drop operation.  Converts _OL_DRAGDROP_DONE on the
 *  current selection, signaling the end, and then puts the rest of the
 *  application back to normal.
 */

void
really_stop_dnd(sel, delete)
Selection_requestor sel ;
Boolean delete ;                /* Send delete atom to sender? */
{
  DB2("really_stop_dnd(sel=%d, delete=%d)\t called\n", sel, delete) ;

/*  Signal termination of transaction. If delete set, then send a signal
 *  to sender to delete transmitted material.
 */

  if (delete)
    XV_SET(sel,
           SEL_REPLY_PROC, dnd_catch_proc,
           SEL_TYPES,
             X->A->delete,
             X->A->done,
             0,
           0) ;
  else
    XV_SET(sel,
           SEL_REPLY_PROC, dnd_catch_proc,
           SEL_TYPES,
             X->A->done,
             0,
           0) ;
  sel_post_req(sel) ;
}


static void
resize_drop_site(canvas, paint_window)
Xv_Window canvas ;
Xv_Window paint_window ;
{
  Rect *winp ;
  Xv_drop_site ds ;

  winp = (Rect *) xv_get(canvas, CANVAS_VIEWABLE_RECT, paint_window) ;

  if (!winp) ERR_EXIT("Unable to get size of pixwin") ;

/*  For performance, check if there really is a change in width & height
 *  before resetting drop site.
 */

  DB("resize_drop_site()\t") ;
  DB4(" (x,y,w,h)=(%d,%d,%d,%d)\n",
      winp->r_left, winp->r_top, winp->r_width, winp->r_height) ;

  ds = xv_get(paint_window, XV_KEY_DATA, Fm->Drop_site_key) ;

  if (!ds)
    {
      DB1("bogus drop site for pw %d\n", paint_window) ;
      return ;
    }
  XV_SET(ds,
         DROP_SITE_DELETE_REGION, NULL,   /* Nullify the list first. */
         DROP_SITE_REGION, winp,          /* Now add drop site (one only). */
         0) ;
}


/* Resizes all drop sites for a given window and its subviews. */

void
resize_window_drop_sites(wno, ispath)
int wno, ispath ;
{
  Xv_Window pw ;

  if (wno == WNO_TREE)
    {
      FM_CANVAS_EACH_PAINT_WINDOW(X->treeX.canvas, pw)
        resize_drop_site(X->treeX.canvas, pw) ;
      FM_CANVAS_END_EACH
    }
  else if (ispath == FALSE)
    {
      FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->canvas, pw)
        resize_drop_site(X->fileX[wno]->canvas, pw) ;
      FM_CANVAS_END_EACH
    }
  else
    {
      FM_CANVAS_EACH_PAINT_WINDOW(X->fileX[wno]->pathX.canvas, pw)
        resize_drop_site(X->fileX[wno]->pathX.canvas, pw) ;
      FM_CANVAS_END_EACH
    }
}


/* Setup and start the requestor (sel) to receive drop. */

static void
begin_item_transfer(sel)
Selection_requestor sel ;
{
  int end_enum_context = -1 ;
  Catch catch = (Catch) xv_get(sel, XV_KEY_DATA, Fm->Catch_key) ;

  if (!catch)
    {
      DB("begin_item_transfer()\t Unable to get dnd catch\n") ;
      really_stop_dnd(sel, FALSE) ;
      return ;
    }

  DB2("begin_item_transfer()\t sel=%d, catch=%d\n", sel, catch) ;
  XV_SET(sel,

/* Procedure to call when selection value received. */

         SEL_REPLY_PROC, dnd_catch_proc,

/* List of atom types we are requesting. */

         SEL_TYPES,
           X->A->enum_item,    /* Begin atom. */
           X->A->targets,
           X->A->host,
           X->A->atm,
           X->A->file,
           X->A->execute,
           X->A->label,
           X->A->location,
           X->A->cursor_loc,
           X->A->enum_item,    /* End atom. */
           0,

/* For atom 0 (X->A->enum_item), associate these properties with: */

         SEL_TYPE_INDEX,    0,
           SEL_PROP_TYPE,   XA_INTEGER,
           SEL_PROP_DATA,   &catch->items_caught,
           SEL_PROP_FORMAT, 32,
           SEL_PROP_LENGTH, 1,

/* for atom 6 (X->A->enum_item), associate these properties with: */

        SEL_TYPE_INDEX,    9,
          SEL_PROP_TYPE,   XA_INTEGER,
          SEL_PROP_DATA,   &end_enum_context,
          SEL_PROP_FORMAT, 32,
          SEL_PROP_LENGTH, 1,
        0) ;

/* Initiate the non-blocking request. */

  sel_post_req(sel) ;
}


/*ARGSUSED*/
static void
count_reply_proc (sel_req, target, type, replyBuf, len, format)
Selection_requestor sel_req ;
Atom target, type ;
Xv_opaque replyBuf ;
unsigned long len ;
int format ;
{
  Catch catch = (Catch) xv_get(sel_req, XV_KEY_DATA, Fm->Catch_key) ;
  int *count_ptr = (int *) replyBuf ;

  if (!catch)
    {
      DB("count_reply_proc()\t Unable to get dnd catch\n") ;
      really_stop_dnd(sel_req, FALSE) ;
      return ;
    }

  DB2("count_reply_proc()\t sel=%d, catch=%d\n", sel_req, catch) ;
  if ((len == SEL_ERROR) && ((*count_ptr) == SEL_BAD_CONVERSION))
    {

/* A conversion of some type failed. Mark the state variable. */

      DB("count_reply_proc()\t count conversion failed\n") ;
      if (target == X->A->enum_count) catch->num_items = 1 ;
    }
  else if (len == SEL_ERROR)
    {

/*  Some internal error happened as a result of an earlier posted request.
 *  Tell the user and stop receiving.
 */

      DB1("replyBuf length error, internal error detected, stopping catch=%d\n",
          catch) ;

      switch (*count_ptr)
        {
          case SEL_BAD_PROPERTY       :
                 fm_msg(TRUE, Str[(int) M_BAD_PROPERTY]) ;
                 break ;
          case SEL_BAD_TIME           :
                 fm_msg(TRUE,Str[(int) M_BAD_TIME]) ;
                 break ;
          case SEL_BAD_WIN_ID         :
                 fm_msg(TRUE, Str[(int) M_BAD_WIN_ID]) ;
                 break ;
          case SEL_TIMEDOUT           :
                 fm_msg(TRUE,Str[(int) M_TIMEDOUT]) ;
                 break ;
          case SEL_PROPERTY_DELETED   :
                 fm_msg(TRUE, Str[(int) M_PROPERTY_DELETED]) ;
                 break ;
          case SEL_BAD_PROPERTY_EVENT :
                 fm_msg(TRUE,Str[(int) M_BAD_PROPERTY_EVENT]) ;
        }
      really_stop_dnd(sel_req, FALSE) ;
      return ;
    }
  else if (target == X->A->enum_count) catch->num_items = *count_ptr ;

  DB2("count_reply_proc()\t catch(%d) had %d items\n",
      catch, catch->num_items) ;

  begin_item_transfer(sel_req) ;
}


void
load_from_dnd(sel)
Selection_requestor sel ;
{
  DB1("load_from_dnd()\t\t sel=%d\n", sel) ;

/* Clear the left footer for new response status. */

  fm_clrmsg() ;

/* Setup a mini-reply proc to get number of items in selection */

  XV_SET(sel,
         SEL_TYPES,      X->A->enum_count, 0,
         SEL_REPLY_PROC, count_reply_proc,
         0) ;

  sel_post_req(sel) ;        /* Send off the non-blocking request */
}


static Xv_Cursor
init_folder_cursor(image)
const unsigned short image[] ;
{
  Server_image sv ;

  sv = xv_create(XV_NULL,            SERVER_IMAGE,
                 SERVER_IMAGE_BITS,  image,
                 SERVER_IMAGE_DEPTH, 1,
                 XV_WIDTH,           64,
                 XV_HEIGHT,          64,
                 0) ;
  return((Xv_Cursor) xv_create(XV_NULL,      CURSOR,
                               CURSOR_IMAGE, sv,
                               CURSOR_XHOT,  20,
                               CURSOR_YHOT,  16,
                               CURSOR_OP,    PIX_SRC ^ PIX_DST,
                               0)) ;
}


static void
init_folder_cursors()
{
  X->folder_drag_move_cursor   = init_folder_cursor(folder_drag_move_image) ;
  X->folders_drag_move_cursor  = init_folder_cursor(folders_drag_move_image) ;
  X->folder_drag_copy_cursor   = init_folder_cursor(folder_drag_copy_image) ;
  X->folders_drag_copy_cursor  = init_folder_cursor(folders_drag_copy_image) ;
  X->folder_accept_move_cursor = init_folder_cursor(folder_accept_move_image) ;
  X->folders_accept_move_cursor =
                               init_folder_cursor(folders_accept_move_image) ;
  X->folder_accept_copy_cursor = init_folder_cursor(folder_accept_copy_image) ;
  X->folders_accept_copy_cursor =
                               init_folder_cursor(folders_accept_copy_image) ;
}


Xv_Cursor
get_folder_cursor(type)
int type ;
{
  switch (type)
    {
      case OLC_FOLDER_MOVE_DRAG  : return(X->folder_drag_move_cursor) ;
      case OLC_FOLDER_MOVE_DROP  : return(X->folder_accept_move_cursor) ;
      case OLC_FOLDERS_MOVE_DRAG : return(X->folders_drag_move_cursor) ;
      case OLC_FOLDERS_MOVE_DROP : return(X->folders_accept_move_cursor) ;
      case OLC_FOLDER_COPY_DRAG  : return(X->folder_drag_copy_cursor) ;
      case OLC_FOLDER_COPY_DROP  : return(X->folder_accept_copy_cursor) ;
      case OLC_FOLDERS_COPY_DRAG : return(X->folders_drag_copy_cursor) ;
      case OLC_FOLDERS_COPY_DROP : return(X->folders_accept_copy_cursor) ;
    }
/*NOTREACHED*/
}


void
init_atoms()
{
  Xv_Server s = X->server ;

  DB("init_atoms()\t called\n") ;

  X->A = (Atoms) LINT_CAST(malloc(sizeof(Atoms_Object))) ;
  if (!X->A)
    {
      FPRINTF(stderr, "%s", Str[(int) M_ALLOC_ATOMS]);
      return ;
    }
  memset((char *) X->A, 0, sizeof(Atoms_Object)) ;

  X->A->text       = xv_get(s, SERVER_ATOM, "TEXT") ;
  X->A->incr       = xv_get(s, SERVER_ATOM, "INCR") ;
  X->A->targets    = xv_get(s, SERVER_ATOM, "TARGETS") ;
  X->A->length     = xv_get(s, SERVER_ATOM, "LENGTH") ;
  X->A->name       = xv_get(s, SERVER_ATOM, "NAME") ;
  X->A->host       = xv_get(s, SERVER_ATOM, "_SUN_FILE_HOST_NAME") ;
  X->A->file       = xv_get(s, SERVER_ATOM, "FILE_NAME") ;
  X->A->atm_file   = xv_get(s, SERVER_ATOM, "_SUN_ATM_FILE_NAME") ;
  X->A->location   = xv_get(s, SERVER_ATOM, "_SUN_OBJECT_LOC") ;
  X->A->cursor_loc = xv_get(s, SERVER_ATOM, "_SUN_CURSOR_LOC") ;
  X->A->delete     = xv_get(s, SERVER_ATOM, "DELETE") ;
  X->A->done       = xv_get(s, SERVER_ATOM, "_SUN_SELECTION_END") ;
  X->A->label      = xv_get(s, SERVER_ATOM, "_SUN_DATA_LABEL") ;
  X->A->atm        = xv_get(s, SERVER_ATOM, "_SUN_ALTERNATE_TRANSPORT_METHODS") ;
  X->A->available  = xv_get(s, SERVER_ATOM, "_SUN_AVAILABLE_TYPES") ;
  X->A->enum_count = xv_get(s, SERVER_ATOM, "_SUN_ENUMERATION_COUNT") ;
  X->A->enum_item  = xv_get(s, SERVER_ATOM, "_SUN_ENUMERATION_ITEM") ;
  X->A->execute    = xv_get(s, SERVER_ATOM, "_SUN_EXECUTABLE") ;
  X->A->link_copy  = xv_get(s, SERVER_ATOM, "_SUN_LINK_COPY") ;
  X->A->null       = xv_get(s, SERVER_ATOM, "NULL") ;

  init_folder_cursors() ;
}


void
create_drop_site(wno, pw, ispath)
int wno, ispath ;
Xv_Window pw ;
{
  if (Fm->Drop_site_key == -1)
    Fm->Drop_site_key = xv_unique_key() ;        /* Dnd receive context key. */

  XV_SET(pw,
         XV_KEY_DATA, Fm->Drop_site_key,
           xv_create(pw, DROP_SITE_ITEM,
                     DROP_SITE_ID,         wno,
                     DROP_SITE_EVENT_MASK, DND_MOTION | DND_ENTERLEAVE,
                     0),
         0) ;
  resize_window_drop_sites(wno, ispath) ;
}


/* Setup the dnd receive context (the catch).  */

static Catch
create_catch(ec)
Event_Context ec ;
{
  Catch catch ;
  int item ;
  Boolean over_name = FALSE ;        /* Count if over filename? */
  char *save_filename, buf[MAXPATHLEN] ;

  catch = (Catch) LINT_CAST(fm_malloc(sizeof(Dnd_Receive_Object))) ;
  MEMSET((char *) catch, 0, sizeof(Dnd_Receive_Object)) ;

/*  Save the destination directory where drop was intended.  If over a
 *  folder, be sure to save it as part of path.  If over another type
 *  of item, save the item to check for a drop method later.
 *
 *  XXX: (comment from Martin Knutson).
 *
 *       After catch->destdir has been constructed, filemgr should test
 *       for the writability of the destination directory. The drawback
 *       I see to this is there is currently no way to percolate any sort
 *       of error condition back up, and terminate the operation here, and
 *       I'm not sure how you cater for the case where you drop on a file
 *       that you have permission to modify, but you don't have write
 *       permission on the directory it resides in.  I suppose
 *       create_seln_requestor() could return a NULL selection handle as
 *       an error condition.  One could also imagine some extra logic where
 *       if you were over a file, and there was a drop method associated
 *       with the file, you would check the access permissions on the file
 *       instead of the access permissions on the directory.
 */

  if (ec->wno == WNO_TREE || ec->ispath)
    {

/* We already know we are over a tree item. */

      FM_GETPATH(ec->tree_item, (WCHAR *) buf) ;
      SPRINTF(catch->destdir, "%s", buf) ;
    }
  else
    {
      item = fm_over_item(ec->x, ec->y, over_name, ec->wno) ;
      if (item != FM_EMPTY)
        {
          if (is_dnd_target(Fm->file[ec->wno]->object[item]))
            {
              if (Fm->file[ec->wno]->object[item]->type == FT_DIR)
                {
                  SPRINTF(catch->destdir, "%s/%s", Fm->file[ec->wno]->path,
                          Fm->file[ec->wno]->object[item]->name) ;
                  catch->over_folder = TRUE ;
                }
              else
                {

/*  Save the filename and drop method of item we are over.  Be sure
 *  to expand the possible filename in the drop method, and to free
 *  over_file and drop_method later.
 */
                  catch->over_file =
                    strdup((char *) Fm->file[ec->wno]->object[item]->name) ;
                  catch->drop_method =
                    strdup(get_drop_method(Fm->file[ec->wno]->object[item])) ;
                  if (!catch->drop_method)
                    {
                      fm_msg(TRUE, Str[(int) M_NO_FILTER], catch->over_file) ;
                      FREE(catch->over_file) ;
                    }
		  else {
			if (catch->over_file && catch->over_file[0] != '/') {
				save_filename = (char*)fm_malloc(strlen(catch->over_file)+1);
				strcpy(save_filename, catch->over_file);
				free(catch->over_file);
				catch->over_file = (char*)fm_malloc(strlen(save_filename) + strlen((char*)Fm->file[ec->wno]->path) + 2);
				sprintf(catch->over_file, "%s/%s",
					Fm->file[ec->wno]->path,
					save_filename);
			}
		  }
                  SPRINTF(catch->destdir, "%s", Fm->file[ec->wno]->path) ;
                }
            }
          else SPRINTF(catch->destdir, "%s", Fm->file[ec->wno]->path) ;
        }
      else SPRINTF(catch->destdir, "%s", Fm->file[ec->wno]->path) ;
    }

  catch->items_caught = 0 ;        /* Default to 1st item */
  catch->stop_state = DO_NOTHING ;
  catch->copying = (ec->id == ACTION_DRAG_COPY ? TRUE : FALSE) ;

  catch->dest_wno = ec->wno ;
  catch->x        = ec->x ;
  catch->y        = ec->y ;

  return(catch) ;
}


Selection_requestor
create_seln_requestor(ec)
Event_Context ec ;
{
  Selection_requestor sel = 0 ;
  Catch catch = NULL ;
  Canvas canvas = 0 ;

  if (Fm->Catch_key == -1)
    Fm->Catch_key = xv_unique_key() ;        /* Dnd receive context key. */

  catch = create_catch(ec) ;

/* Create the requestor for the dnd drop. */

       if (ec->wno == WNO_TREE) canvas = X->treeX.canvas ;
  else if (ec->ispath)          canvas = X->fileX[ec->wno]->pathX.canvas ;
  else                          canvas = X->fileX[ec->wno]->canvas ;

  sel = xv_create(canvas, SELECTION_REQUESTOR,
                  XV_KEY_DATA, Fm->Catch_key, catch,
                  0) ;

  DB3("create_seln_requestor(%d)\t sel=%d, catch=%d\n", ec->wno, sel, catch) ;
  return(sel) ;
}


/* Free up all the memory associated with the pitch (dnd send context). */

static void
cleanup_pitch(p)
Pitch p ;
{
  int i ;

  if (!p) return ;

  DB1("cleanup_pitch(%d)\t freeing memory\n", p) ;

  delete_from_plist(&X->Pitch_list, p) ;
  for (i = 0; i< p->num_items; i++)
    {
      ZFREE(p->files[i].name) ;
      ZFREE(p->files[i].data_label) ;
    }
  ZFREE(p->files) ;
  ZFREE(p) ;
}


/* Free up all the memory associated with the dnd object. */

void
destroy_dnd_object(dnd)
Dnd dnd ;
{
  Pitch p = (Pitch) xv_get(dnd, XV_KEY_DATA, Fm->Pitch_key) ;

  if (p) cleanup_pitch(p) ;
  p = NULL ;
  XV_SET(dnd, XV_KEY_DATA, Fm->Pitch_key, p, 0) ;
  XV_DESTROY_SAFE(dnd) ;
  Fm->started_dnd = FALSE ;
}

/*
**  This function attempts to shrink or expand the canvas as necessary - called
**  after a DnD, it will compute the min and max (X,Y) coordinates of the icons
**  in their new locations and adjust the canvas accordingly.
*/
static void
recompute_canvas_size(wno)
int wno;
{
	int			x, y, minx, maxx, miny, maxy,
				canvas_width, canvas_height,
				incx, incy, delta_x, delta_y;
	File_Pane_Object	**f_p, **s_p, **l_p;

	if (wno == WNO_TREE)
		return;

	l_p = PTR_LAST(wno) ;
	s_p = PTR_FIRST(wno) ;
	for (f_p = s_p; f_p < l_p; f_p++)
	{
		x = get_file_obj_x(wno, *f_p);
		y = get_file_obj_y(wno, *f_p);

		if (f_p == s_p)
		{
			maxx = minx = x; maxy = miny = y;
		}
		else
		{
			if (x > maxx) maxx = x;
			if (x < minx) minx = x;
			if (y > maxy) maxy = y;
			if (y < miny) miny = y;
		}
	}

	canvas_width = get_pos_attr(wno, FM_POS_CWIDTH);
	canvas_height = get_pos_attr(wno, FM_POS_CHEIGHT);
  	incx = get_pos_attr(wno, FM_POS_INCX);
  	incy = get_pos_attr(wno, FM_POS_INCY);

/********** commented to avoid the movement of all icons even when
	just one is moved but there is no icon in the first column
	Although the icons are moved to their left so as to occupy
	less space,  the canvas size is unaffected; hence commented

	delta_x = get_pos_attr(wno, FM_POS_STARTX) - minx;
	delta_y = get_pos_attr(wno, FM_POS_STARTY) - miny;

	if (delta_x || delta_y)
	{
		l_p = PTR_LAST(wno) ;
		s_p = PTR_FIRST(wno) ;
		for (f_p = s_p; f_p < l_p; f_p++)
		{
			if (delta_x)
			{
				x = get_file_obj_x(wno, *f_p) + delta_x;
				set_file_obj_x(wno, *f_p, x) ;
			}
			if (delta_y)
			{
				y = get_file_obj_y(wno, *f_p) + delta_y;
				set_file_obj_y(wno, *f_p, y) ;
			}
		}
	}
******************************************************************/

	clear_area(wno, 0, 0, canvas_width, canvas_height) ;
	canvas_width = (maxx - minx) + (2 * incx);
	FM_SET_CANVAS_WIDTH(canvas_width, wno, incx);
	canvas_height = (maxy - miny) + (2*incy);
	FM_SET_CANVAS_HEIGHT((maxy-miny)+(2*incy), wno, incy);
}

void
handle_move_copy(ec)   /* Move/Dup. icon[s] to new position in same folder. */
Event_Context ec ;
{
  int dx, dy, i ;
  int wno, x, y ;
  File_Pane *f = Fm->file[ec->wno] ;
  File_Pane_Object **f_p ;
  Pitch p = (Pitch) xv_get(X->dnd_object, XV_KEY_DATA, Fm->Pitch_key) ;

  if (Fm->autosort || f->display_mode == VIEW_BY_CONTENT ||
                     (f->display_mode == VIEW_BY_LIST && f->listopts))
    {
      write_footer(ec->wno, Str[(int) M_NO_POS_VIEW], TRUE) ;
      stop_move_operation() ;
      return ;
    }

  wno = ec->wno ;
  dx  = ec->x - ec->start_x ;
  dy  = ec->y - ec->start_y ;
  if (p == NULL)
    {
      stop_move_operation() ;
      return ;
    }
  for (i = 0; i < p->num_items; i++)
    {
      f_p = Fm->file[wno]->object + p->files[i].index ;
      x = get_file_obj_x(wno, *f_p) ;
      y = get_file_obj_y(wno, *f_p) ;

      if (ec->fm_id == E_DRAG_COPY)                           /* Duplicate. */
        {
          x += dx ;
          y += dy ;
          if (make_duplicate(wno, f_p, x, y) == TRUE)
            p->files[i].index = Fm->file[wno]->num_objects-1 ;
        }
      else                                                    /* Move. */
        {
	int pos_startx, pos_starty;

          delete_icon(wno, p->files[i].index, x, y) ;

          x += dx ;
          y += dy ;

		pos_startx = get_pos_attr(wno,FM_POS_STARTX);
		pos_starty = get_pos_attr(wno,FM_POS_STARTY);

          set_file_obj_x(wno, *f_p, (x > pos_startx ? x : pos_startx));
          set_file_obj_y(wno, *f_p, (y > pos_starty ? y : pos_starty));

	/*******
          set_file_obj_x(wno, *f_p, x) ;
          set_file_obj_y(wno, *f_p, y) ;
	********/
        }
    }

  recompute_canvas_size(wno);
  adjust_pos_info(wno) ;
  redisplay_folder(wno, 0, 0, get_pos_attr(wno, FM_POS_CWIDTH),
    get_pos_attr(wno, FM_POS_CHEIGHT)) ;
  set_status_info(wno) ;
  set_pos_attr(wno, FM_POS_MOVED, TRUE) ;

  for (i = 0; i < p->num_items; i++)    /* Draw moved icons at the "front". */
    draw_ith(p->files[i].index, wno) ;

  stop_move_operation() ;
}


/*  The convert proc is called whenever someone makes a request to the dnd
 *  selection.  Two cases we handle within the convert proc: DELETE and
 *  _SUN_SELECTION_END.  Everything else we pass on to the default convert
 *  proc which knows about our selection items.
 */

static int
dnd_convert(seln, type, data, length, format)
Selection_owner seln ;
Atom *type ;
Xv_opaque *data ;
long *length ;
int *format ;
{

/* XXX: bug fix in SVR4. */

  static int length_buf ;
  static int location_buf[2] ;
  static Atom target_list[20] ;
  static Atom types_list[2] ;
  Pitch p = (Pitch) xv_get(seln, XV_KEY_DATA, Fm->Pitch_key) ;
  char *name ;                        /* Atom's name. */

  if (!p)
    {
      if (Fm->debug) FPRINTF(stderr, "%s", Str[(int) M_NO_CONTEXT]);
      return(FALSE) ;
    }

/* Try to turn type and target atoms into some useful text for debugging. */

  DB2("\ndnd_convert() sel=%d, pitch=%d\n", seln, p) ;

  if (Fm->debug)
    {
      name = (*type > 0 ? XGetAtomName(X->display, *type) : "[none]") ;
      dout("------- Converting\n\ttype atom (%d) %s", *type, name) ;
    }

/*  Hopefully this fixes bug #1123962 without causing other bugs ...
 *  The theory I'm basing this on is that if the dnd_convert proc is being
 *  called, then the drag has been dropped somewhere, so I can reset this
 *  flag ...
 */
  Fm->started_dnd = FALSE ;


/*  Interesting sidelight here. You cannot simply set the type in the reply
 *  to the type requested. It must be the actual type of the data returned.
 *  HOST_NAME, for example would be returnd as type STRING.
 */

  if (*type == X->A->done)
    {

/*  Destination has told us it has completed the drag and drop
 *  transaction. We should respond with a zero-length NULL reply.
 */

/* Yield ownership of the selection */

/* XXX: seln = dnd_object? */

/*  In the case of the clipboard, we don't necessarily want to automatically
 *  yield ownership. Let someone else come and get it.
 */

      if (xv_get(seln, SEL_RANK) != xv_get(X->server, SERVER_ATOM, "CLIPBOARD"))
        {
          XV_SET(seln, SEL_OWN, False, 0) ;

          if (Fm->debug) dout("\n\tdnd drop completed\n") ;

          cleanup_pitch(p) ;
          p = NULL ;
          XV_SET(seln, XV_KEY_DATA, Fm->Pitch_key, p, 0) ;
        }
      *format = 32 ;
      *length = 0 ;
      *data   = 0 ;
      *type   = X->A->null ;
      return(TRUE) ;
    }
  else if (*type == X->A->enum_count)
    {
      if (Fm->debug) dout(" : %d\n", p->num_items) ;

      length_buf = p->num_items ;
      *format = 32 ;
      *length = 1 ;
      *type = XA_INTEGER ;
      *data = (Xv_opaque) &length_buf ;
      return(TRUE) ;
    }
  else if (*type == X->A->enum_item)
    {
      Sel_prop_info *spi = NULL ;

      spi = (Sel_prop_info *)xv_get(seln, SEL_PROP_INFO) ;
      if (!spi || spi->length != 1 ||
           spi->format != 32 || spi->type != XA_INTEGER)
        {

/* If the length is not at least 2, format 32 then give up. */

          p->item = 0 ;        /* Default to 1st item. */
        }
      else
        {

/*  The catcher will signal he is done with an item by returning  a value
 *  of -1. Set to 0 so we will always return data, just in case.
 */

          p->item = *(unsigned *) spi->data ;
          if (p->item < 0) p->item = 0 ;        /* Default to 1st item */
        }

      ZFREE(spi) ;

      if (Fm->debug) dout(" : %d\n", p->item) ;

      *format = 32 ;
      *length = 0 ;
      *type = XA_INTEGER ;
      *data = 0  ;
      return(TRUE) ;
    }
  else if (*type == X->A->delete)
    {
      int i ;

/*  If we are moving bits, then we need to delete the original items.
 *  When moving/copying by filename, files are already gone.
 */

      if (Fm->debug) dout("\n\tdeleting files\n") ;

      for (i = 0; i < p->num_items; i++)
        {
          if (access(p->files[i].name, F_OK) == 0)
            {

/*  Just erase the files -- if they do not exist then ignore the error
 *  message. Put out other error messages though.
 */

                  if (unlink(p->files[i].name) && errno != ENOENT)
                    perror("filemgr") ;
            }
        }

      *format = 32 ;
      *length = 0 ;
      *data = 0 ;
      *type = X->A->null ;
      return(TRUE) ;
    }
  else if (*type == X->A->length)
    {
      if (Fm->debug) dout(" : %d\n", p->files[p->item].size) ;

      length_buf = p->files[p->item].size ;
      *format = 32 ;
      *length = 1 ;
      *type = XA_INTEGER ;
      *data = (Xv_opaque) &length_buf ;
      return(TRUE) ;
    }
  else if (*type == X->A->length)
    {
      if (p->link_copy)
        {
          length_buf = 1 ;
          *format = 32 ;
          *length = 1 ;
          *type = XA_INTEGER ;
          *data = (Xv_opaque) &length_buf ;
          return(TRUE) ;
        }
    }
  else if (*type == X->A->host)
    {

/* Return the hostname that the application is running on. */

      if (Fm->debug) dout(" : %s\n", Fm->hostname) ;
      if (Fm->hostname == NULL) return(FALSE) ;

      *format = 8 ;
      *length = strlen((char *) Fm->hostname) ;
      *data = (Xv_opaque) Fm->hostname ;
      *type = XA_STRING ;
      return(TRUE) ;
    }
  else if (*type == X->A->name)
    {

/* Return the hostname that the application is running on */

      if (Fm->debug) dout(" : %s\n", Fm->hostname) ;
      if (Fm->hostname == NULL) return(FALSE) ;

      *format = 8 ;
      *length = strlen((char *) Fm->hostname) ;
      *data = (Xv_opaque) Fm->hostname ;
      *type = XA_STRING ;
      return(TRUE) ;
    }
  else if (*type == X->A->file)
    {

/* Return the full pathname of the file being transferred */

      if (Fm->debug) dout("\n\t%s\n", p->files[p->item].name) ;
      if (p->files[p->item].name == NULL) return(FALSE) ;

      *format = 8 ;
      *length = strlen(p->files[p->item].name) ;
      *data = (Xv_opaque) p->files[p->item].name ;
      *type = XA_STRING ;
      return(TRUE) ;
    }
  else if (*type == X->A->label)
    {

/* Return the basename of the file being transferred */

      if (Fm->debug) dout(" : %s\n", p->files[p->item].data_label) ;
      if (p->files[p->item].data_label == NULL) return(FALSE) ;

      *format = 8 ;
      *length = strlen(p->files[p->item].data_label) ;
      *data = (Xv_opaque) p->files[p->item].data_label ;
      *type = XA_STRING ;
      return(TRUE) ;
    }
  else if ((*type == X->A->location) && p->files[p->item].location_good)
    {
      location_buf[0] = p->files[p->item].x_loc ;
      location_buf[1] = p->files[p->item].y_loc ;
      if (Fm->debug) dout(" : %d %d\n", location_buf[0], location_buf[1]) ;
      *format = 32 ;
      *length = 2 ;
      *type = XA_INTEGER ;
      *data = (Xv_opaque) &location_buf ;
      return(TRUE) ;
    }
  else if ((*type == X->A->cursor_loc) && (xv_get(seln, SEL_RANK) != xv_get(X->server, SERVER_ATOM, "CLIPBOARD")))
    {
      location_buf[0] = p->x_loc ;
      location_buf[1] = p->y_loc ;
      if (Fm->debug) dout(" : %d %d\n", location_buf[0], location_buf[1]) ;
      *format = 32 ;
      *length = 2 ;
      *type = XA_INTEGER ;
      *data = (Xv_opaque) &location_buf ;
      return(TRUE) ;
    }
  else if (*type == X->A->execute)
    {
      static int executable ;

      executable = p->files[p->item].type == FT_APP ? 1 : 0 ;

      if (Fm->debug) dout(" : %s\n", executable ? "true" : "false") ;

      *format = 32 ;
      *length = 1 ;
      *data = (Xv_opaque) &executable ;
      *type = XA_INTEGER ;
    }
  else if (*type == X->A->targets)
    {
      static int i ;             /* Keep around after function exits. */

/*  This request should return all of the targets that can be converted on
 *  this selection. This includes the types, as well as the queries that
 *  can be issued.
 */

      *format = 32 ;
      *type = XA_ATOM ;
      i = 0 ;
      target_list[i++] = XA_STRING ;
      target_list[i++] = X->A->text ;
      target_list[i++] = X->A->delete ;
      target_list[i++] = X->A->targets ;
      target_list[i++] = X->A->host ;
      target_list[i++] = X->A->length ;
      target_list[i++] = X->A->done ;
      target_list[i++] = X->A->atm ;
      target_list[i++] = X->A->enum_item ;
      target_list[i++] = X->A->enum_count ;
      target_list[i++] = X->A->label ;
      target_list[i++] = X->A->file ;
      target_list[i++] = X->A->atm_file ;
      target_list[i++] = X->A->location ;
      target_list[i++] = X->A->cursor_loc ;
      target_list[i++] = X->A->execute ;
      if (p->link_copy)
        target_list[i++] = X->A->link_copy ;
      *length = i ;

      if (Fm->debug)
        {
          dout("\n") ;
          print_atom_list(target_list, i) ;
        }

      *data = (Xv_opaque) target_list ;
      return(TRUE) ;
    }
  else if (*type == X->A->atm)
    {
      static int i ;             /* Keep around after function exits. */

      *format = 32 ;
      *type = XA_ATOM ;
      i = 0 ;
      types_list[i++] = X->A->atm_file ;
      *length = i ;
      *data = (Xv_opaque) types_list ;

      if (Fm->debug)
        {
          dout("\n") ;
          print_atom_list(types_list, i) ;
        }
      return(TRUE) ;
    }
  else if (*type == X->A->available)
    {
      static int i ;             /* Keep around after function exits. */

/*  This target returns all of the data types that the holder can convert on
 *  the selection.
 */

      *format = 32 ;
      *type = XA_ATOM ;

/* Send _SUN_TYPE_<tns entry> atom first for brain-dead mailtool */

      i = 0 ;
      types_list[i++] = p->files[p->item].type_name_atom ;
      types_list[i++] = XA_STRING ;
      *length = i ;
      *data = (Xv_opaque) types_list ;

      if (Fm->debug)
        {
          dout("\n") ;
          print_atom_list(types_list, i) ;
        }
      return(TRUE) ;
    }
  else if ((*type == XA_STRING) || (*type == X->A->text) ||
           (*type == p->files[p->item].type_name_atom))
    {

/*  If the number of bytes will fit into one buffer, then we just ship the
 *  whole thing at once. If it is greater that the size of a buffer, we need
 *  to go into sending INCR messages.
 */

      if (!p->bitsending)
        {
          if (p->files[p->item].type == FT_DIR)
            {
              fm_msg(TRUE, Str[(int) M_NO_APP_DRAG]) ;
              return(0) ;
            }
          else if (p->files[p->item].type == FT_SYS)
            {
              fm_msg(TRUE, Str[(int) M_NO_SYS_DRAG]) ;
              return(0) ;
            }
          else if (p->files[p->item].type != FT_APP &&
                   p->files[p->item].type != FT_DOC)
            {
              fm_msg(TRUE, Str[(int) M_ONLY_DRAG]) ;
              return(0) ;
            }

          if (Fm->debug)
            dout("\n\ttrying to open %s\n", p->files[p->item].name) ;

          if ((p->fp = fopen(p->files[p->item].name, "r")) == NULL)
            {
              fm_msg(TRUE, Str[(int) M_NOOPEN_FILE],
                     p->files[p->item].name) ;
              return(FALSE) ;
            }

          if (p->files[p->item].size <= MIN(XFER_BUFSIZ - 1, *length))
            {

/* It all fits, ship without using INCR. */

              p->files[p->item].size = get_file_contents(p->fp,
                                   p->xfer_buf, MIN(XFER_BUFSIZ, *length)) ;

              if (Fm->debug)
                dout("\tsending bits all at once, size=%d\n",
                     p->files[p->item].size) ;

              FCLOSE(p->fp) ;

              *format = 8 ;
              *length = p->files[p->item].size ;
              *data = (Xv_opaque)p->xfer_buf ;

              if (*type == X->A->text)
                *type = p->files[p->item].type_name_atom ;

              return(TRUE) ;
            }
          else
            {

/* Too big. Set up for shipping the stream as chunks of data. */

              if (Fm->debug)
                dout("\n\ttoo big to ship at once, start asking for %d chunks\n",
                     p->files[p->item].size) ;

              length_buf = p->files[p->item].size ;
              *type = X->A->incr ;
              *length = 1 ;
              *format = 32 ;
              *data = (Xv_opaque) &length_buf ;
              p->bitsending = TRUE ;
              return(TRUE) ;
            }
        }
      else
        {

/* Auxilliary request for more data. */

          p->files[p->item].size = get_file_contents(p->fp,
                                     p->xfer_buf, MIN(XFER_BUFSIZ, *length)) ;

          if (Fm->debug)
            dout("\n\tsending bits, chunk size=%d\n", p->files[p->item].size) ;

          *length = p->files[p->item].size ;
          *format = 8 ;
          *data = (Xv_opaque) p->xfer_buf ;

          if (*type == X->A->text)
            *type = p->files[p->item].type_name_atom ;

          if (*length == 0)
            {
              p->bitsending = FALSE ;
              FCLOSE(p->fp) ;
            }
          return(TRUE) ;
        }
    }
  else
    {
      if (Fm->debug)
        dout("\n\tlet the default convert proc take this\n") ;

/* Let the default convert procedure deal with the request. */

      return(sel_convert_proc(seln, type, data,
                              (unsigned long *) length, format)) ;
    }
/*NOTREACHED*/
}


/* Setup the pitch (dnd send object) with selected items info. */

static void
setup_path_tree_dnd_selections(ec, pitch, cut)
Event_Context ec ;
Pitch pitch ;
int cut ;
{
  Tree_Pane_Object *src_node ;
  register Tree_Pane_Object *t ;
  char *type_name, buf[MAXPATHLEN], name[MAXPATHLEN] ;
  register int cnt = 0 ;
  char *s_ptr, *b_ptr ;

/*  In the case of a cut operation, some funny things have to go on here.
 *  The data label has to be set to the old name, the file needs to me
 *  renamed to "..filename", and the new filename inserted into the
 *  pitch list.  When the selection later gets lost, all the files in the
 *  pitch list will be cleaned up.
 *
 *  When this is all said and done, we need to remember to refresh the folder
 *  and tree panes.
 */

  pitch->num_items = 1 ;
  pitch->files = (Pitch_File_Object *)
             LINT_CAST(calloc(pitch->num_items, sizeof(Pitch_File_Object))) ;

  if (!pitch->files)
    {
      fm_msg(TRUE, Str[(int) M_NO_TREE_DND]) ;
      pitch->num_items = 0 ;
      return ;
    }

  t = ec->tree_item ;
  STRCPY(name, (char *) t->name) ;
  fm_getpath(t, (WCHAR *) buf) ;
  s_ptr = buf ;

  if (cut)
    {

/* Move the directory in question to ..directory, and save it in the pitch. */

      pitch->files[cnt].name = (char *) fm_malloc(strlen(s_ptr) + 4) ;
      SPRINTF(pitch->files[cnt].name, "%s", s_ptr) ;
      b_ptr = pitch->files[cnt].name + strlen(s_ptr) + 2 ;
      while (*(b_ptr - 2) != '/')
        {
          *b_ptr = *(b_ptr - 2) ;
          b_ptr-- ;
        }

      *b_ptr = '.' ;
      *(b_ptr - 1) = '.' ;

      rename(s_ptr, pitch->files[cnt].name) ;

/*  If we've deleted a folder and we are displaying a tree view, then the
 *  tree node structure needs to be updated, before redisplaying the path/
 *  tree pane. It's done here, rather then where drawtree is set, just in
 *  case the delete failed.
 */

      *(b_ptr - 2) = '\0' ;
      src_node = path_to_node(WNO_TREE, (WCHAR *) s_ptr) ;
      if (src_node != NULL && src_node->parent != NULL)
        {
          remove_tree_entry(src_node->parent, (WCHAR *) (b_ptr + 1)) ;
          Fm->tree.selected = NULL ;
          fm_drawtree(TRUE) ;
        }
      *(b_ptr - 2) = '/' ;
    }
  else
    {
      pitch->files[cnt].name = (char *) fm_malloc(strlen(s_ptr) + 2) ;
      SPRINTF(pitch->files[cnt].name, "%s", s_ptr) ;
    }

  pitch->files[cnt].data_label = strdup(name) ;
  pitch->files[cnt].size  = 0 ;            /* Folders are sent w/o a size. */
  pitch->files[cnt].wno   = ec->wno ;
  pitch->files[cnt].type  = FT_UNCHECKED ;
  pitch->files[cnt].location_good = FALSE ;
  type_name = get_default_type_name(FT_DIR) ;
  if (type_name)
    {
      SPRINTF(buf, "%s%s", "_SUN_TYPE_", type_name) ;
      pitch->files[cnt].type_name_atom = xv_get(X->server, SERVER_ATOM, buf) ;
    }
  else pitch->files[cnt].type_name_atom = X->A->null ;
}


/* Setup the pitch (dnd send object) with selected file pane objects. */

/*ARGSUSED*/
static void
setup_file_dnd_selections(ec, pitch, cut)
Event_Context ec ;
Pitch pitch ;
int cut ;
{
  File_Pane_Object **curr, **end, **start ;  /* File pane object ptrs. */
  register int cnt = 0, obj = 0 ;
  char buf[128], buf1[MAXPATHLEN], *s, *s_ptr ;
  int found_selection, i ;
  int redraw_tree  = FALSE;

/*  In the case of a cut operation, some funny things have to go on here.
 *  The data label has to be set to the old name, the file needs to me
 *  renamed to "..filename", and the new filename inserted into the
 *  pitch list.  When the selection later gets lost, all the files in the
 *  pitch list will be cleaned up.
 *
 *  When this is all said and done, we need to remember to refresh the folder
 *  and tree panes.
 */

  pitch->num_items = 0 ;
  pitch->num_items += number_selected_items(ec->wno) ;

  pitch->files = (Pitch_File_Object *)
             LINT_CAST(calloc(pitch->num_items, sizeof(Pitch_File_Object))) ;

  if (!pitch->files)
    {
      fm_msg(TRUE, Str[(int) M_NO_FILE_DND]) ;
      pitch->num_items = 0 ;
      return ;
    }

  obj = 0 ;
  found_selection = FALSE ;

  end  = PTR_LAST(ec->wno) ;
  for (curr = PTR_FIRST(ec->wno); curr != end; curr++)
    {
      if ((*curr)->selected)
        {
          s_ptr = (char *) Fm->file[ec->wno]->path ;

          if (cut)
            {

/* Move the file in question to ..file, and save it in the pitch. */

              pitch->files[cnt].name = (char *)
                fm_malloc((*curr)->flen + strlen(s_ptr) + 4) ;
              SPRINTF(pitch->files[cnt].name, "%s/..%s", s_ptr, (*curr)->name) ;
              SPRINTF(buf1, "%s/%s", s_ptr, (*curr)->name) ;
              if (rename(buf1, (char *) pitch->files[cnt].name) != 0) {
			free(pitch->files[cnt].name);
			continue; /* may be Dos formatted floppy; limitation
					with pcfs: cant name file to ..name  */
       	      }

/*  If we've deleted a folder and we are displaying a tree view, then the
 *  tree node structure needs to be updated, before redisplaying the path/
 *  tree pane. It's done here, rather then where drawtree is set, just in
 *  case the delete failed.
 */

              if (((*curr)->type == FT_DIR) && Fm->treeview)
                {
                  Tree_Pane_Object *t ;

                  t = path_to_node(WNO_TREE, (WCHAR *) Fm->file[ec->wno]->path) ;
                  if (t != NULL)
                    {
                      remove_tree_entry(t, (WCHAR *) (*curr)->name) ;
                      redraw_tree = TRUE ;
                    }
                }
            }
          else
            {
              pitch->files[cnt].name = (char *)
                fm_malloc((*curr)->flen + strlen(s_ptr) + 2) ;
              SPRINTF(pitch->files[cnt].name, "%s/%s", s_ptr, (*curr)->name) ;
            }
          pitch->files[cnt].data_label = strdup((char *)(*curr)->name) ;
          pitch->files[cnt].size  = (*curr)->size ;
          pitch->files[cnt].type  = (*curr)->type ;
          pitch->files[cnt].wno   = ec->wno ;
          pitch->files[cnt].index = obj ;
          if (cnt)
            {
              pitch->files[cnt].x_loc = get_file_obj_x(ec->wno,
                                          (*curr)) - pitch->files[0].x_loc ;
              pitch->files[cnt].y_loc = get_file_obj_y(ec->wno,
                                          (*curr)) - pitch->files[0].y_loc ;
              pitch->files[cnt].location_good = TRUE ;
            }
          else
            {
              pitch->files[cnt].x_loc = get_file_obj_x(ec->wno, (*curr)) ;
              pitch->files[cnt].y_loc = get_file_obj_y(ec->wno, (*curr)) ;
              pitch->files[cnt].location_good = TRUE ;
            }
          if (Fm->debug)
            dout(" pitch setup, object %d location : %d %d\n",
                 cnt, pitch->files[cnt].x_loc, pitch->files[cnt].y_loc) ;

          if ((s = get_type_name(*curr)) != NULL)
            {
              SPRINTF(buf, "%s%s", "_SUN_TYPE_", s) ;
              pitch->files[cnt].type_name_atom =
                xv_get(X->server, SERVER_ATOM, buf) ;
            }
          else pitch->files[cnt].type_name_atom = X->A->null ;

          cnt++ ;
          found_selection = TRUE ;
        }
      obj++ ;
    }

  pitch->files[0].x_loc = 0 ;
  pitch->files[0].y_loc = 0 ;

  pitch->x_loc = ec->start_x - Fm->drag_minx ;
  if (pitch->x_loc < 0)             pitch->x_loc = 0 ;
  if (pitch->x_loc > Fm->drag_maxx) pitch->x_loc = Fm->drag_maxx ;

  pitch->y_loc = ec->start_y - Fm->drag_miny ;
  if (pitch->y_loc < 0)             pitch->y_loc = 0 ;
  if (pitch->y_loc > Fm->drag_maxy) pitch->y_loc = Fm->drag_maxy ;

  if (cut && found_selection)
    {
      start = PTR_FIRST(ec->wno) ;
      end   = PTR_LAST(ec->wno) ;
      for (curr = start; curr < end; curr++) (*curr)->redisplay = TRUE ;

      for (i = 0; i < cnt; i++)
        {
          curr = PTR_FIRST(ec->wno) + pitch->files[i].index ;
          (*curr)->redisplay = FALSE ;
          delete_icon(ec->wno, pitch->files[i].index,
                      get_file_obj_x(ec->wno, (*curr)),
                      get_file_obj_y(ec->wno, (*curr))) ;
        }
      condense_folder(ec->wno) ;
      redisplay_folder(ec->wno, 0, 0, get_pos_attr(ec->wno, FM_POS_CWIDTH),
                                      get_pos_attr(ec->wno, FM_POS_CHEIGHT)) ;
      set_status_info(ec->wno) ;
    }

  if (redraw_tree) fm_drawtree(TRUE) ;
}


/* Setup the dnd send context. */

static Pitch
create_pitch(ec, cut)
Event_Context ec ;
int cut ;
{
  Pitch pitch ;

  pitch = (Pitch) LINT_CAST(fm_malloc(sizeof(Dnd_Send_Object))) ;
  MEMSET((char *) pitch, 0, sizeof(Dnd_Send_Object)) ;

  pitch->item = 0 ;                /* default to 1st item */
  pitch->cut_to_clipboard = cut ;

  if (ec->wno == WNO_TREE || ec->ispath)
    setup_path_tree_dnd_selections(ec, pitch, cut) ;
  else
    setup_file_dnd_selections(ec, pitch, cut) ;

#ifdef DND_DEBUG
  if (1)
    {
      int i ;

      FPRINTF(stderr, "- pitch=%d, hostname=%s", pitch, Fm->hostname) ;
      FPRINTF(stderr, "- pitch drag items:\n") ;
      for (i = 0; i < pitch->num_items; i++)
        FPRINTF(stderr, "  file %3d: size=%8d, name=%s\n",
                i, pitch->files[i].size, pitch->files[i].data_label) ;
    }
#endif /*DND_DEBUG*/

  add_to_plist(&X->Pitch_list, pitch) ;
  return(pitch) ;
}


/* Create the dnd send object, including the context of the send. */

Dnd
create_dnd_object(ec)
Event_Context ec ;
{
  Boolean isfdr = TRUE ;
  Canvas c ;
  Dnd dnd_object ;
  Xv_Cursor cursor, acursor ;
  Pitch pitch ;

  DB("\ncreate_dnd_object() called\n") ;

  if (Fm->Pitch_key == -1)
    Fm->Pitch_key = xv_unique_key() ;        /* Dnd sender context key. */

  pitch = create_pitch(ec, FALSE) ;

       if (ec->wno == WNO_TREE) c = X->treeX.canvas ;
  else if (ec->ispath)          c = X->fileX[ec->wno]->pathX.canvas ;
  else                          c = X->fileX[ec->wno]->canvas ;

  if (ec->wno != WNO_TREE && !ec->ispath)
    {
      isfdr   = (Fm->file[ec->wno]->object[ec->item]->type == FT_DIR) ;
      cursor  = X->drag_cursor ;
      acursor = X->drag_cursor ;
    }
  else
    {
      cursor  = get_drag_cursor(isfdr,   ec->control, (pitch->num_items > 1)) ;
      acursor = get_accept_cursor(isfdr, ec->control, (pitch->num_items > 1)) ;
    }
  if (!cursor) return(0) ;

  dnd_object = (Dnd) xv_create(c, DRAGDROP,
                       SEL_CONVERT_PROC,  dnd_convert,
                       DND_TYPE,          (ec->control ? DND_COPY : DND_MOVE),
                       XV_KEY_DATA,       Fm->Pitch_key, pitch,
                       DND_CURSOR,        cursor,
                       DND_ACCEPT_CURSOR, acursor,
                       0) ;

  if (!dnd_object) fm_msg(TRUE, Str[(int) M_NOCREATE_OBJ]) ;

  return(dnd_object) ;
}


static void
add_dlist(d_list, dirname, new_file)
struct Dnd_Display_Struct **d_list ;
char *dirname, *new_file ;
{
  struct Dnd_Display_Struct *s_ptr, *b_ptr ;

#ifdef DND_DEBUG
    FPRINTF(stderr, "enter add_dlist\n") ;
#endif /*DND_DEBUG*/

  if (!dirname) return ;

  if (!*d_list)
    {

#ifdef DND_DEBUG
      FPRINTF(stderr, "adding folder %s\n", dirname) ;
      if (new_file) fprintf(stderr, "adding file %s\n", new_file) ;
#endif /*DND_DEBUG*/

      s_ptr = *d_list = (struct Dnd_Display_Struct *)
                   LINT_CAST(calloc(1, sizeof(struct Dnd_Display_Struct))) ;
      s_ptr->dir_name = strdup(dirname) ;
      if (new_file) s_ptr->justify = strdup(new_file) ;
      s_ptr->next = NULL;
    }
  else
    {
      s_ptr = *d_list ;
      b_ptr = s_ptr ;

      while (s_ptr)
        {
          if (!strcmp(b_ptr->dir_name, dirname))
            {
              if (new_file)
                {
                  ZFREE(b_ptr->justify) ;
                  b_ptr->justify = strdup(new_file) ;
                }
              return ;
            }
          else
            {
              b_ptr = s_ptr ;
              s_ptr = s_ptr->next ;
            }
        }

#ifdef DND_DEBUG
      FPRINTF(stderr, "adding folder %s\n", dirname) ;
      if (new_file) fprintf(stderr, "adding file %s\n", new_file) ;
#endif /*DND_DEBUG*/

      s_ptr = b_ptr->next = (struct Dnd_Display_Struct *)
                     LINT_CAST(calloc(1, sizeof(struct Dnd_Display_Struct))) ;
      s_ptr->dir_name = strdup(dirname) ;
      if (new_file) s_ptr->justify = strdup(new_file) ;
      s_ptr->next = NULL ;
    }
}


static void
destroy_dlist(d_list)
struct Dnd_Display_Struct **d_list ;
{
  struct Dnd_Display_Struct *s_ptr = *d_list ;
  struct Dnd_Display_Struct *n_ptr = s_ptr ;

  while (s_ptr)
    {
      ZFREE(s_ptr->dir_name) ;
      ZFREE(s_ptr->justify) ;
      n_ptr = s_ptr->next ;
      FREE(s_ptr) ;
      s_ptr = n_ptr ;
    }
  *d_list = NULL ;
}


static void
display_dlist(d_list)
struct Dnd_Display_Struct **d_list ;
{
  struct Dnd_Display_Struct *s_ptr = *d_list ;

  while (s_ptr)
    {

#ifdef DND_DEBUG
      FPRINTF(stderr, "redisplaying folder %s\n", s_ptr->dir_name) ;
#endif /*DND_DEBUG*/

      display_all_folders(FM_BUILD_FOLDER, s_ptr->dir_name) ;
      if (s_ptr->justify)
        scroll_to_file_and_select(s_ptr->dir_name, s_ptr->justify) ;
      s_ptr = s_ptr->next ;
    }
}


static void
add_to_plist(pitch_list, pitch)
struct Pitch_List_Struct **pitch_list ;
Pitch pitch ;
{
  if (*pitch_list == NULL)
    {
      *pitch_list = (struct Pitch_List_Struct *)
                    LINT_CAST(calloc(sizeof(struct Pitch_List_Struct), 1)) ;
      (*pitch_list)->right = NULL ;
      (*pitch_list)->left = NULL ;
      (*pitch_list)->pitch = pitch ;
    }
  else
    {
      struct Pitch_List_Struct *tmp = *pitch_list ;
      while (tmp->right)
        {
          if (tmp->pitch == pitch) return ;
          tmp = tmp->right ;
        }
      tmp->right = (struct Pitch_List_Struct *)
                   LINT_CAST(calloc(sizeof(struct Pitch_List_Struct), 1)) ;
      tmp->right->left = tmp ;
      tmp->right->right = NULL ;
      tmp->right->pitch = pitch ;
    }
}


static void
delete_from_plist(pitch_list, pitch)
struct Pitch_List_Struct **pitch_list ;
Pitch pitch ;
{
  struct Pitch_List_Struct *p_ptr = *pitch_list ;

  do
    {
      if (p_ptr->pitch == pitch)
        {
          if (p_ptr == *pitch_list) *pitch_list = p_ptr->right ;
          else                      p_ptr->left->right = p_ptr->right ;

          if (p_ptr->right) p_ptr->right->left = p_ptr->left ;

          FREE(p_ptr) ;
          return ;
        }
      else p_ptr = p_ptr->right ;
    }
  while (p_ptr) ;
}


int
compare_with_plist(file)
char *file ;
{
  struct Pitch_List_Struct *pitch_list = X->Pitch_list ;

  while (pitch_list)
    {
      int file_count = pitch_list->pitch->num_items ;
      int i ;
      Pitch_File_Object *object_set = pitch_list->pitch->files ;

      for (i = 0; i < file_count; i++)
        if (strcmp(object_set[i].name, file) == 0) return(TRUE) ;

      pitch_list = pitch_list->right ;
    }
  return(FALSE) ;
}


void
paste_like_dnd(ec)
Event_Context ec ;
{
  Catch catch = NULL ;
  Canvas canvas = 0 ;
  Selection_requestor shelf_requestor = 0 ;

  if (Fm->Catch_key == -1)
    Fm->Catch_key = xv_unique_key() ;        /* Dnd receive context key. */

  ec->id = ACTION_DRAG_COPY ;

  catch = create_catch(ec) ;
  catch->is_paste = TRUE ;

/* Create the requestor for the dnd drop. */

  canvas = (ec->wno == WNO_TREE ? X->treeX.canvas : X->fileX[ec->wno]->canvas) ;

/* if (!shelf_requestor) */

  shelf_requestor = xv_create(canvas, SELECTION_REQUESTOR,
    SEL_RANK,       xv_get(X->server, SERVER_ATOM, "CLIPBOARD"),
    SEL_REPLY_PROC, dnd_catch_proc,
    XV_KEY_DATA,    Fm->Catch_key, catch,
    0) ;

  load_from_dnd(shelf_requestor) ;
}


static void
lose_clipboard(selection)
Selection_owner selection ;
{
  Pitch p = (Pitch) xv_get(selection, XV_KEY_DATA, Fm->Pitch_key) ;
  int i ;
  struct stat statbuf ;

  if (p->cut_to_clipboard)
    {

/*  All the files in this pitch were put there thru the action of a "cut".
 *  When the selection is taken by someone else, the files may be cleaned
 *  up.
 */

      for (i = 0; i < p->num_items; i++)
        {
          fm_stat(p->files[i].name, &statbuf) ;
          if (S_ISDIR(statbuf.st_mode))
            {
              Tree_Pane_Object *src_node ;
              char buf[MAXPATHLEN] ;
              char *s ;

              SPRINTF(buf, "rm -rf \"%s\"", p->files[i].name) ;
              if (fm_run_str(buf, TRUE)) fm_showerr(buf) ;

              STRCPY(buf, p->files[i].name) ;
              s = strrchr(buf, '/') ;
              *s = '\0' ;
              s++ ;

              src_node = path_to_node(WNO_TREE, (WCHAR *) buf) ;
              if (src_node  != NULL)
                remove_tree_entry(src_node, (WCHAR *) s) ;
              fm_drawtree(TRUE) ;
            }
          else unlink(p->files[i].name) ;
        }
    }

  cleanup_pitch((Pitch) xv_get(selection, XV_KEY_DATA, Fm->Pitch_key)) ;
  XV_SET(selection, XV_KEY_DATA, Fm->Pitch_key, NULL, 0) ;
}


void
copy_like_dnd(ec)
Event_Context ec ;
{
  copy_link_dnd(ec, FALSE) ;
}


void
link_like_dnd(ec)
Event_Context ec ;
{
  copy_link_dnd(ec, TRUE) ;
}


void
copy_link_dnd(ec, link)
Event_Context ec ;
int link ;
{
  Canvas canvas = 0 ;
  Pitch p ;

  canvas = (ec->wno == WNO_TREE ? X->treeX.canvas : X->fileX[ec->wno]->canvas) ;

  if (Fm->Pitch_key == -1)
    Fm->Pitch_key = xv_unique_key() ;        /* Dnd sender context key. */

  if (!X->Shelf_owner)
    X->Shelf_owner = xv_create(canvas, SELECTION_OWNER,
      SEL_RANK,      xv_get(X->server, SERVER_ATOM, "CLIPBOARD"),
      SEL_LOSE_PROC, lose_clipboard,
      0) ;

  if (!X->Shelf_item) X->Shelf_item = xv_create(X->Shelf_owner, SELECTION_ITEM, 0) ;

  XV_SET(X->Shelf_owner, SEL_OWN, FALSE, 0) ;
  XV_SET(X->Shelf_owner, SEL_OWN, TRUE, 0) ;
  p = create_pitch(ec, FALSE) ;
  p->link_copy = link ;
  XV_SET(X->Shelf_owner, XV_KEY_DATA, Fm->Pitch_key, p, 0) ;
  XV_SET(X->Shelf_owner, SEL_CONVERT_PROC, dnd_convert, 0) ;

  fm_msg(FALSE, Str[(int) M_HOWTOPASTE],
         link ? Str[(int) M_LINK] : Str[(int) M_COPY]) ;
}


void
cut_like_dnd(ec)
Event_Context ec ;
{
  Canvas canvas = 0 ;
  Pitch p ;

  canvas = (ec->wno == WNO_TREE ? X->treeX.canvas : X->fileX[ec->wno]->canvas) ;

  if (Fm->Pitch_key == -1)
    Fm->Pitch_key = xv_unique_key() ;        /* Dnd sender context key. */

  if (!X->Shelf_owner)
    X->Shelf_owner = xv_create(canvas, SELECTION_OWNER,
      SEL_RANK,      xv_get(X->server, SERVER_ATOM, "CLIPBOARD"),
      SEL_LOSE_PROC, lose_clipboard,
      0) ;

  if (!X->Shelf_item) X->Shelf_item = xv_create(X->Shelf_owner, SELECTION_ITEM, 0) ;

  XV_SET(X->Shelf_owner, SEL_OWN, FALSE, 0) ;
  XV_SET(X->Shelf_owner, SEL_OWN, TRUE, 0) ;
  p = create_pitch(ec, TRUE) ;
  XV_SET(X->Shelf_owner, XV_KEY_DATA, Fm->Pitch_key, p, 0) ;
  XV_SET(X->Shelf_owner, SEL_CONVERT_PROC, dnd_convert, 0) ;
  fm_msg(FALSE, Str[(int) M_HOWTOPASTE], Str[(int) M_MOVE]) ;
}


void
yield_shelf()
{
  if (X->Shelf_owner) XV_SET(X->Shelf_owner, SEL_OWN, FALSE, 0) ;
}
