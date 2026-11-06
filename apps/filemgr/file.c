#ifndef lint
static char sccsid[]="@(#)file.c	1.183 04/16/97 Copyright 1987-1992 Sun Microsystem, Inc." ;
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

#ifdef __STDC__
#include <stdarg.h>              /* For variable length declarations. */
#else
#include <varargs.h>
#endif /*__STDC__*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <utmp.h>
#include <sys/mman.h>
/* Linux portability definitions are in linux_port.h. */
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <rpc/rpc.h>
#include <rpcsvc/ypclnt.h>
#include <rpcsvc/yp_prot.h>

#include "support/ce_err.h"

#ifdef SVR4
/* Linux portability definitions are in linux_port.h. */
/* Linux portability definitions are in linux_port.h. */
#include <pixrect/rasterfile.h>
#else
#include <a.out.h>
#include <mntent.h>
#include <rasterfile.h>
#endif /*SVR4*/

#include "defs.h"
#include "fm.h"
#include "xdefs.h"

extern FmVars Fm ;
extern FMXlib X ;
extern char *Str[] ;

static char *nullstr  = "(null)" ;
static char *lowerhex = "0123456789abcdef" ;

static short Month_days[12] = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
} ;


int
add_children(f_p, path, new)          /* Add children to current node. */
register Tree_Pane_Object *f_p ;      /* Node. */
char *path ;                          /* Pathname to node. */
Boolean *new ;                        /* Children found? */
{
  char errbuf[MAXPATHLEN] ;
  char stat_fname[MAXPATHLEN] ;       /* Full pathname to stat. */
  register Tree_Pane_Object *nf_p ;   /* Next tree pointer. */
  Tree_Pane_Object *child[256] ;      /* Subdirectories. */
  register Tree_Pane_Object *existing_child ;   /* Ignore existing child. */
  register int nchild ;                         /* Number of children. */
  register struct dirent *dp ;                  /* File in directory. */
  DIR *dirp ;                                   /* Directory file ptr. */
  struct stat fstatus ;                         /* Status info. */
  register int i ;                              /* Index. */
  Boolean compare ;

/*  Add_children will return 0 on success. There are also two error exits:
 *  -1  - cannot allocate memory for tree node.
 *  -2  -  cannot open directory.
 */

/* Bitch if we can't get in... */

  if ((fm_stat(path, &fstatus) == -1) ||
     ((dirp = opendir(path)) == NULL))
    {
      SPRINTF(errbuf, Str[(int) M_CHDIR], path, strerror(errno)) ;
      write_footer(WNO_TREE, errbuf, TRUE) ;
      return(-2) ;
    }

  compare = f_p->mtime && f_p->child ;
  f_p->mtime = fstatus.st_mtime ;
  nchild = 0 ;
  *new = FALSE ;

/* If directory has greater than two links, then there are subdirectories. */

  if (fstatus.st_nlink > 2)
    while ((dp = readdir(dirp)) != NULL)
      {
      if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        continue;

/*  Ignore '.' and '..' and any existing child.
 *  (Names beginning with ".." are hidden for undelete).
 */

        if (dp->d_name[0] == '.' &&
           (dp->d_name[1] == '.' || dp->d_name[1] == '\0'))
          continue ;

        if (!compare)
          if ((f_p->child &&
              (strcmp((char *) f_p->child->name,dp->d_name) == 0)))
            continue ;

        SPRINTF(stat_fname, "%s/%s", path, dp->d_name) ;
        if ((fm_stat(stat_fname, &fstatus) == 0) &&
            (fstatus.st_mode & S_IFMT) == S_IFDIR)
          {

/*  Found a directory, bitch and return if we can't allocate memory for
 *  structure or name.
 */

            if ((nf_p = make_tree_node((WCHAR *) dp->d_name)) == NULL)
              return(-1) ;

            nf_p->parent = f_p ;
            nf_p->child = NULL ;
            STRCPY((char *)nf_p->name, dp->d_name) ;
            nf_p->mtime = 0 ;
            nf_p->status = 0 ;

/* Remember each child. */

            child[nchild] = nf_p ;
            nchild++ ;
            if (nchild == 256) break ;
          }
      }
    CLOSEDIR(dirp) ;

  if (nchild)
    {
      if (nchild > 1)
        {

/* Sort children alphabetically. */

          qsort((char *) child, nchild,
                sizeof(Tree_Pane_Object *), (int (*)()) compare_name) ;

/* Fix sibling pointers. */

          for (i = 1; i < nchild; i++)
            child[i-1]->sibling = child[i] ;
        }
      child[nchild-1]->sibling = NULL ;

      if (compare)
        {
          compare_child(f_p->child, child[0], new) ;
          return(FM_SUCCESS) ;
        }

      existing_child = f_p->child ;
      f_p->child = child[0] ;

      if (existing_child)
        {

/* Insert existing child into sorted array ... */

          for (i = 0; i < nchild; i++)
            if (strcmp((char *) existing_child->name,
                       (char *) child[i]->name) < 0)
              {
                existing_child->sibling = child[i] ;
                if (i) child[i-1]->sibling = existing_child ;
                else   f_p->child = existing_child ;
                break ;
              }

            if (i == nchild)
              {

/* ...must belong at end. */

                child[i-1]->sibling = existing_child ;
                existing_child->sibling = NULL ;
              }
        }
      *new = TRUE ;
    }
  else if (compare)
    {
      fm_destroy_tree(f_p->child) ;
      f_p->child = NULL ;
      *new = TRUE ;
    }
  return(FM_SUCCESS) ;
}


void
adjust_tree(wno)     /* Adjust main pane directory tree for this window no. */
int wno ;
{
  char errbuf[MAXPATHLEN] ;
  char stat_fname[MAXPATHLEN] ;      /* Full pathname to stat. */
  DIR *dirp ;                        /* Directory file pointer. */
  File_Pane *f = Fm->file[wno] ;
  Tree_Pane_Object *f_p, *p_p ;
  struct dirent *dp ;                /* Current file in directory. */
  struct stat fstatus ;              /* Stat/lstat status information. */

/*  NOTE:
 *
 *  This can probably be improved by defining another entry type for the
 *  status field of a tree node. At the start of the routine, this status
 *  field can be turned on for all the childs sibling entries. When a
 *  directory is found, then this status field is turned off. The delete
 *  folder section then only has to look for entries with this field still
 *  set in the status byte and delete them, rather then stat'ing all the
 *  files again.
 */

  Fm->redraw_tree = FALSE ;

/* Get tree node for directory. */

  f_p = path_to_node(WNO_TREE, f->path) ;

/* Bitch if we can't get in... */

  if ((fm_stat((char *) f->path, &fstatus) == -1) ||
      ((dirp = opendir((char *) f->path)) == NULL))
    {
      SPRINTF(errbuf, Str[(int) M_CHDIR], f->path, strerror(errno)) ;
      write_footer(WNO_TREE, errbuf, TRUE) ;
      return ;
    }

  set_tree_icon(f_p, !is_frame_closed(wno)) ;
  while ((dp = readdir(dirp)) != NULL)  /* Add in new directory tree nodes. */
    {
      if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        continue;
                                        /*  Ignore '.' and '..' */
      if (dp->d_name[0] == '.' &&
         (dp->d_name[1] == '.' || dp->d_name[1] == '\0')) continue ;

      SPRINTF(stat_fname, "%s/%s", f->path, dp->d_name) ;
      if ((fm_stat(stat_fname, &fstatus) == 0) &&
          (fstatus.st_mode & S_IFMT) == S_IFDIR)
        {

/*  Add a tree entry for the directory found. Note that if the tree entry
 *  already exists, then this is a null operation.
 */
          add_tree_entry(f_p, (WCHAR *) dp->d_name) ;
        }
    }
  CLOSEDIR(dirp) ;

/*  Some folders may have been deleted from this directory. The childs
 *  sibling list is followed to the end, and each file is stat'ed to see
 *  if it still exists. If it doesn't, then that entry is removed.
 */

  p_p = f_p ;             /* Save pointer to parent node. */
  f_p = f_p->child ;
  while (f_p != NULL)
    {
      SPRINTF(stat_fname, "%s/%s", f->path, f_p->name) ;
      if (fm_stat(stat_fname, &fstatus) == -1) remove_tree_entry(p_p, f_p->name) ;
      f_p = f_p->sibling ;
    }
  if (Fm->redraw_tree == TRUE)
    {
      fm_drawtree(TRUE) ;
      Fm->redraw_tree = FALSE ;
    }
}


void
build_folder(mtime, widest, wno)
time_t *mtime ;             /* Change date on folder. */
register int *widest ;      /* Return widest name. */
register int wno ;
{
  char tmpname[MAXPATHLEN] ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p ;              /* File array pointers. */
  int i, j ;
  struct dirent *dp ;                   /* Directory content. */
  DIR *dirp ;                           /* Directory file pointer. */
  struct stat fstatus ;                 /* File status. */
  int ncount = 0;			/* Count of new files. */
  int dcount = 0;			/* Count of delete files. */

  fm_namestripe(wno) ;
  update_history(wno) ;

  f->num_objects = 0 ;
  if (!Fm->writable) Fm->writable = (fm_writable((char *) f->path) == 0) ;
  *widest = (f->display_mode != VIEW_BY_LIST) ? GLYPH_WIDTH : 0 ;

/* Read directory. */

  if (((dirp = opendir((char *) f->path)) == NULL) ||
       fstat(dirfd(dirp), &fstatus))
    {
      if (dirp) CLOSEDIR(dirp) ;
#if 0
      fm_msg(TRUE, Str[(int) M_CHDIR], f->path, strerror(errno)) ;
#endif
if (fm_stat((char *) f->path, &fstatus) == 0)
		*mtime = fstatus.st_mtime;
      return ;
    }

  *mtime = fstatus.st_mtime ;    /* To test against current node. */

  i   = 0 ;
  f_p = f->object ;



  while ((dp = readdir(dirp)) != NULL)
    {
      if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        continue;
      if (!f->see_dot)
        {

/* Ignore dot files if "hidden" toggle is pressed. */

          if (dp->d_name[0] == '.') continue ;
        }

      if (!Fm->writable)
        {
          SPRINTF(tmpname, "%s/%s", (char *) f->path, dp->d_name) ;
          Fm->writable = (fm_writable(tmpname) == 0) ;
        }

/* Allocate more memory to file array. */

     if (i == f->max_objects)
       if (alloc_more_files(wno) == FALSE) break ;
       else f_p = f->object + i ;

/* Apply filter... */

     if (f->filter[0])
       {
         j = b_match(dp->d_name, (char *) f->filter) ;
         if (j == 0) continue ;
         else if (j == -1)         /* Pattern in error, null it. */
           f->filter[0] = '\0' ;
       }

      j = strlen(dp->d_name) ;
      if (((*f_p)->name = (WCHAR *) fm_malloc((unsigned) j+1)) == NULL)
         return ;

      STRCPY((char *) (*f_p)->name, dp->d_name) ;
      (*f_p)->flen           = j ;
      (*f_p)->width          = fm_strlen((char *) (*f_p)->name) ;
      if ((*f_p)->width > *widest) *widest = (short) (*f_p)->width ;

      (*f_p)->type           = FT_UNCHECKED ;
      (*f_p)->color          = 1 ;       /* Foreground index = 1. */
      (*f_p)->tns_entry      = NULL ;
      (*f_p)->icon_fg_index = -1 ;
      (*f_p)->icon_bg_index = -1 ;
      (*f_p)->dirpos        = i ;

      /*
       *  ESC# 501162; BID# 1174102
       *
       *  Clear the origin values for the icons everytime a directory
       *  is openned.  This stops the bug where selected items maintain
       *  state between multiple change dirs (out of and back into a given
       *  directory.
       */
      (*f_p)->ox            = -1;
      (*f_p)->oy            = -1;

      SET_FP_IMAGE((char *) NULL, wno, i) ;
      f_p++ ;
      i++ ;
    }
  f->num_objects = i ;

  if (!Fm->isgoto_select)
  {
    if (is_visible(wno)) {
       char buf[MAXPATHLEN];
       if (f->prev_num_objects) {
	  if (i > f->prev_num_objects)
	     ncount = i - f->prev_num_objects;
          else
	     dcount = f->prev_num_objects - i;
       }
       else
	  ncount = i;
       if (!ncount && !dcount)
	  STRCPY(buf, Str[(int) M_UPDATEDNO]) ;
       else {
	  if (ncount && dcount) {
	     if ((ncount > 1) && (dcount > 1))
		SPRINTF(buf, Str[(int) M_UPDATEDASDS], ncount, dcount);
             else if ((ncount == 1) &&(dcount == 1))
		SPRINTF(buf, Str[(int) M_UPDATEDAD], ncount, dcount);
             else if (ncount > 1)
		SPRINTF(buf, Str[(int) M_UPDATEDASD], ncount, dcount);
             else
		SPRINTF(buf, Str[(int) M_UPDATEDADS], ncount, dcount);
          }
	  if (ncount) {
	     if (ncount == 1)
		SPRINTF(buf, Str[(int) M_UPDATEDA], ncount);
             else
		SPRINTF(buf, Str[(int) M_UPDATEDAS], ncount);
          }
	  if (dcount) {
	     if (dcount == 1)
		SPRINTF(buf, Str[(int) M_UPDATEDD], dcount);
	     else
		SPRINTF(buf, Str[(int) M_UPDATEDDS], dcount);
          }
       }
       if (!Fm->error_state)
         write_footer(wno, buf, FALSE) ;
    }
  }
  CLOSEDIR(dirp) ;
}


