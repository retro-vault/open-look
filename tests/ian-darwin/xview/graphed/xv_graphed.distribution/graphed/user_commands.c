/* (C) Universitaet Passau 1986-1991 */
/* GraphEd Source, 1986-1991 by Michael Himsolt	*/
/************************************************************************/
/*									*/
/*			GraphEd User Commands				*/
/*									*/
/************************************************************************/


#include "misc.h"
#include "graph.h"
#include "graphed_subwindows.h"
#include "user_header.h"


#define INCLUDE_SGRAPH
#ifdef INCLUDE_SGRAPH
#include "sgraph/std.h"
#include "sgraph/sgraph.h"
#include "sgraph/slist.h"
#include "sgraph/graphed.h"
#include "graphed_sgraph_interface.h"


static	char		*dummy_sgraph_proc(Sgraph_proc_info info)
{
	return 0;
}


static	char	*dummy(char		* menu, char		 *menu_item)
{
	return call_sgraph_proc (dummy_sgraph_proc);
}


void	init_user_menu(void)
{
	add_to_user_menu ("User-defined extensions may be included here", dummy);
}


int main(int argc, char	** argv)
{
	graphed_main (argc, argv);
}


#endif

