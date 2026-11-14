#ifndef lint
static char sccsid[]="@(#)isubs.c	1.89 05/23/95 Copyright 1987-1991 Sun Microsystem, Inc." ;
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

#include <xview/rect.h>

#include "defs.h"
#include "fm.h"
#include "tree.h"
#include "xdefs.h"

extern FMXlib  X;
extern FmVars Fm ;
extern char *Str[] ;

extern int debug ;

typedef enum { north, south } Direction ;

typedef struct {
  Key max ;
  int i ;
  Bools bool ;
} Rec ;

typedef struct {
  int size ;             /* Used in insert and size. */
  int count ;            /* Used in checktree only. */
  Rb_Status status ;     /* Used in checktree and insert. */
  Data data ;            /* Used in lookups only. */
  Node *i ;              /* Used in insert only. */
  Key key ;              /* Used in insert only. */
  Node *d ;              /* Used in delete only. */
  Node *y ;              /* Dummy that is at both links of z. */
  Node *z ;              /* Dummy used as child of leaf nodes. */
  Rb_tree *tree ;        /* Back link to parent tree. */
  Get_key get ;
  Enumerate_proc enumerate ;
  Compare_proc compare ;
} Private ;

typedef void (*Action_proc) (/* Private *private; Node *y, *z, *root */) ;

/*  256-byte table for quickly reversing the bits in an unsigned 8-bit char,
 *  used to convert between MSBFirst and LSBFirst image formats.
 */

