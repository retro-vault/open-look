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
#include <stdio.h>
#include <math.h>
#include <constants.h>
#include "../continue.h"

/* For Saddle-Nodes */

extern struct  Cont_Cntl_Ds            cont_ds;


int
det_init(int	* n_aug_varb, int	 max_iters, double	*** xr, double	 **fpar, double	 *state, double	 *parameters)
{
  int		i, j, nvar;
  int           n_varb  = *((int *) pm( GET, "Model.Varb_Dim", 0 )) - 1; 
  int           n_param = *((int *) pm( GET, "Model.Param_Dim", 0 ));

  *n_aug_varb   =       0;
  nvar          =       n_varb + (*n_aug_varb) + Cont_Sel[Cont_Cur_Choice].Num_Req_Param;

  cont_alloc( max_iters, n_varb, n_param, nvar, xr, fpar );

  update_cont_state( (*xr)[0], state, parameters, n_varb, *n_aug_varb, n_param );

  for(i=1; i<= n_param; i++) (*fpar)[i] = parameters[i-1];

}


int
det_update(double	* x, double	 *parm, int n_aug_varb)
{
}



int
det_func(int nvar, double	* fpar, int *ipar, double	 *x, double	 *fx)
{
  int             (*function)();
  int		  ierror=0, i, j, *index, status;
  int		  period   = *((int *) pm( GET, "Flow.Skip_Size", 0 ));
  int             map_flag = *((int *) pm( GET, "Model.Mapping_Flag", 0 ));
  int             n_varb   = *((int *) pm( GET, "Model.Varb_Dim", 0 )) - 1;
  int             n_params = *((int *) pm( GET, "Model.Param_Dim", 0 ));
  double          *state, *params, *v, det_comp[2], pow(), value;

  int		iwork[2000];
  double        work[2000], dwork[2000];

  index = cont_ds.i_workspace;
  state = dwork;
  params = state + n_varb + 2;
  v = params + n_params + 1;
  function = (void *) pm( GET, "Model.DS_Def", 0 );
  for(i=0; i<n_params; i++) params[i] = fpar[i+1];

  update_ds_state( x, state, params, n_varb, 0, n_params );

  if(map_flag)
    {
     iter_forw( function, period, &fx[1], state, params, n_varb+1, 1.0, v, cont_ds.manifold);
     get_Dxf( cont_ds.jacobian_wrt_x, n_varb, state, params ); /* calculate (Df^s - id) at given pt. */
     for(i=0; i<n_varb; i++) cont_ds.jacobian_wrt_x[i][i] -= 1.0;
     for(i=0; i<n_varb; i++) fx[i+1] -= state[i]; /* switched lines fjw 8/12/92 */
    }
  else
    {
     function( &fx[1], state, params);
     get_Dxf( cont_ds.jacobian_wrt_x, n_varb, state, params );
    }

  if(n_varb == 2)							/* compute det separately fjw 8/12/92*/
    {
      value = cont_ds.jacobian_wrt_x[0][0]*cont_ds.jacobian_wrt_x[1][1] - 
	cont_ds.jacobian_wrt_x[0][1]*cont_ds.jacobian_wrt_x[1][0];
    }
  else
    {
      status = ludcmp( cont_ds.jacobian_wrt_x, n_varb, index, &value); 
      value = 1.0;
      for(i=0; i<n_varb; i++) value *= cont_ds.jacobian_wrt_x[i][i];
    }
/*
  dgefa(cont_ds.jacobian_wrt_x,n_varb,n_varb,iwork);
  dgedi(cont_ds.jacobian_wrt_x,n_varb,n_varb,iwork,det_comp,work,10);     
  fx[n_varb+1] = det_comp[0]*pow(10.0,det_comp[1]);
  fx[n_varb+1] = fx[n_varb+1];
*/

  fx[n_varb+1] = value;

  return(ierror);
}


int
det_dfunc(int nvar, double	* fpar, int *ipar, double	 *x, double	 **fprime)
{

	int		ierror, i, j, offset;
	int             n_varb = *((int *) pm( GET, "Model.Varb_Dim", 0 )) - 1;
	int		n_param = *((int *) pm( GET, "Model.Param_Dim", 0 ));
	double		*params, *state; 

	ierror = 0;
 
        state = cont_ds.r_workspace;  
	params = state + n_varb + 2;  

        for(i=0; i<n_param; i++) params[i] = fpar[i+1];

	update_ds_state( x, state, params, n_varb, 0, n_param );

	get_Dxf( cont_ds.jacobian_wrt_x, n_varb, state, params );
	get_Dpf( cont_ds.jacobian_wrt_p, n_param, state, params ); /* used to be n_varb; fjw 8/16/92 */

	offset = n_varb;
	for(j=0; j<n_param; j++)
	  if(cont_ds.Active_Param[j])
	    {
             for(i=0; i<n_varb; i++)
	        cont_ds.jacobian_wrt_x[i][offset] = cont_ds.jacobian_wrt_p[i][j];
	     ++offset;
            }

        for(i=0; i<n_varb; i++)
	   for(j=0; j<=n_varb; j++)
	      fprime[i][j] = cont_ds.jacobian_wrt_x[i][j];

        return(ierror);
}


int
det_check(int *color, int n_varb, double		* state, double		 *param)
{
   int	ierror=0;

   if ( *((int *)pm(GET, "Cont.Search", 0)) )
     color[2] = MED_TRI;
   else
     color[2] = MED_POINT;
   if( !cont_ds.Check_Switch )
     {
      color[1] = 1;
     }
   else
     {
      color[1] = SYS_GREEN;
     }

   return(ierror);
}


