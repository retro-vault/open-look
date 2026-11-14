
/*  @(#)defs.h	1.120 01/11/95
 *
 *  Copyright (c) 1987-1992 Sun Microsystems, Inc.
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

/* Defined to stop some lint errors. */

#ifndef  LINT_CAST
#ifdef   lint
#define  LINT_CAST(arg)  (arg ? 0 : 0)
#else
#define  LINT_CAST(arg)  (arg)
#endif /*lint*/
#endif /*LINT_CAST*/

/* For all function declarations, if ANSI then use a prototype. */

#if  defined(__STDC__)
#define P(args)  args
#else
#define P(args)  ()
#endif /*__STDC__*/

#ifdef XGETTEXT
#define  ERRFILE    "SUNW_DESKSET_FM_ERR"
#define  LABELFILE  "SUNW_DESKSET_FM_LABEL"
#define  MSGFILE    "SUNW_DESKSET_FM_MSG"
#else
extern const char *ERRFILE ;
extern const char *LABELFILE ;
extern const char *MSGFILE ;
#endif /*XGETTEXT*/

#define  DGET(s)  s
#define  EGET(s)  (char *) dgettext(ERRFILE,   s)
#define  LGET(s)  (char *) dgettext(LABELFILE, s)
#define  MGET(s)  (char *) dgettext(MSGFILE,   s)

#ifdef OW_I18N
#define  XDRAWIMAGESTRING(disp, d, font_set, gc, x, y, str, num_bytes) \
               XmbDrawImageString(disp, d, font_set, gc, x, y, str, num_bytes)
#else
#define  XDRAWIMAGESTRING(disp, d, font_set, gc, x, y, str, num_bytes) \
               XDrawImageString(disp, d, gc, x, y, str, num_bytes)
#endif /*OW_I18N*/