int
calc_days(mdy)      /* Validate and return # of days from string. */
char *mdy ;         /* Month, day, year. */
{
  int month, day, year, scanned ;
  short leap ;

  scanned = sscanf(mdy, "%d/%d/%d", &month, &day, &year) ;
  if (scanned == 2)
    {

/* Complete the year for the user. */

      struct tm *today ;

      year  = (int) time((time_t *) 0) ;
      today = localtime((long *) &year) ;
      year  = today->tm_year % 100 ;
    }
  else if (scanned != 3)
    return(-1) ;

/* Do we have a valid date? */

/* To fix Bug# 1255947
 * Should now be yr2000 compliant at Levels One and Two.
 *
 * This means that dates like CCYY are interpreted correctly, also dates
 * like YY are interpreted as follows:
 *    00 -> 37  are 2000 -> 2037 incl.
 *    70 -> 99  are 1970 -> 1999 incl.
 *    38 -> 69  are undefined - in this case not accepted!!
 */
  if ( year < 0 )
      return(-1);
  else if ( year < 1900 && year >=0 && year <= 99 )
    if ( year <= 37 )
      year += 2000;
    else if ( year >= 70 && year <= 99 )
      year += 1900;

  /* If we reach this point and the year is still < 1900, then is invalid */
  if ( year < 1900 )
      return(-1);

  leap = (year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0) ;
  if (month < 1 || month > 12 || day < 1 )
    return(-1) ;
/* If it's a leap year, then allow entry of 29 Feb */
  if (day > Month_days[month-1] + (month == 2 && leap ) )
    return(-1) ;

/*  We do indeed have a valid date, now calculate the number
 *  of days from today...
 */

  return(get_days(month, day, year)) ;
}


static int
show_new_dev_window(mtype, n, mask, devname, from_run_child)
enum media_type mtype ;
int n, *mask;
char *devname;
int from_run_child;

{
	FILE *fp ;
  	char mpath[2*MAXPATHLEN], path[MAXPATHLEN], rawpath[MAXPATHLEN] ;
	int i, fd, info[MAXINFO];
	FILE *mfp;
	struct mnttab  mp;
	Tree_Pane_Object *t_p ;
	int wno;
	File_Pane *f;
	static int last_time;

	if ((fp = fopen(devname, "r")) == NULL) {
		fm_msg(TRUE, Str[(int) M_NO_READ], devname) ;
		return 0;
	}
	FSCANF(fp, "%s %s", path, rawpath) ;
	FCLOSE(fp) ;
	/* from_run_child, if true, means we just did a format but the
	user canceled, which is why we are here, so we dont want to do
	another format */
#if 0
	if ((EQUAL(path, "unformatted") || EQUAL(path, "unlabeled"))
			&& !from_run_child) {
#endif
if (path[0] != '/' && !from_run_child) { /* this means no file system on disk */
		sprintf(mpath, "/vol/dev/aliases/floppy%01d", n);
		if ((fd = open(mpath, O_RDWR)) == -1) {
        		if (errno == EACCES || errno == EROFS) {
				write_footer(Fm->curr_wno,
					Str[(int) M_NO_WRITE], TRUE);
				Fm->file[Fm->curr_wno]->Message = TRUE;
                		return 0;
			}
		}
		close(fd);
		/* prevent two windows from coming up */
		if ((time(0) - last_time) > 3)
			do_format_disk("unlabeled", rawpath, path) ;
		last_time = time(0);
	}
	else {
		/* turn off timer so you dont get extra windows */
		set_timer(0);
		for (i = 0; i < MAXINFO; i++) info[i] = -1 ;
		if (path[0] != '/')
			if (mtype == FM_CD)
				sprintf(mpath, "/cdrom/%s", path);
			else
				sprintf(mpath, "/floppy/%s", path);
		else strcpy(mpath, path);
		if (is_window_showing(mpath, 0) ||
	 	     (mtype == FM_FLOPPY && is_window_showing_with_path(mpath))) {
			/* if we just reformatted from pcfs to ufs or vice-versa */
			if (mtype == FM_FLOPPY) {
			  if ((mfp = fopen(MNTTAB, "r")) == NULL) {
		      		set_timer(Fm->interval) ;
				return 0;
			  }
                          memset(&mp, 0, sizeof(mp));
                          while(fm_getmntent(mfp, &mp) != -1)
                            if (EQUAL(mp.mnt_mountp, mpath)) {
				for (wno = 0; wno < Fm->maxwin; wno++)
    					if (is_frame(wno) && strncmp(
						mpath, (char *)Fm->file[wno]->path, strlen(mpath)) == 0)
						break;
				if (wno == Fm->maxwin) {
		      			set_timer(Fm->interval) ;
					return 0;
				}
		      		Fm->mtime[wno] = 0;
                            	if (EQUAL(mp.mnt_fstype, "pcfs")) {
					if (Fm->file[wno]->mtype != FM_DOS) {
						if (Fm->file[wno]->path != NULL)
							free (Fm->file[wno]->path);
						Fm->file[wno]->path = (WCHAR*)strdup(mpath);
						Fm->file[wno]->mtype = FM_DOS;
						draw_folder(wno,
							path_to_node(wno,
							Fm->file[wno]->path),
							FT_DOS, FALSE) ;
					}
				}
				else if (Fm->file[wno]->mtype != FM_FLOPPY) {
					if (Fm->file[wno]->path != NULL)
						free (Fm->file[wno]->path);
					Fm->file[wno]->path = (WCHAR*)strdup(mpath);
					Fm->file[wno]->mtype = FM_FLOPPY;
					draw_folder(wno,
						path_to_node(wno,
						Fm->file[wno]->path),
						FT_FLOPPY, FALSE) ;
				}
			    }
		      }
		      set_timer(Fm->interval) ;
		      return 1;
		}
	       /** check for an unformatted floppy ******/

                if(mtype == FM_FLOPPY){

                        short is_mounted = FALSE;

                        if ((mfp = fopen(MNTTAB, "r")) == NULL) {
		      		set_timer(Fm->interval) ;
				return 0;
			}
                        memset(&mp, 0, sizeof(mp));
                          while(fm_getmntent(mfp, &mp) != -1){
                            if(EQUAL(mp.mnt_mountp, mpath)){
                                        is_mounted = TRUE;
                                        break;
                            }
                          }
                        fclose(mfp);
                        if (!is_mounted) {
				set_timer(Fm->interval) ;
				if (!from_run_child)
					do_format_disk("unlabeled", rawpath, path) ;
 				return 0; /* proceed if only mounted */
			}
                }

		FM_NEW_FOLDER_WINDOW("", mpath, rawpath, mtype, n,
					FALSE, info) ;
		FM_SETDEVICE(*mask, n) ;
		set_timer(Fm->interval) ;
	}
	return 1;
}

int
check_media(mtype, maxdevices, mask, from_run_child)
enum media_type mtype ;
int maxdevices, *mask, from_run_child ;
{
  	char *dname = "/tmp/.removable";
  	char devname[MAXPATHLEN];
	int exists, n;

	for (n = 0; n < maxdevices; n++) {
            	if (mtype == FM_CD)
			SPRINTF(devname, "%s/cdrom%01d",  dname, n) ;
            	else
			SPRINTF(devname, "%s/floppy%01d", dname, n) ;
            	exists = (access(devname, F_OK) == 0) ;
		if (FM_ISDEVICE(*mask, n)) {
			if (!exists) {
				handle_media_removal(mtype, n) ;
                    		if (mtype == FM_CD) {
                        		ZFREE(Fm->cmntpt[n]) ;
                        		ZFREE(Fm->crawpath[n]) ;
				}
                    		else {
                        		ZFREE(Fm->fmntpt[n]) ;
                        		ZFREE(Fm->frawpath[n]) ;
                      		}
                    		FM_CLRDEVICE(*mask, n) ;
			}
                	else if (mtype == FM_FLOPPY) {
                                handle_floppy_rename(devname, n, from_run_child);
				show_new_dev_window(mtype, n, mask, devname, from_run_child);
			}
		}
		else if (exists)
			show_new_dev_window(mtype, n, mask, devname, from_run_child);
	}
  	return(FM_SUCCESS) ;
}


/* Open a connection to the CE, and setup CE_Object struct fields. */

char *
connect_with_ce()
{
  errno = 0 ;

/* Establish a connection to the Classing Engine (CE). */

  if ((errno = ce_begin(NULL)) != 0)
    {
      if (errno == CE_ERR_WRONG_DATABASE_VERSION)
        return(Str[(int) M_INV_CE_VER]) ;
      else if (errno == CE_ERR_ERROR_READING_DB)
        return(Str[(int) M_NO_CE_CONNECT]) ;
    }

/* File and type namespace handles. */

  Fm->ceo->file_ns = ce_get_namespace_id("Files") ;
  if (!Fm->ceo->file_ns) return(Str[(int) M_CE_NAMESPACE]) ;

  Fm->ceo->type_ns = ce_get_namespace_id("Types") ;
  if (!Fm->ceo->type_ns) return(Str[(int) M_CE_TYPESPACE]) ;

/* Index to type_id handle. */

  Fm->ceo->file_type = ce_get_attribute_id(Fm->ceo->file_ns, "FNS_TYPE") ;
  if (!Fm->ceo->file_type) return(Str[(int) M_CE_NO_FILE]) ;

/* Type_name handle. */

  Fm->ceo->type_name = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_NAME") ;
  if (!Fm->ceo->type_name) return(Str[(int) M_CE_NO_TYPE]) ;

/* Open method handle. */

  Fm->ceo->type_open = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_OPEN") ;
  if (!Fm->ceo->type_open) return(Str[(int) M_CE_NO_OPEN]) ;

/* Icon pathname handle. */

  Fm->ceo->type_icon = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_ICON") ;
  if (!Fm->ceo->type_icon) return(Str[(int) M_CE_NO_ICON]) ;

/* Icon mask pathname handle. */

  Fm->ceo->type_icon_mask = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_ICON_MASK") ;
  if (!Fm->ceo->type_icon_mask) return(Str[(int) M_CE_NO_MASK]) ;

/* Icon foreground color handle. */

  Fm->ceo->type_fgcolor = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_FGCOLOR") ;
  if (!Fm->ceo->type_fgcolor) return(Str[(int) M_CE_NO_FG]) ;

/* Icon background color handle. */

  Fm->ceo->type_bgcolor = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_BGCOLOR") ;
  if (!Fm->ceo->type_bgcolor) return(Str[(int) M_CE_NO_BG]) ;

/* Print method handle. */

  Fm->ceo->type_print = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_PRINT") ;
  if (!Fm->ceo->type_print) return(Str[(int) M_CE_NO_PRINT]) ;

/* File template handle. */

  Fm->ceo->type_template = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_FILE_TEMPLATE") ;
  if (!Fm->ceo->type_template) return(Str[(int) M_CE_NO_TEM]) ;

/* Generic folder tns handle. */

  Fm->ceo->generic_dir = (CE_ENTRY) ce_get_entry(Fm->ceo->type_ns, 1, "default-dir") ;
  if (!Fm->ceo->generic_dir) return(Str[(int) M_CE_NO_FDR]) ;

/* Generic document tns handle. */

  Fm->ceo->generic_doc = (CE_ENTRY) ce_get_entry(Fm->ceo->type_ns, 1, "default-doc") ;
  if (!Fm->ceo->generic_doc) return(Str[(int) M_CE_NO_DOC]) ;

/* Generic application tns handle. */

  Fm->ceo->generic_app = (CE_ENTRY) ce_get_entry(Fm->ceo->type_ns, 1, "default-app") ;
  if (!Fm->ceo->generic_app) return(Str[(int) M_CE_NO_APP]) ;

/* Drop method handle. */

  Fm->ceo->type_drop = ce_get_attribute_id(Fm->ceo->type_ns, "TYPE_DROP") ;
  if (!Fm->ceo->type_drop) return(Str[(int) M_CE_NO_DROP]) ;

  return(NULL) ;
}


void
create_fm_dir(name)
char *name ;
{
  if (access(name, F_OK) == -1)
    if (mkdir(name, 0700) == -1)
      fm_msg(TRUE, Str[(int) M_NO_CREATE], name, strerror(errno)) ;
}


void
delete_selected(to_waste, wno)
int to_waste, wno ;
{
  Boolean wastree  = FALSE ;
  Boolean drawtree = FALSE ;
  Tree_Pane_Object *src_node ;
  Tree_Pane_Object *dest_node ;
  File_Pane_Object **f_p, **l_p ;     /* Files array pointers. */
  char *s, buf[MAXPATHLEN+32], src_dir[MAXPATHLEN+32] ;
  char name[MAXPATHLEN], newname[MAXPATHLEN], *ptr ;
  int error = 0 ;
  int i, reply, status ;
  WCHAR **av ;
  WCHAR **fv ;                    /* Folders being moved. */
  int argc      = 0 ;
  int fcnt      = 0 ;             /* Folder move count. */
  int redisplay = 0 ;             /* Set if one or more items deleted. */
  int sel       = 0 ;             /* Number of files selected. */
  File_Pane *f  = Fm->file[wno] ;

  if (to_waste && !is_frame(WASTE)) {
	if (fm_create_wastebasket() == FAILURE)
   	 	return;
   	set_frame_attr(WASTE, FM_FRAME_SHOW, TRUE) ;
  }

/*  Check for write permission on the source directory. If we haven't got it,
 *  then we won't be able to update the directory block.
 */

  if (access((char *) f->path, W_OK) < 0)
    {
      fm_msg(TRUE, Str[(int) M_PERM_DENIED]) ;
      return ;
    }

  fm_busy_cursor(TRUE, wno) ;
  if (to_waste) fm_busy_cursor(TRUE, WASTE) ;

/* NOTE: The first two fields of the av[] array are only used when really
 *       deleting files.
 */

  set_timer(0) ;                            /* Turn timer repaint check off. */

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p < l_p; f_p++)
    if ((*f_p)->selected) sel++ ;
  av = (WCHAR **)
    LINT_CAST(fm_malloc((unsigned) (sizeof(WCHAR **) * (sel + 3)))) ;
  fv = (WCHAR **)
    LINT_CAST(fm_malloc((unsigned) (sizeof(WCHAR **) * (sel + 1)))) ;

  av[argc++] = (WCHAR *) "/bin/rm" ;
  av[argc++] = (WCHAR *) "-rf" ;            /* Recursive, silent. */

  if (to_waste == FALSE || wno != WASTE)
    {
      l_p = PTR_LAST(wno) ;
      for (f_p = PTR_FIRST(wno); f_p < l_p; f_p++)
        {
          if ((*f_p)->selected)
            {
              redisplay = 1 ;
              STRCPY(newname, (char *) (*f_p)->name) ;

              if ((*f_p)->type == FT_DIR)
                {
                  status = confirm_dir_delete(wno, newname, to_waste) ;
                  if (status == FM_CANCEL) goto exit_delete ;
                }

              if (to_waste)
                {
                  fm_snprintf(name, sizeof(name), "%s/%s", Fm->file[WASTE]->path, (*f_p)->name) ;
                  if (access(name, F_OK) == 0)
                    {
                      status = exists_prompt(wno, name) ;
                      if (status == FM_CANCEL) goto exit_delete ;
                      if (status == FM_NO)
                        {
                          unique_filename(name) ;
                          if ((ptr = strrchr(name, '/')) != NULL)
                            STRCPY(newname, ptr+1) ;
                          else
                            STRCPY(newname, name) ;

                          save_delete_path(f->path, (WCHAR *) newname) ;
                          (*f_p)->mtime = 1 ;

                          fm_snprintf(name, sizeof(name), "%s/%s", Fm->file[WASTE]->path,
                                                 newname) ;
                          if ((*f_p)->type == FT_DIR)
                            {
                              drawtree = TRUE ;
                              fv[fcnt] = (WCHAR *) fm_malloc((unsigned)
                                                           strlen(name) + 1) ;
                              STRCPY((char *) fv[fcnt++], name) ;
                            }

                          SPRINTF(buf, "%s/%s", f->path, (*f_p)->name) ;
                          reply = do_move(buf, name) ;
                          error += reply ;

/*  If there wasn't an error in the move, then the entry is timestamped with
 *  the current time. If we moved a folder, we need to check if there are any
 *  filemgr windows currently monitoring that directory. If so, then they need
 *  to be adjusted to monitor their parent.
 */

                          if (!reply)
                            {
                              UTIMES(buf, (struct timeval *) NULL) ;
                              if (drawtree) check_del_dir(buf) ;
                            }
                          continue ;
                        }
                      if (status == FM_YES)
                        {
                          fm_snprintf(name, sizeof(name), "%s/%s",
                                  Fm->file[WASTE]->path, newname) ;
                          SPRINTF(buf, "/bin/rm -rf %s", name) ;
                          if (fm_run_str(buf, TRUE)) fm_showerr(buf) ;
                        }
                      save_delete_path(f->path, (WCHAR *) newname) ;
                    }
                  else save_delete_path(f->path, (WCHAR *) newname) ;

/* Give warning if user is trying to drop a folder on itself. */

                  if (EQUAL(name, (char *) Fm->file[WASTE]->path))
                    {
                      fm_msg(TRUE, Str[(int) M_CANT_DROP], newname) ;
                      continue ;
                    }
                }

              if (to_waste == FALSE || wno == WASTE)
                fm_snprintf(name, sizeof(name), "\"%s/%s\"", f->path, newname);
	      else
                fm_snprintf(name, sizeof(name), "%s/%s", f->path, newname);
              av[argc] = (WCHAR *) fm_malloc((unsigned) strlen(name) + 1) ;
              STRCPY((char *) av[argc++], name) ;
              (*f_p)->mtime = 1 ;

/*  If any of the files we are moving are folders, then set drawtree to true,
 *  and save that folder name. If we are displaying a tree view, and drawtree
 *  is true, then we need to update the tree node structure before
 *  redisplaying the path/tree pane.
 */
              if ((*f_p)->type == FT_DIR)
                {
                  drawtree = TRUE ;
                  fv[fcnt] = (WCHAR *) fm_malloc((unsigned) strlen(name) + 1) ;
                  STRCPY((char *) fv[fcnt++], name) ;
                }
            }
        }
    }

/* Do we have any files to transfer? */

  if (argc > 2)
    {
      if (to_waste)
        {
          for (i = 2; i < argc; i++)
            {
              if ((s = strrchr((char *) av[i], '/')) == NULL)
                s = (char *) av[i] ;
              else
                s++ ;                         /* Skip past slash. */

              SPRINTF(buf, "%s/%s", Fm->file[WASTE]->path, s) ;
              reply = do_move((char *) av[i], buf) ;
              error += reply ;

/*  If there wasn't an error in the move, then the entry is timestamped with
 *  the current time. If we moved a folder, we need to check if there are any
 *  filemgr windows currently monitoring that directory. If so, then they need
 *  to be adjusted to monitor their parent.
 */

              if (!reply)
                {
                  UTIMES(buf, (struct timeval *) NULL) ;
                  if (drawtree) check_del_dir((char *) av[i]) ;
                }
            }
        }
      else
        {
          av[argc] = NULL ;
          if (fm_run_argv((char **) av, TRUE))
            {
              error = 1 ;
              fm_showerr((char *) av[0]) ;
            }
        }
    }

  if (argc)
    {

/*  If there wasn't an error on the move, and we've moved one or more folders,
 *  and we are displaying a tree view, then the tree node structure needs to
 *  be updated, before redisplaying the path/tree pane. It's done here, rather
 *  then where drawtree is set, just in case the move failed.
 */

      if (!error && drawtree && Fm->treeview)
        {

/* If we've dragged a folder from the tree to wastebasket, then deselect. */

          if (Fm->tree.selected)
            {
              wastree = TRUE ;
              fm_treedeselect() ;
              if (Fm->file[WASTE]->path_pane.selected) fm_pathdeselect(WASTE) ;
            }

          if (to_waste)
            dest_node = path_to_node(WNO_TREE, Fm->file[WASTE]->path) ;
          for (i = 0; i < fcnt; i++)
            {
              src_node = path_to_node(WNO_TREE, fv[i]) ;
              src_node = src_node->parent ;
              if ((s = strrchr((char *) fv[i], '/')) == NULL)
                s = (char *) fv[i] ;
              else
                s++ ;                             /* Skip past last slash. */
              remove_tree_entry(src_node, (WCHAR *) s) ;
              if (to_waste) add_tree_entry(dest_node, (WCHAR *) s) ;
            }
          fm_drawtree(TRUE) ;
        }

      if (to_waste)  fm_display_folder((BYTE) FM_BUILD_FOLDER, WASTE) ;
      if (redisplay) fm_display_folder((BYTE) FM_BUILD_FOLDER, wno) ;

/*  If the transfer came from the path/tree pane, and there is a folder pane
 *  monitoring the parent of the directory that has been moved, then redisplay
 *  that folder.
 */

      if (wastree == TRUE)
        if (EQUAL((char *) f->path, src_dir))
          fm_display_folder((BYTE) FM_BUILD_FOLDER, wno) ;

      if (to_waste)
        {
          set_waste_icon() ;
          if (f->mtype == FM_FLOPPY || f->mtype == FM_DOS)
            f->have_deleted = TRUE ;
        }
      av[argc] = NULL ;
      fm_free_argv((char **) &av[2]) ;
      fv[fcnt] = NULL ;
      fm_free_argv((char **) fv) ;
      FREE((char *) av) ;
      FREE((char *) fv) ;
    }

exit_delete:
  set_timer(Fm->interval) ;        /* Turn timer repaint check back on. */
  fm_busy_cursor(FALSE, wno) ;
  if (to_waste) fm_busy_cursor(FALSE, WASTE) ;
}