#if !defined(__ppc)
static const char revtable[256] = {
#else
static const signed char revtable[256] = {
#endif /* __ppc */
   0, -128,   64,  -64,   32,  -96,   96,  -32,
  16, -112,   80,  -48,   48,  -80,  112,  -16,
   8, -120,   72,  -56,   40,  -88,  104,  -24,
  24, -104,   88,  -40,   56,  -72,  120,   -8,
   4, -124,   68,  -60,   36,  -92,  100,  -28,
  20, -108,   84,  -44,   52,  -76,  116,  -12,
  12, -116,   76,  -52,   44,  -84,  108,  -20,
  28, -100,   92,  -36,   60,  -68,  124,   -4,
   2, -126,   66,  -62,   34,  -94,   98,  -30,
  18, -110,   82,  -46,   50,  -78,  114,  -14,
  10, -118,   74,  -54,   42,  -86,  106,  -22,
  26, -102,   90,  -38,   58,  -70,  122,   -6,
   6, -122,   70,  -58,   38,  -90,  102,  -26,
  22, -106,   86,  -42,   54,  -74,  118,  -10,
  14, -114,   78,  -50,   46,  -82,  110,  -18,
  30,  -98,   94,  -34,   62,  -66,  126,   -2,
   1, -127,   65,  -63,   33,  -95,   97,  -31,
  17, -111,   81,  -47,   49,  -79,  113,  -15,
   9, -119,   73,  -55,   41,  -87,  105,  -23,
  25, -103,   89,  -39,   57,  -71,  121,   -7,
   5, -123,   69,  -59,   37,  -91,  101,  -27,
  21, -107,   85,  -43,   53,  -75,  117,  -11,
  13, -115,   77,  -51,   45,  -83,  109,  -19,
  29,  -99,   93,  -35,   61,  -67,  125,   -3,
   3, -125,   67,  -61,   35,  -93,   99,  -29,
  19, -109,   83,  -45,   51,  -77,  115,  -13,
  11, -117,   75,  -53,   43,  -85,  107,  -21,
  27, -101,   91,  -37,   59,  -69,  123,   -5,
   7, -121,   71,  -57,   39,  -89,  103,  -25,
  23, -105,   87,  -41,   55,  -73,  119,   -9,
  15, -113,   79,  -49,   47,  -81,  111,  -17,
  31,  -97,   95,  -33,   63,  -65,  127,   -1,
} ;

static Bools visit_subtree        P((Node *, Private *, Node *, Direction)) ;

static char *ckalloc              P((unsigned int)) ;

static int assert                 P((int)) ;
int fm_match               P((register char *, register char *)) ;

static Node *balance              P((Node *, Node *, Node *, Node *)) ;

static Rb_Status rb_insert_node   P((Rb_tree *, Node *, Key)) ;

static void check1                P((Node *, Key, Node *, Rec *,
                                     Private *)) ;
static void delete_callback       P((Private *, Node *, Node *, Node *)) ;
static void doit                  P((Rb_tree *, Action_proc)) ;
static void insert_callback       P((Private *, Node *, Node *, Node *)) ;
static void lookup_callback       P((Private *, Node *, Node *, Node *)) ;
static void out_of_memory         P((unsigned int)) ;
static void sys_err_func          P((int)) ;


/* 2-3-4 tree, a.k.a. red-black tree, implementation. */

/*ARGSUSED*/
static void
sys_err_func(error)
int error ;
{}


/*VARARGS1*/
static void
syserr(char *msg, ...)
{
    int saved_errno = errno;
    va_list args;
    if (Fm->Progname) fprintf(stderr, "%s: ", Fm->Progname);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    if (saved_errno) fprintf(stderr, ": %s", strerror(saved_errno));
    fputc('\n', stderr);
    sys_err_func(saved_errno);
    exit(saved_errno ? saved_errno : EXIT_FAILURE);
}

/* ARGSUSED */
static void
out_of_memory(size)
unsigned int size ;
{
}

/*  Wrapper around standard storage allocation, to localize errors.
 *  Taken from Unix World, July 1989, p. 66.
 */

static char *
ckalloc(size)
unsigned int size ;
{
  register char *p ;

/* Try to get the memory. */

  p = (char *) calloc(1, size) ;

/* If it worked, return the memory directly. */

  if (p != NULL) return(p) ;

/* Call a user function, if present. */

  if (out_of_memory != (void (*)()) NULL)
    {
      (*out_of_memory)(size) ;

/* If it returns, try allocation again. */

      p = (char *) calloc(1, size) ;

/* See if it worked the second time. */

      if (p != NULL) return(p) ;
    }

/* No recovery available. */

  syserr("ckalloc: cannot allocate %d bytes", size) ;
  return((char *) NULL) ;
}


static Node *
balance(gg, g, f, x)
Node *gg, *g, *f, *x ;
{
  Node *t ;
  Color tc ;
  if (gg == NULL || g == NULL) exit (-1) ;
  if (f == g->llink)
    {
      if (x == f->rlink)
        {
          f->rlink = x->llink ;
          x->llink = f ;
          t = f ;
          f = x ;
          x = t ;
        }
    }
  else
    {
      if (x == f->llink)
        {
          f->llink = x->rlink ;
          x->rlink = f ;
          t = f ;
          f = x ;
          x = t ;
        }
    }
  if (x == f->llink)
    {
      g->llink = f->rlink ;
      f->rlink = g ;
    }
  else
    {
      g->rlink = f->llink ;
      f->llink = g ;
    }
  if (g == gg->rlink) gg->rlink = f ;
  else gg->llink = f ;
  tc = g->color ;
  g->color = f->color ;
  f->color = tc ;
  return(f) ;
}


static void
doit(tree, proc)
Rb_tree *tree ;
Action_proc proc ;
{
  Private *private ;
  Node *root ;

  if (tree == NULL) return ;
  private = (Private *) LINT_CAST(tree->private) ;
  root = tree->root ;
  if (root == NULL || root->llink != NULL)
    {
      private->status = rb_badtable ;
      return ;
    }
  proc(private, private->y, private->z, root) ;
}


Rb_tree *
rb_create(get, compare)
Get_key get ;
Compare_proc compare ;
{
  Private *p ;
  Node *root, *y, *z ;
  Rb_tree *tree ;

  p = (Private *) LINT_CAST(ckalloc(sizeof(Private))) ;
  p->size      = 0 ;
  p->count     = 0 ;
  p->status    = rb_ok ;
  p->data      = NULL ;
  p->i         = NULL ;
  p->key       = 0 ;
  p->d         = NULL ;
  p->y         = (Node *) LINT_CAST(ckalloc(sizeof(Node))) ;
  p->z         = (Node *) LINT_CAST(ckalloc(sizeof(Node))) ;
  p->get       = get ;
  p->enumerate = NULL ;
  p->compare   = compare ;

  root = (Node *) LINT_CAST(ckalloc(sizeof(Node))) ;
  y = p->y ;
  z = p->z ;
  tree = (Rb_tree *) LINT_CAST(ckalloc(sizeof(Rb_tree))) ;
  tree->root = root ;
  tree->private = (caddr_t) p ;
  tree->client_data = NULL ;
  p->tree = tree ;                 /* Link back so callbacks can access. */
  root->color = black ;
  root->llink = NULL ;
  root->rlink = z ;
  y->color = red ;
  y->llink = y->rlink = NULL ;
  z->color = black ;
  z->llink = z->rlink = y ;

  return(tree) ;
}


/* ARGSUSED */
static void
insert_callback(private, y, z, root)
Private *private ;
Node *y, *z, *root ;
{
  Node *x = NULL, *gg = NULL, *g = NULL, *f = NULL ;
  Comparison c = greater ;

  f = root ;
  x = f->rlink ;
  for (;;)
    {
      if (x->llink->color == red && x->rlink->color == red)
        {
          if (x == z)
            {
              if (c == equal)
                {
                  private->status = rb_duplicate ;
                  root->rlink->color = black ;
                  return ;
                }
              x = private->i ;
              x->llink = z ;
              x->rlink = z ;
              if (c == less) f->llink = x ;
              else           f->rlink = x ;
              c = equal ;
              private->size++ ;
            }
          x->llink->color = black ;
          x->rlink->color = black ;
          x->color = red ;
          if (f->color == red)
            {
              g = balance(gg, g, f, x) ;
              x = g ;
            }
        }
      if (c == equal) break ;
      gg = g ;
      g = f ;
      f = x ;
      c = private->compare(private->key, x->data) ;
      if (c == equal)
        {
          private->status = rb_duplicate ;
          root->rlink->color = black ;
          return ;
        }
      x = (c == less) ? x->llink : x-> rlink ;
    }
  root->rlink->color = black ;
}


static Rb_Status
rb_insert_node(tree, node, key)
Rb_tree *tree ;
Node *node ;
Key key ;
{
  Private *private ;

  if (tree == NULL) return(rb_notable) ;
  private = (Private *) LINT_CAST(tree->private) ;
  private->status = rb_ok ;
  private->i = node ;
  private->key = key ;
  doit(tree, insert_callback) ;
  return(private->status) ;
}


Rb_Status
rb_insert(tree, data, key)
Rb_tree *tree ;
Data data ;
Key key ;
{
  Node *node ;

  if (tree == NULL) return(rb_notable) ;
  node = (Node *) LINT_CAST(ckalloc(sizeof(Node))) ;
  node->data = data ;
  return(rb_insert_node(tree, node, key)) ;
}


static void
delete_callback(private, y, z, root)
Private *private ;
Node *y, *z, *root ;
{
  Node *f, *result, *parent ;
  Node *x, *g, *b ;
  Comparison c ;

  f = root ;
  x = f->rlink ;
  result = NULL ;

  if (x == z) return ;
  y->color = black ;
  if (x->llink->color == black && x->rlink->color == black)
    x->color = red ;
  c = private->compare(private->key, x->data) ;
  if (c == equal)
    {
      result = x ;
      parent = f ;
    }
  for (;;)
    {
      g = f ;
      f = x ;
      if (c == less)
        {
          b = x->rlink ;
          x = x->llink ;
        }
      else
        {
          b = x->llink ;
          x = x->rlink ;
        }
      if (x != z)
        {
          c = private->compare(private->key, x->data) ;
          if (c == equal)
            {
              result = x ;
              parent = f ;
            }
        }
      if (x->color == red || x->llink->color == red || x->rlink->color == red)
        continue ;
      if (b->color == red)
        {
          if (b == f->llink)
            {
              f->llink = b->rlink ;
              b->rlink = f ;
            }
          else
            {
              f->rlink = b->llink ;
              b->llink = f ;
            }
          f->color = red ;
          b->color = black ;
          if (f == g->llink) g->llink = b ;
          else g->rlink = b ;
          x = b ;
          c = private->compare(private->key, x->data) ;
          continue ;
        }
      if (x == z) break ;
      x->color = red ;
      if (b->llink->color == red)
        {
          b->llink->color = black ;
          x = balance(g, f, b, b->llink) ;
          c = private->compare(private->key, x->data) ;
          continue;
        }
      if (b->rlink->color == red)
        {
          b->rlink->color = black ;
          x = balance(g, f, b, b->rlink) ;
          c = private->compare(private->key, x->data) ;
          continue;
        }
      f->color = black;
      b->color = red;
    }
  root->rlink->color = black ;
  z->color = black ;
  y->color = red ;
  if (result != NULL)
    {
      if (g->llink == f) g->llink = z ;
      else g->rlink = z ;
      if (f != result)
        {
          if (parent->llink == result) parent->llink = f ;
          else parent->rlink = f ;
          f->llink = result->llink ;
          f->rlink = result->rlink ;
          f->color = result->color ;
        }
      private->size-- ;
    }
  private->d = result ;
}


Node *
rb_delete(tree, key)
Rb_tree *tree ;
Key key ;
{
  Private *p ;
  if (tree == NULL) return((Node *) NULL) ;
  p = (Private *) LINT_CAST(tree->private) ;
  p->key = key ;
  p->d = NULL ;                     /* In case the key is not found. */
  doit(tree, delete_callback) ;
  return(p->d) ;
}


/*ARGSUSED*/
static void
lookup_callback(private, y, z, root)
Private *private ;
Node *y, *z, *root ;
{
  Comparison c ;
  Node *eq = root->rlink ;
  for (;;)
    {
      if (eq == z) return ;
      c = private->compare(private->key, eq->data) ;
      switch (c)
        {
          case equal   : goto bye ;
          case less    : eq = eq->llink ;
                         break ;
          case greater : eq = eq->rlink ;
                         break ;
          default      : break ;
        }
    }
bye:
   private->data = eq->data ;
}


Data
rb_lookup(tree, key)
Rb_tree *tree ;
Key key ;
{
  Private *private ;

  if (tree == NULL) return((caddr_t) NULL) ;
  private = (Private *) LINT_CAST(tree->private) ;
  private->key = key ;
  private->data = NULL ;         /* Might have been previously used. */
  doit(tree, lookup_callback) ;
  return(private->data) ;
}


static Bools
visit_subtree(node, p, z, dir)
Node *node ;
Private *p ;
Node *z ;
Direction dir ;
{
  Node *link ;

  link = (dir == north) ? node->llink : node->rlink ;
  if (link != z && visit_subtree(link, p, z, dir)) return(true) ;
  if (p->enumerate(node->data)) return(true) ;
  link = (dir == north) ? node->rlink : node->llink ;
  if (link != z) return(visit_subtree(link, p, z, dir)) ;
  else return(false) ;
}


/* ------------------------DEBUGGING------------------------- */

static int
assert(p)
int p ;
{
  return(p) ;
}


static void
check1(x, max, z, rec, private)
Node *x ;
Key max ;
Node *z ;
Rec *rec ;
Private *private ;
{
  int dl, dr ;
  Bools redchild ;
  Rec localrec ;
  Rec *localp = &localrec ;

  if (x == z)
    {
      rec->max = max ;
      rec->i = 0 ;
      rec->bool = false ;
      return ;
    }
  check1(x->llink, max, z, localp, private) ;
  if (private->status == rb_badtable) return ;
  max = localp->max ;
  dl = localp->i ;
  redchild = localp->bool ;
  if (!assert(!(redchild && (x->color == red))))
    {
      private->status = rb_badtable ;
      return ;
    }
  if (!assert(private->compare(max, x->data) ==
             (private->count == 0 ? equal : less)))
    {
      private->status = rb_badtable ;
      return ;
    }
  private->count++ ;
  check1(x->rlink, private->get(x->data), z, localp, private) ;
  if (private->status == rb_badtable) return ;
  max = localp->max ;
  dr = localp->i ;
  redchild = localp->bool ;
  if (!assert (!(redchild && (x->color == red))))
    {
      private->status = rb_badtable ;
      return ;
    }
  if (!assert (dl == dr))
    {
      private->status = rb_badtable ;
      return ;
    }
  rec->max = max ;
  rec->i = dl + ((x->color == black) ? 1 : 0) ;
  rec->bool = ((x->color == red) ? true : false) ;
}


static char *Missing =  NULL ;

int execbrc P((char *, char *)) ;


int
fm_match(s, p)
register char *s, *p ;
{
  register int scc ;
  int c, cc, ok, lc ;

  if (Missing == NULL) Missing = Str[(int) M_MISSING] ;

  for (;;)
    {
      scc = *s++ & TRIM ;
      switch (c = *p++)
        {
          case '{' : return (execbrc(p - 1, s - 1)) ;
          case '[' : ok = 0 ;
                     lc = 077777 ;
                     while ((cc = *p++) != '\0')
                       {
                         if (cc == ']')
                           {
                             if (ok) break ;
                             return(0) ;
                           }
                         if (cc == '-')
                           {
                             if (lc <= scc && scc <= *p++) ok++ ;
                           }
                         else if (scc == (lc = cc)) ok++ ;
                       }
                     if (cc == 0)
                       {
                         fm_msg(TRUE, Missing, ']') ;
                         return(-1) ;
                       }
                     continue ;

          case '*' : if (!*p) return (1) ;
                     for (s--; *s; s++)
                       if (fm_match(s, p)) return(1) ;
                     return(0) ;

          case 0   : return(scc == 0) ;

          default  : if ((c & TRIM) != scc) return(0) ;
                     continue ;

          case '?' : if (scc == 0) return(0) ;
                     continue ;
        }
    }
}


int
execbrc(p, s)
char *p, *s ;
{
  char restbuf[BUFSIZ+2] ;
  register char *pe, *pm, *pl ;
  int brclev = 0 ;
  char *lm, savec ;

  if (Missing == NULL) Missing = Str[(int) M_MISSING] ;

  for (lm = restbuf; *p != '{'; *lm++ = *p++)
    continue ;
  for (pe = ++p; *pe; pe++)
    switch (*pe)
      {
        case '{' : brclev++ ;
                   continue ;

        case '}' : if (brclev == 0) goto pend ;
                   brclev-- ;
                   continue ;

        case '[' : for (pe++; *pe && *pe != ']'; pe++)
                     continue;
                   if (!*pe)
                     {
                       fm_msg(TRUE, Missing, ']') ;
                       return(-1) ;
                     }
                   continue;
      }

pend:

  if (brclev || !*pe)
    {
      fm_msg(TRUE, Missing, '}') ;
      return(-1) ;
    }
  for (pl = pm = p; pm <= pe; pm++)
    switch (*pm & (QUOTE | TRIM))
      {
        case '{'         : brclev++;
                           continue ;

        case '}'         : if (brclev)
                             {
                               brclev-- ;
                               continue ;
                             }
                           goto doit ;

        case ',' | QUOTE :
        case ','         : if (brclev) continue;

doit:

                           savec = *pm ;
                           *pm = 0 ;
                           STRCPY(lm, pl) ;
                           STRCAT(restbuf, pe + 1) ;
                           *pm = savec ;
                           if (fm_match(s, restbuf))
                             return(1) ;
                           pl = pm + 1 ;
                           continue ;

        case '['         : for (pm++; *pm && *pm != ']'; pm++)
                             continue ;
                           if (!*pm)
                             {
                               fm_msg(TRUE, Missing, ']') ;
                               return(-1) ;
                             }
                           continue ;
      }
  return(0) ;
}


void
adjust_pos_info(wno)
int wno ;
{
  File_Pane *f = Fm->file[wno] ;
  int height, incx, incy, max_filespercol, max_filesperline, sb_wid ;
  int startx, starty, width, win_width, win_height ;

  if (wno == WNO_TREE || !Fm->running || !f->num_objects) return ;
  if (f->display_mode == VIEW_BY_LIST && f->listopts) return ;

  height = get_canvas_attr(wno, FM_CAN_HEIGHT) ;
  width  = get_canvas_attr(wno, FM_CAN_WIDTH) ;
  incx   = get_pos_attr(wno, FM_POS_INCX) ;
  incy   = get_pos_attr(wno, FM_POS_INCY) ;
  sb_wid     = sb_width(wno) ;
  startx     = get_pos_attr(wno, FM_POS_STARTX) ;
  starty     = get_pos_attr(wno, FM_POS_STARTY) ;
  win_width  = get_canvas_attr(wno, FM_WIN_WIDTH)  - sb_wid ;
  win_height = get_canvas_attr(wno, FM_WIN_HEIGHT) - sb_wid ;

  if (incx)
  {
    if (width < win_width) width = win_width ;
    if (f->dispdir == FM_DISP_BY_COLS) width -= (2 * startx) ;
    max_filesperline = width / incx ;
    if (max_filesperline == 0) max_filesperline = 1 ;
    set_pos_attr(wno, FM_POS_MAXLINE, max_filesperline) ;
  }
  if (incy)
  {
    if (height < win_height) height = win_height ;
    if (f->dispdir == FM_DISP_BY_ROWS) height -= (2 * starty) ;
    max_filespercol = height / incy ;
    if (max_filespercol == 0) max_filespercol = 1 ;
    set_pos_attr(wno, FM_POS_MAXCOL, max_filespercol) ;
  }
}


void
animate_icon(wno, off, newx, newy)
int wno, off, newx, newy ;
{
  File_Pane_Object **f_p = Fm->file[wno]->object + off ;
  int x, y ;

  x = get_file_obj_x(wno, *f_p) ;
  y = get_file_obj_y(wno, *f_p) ;
  delete_icon(wno, off, x, y) ;
  set_file_obj_x(wno, *f_p, newx) ;
  set_file_obj_y(wno, *f_p, newy) ;
  draw_ith(off, wno) ;
}


void
apply_goto_panel()
{
  char *alias, *pathname ;
  int i, len, nitems, selected ;

  if ((selected = get_list_first_selected(FM_PO_GOTO_LIST)) != -1)
    {
      do_goto_list_deselect(selected) ;
      select_list_item(FM_PO_GOTO_LIST, selected, FALSE) ;
      Fm->ignore_goto_desel = TRUE ;
    }

  Fm->maxgoto = get_item_int_attr(FM_PO_GOTO_NUMBER, FM_ITEM_IVALUE) ;
  nitems      = get_item_int_attr(FM_PO_GOTO_LIST,   FM_ITEM_LROWS) ;

  Fm->max_user_goto = Fm->no_user_goto = nitems - 1 ;
  Fm->user_goto = (char **) LINT_CAST(make_mem((char *) Fm->user_goto,
                          0, sizeof(char **) * Fm->max_user_goto, TRUE)) ;
  Fm->user_goto_label = (char **)
    LINT_CAST(make_mem((char *) Fm->user_goto_label, 0,
              sizeof(char **) * Fm->max_user_goto, TRUE)) ;
  for (i = 0; i < Fm->max_user_goto; i++)
    {
      alias = get_list_str(FM_PO_GOTO_LIST, i+1) ;
      len = strlen(alias) + 1 ;
      Fm->user_goto_label[i] = fm_malloc(len) ;
      STRCPY(Fm->user_goto_label[i], alias) ;

      pathname = get_list_client_data(FM_PO_GOTO_LIST, i+1) ;
      len = strlen(pathname) + 1 ;
      Fm->user_goto[i] = fm_malloc(len) ;
      STRCPY(Fm->user_goto[i], pathname) ;
    }

  Fm->goto_changed = FALSE ;
  set_goto_button_menu() ;
}


void
change_pos_view(new_mode)       /* Change to a new positional view. */
int new_mode ;
{
  BYTE mode ;
  int wno      = Fm->curr_wno ;
  File_Pane *f = Fm->file[wno] ;

  fm_busy_cursor(TRUE, wno) ;
  f->display_mode = new_mode ;
  if (new_mode == VIEW_BY_LIST) f->listopts = 0 ;
  if (!get_pos_attr(wno, FM_POS_MOVED))
    {
      mode = FM_STYLE_FOLDER ;
      set_pos_attr(wno, FM_POS_POS, FALSE) ;
    }
  else
    {
      mode = FM_BUILD_FOLDER ;
      if (!Fm->autosort) set_pos_attr(wno, FM_POS_POS, TRUE) ;
    }
  fm_scrollbar_scroll_to(wno, FM_H_SBAR, 0) ;
  fm_scrollbar_scroll_to(wno, FM_V_SBAR, 0) ;
  if (set_canvas_dims(wno, mode) == 0) fm_display_folder(mode, wno) ;

  if (is_popup_showing(FM_PO_PO_FRAME))
    {
      reset_cur_folder_panel() ;   /* Set view panel items. */
      set_list_item(FALSE) ;       /* Grayout/activate view panel items. */
    }
  fm_busy_cursor(FALSE, wno) ;
}


int
check_advanced_panel()
{
  int c = 0 ;

  c += (Fm->follow_links == get_item_int_attr(FM_PO_LINK_I,  FM_ITEM_IVALUE)) ;
  c += (Fm->interval != get_item_int_attr(FM_PO_INTERVAL_I,  FM_ITEM_IVALUE)) ;
  c += (Fm->editor != (short) get_item_int_attr(FM_PO_EDITOR_I,
                                                FM_ITEM_IVALUE)) ;
  c += strcmp(Fm->other_editor,
              get_item_str_attr(FM_PO_OTHER_EDITOR_I, FM_ITEM_IVALUE)) ;
  c += (Fm->show_cd     == get_item_int_attr(FM_PO_CDROM_I,  FM_ITEM_IVALUE)) ;
  c += (Fm->cd_content ==
        get_item_int_attr(FM_PO_CD_CONTENT, FM_ITEM_IVALUE)) ;
  c += (Fm->floppy_content ==
        get_item_int_attr(FM_PO_FLOPPY_CONTENT, FM_ITEM_IVALUE)) ;
  c += strcmp((char *) Fm->print_script,
              get_item_str_attr(FM_PO_PRINT_I, FM_ITEM_IVALUE)) ;
  c += strcmp((char *) Fm->filter,
              get_item_str_attr(FM_PO_FILTER_I, FM_ITEM_IVALUE)) ;
  return(c) ;
}


int
check_cur_folder_panel()
{
  int c = 0 ;
  File_Pane *f = Fm->file[Fm->curr_wno] ;

  c += (f->num_file_chars != get_item_int_attr(FM_PO_CURF_NAMELEN,
                                               FM_ITEM_IVALUE)) ;
  c += (f->dispdir == get_item_int_attr(FM_PO_CURF_LAYOUT, FM_ITEM_IVALUE)) ;
  c += (f->display_mode   != (short) get_item_int_attr(FM_PO_CURF_DISPLAY,
                                                        FM_ITEM_IVALUE)) ;
  c += (f->content_types  != get_item_int_attr(FM_PO_CURF_CONTENT,
                                                FM_ITEM_IVALUE)) ;
  c += (f->sortby         != (short) get_item_int_attr(FM_PO_CURF_SORTBY,
                                                        FM_ITEM_IVALUE)) ;
  c += (f->see_dot  != get_item_int_attr(FM_PO_CURF_HIDDEN, FM_ITEM_IVALUE)) ;
  c += (f->listopts != get_item_int_attr(FM_PO_CURF_LISTOPTS,
                                          FM_ITEM_IVALUE)) ;
  return(c) ;
}


int
check_custom_panel()    /* Return TRUE, if command popup has been modified. */
{
  Custom_Command *cc ;
  int selected = get_list_first_selected(FM_PO_CMD_LIST) ;
  int is_output, is_prompt ;
  int c = 0 ;

  if (selected != -1)
    {
      cc = (Custom_Command *) get_list_client_data(FM_PO_CMD_LIST, selected) ;
      c += strcmp(get_item_str_attr(FM_PO_CMD_MLABEL,  FM_ITEM_IVALUE),
                  cc->alias) ;
      c += strcmp(get_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE),
                  cc->command) ;
      if (cc->prompt)
        c += strcmp(get_item_str_attr(FM_PO_CMD_PTEXT1,  FM_ITEM_IVALUE),
                    cc->prompt) ;
      is_prompt = (EQUAL(cc->is_prompt, "false")) ;
      c += (get_item_int_attr(FM_PO_CMD_PROMPT, FM_ITEM_IVALUE) != is_prompt) ;
      is_output = (EQUAL(cc->is_output, "false")) ;
      c += (get_item_int_attr(FM_PO_CMD_OUTPUT, FM_ITEM_IVALUE) != is_output) ;
    }
  else
    {
      c += strcmp(get_item_str_attr(FM_PO_CMD_MLABEL,  FM_ITEM_IVALUE), "") ;
      c += strcmp(get_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE), "") ;
      c += strcmp(get_item_str_attr(FM_PO_CMD_PTEXT1,  FM_ITEM_IVALUE), "") ;
      c += (get_item_int_attr(FM_PO_CMD_PROMPT, FM_ITEM_IVALUE) != TRUE) ;
      c += (get_item_int_attr(FM_PO_CMD_OUTPUT, FM_ITEM_IVALUE) != TRUE) ;
    }
  c += Fm->custom_changed ;
  return(c) ;
}


