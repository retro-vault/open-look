
/*  @(#)tree.h	1.1 03/03/92
 *
 *  Copyright (c) 1987-1991 Sun Microsystems, Inc.
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

/*  tree.h -- implements a 2-3-4 tree, a.k.a. red-black tree
 *            Many thanks to Nannette Simpson for use of her code.
 */

typedef enum { red = 0, black = 1 } Color ;
typedef enum { rb_ok = 0,      rb_duplicate = 1, rb_badtable = 2,
               rb_notable = 3, rb_failed = 4,    rb_other = 5 } Rb_Status ;
typedef enum { less, equal, greater } Comparison ;
typedef enum { false, true } Bools ;
typedef caddr_t Key ;
typedef caddr_t Data ;

typedef struct node {
  struct node *llink ;
  struct node *rlink ;
  Color color ;
  Data  data ;
} Node ;

typedef struct {
  Node *root ;
  caddr_t private ;         /* For internal tool state. */
  caddr_t client_data ;     /* Available to clients.    */
} Rb_tree ;

typedef Key        (*Get_key)        (/* Data data */) ;
typedef Comparison (*Compare_proc)   (/* Key key ; Data data */) ;
typedef Bools      (*Enumerate_proc) (/* Data data */) ;

extern Rb_tree*  rb_create (/* Get_key get ; Compare_proc compare */) ;
extern Rb_Status rb_insert (/* Rb_tree *t ; Data data; Key key */) ;
extern Node*     rb_delete (/* Rb_tree *t ; Key key */) ;
extern Data      rb_lookup (/* Rb_tree *t ; Key key */) ;