/* The following three routines (do_move, do_fastcopy and do_copy) are
 * based on the BSD 4.4 mv program, which has the following copyright.
 *
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ken Smith of The State University of New York at Buffalo.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

int
do_move(from, to)
char *from, *to ;
{
  char buf[MAXPATHLEN+100] ;    /* For constructing error messages. */
  char tmp_buf[MAXPATHLEN+100] ;
  struct stat sbuf ;

  if (!rename(from, to)) return(0) ;
  if (errno != EXDEV)
    {
      int save_wno;
      SPRINTF(tmp_buf, Str[(int) M_LRENAME], from, to);
      fm_snprintf(buf, sizeof(buf), "mv: %s: %s",
	      tmp_buf, strerror(errno)) ;
      /* permission denied mesasge displayed in the source folder instead
	 of destination (curr) folder. */
      save_wno = Fm->curr_wno;
      Fm->curr_wno = Fm->move_wno;
      fm_msg(TRUE, buf) ;
      Fm->curr_wno = save_wno;
      return(1) ;
    }

/*  If rename fails, and it's a regular file, do the copy internally;
 *  otherwise, use cp and rm.
 */

  if (fm_stat(from, &sbuf))
    {
      SPRINTF(buf, "/bin/mv %s %s", from, to);
      if (fm_run_str(buf, TRUE))
	{
	  fm_showerr(buf);
          return(1) ;
	}
      else
          return(0) ;
    }
  return(S_ISREG(sbuf.st_mode) ? do_fastcopy(from, to, &sbuf)
                               : do_copy(from, to)) ;
}


int
do_fastcopy(from, to, sbp)     /* Copy file (in same partition). */
char *from, *to ;
struct stat *sbp ;
{
  char buf[MAXPATHLEN+100] ;   /* For constructing error messages. */
  struct timeval tval[2] ;
  static u_int blen ;
  static char *bp ;
  register int nread, from_fd, to_fd ;

  if ((from_fd = open(from, O_RDONLY, 0)) < 0)
    {
      fm_snprintf(buf, sizeof(buf), "mv: %s: %s", from, strerror(errno)) ;
      fm_msg(TRUE, buf) ;
      return(1) ;
    }
  if ((to_fd = open(to, O_WRONLY | O_CREAT | O_TRUNC, sbp->st_mode)) < 0)
    {
      fm_snprintf(buf, sizeof(buf), "mv: %s: %s", to, strerror(errno)) ;
      fm_msg(TRUE, buf) ;
      CLOSE(from_fd) ;
      return(1) ;
    }
  if (!blen && !(bp = malloc(blen = sbp->st_blksize)))
    {
      fm_snprintf(buf, sizeof(buf), "mv: %s: %s", from, Str[(int) M_OUT_MEM]) ;
      fm_msg(TRUE, buf) ;
      return(1) ;
    }
  while ((nread = read(from_fd, bp, blen)) > 0)
    if (write(to_fd, bp, nread) != nread)
      {
        SPRINTF("mv: %s: %s", to, strerror(errno)) ;
        fm_msg(TRUE, buf) ;
        goto err ;
      }

  if (nread < 0)
    {
      fm_snprintf(buf, sizeof(buf), "mv: %s: %s", from, strerror(errno)) ;
      fm_msg(TRUE, buf) ;

err:  UNLINK(to) ;
      CLOSE(from_fd) ;
      CLOSE(to_fd) ;
      return(1) ;
    }
  FCHOWN(to_fd, sbp->st_uid, sbp->st_gid) ;
  FCHMOD(to_fd, sbp->st_mode) ;

  CLOSE(from_fd) ;
  CLOSE(to_fd) ;

  tval[0].tv_sec  = sbp->st_atime ;
  tval[1].tv_sec  = sbp->st_mtime ;
  tval[0].tv_usec = tval[1].tv_usec = 0 ;
  UTIMES(to, tval) ;
  /* if fail to unlink the file then it needs to unlink the file in dest dir */
  if (unlink(from)) {
     int tmp_wno = Fm->curr_wno;
     UNLINK(to);
     fm_snprintf(buf, sizeof(buf), "mv: %s: %s", from, strerror(errno)) ;
     Fm->curr_wno = Fm->move_wno;
     fm_msg(TRUE, buf) ;  /* print error message in source folder */
     Fm->curr_wno = tmp_wno;
     return(1) ;
  }
  return(0) ;
}


int
do_copy(from, to)         /* Copy file recursively (across partitions). */
char *from, *to ;
{
  char buf[MAXPATHLEN+100] ;   /* For constructing error messages. */
  int error ;

/* Check for the possibility of a recursive copy; which should be aborted. */

  if (is_recursive(Fm->curr_wno, from, to)) return(FALSE) ;

  SPRINTF(buf, "cp -pr \"%s\" \"%s\"", from, to) ;
  if ((error = fm_run_str(buf, TRUE)) != 0)
    {
      fm_showerr(buf) ;
      return(error) ;
    }

  SPRINTF(buf, "rm -rf \"%s\"", from) ;
  if ((error = fm_run_str(buf, TRUE)) != 0) fm_showerr(buf) ;
  return(error) ;
}