int
check_general_panel()
{
  int c = 0 ;

  c += (Fm->newwin != get_item_int_attr(FM_PO_OPEN_FOLDER, FM_ITEM_IVALUE)) ;
  c += (Fm->tree_vertical == get_item_int_attr(FM_PO_TREE_DIR,
                                               FM_ITEM_IVALUE)) ;
  c += (Fm->confirm_delete == (Boolean) get_item_int_attr(FM_PO_DELETE_I,
                                                          FM_ITEM_IVALUE)) ;
  c += strcmp(Fm->newdoc, get_item_str_attr(FM_PO_DEFDOC, FM_ITEM_IVALUE)) ;
  return(c) ;
}


int
check_goto_panel()
{
  int selected = get_list_first_selected(FM_PO_GOTO_LIST) ;
  int c = 0 ;

  if (selected != -1)
    {
      c += strcmp(get_item_str_attr(FM_PO_PATHNAME,    FM_ITEM_IVALUE),
                  get_list_client_data(FM_PO_GOTO_LIST, selected)) ;
      c += strcmp(get_item_str_attr(FM_PO_GOTO_MLABEL, FM_ITEM_IVALUE),
                  get_list_str(FM_PO_GOTO_LIST, selected)) ;
    }
  else
    {
      c += strcmp(get_item_str_attr(FM_PO_PATHNAME,    FM_ITEM_IVALUE), "") ;
      c += strcmp(get_item_str_attr(FM_PO_GOTO_MLABEL, FM_ITEM_IVALUE), "") ;
    }
  c += Fm->goto_changed ;
  c += (Fm->maxgoto != get_item_int_attr(FM_PO_GOTO_NUMBER, FM_ITEM_IVALUE)) ;
  return(c) ;
}


int
check_new_folder_panel()
{
  int c = 0 ;

  c += strcmp(Fm->newdir,
              get_item_str_attr(FM_PO_NEWF_DEFNAME, FM_ITEM_IVALUE)) ;
  c += (Fm->num_file_chars != get_item_int_attr(FM_PO_NEWF_NAMELEN,
                                                FM_ITEM_IVALUE)) ;
  c += (Fm->dispdir == get_item_int_attr(FM_PO_NEWF_LAYOUT, FM_ITEM_IVALUE)) ;
  c += (Fm->display_mode   != (short) get_item_int_attr(FM_PO_NEWF_DISPLAY,
                                                        FM_ITEM_IVALUE)) ;
  c += (Fm->content_types  != get_item_int_attr(FM_PO_NEWF_CONTENT,
                                                FM_ITEM_IVALUE)) ;
  c += (Fm->sortby         != (short) get_item_int_attr(FM_PO_NEWF_SORTBY,
                                                        FM_ITEM_IVALUE)) ;
  c += (Fm->see_dot  != get_item_int_attr(FM_PO_NEWF_HIDDEN, FM_ITEM_IVALUE)) ;
  c += (Fm->listopts != get_item_int_attr(FM_PO_NEWF_LISTOPTS,
                                          FM_ITEM_IVALUE)) ;
  return(c) ;
}


int
check_prop_values(value)
int value ;
{
  int c = 0 ;   /* Set true if property item change on this sheet. */
  int result ;

  switch (value)
    {
      case PROPS_STACK_GENERAL    : c = check_general_panel() ;
                                    break ;
      case PROPS_STACK_NEW_FOLDER : c = check_new_folder_panel() ;
                                    break ;
      case PROPS_STACK_CUR_FOLDER : c = check_cur_folder_panel() ;
                                    break ;
      case PROPS_STACK_GOTO       : c = check_goto_panel() ;
                                    break ;
      case PROPS_STACK_CUSTOM     : c = check_custom_panel() ;
                                    break ;
      case PROPS_STACK_ADVANCED   : c = check_advanced_panel() ;
    }

  if (!c) return(FALSE) ;

  result = prompt_with_message(Fm->curr_wno,
             Str[(int) M_NO_APPLY],
             Str[(int) M_APPLY], Str[(int) M_DISCARD], Str[(int) M_CANCEL]) ;

       if (result == FM_YES) apply_prop_values(value) ;
  else if (result == FM_NO)  reset_prop_values(value) ;
  return(result == FM_CANCEL) ;
}


/*  compress_image() uses a reduction algorithm based on:
 *
 *  pbmreduce.c - read a portable bitmap and reduce it N times
 *
 *  Copyright (C) 1989 by Jef Poskanzer.
 *
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for any purpose and without fee is hereby granted, provided
 *  that the above copyright notice appear in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation.  This software is provided "as is" without express or
 *  implied warranty.
 */


/*ARGSUSED*/
unsigned long
compress_image(src, srcsize, swidth, sheight, slength)
unsigned char *src ;
int swidth, sheight, slength, srcsize ;
{
  register unsigned char **bitslice, *newbitrow, *nbP ;
  unsigned char *bP, *dptr, *dst, item, *sptr ;
  int bitshift, c, col, cols, count ;
  int dheight, direction, dlength, dstsize, dwidth ;
  int limitcol, n, newcols, newrows, resid, row, rows ;
  int subrow, subcol ;
  long sum, *thiserr, *nexterr, *temperr ;

  unsigned long image ;

  dheight = DST_HT ;
  dwidth = DST_WD ;
  dlength = (dwidth + 7) / 8 ;
  dstsize = dlength * dheight ;
  if ((dst = (unsigned char *) malloc((unsigned) dstsize)) == NULL)
    return((unsigned long) NULL) ;

  MEMSET((char *) dst, 0, dstsize) ;

  cols = swidth ;
  rows = sheight ;
  slength *= 8 ;                /* Scanline length in bits. */

  if (cols > rows)              /* Determine reduction factor. */
    {
      n = cols / dwidth ;
      if ((n * dwidth) < cols) n++ ;
    }
  else
    {
      n = rows / dheight ;
      if ((n * dheight) < rows) n++ ;
    }

  bitslice = (unsigned char **)
             LINT_CAST(malloc((unsigned) (n * sizeof(char *)))) ;
  if (bitslice == (unsigned char **) NULL) return((unsigned long) NULL) ;
  bitslice[0] = (unsigned char *) malloc((unsigned) (n * slength)) ;
  if (bitslice[0] == (unsigned char *) NULL) return((unsigned long) NULL) ;
  for (c = 1; c < n; c++) bitslice[c] = &(bitslice[0][c * slength]) ;

  newrows = rows / n ;
  newcols = cols / n ;
  resid = (newcols + 7) / 8 ;
  newbitrow = (unsigned char *) malloc((unsigned) newcols) ;
  if (newbitrow == (unsigned char *) NULL) return((unsigned long) NULL) ;

/* Initialize Floyd-Steinberg. (random errors in [-SCALE/8 .. SCALE/8]) */

  thiserr = (long *)
            LINT_CAST(malloc((unsigned) ((newcols + 2) * sizeof(long)))) ;
  nexterr = (long *)
            LINT_CAST(malloc((unsigned) ((newcols + 2) * sizeof(long)))) ;
  if (thiserr == NULL || nexterr == NULL) return((unsigned long) NULL) ;

  srand((int) time((time_t *) 0)) ;
  for (col = 0; col < newcols + 2; col++)
    thiserr[col] = (rand() % SCALE - HALFSCALE) / 4 ;

  direction = 1 ;

  sptr = src ;
  dptr = dst ;
  for (row = 0; row < newrows; row++)
    {
      for (subrow = 0; subrow < n; subrow++)
        {
          bitshift = -1 ;
          for (c = 0, bP = bitslice[subrow]; c < slength; c++, bP++)
            {
              if (bitshift == -1)
                {
                  item = *sptr++ ;
                  bitshift = 7 ;
                }
              *bP = (item >> bitshift) & 1 ;
              bitshift-- ;
            }
        }

      for (col = 0; col < newcols + 2; col++) nexterr[col] = 0 ;
      if (direction)
        {
          col = 0 ;
          limitcol = newcols ;
          nbP = newbitrow ;
        }
      else
        {
          col = newcols - 1 ;
          limitcol = -1 ;
          nbP = &(newbitrow[col]) ;
        }

      do
        {
          sum = 0 ;
          count = 0 ;
          for (subrow = 0; subrow < n; subrow++)
            for (subcol = 0; subcol < n; subcol++)
              if (row * n + subrow < rows && col * n + subcol < cols)
                {
                  count += 1 ;
                  if (bitslice[subrow][col * n + subcol] == 0)  /* White? */
                    sum += 1 ;
                }

          sum = (sum * SCALE) / count ;
          sum += thiserr[col + 1] ;

          if (sum >= HALFSCALE)
            {
              *nbP = 0 ;       /* White. */
              sum -= SCALE ;
            }
          else *nbP = 1 ;      /* Black. */

          if (direction)
            {
              thiserr[col + 2] += (sum * 7) / 16 ;
              nexterr[col    ] += (sum * 3) / 16 ;
              nexterr[col + 1] += (sum * 5) / 16 ;
              nexterr[col + 2] += (sum    ) / 16 ;
            }
          else
            {
              thiserr[col    ] += (sum * 7) / 16 ;
              nexterr[col + 2] += (sum * 3) / 16 ;
              nexterr[col + 1] += (sum * 5) / 16 ;
              nexterr[col    ] += (sum    ) / 16 ;
            }
          if (direction)
            {
              col++ ;
              nbP++ ;
            }
          else
            {
              col-- ;
              nbP-- ;
            }
        }
      while (col != limitcol) ;

      bitshift = 7 ;
      item = 0 ;
      for (c = 0, bP = newbitrow; c < newcols; c++, bP++)
        {
          if (*bP) item += 1 << bitshift ;
          bitshift-- ;
          if (bitshift == -1)
            {
              *dptr++ = item ;
              bitshift = 7 ;
              item = 0 ;
            }
        }
      if (bitshift != 7) *dptr++ = item ;
      if (resid < 8) dptr += (8 - resid) ;

      temperr = thiserr ;
      thiserr = nexterr ;
      nexterr = temperr ;
      direction = !direction ;
    }

  image = make_compressed(Fm->curr_wno, dwidth, dheight, dst) ;

/* Free up as much memory as possible. */

  FREE((char *) src) ;
  FREE((char *) dst) ;
  FREE((char *) bitslice[0]) ;
  FREE((char *) bitslice) ;
  FREE((char *) newbitrow) ;
  FREE((char *) thiserr) ;
  FREE((char *) nexterr) ;
  return(image) ;
}


