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
/* --------------------------------------------------
   driver for computing fixed points and periodic orbits

   -------------------------------------------------- */
#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include <constants.h>
#include <prop.h>
#include <modellib.h>
#include <defaults.h>
#include <pm.h>
#include <fixptlib.h>


/*
 * fp_load(fp,flag)
 *         fp is a pointer to the fixed point data structure
 *         flag tells whether you are preparing to compute fixed 
 *         points or manifolds (0 or 1, respectively)
 * 
 *  procedure to allocate memory and load the fixed point data structure
 *
 * last modified: 12/2/91 paw
 */
int
fp_load(fp,flag)
struct Fixpt_DataS *fp;
int flag;               /* 0 for fixed point, 1 for manifolds */
{
  int *ivector(),traj_length,var_dim, status=0;
  double **dmatrix(), *dvector(); 

  /* load info from periodic panel via the postmaster */
  fp->map_period = *((int *) pm(GET, "Fixed.Map_Period", 0));
  fp->vf_period = *((double *) pm(GET, "Fixed.Vf_Period", 0));
  fp->algorithm = *((int *) pm(GET, "Fixed.Algorithm", 0));
  fp->pick_pt = *((int *) pm(GET, "Fixed.Guess", 0));
  fp->ntrials = *((int *) pm(GET, "Fixed.Mc_Guesses", 0));
  fp->nsteps = *((int *) pm(GET, "Fixed.Num_Iters", 0));
  fp->setting = *((int *) pm(GET, "Fixed.Setting", 0));
  fp->fd_step = *((double *) pm(GET, "Fixed.FD_Step", 0));
  fp->dup = *((double *) pm(GET, "Fixed.Dups", 0));
  fp->varb_conv = *((double *) pm(GET, "Fixed.Var_Conv", 0));
  fp->funct_conv = *((double *) pm(GET, "Fixed.Funct_Conv", 0));
  fp->eigen_dist = *((double *) pm(GET, "Fixed.Eigen_Dist", 0));
  fp->stab_pts = *((int *) pm(GET, "Fixed.Stab_Points", 0));
  fp->stab_steps = *((int *) pm(GET, "Fixed.Stab_Steps", 0));
  fp->unstab_pts = *((int *) pm(GET, "Fixed.Unstab_Points", 0));
  fp->unstab_steps = *((int *) pm(GET, "Fixed.Unstab_Steps", 0));
  fp->memory = (memory) pm(GET, "Memory.Fixed", 0);

  if (flag==0)
    {
      /* we are looking for periodic points */
      traj_length = fp->map_period;
    }
  else
    {
      /* drawing manifolds, take the longest one ! */
      traj_length = (fp->stab_steps*fp->stab_pts > fp->unstab_steps*fp->unstab_pts) ? 
	(fp->stab_steps*fp->stab_pts) : (fp->unstab_steps*fp->unstab_pts) ;
    }
  
  /* setup propagation structure */
  fp->prop_cntl.direction = FORWARD;
  fp->prop_cntl.iterations = traj_length;
  fp->prop_cntl.start_to_save = 0;
  fp->prop_cntl.time_step = *((double *) pm( GET, "Flow.Stepsize", 0));
  fp->prop_cntl.prop_mode = PROP_NSTEP;

  fp->prop_cntl.f_iter = fp->prop_cntl.f_skip = 
    *((int *) pm(GET, "Flow.Skip_Size", 0));
  fp->prop_cntl.table_color = -1; /* system color */
  
  /* do memory allocations */
  if (allocate_integ(&(fp->prop_cntl),traj_length)) status = -1;
  var_dim = fp->prop_cntl.ph_space_dim -1;
  if (!(fp->indx = ivector(0,var_dim))) status= -1;
  if (!(fp->x1 = dvector(0,var_dim))) status= -1;
  if (!(fp->x2 = dvector(0,var_dim))) status= -1;
  if (!(fp->x3 = dvector(0,var_dim))) status= -1;
  if (!(fp->fx = dvector(0,var_dim))) status= -1;
  if (!(fp->jacobian = dmatrix(0,var_dim, 0 , var_dim))) status= -1;
  if (!(fp->jact = dmatrix(0,var_dim, 0 , var_dim))) status= -1;
  if (!(fp->eval = dmatrix(0,1,0,var_dim))) status= -1;
  if (!(fp->evectors = dmatrix(0,var_dim,0,var_dim))) status= -1;

  if (comp_alloc(&(fp->prop_cntl))) status = -1;
  else comp_def_setup(&(fp->prop_cntl));

  if (status==-1) 
    {
      system_mess_proc(1,"fp_load: Memory Allocation Failure \n");
      release_fp(fp,flag);
    }

  return(status);
}