void
do_find_button()
{
  int len, i, ignore_case, pid, after_days, before_days, today ;
  WCHAR *from, *name, *owner, *after, *before, *pattern;
  WCHAR *av[30], buf[10], buf1[10], dir[MAXPATHLEN], sname[MAXLINE] ;
  struct rlimit rlp ;

  ignore_case = (get_item_int_attr(FM_FIND_CASETOGGLE, FM_ITEM_IVALUE) == 0) ;

  Fm->fout_eol = Fm->ferr_eol = FALSE ;
  Fm->fout_ptr = Fm->fout_fname ;
  Fm->ferr_ptr = Fm->ferr_fname ;

/* Get and check name, owner, modify time, and pattern parameters. */

  from    = (WCHAR *) get_item_str_attr(FM_FIND_FROMITEM,    FM_ITEM_IVALUE) ;
  name    = (WCHAR *) get_item_str_attr(FM_FIND_NAMEITEM,    FM_ITEM_IVALUE) ;
  owner   = (WCHAR *) get_item_str_attr(FM_FIND_OWNERITEM,   FM_ITEM_IVALUE) ;
  after   = (WCHAR *) get_item_str_attr(FM_FIND_AFTERITEM,   FM_ITEM_IVALUE) ;
  before  = (WCHAR *) get_item_str_attr(FM_FIND_BEFOREITEM,  FM_ITEM_IVALUE) ;
  pattern = (WCHAR *) get_item_str_attr(FM_FIND_PATTERNITEM, FM_ITEM_IVALUE) ;
/* "don't get pattern, if not active", doesn't work as this wchar* is used
 later also */

 if(get_item_int_attr(FM_FIND_PATTERNITEM, FM_ITEM_INACTIVE))
        strcpy((char *)pattern, "");

  if (*from == 0)
    {
      error(FALSE, Str[(int) M_SEARCH], (char *) Fm->file[Fm->curr_wno]->path) ;
      from = Fm->file[Fm->curr_wno]->path ;
    }

  ds_expand_pathname(from, dir) ;

  if (*name  == 0 && *owner  == 0 &&
      *after == 0 && *before == 0 && *pattern == 0)
    {
      *name = '*' ;                 /* Supply default parameter. */
      *(name+1) = 0 ;
    }

  if (*after && (after_days = calc_days((char *) after)) == -1)
    {
      error(TRUE, Str[(int) M_DATE], Str[(int) M_AFTER]) ;
      return ;
    }
  if (*before && (before_days = calc_days((char *) before)) == -1)
    {
      error(TRUE, Str[(int) M_DATE], Str[(int) M_BEFORE]) ;
      return ;
    }
  today = today_in_days() ;

  if (*owner && getunum((char *) owner) == -1)
    {
      error(TRUE, Str[(int) M_OWNER], (char *) owner) ;
      return ;
    }

  clear_list(FM_FIND_FIND_LIST) ;

  Fm->Nfound = 0 ;
  ZFREE(Fm->Found_chosen) ;

/* Fork off the find... */

  if (pipe(Fm->pstdout) < 0 || pipe(Fm->pstderr) < 0)
    {
      error(TRUE, Str[(int) M_PTY], (char *) 0) ;
      return ;
    }

  if ((pid = vfork()) < 0)
    {
      error(TRUE, strerror(errno), (char *) 0) ;
      return ;
    }
  if (pid == 0)           /* Child. */
    {
      DUP2(Fm->pstdout[1], 1) ;
      DUP2(Fm->pstderr[1], 2) ;
      getrlimit(RLIMIT_NOFILE, &rlp) ;

      for (i = rlp.rlim_cur - 1; i >= 3; i--) CLOSE(i) ;

      i = 0 ;
      av[i++] = (WCHAR *) "find" ;
      av[i++] = dir ;

      if (name[0])
        {
	  while (name != NULL && *name == ' ') name++;
          for (len = fm_mbstrlen((char*)name)-1; name[len] == ' '; name[len--] = 0);
          SPRINTF((char *) sname, "*%s*", name) ;
          if (get_item_int_attr(FM_FIND_NAMETOGGLE, FM_ITEM_IVALUE))
            av[i++] = (WCHAR *) "!" ;
          av[i++] = (WCHAR *) "-name" ;
          av[i++] = sname ;
        }
      if (owner[0])
        {
          if (get_item_int_attr(FM_FIND_OWNERTOGGLE, FM_ITEM_IVALUE))
            av[i++] = (WCHAR *) "!" ;
          av[i++] = (WCHAR *) "-user" ;
          av[i++] = owner ;
        }
      if (after[0])
        {
          av[i++] = (WCHAR *) "-mtime" ;
          SPRINTF((char *) buf, "-%d", today-after_days) ;
          av[i++] = buf ;
        }
      if (before[0])
        {
          av[i++] = (WCHAR *) "!" ;
          av[i++] = (WCHAR *) "-mtime" ;
          SPRINTF((char *) buf1, "-%d", today - before_days + 1) ;
          av[i++] = buf1 ;
        }
      if (pattern[0])
        {
          av[i++] = (WCHAR *) "-exec" ;
          av[i++] = (WCHAR *) "egrep" ;
          av[i++] = (WCHAR *) "-l" ;
          if (ignore_case) av[i++] = (WCHAR *) "-i" ;
          av[i++] = pattern ;
          av[i++] = (WCHAR *) "{}" ;
          av[i++] = (WCHAR *) ";" ;
        }
      else
        av[i++] = (WCHAR *) "-print" ;
      av[i++] = (WCHAR *) "-follow"  ;
      av[i++] = NULL ;

      EXECVE(FINDPROG, (char **) av, (char **) 0) ;
      FPRINTF(stderr, "%s", Str[(int) M_EXEC_FAIL]);
      exit(1) ;
    }

  if ((Fm->fpout = fdopen(Fm->pstdout[0], "r")) == NULL ||
      (Fm->fperr = fdopen(Fm->pstderr[0], "r")) == NULL)
    error(TRUE, Str[(int) M_READSTR], (char *) 0) ;
  else
    {
      Fm->Pid = pid ;
      find_do_connections() ;
      error(FALSE, Str[(int) M_SEARCHING], (char *) 0) ;
      set_item_int_attr(FM_FIND_FIND_BUTTON, FM_ITEM_INACTIVE, TRUE) ;
      set_item_int_attr(FM_FIND_STOP_BUTTON, FM_ITEM_INACTIVE, FALSE) ;
      set_item_int_attr(FM_FIND_OPEN_BUTTON, FM_ITEM_INACTIVE, TRUE) ;
    }
}


void
do_find_stderr(fd)
int fd ;
{
  char buf[512], *sptr ;
  int bytes ;              /* Total number of bytes to read. */
  int n ;                  /* Count of number of bytes read. */

  if (ioctl(fd, FIONREAD, &bytes) == 0)
    while (bytes > 0)
      {
        if ((n = read(fd, buf, sizeof(buf))) > 0)
          {
            sptr = buf ;
            while (n > 0)
              {
                while (*sptr != '\0' && *sptr != '\n')
                  {
                    *Fm->ferr_ptr++ = *sptr++ ;
                    bytes-- ;
                    n-- ;
                  }
                if (*sptr == '\n') Fm->ferr_eol = TRUE ;
                *Fm->ferr_ptr = '\0' ;

                if (Fm->ferr_eol == TRUE)
                  {
                    error(FALSE, Fm->ferr_fname, (char *) 0) ;
                    sptr++ ;
                    bytes-- ;
                    n-- ;
                    Fm->ferr_ptr = Fm->ferr_fname ;
                    Fm->ferr_eol = FALSE ;
                  }
              }
          }
      }
}


void
do_find_stdout(fd)
int fd ;
{
  char buf[512], *sptr ;
  int bytes ;              /* Total number of bytes to read. */
  int n ;                  /* Count of number of bytes read. */
  struct stat fstatus ;
  int row_num = 0, index = 0, type_val = get_item_int_attr(FM_FIND_TYPEITEM, FM_ITEM_IVALUE) ;
  Panel_list_row_values find_values_arr[MAXFIND];

  if (ioctl(fd, FIONREAD, &bytes) == 0) {
    row_num = Fm->Nfound;
    while (bytes > 0) {
        if ((n = read(fd, buf, sizeof(buf))) > 0)
          {
            sptr = buf ;
            while (n > 0)
              {
                while (*sptr != '\0' && *sptr != '\n')
                  {
                    *Fm->fout_ptr++ = *sptr++ ;
                    bytes-- ;
                    n-- ;
                  }
                if (*sptr == '\n') Fm->fout_eol = TRUE ;
                *Fm->fout_ptr = '\0' ;

                if (Fm->fout_eol == TRUE)
                  {
                    if (fm_stat(Fm->fout_fname, &fstatus) == -1)
                      Fm->Type = FT_DOC ;
                    else
                      {
                        if ((fstatus.st_mode & S_IFMT) == S_IFDIR)
                          Fm->Type = FT_DIR ;
                        else if ((fstatus.st_mode & S_IFMT) == S_IFREG &&
                                 (fstatus.st_mode & S_IEXEC))
                          Fm->Type = FT_APP ;
                        else
                          Fm->Type = FT_DOC ;
                      }

/* Check file type. */

                    if (type_val & (1 << Fm->Type))
                      {
			/* Dont overflow array */
			if (index == MAXFIND) {
				XV_SET(X->items[(int) FM_FIND_FIND_LIST],
       		  			PANEL_LIST_ROW_VALUES, row_num, find_values_arr, index, NULL);
				for(row_num=0; row_num < index; row_num++)
					free(find_values_arr[row_num].string);
				index = 0;
				row_num = Fm->Nfound;
			}
			find_values_arr[index].string = strdup(Fm->fout_fname);
  			find_values_arr[index].glyph = fm_server_image((SBYTE)Fm->Type);
  			find_values_arr[index].mask_glyph = 0;
  			find_values_arr[index].font = 0;
  			find_values_arr[index].client_data = 0;
  			find_values_arr[index].extension_data = 0;
  			find_values_arr[index].inactive = 0;
  			find_values_arr[index].selected = 0;
                        Fm->Nfound++ ;
			index++;
                      }
                    sptr++ ;
                    bytes-- ;
                    n-- ;
                    Fm->fout_ptr = Fm->fout_fname ;
                    Fm->fout_eol = FALSE ;
                  }
              }
          }
      }
      if (index > 0) {
		XV_SET(X->items[(int) FM_FIND_FIND_LIST],
       			PANEL_LIST_ROW_VALUES, row_num, find_values_arr, index, NULL);
		for(row_num=0; row_num < index; row_num++)
			free(find_values_arr[row_num].string);
      }
   }
}


/*VARARGS*/
#ifdef __STDC__
void dout(char *fmt, ...)             /* For outputting debugging messages. */
#else
dout(fmt, va_alist)              /* Debug statements for dnd. */
char *fmt ;
va_dcl
#endif /* __STDC__ */
{
  va_list args ;
  char buffer[MAXPATHLEN+80] ;

#ifdef __STDC__
  va_start(args, fmt) ;
#else
  va_start(args) ;
#endif /*__STDC__*/

  VSPRINTF(buffer, fmt, args) ;
  if (*buffer) PRINTF("%s", buffer) ;
  va_end(args) ;
}


/*VARARGS*/
#ifdef __STDC__
void err_exit(char *file, int line, char *fmt, ...)
#else
err_exit(file, line, fmt, va_alist)     /* Send a msg to stderr and exit. */
char *file ;
int line ;
char *fmt ;
va_dcl
#endif /*__STDC__*/
{
  va_list args ;
  char buffer[MAXPATHLEN+80] ;

#ifdef __STDC__
  va_start(args, fmt) ;
#else
  va_start(args) ;
#endif /*__STDC__*/

  VSPRINTF(buffer, fmt, args) ;

  if (*buffer)
    FPRINTF(stderr, Str[(int) M_ERR_EXIT], file, line, buffer) ;

  va_end(args) ;
  exit(1) ;
}


void
fast_close(ptr)
mmap_handle_t *ptr ;
{
  (void) munmap(ptr->addr, ptr->len) ;
  FREE(ptr) ;
}


mmap_handle_t *
fast_read(pathname)
char *pathname ;            /* Full pathname of the file to read in. */
{
  struct stat buff ;
  mmap_handle_t *ptr ;
  int fd ;

  if ((ptr = (mmap_handle_t *) malloc(sizeof(*ptr))) == NULL) return(NULL) ;
  if ((fd = open(pathname, O_RDONLY)) < 0)
    {
      FREE(ptr) ;
      return(NULL) ;
    }

  if (fstat(fd, & buff) < 0)
    {
      CLOSE(fd) ;
      FREE(ptr) ;
      return(NULL) ;
    }

  if ((ptr->addr = mmap(NULL, ptr->len = (int) buff.st_size, PROT_READ,
		        MAP_PRIVATE, fd, 0)) == (caddr_t) -1)
    {
      CLOSE(fd) ;
      FREE(ptr) ;
      return(NULL) ;
    }
  CLOSE(fd) ;
  return(ptr) ;
}