void
display_tree(f_p)
Tree_Pane_Object *f_p ;
{
  int damx1, damy1, damx2, damy2, type, x1, x2, xoff, y1, y2, yoff ;

  xoff = (Fm->tree_vertical) ? Fm->tree_Ymin : 0 ;
  yoff = (Fm->tree_vertical) ? 0 : Fm->tree_Ymin ;
  for (; f_p; f_p = f_p->sibling)
    {
      if (!Fm->file[Fm->curr_wno]->see_dot && f_p->name[0] == '.')
        if (strcmp((char *) f_p->name, ".wastebasket")) continue ;

           if (f_p == Fm->tree.current) type = FT_DIR_OPEN ;
      else                              type = FT_DIR ;

      if (IS_DAMAGED(f_p->Xpos - xoff, f_p->Ypos - yoff,
                     f_p->Xpos - xoff + f_p->width,
                     f_p->Ypos - yoff + Fm->tree.height))
        {
          draw_folder(WNO_TREE, f_p, type, FALSE) ;
          draw_tree_text(f_p) ;
        }

/* Connect node with parent. */

      if (f_p != Fm->tree.head)
        {
          if (Fm->tree_vertical)
            {
              x1 = f_p->Xpos - xoff + (GLYPH_WIDTH >> 1) ;
              y1 = f_p->Ypos - yoff ;
              x2 = f_p->parent->Xpos - xoff + (GLYPH_WIDTH >> 1) ;
              y2 = f_p->parent->Ypos - yoff + (2 * MARGIN) + Fm->tree.height ;
            }
          else
            {
              x1 = f_p->Xpos - xoff ;
              y1 = f_p->Ypos - yoff + (GLYPH_HEIGHT >> 1) ;
              y2 = f_p->parent->Ypos - yoff + (GLYPH_HEIGHT >> 1) ;
              if (f_p->parent->width > GLYPH_WIDTH)
                {
                  x2 = f_p->parent->Xpos - xoff + f_p->parent->width ;
                  if (IS_DAMAGED(x2, y2,
                                 f_p->parent->Xpos - xoff + GLYPH_WIDTH, y2))
                    draw_tree_line(f_p->parent->Xpos - xoff + GLYPH_WIDTH, y2,
                                   x2, y2) ;
                }
              else x2 = f_p->parent->Xpos - xoff + GLYPH_WIDTH ;
            }
          damx1 = (x1 < x2) ? x1 : x2 ;
          damx2 = (x1 < x2) ? x2 : x1 ;
          damy1 = (y1 < y2) ? y1 : y2 ;
          damy2 = (y1 < y2) ? y2 : y1 ;
          if (IS_DAMAGED(damx1, damy1, damx2, damy2))
            draw_tree_line(x1, y1, x2, y2) ;
        }

      if (f_p->child && !(f_p->status & PRUNE))
        display_tree(f_p->child) ;
    }
}


void
do_cleanup_icons()
{
  int n, reply ;
  File_Pane_Object **f_p, **l_p, **s_p ;
  int c, col, incx, incy, ix, iy, max_filespercol, max_filesperline ;
  int r, row, startx, starty, x, y ;
  int wno      = Fm->curr_wno ;
  int selected = (get_first_selection(wno) != NULL) ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Pos *p ;

       if (f->display_mode == VIEW_BY_CONTENT) p = &f->c ;
  else if (f->display_mode == VIEW_BY_ICON)    p = &f->i ;
  else if (f->display_mode == VIEW_BY_LIST)    p = &f->l ;

  incx             = get_pos_attr(wno, FM_POS_INCX) ;
  incy             = get_pos_attr(wno, FM_POS_INCY) ;
  max_filespercol  = get_pos_attr(wno, FM_POS_MAXCOL) ;
  max_filesperline = get_pos_attr(wno, FM_POS_MAXLINE) ;
  startx           = get_pos_attr(wno, FM_POS_STARTX) ;
  starty           = get_pos_attr(wno, FM_POS_STARTY) ;

  set_grid_points(wno) ;
  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    {
      if (selected && (*f_p)->selected == FALSE) continue ;
      x  = get_file_obj_x(wno, *f_p) ;   /* Icon already on a grid point? */
      ix = (x - startx) / incx ;
      y  = get_file_obj_y(wno, *f_p) ;
      iy = (y - starty) / incy ;
      if (((ix*incx + startx) == x) && ((iy*incy + starty) == y)) continue ;
      (void) find_closest_grid_point(wno, *f_p, &row, &col) ;
      animate_icon(wno, f_p - s_p, col*incx + startx, row*incy + starty) ;
      n = (row*max_filesperline) + col ;
      if (n >= 0 && n < max_filesperline*max_filespercol) p->grid[n]++ ;
    }

  redisplay_folder(wno, 0, 0, get_pos_attr(wno, FM_POS_CWIDTH),
                              get_pos_attr(wno, FM_POS_CHEIGHT)) ;
}


void
do_custom_list_deselect(row)
int row ;
{
  int selected = get_list_first_selected(FM_PO_CMD_LIST) ;

  if (selected != -1 && valid_custom_command())
    {
      change_custom_list_item(row) ;
      reset_custom_fields() ;
    }
}


void
do_custom_list_select(cc)
Custom_Command *cc ;
{
  int val ;

  if (cc->command != NULL && *cc->command != '\0')
    set_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE, cc->command) ;
  else
    set_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE, "") ;

  if (cc->alias != NULL && *cc->alias != '\0')
    set_item_str_attr(FM_PO_CMD_MLABEL,  FM_ITEM_IVALUE, cc->alias) ;
  else
    set_item_str_attr(FM_PO_CMD_MLABEL,  FM_ITEM_IVALUE, "") ;

  if (cc->prompt != NULL && *cc->prompt != '\0')
    set_item_str_attr(FM_PO_CMD_PTEXT1,  FM_ITEM_IVALUE, cc->prompt) ;
  else
    set_item_str_attr(FM_PO_CMD_PTEXT1,  FM_ITEM_IVALUE, "") ;

  if (cc->is_prompt == NULL)
    set_item_int_attr(FM_PO_CMD_PROMPT, FM_ITEM_IVALUE, TRUE) ;
  else
    {
      val = EQUAL(cc->is_prompt, "false") ;
      set_item_int_attr(FM_PO_CMD_PROMPT, FM_ITEM_IVALUE, val) ;
    }
  if (cc->is_output == NULL)
    set_item_int_attr(FM_PO_CMD_OUTPUT, FM_ITEM_IVALUE, TRUE) ;
  else
    {
      val = EQUAL(cc->is_output, "false") ;
      set_item_int_attr(FM_PO_CMD_OUTPUT, FM_ITEM_IVALUE, val) ;
    }
  Fm->ignore_custom_desel = FALSE ;
}


void
do_duplicate_file(wno)
int wno ;
{
  File_Pane_Object **f_p ;
  int error = FALSE ;

  fm_busy_cursor(TRUE, wno) ;
  set_timer(0) ;
  for (f_p = PTR_FIRST(wno); f_p < PTR_LAST(wno); f_p++)
    if ((*f_p)->selected)
      if (make_duplicate(wno, f_p, NULL_X, NULL_Y) == FALSE) error = TRUE ;

  if (!error) fm_clrmsg() ;
  fm_display_folder((BYTE) FM_BUILD_FOLDER, wno) ;
  set_timer(Fm->interval) ;
  fm_busy_cursor(FALSE, wno) ;
}


void
do_file_info_popup(wno)
int wno ;
{

/* If the popup already exists, just show it on the screen. */

  if (is_item(FM_FIO_FIO_FRAME))
    {
      fio_update_panel(wno) ;
      set_item_int_attr(FM_FIO_FIO_FRAME, FM_ITEM_SHOW, TRUE) ;
      Fm->fprops_showing = TRUE ;
      if (wno == WNO_TREE)
        Fm->popup_wno[(int) FM_FIO_POPUP] = WNO_TREE ;
      else
        Fm->popup_wno[(int) FM_FIO_POPUP] = Fm->curr_wno ;
      return ;
    }

  fio_make() ;     /* Otherwise, create the popup and position it. */

  fm_position_popup(Fm->curr_wno, FM_FIO_FIO_FRAME) ;
  fio_update_panel(wno) ;
  set_item_int_attr(FM_FIO_FIO_FRAME, FM_ITEM_SHOW, TRUE) ;
  Fm->fprops_showing = TRUE ;
  if (wno == WNO_TREE)
    Fm->popup_wno[(int) FM_FIO_POPUP] = WNO_TREE ;
  else
    Fm->popup_wno[(int) FM_FIO_POPUP] = Fm->curr_wno ;
}


/* Show folder by size of directory or by size of contents. */

void
do_fio_folder_by_content()
{
  char buf[255] ;
  int cval ;

  set_item_int_attr(FM_FIO_FIO_FRAME, FM_ITEM_BUSY, TRUE) ;

/* If contents panel item turned on. */

  if (get_item_int_attr(FM_FIO_CONTENTS, FM_ITEM_IVALUE))
    {
      FILE *fp ;
      char contents[255], *b_p ;

      SPRINTF(buf, "/usr/bin/du -s %s", Fm->fullname) ;
      if ((fp = popen(buf, "r")) != NULL)
        {
          *contents = 0 ;
          while (fgets(contents, 255, fp) && !isdigit(*contents)) continue ;
          PCLOSE(fp) ;
        }

      if ((b_p = strchr((char *) contents, '\t')) != NULL) *b_p = '\0' ;
      SSCANF(contents, "%d", &cval) ;
#ifdef SVR4
      cval /= 2 ;
#endif /*SVR4*/
      SPRINTF(buf, Str[(int) M_KBYTES1], cval) ;
    }
  else
    SPRINTF(buf, Str[(int) M_BYTES],
            fio_add_commas((off_t) Fm->status.st_size)) ;
  set_item_int_attr(FM_FIO_BITE_SIZE, FM_ITEM_INACTIVE, FALSE) ;
  set_item_str_attr(FM_FIO_BITE_SIZE, FM_ITEM_IVALUE, buf) ;
  set_item_int_attr(FM_FIO_FIO_FRAME, FM_ITEM_BUSY, FALSE) ;
  set_item_int_attr(FM_FIO_CONTENTS,  FM_ITEM_SHOW, TRUE) ;
}


void
do_panning(ec)      /* Handle panning in the file and tree panes. */
Event_Context ec ;
{
  int do_X_pan, do_Y_pan, hinc, vinc, x_delta, y_delta ;
  static int x_pcoff, x_polen, x_pvlen, x_offset ;
  static int y_pcoff, y_polen, y_pvlen, y_offset ;
  int wno = ec->wno ;

  hinc = (wno == WNO_TREE) ? Fm->Hscroll_unit : get_pos_attr(wno, FM_POS_INCX) ;
  vinc = (wno == WNO_TREE) ? Fm->Vscroll_unit : get_pos_attr(wno, FM_POS_INCY) ;

/*  Create the pixmap which is used for folder panning, if we haven't
 *  already done it, and copy the currently visible portion to it.
 */

  if (!is_panpix())
    {
      int pixh, pixw, pixx, pixy ;

      fm_set_pan_cursor(wno, TRUE) ;

      pixw = get_canvas_attr(wno, FM_WIN_WIDTH)  - sb_width(wno) ;
      pixh = get_canvas_attr(wno, FM_WIN_HEIGHT) - sb_width(wno) ;
      fm_make_panpix(pixw, pixh) ;

      pixx = get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_VIEW_ST) * hinc ;
      pixy = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_ST) * vinc ;
      fm_pane_to_pixmap(wno, pixx, pixy) ;

      x_offset = 0 ;
      x_pcoff = get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_VIEW_ST) ;
      x_polen = get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_OBJ_LEN) ;
      x_pvlen = get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_VIEW_LEN) ;

      y_offset = 0 ;
      y_pcoff = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_ST) ;
      y_polen = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_OBJ_LEN) ;
      y_pvlen = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_LEN) ;

      Fm->TLx = 0 ;
      Fm->TLy = 0 ;
      Fm->BRx = get_scrollbar_attr(wno, FM_H_SBAR, FM_SB_OBJ_LEN) * hinc ;
      Fm->BRy = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_OBJ_LEN) * vinc ;
    }

  do_X_pan = TRUE ;
  x_delta = ec->x - ec->old_x ;
  x_offset += x_delta ;
       if (x_delta == 0) do_X_pan = FALSE ;
  else if (x_delta > 0 && x_offset > 0 && x_pcoff == 0)
    {
      x_offset -= x_delta ;
      do_X_pan = FALSE ;
    }
  else if (x_delta < 0 && x_offset < 0 && (x_pcoff + x_pvlen) == x_polen)
    {
      x_offset -= x_delta ;
      do_X_pan = FALSE ;
    }

  do_Y_pan = TRUE ;
  y_delta = ec->y - ec->old_y ;
  y_offset += y_delta ;
       if (y_delta == 0) do_Y_pan = FALSE ;
  else if (y_delta > 0 && y_offset > 0 && y_pcoff == 0)
    {
      y_offset -= y_delta ;
      do_Y_pan = FALSE ;
    }
  else if (y_delta < 0 && y_offset < 0 && (y_pcoff + y_pvlen) == y_polen)
    {
      y_offset -= y_delta ;
      do_Y_pan = FALSE ;
    }

  if (do_X_pan == TRUE)
    {
      Fm->panx1 = Fm->panx2 ;
      Fm->panx2 += ec->x - ec->old_x ;
      ec->old_x = ec->x ;
    }
  if (do_Y_pan == TRUE)
    {
      Fm->pany1 = Fm->pany2 ;
      Fm->pany2 += ec->y - ec->old_y ;
      ec->old_y = ec->y ;
    }
  if (do_X_pan == TRUE || do_Y_pan == TRUE) fm_update_pan(wno) ;
}


