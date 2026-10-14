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

extern struct  Cont_Cntl_Ds            cont_ds;

int
hopf_bp_init(int	* n_aug_varb, int	 max_iters, double	*** xr, double	 **fpar, double	 *state, double	 *parameters)
{
  int		i, nvar;
  double	**dmatrix(), delta;
  int           n_varbs  = *((int *) pm( GET, "Model.Varb_Dim", 0 )) - 1; 
  int           n_param = *((int *) pm( GET, "Model.Param_Dim", 0 ));

  *n_aug_varb   =       1;
  nvar          =       n_varbs + (*n_aug_varb) + Cont_Sel[Cont_Cur_Choice].Num_Req_Param;

  cont_alloc( max_iters, n_varbs, n_param, nvar, xr, fpar );

  update_cont_state( (*xr)[0], state, parameters, n_varbs, *n_aug_varb, n_param );

  for(i=1; i<= n_param; i++) (*fpar)[i] = parameters[i-1];

  get_Dxf( cont_ds.jacobian_wrt_x, n_varbs, state, parameters );

  delta = 1.0; 							/* replace with estimate on min Re part! */
  (*xr)[0][n_varbs+1] = delta;

}


int
hopf_bp_update(double	* x, double	 *parm, int n_aug_varb)
{
}



int
hopf_bp_func(int nvar, double	* fpar, int *ipar, double	 *x, double	 *fx)
{
  int		  ierror=0, i, biprod_dim;
  int             n_varb = *((int *) pm( GET, "Model.Varb_Dim", 0 )) - 1;
  int             n_params = *((int *) pm( GET, "Model.Param_Dim", 0 ));
  int             (*function)();
  double          *dwork, det, det_comp[2], *state, *params, **matrix, **dmatrix(), delta;

  int j;

  state = cont_ds.r_workspace;
  params = state + n_varb + 1;
  dwork = params + n_params + 1;
  function = (void *) pm( GET, "Model.DS_Def", 0 );
  for(i=0; i<n_params; i++) params[i] = fpar[i+1];

  update_ds_state( x, state, params, n_varb, 1, n_params );
  function( &fx[1], state, params);
  delta = x[n_varb+1];

  get_Dxf( cont_ds.jacobian_wrt_x, n_varb, state, params );

  biprod_dim = (int)((n_varb-1)*n_varb/2);
  matrix = dmatrix(0,biprod_dim,0,biprod_dim);
  biprod( matrix, cont_ds.jacobian_wrt_x, n_varb, &det );
  fx[n_varb+1] = det;

  multAB( matrix, cont_ds.jacobian_wrt_x, n_varb, cont_ds.jacobian_wrt_x, n_varb);

  for(i=0; i<n_varb; i++) matrix[i][i] += delta*delta;

/*
	        fprintf(stderr,"\n ------------------------------------------------------\n");	
                fprintf(stderr,"\n delta = %lg \n \n", delta);       

                fprintf(stderr,"\nJacobian in  hopf_bp_func() :\n-----------------\n");       
		for(i=0; i<n_varb; i++)
		  {
		   fprintf(stderr,"\n");
		   for(j=0; j<n_varb; j++)
		      fprintf(stderr,"%11.6lf ",cont_ds.jacobian_wrt_x[i][j]);
		  }
                fprintf(stderr,"\n\n");       

                fprintf(stderr,"\nJacobian squared in  hopf_bp_func() :\n-----------------\n");       
		for(i=0; i<n_varb; i++)
		  {
		   fprintf(stderr,"\n");
		   for(j=0; j<n_varb; j++)
		      fprintf(stderr,"%11.6lf ",matrix[i][j]);
		  }
*/


  dgefa(matrix,n_varb,n_varb,cont_ds.i_workspace);
  dgedi(matrix,n_varb,n_varb,cont_ds.i_workspace,det_comp,dwork,10);      /* determinant only */
  fx[n_varb+2] = det_comp[0]*pow(10.0,det_comp[1]);

  free_dmatrix(matrix,0,biprod_dim,0,biprod_dim);

  return(ierror);
}


int
hopf_bp_dfunc(int nvar, double	* fpar, int *ipar, double	 *x, double	 **fprime)
{
}


int
hopf_bp_check(int *color, int n_varb, double		* state, double		 *param)
{
   int	ierror=0;

   color[1] = 10;	/* SYS_SKY_ORANGE */
   color[2] = MED_POINT;

   return(ierror);

}