int
fast_write(pathname, mem_ptr, size)
char *pathname ;        /* Full pathname of where to write the file out. */
char *mem_ptr ;         /* Pointer in memory to the data to be written. */
int size ;              /* Number of bytes to write. */
{
  int fd, ret ;

  if ((fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) return(-1) ;
  ret = write(fd, mem_ptr, size) ;

  if (ret == -1)
    {
      int s_errno = errno ;         /* Save errno across close call. */

      CLOSE(fd) ;
      errno = s_errno ;
      return(-1) ;
    }

  if (close(fd) == -1) return(-1) ; /* Means we couldn't really write the data */
  if (ret != size)
    {
      UNLINK(pathname) ;
      return(-1) ;
    }
  return(0) ;
}


int
fio_apply_change(fullname)
char *fullname ;                /* Full pathname. */
{
  struct stat sbuf ;
  Boolean inactive ;
  char *ptr, *sp ;
  int i, error = FALSE ;
  int do_rename = TRUE ;
  int status ;

/* Change the actual file's name. */

  inactive = (Boolean) get_item_int_attr(FM_FIO_FILE_NAME, FM_ITEM_INACTIVE) ;
  if (!inactive)
    {

/* Make sure we have full pathname */

      if (fullname[0] != '/')
        {
          FPRINTF(stderr, Str[(int) M_NEED_FPATH], fullname) ;
          error = TRUE ;
        }
      else
        {
          char *basename, newname[MAXPATHLEN] ;

          basename = strrchr(fullname, '/') + 1 ;

          sp = get_item_str_attr(FM_FIO_FILE_NAME, FM_ITEM_IVALUE) ;
          if (sp && !EQUAL(sp, basename))
            {
              if (*sp == '\0')
                {
                  fm_msg(TRUE, Str[(int) M_NO_INFONAME]) ;
                  error = TRUE ;
                }
              else if (strchr(sp, '/') != NULL)
                {
                  fm_msg(TRUE, Str[(int) M_NOINFO_RENAME], basename, sp) ;
                  error = TRUE ;
                }
              else
                {
                  STRNCPY(newname, fullname, basename-fullname) ;
                  newname[basename-fullname] = '\0' ;
                  STRCAT(newname, sp) ;

                  if (access(newname, F_OK) == 0)
                    {
                      status = exists_prompt(Fm->curr_wno, newname) ;
                      if (status == FM_CANCEL)
                        {
                          do_rename = FALSE ;
                          set_item_str_attr(FM_FIO_FILE_NAME, FM_ITEM_IVALUE,
                                            basename) ;
                        }
                      if (status == FM_NO)
                        {
                          unique_filename(newname) ;
                          if ((ptr = strrchr(newname, '/')) != NULL) ptr++ ;
                          else ptr = newname ;
                          set_item_str_attr(FM_FIO_FILE_NAME, FM_ITEM_IVALUE,
                                            ptr) ;
                        }
                    }

                  if (do_rename && rename(fullname, newname) != 0)
                    {
                      fm_msg(TRUE, strerror(errno)) ;
                      error = TRUE ;
                    }
                }
            }
        }
    }

/*  Note: we are comparing the current value of the text item with the 1st
 *  file's value, to see if they have changed.  If so, then we apply the
 *  changed value to the current selected file we are looking at.  It is
 *  possible to confuse this simple test, but this should work in most cases
 *  and saves a lot of system calls to check the currect file's values.
 */

/* Change the actual file's owner. */

  inactive = (Boolean) get_item_int_attr(FM_FIO_OWNER, FM_ITEM_INACTIVE) ;
  if (!inactive)
    {
      sp = get_item_str_attr(FM_FIO_OWNER, FM_ITEM_IVALUE) ;
      if (sp && !EQUAL(sp, Fm->f_owner))
        {
          if ((i = fm_getuid(sp)) == -1)
            fm_msg(TRUE, Str[(int) M_UNKNOWNOWNER]) ;
          else if (chown(fullname, i, -1) == -1)
            {
              fm_msg(TRUE, strerror(errno)) ;
              error = TRUE ;
            }
        }
    }

/* Change the actual file's group. */

  inactive = (Boolean) get_item_int_attr(FM_FIO_GROUP, FM_ITEM_INACTIVE) ;
  if (!inactive)
    {
      sp = get_item_str_attr(FM_FIO_GROUP, FM_ITEM_IVALUE) ;
      if (sp && !EQUAL(sp, Fm->f_group))
        {
          if ((i = fm_getgid(sp)) == -1)
            fm_msg(TRUE, Str[(int) M_UNKNOWNGROUP]) ;
          else if (chown(fullname, -1, i) == -1)
            {
              fm_msg(TRUE, strerror(errno)) ;
              error = TRUE ;
            }
        }
    }

/* Change the actual file's permissions. */

  if (!inactive)
    inactive = (Boolean)
               get_item_int_attr(FM_FIO_OWNER_PERM, FM_ITEM_INACTIVE) ;
  if (!inactive)
    inactive = (Boolean)
               get_item_int_attr(FM_FIO_GROUP_PERM, FM_ITEM_INACTIVE) ;
  if (!inactive)
    inactive = (Boolean)
               get_item_int_attr(FM_FIO_WORLD_PERM, FM_ITEM_INACTIVE) ;

  if (!inactive)
    {
      i = fio_toggle_to_int() ;

      if ((Fm->f_status.st_mode & ~S_IFMT) != i)
        if (fm_stat(fullname, &sbuf) == 0)
          {
            if (sbuf.st_mode & S_ISUID) i += S_ISUID ;
            if (sbuf.st_mode & S_ISGID) i += S_ISGID ;
            if (chmod(fullname, i) == -1)
              {
                fm_msg(TRUE, strerror(errno)) ;
                error = TRUE ;
                if (fm_stat(fullname, &sbuf) == 0)
                  set_toggles(sbuf, TRUE) ;    /* Reset permission toggles. */
              }
	    else Fm->f_status.st_mode = i;
          }
    }
  return(error) ;
}


char *
fio_get_group(gid)        /* Return group name from group id */
int gid ;
{
  register struct group *gr ;
  struct utmp utmp ;
  static int last_gid = FM_EMPTY ;      /* Last gid rec'd. */
  static char last_name[NMAX+1] ;       /* Last computed name. */

  if (gid >= 0 && gid == last_gid) return(last_name) ;
  if (gid < 0)                     return(NULL) ;

  gr = getgrgid(gid) ;
  if (gr == NULL) return(NULL) ;

  last_gid = gr->gr_gid ;
  STRNCPY((char *) last_name, gr->gr_name, NMAX) ;
  return((char *) last_name) ;
}


char *
fio_get_name(uid)        /* Return user name from user id */
int uid ;
{
  register struct passwd *pw ;
  struct utmp utmp ;
  static int last_uid = FM_EMPTY ;    /* Last uid rec'd. */
  static char last_name[NMAX+1] ;     /* Last computed name. */

  if (uid == last_uid) return(last_name) ;

  if (uid < MINUID) return(NULL) ;

  pw = getpwuid(uid) ;
  if (pw == NULL) return(NULL) ;

  last_uid = uid ;
  STRNCPY((char *) last_name, pw->pw_name, NMAX) ;
  return((char *) last_name) ;
}


/* Duplicate the file's mount point info. */

#ifdef SVR4
struct mnttab *
fio_mount_copy(mnt)
register struct mnttab *mnt ;
{
  register struct mnttab *new ;

  new = (struct mnttab *) LINT_CAST(fm_malloc((unsigned) (sizeof(*new)))) ;

  new->mnt_special = fm_malloc((unsigned) strlen(mnt->mnt_special) + 1) ;
  STRCPY(new->mnt_special, mnt->mnt_special) ;

  new->mnt_mountp = fm_malloc((unsigned) strlen(mnt->mnt_mountp) + 1) ;
  STRCPY(new->mnt_mountp, mnt->mnt_mountp) ;

  new->mnt_fstype = fm_malloc((unsigned) strlen(mnt->mnt_fstype) + 1) ;
  STRCPY(new->mnt_fstype, mnt->mnt_fstype) ;

  new->mnt_mntopts = fm_malloc((unsigned) strlen(mnt->mnt_mntopts) + 1) ;
  STRCPY(new->mnt_mntopts, mnt->mnt_mntopts) ;

  new->mnt_time = fm_malloc((unsigned)  strlen(mnt->mnt_time) + 1) ;
  STRCPY(new->mnt_time, mnt->mnt_time) ;

  return(new) ;
}
#else
struct mntent *
fio_mount_copy(mnt)
register struct mntent *mnt ;
{
  register struct mntent *new ;

  new = (struct mntent *) LINT_CAST(fm_malloc((unsigned) (sizeof(*new)))) ;

  new->mnt_fsname = fm_malloc((unsigned) strlen(mnt->mnt_fsname) + 1) ;
  STRCPY(new->mnt_fsname, mnt->mnt_fsname) ;

  new->mnt_dir = fm_malloc((unsigned) strlen(mnt->mnt_dir) + 1) ;
  STRCPY(new->mnt_dir, mnt->mnt_dir) ;

  new->mnt_type = fm_malloc((unsigned) strlen(mnt->mnt_type) + 1) ;
  STRCPY(new->mnt_type, mnt->mnt_type) ;

  new->mnt_opts = fm_malloc((unsigned) strlen(mnt->mnt_opts) + 1) ;
  STRCPY(new->mnt_opts, mnt->mnt_opts) ;

  new->mnt_freq = mnt->mnt_freq ;
  new->mnt_passno = mnt->mnt_passno ;

  return(new) ;
}
#endif /*SVR4*/


#ifdef SVR4
void
fio_mount_free(mnt)
struct mnttab *mnt ;
{
  FREE(mnt->mnt_special) ;
  FREE(mnt->mnt_mountp) ;
  FREE(mnt->mnt_fstype) ;
  FREE(mnt->mnt_mntopts) ;
  FREE(mnt->mnt_time) ;
  FREE((char *) mnt) ;
}
#else
void
fio_mount_free(mnt)
struct mntent *mnt ;
{
  FREE(mnt->mnt_fsname) ;
  FREE(mnt->mnt_dir) ;
  FREE(mnt->mnt_type) ;
  FREE(mnt->mnt_opts) ;
  FREE((char *) mnt) ;
}
#endif /*SVR4*/


/* Return the file's mount point info. */

#ifdef SVR4
struct mnttab *
#else
struct mntent *
#endif /*SVR4*/
fio_mount_point(file)
char *file ;
{
#ifdef SVR4
  struct mnttab mnt, *mntsave ;
#else
  struct mntent *mnt, *mntsave ;
#endif /*SVR4*/
  FILE *mntp ;
  mntsave = NULL ;

/* Get the filesystem mounted at the lowest point in the path & malloc it. */

#ifdef SVR4

  if ((mntp = fopen(MNTTAB, "r")) == 0) return(NULL) ;

  while ((fm_getmntent(mntp, &mnt)) == 0)
    {
      if (fio_prefix(mnt.mnt_mountp, file))
        {
          if (!mntsave || (int) strlen(mntsave->mnt_mountp) <=
                          (int) strlen(mnt.mnt_mountp))
            {
              if (mntsave) fio_mount_free(mntsave) ;
              mntsave = fio_mount_copy(&mnt) ;
            }
        }
    }
  FCLOSE(mntp) ;
#else

  if ((mntp = setmntent(MOUNTED, "r")) == 0) return(NULL) ;

  while ((mnt = fm_getmntent(mntp)) != NULL)
    {
      if (strcmp(mnt->mnt_type, MNTTYPE_IGNORE) == 0 ||
          strcmp(mnt->mnt_type, MNTTYPE_SWAP) == 0)
        continue ;

      if (fio_prefix(mnt->mnt_dir, file))
        {
          if (!mntsave || strlen(mntsave->mnt_dir) <= strlen(mnt->mnt_dir))
            {
              if (mntsave) fio_mount_free(mntsave) ;
              mntsave = fio_mount_copy(mnt) ;
            }
        }
    }
  ENDMNTENT(mntp) ;
#endif
return(mntsave ? mntsave : NULL) ;
}


char *
fm_getcwd(buf, size)
char *buf ;
int size ;
{
  char *newdir ;

  if (Fm->follow_links || buf == NULL) newdir = getcwd(buf, size) ;
  else                                 newdir = buf ;
  return(newdir) ;
}


int
fm_getgid(name)        /* Get group id. */
char *name ;
{
  struct group *gr ;

  if ((gr = getgrnam(name)) == NULL) return(-1) ;
  return(gr->gr_gid) ;
}


int
fm_getuid(name)        /* Get owner id. */
char *name ;
{
  struct passwd *pw ;

  if ((pw = getpwnam(name)) == NULL) return(-1) ;
  return(pw->pw_uid) ;
}


/* Identify scripts without a dependency on Solaris libelf. */
int
fm_get_app_type(char *fname)
{
    unsigned char signature[2];
    FILE *fp = fopen(fname, "rb");
    int type = FM_BIN_APP;
    if (fp == NULL) return type;
    if (fread(signature, 1, sizeof(signature), fp) == sizeof(signature) &&
        signature[0] == '#' && signature[1] == '!') type = FM_SHELL_APP;
    fclose(fp);
    return type;
}


void
rn_no_template(dir, name, buf, ext)
char *dir, *name, *buf, *ext ;
{
  Boolean hascopy = FALSE ;  /* Set if filename has "copy" number. */
  char *ptr, *sptr = NULL, tmp[MAXPATHLEN] ;
  int version = 0 ;

  /* See if the filename already has a "copy" number. */

  if ((sptr = ptr = strrchr(name, '.')) != NULL)
    {
      while (*++ptr)
        {
          if (*ptr < '0' || *ptr > '9')
            {
              hascopy = FALSE ;
              version = 0 ;
              break ;
            }
          else
	    {
	      hascopy = TRUE;
	      version = (version*10) + *ptr - '0' ;
	    }
        }
    }

/* Increment "copy" number (if any), and continue until a unique name. */

  do
    {
      version++ ;
      if (hascopy == TRUE)
        {
          STRCPY(tmp, name) ;
          tmp[sptr-name]= '\0' ;

	  if (dir) SPRINTF(buf, "%s/%s.%d", dir, tmp, version) ;
	  else     SPRINTF(buf, "%s.%d", tmp, version) ;
	  if (ext)
	     strcat(buf, ext);
        }
      else
	{
	  if (dir) SPRINTF(buf, "%s/%s.%d", dir, name, version) ;
	  else     SPRINTF(buf, "%s.%d", name, version) ;
	  if (ext)
	     strcat(buf, ext);
	}
    }
  while (access(buf, F_OK) == 0) ;
}


void
rn_hidden_case(dir, name, buf)
char *dir, *name, *buf;
{
   char *ptr, *sptr = NULL, tmp[MAXPATHLEN] ;
   int version = 0;
   Boolean hascopy = FALSE;

   sptr = ptr = name;

   while (*++ptr)
     {
       if (*ptr < '0' || *ptr > '9')
         {
	   if (*ptr != '.')
	     {
	       hascopy = FALSE;
               version = 0 ;
	     }
           break ;
         }
       else
	 {
	   hascopy = TRUE;
	   version = (version*10) + *ptr - '0' ;
	 }
     }
   /* Increment "copy" number (if any), and continue until a unique name.  */
   do
     {
       version++ ;
       if (!hascopy)
	  ptr = sptr;
       if (dir) SPRINTF(buf, "%s/.%d%s", dir, version, ptr) ;
       else     SPRINTF(buf, ".%d%s", version, ptr) ;
     }
   while (access(buf, F_OK) == 0) ;
}


void
rn_other_cases(dir, name, template, buf)
char *dir, *name, *template, *buf;
{
  char *ptr;
  char tmp[MAXPATHLEN];
  int len;

  if (((ptr = strchr(name, '.')) == NULL) ||
      (!(*++ptr < '0' || *ptr > '9') && (strchr(ptr, '.') == NULL)))
    {  /* eg. README, README.1, the naming is the same as no template */
       rn_no_template(dir, name, buf, NULL);
       return;
    }


  /*  meeting.ps. will rename to meeting.ps.1. */
  len = fm_mbstrlen(name);
  if (name[len - 1] == '.')
     {
       STRCPY(tmp, name);
       tmp[len-1] = '\0';
       rn_no_template(dir, tmp, buf, ".");
       return;
     }

  /* meeting.ps  will rename to meeting.1.ps */
  if ((ptr = strrchr(name, '.')) != NULL)
    {
      STRCPY(tmp, name);
      tmp[ptr-name] = '\0';
      rn_no_template(dir, tmp, buf, ptr);
      return;
    }
}


/* The rules to name a duplicate file are:

   1. If no file template is found in CE then append ".n" to the filename.
      eg. meeting --> meeting.1, meeting.2, ...

   2. If file template is found in CE and dot(s) isn't the 1st char (ie. not
      hidden file) then add ".n" infront of the last dot (.) extension.
      eg. meeting.ps    --> meeting.1.ps, meeting.2.ps, ...
          my.meeting.ps --> my.meeting.1.ps, my.meeting.1.ps, ...
          meeting.ps.   --> meeting.ps.1., meeting.ps.2., ...

   3. If file template is found in CE and it starts with dot (ie. hidden file)
      then prepend ".n" to the filename.
      eg. .meeting.ps  --> .1.meeting.ps, .2.meeting.ps, ...
          ..meeting.ps --> .1..meeting.ps, .2..meeting.ps, ...

   4. If file template is found in CE but without dot/extension then
      append ".n" to the filename.
      eg. README --> README.1, README.2, ...
*/
void
increment_copy_number(dir, name, template, buf)
char *dir, *name, *template, *buf ;
{
  if (!template)
     rn_no_template(dir, name, buf, NULL);
  else
    {
      if (*name == '.')
	rn_hidden_case(dir, name, buf);/* name starts with dot */
      else
	rn_other_cases(dir, name, template, buf);
    }
}


char *
fm_malloc(size)        /* Malloc check. */
unsigned int size ;
{
  char *m_p = calloc(1, size) ;

/* XXX: See if we can grab memory from someplace else if we fail. */

  if (m_p == NULL) fm_msg(TRUE, Str[(int) M_NOMEMORY]) ;

  return(m_p) ;
}


/*VARARGS*/
#ifdef __STDC__
void
fm_msg(int bell, char * fmt, ...)
#else
void
fm_msg(bell, fmt, va_alist)     /* Display message on footer. */
int bell ;
char *fmt ;
va_dcl
#endif /*__STDC__*/
{
  va_list args ;
  char buf[1000], output[MSGBUFSIZE], c, *ip, *op, *p ;
  register char *bp ;
  register unsigned long val ;

#ifdef __STDC__
  va_start(args, fmt);
#else
  va_start(args) ;
#endif /*__STDC__*/

  op = output ;
  for (ip = fmt; *ip; ip++)
    {
      if (*ip != '%')
        {
          EMITCHAR(*ip) ;
          continue ;
        }
      switch (*++ip)
        {
          case 'c' : c = va_arg(args, int) ;
                     EMITCHAR(c) ;
                     break ;
          case 'd' : val = va_arg(args, int) ;
                     if ((long) val < 0)
                       {
                         EMITCHAR('-') ;
                         val = -val ;
                       }
                     goto udcommon ;
          case 's' : bp = va_arg(args, char *) ;
                     if (bp == NULL) bp = nullstr ;
                     while ((c = *bp++) != '\0') EMITCHAR(c) ;
                     break ;
          case 'u' : val = va_arg(args, unsigned) ;
          udcommon : {
                       register char *stringp = lowerhex ;
                       bp = buf + MAXDIGS ;
                       stringp = lowerhex ;
                       do
                         {
                           *--bp = stringp[val % 10] ;
                           val /= 10 ;
                         }
                       while (val) ;
                     }
                     p = buf + MAXDIGS ;
                     while (bp < p)
                       {
                         c = *bp++ ;
                         EMITCHAR(c) ;
                       }
                     break ;
          default  : EMITCHAR(*ip) ;
                     break ;
        }
    }
  *op = '\0' ;

  if (Fm->running)
    {
      write_footer(Fm->curr_wno, output, bell) ;
      Fm->file[Fm->curr_wno]->Message = TRUE ;
    }
  else if (*output)
    FPRINTF(stderr, Str[(int) M_FM_MSG], output) ;

  va_end(args) ;
}


int
fm_writable(fname)
char *fname ;
{
  if (geteuid() == 0) return(0) ;
  else                return(access(fname, W_OK)) ;
}


/* Returns number of days since start of century; no error checking. */

int
get_days(month, day, year)
register int month ;
int day ;
register int year ;
{
  register int days = 0 ;
  short leap ;

  /* To fix Bug# 1255947 this was added to protect dates with format CCYY */
  if ( year < 1900 )
    year += 1900 ;

  leap = (year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0) ;
  year-- ;
  while (year >= 1900)
    {
      days += 365 + ((year % 4 == 0) && (year % 100) || (year % 400 == 0)) ;
      year-- ;
    }

  if (leap && month > 2) days++ ;
  month -= 2 ;
  while (month >= 0)
    days += Month_days[month--] ;
  days += day ;

  return(days) ;
}


/*  Read in the contents of a 1bit deep Sun raster file, converting it to
 *  drawable, and reducing it in size, to be less then 64x64 pixels.
 */

unsigned long
get_raster_content(path)
char *path ;
{
  unsigned char *src ;
  int i, slength, srcsize ;
  FILE *fp ;
  struct rasterfile r ;      /* Sun rasterfile header. */

  if ((fp = fopen(path, "r")) == NULL)
    return((unsigned long) NULL) ;

  if (fread((char *) &r, 1, sizeof(r), fp) == 0)
    {
      if (fp) FCLOSE(fp) ;
      return((unsigned long) NULL) ;
    }

  if (r.ras_magic != RAS_MAGIC || r.ras_depth != 1)
    {
      if (fp) FCLOSE(fp) ;
      return((unsigned long) NULL) ;
    }

/*  Skip possible colormap. Yes I know, why is there a colormap associated
 *  with a monochrome image?
 */

  for (i = 0; i < r.ras_maplength; i++) fgetc(fp) ;

  slength = ((r.ras_width + 15) >> 3) &~ 1 ;
  srcsize = slength * r.ras_height ;
  if ((src = (unsigned char *) malloc((unsigned) srcsize)) == NULL)
    {
      if (fp) FCLOSE(fp) ;
      return((unsigned long) NULL) ;
    }

  switch (r.ras_type)
    {
      case RT_OLD           :
      case RT_STANDARD      :             /* standard sun raster files */
                              if (fread(src, 1, srcsize, fp) != srcsize)
                                {
                                  if (fp) FCLOSE(fp) ;
                                  return((unsigned long) NULL) ;
                                }
                              break ;

      case RT_BYTE_ENCODED :             /* encoded sun raster file */
                             if (read_encoded(fp, r.ras_length, src, srcsize))
                               {
                                 if (fp) FCLOSE(fp) ;
                                 return((unsigned long) NULL) ;
                               }
                             break ;

      default              :            /* unreadable sun raster file */
                             if (fp) FCLOSE(fp) ;
                             return((unsigned long) NULL) ;
    }

  FCLOSE(fp) ;        /* no more need for the file ptr so close it */

  return(compress_image(src, srcsize, r.ras_width, r.ras_height, slength)) ;
}


int
getunum(s)
char *s ;
{
  register int i ;
  struct passwd *pw ;

  i = -1 ;
  if ((pw = getpwnam(s)) != NULL) i = pw->pw_uid ;
  return(i) ;
}


void
init_ce(use_ce)                     /* Connect with the CE. */
Boolean use_ce ;
{
  char *err_msg = NULL ;
  char *env, fname[MAXPATHLEN] ;
  struct stat fstatus ;

  Fm->ceo = (CEO) LINT_CAST(calloc(1, sizeof(CE_Object))) ;
  if (use_ce == FALSE) Fm->ceo->running = FALSE ;
  else if ((err_msg = connect_with_ce()) != NULL)
    {
      fm_msg(TRUE, "%s", err_msg) ;
      Fm->ceo->running = FALSE ;
    }
  else Fm->ceo->running = TRUE ;

  STRCPY(fname, "/etc/cetables/cetables") ;
  if (fm_stat(fname, &fstatus) == 0) Fm->eCEmtime = fstatus.st_mtime ;

  env = getenv("OPENWINHOME") ;
  if (env)
      SPRINTF(fname, "%s/lib/cetables/cetables", env) ;
  else
      STRCPY(fname, "/usr/openwin/lib/cetables/cetables") ;
  if (fm_stat(fname, &fstatus) == 0) Fm->oCEmtime = fstatus.st_mtime ;

  SPRINTF(fname, "%s/.cetables/cetables", Fm->home) ;
  if (fm_stat(fname, &fstatus) == 0) Fm->uCEmtime = fstatus.st_mtime ;
}


int
is_recursive(wno, src, dest)     /* Check for recursive copy operations. */
int wno ;
char *src, *dest ;
{
  struct stat sbuf ;

  if (fm_stat(src, &sbuf) == 0)
    if ((sbuf.st_mode & S_IFMT) == S_IFDIR)
      if (EQUALN(dest, src))
        {
          write_footer(wno, Str[(int) M_RECURSIVE], TRUE) ;
          return(TRUE) ;
        }
  return(FALSE) ;
}


void
load_cache(wno)
int wno ;
{
  char fname[MAXPATHLEN] ;
  File_Pane *f = Fm->file[wno] ;
  struct stat fstatus, pstatus ;
  struct utsname name;

  SPRINTF(fname, "%s/.fm%s/.cache", Fm->home, f->path) ;
  f->has_cache = FALSE ;
  if (fm_stat(fname, &fstatus) == 0 && fm_stat((char *)f->path, &pstatus) == 0)
    if (fstatus.st_mtime > pstatus.st_mtime &&
        fstatus.st_mtime > Fm->eCEmtime   &&
        fstatus.st_mtime > Fm->oCEmtime   &&
        fstatus.st_mtime > Fm->uCEmtime)
      if ((f->mmap_ptr = fast_read(fname)) != NULL)
        {
	  if (uname(&name) == -1 || name.machine[0] != f->mmap_ptr->addr[0]) {
		fast_close(f->mmap_ptr) ;
		return;
	  }
          f->cptr = (Cache_Object *) (f->mmap_ptr->addr+4);
          f->ctext = f->cptr->foff ;
          f->has_cache = TRUE ;
        }
}


void
load_directory_info(wno, pos_given, info)
int wno, pos_given, info[MAXINFO] ;
{
  File_Pane *f = Fm->file[wno] ;
  int boolval, intval ;
  char fname[MAXPATHLEN], str[MAXLINE] ;

  SPRINTF(fname, "%s/.fm%s/.state", Fm->home, Fm->file[wno]->path) ;
  if (access(fname, R_OK) == -1) return ;

  load_dir_defs(fname) ;
  if (get_str_resource(FM_DIR_DB, R_DISPMODE, str))
    {
           if (EQUAL(str, "icon"))    f->display_mode = VIEW_BY_ICON ;
      else if (EQUAL(str, "list"))    f->display_mode = VIEW_BY_LIST  ;
      else if (EQUAL(str, "content")) f->display_mode = VIEW_BY_CONTENT ;
    }
  if (get_str_resource(FM_DIR_DB, R_SORTBY, str))
    {
           if (EQUAL(str, "name")) f->sortby = SORT_BY_NAME ;
      else if (EQUAL(str, "type")) f->sortby = SORT_BY_TYPE ;
      else if (EQUAL(str, "size")) f->sortby = SORT_BY_SIZE ;
      else if (EQUAL(str, "date")) f->sortby = SORT_BY_TIME ;
    }
  if (get_bool_resource(FM_DIR_DB, R_SEEDOT, &boolval))
    f->see_dot = boolval ;
  if (get_int_resource(FM_DIR_DB, R_STYLE, &intval))
    f->listopts = intval ;
  f->listopts = f->listopts & 077 ;

  if (get_bool_resource(FM_DIR_DB, R_DISPDIR, &boolval))
    f->dispdir = boolval ;
  if (get_int_resource(FM_DIR_DB, R_NOCHARS, &intval))
    f->num_file_chars = intval ;
  if (get_int_resource(FM_DIR_DB, R_NAMEWID, &intval))
    f->widest_name = intval ;

  if (!pos_given)
    {
      if (get_int_resource(FM_DIR_DB, R_ICONX, &intval)) info[1] = intval ;
      if (get_int_resource(FM_DIR_DB, R_ICONY, &intval)) info[2] = intval ;
      if (get_int_resource(FM_DIR_DB, R_WINX,  &intval)) info[3] = intval ;
      if (get_int_resource(FM_DIR_DB, R_WINY,  &intval)) info[4] = intval ;
      if (get_int_resource(FM_DIR_DB, R_WINW,  &intval)) info[5] = intval ;
      if (get_int_resource(FM_DIR_DB, R_WINH,  &intval)) info[6] = intval ;
    }
  else {
      if(info[1] < 0)
        if (get_int_resource(FM_DIR_DB, R_ICONX, &intval)) info[1] = intval ;
      if(info[2] < 0)
        if (get_int_resource(FM_DIR_DB, R_ICONY, &intval)) info[2] = intval ;
      if(info[3] < 0)
        if (get_int_resource(FM_DIR_DB, R_WINX,  &intval)) info[3] = intval ;
      if(info[4] < 0)
        if (get_int_resource(FM_DIR_DB, R_WINY,  &intval)) info[4] = intval ;
      if(info[5] < 1)
        if (get_int_resource(FM_DIR_DB, R_WINW,  &intval)) info[5] = intval ;
      if(info[6] < 1)
        if (get_int_resource(FM_DIR_DB, R_WINH,  &intval)) info[6] = intval ;

  }


  load_positional_info(wno, "content") ;
  load_positional_info(wno, "icon") ;
  load_positional_info(wno, "list") ;
}


void
load_icon_positions(wno)
int wno ;
{
  char fname[MAXPATHLEN], *ptr ;
  FILE *fp ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p ;
  int ix, iy, lx, ly, n, reply ;

  if (!f->i.moved && !f->l.moved) return ;
  SPRINTF(fname, "%s/.fm%s/.icons", Fm->home, f->path) ;
  f->num_objects = 0 ;
  if ((f->mmap_ptr = fast_read(fname)) == NULL) return ;
  ptr = f->mmap_ptr->addr ;

  while (ptr)
    {
      reply = sscanf(ptr, "%d %d %d %d %s", &ix, &iy, &lx, &ly, fname) ;
      if (reply == 5)
        {
          if (f->num_objects == f->max_objects)
            if (alloc_more_files(wno) == FALSE) break ;

          f_p = f->object + f->num_objects ;
          n = strlen(fname) ;
          if (((*f_p)->name = (WCHAR *) fm_malloc((unsigned) n+1)) == NULL)
            break ;
          STRCPY((char *) (*f_p)->name, fname) ;
          (*f_p)->ix       = ix ;
          (*f_p)->iy       = iy ;
          (*f_p)->lx       = lx ;
          (*f_p)->ly       = ly ;
          (*f_p)->flen     = n ;
          (*f_p)->selected = FALSE ;
          f->num_objects++ ;
        }
      if ((ptr = strchr(ptr, '\n')) != NULL) ptr++ ;
    }
  fast_close(f->mmap_ptr) ;
  if (ix != -1) f->i.pos = TRUE ;
  if (lx != -1) f->l.pos = TRUE ;
}


File_Pane_Object **
make_file_obj(wno, name, x, y)
int wno, x, y ;
char *name ;
{
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p ;
  int n ;

/* Allocate more memory to file array. */

  if (f->num_objects == f->max_objects)
    if (alloc_more_files(wno) == FALSE) return((File_Pane_Object **) NULL) ;

  f_p = f->object + f->num_objects ;

  n = strlen(name) ;
  if (((*f_p)->name = (WCHAR *) fm_malloc((unsigned) n+1)) == NULL)
    return((File_Pane_Object **) NULL) ;

  STRCPY((char *) (*f_p)->name, name) ;
  (*f_p)->flen  = n ;
  (*f_p)->width = fm_strlen((char *) (*f_p)->name) ;
  if ((*f_p)->width > f->widest_name) f->widest_name = (*f_p)->width ;
  set_file_obj_x(wno, *f_p, x) ;
  set_file_obj_y(wno, *f_p, y) ;
  SET_FP_IMAGE((char *) NULL, wno, f->num_objects) ;
  f->num_objects++ ;
  return(f_p) ;
}


char *
make_list_time(val)
time_t *val ;
{
  struct tm *tm ;
  static char timebuf[TIMEBUFLEN] ;

  tm = localtime(val) ;
  strftime(timebuf, TIMEBUFLEN, "%h %e %R %Y\n", tm) ;
  return(timebuf) ;
}


char *
make_mem(ptr, old, new, firsttime)
char *ptr ;
int firsttime, old, new ;
{
  char *ret ;

  if (firsttime) return(fm_malloc((unsigned) new)) ;
  ret = realloc(ptr, old+new) ;
  MEMSET(ret+old, 0, new) ;
  return(ret) ;
}


char *
make_time(val)
time_t *val ;
{
  struct tm *tm ;
  static char timebuf[TIMEBUFLEN] ;

  tm = localtime(val) ;
  strftime(timebuf, TIMEBUFLEN, "%c %Z\n", tm) ;
  return(timebuf) ;
}


void
new_file(folder, fname)      /* Create an empty file or directory. */
Boolean folder ;             /* Create folder? */
char *fname ;                /* Object name. */
{
  Tree_Pane_Object *t_p, *nt_p, *ot_p ;   /* Tree pointers. */
  File_Pane_Object **f_p, **l_p ;         /* Files array pointers. */
  int fd, i ;
  int wno      = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;
  char name[MAXPATHLEN] ;

  CHDIR((char *) f->path) ;
  STRCPY(name, fname) ;
  unique_filename(name) ;        /* Get a unique file name. */

  errno = 0 ;
       if (folder) MKDIR(name, 0777) ;
  else if ((fd = open(name, O_WRONLY | O_CREAT, 00666)) != -1) CLOSE(fd) ;

  if (errno)
    {

/* No luck, bitch and return. */

      fm_msg(TRUE, Str[(int) M_CREAT], name, strerror(errno)) ;
      return ;
    }

  if (folder)
    {

/*  Build structure and insert it into tree, placing it in correct alphabetic
 *  order.
 */

      if ((t_p = make_tree_node((WCHAR *) name)) == NULL) return ;

      t_p->child = NULL ;
      t_p->parent = Fm->tree.current ;
      t_p->mtime = time((time_t *) 0) ;
      t_p->status = 0 ;
      STRCPY((char *) t_p->name, name) ;

      nt_p = Fm->tree.current->child ;
      for (ot_p = NULL; nt_p; ot_p = nt_p, nt_p = nt_p->sibling)
        if (strcmp((char *) t_p->name, (char *) nt_p->name) < 0)
          {
            if (ot_p) ot_p->sibling = t_p ;
            else      Fm->tree.current->child = t_p ;
            t_p->sibling = nt_p ;
            break ;
          }

/* Deselect folder in tree, select folder in folder pane, redraw... */

      if (Fm->tree.selected)                 fm_treedeselect() ;
      if (Fm->file[wno]->path_pane.selected) fm_pathdeselect(wno) ;
      fm_drawtree(TRUE) ;
    }
  fm_display_folder((BYTE) FM_BUILD_FOLDER, wno) ;

/*  Let's assume that the next thing the user will do is to rename that newly
 *  created file.
 */

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno), i = 0; f_p != l_p; f_p++, i++)
    if (strcmp(name, (char *) (*f_p)->name) == 0)
      {
        fm_scroll_to(i, wno) ;
        fm_rename(wno, i) ;
        break ;
      }
}