void
do_show_tree(force_open, tofront)
int force_open, tofront ;
{
  if (!is_frame(WNO_TREE)) fm_create_tree_canvas() ;
  Fm->treeview = TRUE ;
  if (force_open && is_frame_closed(WNO_TREE))
    set_frame_attr(WNO_TREE, FM_FRAME_CLOSED, FALSE) ;
  if (tofront) set_frame_attr(WNO_TREE, FM_FRAME_SHOW, TRUE) ;

  if (Fm->tree.head && Fm->tree.head != Fm->tree.root)
    {

/*  Is the current folder a descendent of the current root?  If not, then
 *  change the root back to '/'.
 */

      if (!fm_descendant(Fm->tree.current, Fm->tree.head))
        {
          Fm->tree.head->sibling = Fm->tree.sibling ;
          Fm->tree.head          = Fm->tree.root ;
        }
    }
  fm_drawtree(TRUE) ;
  fm_visiblefolder(path_to_node(WNO_TREE, Fm->file[Fm->curr_wno]->path)) ;
}


void
error(bell, msg, arg)
char bell, *msg, *arg ;
{
  char buffer[256] ;

  if (arg)
    {
      SPRINTF(buffer, msg, arg) ;
      msg = buffer ;
    }
  write_item_footer(FM_FIND_FIND_FRAME, msg, bell) ;
}


/* Execute a given file's drop method. */

Boolean
exec_drop_method(src, destdir, dest, drop_method, copy)
char *src ;              /* Dragged object's filename. */
char *destdir ;          /* Destination pathname of current dir. */
char *dest ;             /* Destination file we are over. */
char *drop_method ;      /* Drop method to execute. */
Boolean copy ;           /* Copy or move the file? */
{
  FILE *fp, *pp ;             /* File and pipe ptrs. */
  char buf[XFER_BUFSIZ] ;     /* Dummy buffer to hold char strings. */
  int count ;                 /* Number of bytes read. */

  SPRINTF(buf, "%s/%s", destdir, dest) ;
  if (!strcmp(src, buf))
    {
      fm_msg(TRUE, Str[(int) M_DND_SAME]) ;
      return(FALSE) ;
    }

  fp = fopen(src, "r") ;      /* Open src file. */
  if (!fp)
    {
      fm_msg(TRUE, Str[(int) M_NO_SOURCE], src) ;
      return(FALSE) ;
    }

/* Setup drop method for pipe. */

  STRCPY(buf, drop_method) ;
  expand_filename(buf, dest) ;

  pp = popen(buf, "w") ;      /* Open pipe to output file to. */
  if (!pp)
    {
      fm_msg(TRUE, Str[(int) M_START_FILTER], buf, dest) ;
      if (fp) fclose(fp) ;
      return(FALSE) ;
    }

/* Send bits. */

  while ((count = fread(buf, 1, XFER_BUFSIZ, fp)) != 0)
    (void) fwrite(buf, 1, count, pp) ;

/* Close file and pipe. */

  FCLOSE(fp) ;
  PCLOSE(pp) ;

/* Remove old object if a not a copy operation. */

  if (!copy) UNLINK(src) ;

/*  XXX: overkill -- we do not have an easy way to erase inverted
 *       image, so we are using an expensive routine to do it.  Need to
 *       find some way other than draw_ith() to invert a file pane item.
 */

  display_all_folders(FM_BUILD_FOLDER, destdir) ;
  return(TRUE) ;
}


#define  HUGE_DISTANCE 1000000

int
find_closest_grid_point(wno, f_p, row, col)
int wno, *row, *col ;
File_Pane_Object *f_p ;
{
  int c, d, distance, incx, incy, max_filespercol, max_filesperline, r ;
  int startx, starty, x, y ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Pos *p ;

       if (f->display_mode == VIEW_BY_CONTENT) p = &f->c ;
  else if (f->display_mode == VIEW_BY_ICON)    p = &f->i ;
  else if (f->display_mode == VIEW_BY_LIST)    p = &f->l ;

  incx             = get_pos_attr(wno, FM_POS_INCX) ;
  incy             = get_pos_attr(wno, FM_POS_INCY) ;
  max_filespercol  = get_pos_attr(wno, FM_POS_MAXCOL) ;
  max_filesperline = get_pos_attr(wno, FM_POS_MAXLINE) ;
  startx           = get_pos_attr(wno, FM_POS_STARTX) ;
  starty           = get_pos_attr(wno, FM_POS_STARTY) ;
  x                = get_file_obj_x(wno, f_p) ;
  y                = get_file_obj_y(wno, f_p) ;

  distance = HUGE_DISTANCE ;       /* Some impossibly large value. */
  for (r = 0; r < max_filespercol; r++)
    for (c = 0; c < max_filesperline; c++)
      if (p->grid[r*max_filesperline + c] == 0)
        {
          d = (abs(((c*incx) + startx) - x) << 1) +
              (abs(((r*incy) + starty) - y) << 1) ;
          if (d < distance)
            {
              distance = d ;
              *row     = r ;
              *col     = c ;
            }
        }
  return(distance != HUGE_DISTANCE) ;
}


void
find_make()
{
  int tab, sb_wid ;

  fm_popup_create(FM_FIND_POPUP) ;
  link_find_items() ;

  fm_add_help(FM_FIND_FIND_PANEL,     "filemgr:Find_Panel") ;
  fm_add_help(FM_FIND_FROMITEM,       "filemgr:Findfrom") ;
  fm_add_help(FM_FIND_NAMEITEM,       "filemgr:Findname") ;
  fm_add_help(FM_FIND_NAMETOGGLE,     "filemgr:Findnametoggle") ;
  fm_add_help(FM_FIND_OWNERITEM,      "filemgr:Findowner") ;
  fm_add_help(FM_FIND_OWNERTOGGLE,    "filemgr:Findownertoggle") ;
  fm_add_help(FM_FIND_AFTERITEM,      "filemgr:Findafter") ;
  fm_add_help(FM_FIND_AFTER_DATE_PI,  "filemgr:Findafter") ;
  fm_add_help(FM_FIND_BEFOREITEM,     "filemgr:Findbefore") ;
  fm_add_help(FM_FIND_BEFORE_DATE_PI, "filemgr:Findbefore") ;
  fm_add_help(FM_FIND_TYPEITEM,       "filemgr:Findtype") ;
  fm_add_help(FM_FIND_PATTERNITEM,    "filemgr:Findpattern") ;
  fm_add_help(FM_FIND_CASETOGGLE,     "filemgr:Findcase") ;
  fm_add_help(FM_FIND_FIND_BUTTON,    "filemgr:Findfind") ;
  fm_add_help(FM_FIND_OPEN_BUTTON,    "filemgr:Findopen") ;
  fm_add_help(FM_FIND_STOP_BUTTON,    "filemgr:Findstop") ;

  set_item_str_attr(FM_FIND_FROMITEM, FM_ITEM_IVALUE,
                    (char *) Fm->file[Fm->curr_wno]->path) ;
  set_item_int_attr(FM_FIND_TYPEITEM, FM_ITEM_IVALUE, FLDR + DOC + APP) ;

/* Get correct positions for the five items on the right side of the popup. */

  set_item_int_attr(FM_FIND_NAMETOGGLE,     FM_ITEM_Y,
                    get_item_int_attr(FM_FIND_NAMEITEM,    FM_ITEM_Y)-6) ;
  set_item_int_attr(FM_FIND_OWNERTOGGLE,    FM_ITEM_Y,
                    get_item_int_attr(FM_FIND_OWNERITEM,   FM_ITEM_Y)-6) ;
  set_item_int_attr(FM_FIND_AFTER_DATE_PI,  FM_ITEM_Y,
                    get_item_int_attr(FM_FIND_AFTERITEM,   FM_ITEM_Y)) ;
  set_item_int_attr(FM_FIND_BEFORE_DATE_PI, FM_ITEM_Y,
                    get_item_int_attr(FM_FIND_BEFOREITEM,  FM_ITEM_Y)) ;
  set_item_int_attr(FM_FIND_CASETOGGLE,     FM_ITEM_Y,
                    get_item_int_attr(FM_FIND_PATTERNITEM, FM_ITEM_Y)-6) ;

/* Position those items which are to the right of the text items. */

  tab = get_item_int_attr(FM_FIND_NAMEITEM,   FM_ITEM_X) +
        get_item_int_attr(FM_FIND_NAMEITEM,   FM_ITEM_WIDTH) +
        get_item_int_attr(FM_FIND_FIND_PANEL, FM_ITEM_X_GAP) ;
  set_item_int_attr(FM_FIND_NAMETOGGLE,     FM_ITEM_X, tab) ;
  set_item_int_attr(FM_FIND_OWNERTOGGLE,    FM_ITEM_X, tab) ;
  set_item_int_attr(FM_FIND_AFTER_DATE_PI,  FM_ITEM_X, tab) ;
  set_item_int_attr(FM_FIND_BEFORE_DATE_PI, FM_ITEM_X, tab) ;
  set_item_int_attr(FM_FIND_CASETOGGLE,     FM_ITEM_X, tab) ;

  set_item_int_attr(FM_FIND_FIND_PANEL, FM_ITEM_FIT, 0) ;
  set_item_int_attr(FM_FIND_FIND_FRAME, FM_ITEM_FIT, 0) ;
  fm_resize_text_item(FM_FIND_FIND_PANEL, FM_FIND_FROMITEM) ;

/* Set the width of the scrolling list as wide as the panel. */

  sb_wid = sb_width(Fm->curr_wno) ;
  set_item_int_attr(FM_FIND_FIND_LIST, FM_ITEM_X, 0) ;
  set_item_int_attr(FM_FIND_FIND_LIST, FM_ITEM_LWIDTH,
            get_item_int_attr(FM_FIND_FIND_PANEL, FM_ITEM_WIDTH) - sb_wid) ;
}


/* Redisplay folder pane at new position, after a possible pan, and tidyup. */

void
finish_panning(ec)
Event_Context ec ;
{
  int hinc, vinc, wno, x, y ;

  wno = ec->wno ;
  Fm->panx1 = Fm->pany1 = Fm->panx2 = Fm->pany2 = 0 ;
  if ((ec->start_x == ec->x) && (ec->start_x == ec->y))
    return ;                                             /* No movement. */

  hinc = (wno == WNO_TREE) ? Fm->Hscroll_unit : get_pos_attr(wno, FM_POS_INCX) ;
  vinc = (wno == WNO_TREE) ? Fm->Vscroll_unit : get_pos_attr(wno, FM_POS_INCY) ;

  x = (ec->x - ec->start_x) / hinc ;
  y = (ec->y - ec->start_y) / vinc ;

  fm_adjust_sb(wno, FM_H_SBAR, x) ;
  fm_adjust_sb(wno, FM_V_SBAR, y) ;

  free_panpix_image() ;
  fm_set_pan_cursor(wno, FALSE) ;
}


