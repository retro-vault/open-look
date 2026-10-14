/*  -------------------------------------------------------------------

This program is the property of:

                             Cornell University 
                        Center for Applied Mathematics 
                              Ithaca, NY 14853

and may be used, modified and distributed freely, subject to the 
following restrictions:

       Any product which incorporates source code from the dstool
       program or utilities, in whole or in part, is distributed
       with a copy of that source code, including this notice. You
       must give the recipients all the rights that you have with
       respect to the use of this software. Modifications of the
       software must carry prominent notices stating who changed
       the files and the date of any change.

DsTool is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of FITNESS 
FOR A PARTICULAR PURPOSE.  The software is provided as is without 
any obligation on the part of Cornell faculty, staff or students to 
assist in its use, correction, modification or enhancement.

  -----------------------------------------------------------------  */
/*
 * unform.c - procedures for loading unformatted data
 */

#include <stdio.h>
#include <math.h>
#include <sys/param.h>
#include <sys/types.h>

#include <pm.h>
#include <constants.h>
#include <saveload.h>
#include <memory.h>

#define  FREE_IVECTORS \
           free_ivector(p_index,0,p_dim-1); \
           free_ivector(v_index,0,p_dim-1);
#define  FREE_DVECTORS \
           free_dvector(pt,0,num_fields-1); \
           free_dvector(param,0,p_dim); \
           free_dvector(varb,0,v_dim);

/*
load_unform_data()   loads unformatted data from specified file.  This is useful
for displaying and manipulating data created by other programs.
*/
int read_unform_header(FILE		* fp, int v_dim, int p_dim, char * mem_type, int *len, int *p_index, int *v_index);

int load_unform_data(FILE    * fp)
{
  memory             mem_ptr;
  double             *pt,*orig_pt,*param,*varb,*dvector();
  int                color[3];
  int                i,def_len,traj_check;
  int                get_double_list_1p1(),*ivector();
  int                *p_index,*v_index;
  char mem_type[MAX_LABEL_LEN];
  int                symbol_on, color_on, varb_on=FALSE, param_on=FALSE;
  int                num_fields=0;
  int                v_dim = *((int *)pm(GET, "Model.Varb_Dim", 0));
  int                p_dim = *((int *)pm(GET, "Model.Param_Dim", 0));
  
  color[0] = max( 0, get_alt_color() );	/* use maximum non-system color */
  color[1] = *((int *) pm( GET, "Color.Pick_Color_Choice", 0));
  color[2] = get_symbol_code( *((int *) pm(GET, "Defaults.Symbol_Index", 0)), 
			     *((int *) pm(GET, "Defaults.Size_Index", 0)));

  p_index = ivector(0,p_dim-1);
  v_index = ivector(0,v_dim-1);

  num_fields = read_unform_header(fp,v_dim,p_dim,mem_type,&def_len,p_index,v_index); 

  /* determine what the user has selected */
  color_on  = *((int *)pm(GET, "Load.Color_Flag", 0));
  symbol_on = *((int *)pm(GET, "Load.Symbol_Flag", 0));
  for (i=0; i<v_dim; i++)
    if ( v_index[i] )
      varb_on = TRUE;
  for (i=0; i<p_dim; i++)
    if ( p_index[i] )
      param_on = TRUE;

  if (!(mem_ptr = (memory) pm(GET, mem_type, 0)))	/* assign mem type */
    {
      if ( *((int *)pm(INIT, mem_type, 0)) ) 
	{ /* init failed! */
	  FREE_IVECTORS
	    return (-3);
	}
      else
	mem_ptr = (memory) pm(GET, mem_type, 0);
    }
 
  param = dvector(0,p_dim-1);	/* allocate and fill default values */
  varb = dvector(0,v_dim-1);

  pm(GET_LIST, "Selected.Param_Ic",   0, p_dim-1, param, 0);
  pm(GET_LIST, "Selected.Varb_Ic", 0, v_dim-1, varb,  0);

  pt = orig_pt = dvector(0,num_fields-1);

  if ((strcmp(mem_type,"Memory.Traj")==0) && param_on )	/* quick error checking */
    {			/* error if anything other than varbs */
      FREE_IVECTORS
      FREE_DVECTORS
      return ( -2 );
    }

  if (memory_start_new_flow(mem_ptr,1,0, 0, def_len, 0, 0) == 0)
    {
      while ( get_double_list_1p1(fp,num_fields,pt) ) 
	{
	  if (varb_on)
	    for (i=0; i<v_dim; i++) /* assign variables if any */
	      if ( v_index[i] )
		varb[i] = *(pt++);
	  if (param_on)
	    for (i=0; i<p_dim; i++) /* assign parameters if any */
	      if ( p_index[i] )
		param[i] = *(pt++);
	  pt = orig_pt;		/* reset pointer */
	  if (color_on )	/* read color coding if any */
	    fscanf(fp,"%d",color+1);
	  if (symbol_on )	/* read symbol coding if any */
	    fscanf(fp,"%d",color+2);
	  
	  memory_add_point(mem_ptr, varb, param, color, 0, 0, 0, 0);
	}
      memory_end_current_flow(mem_ptr);
    }
	
  pt = orig_pt;
  FREE_IVECTORS
  FREE_DVECTORS
  return( 0 );
}


/*
 * read_unform_header() gets information about an unformatted file
 * The function returns the number of double data points.
 */
int read_unform_header(FILE		* fp, int v_dim, int p_dim, char * mem_type, int *len, int *p_index, int *v_index)
{
  int      i,c;
  int      index_selected=0;                          /* used to check if any data is expected */

 
  pm(GET, "Load.Data_Type", mem_type, 0);

  if (!strcmp(mem_type, "Memory.Traj"))
    *len = DEFAULT_TRAJ_LENGTH;
  else if (!strcmp(mem_type, "Memory.Fixed"))
    *len = DEFAULT_FP_LENGTH;
  else if (!strcmp(mem_type, "Memory.Cont"))
      *len = DEFAULT_CONT_LENGTH;
  else if (!strcmp(mem_type, "Memory.Param"))
      *len = DEFAULT_PARAM_LENGTH;
  else if (!strcmp(mem_type, "Memory.Sel_Pt"))
    *len = DEFAULT_SEL_PT_LENGTH;
  else return(-1);

  pm(GET_LIST, "Load.Varb_Index",  0, v_dim-1, v_index, 0);
  pm(GET_LIST, "Load.Param_Index", 0, p_dim-1, p_index,  0);

   for ( i=0; i<v_dim; i++ )			      /* add up number of chosen indices */
      if ( v_index[i] ) index_selected++;
   for ( i=0; i<p_dim; i++ )
      if ( p_index[i] ) index_selected++;
   
   while ( (c = getc(fp)) == '#' )        /* read (and discard) any comments at beginning of file */
      while ( (c = getc(fp)) != '\n' && (c != EOF) )
         ;  
   ungetc(c,fp);                                         /* read too far; push char back on stream */
   return ( index_selected );
}