void
read_user_goto_menu()    /* Read users goto menu entries from ~/.fm/.goto. */
{
  char *alias ;
  char *pathname ;
  struct stat s ;                           /* Status info. */
  int i, intval ;
  int n = 0 ;
  unsigned int len ;

  if (get_int_resource(FM_MERGE_DB, R_NOUGOTO, &intval)) n = intval ;
  Fm->no_user_goto = 0 ;
  for (i = 0; i < n; i++)
    {
      if ((pathname = get_goto_resource(FM_GOTO_PATH, i)) != NULL)
        if (access(pathname, F_OK) == 0)
          if ((fm_stat(pathname, &s) == 0) && (s.st_mode & S_IFMT) == S_IFDIR)
            {
              alias = get_goto_resource(FM_GOTO_LABEL, i) ;
              if (i >= Fm->max_user_goto)
                {
                  Fm->user_goto = (char **)
                    LINT_CAST(make_mem((char *) Fm->user_goto,
                      sizeof(char **) * Fm->max_user_goto,
                      sizeof(char **) * (Fm->max_user_goto + GOTO_ALLOC_INC),
                        (Fm->max_user_goto == 0))) ;
                  Fm->user_goto_label = (char **)
                    LINT_CAST(make_mem((char *) Fm->user_goto_label,
                      sizeof(char **) * Fm->max_user_goto,
                      sizeof(char **) * (Fm->max_user_goto + GOTO_ALLOC_INC),
                        (Fm->max_user_goto == 0))) ;
                  Fm->max_user_goto += GOTO_ALLOC_INC ;
                }
              len = strlen(pathname) + 1 ;
              Fm->user_goto[Fm->no_user_goto] = fm_malloc(len) ;
              STRCPY(Fm->user_goto[Fm->no_user_goto], pathname) ;

              if (alias == NULL) alias = pathname ;
              len = strlen(alias) + 1 ;
              Fm->user_goto_label[Fm->no_user_goto] = fm_malloc(len) ;
              STRCPY(Fm->user_goto_label[Fm->no_user_goto], alias) ;

              Fm->no_user_goto++ ;
            }
    }
}