void
fio_make()
{
  int x, y ;

  fm_popup_create(FM_FIO_POPUP) ;
  link_fio_items() ;

  fm_add_help(FM_FIO_FI_PANEL,     "filemgr:Info_Panel") ;
  fm_add_help(FM_FIO_FILE_NAME,    "filemgr:Info_File_Name") ;
  fm_add_help(FM_FIO_OWNER,        "filemgr:Info_Owner") ;
  fm_add_help(FM_FIO_GROUP,        "filemgr:Info_Group") ;
  fm_add_help(FM_FIO_BITE_SIZE,    "filemgr:Info_Byte_Size") ;
  fm_add_help(FM_FIO_MOD_TIME,     "filemgr:Info_Modify_Time") ;
  fm_add_help(FM_FIO_ACCESS_TIME,  "filemgr:Info_Access_Time") ;
  fm_add_help(FM_FIO_FILE_TYPE,    "filemgr:Info_File_Type") ;
  fm_add_help(FM_FIO_PERMISSIONS,  "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_PERM_READ,    "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_PERM_WRITE,   "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_PERM_EXE,     "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_OWNER_PERM,   "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_GROUP_PERM,   "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_WORLD_PERM,   "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_OPEN_METHOD,  "filemgr:Info_Open_Method") ;
  fm_add_help(FM_FIO_PRINT_METHOD, "filemgr:Info_Print_Method") ;
  fm_add_help(FM_FIO_MOUNT_POINT,  "filemgr:Info_Mount_Point") ;
  fm_add_help(FM_FIO_MOUNT_FROM,   "filemgr:Info_Mount_From") ;
  fm_add_help(FM_FIO_FREE_SPACE,   "filemgr:Info_Free_Space") ;
  fm_add_help(FM_FIO_ICON_ITEM,    "filemgr:Info_Icon") ;
  fm_add_help(FM_FIO_CONTENTS,     "filemgr:Info_Contents") ;
  fm_add_help(FM_FIO_APPLY_BUTTON, "filemgr:apply") ;
  fm_add_help(FM_FIO_RESET_BUTTON, "filemgr:reset") ;
  fm_add_help(FM_FIO_MORE_BUTTON,  "filemgr:Info_More") ;

  fm_add_help(FM_FIO_OWNER_R, "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_OWNER_W, "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_OWNER_E, "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_GROUP_R, "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_GROUP_W, "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_GROUP_E, "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_WORLD_R, "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_WORLD_W, "filemgr:Info_Permissions") ;
  fm_add_help(FM_FIO_WORLD_E, "filemgr:Info_Permissions") ;

  set_default_item(FM_FIO_FI_PANEL, FM_FIO_APPLY_BUTTON) ;
  init_toggles() ;

  y = get_item_int_attr(FM_FIO_OWNER_PERM, FM_ITEM_Y) - 5 ;
  set_item_int_attr(FM_FIO_OWNER_R, FM_ITEM_PANELY, y) ;
  set_item_int_attr(FM_FIO_OWNER_W, FM_ITEM_PANELY, y) ;
  set_item_int_attr(FM_FIO_OWNER_E, FM_ITEM_PANELY, y) ;

  y = get_item_int_attr(FM_FIO_GROUP_PERM, FM_ITEM_Y) - 5 ;
  set_item_int_attr(FM_FIO_GROUP_R, FM_ITEM_PANELY, y) ;
  set_item_int_attr(FM_FIO_GROUP_W, FM_ITEM_PANELY, y) ;
  set_item_int_attr(FM_FIO_GROUP_E, FM_ITEM_PANELY, y) ;

  y = get_item_int_attr(FM_FIO_WORLD_PERM, FM_ITEM_Y) - 5 ;
  set_item_int_attr(FM_FIO_WORLD_R, FM_ITEM_PANELY, y) ;
  set_item_int_attr(FM_FIO_WORLD_W, FM_ITEM_PANELY, y) ;
  set_item_int_attr(FM_FIO_WORLD_E, FM_ITEM_PANELY, y) ;

  fio_set_panel_x_values() ;   /* Set X coordinates of panel items */

/* Resize text items to extend to the width of the panel. */

  fm_resize_text_item(FM_FIO_FI_PANEL, FM_FIO_FILE_NAME) ;
  fm_resize_text_item(FM_FIO_FI_PANEL, FM_FIO_OWNER) ;
  fm_resize_text_item(FM_FIO_FI_PANEL, FM_FIO_GROUP) ;
  fm_resize_text_item(FM_FIO_FI_PANEL, FM_FIO_OPEN_METHOD) ;
  fm_resize_text_item(FM_FIO_FI_PANEL, FM_FIO_PRINT_METHOD) ;
  fm_resize_text_item(FM_FIO_FI_PANEL, FM_FIO_MOUNT_POINT) ;
  fm_resize_text_item(FM_FIO_FI_PANEL, FM_FIO_MOUNT_FROM) ;

  fm_center_info_buttons(FM_FIO_FI_PANEL,
                         FM_FIO_APPLY_BUTTON, FM_FIO_RESET_BUTTON) ;

  x = get_item_int_attr(FM_FIO_FI_PANEL, FM_ITEM_WIDTH) -
      get_item_int_attr(FM_FIO_MORE_BUTTON, FM_ITEM_WIDTH) - 10 ;
  set_item_int_attr(FM_FIO_MORE_BUTTON, FM_ITEM_X, x) ;

  do_fi_basic() ;
}


void
fio_set_panel_x_values()
{
  int col_x, icon_wid, left_x, tmp_x, w, x, y ;

/* Get the X coordinate items are centered around. */

  col_x = get_item_int_attr(FM_FIO_FILE_NAME, FM_ITEM_VALX) ;

/* Find the icon width. */

  icon_wid = GLYPH_WIDTH ;      /* Assuming icon width. */

/* Find X coordinate of left-most label of the 1st 3 items. */

  left_x = get_item_int_attr(FM_FIO_FILE_NAME, FM_ITEM_X) ;
  tmp_x  = get_item_int_attr(FM_FIO_OWNER,     FM_ITEM_X) ;
  if (tmp_x < left_x) left_x = tmp_x ;

  tmp_x = get_item_int_attr(FM_FIO_GROUP, FM_ITEM_X) ;
  if (tmp_x < left_x) left_x = tmp_x ;

/* If no room for 2 icon's, then increase column starting point. */

  if (left_x < (2*icon_wid))
    {
      col_x += (2*icon_wid) - left_x ;
      left_x = 2 * icon_wid ;
    }

/* Set the X coord of the icon to the middle of the empty space. */

  tmp_x = (left_x / 2) - (icon_wid / 2) ;
  set_item_int_attr(FM_FIO_ICON_ITEM, FM_ITEM_X, tmp_x) ;

/* Adjust the position of all items on the panel appropriately. */

  set_item_int_attr(FM_FIO_FILE_NAME,    FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_OWNER,        FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_GROUP,        FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_PERMISSIONS,  FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_OPEN_METHOD,  FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_PRINT_METHOD, FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_MOUNT_POINT,  FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_MOUNT_FROM,   FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_BITE_SIZE,    FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_MOD_TIME,     FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_ACCESS_TIME,  FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_FILE_TYPE,    FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_FREE_SPACE,   FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_OWNER_PERM,   FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_GROUP_PERM,   FM_ITEM_VALX, col_x) ;
  set_item_int_attr(FM_FIO_WORLD_PERM,   FM_ITEM_VALX, col_x) ;

  x = get_item_int_attr(FM_FIO_PERM_READ, FM_ITEM_X);
  set_item_int_attr(FM_FIO_OWNER_R, FM_ITEM_PANELX, x) ;
  set_item_int_attr(FM_FIO_GROUP_R, FM_ITEM_PANELX, x) ;
  set_item_int_attr(FM_FIO_WORLD_R, FM_ITEM_PANELX, x) ;

  x = get_item_int_attr(FM_FIO_PERM_WRITE, FM_ITEM_X);
  set_item_int_attr(FM_FIO_OWNER_W, FM_ITEM_PANELX, x) ;
  set_item_int_attr(FM_FIO_GROUP_W, FM_ITEM_PANELX, x) ;
  set_item_int_attr(FM_FIO_WORLD_W, FM_ITEM_PANELX, x) ;

  x = get_item_int_attr(FM_FIO_PERM_EXE, FM_ITEM_X);
  set_item_int_attr(FM_FIO_OWNER_E, FM_ITEM_PANELX, x) ;
  set_item_int_attr(FM_FIO_GROUP_E, FM_ITEM_PANELX, x) ;
  set_item_int_attr(FM_FIO_WORLD_E, FM_ITEM_PANELX, x) ;

/* Position toggle to the right of the BITE_SIZE value item. */

  x = get_item_int_attr(FM_FIO_BITE_SIZE, FM_ITEM_VALX) ;
  y = get_item_int_attr(FM_FIO_BITE_SIZE, FM_ITEM_Y) ;

/* Set PANEL_VALUE so there will be a width. */

  w = get_panel_item_value_width(FM_FIO_BITE_SIZE, Str[(int) M_XXXXXXXXX]) ;

  set_item_int_attr(FM_FIO_CONTENTS,  FM_ITEM_X,   x + w) ;
  set_item_int_attr(FM_FIO_CONTENTS,  FM_ITEM_Y,   y) ;
  set_item_int_attr(FM_FIO_FI_PANEL,  FM_ITEM_FIT, 0) ;
  set_item_int_attr(FM_FIO_FIO_FRAME, FM_ITEM_FIT, 0) ;
}


void
fm_adjust_sb(wno, stype, delta)
int wno, delta ;
enum fm_sb_type stype ;
{
  int coff, olen, redraw, val, vlen ;

  redraw = FALSE ;
  coff   = get_scrollbar_attr(wno, stype, FM_SB_VIEW_ST) ;
  olen   = get_scrollbar_attr(wno, stype, FM_SB_OBJ_LEN) ;
  vlen   = get_scrollbar_attr(wno, stype, FM_SB_VIEW_LEN) ;
       if (delta == 0) redraw = TRUE ;
  else if (delta > 0)
    {
           if (coff == 0)    redraw = TRUE ;
      else if (coff < delta) val = 0 ;
      else                   val = coff - delta ;
    }
  else if (delta < 0)
    {
           if ((coff + vlen) == olen)        redraw = TRUE ;
      else if ((coff + vlen - delta) > olen) val = olen - vlen ;
      else                                   val = coff - delta ;
    }
  if (redraw == TRUE)
    {
      if (wno == WNO_TREE)
        {
          clear_area(wno, Fm->tree.r_left, Fm->tree.r_top,
                          Fm->tree.r_width, Fm->tree.r_height) ;
          fm_drawtree(FALSE) ;
        }
      else fm_display_folder(FM_DISPLAY_FOLDER, wno) ;
    }
  else
    {
      if (wno == WNO_TREE) Fm->StateChanged = TRUE ;
      Fm->panning = TRUE ;
      fm_scrollbar_scroll_to(wno, stype, val) ;
    }
}


void
fm_canvas_page_scroll(wno, action)
int wno ;
enum action_type action ;
{
  int olen, pos, vlen, vstart ;

  vlen   = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_LEN) ;
  vstart = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_ST) ;
  olen   = get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_OBJ_LEN) ;

  switch (action)
    {
      case FM_HOME : pos = 0 ;                      /* Home key. */
                     break ;
      case FM_END  : pos = olen ;                   /* End  key. */
                     break ;
      case FM_PGDN : pos = vstart + vlen ;          /* PgDn key. */
                     if (pos > olen) pos = olen ;
                     break ;
      case FM_PGUP : pos = vstart - vlen ;          /* PgUp key. */
                     if (pos < 0) pos = 0 ;
    }
  fm_scrollbar_scroll_to(wno, FM_V_SBAR, pos) ;
}


void
fm_file_scroll_width(wno)         /* Set scroll increment. */
int wno ;
{
  int inc   = get_pos_attr(wno,    FM_POS_INCX) ;
  int width = get_canvas_attr(wno, FM_WIN_WIDTH) ;

  if (inc)
    {
      set_scrollbar_attr(wno, FM_H_SBAR, FM_SB_VIEW_LEN, width / inc) ;
      set_scrollbar_attr(wno, FM_H_SBAR, FM_SB_LINE_HT, inc) ;
    }
}


void
fm_file_scroll_height(wno)        /* Set scroll increment. */
int wno ;
{
  int inc    = get_pos_attr(wno,    FM_POS_INCY) ;
  int height = get_canvas_attr(wno, FM_WIN_HEIGHT) ;

  if (inc)
    {
      set_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_LEN, height / inc) ;
      set_scrollbar_attr(wno, FM_V_SBAR, FM_SB_LINE_HT, inc) ;
    }
}


/*  Read in the contents of a 1bit deep X bitmap file, converting it to
 *  drawable, and reducing it in size, to be less then 64x64 pixels.
 */

unsigned long
get_bitmap_content(path)
char *path ;
{
  FILE *fp ;
  char nextline[MAXPATHLEN] ;
  int count ;                  /* Number of items to read from bitmap file. */
  int height = -1 ;
  int value ;
  int width = -1 ;
  int slength ;                /* Length in bytes of a scan line. */
  int srcsize ;                /* Complete image size in bytes. */
#if !defined(__ppc)
  char c, *cptr ;
#else
  int c;
  char *cptr ;
#endif /* __ppc */
  unsigned char c1, c2 ;       /* Temporary locations for latest 8 bits. */
  unsigned char *src, vbuf[512] ;

  if ((fp = fopen(path, "r")) == NULL) return((unsigned long) NULL) ;
  for (;;)
    {
      if (fgets(nextline, MAXPATHLEN, fp) == NULL) goto err_end ;

      if (sscanf(nextline, "#define %s %d", vbuf, &value) == 2)
        {
          if ((cptr = strrchr((char *) vbuf, '_')) == NULL)
            cptr = (char *) vbuf ;
          else
            cptr++ ;
          if (!strcmp(cptr, "height")) height = value ;
          if (!strcmp(cptr, "width"))  width  = value ;
        }

      if (sscanf(nextline, "static %s %*[^{]%c", vbuf, &c) == 2)
        {
          if (width == -1 || height == -1) return((unsigned long) NULL) ;
          if (strcmp((char *) vbuf, "char") == 0)
            {
              slength = (width + 7) / 8 ;
              srcsize = slength * height ;
              if ((src = (unsigned char *) malloc((unsigned) srcsize)) == NULL)
                {
                  if (fp) FCLOSE(fp) ;
                  return((unsigned long) NULL) ;
                }
              MEMSET((char *) src, 0, srcsize) ;

              count = 0 ;
              while (count < srcsize)
                {
                  c = fscanf(fp, " 0x%c%c,", &c1, &c2) ;
                  if (c == 0 || c == EOF) break ;
                  src[count++] = (unsigned char)
                                 revtable[get_hex(c1)*16 + get_hex(c2)] ;
                }
              FCLOSE(fp) ;
              return(compress_image(src, srcsize, width, height, slength)) ;
            }
        }
    }

err_end:

  FCLOSE(fp) ;
  return((unsigned long) NULL) ;
}