/*
 * release_fp(fp,flag)
 *         fp is a pointer to the fixed point data structure
 *         flag tells whether you have been computing fixed points or manifolds (0 or 1, respectively)
 *
 * procedure to free up the memory allocated with the fixed point data structure
 *
 * last modified: 7/11/91  paw
 */
int fp_1dman(struct Fixpt_DataS * fp);
int fp_compute(struct Fixpt_DataS * fp);
int release_fp(struct Fixpt_DataS * fp, int flag);

int release_fp(struct Fixpt_DataS * fp, int flag)
{
  int traj_length;
  int var_dim = fp->prop_cntl.ph_space_dim - 1;

  if (flag==0)
    traj_length = fp->map_period;  /* FIX THIS FOR FLOWS */
  else
    traj_length = (fp->stab_steps*fp->stab_pts > fp->unstab_steps*fp->unstab_pts) ? 
                    (fp->stab_steps*fp->stab_pts) : (fp->unstab_steps*fp->unstab_pts) ;

  comp_free(&(fp->prop_cntl));
  release_integ(&(fp->prop_cntl),traj_length);

  free_ivector(fp->indx,0,var_dim);
  free_dvector(fp->x1,0,var_dim);   
  free_dvector(fp->x2,0,var_dim);   
  free_dvector(fp->x3,0,var_dim);   
  free_dvector(fp->fx,0,var_dim);
  free_dmatrix(fp->jacobian,0,var_dim, 0, var_dim);
  free_dmatrix(fp->jact,0,var_dim, 0, var_dim);
  free_dmatrix(fp->eval,0,1,0,var_dim);
  free_dmatrix(fp->evectors,0,var_dim,0,var_dim);

  fp->indx = (int *) 0;
  fp->x1 = fp->x2 = fp->x3 = fp->fx = (double *) 0;
  fp->jacobian = fp->jact = fp->eval = fp->evectors = (double **) 0;

}


/*
 * fp_init
 *
 * procedure called to find fixed points of a vector field and
 * periodic points of a mapping
 * The results are stored in the fixed point memory object and displayed.
 *
 * last modified: 7/11/91 paw
 */
void
fp_init(void)
{
  struct Fixpt_DataS	fixpt_cntl;
  int 	status = 0;
  int	found = *((int *) pm(GET, "Fixed.Found", 0));

  /* set up interrupts */
  reset_interrupt();

  /* fill up fixed point control structure */
  status = fp_load(&fixpt_cntl,0);
  if (status == 0) 
    {
      /* get parameters and initial condition */
      pm( GET_LIST, "Selected.Param_Ic", 0, fixpt_cntl.prop_cntl.parameter_dim-1, 
	 fixpt_cntl.prop_cntl.parameters, 0); 
      pm( GET_LIST, "Selected.Varb_Ic", 0, fixpt_cntl.prop_cntl.ph_space_dim-1,
	 fixpt_cntl.x1, 0); 
      
      /* if not Monte Carlo then we only do once */
      if (fixpt_cntl.pick_pt) fixpt_cntl.ntrials=1;
      
      /* compute fixed points */
      status = fp_compute(&fixpt_cntl);
      
      if (status>0) 
	pm(PUT, "Fixed.Found", found+status, 0);
      
      release_fp(&fixpt_cntl,0);
    }
  return;
}


/*
 * fp_1dman_init()
 *
 * procedure called to compute manifolds of fixed points (or periodic points)
 * The results are stored in the fixed point memory object and displayed.
 *
 * NOTE: currently only 1-d manifolds are implemented!
 *
 * last modified: 7/11/91  paw
 */
void
fp_1dman_init(void)
{
  int status = 0;
  struct Fixpt_DataS fixpt_cntl;
  double *dvector(),**dmatrix();

  if (fp_load(&fixpt_cntl,1) == -1) 
    {
      system_mess_proc(1,"fpman_init: Memory alloc error.");
    }
  else
    {
      status = fp_1dman(&fixpt_cntl);
      release_fp(&fixpt_cntl,1);
    }
  return;
}



/*
 * periodic_clear_fixpts_go()
 *
 * Procedure to clear fixed points from fixed point memory object
 *
 * last modified:  8/1/91  paw
 */
void
periodic_clear_fixpts_go(void)
{
  memory m;
  int fixpt_test();
  
  m = (memory) pm(GET, "Memory.Fixed", 0);
  memory_clear_selected_flows(m,fixpt_test);
  pm(PUT, "Fixed.Found", 0, 0);
}


/*
 * periodic_clear_mans_go()
 *
 * Procedure to clear manifolds from fixed point memory object
 *
 * last modified:  8/1/91  paw
 */
void
periodic_clear_mans_go(void)
{
  memory m;
  int manifold_test();
  
  m = (memory) pm(GET, "Memory.Fixed", 0);
  memory_clear_selected_flows(m,manifold_test);
}