void
rebuild_folder(wno)     /* Rebuild folder using existing x/y positions. */
int wno ;
{
  char buf[MAXPATHLEN];
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **l_p, **s_p ;    /* Files array pointers. */
  int dcount = 0 ;                          /* Count of deleted files. */
  int ncount = 0 ;                          /* Count of new files. */
  int n, off, x, y ;
  struct dirent *dp ;                       /* Directory content. */
  DIR *dirp ;                               /* Directory file pointer. */
  struct stat fstatus ;                     /* File status. */

  fm_busy_cursor(TRUE, wno) ;
  fm_namestripe(wno) ;
  update_history(wno) ;
  if ((!Fm->isgoto_select) && (is_visible(wno)) && !Fm->error_state)
    write_footer(wno, Str[(int) M_UPDATING], FALSE) ;
  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++) (*f_p)->redisplay = FALSE ;
  qsort((char *) f->object, f->num_objects, sizeof(File_Pane_Object *),
        (int (*)()) compare_name) ;

/* Read directory. */

  if (((dirp = opendir((char *) f->path)) == NULL) ||
        fstat(dirfd(dirp), &fstatus))
    {
      if (dirp) CLOSEDIR(dirp) ;
      fm_msg(TRUE, Str[(int) M_CHDIR], f->path, strerror(errno)) ;
      fm_busy_cursor(FALSE, wno) ;
      return ;
    }

  Fm->mtime[wno] = fstatus.st_mtime ;    /* To test against current node. */


  while ((dp = readdir(dirp)) != NULL)
    {
      if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        continue;
      if ((off = entry_found(wno, dp->d_name)) != -1)
        {
          f_p = f->object + off ;
          my_stat((char *) f->path, f_p, wno, off) ;
        }
      else
        {
          if (!f->see_dot)
            {

/* Ignore dot files if "hidden" toggle is pressed. */

              if (dp->d_name[0] == '.') continue ;
            }

          if (!Fm->writable)
            {
              SPRINTF(buf, "%s/%s", (char *) f->path, dp->d_name) ;
              Fm->writable = (fm_writable(buf) == 0) ;
            }

/* Allocate more memory to file array. */

          if (f->num_objects == f->max_objects)
            if (alloc_more_files(wno) == FALSE) break ;

          f_p = f->object + f->num_objects ;

/* Apply filter... */

          if (f->filter[0])
            {
              n = b_match(dp->d_name, (char *) f->filter) ;
              if (n == 0) continue ;
              else if (n == -1)         /* Pattern in error, null it. */
                f->filter[0] = '\0' ;
            }

          n = strlen(dp->d_name) ;
          if (((*f_p)->name = (WCHAR *) fm_malloc((unsigned) n+1)) == NULL)
            {
              fm_busy_cursor(FALSE, wno) ;
              return ;
            }

          STRCPY((char *) (*f_p)->name, dp->d_name) ;
          (*f_p)->flen  = n ;
          (*f_p)->width = fm_strlen((char *) (*f_p)->name) ;
          if ((*f_p)->width > f->widest_name) f->widest_name = (*f_p)->width ;

          SET_FP_IMAGE((char *) NULL, wno, f->num_objects) ;
          my_stat((char *) f->path, f_p, wno, f->num_objects) ;
          find_free_grid_point(wno, *f_p) ;
          f->num_objects++ ;
        }
      (*f_p)->redisplay = TRUE ;
    }
  CLOSEDIR(dirp) ;

  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    if ((*f_p)->redisplay == FALSE)
      {
        dcount++ ;
        off = f_p - s_p ;
        x   = get_file_obj_x(wno, *f_p) ;
        y   = get_file_obj_y(wno, *f_p) ;
        delete_icon(wno, off, x, y) ;
      }

  if (dcount) condense_folder(wno) ;

  if (((ncount = f->num_objects - f->prev_num_objects) < 0) || Fm->isgoto_select || (!is_visible(wno)))
     ncount = 0;
  /* readjust the dcount for file is overwritten */
  if ((f->num_objects == f->prev_num_objects) && dcount > 0)
     dcount = 0;
  if (!ncount && !dcount && !Fm->isgoto_select && is_visible(wno)) STRCPY(buf, Str[(int) M_UPDATEDNO]) ;
  else
    {
      if (ncount && dcount) {
         if ((ncount > 1) && (dcount > 1))
	    SPRINTF(buf, Str[(int) M_UPDATEDASDS], ncount, dcount);
         else if ((ncount == 1) &&(dcount == 1))
            SPRINTF(buf, Str[(int) M_UPDATEDAD], ncount, dcount);
         else if (ncount > 1)
            SPRINTF(buf, Str[(int) M_UPDATEDASD], ncount, dcount);
         else
            SPRINTF(buf, Str[(int) M_UPDATEDADS], ncount, dcount);
      }

      if (ncount)
        {
          if (ncount == 1)
	     SPRINTF(buf, Str[(int) M_UPDATEDA], ncount);
          else
	     SPRINTF(buf, Str[(int) M_UPDATEDAS], ncount);
        }
      if (dcount)
        {
          if (dcount == 1)
	     SPRINTF(buf, Str[(int) M_UPDATEDD], dcount);
          else
	     SPRINTF(buf, Str[(int) M_UPDATEDDS], dcount);
        }
    }

  if (!Fm->error_state && !Fm->isgoto_select && is_visible(wno))
    write_footer(wno, buf, FALSE) ;
  if (set_canvas_dims(wno, FM_DISPLAY_FOLDER) == 0)
    redisplay_folder(wno, 0, 0, get_pos_attr(wno, FM_POS_CWIDTH),
                                get_pos_attr(wno, FM_POS_CHEIGHT)) ;
  fm_busy_cursor(FALSE, wno) ;
}


void
set_home()    /* Store value of $HOME; we use it often enough! */
{
  char *buf, resolved_path[MAXPATHLEN] ;
  int len ;

  if ((buf = getenv("HOME")) != NULL)
    if (fm_realpath(buf, resolved_path) != NULL)
      {
        len = strlen(fm_realpath(buf, resolved_path)) ;
        Fm->home = (WCHAR *) fm_malloc((unsigned) len+1) ;
        STRCPY((char *) Fm->home, resolved_path) ;
        return ;
      }

  fm_msg(TRUE, Str[(int) M_NEED_HOME_VAR]) ;
  exit(1) ;
}