char *
get_default_drop_method(type)    /* Get the default file's drop method. */
int type ;
{
  CE_ENTRY entry = NULL ;
  char *str = NULL ;

/* XXX -- need to add FT_BADLINK, etc. to CE */

       if (type == FT_DIR) entry = Fm->ceo->generic_dir ;
  else if (type == FT_APP) entry = Fm->ceo->generic_app ;
  else                     entry = Fm->ceo->generic_doc ;

  str = get_tns_attr(entry, Fm->ceo->type_drop) ;
  if (!str)
    CEDB("CE- no default tns drop method found\n") ;

  return(str) ;
}


char *
get_default_type_name(type)    /* Get the file's default type name entry. */
int type ;
{
  char *str = NULL ;
  CE_ENTRY entry = NULL ;

/* XXX -- need to add FT_BADLINK, etc. to CE */

       if (type == FT_DIR) entry = Fm->ceo->generic_dir ;
  else if (type == FT_APP) entry = Fm->ceo->generic_app ;
  else                     entry = Fm->ceo->generic_doc ;

  str = get_tns_attr(entry, Fm->ceo->type_name) ;
  if (!str)
    CEDB("CE- no default tns type name found\n") ;

  return(str) ;
}


char *
get_drop_method(fpo)              /* Get the file's drop method. */
File_Pane_Object *fpo ;
{
  char *str = NULL ;

  str = get_tns_attr(fpo->tns_entry, Fm->ceo->type_drop) ;
  if (!str)
    {
      CEDB("CE- no tns drop method found\n") ;
      str = get_default_drop_method( fpo->type ) ;
    }

  return(str) ;
}


char *
get_file_template(fpo, filename)        /* Get the file's file template. */
File_Pane_Object *fpo ;
char *filename ;
{
  char *str = NULL ;

  filename[0] = '\0' ;

  str = get_tns_attr(fpo->tns_entry, Fm->ceo->type_template) ;
  if (!str)
    {
      CEDB("CE- no tns file template found\n") ;
      STRCPY(filename, "NoName%t") ;
    }
  else STRCPY(filename, str) ;

  return(filename) ;
}


/*  Read in the contents of a 1bit deep Sun icon file, converting it to
 *  drawable, and reducing it in size, to be less then 64x64 pixels.
 */

unsigned long
get_icon_content(path)
char *path ;
{
  unsigned char *src ;
  FILE *fp ;
  char htype[MAXPATHLEN] ;        /* Current header comment parameter. */
  int c ;                         /* Count of items found from fscanf call. */
  int comment = TRUE ;            /* Set if more initial comment to read. */
  int count ;                     /* Number of items to read from icon file. */
  int idepth  = -1 ;              /* Depth of this icon. */
  int iheight = -1 ;              /* Height of this icon. */
  int iwidth  = -1 ;              /* Width of this icon. */
  int ivbpi   = -1 ;              /* Number of valid bits per item. */
  int slength ;                   /* Length in bytes of a scan line. */
  int srcsize ;                   /* Complete image size in bytes. */
  int start_comment = FALSE ;     /* Sanity check attempt. */
  unsigned char c1, c2, c3, c4 ;  /* Temporary locations for latest 16 bits. */

  if ((fp = fopen(path, "r")) == NULL) return((unsigned long) NULL) ;

  if (fgetc(fp) != '/')
    {
      FCLOSE(fp) ;
      return((unsigned long) NULL) ;
    }
  ungetc('/', fp) ;

/*  Start_comment is an attempt at sanity checking, for when you pass
 *  get_icon_content binary files. It is assumed that with a valid icon
 *  file, you will find an '*' (part of the start of comment) first, before
 *  you get a W, H, D or V parameter. If this doesn't happen, return NULL.
 */

  while (comment == TRUE)
    {
      if (fscanf(fp, "%*[^WHDV*]%s", htype) == EOF) break ;
      switch (htype[0])
        {
          case 'W' : if (!start_comment)
                       {
                         FCLOSE(fp) ;
                         return((unsigned long) NULL) ;
                       }
                     if (sscanf(htype, "Width=%d", &iwidth) != 1) break ;
                     break ;
          case 'H' : if (!start_comment)
                       {
                         FCLOSE(fp) ;
                         return((unsigned long) NULL) ;
                       }
                     if (sscanf(htype, "Height=%d", &iheight) != 1) break ;
                     break ;
          case 'D' : if (!start_comment)
                       {
                         FCLOSE(fp) ;
                         return((unsigned long) NULL) ;
                       }
                     if (sscanf(htype, "Depth=%d", &idepth) != 1) break ;
                     if (idepth != 1)
                       {
                         FCLOSE(fp) ;
                         return((unsigned long) NULL) ;
                       }
                     break ;
          case 'V' : if (!start_comment)
                       {
                         FCLOSE(fp) ;
                         return((unsigned long) NULL) ;
                       }
                     if (sscanf(htype, "Valid_bits_per_item=%d", &ivbpi) != 1)
                       break ;
                     if (ivbpi != 16)
                       {
                         FCLOSE(fp) ;
                         return((unsigned long) NULL) ;
                       }
                     break ;
          case '*' : start_comment = TRUE ;
                     if (htype[1] == '/') comment = FALSE ;
        }
    }

  if (idepth == -1 || iheight == -1 || iwidth == -1 || ivbpi == -1)
    {
      FCLOSE(fp) ;
      return((unsigned long) NULL) ;
    }

  slength = ((iwidth + 15) >> 3) &~ 1 ;
  srcsize = slength * iheight ;
  if ((src = (unsigned char *) malloc((unsigned) srcsize)) == NULL)
    {
      FCLOSE(fp) ;
      return((unsigned long) NULL) ;
    }
  MEMSET((char *) src, 0, srcsize) ;

  count = 0 ;
  while (count < srcsize)
    {
      c = fscanf(fp, " 0x%c%c%c%c,", &c1, &c2, &c3, &c4) ;
      if (c == 0 || c == EOF) break ;
      src[count++] = (unsigned char) (get_hex(c1)*16 + get_hex(c2)) ;
      src[count++] = (unsigned char) (get_hex(c3)*16 + get_hex(c4)) ;
    }
  FCLOSE(fp) ;
  return(compress_image(src, srcsize, iwidth, iheight, slength)) ;
}


char *
get_open_method(fpo)                  /* Get the file's open method. */
File_Pane_Object *fpo ;
{
  char *method = NULL ;

/*  Get the icon's open method from the CE.  If this fails, then use the
 *  default open method defined in the tool props.
 */

  method = get_tns_attr(fpo->tns_entry, Fm->ceo->type_open) ;
  if (!method)
    {
      CEDB("CE- no tns open method found\n") ;
      method = get_default_open_method(fpo, FALSE) ;
    }

  return(method) ;
}


char *
get_print_method(fpo)                 /* Get the file's print method. */
File_Pane_Object *fpo ;
{
  char *method = NULL ;

/*  Get the icon's open method from the CE.  If this fails, then use the
 *  default open method defined in the tool props.
 */

  method = get_tns_attr(fpo->tns_entry, Fm->ceo->type_print) ;
  if (!method)
    {
      CEDB("CE- no tns print method found\n") ;
      method = get_default_print_method() ;
    }

  return(method) ;
}


char *
get_tns_attr(entry, attr)    /* Get the given attribute value from the CE. */
CE_ENTRY entry ;
CE_ATTRIBUTE attr ;
{
  char *str = NULL ;

  if (Fm->ceo->running && entry && attr)
    str = (char *) ce_get_attribute(Fm->ceo->type_ns, entry, attr) ;

  return(str) ;
}


CE_ENTRY
get_tns_entry(fpo, name, bufsize)
File_Pane_Object *fpo ;
char *name ;                    /* Filename. */
int bufsize ;                   /* How much data to read from file for CE. */
{
  int fd ;                      /* File descriptor. */
  CE_ENTRY entry ;
  char *fname ;
  char *type ;

  if (!Fm->ceo->running) return(NULL) ;

/* Get the last component of the name. */

  if ((fname = strrchr(name, '/')) != NULL) fname++ ;
  else                                      fname = name ;

  CEDB1("\nCE- checking filename = (%s)\n", fname) ;

/*  Get the starting buffer of the file.  By opening, we have updated
 *  the file's access time; we could reset this by remembering the access
 *  time and restoring it with utime().  But, for efficiency's sake, we
 *  will avoid two extra syscalls on each file.
 */

/*  If the CE buffer hasn't been allocated yet (ie. first time though), then
 *  make space for it.
 */

  if (Fm->cebuf == NULL)
    {
      if ((Fm->cebuf = (char *)
           LINT_CAST(fm_malloc((unsigned) Fm->ce_bufsize))) == NULL)
        return(NULL) ;
    }

/*  Opening character special devices like /dev/console will hang your
 *  system, so check the type of file first!
 */

  if (bufsize)
    {
      Fm->cebuf[0] = 0 ;
      if (fpo->type != FT_SYS && ((fd = open(name, 0)) != -1))
        {
          bufsize = read(fd, Fm->cebuf, bufsize) ;
          if (bufsize == -1) bufsize = 0 ;
          CLOSE(fd) ;
        }
    }

  entry = (CE_ENTRY) ce_get_entry(Fm->ceo->file_ns, 3, fname,
                                  Fm->cebuf, bufsize) ;

  if (!entry)
    {
      CEDB("CE- no fns file entry found\n") ;
      return(NULL) ;
    }

  type = ce_get_attribute(Fm->ceo->file_ns, entry, Fm->ceo->file_type) ;
  if (!type)
    {
      CEDB("no fns type attribute found\n") ;
      return(NULL) ;
    }

  entry = (CE_ENTRY) ce_get_entry(Fm->ceo->type_ns, 1, type) ;

  CEDB1("CE- tns entry = %d\n", entry) ;
  return(entry) ;
}


CE_ENTRY
get_tns_entry_by_bits(buf, bufsize)
char *buf ;                          /* Bits to check. */
int bufsize ;                        /* Length of bits. */
{
  CE_ENTRY entry ;
  char *type ;

  if (!Fm->ceo->running) return(NULL) ;

  CEDB1("\nCE- get_tns_entry_by_bits() bits = (%s)\n", buf) ;

  if (bufsize == 0) return(NULL) ;

  entry = (CE_ENTRY) ce_get_entry(Fm->ceo->file_ns, 3, "", buf, bufsize) ;

  if (!entry)
    {
      CEDB("CE- no fns file entry found\n") ;
      return(NULL) ;
    }

  type = ce_get_attribute(Fm->ceo->file_ns, entry, Fm->ceo->file_type) ;
  if (!type)
    {
      CEDB("no fns type attribute found\n") ;
      return(NULL) ;
    }

  entry = (CE_ENTRY) ce_get_entry(Fm->ceo->type_ns, 1, type) ;

  CEDB1("CE- tns entry = %d\n", entry) ;
  return(entry) ;
}


char *
get_type_name(fpo)             /* Get the file's type name entry. */
File_Pane_Object *fpo ;
{
  char *str = NULL ;

  str = get_tns_attr(fpo->tns_entry, Fm->ceo->type_name) ;
  if (!str)
    CEDB("CE- no tns type name found\n") ;

  return(str) ;
}


void
init_goto_panel()
{
  int w, x, y ;

  fm_add_help(FM_PO_PROPS_GOTO_PANEL, "filemgr:Props_Goto_Panel") ;
  fm_add_help(FM_PO_GOTO_CLICK_MES,   "filemgr:Props_Goto_Add_Button") ;
  fm_add_help(FM_PO_PATHNAME,         "filemgr:Props_Goto_Pathname") ;
  fm_add_help(FM_PO_GOTO_MLABEL,      "filemgr:Props_Goto_Menu_Label") ;
  fm_add_help(FM_PO_GOTO_LIST,        "filemgr:Props_Goto_List") ;
  fm_add_help(FM_PO_GOTO_LAST_MES,    "filemgr:Props_Goto_Number") ;
  fm_add_help(FM_PO_GOTO_NUMBER,      "filemgr:Props_Goto_Number") ;
  fm_add_help(FM_PO_GOTO_ADD,         "filemgr:Props_Goto_Add_Button") ;
  fm_add_help(FM_PO_GOTO_EDIT,        "filemgr:Props_Goto_Edit_Button") ;
  fm_add_help(FM_PO_GOTO_APPLY,       "filemgr:apply") ;
  fm_add_help(FM_PO_GOTO_DEFAULT,     "filemgr:set_default") ;
  fm_add_help(FM_PO_GOTO_RESET,       "filemgr:reset") ;

  set_default_item(FM_PO_PROPS_GOTO_PANEL, FM_PO_GOTO_APPLY) ;

  x = get_item_int_attr(FM_PO_GOTO_LAST_MES, FM_ITEM_X) ;
  set_item_int_attr(FM_PO_GOTO_CLICK_MES, FM_ITEM_X, x) ;

  w = get_item_int_attr(FM_PO_PATHNAME,  FM_ITEM_WIDTH) ;
  x = get_item_int_attr(FM_PO_GOTO_LIST, FM_ITEM_X) ;
  y = get_item_int_attr(FM_PO_GOTO_LIST, FM_ITEM_VALY) ;
  set_item_int_attr(FM_PO_GOTO_EDIT, FM_ITEM_X, x + w + 20) ;
  set_item_int_attr(FM_PO_GOTO_EDIT, FM_ITEM_Y, y) ;
  set_item_int_attr(FM_PO_GOTO_LIST, FM_ITEM_LWIDTH, w - 20) ;

  load_goto_list() ;     /* Load scrolling list with menu values. */
  set_item_int_attr(FM_PO_GOTO_NUMBER, FM_ITEM_IVALUE, Fm->maxgoto) ;

  fm_center_prop_buttons(FM_PO_PROPS_GOTO_PANEL, FM_PO_GOTO_APPLY,
                         FM_PO_GOTO_DEFAULT, FM_PO_GOTO_RESET) ;
  Fm->goto_pw = get_item_int_attr(FM_PO_PROPS_GOTO_PANEL, FM_ITEM_WIDTH) ;
}