#define FM_CANVAS_EACH_PAINT_WINDOW(canvas, pw)    \
   {int _pw_cnt = 0; while (((pw) = \
   (Xv_Window) xv_get((canvas), CANVAS_NTH_PAINT_WINDOW, _pw_cnt++)) != 0) { \

#define FM_CANVAS_END_EACH }}

#define IS_DAMAGED(ix1, iy1, ix2, iy2) \
                  (ix1 < Fm->tree.dx2 && iy1 < Fm->tree.dy2 && \
                   Fm->tree.dx1 < ix2 && Fm->tree.dy1 < iy2)

#define  EQUAL(a, b)      (*a == *b && !strcmp(a, b))
#define  EQUALN(a, b)     (*a == *b && !strncmp(a, b, strlen(b)))
#define  INC              argc-- ; argv++
#define  FM_TIMER_OFF     (Fm->cur_interval == 0)
#define  ZFREE(s)         { if (s != NULL) { FREE(s) ; s = NULL ; } }

#define  FM_CLRDEVICE(mask, n)  (mask &= ~(1 << n))
#define  FM_ISDEVICE(mask, n)   ((mask >> n) & 1)
#define  FM_SETDEVICE(mask, n)  (mask |= (1 << n))

#ifndef MIN
#define  MIN(a, b)  (a > b ? b : a)
#define  MAX(a, b)  (a > b ? a : b)
#endif /*MIN*/

#ifdef CE_DEBUG
#define  CEDB(f)            FPRINTF(stderr, f)
#define  CEDB1(f, a)        FPRINTF(stderr, f, a)
#define  CEDB2(f, a, b)     FPRINTF(stderr, f, a, b)
#else
#define  CEDB(f)
#define  CEDB1(f, a)
#define  CEDB2(f, a, b)
#endif /*CE_DEBUG*/

#ifdef DND_DEBUG
#define  DB(f)                  FPRINTF(stderr, f)
#define  DB1(f, a)              FPRINTF(stderr, f, a)
#define  DB2(f, a, b)           FPRINTF(stderr, f, a, b)
#define  DB3(f, a, b, c)        FPRINTF(stderr, f, a, b, c)
#define  DB4(f, a, b, c, d)     FPRINTF(stderr, f, a, b, c, d)
#define  DB5(f, a, b, c, d, e)  FPRINTF(stderr, f, a, b, c, d, e)
#else
#define  DB(f)
#define  DB1(f, s)
#define  DB2(f, a, b)
#define  DB3(f, a, b, c)
#define  DB4(f, a, b, c, d)
#define  DB5(f, a, b, c, d, e)
#endif /*DND_DEBUG*/

enum res_type {           /* Filemgr X resources. */

/* Custom view properties. */

  R_DISPMODE, R_SORTBY, R_SEEDOT, R_STYLE, R_DISPDIR,

/* Tool properties. */

  R_PRINT,   R_FILTER,   R_NAMEWID,  R_DELETE,   R_DELFDR,
  R_DESFDR,  R_QUIT,
  R_EDITOR,  R_OTHERED,  R_INTERVAL, R_NOCHARS,

/* Misc properties. */

  R_DIRDC,    R_CEBUFN,   R_NEWDOC,   R_NEWDIR,  R_NOSWINS,
  R_CENGINE,  R_TREEVIEW, R_DCOLOR,   R_FCOLOR,  R_ACOLOR,
  R_SHTYPE,   R_MAXGOTO,  R_SYMLINKS, R_NOUGOTO, R_NOCCMDS,
  R_DOCD,     R_LSTATE,   R_LICONS,   R_TREEGAP,
  R_TREEDIR,  R_OLDCCMDS, R_FL_CON,   R_CD_CON,  R_SORTCASE,
  R_AUTOSORT, R_CACHE,    R_GCSIZE,

/* Directory info properties not included above. */

  R_ICONX,   R_ICONY,    R_WINX,     R_WINY,    R_WINW,
  R_WINH,    R_CWIDTH,   R_CHEIGHT,  R_INCX,    R_INCY,
  R_MAXCOL,  R_MAXLINE,  R_STARTX,   R_STARTY,  R_MOVED,
  R_HSBAR,   R_VSBAR,

/* Entries for menu defaults. */

  R_FILE_MENU,       R_VIEW_MENU,        R_EDIT_MENU,
  R_GOTO_MENU,       R_PP_MENU,          R_TFILE_MENU,
  R_TVIEW_MENU,      R_TP_MENU,          R_FP_MENU,
  R_WEDIT_MENU,      R_WPANE_MENU,       R_CC_MENU,
  R_CC_GOTO_EL_MENU, R_CC_GOTO_ELP_MENU,

  R_DUMMYLAST            /* Should always be the last entry. */
} ;

/* Indices into folder icon server image array. */

enum tsv_type { BASICICON, BASICMASK, OPENICON,  OPENMASK } ;

/* Private event codes used in canvas event procs. */

#define MY_ASCII    -101        /* Private event code for ascii key event. */
#define MY_FKEY     -102        /* Private event code for funct. key event. */

/* Not defined in cursor.h */
#define  OLC_FIRST_INDEX  0     /* First index marker. */
#define  OLC_LAST_INDEX   127   /* Last index marker. */

/* Defined & used to stop "unused return value" warnings from lint. */

#define  CHDIR                  (void) chdir
#define  CLOSE                  (void) close
#define  CLOSEDIR               (void) closedir
#define  DUP2                   (void) dup2
#define  ENDMNTENT              (void) endmntent
#define  EXECVE                 (void) execve
#define  FCHMOD                 (void) fchmod
#define  FCHOWN                 (void) fchown
#define  FCLOSE                 (void) fclose
#define  FGETS                  (void) fgets
#define  FPRINTF                (void) fprintf
#define  FPUTS                  (void) fputs
#define  FREE                   (void) free
#define  FSCANF                 (void) fscanf
#define  FSEEK                  (void) fseek
#define  FSTAT                  (void) fstat
#define  FTIME                  (void) ftime
#define  GETHOSTNAME            (void) gethostname
#define  KILL                   (void) kill
#define  MEMCPY                 (void) memcpy
#define  MEMSET                 (void) memset
#define  MKDIR                  (void) mkdir
#define  PCLOSE                 (void) pclose
#define  PRINTF                 (void) printf
#define  PUTS                   (void) puts
#define  READDIR                (void) readdir
#define  REALPATH               (void) realpath
#define  RENAME                 (void) rename
#define  SETEUID                (void) seteuid
#define  SPRINTF                (void) sprintf
#define  SSCANF                 (void) sscanf
#define  STRCAT                 (void) strcat
#define  STRCPY                 (void) strcpy
#define  STRNCPY                (void) strncpy
#define  UNLINK                 (void) unlink
#define  UTIMES                 (void) utimes
#define  VSPRINTF               (void) vsprintf
#define  WRITE                  (void) write

#define  ICON_SET                      (void) icon_set
#define  NOTICE_PROMPT                 (void) notice_prompt
#define  NOTIFY_INTERPOSE_EVENT_FUNC   (void) notify_interpose_event_func
#define  NOTIFY_SET_ITIMER_FUNC        (void) notify_set_itimer_func
#define  NOTIFY_VETO_DESTROY           (void) notify_veto_destroy
#define  SELN_QUERY                    (void) seln_query
#define  XV_CREATE                     (void) xv_create
#define  XV_DESTROY                    (void) xv_destroy
#define  XV_DESTROY_SAFE               (void) xv_destroy_safe
#define  XV_GET                        (void) xv_get
#define  XV_SET                        (void) xv_set

#define  ADD_CHILDREN                  (void) add_children
#define  DS_ADD_HELP                   (void) ds_add_help
#define  DS_SAVE_CMDLINE               (void) ds_save_cmdline
#define  DS_SET_COLORMAP               (void) ds_set_colormap
#define  EXPAND_KEYWORDS               (void) expand_keywords
#define  FM_CREATE_WASTEBASKET         (void) fm_create_wastebasket
#define  FM_GETPATH                    (void) fm_getpath
#define  FM_NEW_FOLDER_WINDOW          (void) fm_new_folder_window
#define  FM_OPENFILE                   (void) fm_openfile
#define  FM_REALPATH                   (void) fm_realpath
#define  FM_RUN_STR                    (void) fm_run_str
#define  FM_SET_CANVAS_HEIGHT          (void) fm_set_canvas_height
#define  FM_SET_CANVAS_WIDTH           (void) fm_set_canvas_width
#define  FIO_COMPARE_AND_UPDATE_PANEL  (void) fio_compare_and_update_panel
#define  GET_FILE_TEMPLATE             (void) get_file_template
#define  GET_SELECTIONS                (void) get_selections
#define  SET_CANVAS_DIMS               (void) set_canvas_dims
#define  SET_FP_IMAGE                  (void) set_fp_image
#define  SET_SHELF_SELECTION           (void) set_shelf_selection

/* For debugging purposes. */

#define  LINENO       printf("--%s[%d]\n",__FILE__, __LINE__) ;
#define  FILE_LINE    __FILE__, __LINE__
#define  ERR_EXIT(s)  err_exit(FILE_LINE, s)


/*  Messages (error or otherwise) have been set into an array which can
 *  be initialized (and internationalized) later.
 *
 *  These are indices into the message array.
 */

enum mess_type {
  M_WIN,                M_NOMEMORY,        M_CHDIR,
  M_PRINT,              M_NOCOPIES,        M_CREAT,
  M_DELETE,             M_SELFIRST,        M_LINK,
  M_COPY,               M_MOVE,            M_READSTR,
  M_NOFLOPPY,		M_NO_INFONAME,     M_CONFIRMDEL,
  M_DELFAILED,          M_EXIST,           M_RENAME,
  M_RENAMEPERM,         M_2MANYFILES,      M_FIND,
  M_CANTRENAME,         M_DATE,            M_OWNER,
  M_SEARCH,             M_PTY,
  M_HOWTOPASTE,         M_HIDDEN,
  M_CMD,                M_EXEC,            M_EXPANDALL_1,
  M_YES,                M_NO,
  M_LINKEDTO,           M_SEARCHING,       M_NFILESFOUND,
  M_NOTHINGFOUND,       M_MAILAPPEND,      M_APPINSHELLTOOL,
  M_APP_SHELLYES,       M_APP_SHELLNO,     M_CANCEL,
  M_UNKNOWNOWNER,       M_UNKNOWNGROUP,
  M_NEED_HOME_VAR,      M_BAD_START_DIR,
  M_DEBUGGING_ON,       M_BAD_INTERVAL,    M_NO_INTERVAL,
  M_USAGE,              M_FILEMGR_VER,

  M_NOSEL,              M_NOOPEN_INV_OBJ,  M_NOGOTO_ARGS,
  M_NOOPEN_METHOD,      M_BADCHAR,         M_INVCHAR,
  M_ENTER_PMETHOD,      M_INV_NOTICE,      M_FULLPATH,
  M_IGNORE_DND,         M_NO_OPERATE1,
  M_NOOPEN_PIPE,        M_NOWRITE_DND,
  M_NOCREATE_DND,       M_NOOPEN_FILE,     M_NOCREATE_CATCH,
  M_NO_REALLOCATE,      M_ALLOC_ATOMS,     M_NO_FILTER,
  M_NO_CONTEXT,         M_NO_PLACE_WASTE,  M_NO_WASTE_MOVE,
  M_NO_APP_DRAG,
  M_NO_SYS_DRAG,        M_ONLY_DRAG,       M_NO_TREE_DND,
  M_NO_FILE_DND,        M_NOCREATE_OBJ,

  M_DND_ERROR,          M_NOGET_DND,       M_DND_TIMEOUT,
  M_DND_ILLEGAL,        M_DND_BADSEL,      M_DND_FAIL,
  M_DND_OLD,            M_REPAINTS,        M_NOT_FDR,
  M_EXEC_FAIL,

  M_MAX_UNDELETE,       M_NO_ORIG,         M_ERR_EXIT,
  M_FM_MSG,             M_OPEN_ERROR,      M_NOPRINT_METHOD,
  M_EMPTY_FILE,         M_NO_DELETE,

  M_BAD_PROPERTY,       M_BAD_TIME,        M_BAD_WIN_ID,
  M_TIMEDOUT,           M_PROPERTY_DELETED,     M_BAD_PROPERTY_EVENT,
  M_BAD_FILENAME,

  M_SURE_DELFOLDER,     M_SURE_DESFOLDER,
  M_DELFOLDER,          M_DESFOLDER,

  M_INV_CE_VER,         M_NO_CE_CONNECT,   M_CE_NAMESPACE,
  M_CE_TYPESPACE,       M_CE_NO_FILE,      M_CE_NO_TYPE,
  M_CE_NO_OPEN,         M_CE_NO_ICON,      M_CE_NO_MASK,
  M_CE_NO_FG,           M_CE_NO_BG,        M_CE_NO_PRINT,
  M_CE_NO_TEM,          M_CE_NO_FDR,       M_CE_NO_DOC,
  M_CE_NO_APP,          M_CE_NO_DROP,      M_DND_SAME,
  M_NO_SOURCE,          M_START_FILTER,    M_NEED_FPATH,

  M_NO_FDROP,           M_NO_CMD,
  M_NO_ACT_NAME,        M_NO_BACK,         M_NO_FORE,
  M_CREATE_CMAP,        M_NO_OPEN,         M_NO_FORK,
  M_NO_DUP,             M_NO_FIND,         M_CHILD_STOP,
  M_CHILD_SIGNAL,       M_CHILD_EXIT,      M_CREATE_GEN_ICON,
  M_CREATE_GEN_MASK,    M_UNCHECK_LICON,   M_NO_ALLOC_TICON,
  M_ERROR_MES,          M_NO_READ,         M_CREATE_GC,

  M_HOME,               M_CONTINUE,        M_WASTE,
  M_VIEW,               M_NO_PATH,         M_LRENAME,
  M_OUT_MEM,            M_NEWFOLDER,
  M_KBYTES1,            M_BYTES,           M_UNLABELED,
  M_EXISTS,
  M_EXISTS2,
  M_EX_OVERWRITE,       M_EX_ALTER,        M_FILEMGR,
  M_ENTER_CMD_ARGS,     M_CMD_ARGS,        M_RUN,
  M_RUNNING,            M_MENU_UPDATE,     M_CONTAINS,
  M_UNDEFINED,          M_AVAILM,          M_AVAILK,
  M_XXXXXXXXX,
  M_UNKNOWN,
  M_KBYTES2,            M_OK,
  M_FM,                 M_WASTEBASKET,
  M_NEWDOCUMENT,        M_CLEANUP_SEL,     M_CLEANUP_ICONS,
  M_OUT_PATH,           M_TRUNC_VIEW,      M_IS_VIEWED,
  M_OPENING,            M_MATCHING,        M_BUILDING,
  M_OPEN_ERRORS,        M_UNABLE_OPEN,
  M_UNABLE_RENAME,      M_DND_SOURCE,
  M_OVER,               M_LUCIDA,          M_LEVELS,
  M_LIST_EXIST,         M_FILE1,
  M_FILE_APPLY,         M_FILE_ADD,        M_DIR_GONE,
  M_AFTER,              M_BEFORE,          M_DO_DELETE,
  M_NO_CREATE_WASTE,    M_NO_VALID_DIR,    M_REDRAWING,
  M_UNDELETING,         M_PRINTING,        M_UNABLE_PRINT,
  M_MISSING,            M_NO_APPLY,        M_APPLY,
  M_DISCARD,            M_CANT_DROP,
  M_DUPLICATE,          M_NO_IFNAME,       M_UNAPPLIED,
  M_APPLY_DISMISS,      M_DISCARD_DISMISS, M_UNIX_CMD,
  M_NO_CREATE,          M_NORESSAVE,       M_NOINFO_RENAME,
  M_GOTOAPP_TREE,       M_GOTOAPP_WASTE,   M_PERM_DENIED,
  M_INFO_MORE,          M_INFO_LESS,       M_TREESEL,
  M_UPDATING,           M_UPDATEDNO,       M_UPDATEDASDS,
  M_UPDATEDAD,          M_UPDATEDASD,      M_UPDATEDADS,
  M_UPDATEDA,           M_UPDATEDAS,       M_UPDATEDD,
  M_UPDATEDDS,		M_EDIT_DELETE,
  M_EDIT_DESTROY,       M_TREE_VERT,       M_TREE_HORZ,
  M_MAIN_TITLE,         M_WASTE_TITLE,
  M_FV_TITLE,           M_NO_POS_VIEW,     M_CCMD_TITLE,
  M_NO_HOME_CHANGE,     M_NO_DISK_NAME,    M_RECURSIVE,
  M_STOP_ABORT,         M_NSELECTED,

  M_COM_TITLE,          M_COM_LONG,        M_COM_TIME,
  M_COM_PRIMARY,        M_COM_OTHER,       M_COM_APPS,
  M_COM_USAGE,          M_COM_USAGE1,      M_COM_USAGE2,
  M_COM_COM,            M_COM_DIVIDE,

  M_QUIT_MES,           M_QUIT_FM, 	   M_RCP_FAIL,
  M_NO_WRITE,

  M_DUMMY_LAST          /* Should always be the last entry. */
} ;

#define  MAXCD              8     /* Maximum number of CD devices. */
#define  MAXFLOPPY          8     /* Maximum number of floppy devices. */

#define  MAXITEMS           (int) FM_ITEM_DUMMY
#define  MAXMENUS           (int) FM_MENU_DUMMY
#define  MAX_MESSAGES       (int) M_DUMMY_LAST
#define  MAXPOPUPS          (int) FM_POPUP_DUMMY
#define  MAXRESOURCES       (int) R_DUMMYLAST

#define  FILE_MENU_TITLED_OR_PINNED   0       /* File button menu entries. */
#define  FILE_BUT_OPEN      FILE_MENU_TITLED_OR_PINNED+1
#define  FILE_BUT_OPEN_ED   FILE_MENU_TITLED_OR_PINNED+2
#define  FILE_BUT_CDOC      FILE_MENU_TITLED_OR_PINNED+3
#define  FILE_BUT_CFOLD     FILE_MENU_TITLED_OR_PINNED+4
#define  FILE_BUT_DUP       FILE_MENU_TITLED_OR_PINNED+5
#define  FILE_BUT_PRINT1    FILE_MENU_TITLED_OR_PINNED+7
#define  FILE_BUT_PRINT     FILE_MENU_TITLED_OR_PINNED+8
#define  FILE_BUT_FIND      FILE_MENU_TITLED_OR_PINNED+10
#define  FILE_BUT_FILEI     FILE_MENU_TITLED_OR_PINNED+11
#define  FILE_BUT_RCOPY     FILE_MENU_TITLED_OR_PINNED+12
#define  FILE_BUT_CCMDS     FILE_MENU_TITLED_OR_PINNED+13
#define  FILE_BUT_FCHECK    FILE_MENU_TITLED_OR_PINNED+15
#define  FILE_BUT_FORMATD   FILE_MENU_TITLED_OR_PINNED+16
#define  FILE_BUT_RENAMED   FILE_MENU_TITLED_OR_PINNED+17
#define  FILE_BUT_COMMENTS  FILE_MENU_TITLED_OR_PINNED+19
#define  FILE_BUT_QUIT      FILE_MENU_TITLED_OR_PINNED+20

#define  VIEW_MENU_TITLED_OR_PINNED   0       /* View button menu entries. */
#define  VIEW_TREE          VIEW_MENU_TITLED_OR_PINNED+1
#define  VIEW_POS_ICON      VIEW_MENU_TITLED_OR_PINNED+3
#define  VIEW_POS_LIST      VIEW_MENU_TITLED_OR_PINNED+4
#define  VIEW_ICON_BY_NAME  VIEW_MENU_TITLED_OR_PINNED+6
#define  VIEW_ICON_BY_TYPE  VIEW_MENU_TITLED_OR_PINNED+7
#define  VIEW_LIST_BY_NAME  VIEW_MENU_TITLED_OR_PINNED+9
#define  VIEW_LIST_BY_TYPE  VIEW_MENU_TITLED_OR_PINNED+10
#define  VIEW_LIST_BY_SIZE  VIEW_MENU_TITLED_OR_PINNED+11
#define  VIEW_LIST_BY_DATE  VIEW_MENU_TITLED_OR_PINNED+12
#define  VIEW_CLEANUP       VIEW_MENU_TITLED_OR_PINNED+14

#define  EDIT_MENU_TITLED_OR_PINNED   0       /* Edit button menu entries. */
#define  EDIT_BUT_SEL_ALL   EDIT_MENU_TITLED_OR_PINNED+1
#define  EDIT_BUT_CUT       EDIT_MENU_TITLED_OR_PINNED+3
#define  EDIT_BUT_COPY      EDIT_MENU_TITLED_OR_PINNED+4
#define  EDIT_BUT_LINK      EDIT_MENU_TITLED_OR_PINNED+5
#define  EDIT_BUT_PASTE     EDIT_MENU_TITLED_OR_PINNED+6
#define  EDIT_BUT_DELETE    EDIT_MENU_TITLED_OR_PINNED+8
#define  EDIT_BUT_PROPS     EDIT_MENU_TITLED_OR_PINNED+10

#define  GOTO_MENU_TITLED_OR_PINNED   0       /* Goto button menu entry. */
#define  GOTO_BUT_HOME      GOTO_MENU_TITLED_OR_PINNED+1

#define  INFO_MENU_TITLED_OR_PINNED   0       /* File information popup menu. */
#define  INFO_BASIC         INFO_MENU_TITLED_OR_PINNED+1
#define  INFO_EXTENDED      INFO_MENU_TITLED_OR_PINNED+2

#define  PROPS_STACK_GENERAL          0   /* Property sheet menu stack. */
#define  PROPS_STACK_NEW_FOLDER       1
#define  PROPS_STACK_CUR_FOLDER       2
#define  PROPS_STACK_GOTO             3
#define  PROPS_STACK_CUSTOM           4
#define  PROPS_STACK_ADVANCED         5

#define  TFILE_MENU_TITLED_OR_PINNED   0    /* Tree pane File button menu. */
#define  TFILE_BUT_OPEN      TFILE_MENU_TITLED_OR_PINNED+1
#define  TFILE_BUT_OPENF     TFILE_MENU_TITLED_OR_PINNED+2
#define  TFILE_BUT_FIND      TFILE_MENU_TITLED_OR_PINNED+4
#define  TFILE_BUT_FILEI     TFILE_MENU_TITLED_OR_PINNED+5
#define  TFILE_BUT_RCOPY     TFILE_MENU_TITLED_OR_PINNED+6
#define  TFILE_BUT_CCMDS     TFILE_MENU_TITLED_OR_PINNED+8

#define  TVIEW_MENU_TITLED_OR_PINNED   0     /* Tree pane View button menu. */
#define  TVIEW_DIR           TVIEW_MENU_TITLED_OR_PINNED+1
#define  TVIEW_SHOW          TVIEW_MENU_TITLED_OR_PINNED+3
#define  TVIEW_HIDE          TVIEW_MENU_TITLED_OR_PINNED+4
#define  TVIEW_START         TVIEW_MENU_TITLED_OR_PINNED+6
#define  TVIEW_ADD           TVIEW_MENU_TITLED_OR_PINNED+7

#define  TREE_PANE_MENU_TITLED_OR_PINNED  0   /* Tree pane menu entries. */
#define  TREE_MENU_OPEN     TREE_PANE_MENU_TITLED_OR_PINNED+1
#define  TREE_MENU_OPENF    TREE_PANE_MENU_TITLED_OR_PINNED+2
#define  TREE_MENU_DIR      TREE_PANE_MENU_TITLED_OR_PINNED+4
#define  TREE_MENU_SHOW     TREE_PANE_MENU_TITLED_OR_PINNED+6
#define  TREE_MENU_HIDE     TREE_PANE_MENU_TITLED_OR_PINNED+7
#define  TREE_MENU_START    TREE_PANE_MENU_TITLED_OR_PINNED+8
#define  TREE_MENU_ADD      TREE_PANE_MENU_TITLED_OR_PINNED+9

#define  PATH_MENU_OPEN     1         /* Path pane menu entries. */

#define  FILE_PANE_MENU_TITLED_OR_PINNED  0   /* File pane menu entries. */
#define  FILE_MENU_SEL_ALL  FILE_PANE_MENU_TITLED_OR_PINNED+1
#define  FILE_MENU_CUT      FILE_PANE_MENU_TITLED_OR_PINNED+3
#define  FILE_MENU_COPY     FILE_PANE_MENU_TITLED_OR_PINNED+4
#define  FILE_MENU_LINK     FILE_PANE_MENU_TITLED_OR_PINNED+5
#define  FILE_MENU_PASTE    FILE_PANE_MENU_TITLED_OR_PINNED+6
#define  FILE_MENU_DELETE   FILE_PANE_MENU_TITLED_OR_PINNED+8
#define  FILE_MENU_PRINT1   FILE_PANE_MENU_TITLED_OR_PINNED+10
#define  FILE_MENU_PRINT    FILE_PANE_MENU_TITLED_OR_PINNED+11
#define  FILE_MENU_CLEAN    FILE_PANE_MENU_TITLED_OR_PINNED+13

#define  WASTE_EDIT_MENU_TITLED_OR_PINNED 0   /* Waste edit menu entries. */
#define  WASTE_EDIT_SEL_ALL   WASTE_EDIT_MENU_TITLED_OR_PINNED+1
#define  WASTE_EDIT_CUT       WASTE_EDIT_MENU_TITLED_OR_PINNED+3
#define  WASTE_EDIT_COPY      WASTE_EDIT_MENU_TITLED_OR_PINNED+4
#define  WASTE_EDIT_PASTE     WASTE_EDIT_MENU_TITLED_OR_PINNED+5
#define  WASTE_EDIT_UNDELETE  WASTE_EDIT_MENU_TITLED_OR_PINNED+6
#define  WASTE_EDIT_DESTROY   WASTE_EDIT_MENU_TITLED_OR_PINNED+8
#define  WASTE_EDIT_PROPS     WASTE_EDIT_MENU_TITLED_OR_PINNED+10

#define  WASTE_PANE_MENU_TITLED_OR_PINNED 0   /* Waste pane menu entries. */
#define  WASTE_PANE_EMPTY     WASTE_PANE_MENU_TITLED_OR_PINNED+1
#define  WASTE_PANE_SEL_ALL   WASTE_PANE_MENU_TITLED_OR_PINNED+3
#define  WASTE_PANE_CUT       WASTE_PANE_MENU_TITLED_OR_PINNED+4
#define  WASTE_PANE_COPY      WASTE_PANE_MENU_TITLED_OR_PINNED+5
#define  WASTE_PANE_PASTE     WASTE_PANE_MENU_TITLED_OR_PINNED+6
#define  WASTE_PANE_UNDELETE  WASTE_PANE_MENU_TITLED_OR_PINNED+7
#define  WASTE_PANE_DESTROY   WASTE_PANE_MENU_TITLED_OR_PINNED+9

#define  FLDR               1        /* Find file types. */
#define  DOC                2
#define  APP                4

#define  M_EL_CUT           1        /* Edit list button menu commands. */
#define  M_EL_COPY          2
#define  M_EL_PASTE         3
#define  M_EL_DELETE        4

#define  M_EL_PASTE_BEFORE  31       /* Edit list paste menu commands. */
#define  M_EL_PASTE_AFTER   32
#define  M_EL_PASTE_TOP     33
#define  M_EL_PASTE_BOTTOM  34

#define  M_PASTE_BEFORE     1        /* Edit list paste menu entries. */
#define  M_PASTE_AFTER      2
#define  M_PASTE_TOP        3
#define  M_PASTE_BOTTOM     4

#define  PROPS_APPLY        100
#define  PROPS_DISCARD      101
#define  PROPS_CANCEL       102

#define  FILE_CONTINUE      100
#define  FILE_CANCEL        101
#define  FILE_ADDFILE       102

/* Define boolean constants. */

#ifndef TRUE
#define  TRUE               1
#endif /*TRUE*/

#ifndef FALSE
#define  FALSE              0
#endif /*FALSE*/

/* Temp buffer size for managing INCR transfers thru the selection service. */

#define XFER_BUFSIZ   64*1024      /* 64k byte transfer size. */

/* Useful constants. */

#define  FM_SUCCESS       0          /* Function succeeded. */
#define  FAILURE          1          /* Function failed. */
#define  FM_EMPTY         (-1)       /* Indicates an empty array. */
#define  UPDATE_INTERVAL  5          /* Secs between folder stat. */
#define  FM_DOC_COLOR     (Fm->folder_color + 1)
#define  FM_APP_COLOR     (Fm->folder_color + 2)
#define  NULL_X           -1         /* Undefined X coord. */
#define  NULL_Y           -1         /* Undefined Y coord. */
#define  OLD_CMD_FILE     ".fmcmd"
#define  PIPE_CLIENT_NO   10         /* XView named pipe client number. */
#define  REGEX_ABORT      2          /* Regular expression error abort. */

#define  PTR_FIRST(wno)   Fm->file[wno]->object
#define  PTR_LAST(wno)    Fm->file[wno]->object + Fm->file[wno]->num_objects

#define  FM_SHELL_APP     0
#define  FM_BIN_APP       1
#define  FM_SUNVIEW_APP   2
#define  FM_X_APP         3

/* Window array indices. */

#define  WNO_TREE        -1          /* Tree pane window index. */
#define  WASTE            0          /* Wastebasket window index. */
#define  MAIN_WIN         1          /* Normal directory window tag. */
#define  MEDIA_WIN        2          /* CD/Floppy window tag. */
#define  WIN_SPECIAL      0          /* Last of the "special" windows. */

/* View by content types. */

#define  CON_MONO_ICON    0x1
#define  CON_MONO_IMAGE   0x2
#define  CON_COLOR_ICON   0x4

/* Maximums: */

#define  FILE_ALLOC_INC     128      /* File pointer allocation increment. */
#define  GOTO_ALLOC_INC     10       /* User goto menu allocation increment. */
#define  HIST_ALLOC_INC     10       /* History goto menu alloc. increment. */
#define  TLINE_ALLOC_INC    256      /* Tree pane Tree_polyline alloc. inc. */
#define  TPANE_ALLOC_INC    512      /* Tree Pane object allocation inc. */

#define  GCDEFSIZE          1500000  /* Default fmgc garbage collect size. */
#define  MAXBIND            128
#define  MAX_CANVAS_SIZE    32767    /* Max size in pixels of a canvas dim. */
#define  MAXINFO            7        /* No of entries in window info. array. */
#define  MAX_INTERVAL       600      /* Maximum file checking interval. */
#define  MAXLEVELS          256      /* Maximum no. of directories in path. */
#define  MAXLINE            120      /* Number of chars in a string. */
#define  MAXCOLORMAPSIZE    255
#define  FM_MAXHISTORY      10       /* Max number of remembered folders. */
#define  MAXSPLITS          4        /* Max no. splits in a folder pane. */
#define  MAXWIN             6        /* Default max. no. of windows. */
#define  MAX_CMD_LEN        512      /* Max length of UNIX command. */
#define  SEGCACHESIZE       50       /* Tree line XDrawSegments cache size. */

/* Margins, gaps and screen dimensions: */

#define  MARGIN             3
#define  TOP_MARGIN         10
#define  POPUP_COLS         40       /* Popup frame width. */
#define  POPUP_ROWS         18       /* Popup frame height. */

#define  CONTENT_HEIGHT     88       /* Height of lines in content mode. */
#define  FM_ICON_HEIGHT     56       /* Height of lines in icon mode. */
#define  LIST_HEIGHT        20       /* Height of lines in list mode. */

/* Sizes of icons in various modes. */

#define  CONTENT_GLYPH_HEIGHT  64    /* View by content. */
#define  CONTENT_GLYPH_WIDTH   64
#define  GLYPH_HEIGHT          32    /* View by icon. */
#define  GLYPH_WIDTH           32
#define  LIST_GLYPH_HEIGHT     16    /* View by list. */
#define  LIST_GLYPH_WIDTH      16

/* Objects: */

#define  FT_DIR             0       /* Tree folder. */
#define  FT_DOC             1       /* Generic document. */
#define  FT_APP             2       /* Executable. */
#define  FT_SYS             3       /* Pipe, socket, /dev stuff... */
#define  FT_BAD_LINK        4       /* Broken symbolic link. */
#define  FT_DIR_UNEXPLORED  5       /* Explored folder. */
#define  FT_DIR_OPEN        6       /* Open folder. */
#define  FT_UNCHECKED       7       /* Unchecked file type. */
#define  FT_WASTE           8       /* Waste basket. */
#define  FT_CD              9       /* CD device. */
#define  FT_FLOPPY          10      /* Floppy device (UNIX). */
#define  FT_DOS             11      /* Floppy device (DOS). */
#define  FT_MAX_TYPES       12

/* Tree status: */

#define  PRUNE             001      /* Pruned? */
#define  SYMLINK           002      /* Symlink? */
#define  TEXPLORE          004      /* Explored this folder? */
#define  TOPEN             010      /* Folder currently open? */

/* Display mode style. */           /* Correspond to panel item values. */

#define  VIEW_BY_ICON       0       /* View by icon. */
#define  VIEW_BY_LIST       1       /* View by list. */
#define  VIEW_BY_CONTENT    2       /* View by content. */

/* Display sort method */           /* Correspond to panel item values. */

#define  SORT_BY_NAME       0       /* Sort alphabetically. */
#define  SORT_BY_TYPE       1       /* Sort name within type. */
#define  SORT_BY_SIZE       2       /* Sort by file size. */
#define  SORT_BY_TIME       3       /* Sort by modification time. */

/* List options. */

#define  LS_DATE            0001    /* Display date. */
#define  LS_OWNER           0002    /* Display owner. */
#define  LS_LINKS           0004    /* Display links. */
#define  LS_SIZE            0010    /* Display size. */
#define  LS_GROUP           0020    /* Display group. */
#define  LS_PERMS           0040    /* Display permissions. */

#define  FM_TEXTEDIT        0       /* Use textedit editor. */
#define  FM_OTHER_EDITOR    1       /* Or other. */

/*  KLUDGE: this value is hardcoded to get around a race condition
 *  in shelltool which does not give vi the proper width and height values
 *  when it is invoked.  The following command will force shelltool to
 *  sleep for 3 seconds before invoking vi.
 */

#define  OTHER_EDITOR       "cmdtool sh -c \"sleep 3; vi $FILE\""

#define  FM_DISP_BY_ROWS    0       /* Display list by rows. */
#define  FM_DISP_BY_COLS    1       /* Display list by columns. */

#define  FM_NEW_FOLDER      1       /* Display dir in new frame. */
#define  FM_BUILD_FOLDER    2       /* Display dir in current frame. */
#define  FM_STYLE_FOLDER    3       /* Display options have changed. */
#define  FM_DISPLAY_FOLDER  4       /* Just display it. */

#define  FM_COPYCURSOR      1       /* Copy cursor. */
#define  FM_MOVECURSOR      2       /* Move cursor. */

/* Folder cursor types. */

#define  OLC_FOLDER_MOVE_DRAG   0
#define  OLC_FOLDER_MOVE_DROP   1
#define  OLC_FOLDERS_MOVE_DRAG  2
#define  OLC_FOLDERS_MOVE_DROP  3
#define  OLC_FOLDER_COPY_DRAG   4
#define  OLC_FOLDER_COPY_DROP   5
#define  OLC_FOLDERS_COPY_DRAG  6
#define  OLC_FOLDERS_COPY_DROP  7

/* Defined types. */

typedef  unsigned char  WCHAR ;          /* For Internationalization. */
typedef  unsigned char  Boolean ;        /* Either TRUE or FALSE. */
typedef  int            (*ROUTINE)() ;   /* Generic function definition. */
typedef  unsigned long  XPixel ;         /* Xlib color element. */
typedef  unsigned long  XGCMask ;        /* Mask used to setup an Xlib gc. */
typedef  unsigned char  BYTE ;
typedef  char           SBYTE ;

enum action_type { FM_HOME, FM_END, FM_PGDN, FM_PGUP } ;

enum db_type { FM_DESKSET_DB, FM_DIR_DB, FM_MERGE_DB, FM_OLDCCMD_DB } ;

enum goto_type { FM_GOTO_PATH, FM_GOTO_LABEL } ;

enum info_type { FM_INFO_ISTATE, FM_INFO_XICON, FM_INFO_YICON,
                 FM_INFO_XOPEN,  FM_INFO_YOPEN, FM_INFO_WIDTH,
                 FM_INFO_HEIGHT } ;

enum item_type {
  FM_FIO_FIO_FRAME,           FM_FIO_FI_PANEL,          FM_FIO_FILE_NAME,
  FM_FIO_OWNER,               FM_FIO_GROUP,             FM_FIO_BITE_SIZE,
  FM_FIO_MOD_TIME,            FM_FIO_ACCESS_TIME,       FM_FIO_FILE_TYPE,
  FM_FIO_PERMISSIONS,         FM_FIO_PERM_READ,		FM_FIO_PERM_WRITE,
  FM_FIO_PERM_EXE,            FM_FIO_OWNER_PERM,        FM_FIO_GROUP_PERM,
  FM_FIO_WORLD_PERM,          FM_FIO_OPEN_METHOD,       FM_FIO_PRINT_METHOD,
  FM_FIO_MOUNT_POINT,         FM_FIO_MOUNT_FROM,        FM_FIO_FREE_SPACE,
  FM_FIO_APPLY_BUTTON,        FM_FIO_ICON_ITEM,
  FM_FIO_CONTENTS,            FM_FIO_OWNER_R,           FM_FIO_OWNER_W,
  FM_FIO_OWNER_E,             FM_FIO_GROUP_R,           FM_FIO_GROUP_W,
  FM_FIO_GROUP_E,             FM_FIO_WORLD_R,           FM_FIO_WORLD_W,
  FM_FIO_WORLD_E,             FM_FIO_RESET_BUTTON,      FM_FIO_MORE_BUTTON,

  FM_PO_PO_FRAME,             FM_PO_PROPS_STACK_PANEL,  FM_PO_PROPS_STACK,

  FM_PO_PROPS_GEN_PANEL,      FM_PO_OPEN_FOLDER,        FM_PO_TREE_DIR,
  FM_PO_DELETE_I,             FM_PO_DEL_MES,            FM_PO_DES_MES,
  FM_PO_DEFDOC,               FM_PO_GEN_APPLY,          FM_PO_GEN_DEFAULT,
  FM_PO_GEN_RESET,

  FM_PO_PROPS_NEWF_PANEL,     FM_PO_NEWF_DEFNAME,
  FM_PO_NEWF_NAMELEN,         FM_PO_NEWF_LAYOUT,        FM_PO_NEWF_SORTBY,
  FM_PO_NEWF_HIDDEN,          FM_PO_NEWF_DISPLAY,       FM_PO_NEWF_LISTOPTS,
  FM_PO_NEWF_CONTENT,         FM_PO_NEW_APPLY,          FM_PO_NEW_DEFAULT,
  FM_PO_NEW_RESET,

  FM_PO_PROPS_CUR_PANEL,      FM_PO_CURF_NAMELEN,
  FM_PO_CURF_LAYOUT,          FM_PO_CURF_DISPLAY,
  FM_PO_CURF_SORTBY,          FM_PO_CURF_HIDDEN,        FM_PO_CURF_LISTOPTS,
  FM_PO_CURF_CONTENT,         FM_PO_CUR_APPLY,          FM_PO_CUR_DEFAULT,
  FM_PO_CUR_RESET,

  FM_PO_PROPS_GOTO_PANEL,     FM_PO_GOTO_CLICK_MES,     FM_PO_PATHNAME,
  FM_PO_GOTO_ADD,             FM_PO_GOTO_MLABEL,        FM_PO_GOTO_LIST,
  FM_PO_GOTO_EDIT,            FM_PO_GOTO_LAST_MES,      FM_PO_GOTO_NUMBER,
  FM_PO_GOTO_APPLY,           FM_PO_GOTO_DEFAULT,       FM_PO_GOTO_RESET,

  FM_PO_PROPS_CUSTOM_PANEL,   FM_PO_CMD_LIST,           FM_PO_EDIT_BUTTON,
  FM_PO_NEW_COMMAND_BUTTON,   FM_PO_CMD_MLABEL,         FM_PO_CMD_CMDLINE,
  FM_PO_CMD_PROMPT,           FM_PO_CMD_PTEXT1,         FM_PO_CMD_OUTPUT,
  FM_PO_CUSTOM_APPLY,         FM_PO_CUSTOM_DEFAULT,     FM_PO_CUSTOM_RESET,

  FM_PO_PROPS_ADVANCED_PANEL, FM_PO_PRINT_I,            FM_PO_FILTER_I,
  FM_PO_INTERVAL_I,           FM_PO_EDITOR_I,           FM_PO_OTHER_EDITOR_I,
  FM_PO_LINK_I,               FM_PO_MEDIA_M,
  FM_PO_CDROM_I,              FM_PO_CONTENT_M,
  FM_PO_FLOPPY_CONTENT,       FM_PO_CD_CONTENT,         FM_PO_ADV_APPLY,
  FM_PO_ADV_DEFAULT,          FM_PO_ADV_RESET,

  FM_RCP_RCP_FRAME,           FM_RCP_REMOTE_PANEL,      FM_RCP_SOURCE_MC,
  FM_RCP_SOURCE,              FM_RCP_DESTINATION_MC,    FM_RCP_DESTINATION,
  FM_RCP_COPY_BUTTON,         FM_RCP_CANCEL,

  FM_CP_CPO_FRAME,            FM_CP_CP_PANEL,           FM_CP_METHOD_ITEM,
  FM_CP_COPIES_ITEM,          FM_CP_PRINT_BUTTON,

  FM_FIND_FIND_FRAME,         FM_FIND_FIND_PANEL,       FM_FIND_FROMITEM,
  FM_FIND_NAMEITEM,           FM_FIND_NAMETOGGLE,       FM_FIND_OWNERITEM,
  FM_FIND_OWNERTOGGLE,        FM_FIND_AFTERITEM,        FM_FIND_AFTER_DATE_PI,
  FM_FIND_BEFOREITEM,         FM_FIND_BEFORE_DATE_PI,   FM_FIND_TYPEITEM,
  FM_FIND_PATTERNITEM,        FM_FIND_CASETOGGLE,       FM_FIND_FIND_BUTTON,
  FM_FIND_OPEN_BUTTON,        FM_FIND_STOP_BUTTON,      FM_FIND_FIND_LIST,

  FM_COM_FRAME,               FM_COM_TEXT,

  FM_REN_FRAME,               FM_REN_PANEL,             FM_REN_DNAME,
  FM_REN_RENAME,              FM_REN_CANCEL,

  FM_ITEM_DUMMY               /* Should always be the last entry. */
} ;

enum item_attr_type { FM_ITEM_BUSY,     FM_ITEM_FIT,      FM_ITEM_HEIGHT,
                      FM_ITEM_INACTIVE, FM_ITEM_IVALUE,   FM_ITEM_LDELETE,
                      FM_ITEM_LROWS,    FM_ITEM_LWIDTH,   FM_ITEM_PANELX,
		      FM_ITEM_PANELY,	FM_ITEM_SHOW,     FM_ITEM_VALX,
		      FM_ITEM_VALY,	FM_ITEM_WIDTH,    FM_ITEM_X,
		      FM_ITEM_X_GAP,	FM_ITEM_Y } ;

enum media_type       { FM_DISK, FM_CD, FM_DOS, FM_FLOPPY } ;

enum menu_type        { FM_FILE_MENU,       FM_VIEW_MENU,        FM_EDIT_MENU,
                        FM_GOTO_MENU,       FM_PP_MENU,          FM_TFILE_MENU,
                        FM_TVIEW_MENU,      FM_TP_MENU,          FM_FP_MENU,
                        FM_WEDIT_MENU,      FM_WPANE_MENU,       FM_CC_MENU,

                        FM_MENU_DUMMY  /* Should always be the last entry. */
                      } ;

enum popup_type { FM_CP_POPUP,  FM_FIO_POPUP, FM_FIND_POPUP,
                  FM_PO_POPUP,  FM_RCP_POPUP, FM_COMMENTS_POPUP,
                  FM_REN_POPUP, FM_POPUP_DUMMY } ;

enum pos_type { FM_POS_CHEIGHT, FM_POS_CWIDTH, FM_POS_INCX,
                FM_POS_INCY,    FM_POS_MAXCOL, FM_POS_MAXLINE,
                FM_POS_STARTX,  FM_POS_STARTY, FM_POS_POS,
                FM_POS_MOVED,   FM_POS_HSBAR,  FM_POS_VSBAR } ;

enum select_type      {      UNIQUE_SELECTION,      EXTEND_SELECTION } ;
enum tree_select_type { TREE_UNIQUE_SELECTION, TREE_EXTEND_SELECTION } ;
enum window_type      { FM_MAIN_FRAME, FM_WASTE_FRAME, FM_TREE_FRAME } ;

enum fm_can_type      { FM_CAN_HEIGHT, FM_CAN_WIDTH,
                        FM_WIN_HEIGHT, FM_WIN_WIDTH, FM_WIN_X, FM_WIN_Y } ;
enum fm_color_type    { FM_FGCOLOR,      FM_BGCOLOR,
                        FM_FG_DEF_COLOR, FM_BG_DEF_COLOR } ;
enum fm_event_type    { E_DRAG,         E_SELECT,    E_ADJUST,
                        E_MENU,         E_RESIZE,    E_FKEY,
                        E_ASCII,        E_DRAG_COPY, E_DRAG_MOVE,
                        E_DRAG_PREVIEW, E_DRAG_LOAD, E_WINENTER,
                        E_WINEXIT,      E_SCROLL,    E_OTHER } ;
enum fm_fp_type       { FM_FP_IMAGE, FM_FP_FGCOLOR, FM_FP_BGCOLOR } ;
enum fm_frame_type    { FM_FRAME_BELL, FM_FRAME_CLOSED, FM_FRAME_SHOW } ;
enum fm_sb_type       { FM_H_SBAR, FM_V_SBAR } ;
enum fm_sb_attr_type  { FM_SB_LINE_HT,  FM_SB_OBJ_LEN, FM_SB_SPLIT,
                        FM_SB_VIEW_LEN, FM_SB_VIEW_ST } ;

/*  KLUDGE - a small state machine is needed to correctly interpret the
 *  stop dnd call.  We cannot stop the dnd request in the middle of
 *  a list of atoms else something will break, and cause the stop
 *  to screw up.
 */

enum Stop_State_Type {
  DO_NOTHING,                   /* Ignore request. */
  TRANSFER_INTERRUPTED,         /* Ready to stop the dnd catch. */
  TRANSFER_FINISHED,            /* No problems occurred transferring. */
  READY_TO_GET_NEXT,            /* Ready to grab next item. */
  READY_TO_GET_BITS             /* Ready to grab bits. */
} ;

enum state_type {
  MAYBE_DRAG,                   /* Possible drag in progress. */
  MAYBE_PAN,                    /* Possible pan in progress. */
  MAYBE_MARQUIS,                /* Possible marquis selection in progress. */
  PAN,                          /* Pan in progress. */
  MARQUIS,                      /* Marquis selection in progress. */
  INCR_SEL,                     /* Incremental selection in progress. */
  RENAMING,                     /* Rename in progress. */
  RESIZING,                     /* Canvas resize in progress. */
  DND_OVER_ITEM,                /* Dragging over an item in progress. */
  OPENING,                      /* Opening an item in progress. */
  CLEAR                         /* Clear state. */
} ;


/* Definitions used by bind.c */
#define  DELIMITER        ','    /* Field delimiter. */
#define  MAXMAGIC         128    /* Maximum number of magic descriptions. */
#define  MAXCOLORMAPSIZE  255

#define  EMAGIC           EGET("Too many magic numbers\n")
#define  EUNKNOWNMAGIC    EGET("Unknown magic number %s\n")
#define  ENOICON          EGET("Must attach icon to rule entry \"%s\"\n")
#define  EBADCOLOR        EGET("Bad color: %s\n")
#define  ENOMEMORY        EGET("Out of memory\n")
#define  BAD_ICON_XID     -1
#define  BAD_COLOR_INDEX  MAXLONG

#define  CMDLINE_KEY        100      /* Keys for XV_KEY_DATA for menu items. */
#define  ALIAS_KEY          200
#define  OUTPUT_KEY         300
#define  PROMPT_KEY         400
#define  PTEXT1_KEY         500
#define  PTEXT2_KEY         600

#define  FM_CANCEL          0        /* Notice prompt types. */
#define  FM_YES             1
#define  FM_NO              2

/* Definitions used by dragdrop.c */
#define  TARGET_RESPONSE      0x1
#define  HOST_RESPONSE        0x2
#define  CURSOR_LOC_RESPONSE  0x4
#define  TRANSPORT_RESPONSE   0x8
#define  FILE_NAME_RESPONSE   0x10
#define  DATA_LABEL_RESPONSE  0x20
#define  OBJECT_LOC_RESPONSE  0x40
#define  BYTES_REQUESTED      0x80
#define  BYTES_XFERRED        0x100

#define  RESPONSE_1           0x7f
#define  RESPONSE_2           0x1ff

/* Definitions used by file.c */
#define  LIST_SHOW(attr)      (Fm->file[wno]->listopts & attr)
#define  SIX_MONTHS_IN_SECS   15552000
#define  HOUR_IN_SECS         3600

#define  THRESHOLD            4   /* Move/copy mode (in pixels). */
#define  LIST_OFFSET          4   /* Offset in list mode for cursor hot spot. */
#define  OVER_NAME            1   /* Include name portion of file object? */
#define  CLEAR_SEL            1   /* Clear the incremental selection? */
#define  THRESHHOLD           4   /* Pixels to move start dnd dragging. */
#define  FM_ICON_IMAGE        0   /* Get icon image? */
#define  FM_ICON_MASK         1   /* Get icon mask? */

#define  FDR_OFF              40  /* Folder stagger offset. */
#define  MAXFILESPERROW       25

/* Definitions used by find.c */
#define  FINDPROG             "/usr/bin/find"
#define  MAXFIND              512

/* Definitions used by fmtree.c */
#define  MAXSTACKDEPTH        0    /* Folder overlap depth. */
#define  THRESHOLD            4    /* Pixels before object move. */
#define  MAXTICONS            4    /* Maximum number of tree folder icons. */

/* Definitions used by info.c */
#define  NMAX                 (sizeof(utmp.ut_name))
#define  MINUID               -2   /* For NFS. */
#define  MAXGID               300

#define  NUM_TOGGLES          9    /* Number of toggles for file permissions. */

/* Definitions used in isubs.c */

/*  Lexical definitions.
 *
 *  All lexical space is allocated dynamically.
 *  The eighth bit of characters is used to prevent recognition,
 *  and eventually stripped.
 */

#define  QUOTE                0200    /* 8th char bit used for 'ing. */
#define  TRIM                 0177    /* Mask to strip quote bit. */

/* Definitions used in main.c */

/* Filemgr rc file definition information. */
#define  DEF_VARIOUS          0       /* Various flags. */
#define  DEF_PSCRIPT          1       /* Print script to use. */
#define  DEF_FILTER           2       /* Filter to use. */
#define  DEF_EDITOR           3       /* Other editor to use. */
#define  DEF_COLOR            4       /* Filemgr color definitions. */
#define  DEF_SHTYPE           5       /* Shell "tool" type name. */
#define  DEF_WASTE            6       /* Filemgr waste basket information. */
#define  MAXDEFS              7       /* Maximum no of rc definition lines. */

#define  S_SMALL              0
#define  S_MEDIUM             1
#define  S_LARGE              2
#define  S_EXTRALARGE         3

/* Definitions used in util.c */

/* Next 5 defines used for rasterfile content viewing code. */
#define  DST_HT               64       /* Height of the compressed image. */
#define  DST_WD               64       /* Width of the compressed image. */
#define  ESCAPE               128
#define  HALFSCALE            512
#define  SCALE                1024

#define  EMITCHAR(c)          *op++ = c
#define  MAXDIGS              11
#define  MSGBUFSIZE           6000

#define  TIMEBUFLEN           40