void
stop_rename()
{
  int n, nlen, tlen ;
  Tree_Pane_Object *t_p ;      /* Tree pointer. */
  char *s, new_name[128] ;     /* New name. */
  File_Pane_Object *f_p;
  if (!Fm->Renaming || Fm->Rename_wno < 0 ||
      Fm->Rename_wno >= Fm->maxwin || !Fm->file[Fm->Rename_wno] ||
      Fm->Renamed_file < 0 ||
      Fm->Renamed_file >= Fm->file[Fm->Rename_wno]->num_objects)
    return;
  f_p = Fm->file[Fm->Rename_wno]->object[Fm->Renamed_file];
  WCHAR *m_p ;
  char *curr_dir=NULL, chdir_stat = -1;

  Fm->Renaming = FALSE ;
  if ((s = get_rename_str(Fm->Rename_wno)) == NULL || !valid_filename(s)) {
 	set_timer(Fm->interval);
	return;
  }


  if (strchr(s, '/') != NULL) {
      	fm_msg(TRUE, Str[(int) M_NOINFO_RENAME], f_p->name, s) ;
      	set_timer(Fm->interval);
 	return;
  }

  STRCPY(new_name, s) ;

  if (EQUAL((char *) f_p->name, new_name)) {
	fm_clrmsg() ;
      	set_timer(Fm->interval);
 	return;
  }

  curr_dir = getcwd(NULL, 1024);
  if (curr_dir &&
	strcmp(curr_dir, (char*)Fm->file[Fm->curr_wno]->path) != 0)
	chdir_stat = chdir((char*)Fm->file[Fm->curr_wno]->path);
  if (access(new_name, F_OK) == 0) {
    	fm_msg(TRUE, Str[(int) M_EXIST], new_name) ;
	if (curr_dir) {
		if (chdir_stat != -1) chdir(curr_dir);
		free(curr_dir);
	}
  }
  else {
	/* Change name in tree. */
	if (rename((char *) f_p->name, new_name) == -1) {
		fm_msg(TRUE, Str[(int) M_CANTRENAME], strerror(errno), f_p->name) ;
		if (curr_dir) {
 			if (chdir_stat != -1) chdir(curr_dir);
			free(curr_dir);
		}
		set_timer(Fm->interval);
		return;
	}
	if (curr_dir) {
		if (chdir_stat != -1) chdir(curr_dir);
		free(curr_dir);
	}
	if (f_p->type == FT_DIR) {
		for (t_p = Fm->tree.current->child; t_p; t_p = t_p->sibling)
		if (strcmp((char *) t_p->name, (char *) f_p->name) == 0)
			break;
		if (t_p) {
			nlen = strlen(new_name) ;
			tlen = strlen((char *) t_p->name) ;
			if (nlen > tlen) {
				m_p = (WCHAR *) fm_malloc((unsigned) strlen(new_name) + 1) ;
				if (m_p == NULL) {
					fm_msg(TRUE, Str[(int) M_UNABLE_RENAME], f_p->name) ;
  					set_timer(Fm->interval);
					return;
				}
				FREE(t_p->name) ;
				t_p->name = m_p ;
			}
			STRCPY((char *) t_p->name, new_name) ;
			fm_sort_children(t_p->parent) ;
		}
		fm_drawtree(FALSE) ;
	}
	n = strlen(new_name) ;
	m_p = (WCHAR *) fm_malloc((unsigned) n + 1) ;
	if (m_p) {
		FREE(f_p->name) ;
		f_p->name = m_p ;
		STRCPY((char *) f_p->name, new_name) ;
		f_p->flen = n ;
		fm_clrmsg() ;            /* Clear "renaming..." prompt. */
	}
	else {
		fm_msg(TRUE, Str[(int) M_UNABLE_RENAME], f_p->name) ;
  		set_timer(Fm->interval);
		return;
	}

	/* Name spacing might change. */
	f_p->width = fm_strlen(new_name) ;
	if (Fm->fprops_showing)
		set_item_str_attr(FM_FIO_FILE_NAME, FM_ITEM_IVALUE, new_name) ;
	fm_match_files(Fm->curr_wno, new_name, FALSE) ;
  }
  set_timer(Fm->interval);
}


#ifdef START_POPUP
void
time_check()
{
  int today = today_in_days() ;

#define END_OF_FM 34030     /* End of allowable time period for filemgr use. */

  if (today > END_OF_FM)
    {
      FPRINTF(stderr, "This pre-beta version of filemgr is no longer") ;
      FPRINTF(stderr, "valid. Terminating...\n") ;
      exit(1) ;
    }
}
#endif /*START_POPUP*/


int
today_in_days()  /* Return number of days from century start 'til today. */
{
  time_t time_p ;
  struct tm *ltime ;

  time_p = time((time_t *) 0) ;
  ltime = localtime(&time_p) ;

  return(get_days(ltime->tm_mon + 1, ltime->tm_mday, ltime->tm_year)) ;
}


void
toggle_hidden_files(wno)
int wno ;
{
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **l_p, **s_p ;    /* Files array pointers. */
  int n ;
  struct dirent *dp ;                       /* Directory content. */
  DIR *dirp ;                               /* Directory file pointer. */
  struct stat fstatus ;                     /* File status. */

  if (f->see_dot == FALSE)
    {
      l_p = PTR_LAST(wno) ;
      s_p = PTR_FIRST(wno) ;
      for (f_p = s_p; f_p < l_p; f_p++) (*f_p)->redisplay = TRUE ;
      n = 0 ;
      for (f_p = s_p; f_p < l_p; f_p++)
        if ((*f_p)->name[0] == '.')
         {
           (*f_p)->redisplay = FALSE ;
           n++ ;
         }
      if (n) condense_folder(wno) ;
    }
  else
    {
      if (((dirp = opendir((char *) f->path)) == NULL) ||
           fstat(dirfd(dirp), &fstatus))
        {
          if (dirp) CLOSEDIR(dirp) ;
          fm_msg(TRUE, Str[(int) M_CHDIR], f->path, strerror(errno)) ;
          return ;
        }



      while ((dp = readdir(dirp)) != NULL)
        if (dp->d_name[0] == '.' && strcmp(dp->d_name, ".") != 0 &&
            strcmp(dp->d_name, "..") != 0)
          {
            if (f->num_objects == f->max_objects)
              if (alloc_more_files(wno) == FALSE) break ;

            f_p = f->object + f->num_objects ;

            if (f->filter[0])
              {
                n = b_match(dp->d_name, (char *) f->filter) ;
                if (n == 0) continue ;
                else if (n == -1)         /* Pattern in error, null it. */
                  f->filter[0] = '\0' ;
              }

            n = strlen(dp->d_name) ;
            if (((*f_p)->name = (WCHAR *) fm_malloc((unsigned) n+1)) == NULL)
              return ;

            STRCPY((char *) (*f_p)->name, dp->d_name) ;
            (*f_p)->flen  = n ;
            (*f_p)->width = fm_strlen((char *) (*f_p)->name) ;
            if ((*f_p)->width > f->widest_name)
              f->widest_name = (*f_p)->width ;

            SET_FP_IMAGE((char *) NULL, wno, f->num_objects) ;
            my_stat((char *) f->path, f_p, wno, f->num_objects) ;
            find_free_grid_point(wno, *f_p) ;
            f->num_objects++ ;
          }
      CLOSEDIR(dirp) ;
    }
}


void
unique_filename(name)
char *name ;
{
  if (access(name, F_OK) == 0)
    {
       File_Pane_Object *tmp ;
       char *label = NULL, *template = NULL, *type, *dir = NULL;
       char buf[512], t_dir[MAXPATHLEN], newname[MAXPATHLEN] ;
       int fd, bufsize;

       char *d_ptr = strrchr(name, '/') ;
       char *e_ptr = d_ptr ;

       if (e_ptr)
          label = ++e_ptr;
       else
	  label = name;

       tmp = (File_Pane_Object *)
	     LINT_CAST(fm_malloc((unsigned) sizeof(File_Pane_Object))) ;
       if ((fd=open(name, 0)) !=-1)
         {
           bufsize = read(fd, buf, sizeof(buf));
           if( bufsize == -1) bufsize = 0;
           close(fd) ;
         }
       if (label && *label)
         {
           if (Fm->ceo->running) {
           	tmp->tns_entry = ce_get_entry(Fm->ceo->file_ns, 3, label, buf, bufsize) ;
           	if (tmp->tns_entry) {
	       		type = ce_get_attribute(Fm->ceo->file_ns, tmp->tns_entry,
				       Fm->ceo->file_type) ;
               		if (!type)
				tmp->tns_entry = NULL ;
	       		else
				tmp->tns_entry = ce_get_entry(Fm->ceo->type_ns, 1, type) ;
	     	}
	   }
           else tmp->tns_entry = NULL ;
         }
       else tmp->tns_entry = get_tns_entry_by_bits((char*)buf, bufsize) ;
       template = get_tns_attr(tmp->tns_entry, Fm->ceo->type_template) ;
       FREE(tmp) ;
       if (d_ptr)
         {
          STRCPY(t_dir, name);
	  t_dir[d_ptr-name] = '\0';
	  dir = t_dir;
	 }
       increment_copy_number(dir, label, template, (char *) newname) ;
       STRCPY(name, newname) ;
    }
}


void
update_history(wno)
int wno ;
{
  File_Pane *f = Fm->file[wno] ;
  int i, n ;

  n = (Fm->maxgoto < Fm->nhistory) ? Fm->maxgoto : Fm->nhistory ;
  for (i = 0; i < n; i++)
    if (strcmp((char *) Fm->history[i], (char *) f->path) == 0) break ;

  if (i == n)
    {
      if (Fm->nhistory && Fm->nhistory == Fm->maxhistory)
        resize_history(FALSE, HIST_ALLOC_INC) ;
      else Fm->nhistory++ ;

      for (i = Fm->nhistory-1; i > 0; i--)
        Fm->history[i] = Fm->history[i-1] ;
      Fm->history[0] = (WCHAR *)
                       fm_malloc((unsigned) strlen((char *) f->path)+1) ;
      if (Fm->history[0]) STRCPY((char *) Fm->history[0], (char *) f->path) ;
    }
}


int
valid_custom_command()
{
  char *cmd = get_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE) ;

/* Make sure UNIX command is not empty. */

  if (cmd == NULL || isempty((WCHAR *) cmd))
    {
      notice(Str[(int) M_UNIX_CMD], Str[(int) M_OK], NULL) ;
      return(FALSE) ;
    }
  return(TRUE) ;
}


int
valid_goto_name(name)
char *name ;
{
  char buf[MAXPATHLEN] ;
  struct stat fstatus ;             /* Status info. */

  if (name == NULL || isempty((WCHAR *) name))
    {
      write_item_footer(FM_PO_PO_FRAME, Str[(int) M_NO_PATH], TRUE) ;
      return(0) ;
    }
  else if (access(name, F_OK) == -1 ||
         (fm_stat(name, &fstatus) == 0) && (fstatus.st_mode & S_IFMT) != S_IFDIR)
    {
      SPRINTF(buf, Str[(int) M_NO_VALID_DIR], name) ;
      write_item_footer(FM_PO_PO_FRAME, buf, TRUE) ;
      return(0) ;
    }
  return(1) ;
}


void
write_cache(wno)
int wno ;
{
  Cache_Object *cptr ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **l_p ;
  struct stat fstatus ;
  char *buf, fname[MAXPATHLEN], *fptr ;
  int buflen ;
  int total = 0 ;
  struct utsname name;

  SPRINTF(fname, "%s/.fm%s/.cache", Fm->home, f->path) ;
  if (fm_stat(fname, &fstatus) == 0)
    if (fstatus.st_mtime > Fm->mtime[wno]) return ;

  qsort((char *) f->object, f->num_objects, sizeof(File_Pane_Object *),
        (int (*)()) compare_dirpos) ;

  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p < l_p; f_p++)
    total += (*f_p)->flen + (*f_p)->tlen ;
  buflen = (f->num_objects * sizeof(Cache_Object)) + total + 4;
  if ((buf = fm_malloc((unsigned) buflen)) == NULL) return ;

  /* this way we know what architecture to get around bogus machine dependent
     cache file */
  if (uname(&name) == -1)
  	strncpy(buf, "spar", 4);
  else
	strncpy(buf, name.machine, 4);

  cptr = (Cache_Object *) (buf+4) ;
  fptr = (buf + 4) + (f->num_objects * sizeof(Cache_Object)) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = PTR_FIRST(wno); f_p < l_p; f_p++)
    {
      cptr->st_mode  = (*f_p)->mode ;     /* File mode. */
      cptr->st_nlink = (*f_p)->nlink ;    /* Number of links. */
      cptr->st_uid   = (*f_p)->uid ;      /* User ID of the file's owner. */
      cptr->st_gid   = (*f_p)->gid ;      /* Group ID of the file's group. */
      cptr->st_size  = (*f_p)->size ;     /* File size in bytes. */
      cptr->mtime    = (*f_p)->mtime ;    /* Time of last data modification */
      cptr->ctime    = (*f_p)->ctime ;    /* Time of last file status change */
      cptr->flen     = (*f_p)->flen ;     /* Length of the filename. */
      cptr->foff     = fptr - (buf + 4) ;       /* Pointer to start of filename. */
      STRNCPY(fptr, (char *) (*f_p)->name, (*f_p)->flen) ;

      cptr->tlen     = (*f_p)->tlen ;     /* Length of the CE TYPE_NAME field. */
      if (cptr->tlen)
        {
          cptr->toff = fptr - (buf + 4) + cptr->flen ;  /* Start of TYPE_NAME field. */
          STRNCPY(fptr + (*f_p)->flen, (*f_p)->type_name, (*f_p)->tlen) ;
        }
      cptr++ ;
      fptr += (*f_p)->flen + (*f_p)->tlen ;
    }
  fast_write(fname, buf, buflen) ;
  FREE(buf) ;
}