int
make_duplicate(wno, f_p, x, y)
File_Pane_Object **f_p ;
int wno, x, y ;
{
  char buf[MAXPATHLEN], destfile[MAXPATHLEN] ;
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **n_p ;
  int n ;

  sprintf(destfile, "%s/%s", f->path, (char *) (*f_p)->name) ;
  fm_msg(FALSE, Str[(int) M_DUPLICATE], (*f_p)->name) ;
  unique_filename(destfile) ;
  fm_snprintf(buf, sizeof(buf), "cp -r \"%s/%s\" \"%s\"", f->path, (*f_p)->name, destfile) ;
  if (fm_run_str(buf, TRUE))
    {
      fm_showerr(buf) ;
      return(FALSE) ;
    }

  if (x != NULL_X && y != NULL_Y)
    {
      if (f->num_objects == f->max_objects)
        if (alloc_more_files(wno) == FALSE) return(FALSE) ;

      n_p = f->object + f->num_objects ;
      n = strlen(destfile) ;
      if (((*n_p)->name = (WCHAR *) fm_malloc((unsigned) n+1)) == NULL)
        return(FALSE) ;

      STRCPY((char *) (*n_p)->name, destfile) ;
      (*n_p)->flen  = n ;
      (*n_p)->width = fm_strlen((char *) (*n_p)->name) ;
      if ((*n_p)->width > f->widest_name) f->widest_name = (*n_p)->width ;

      SET_FP_IMAGE((char *) NULL, wno, f->num_objects) ;
      my_stat((char *) f->path, n_p, wno, f->num_objects) ;
      find_free_grid_point(wno, *n_p) ;
      set_file_obj_x(wno, *n_p, x) ;
      set_file_obj_y(wno, *n_p, y) ;
      f->num_objects++ ;
    }
  return(TRUE) ;
}


void
over_path_tree_folder(wno, t_p, state)
int wno, state ;
Tree_Pane_Object *t_p ;
{
  BYTE ttype ;
  File_Pane *f ;

  if (wno != WNO_TREE) f = Fm->file[wno] ;

  ttype = FT_DIR ;
       if (wno == WNO_TREE)             ttype = FT_DIR ;
  else if (wno == WASTE)                ttype = FT_WASTE ;
  else if (f->mtype == FM_CD)           ttype = FT_CD ;
  else if (f->mtype == FM_FLOPPY)       ttype = FT_FLOPPY ;
  else if (f->mtype == FM_DOS)          ttype = FT_DOS ;
  else if (t_p == f->path_pane.current) ttype = FT_DIR_OPEN ;
  draw_folder(wno, t_p, ttype, state) ;
}


void
reset_custom_fields()
{
  set_item_str_attr(FM_PO_CMD_MLABEL,  FM_ITEM_IVALUE, "") ;
  set_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE, "") ;
  set_item_str_attr(FM_PO_CMD_PTEXT1,  FM_ITEM_IVALUE, "") ;
  set_item_int_attr(FM_PO_CMD_PROMPT,  FM_ITEM_IVALUE, 1) ;
  set_item_int_attr(FM_PO_CMD_OUTPUT,  FM_ITEM_IVALUE, 1) ;
}


void
reset_custom_panel()
{
  int selected = get_list_first_selected(FM_PO_CMD_LIST) ;

  if (selected != -1) select_list_item(FM_PO_CMD_LIST, selected, FALSE) ;
  reset_custom_fields() ;
  Fm->custom_changed = FALSE ;
}


void
resize_arrays(f, n)    /* Resize dynamic arrays, resize main frame. */
int f ;                /* True if first time. Malloc, else realloc. */
int n ;                /* Initial size or increment. */
{
  int limit, wno ;
  int old ;            /* Old size of the array. */

  if (f)
    {
      old = 0 ;
      Fm->maxwin = limit = n ;
    }
  else
    {
      old = Fm->maxwin ;
      limit = Fm->maxwin + n ;
    }

  Fm->file  = (File_Pane **) LINT_CAST(make_mem((char *) Fm->file,
              sizeof(File_Pane **) * old, sizeof(File_Pane **) * n, f)) ;

  Fm->mtime = (time_t *) LINT_CAST(make_mem((char *) Fm->mtime,
              sizeof(time_t *) * old, sizeof(time_t *) * n, f)) ;

  for (wno = old; wno < limit; wno++)
    {
      Fm->file[wno] = (File_Pane *)
                      LINT_CAST(fm_malloc((unsigned) sizeof(File_Pane))) ;
      set_pos_attr(wno, FM_POS_INCX, 1) ;
      Fm->file[wno]->path = (WCHAR *) fm_malloc((unsigned) (MAXPATHLEN+1)) ;
    }
}


int
set_canvas_dims(wno, mode)
int wno, mode ;
{
  int reply  = 0 ;
  int incx   = get_pos_attr(wno, FM_POS_INCX) ;
  int incy   = get_pos_attr(wno, FM_POS_INCY) ;
  int height = get_pos_attr(wno, FM_POS_CHEIGHT) ;
  int width  = get_pos_attr(wno, FM_POS_CWIDTH) ;

  Fm->resize_dispmode = mode ;
  if (width && incx)
    reply += fm_set_canvas_width(width,   wno, incx) ;
  if (height && incy)
    reply += fm_set_canvas_height(height, wno, incy) ;
  return(reply) ;
}


void
set_grid_points(wno)    /* Set all occupied grid points. */
int wno ;
{
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object **f_p, **l_p, **s_p ;
  File_Pane_Pos *p ;
  int incx, incy, ix, iy, max_filespercol, max_filesperline ;
  int n, startx, starty, x, y ;

  if (!get_pos_attr(wno, FM_POS_POS)) return ;

       if (f->display_mode == VIEW_BY_CONTENT) p = &f->c ;
  else if (f->display_mode == VIEW_BY_ICON)    p = &f->i ;
  else if (f->display_mode == VIEW_BY_LIST)    p = &f->l ;

  incx             = get_pos_attr(wno, FM_POS_INCX) ;
  incy             = get_pos_attr(wno, FM_POS_INCY) ;
  max_filespercol  = get_pos_attr(wno, FM_POS_MAXCOL) ;
  max_filesperline = get_pos_attr(wno, FM_POS_MAXLINE) ;
  startx           = get_pos_attr(wno, FM_POS_STARTX) ;
  starty           = get_pos_attr(wno, FM_POS_STARTY) ;

  if (!incx || !incy || !max_filespercol || !max_filesperline) return ;

  if (p->grid) FREE((char *) p->grid) ;
  p->grid = (WCHAR *)
    LINT_CAST(fm_malloc((unsigned) (max_filesperline * max_filespercol))) ;

  s_p = PTR_FIRST(wno) ;
  l_p = PTR_LAST(wno) ;
  for (f_p = s_p; f_p < l_p; f_p++)
    {
      x  = get_file_obj_x(wno, *f_p) ;
      ix = (x - startx) / incx ;
      y  = get_file_obj_y(wno, *f_p) ;
      iy = (y - starty) / incy ;
      if (((ix*incx + startx) == x) && ((iy*incy + starty) == y))
	{
          if (f->dispdir == FM_DISP_BY_ROWS)
	    n = (iy*max_filesperline) + ix ;
	  else
	    n = (ix*max_filespercol) + iy ;
	  if (n >= 0 && n < max_filesperline * max_filespercol) p->grid[n]++ ;
	}
    }
}


void
set_tree_icon(t_p, isopen)
Tree_Pane_Object *t_p ;
int isopen ;
{
  if (t_p == NULL) return ;
  if (isopen) t_p->status |= TOPEN ;
  else        t_p->status &= ~TOPEN ;
}


/* Returns the x coordinate where the file's name starts on the canvas. */

int
text_x_coord(i, wno)
int i, wno ;
{
  File_Pane *f = Fm->file[wno] ;
  File_Pane_Object *f_p ;
  int filex, x ;
  short dmode = f->display_mode ;

	Rect  *rect;
	Xv_Window  pwin;

  f_p = f->object[i] ;

  pwin = canvas_paint_window(X->fileX[wno]->canvas);
  rect = (Rect *)xv_get(X->fileX[wno]->canvas, CANVAS_VIEWABLE_RECT,
   			 pwin);

  filex =  get_file_obj_x(wno, f_p) - rect->r_left;


  if (dmode == VIEW_BY_ICON)                                  /* Icon. */
    x = filex + (GLYPH_WIDTH >> 1) - (f_p->width >> 1) ;
  else if (dmode == VIEW_BY_CONTENT)                          /* Content. */
    x = filex + (GLYPH_WIDTH >> 1) - (f_p->width >> 1) ;
  else if (dmode == VIEW_BY_LIST && f->listopts)              /* List+opts. */
    x = MARGIN + LIST_HEIGHT ;
  else                                                        /* List. */
    x = filex + LIST_HEIGHT ;

/* Offset by left scrollbar if there. */

  if (Fm->left_scrollbar) x += sb_width(wno) ;

/* Offset by width of canvas. */

  x += get_canvas_attr(wno, FM_WIN_X) ;
  return(x) ;
}


/* Returns the y coordinate where the file's name starts on the canvas. */

int
text_y_coord(i, wno)
int i, wno ;
{
  File_Pane *f = Fm->file[wno] ;
  int y ;

	Rect  *rect;
	Xv_Window  pwin;


  pwin = canvas_paint_window(X->fileX[wno]->canvas);
  rect = (Rect *)xv_get(X->fileX[wno]->canvas, CANVAS_VIEWABLE_RECT,
   			 pwin);

  y =  get_file_obj_y(wno, f->object[i]) - rect->r_top;


/* Translate file index to y coordinate. */
/******************************

  y = get_file_obj_y(wno, f->object[i]) - (get_pos_attr(wno, FM_POS_INCY) *
                     get_scrollbar_attr(wno, FM_V_SBAR, FM_SB_VIEW_ST)) ;

**************************************/
/* Offset by size of icon. */

  if (f->display_mode != VIEW_BY_LIST) y += GLYPH_HEIGHT + Fm->font_sizeH - 10 ;
  else                                 y -= 10 ;

/* Offset by canvas height. */

  y += get_canvas_attr(wno, FM_WIN_Y) ;
  return(y) ;
}


Boolean
valid_new_cmd()
{
  char *cmd = get_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE) ;

/* Make sure UNIX command is not empty. */

  if (cmd == NULL || isempty((WCHAR *) cmd))
    {
      notice(Str[(int) M_UNIX_CMD], Str[(int) M_OK], NULL) ;
      return(FALSE) ;
    }

/* Check UNIX command for $FILE */

  if (!find_pattern(cmd, "$FILE"))            /* Check for $FILE */
    {
      int choice ;

/* Give warning and get user's choice. */

      choice = give_file_warning() ;

           if (choice == FILE_CANCEL) return(FALSE) ;  /* User cancelled. */
      else if (choice == FILE_ADDFILE)
        {
          char *newcmd = fm_malloc(strlen(cmd) + 7) ;

          SPRINTF(newcmd, "%s $FILE", cmd) ;      /* User wants $FILE added. */
          set_item_str_attr(FM_PO_CMD_CMDLINE, FM_ITEM_IVALUE, newcmd) ;
        }
      else if (choice != FILE_CONTINUE)               /* User continued? */
        ERR_EXIT(Str[(int) M_INV_NOTICE]) ;
    }
  return(TRUE) ;
}


void
write_custom_commands()
{
  Custom_Command *cc ;
  int i ;

  Fm->no_ccmds = get_item_int_attr(FM_PO_CMD_LIST, FM_ITEM_LROWS) ;
  load_deskset_defs() ;
  for (i = 0; i < Fm->no_ccmds; i++)
    {
      cc = (Custom_Command *) get_list_client_data(FM_PO_CMD_LIST, i) ;

      set_ccmd_str_resource(i, "Alias",    cc->alias) ;
      set_ccmd_str_resource(i, "Command",  cc->command) ;
      set_ccmd_str_resource(i, "Prompt",   cc->prompt) ;
      set_ccmd_str_resource(i, "IsPrompt", cc->is_prompt) ;
      set_ccmd_str_resource(i, "IsOutput", cc->is_output) ;
    }
  save_resources() ;
}
