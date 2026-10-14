/* (C) Universitaet Passau 1986-1991 */
/* Hauptprogramm */

#include "std.h"
#include "sgraph.h"
#include "slist.h"
#include "graphed.h"

#include "planar.h"
#include "embed.h"

extern	void	dualgraph();

#define ENDPOINT(node,edge) ((node) == (edge)-> tnode ? (edge)-> snode : (edge)-> tnode)

/* ========================================================================= */
  
void planarity_test(Sgraph_proc_info info)
{ 
  switch(planarity(info-> sgraph))
    {
      case SUCCESS       : message("graph is planar.\n");
                           break;
      case NONPLANAR     : message("graph is nonplanar.\n");
                           break;
      case SELF_LOOP     : message("graph contains self-loops.\n");
                           break;
      case MULTIPLE_EDGE : message("graph contains multiple edges.\n");
                           break;
      case NO_MEM        : message("not enough memory.\n");
                           break;
    }
}
  
/* ====================================================================== */

void write_graph(graph)
Sgraph graph;

{ Snode node;
  Slist list,l;

  message("ordered adjacency lists:\n");
  for_all_nodes(graph,node)
    {
      message("node <%s> : edges to",node->label);
      list = attr_data_of_type(node,Slist);
      for_slist(list,l)
        message(" <%s>",ENDPOINT(node,attr_data_of_type(l,Sedge))->label);
      end_for_slist(list,l)
      message("\n");
    }
  end_for_all_nodes(graph,node)
}
 
/* ---------------------------------------------------------------------- */ 

void embedding(Sgraph_proc_info info)
{ 
  switch(embed(info-> sgraph))
    {
      case SUCCESS       : message("graph is planar.\n");
                           write_graph(info-> sgraph);
                           break;
      case NONPLANAR     : message("graph is nonplanar.\n");
                           break;
      case SELF_LOOP     : message("graph contains self-loops.\n");
                           break;
      case MULTIPLE_EDGE : message("graph contains multiple edges.\n");
                           break;
      case NO_MEM        : message("not enough memory.\n");
                           break;
    }
}
  
/* ====================================================================== */

/* Einfuegen eigener Menueeintraege ins Hauptprogramm */
    
char *planarity_test_call(char * menu, char *menu_item)
{
  return call_sgraph_proc (planarity_test);
}
  
char *embedding_test_call(char * menu, char *menu_item)
{
  return call_sgraph_proc (embedding);
}


void einbettung(Sgraph_proc_info info)
  {
   Sgraph graph;
   graph = info->sgraph;
   switch(embed(graph))
     {
      case SELF_LOOP :     message("graph contains self-loops\n");
                           break;
      case MULTIPLE_EDGE : message("graph contains multiple edges\n");
                           break;
      case NONPLANAR     : message("graph is nonplanar\n");
                           break;
      case NO_MEM        : message("not enough memory\n");
                           break;
      case SUCCESS       : message("ok\n");
                           break;
      default : break;
      } 
  }

void dualgraph(Sgraph_proc_info info)
  {
   Sgraph graph, DualGraph;
   graph = info->sgraph;
   switch(embed(graph))
     {
      case SELF_LOOP :     message("graph contains self-loops\n");
                           break;
      case MULTIPLE_EDGE : message("graph contains multiple edges\n");
                           break;
      case NONPLANAR     : message("graph is nonplanar\n");
                           break;
      case NO_MEM        : message("not enough memory\n");
                           break;
      case SUCCESS       : DualGraph = dual(graph);
                           break;
      default : break;
      }
  }

char *Einbettungs_aufruf(char * menu, char *menu_item)
  {
   return call_sgraph_proc (einbettung);
  }

char *Dual_aufruf(char * menu, char *menu_item)
  {
   return call_sgraph_proc (dualgraph);
  }


