#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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
 * load_procs.c contains routines to load dstool configuration and memory objects from a data file 
 */
#include <stdio.h>
#include <stdlib.h>

#include <portability.h>

#ifdef HAS_STRINGS_H
#include <strings.h>
#endif

#ifdef HAS_STRING_H
#include <string.h>
#endif

#include <math.h>
#include <sys/param.h>
#include <sys/types.h>

#include <pm.h>
#include "saveload_def.h"
#include <memory.h>
#include <modellib.h>
#include <constants.h>


/*
 * load_go()	reads data from file in format determined by option
 * Returns 0 = successful load; -1 = cannot open or find file;
 *        -2 = illegal data present; -3 = cannot initialize memory;
 *	  -4 = current varb dim differs from file varb dim;
 *	  -5 = current param dim differs from file param dim;
 * Arguments: if *force_varb = FALSE then ask permission in case file varb_dim differs from current.
 * 	      Similarly with force_param.
 */
int keyword(char * word);
int load_unform_data(FILE    * fp);
int read_data_obj(FILE            * fp, memory ptr, int length);
int read_data_obj_1p1(FILE            * fp, memory ptr, int length);
int read_sys_name(FILE     * fp);
int set_pm_dvalue(FILE 		* fp, int data_dim, int cur_dim, char * pm_label);
int set_pm_ivalue(FILE 		* fp, int data_dim, int cur_dim, char * pm_label);

int
load_go(int	* force_varb, int	 *force_param)
{
  FILE          *fp,*fopen();
  
  int		status=0;
  char          *fname =  (char *) calloc(SIZE_OF_FNAME,sizeof(char)); 
  char          *dirname =  (char *) calloc(SIZE_OF_DIR_PLUS_FNAME,sizeof(char));

  dirname = strcat(strcat((char *)pm(GET, "Load.Directory", dirname, 0), "/"), 
		   (char *)pm(GET, "Load.Filename", fname, 0) );
  if ( check_file_to_read(dirname) )	
    {
      fp = fopen(dirname,"r");
      if ( *((int *)pm(GET, "Load.Format_Flag", 0)) == 0)
	status = load_form_data(fp, force_varb, force_param);
      else if ( *((int *)pm(GET, "Load.Format_Flag", 0)) == 1)
	status = load_oldform_data(fp, force_varb, force_param);
      else
	status = load_unform_data(fp);
      fclose(fp);
    }
  else				/* file does not exist */
    status = -1;

  free(fname);
  free(dirname);
  return( status );
}

/*
 * load_form_data()
 *
 */
int
  load_form_data(fp, force_varb, force_param)
FILE		*fp;
int		*force_varb, *force_param; 
{
  char word[256], c;
  int kw, keep_going = 0;

  while ( (fscanf(fp,"%s",word) != EOF) && (keep_going==0) )
    {
      switch(kw = keyword(word))
	{
	case COMMENT:
	  /* skip to end of line */
	  while (( (c=getc(fp)) != '\n') && (c != EOF));
	  break;
	case POSTMASTER:
	  /* interpret as postmaster command */
	  load_pm(fp);
	  break;
        case SET:
	  load_set(fp);
	  break;
	default:
	  /* print error message and skip to end of line */
/*	  system_mess_proc(0,"Illegal command in input file."); */
	  fprintf(stderr,"Illegal command %s in input file.\n",word);
	  while (( (c=getc(fp)) != '\n') && (c != EOF));
	  break;
	}
    }

  return(0);
}

int
keyword(char * word)
{
  int i;
  
  for (i=0; i< N_KEYWORD; i++)
    if (!strcmp(word,key[i].word))
      return(key[i].index);
  return(NO_KEY);
}

int
  load_pm(fp)
FILE *fp;
{
  char cmd[MAX_LABEL_LEN], str[MAX_LABEL_LEN], sdata[MAX_LONG_STR], *sdata_p, c;
  int kw, status, pmtype, idata, i, n,lower, *ildata, len;
  double ddata, *dldata;

  if (fscanf(fp, "%s %s", cmd, str) != 2) return;

  pmtype = pm_type(str, 0, 0);
  if (pmtype==0)
    {
      while (( (c=getc(fp)) != '\n') && (c != EOF));
      return;
    }
  switch(kw = pm_keyword(cmd))
    {
    case EXEC:
/*      fprintf(stderr, "pm(EXEC, %s, 0)\n", str);*/
      pm(EXEC, str, 0);
      break;
    case PUT:
      switch (pmtype)
	{
	case INT:
	  status = fscanf(fp, "%d", &idata);
/*	  fprintf(stderr, "pm(PUT, %s, %d, 0)\n", str, idata);*/
	  pm(PUT, str, idata, 0);
	  break;
	case DBL:
	  status = fscanf(fp, "%lf",&ddata);
/*	  fprintf(stderr, "pm(PUT, %s, %lf, 0)\n", str, ddata); */
	  pm(PUT, str, ddata, 0);
	  break;
	case STRNG:
	  fgets((char *) sdata, MAX_LONG_STR, fp);
	  sdata_p = sdata;
/*	  status = fscanf(fp, "%s", sdata);*/
	  /* fprintf(stderr, "pm(PUT, %s, %d, 0)\n", str, idata); */
/*	  pm(INIT, str, strlen(sdata)+1,*/

	  /* strip off leading whitespace */
	  while (isspace((int)*sdata_p++)) 
	      ;
	  sdata_p--;

	  /* strip off leading " if present */
	  if( sdata_p[0] == '"' )
	      sdata_p++;

	  /* clear trailing newline if present */
	  if (((len = strlen(sdata_p)) >0) && 
	      (sdata_p[len-1] == '\n'))
	      sdata_p[len-1] = '\0';

	  /* strip off trailing whitespace if present */
	  while (((len = strlen(sdata_p)) >0) && 
		 isspace(sdata_p[len -1]))
	      sdata_p[len-1] = '\0';

	  /* strip trailing " if present */
	  if (((len = strlen(sdata_p)) >0) && 
	      (sdata_p[len-1] == '"'))
	      sdata_p[len-1] = '\0';
	      

	  pm(PUT, str, sdata_p, 0);
	  break;
	case ADDRS:
	case MEMRY:
	    case FNCT:
	  system_mess_proc(0,"load_pm: cannot transfer this data type.");
	  break;
	case 0:
	  system_mess_proc(0,"load_pm: received an unknown pm object.");
	  break;
	default:
	  system_mess_proc(0,"load_pm: this pm type is not recognized.");
	  break;
	}
      break;
  case PUT_LIST:
      switch (pmtype)
	{
	case INT_LIST:
	  status = fscanf(fp,"%d %d", &lower, &n);
	  pm(INIT, str, n+1, 0);
	  for (i=0; i<=n; i++) 
	    {
	      status = fscanf(fp, "%d", &idata);
	      pm(PUT, str, i, idata, 0);
	    }
	  /* fprintf(stderr, "pm(PUT_LIST, %s, %d, [ ",str,n);
	     for(i=0; i<n; i++) fprintf(stderr,"%d ", ildata[i]);
		 fprintf(stderr, "], 0)\n"); */
	  break;
	case DBL_LIST:
	  status = fscanf(fp, "%d %d", &lower, &n);
	  pm(INIT, str, n+1, 0);
	  for (i=0; i<=n; i++)
	    {
	      status = fscanf(fp, "%lf", &ddata);
	      pm(PUT, str, i, ddata, 0);
	      /* fprintf(stderr, "pm(PUT_LIST, %s, %d,[ ",str,n);
		 for(i=0; i<n; i++) fprintf(stderr,"%lf ", dldata[i]);
		     fprintf(stderr, "], 0)\n"); */
	    }
	  break;
	case STRNG_LIST:
	  system_mess_proc(0,"load_pm: this pm type not loadable yet.");
	  break;
	default:
	  system_mess_proc(0,"load_pm: this pm type is not recognized.");
	  break;
	}
      break;
    case INIT:
      system_mess_proc(0, "Initializing not yet allowed in input files.");
      while (( (c=getc(fp)) != '\n') && (c != EOF));
      break;
    case CLEAR:
      system_mess_proc(0, "Clearing not yet allowed in input files.");
      while (( (c=getc(fp)) != '\n') && (c != EOF));
      break;
    case CREATE_OBJ:
    case CREATE_ELEM:
      /* not implemented - skip rest of line */
      system_mess_proc(0, "Creation not yet allowed in input files.");
      while (( (c=getc(fp)) != '\n') && (c != EOF));
      break;
    case GET:
    case GET_LIST:
    default:
      /* give error msg and skip rest of line */
      system_mess_proc(0,"Illegal pm command in input file");
      while (( (c=getc(fp)) != '\n') && (c != EOF));
      break;
    }
}

int
  load_set(fp)
FILE *fp;
{

    char
	*pm_result,
	word[MAX_SHORT_STR],
	cur_mem_str[MAX_SHORT_STR];

    int		
	keep_going = 0;		/* value 0 means continue !! */

    pm_result = (char *)pm(GET,"Cur_Memory.Mem_Type",cur_mem_str,0);

    fscanf(fp,"%s",word); /* new_object or func_object */
    if 	(!strcmp(word,"func_object"))
	keep_going = skip_function_obj (fp);
    else if (!strcmp(word,"new_object")) {
	/* temporary */
	if (!strcmp(cur_mem_str,"Traj"))
	    keep_going = fill_memory_obj(fp, "Memory.Traj", 
					 DEFAULT_TRAJ_LENGTH);
	else if (!strcmp(cur_mem_str,"Mult"))
	    keep_going = fill_memory_obj(fp, "Memory.Mult", 
					 DEFAULT_MULT_LENGTH);
	else if (!strcmp(cur_mem_str,"Fixed"))
	    keep_going = fill_memory_obj(fp, "Memory.Fixed", 
					 DEFAULT_FP_LENGTH);
	else if (!strcmp(cur_mem_str,"Cont"))
	    keep_going = fill_memory_obj(fp, "Memory.Cont", 
					 DEFAULT_CONT_LENGTH);
	else if (!strcmp(cur_mem_str,"Param"))
	    keep_going = fill_memory_obj(fp, "Memory.Param", 
					 DEFAULT_PARAM_LENGTH);
	else if (!strcmp(cur_mem_str,"Sel_Pt"))
	    keep_going = fill_memory_obj(fp, "Memory.Sel_Pt", 
					 DEFAULT_SEL_PT_LENGTH);
    }
    return keep_going;
	
}

/* 
   read next two words ( both of which should be a "{" ) and
   continue reading until both these brackets are closed
*/   
int
  skip_function_obj(fp)

FILE
   *fp;
{
    int
	c,
	depth = 0;		/* depth of brackets */
    char 
	word[MAX_SHORT_STR];

    fscanf(fp,"%s",word);
    if (strcmp(word,"{"))
	return MINOR_ERROR;
    else
	depth = 1;
    
    fscanf(fp,"%s",word);
    if (strcmp(word,"{"))
	return MINOR_ERROR;
    else
	depth = 2;

    while ((depth > 0) && ((c = fgetc(fp)) != EOF)) 
	if (c == '{')
	    ++depth;
	else if (c == '}')
	    --depth;

    if (c == EOF)
	return MINOR_ERROR;
    else	    
	return NO_ERROR;
}

int
  pm_keyword(word)
char *word;
{
  int i;
  
  for (i=0; i< N_PMKEY; i++)
    if (!strcmp(word,pm_key[i].word))
      return(pm_key[i].index);
  return(NO_KEY);
}

/* 
 * load_oldform_data()	reads data from file in dstool format.
 * Arguments: if force_varb = FALSE then ask permission in case file varb_dim differs from current
 * 	      Similarly with force_param.
 * Returns error code to load_go().
 */
int
  load_oldform_data(fp, force_varb, force_param)
FILE		*fp;
int		*force_varb, *force_param; 
{
  int		n;
  int		data_v_dim, data_p_dim, data_f_dim;
  char		word[40];
  int		cur_v_dim, cur_p_dim;
  int		cur_f_dim;
  int		keep_going = 0, kw, ivalue;
  double        dvalue;
  
      while ( (fscanf(fp,"%s",word) != EOF) && (keep_going==0) )
	{
	  switch(kw = old_keyword(word))
	    {
	    case System_Name:
	      if ( (n=read_sys_name(fp)) >= 0 )
		{
		  pm(PUT, "Model.Load_Number", n,
		     EXEC, "Model.Load", 
		     0);
		  cur_f_dim  = *((int *)pm(GET, "Model.Funct_Dim", 0));
		}
	      break;
	    case Varb_Dim:
	      fscanf(fp,"%d", &data_v_dim); 
	      cur_v_dim = *((int *)pm(GET, "Model.Varb_Dim", 0));
	      if ( (data_v_dim != cur_v_dim) && !(*force_varb) )
			keep_going = -4;
	      break;
	    case Param_Dim:
	      fscanf(fp,"%d", &data_p_dim);
	      cur_p_dim = *((int *)pm(GET, "Model.Param_Dim", 0));
	      if ( (data_p_dim != cur_p_dim) && !(*force_param) )
		      keep_going = -5;
	      break;
	    case Function_Dim:
	      fscanf(fp,"%d", &data_f_dim);
	      break;

	    case Varb_Ic:	/*  double arrays of length v_dim for traj */
	      set_pm_dvalue(fp,data_v_dim, cur_v_dim, "Selected.Varb_Ic");
	      break;
	    case Varb_Fc:
	      set_pm_dvalue(fp,data_v_dim, cur_v_dim, "Selected.Varb_Fc");
	      break;
	    case Varb_Min:
	      set_pm_dvalue(fp,data_v_dim, cur_v_dim, "Defaults.Varb_Min");
	      break;
	    case Varb_Max:
	      set_pm_dvalue(fp,data_v_dim, cur_v_dim, "Defaults.Varb_Max");
	      break;

	    case Param:		/* double arrays of length p_dim for traj */
	      set_pm_dvalue(fp,data_p_dim, cur_p_dim, "Selected.Param_Ic");
	      break;
	    case Param_Min:
	      set_pm_dvalue(fp,data_p_dim, cur_p_dim, "Defaults.Param_Min");
	      break;
	    case Param_Max:
	      set_pm_dvalue(fp,data_p_dim, cur_p_dim, "Defaults.Param_Max");
	      break;

	    case Function_Min:	 /* double arrays of length f_dim for traj*/
	      set_pm_dvalue(fp,data_f_dim, cur_f_dim, "Defaults.Funct_Min");
	      break;
	    case Function_Max:
	      set_pm_dvalue(fp,data_f_dim, cur_f_dim, "Defaults.Funct_Max");
	      break;

	    case Vf_Period:			  /* doubles for periodic*/
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Fixed.Vf_Period", dvalue, 0);
	      break;
	    case Dups:
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Fixed.Dups", dvalue, 0);
	      break;
	    case Var_Conv:
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Fixed.Var_Conv", dvalue, 0);
	      break;
	    case Funct_Conv:
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Fixed.Funct_Conv", dvalue, 0);
	      break;
	    case FD_Step:
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Fixed.FD_Step", dvalue, 0);
	      break;
	    case Eigen_Dist:
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Fixed.Eigen_Dist", dvalue, 0);
	      break;

	    case Map_Period:			  /* ints for periodic */
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Map_Period", ivalue, 0);
	      break;
	    case Algorithm:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Algorithm", ivalue, 0);
	      break;
	    case Guess:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Guess", ivalue, 0);
	      break;
	    case Guess_Num:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Mc_Guesses", ivalue, 0);
	      break;
	    case Setting:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Setting", ivalue, 0);
	      break;
	    case Num_Iters:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Num_Iters", ivalue, 0);
	      break;
	    case Stab_Points:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Stab_Points", ivalue, 0);
	      break;
	    case Stab_Steps:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Stab_Steps", ivalue, 0);
	      break;
	    case Unstab_Points:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Unstab_Points", ivalue, 0);
	      break;
	    case Unstab_Steps:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Fixed.Unstab_Steps", ivalue, 0);
	      break;

	    case Stepsize:	 /* double for Flow_Control (orbits) */
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Flow.Stepsize", dvalue, 0);
	      break;	     
	    case  Diverg_Cutoff:
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Flow.Diverg_Cutoff", dvalue, 0);
	      break;	     
	    case Final_Time:
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Flow.Final_Time", dvalue, 0);
	      break;	     
 
	    case Start_Save_Points:		  /* ints for Flow_Control */
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Flow.Start_Save_Points", ivalue, 0);
	      break;	      
	    case Total_Iterates:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Flow.Total_Iterates", ivalue, 0);
	      break;	      
	    case Skip_Size:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Flow.Skip_Size", ivalue, 0);
	      break;	      
	    case Stopping_Condition:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Flow.Stopping_Condition", ivalue, 0);
	      break;	      

	    case Varb_Events:			  /* arrays for Flow_Control */
	      set_pm_ivalue(fp, data_v_dim, cur_v_dim, "Flow.Varb_Events");
	      break;
	    case Funct_Events:
	      set_pm_ivalue(fp, data_f_dim, cur_f_dim, "Flow.Funct_Events");
	      break; 
	    case Varb_Event_Values:
	      set_pm_dvalue(fp, data_v_dim, cur_v_dim, 
			    "Flow.Varb_Event_Values");
	      break;
	    case Funct_Event_Values:
	      set_pm_dvalue(fp, data_f_dim, cur_f_dim, 
			    "Flow.Funct_Event_Values");
	      break;

	    case Disp_Points:			  /* ints for Defaults */
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Defaults.Disp_Points", ivalue, 0);
	      break;	
	    case Clipping:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Defaults.Clipping", ivalue, 0);
	      break;	
	    case Recording:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Defaults.Recording", ivalue, 0);
	      break;	
	    case Def_Symbol_Index:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Defaults.Symbol_Index", ivalue, 0);
	      break;	
	    case Def_Size_Index:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Defaults.Size_Index", ivalue, 0);
	      break;	
	    case Precision:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Defaults.Precision", ivalue, 0);
	      break;	

	    case Mult_Load_Choice:		  /* ints for Mult_Cntl */
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Mult.Load_Choice", ivalue, 0);
	      break;
	    case Mult_Transformation:
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Mult.Transformation", ivalue, 0);
	      break;
	    case Images:      
	      fscanf(fp, "%d", &ivalue );
	      pm(PUT, "Mult.Images", ivalue, 0);
	      break;

	    case Mult_Trans_Param:		  /* double for Mult_Cntl */
	      fscanf(fp, "%lg", &dvalue );
	      pm(PUT, "Mult.Trans_Param", dvalue, 0);
	      break;

	    case Mult_Points:		      /* int arrays for Mult_Cntl */
	      set_pm_ivalue(fp, data_v_dim+data_p_dim, cur_v_dim+cur_p_dim, 
			    "Mult.Points");
	      break;

	    case Mult_Radius:		    /* double arrays for Mult_Cntl */
	      set_pm_dvalue(fp, data_v_dim+data_p_dim, cur_v_dim+cur_p_dim, 
			    "Mult.Radius");
	      break;

	    case Traj_Mem_Ptr:
	      keep_going = fill_memory_obj_1p1(fp, "Memory.Traj", 
					   DEFAULT_TRAJ_LENGTH);
	      break;	    
	    case Mult_Mem_Ptr:
	      keep_going = fill_memory_obj_1p1(fp, "Memory.Mult", 
					   DEFAULT_MULT_LENGTH);
	      break;
	    case Fixed_Mem_Ptr:
	      keep_going = fill_memory_obj_1p1(fp, "Memory.Fixed", 
					   DEFAULT_FP_LENGTH);
	      break;
	    case Cont_Mem_Ptr:
	      keep_going = fill_memory_obj_1p1(fp, "Memory.Cont", 
					   DEFAULT_CONT_LENGTH);
	      break;
	    case Param_Mem_Ptr:
	      keep_going = fill_memory_obj_1p1(fp, "Memory.Param", 
					   DEFAULT_PARAM_LENGTH);
	      break;
	    case Sel_Pt_Mem_Ptr:
	      keep_going = fill_memory_obj_1p1(fp, "Memory.Sel_Pt", 
					   DEFAULT_SEL_PT_LENGTH);
	      break;
	    case NEW_FUNC:
	      /* NOTE: function calls are not reloaded into memory.  
		 File CLOSED at this point */
	      /* Be sure that the functions are ALWAYS placed at the 
		 end of the data file so that we don't have to worry 
		 about filtering the function data. */
	      keep_going = 1;
	      break;
	    case CONFIGURATION:
	      load_config(fp);
	      break;
	    default:
	      break;
	    }	
	}

  return(keep_going);
}


/*
  fill_memory_obj()  initializes a pointer to a memory object and reads data from a file into that object.
  Returns 0 upon sucessful completion; -3 if cannot initialize memory.
  */
int
  fill_memory_obj(fp,ptr_type,length)
FILE *fp;
char *ptr_type;
int length;
{
  memory		ptr;
  
  if (!(ptr = (memory) pm(GET, ptr_type, 0)))
    {
      if ( *(int *)pm(INIT, ptr_type, 0) ) 
	return( -3 );
      else
	ptr = (memory) pm(GET, ptr_type, 0);
    }
  return( read_data_obj(fp,ptr,length) );
}

/*
  read_data_obj() reads from a file and loads data objects into memory.
  The objects handled are trajectories, fixed points, continuation data, parameter data, and selected points.
  */
int
read_data_obj(FILE            * fp, memory ptr, int length)
{
  char          word[SIZE_OF_GEN_INPUT];
  int           n_obj,n_pts,n_doubles,n_integers,n_dparams, n_iparams;
  int           *ilist,*iparams,*ivector();
  double	*dlist, *dparams, *dvector();
  int           keep_going,
  		first_obj = TRUE;
  
  /*  fscanf(fp,"%s %s %d %s %s %d %s",word,word,&n_obj,word,word,&n_dparams,word);*/ /* read num objects, num doubles in header */
  /*  fscanf(fp,"%s %s %d %s",word,word,&n_iparams,word); */ /* read number of integers in header */
  /*  fscanf(fp,"%s %s %d %s %d",word,word,&n_doubles,word,&n_integers); */
  /*  get_double_list_1p1(fp,n_dparams,dparams);*/
  /*  get_integer_list_1p1(fp,n_iparams,iparams); */
  n_obj = * ((int *) pm(GET,"Cur_Memory.Num_Objects",0));

  n_dparams = * ((int *) pm(GET,"Cur_Memory.Num_Header_Doubles",0));
  n_iparams = * ((int *) pm(GET,"Cur_Memory.Num_Header_Integers",0));

  dparams = dvector(0,n_dparams-1);
  pm(GET_LIST,"Cur_Memory.Header_Doubles",0,n_dparams - 1,dparams,0);


  iparams = ivector(0,n_iparams-1);
  pm(GET_LIST,"Cur_Memory.Header_Integers",0,n_iparams - 1,iparams,0);

  n_doubles = * ((int *) pm(GET,"Cur_Memory.Num_Body_Doubles",0));
  n_integers = * ((int *) pm(GET,"Cur_Memory.Num_Body_Integers",0));
 
  dlist = dvector(0,n_doubles-1);
  ilist = ivector(0,n_integers-1);

  if (memory_vanilla_start_new_flow(ptr, n_obj, n_doubles, n_integers, length, n_dparams, n_iparams) == 0) {
      memory_vanilla_add_point(ptr, 0, 0, dparams, iparams); /* set header info */
      while ( n_obj-- )	{
	  keep_going = TRUE;
	  if (first_obj)
	      fscanf(fp,"%s %s",word,word);	/* { \n { */
	  else
	      fscanf(fp,"%s",word);	/* { */
	  n_pts = *((int *)pm(GET,"Cur_Memory.Object_Num_Points",0));
	  /*	  if (first_obj) 
		  fscanf(fp,"%s %s %d %s",word,word,&n_pts,word); *//* don't assume n_pts pts follow; read till next traj */
	  /* else
	     fscanf(fp,"%s %d %s",word,&n_pts,word); *//* read one less string */
	  while( keep_going ) {
	      if ( n_doubles )   
		  keep_going = get_double_list(fp,n_doubles,dlist);
	      if ( n_integers && keep_going )
		  keep_going =  get_integer_list(fp,n_integers,ilist);
	      if ( keep_going )
		  memory_vanilla_add_point(ptr, dlist, ilist, 0, 0); /* do not change header; only body */
	    }
	  /*fscanf(fp,"%s",word);*/	/* trailing } read in point*/
	  memory_next_traj(ptr);
	  first_obj = FALSE;
      }
      memory_end_current_flow(ptr);
      fscanf(fp,"%s",word);	/* closing bracket "}" of set of objects */ 
  }
  free_dvector(dlist,0,n_doubles-1);
  free_ivector(ilist,0,n_integers-1);
  free_dvector(dparams,0,n_dparams-1);
  free_ivector(iparams,0,n_iparams-1);
  return( 0 );
}



/*
  fill_memory_obj()  initializes a pointer to a memory object and reads data from a file into that object.
  Returns 0 upon sucessful completion; -3 if cannot initialize memory.
  */
int
  fill_memory_obj_1p1(fp,ptr_type,length)
FILE *fp;
char *ptr_type;
int length;
{
  memory		ptr;
  
  if (!(ptr = (memory) pm(GET, ptr_type, 0)))
    {
      if ( *(int *)pm(INIT, ptr_type, 0) ) 
	return( -3 );
      else
	ptr = (memory) pm(GET, ptr_type, 0);
    }
  return( read_data_obj_1p1(fp,ptr,length) );
}


/*
  read_data_obj() reads from a file and loads data objects into memory.
  The objects handled are trajectories, fixed points, continuation data, parameter data, and selected points.
  */
int read_data_obj_1p1(FILE            * fp, memory ptr, int length)
{
  char          word[SIZE_OF_GEN_INPUT];
  int           n_obj,n_pts,n_doubles,n_integers,n_dparams, n_iparams;
  int           *ilist,*iparams,*ivector();
  double	*dlist, *dparams, *dvector();
  int           keep_going, first_obj = TRUE;
  
  fscanf(fp,"%s %s %d %s %s %d %s",word,word,&n_obj,word,word,&n_dparams,word); /* read num objects, num doubles in header */
  dparams = dvector(0,n_dparams-1);
  get_double_list_1p1(fp,n_dparams,dparams);
  fscanf(fp,"%s %s %d %s",word,word,&n_iparams,word); /* read number of integers in header */
  iparams = ivector(0,n_iparams-1);
  get_integer_list_1p1(fp,n_iparams,iparams);

  fscanf(fp,"%s %s %d %s %d",word,word,&n_doubles,word,&n_integers); 
  dlist = dvector(0,n_doubles-1);
  ilist = ivector(0,n_integers-1);

  if (memory_vanilla_start_new_flow(ptr, n_obj, n_doubles, n_integers, length, n_dparams, n_iparams) == 0)
    {
      memory_vanilla_add_point(ptr, 0, 0, dparams, iparams); /* set header info */
      while ( n_obj-- )
	{
	  keep_going = TRUE; 
	  if (first_obj) 
	    fscanf(fp,"%s %s %d %s",word,word,&n_pts,word); /* don't assume n_pts pts follow; read till next traj */
	  else
	    fscanf(fp,"%s %d %s",word,&n_pts,word); /* read one less string */
	  while( keep_going )
	    {
	      if ( n_doubles )   
		keep_going = get_double_list_1p1(fp,n_doubles,dlist);
	      if ( n_integers && keep_going )
		keep_going =  get_integer_list_1p1(fp,n_integers,ilist);
	      if ( keep_going )
		memory_vanilla_add_point(ptr, dlist, ilist, 0, 0); /* do not change header; only body */
	    }
	  memory_next_traj(ptr);
	  first_obj = FALSE;
	}
      memory_end_current_flow(ptr);
    }
  free_dvector(dlist,0,n_doubles-1);
  free_ivector(ilist,0,n_integers-1);
  free_dvector(dparams,0,n_dparams-1);
  free_ivector(iparams,0,n_iparams-1);
  return( 0 );
}


/*
  read_sys_name() reads the system name from a file.  If the system is currently loaded,
  -1 is returned.  If the system is not currently
  installed, the number of the system (in DS_Sel) is returned.  
  If the system name cannot be found, -2 is returned.
*/
int
read_sys_name(FILE     * fp)
{
  char      name[MAX_LEN_DS_TITLE], *modelname, *get_ds_name(), *pname;
  int       i;

  fgets( name, MAX_LEN_DS_TITLE, fp );		  /* read to end of line */
  pname = name;					  /* strip off leading white space */
  while ( isspace( (int)*pname++ ))
    ;
  pname--;
  modelname = get_ds_name();

  if ( !strncmp( pname, modelname, strlen(modelname) )) /* current system matches name in file */
    return -1;
  for( i=0; i<N_DS; i++)
    {
      modelname = DS_Sel[i].DS_Name;
      if ( !strncmp( pname, modelname, strlen(modelname) )) /* compare at most strlen(modelname) chars. */
	return( i );				  /* This eliminate mismatch because of junk or whitespace */
    }						  /*  at the end of name */
  return( -2 );
}

/*     
  get_integer_list_1p1() fills up an integer array with integers read from a file
  Function returns TRUE if it properly fills array.
  Function returns FALSE if next line is a text line (eg, a new trajectory)
  */
int
  get_integer_list(fp,data_dim,p)
FILE 		*fp;
int 		data_dim;
int 		*p;
{
  int		i;
  char		word[SIZE_OF_GEN_INPUT];
/*  int         	atoi(); */
  
  
  if ( (data_dim == 0) || (fscanf(fp,"%s",word)==EOF)  || (strcmp(word,"{")) ) /* data strats with { */
    return(FALSE);

/*  *p = atoi(word); */
  for (i=0; i<data_dim; i++)
    fscanf(fp,"%d", (p + i) );
  fscanf(fp,"%s",word);		/* read trailing } at end of point */
  return(TRUE);  
}

/*     
  get_double_list() fills up a double array with double read from a file
  Function returns TRUE if it properly fills array.
  Function returns FALSE if next line is a text line (eg, a new trajectory)
  */
int
  get_double_list(fp,data_dim,p)
FILE 		*fp;
int 		data_dim;
double 		*p;
{
  int		i;
  char		word[SIZE_OF_GEN_INPUT];
/*  double	atof(); */
  
  if ( (data_dim==0) || (fscanf(fp,"%s",word)==EOF)  || strcmp(word,"{") ) /* data strats with { */
    return(FALSE);
/*  *p = atof(word); */
  for (i=0; i<data_dim; i++)
    fscanf(fp,"%lg", (p + i) );
  fscanf(fp,"%s",word);		/* read trailing } at end of point */
  return(TRUE);
}

/*     
  get_integer_list_1p1() fills up an integer array with integers read from a file
  Function returns TRUE if it properly fills array.
  Function returns FALSE if next line is a text line (eg, a new trajectory)
  */
int
  get_integer_list_1p1(fp,data_dim,p)
FILE 		*fp;
int 		data_dim;
int 		*p;
{
  int		i;
  char		word[SIZE_OF_GEN_INPUT];
/*  int         	atoi(); */
  
  
  if ( (data_dim == 0) || (fscanf(fp,"%s",word)==EOF)  || (!strcmp(word,"#")) ) /* new text line begins with pound sign*/
    return(FALSE);

  *p = atoi(word); 
  for (i=1; i<data_dim; i++)
    fscanf(fp,"%d", (p + i) );
  return(TRUE);  
}

/*     
  get_double_list_1p1() fills up a double array with double read from a file
  Function returns TRUE if it properly fills array.
  Function returns FALSE if next line is a text line (eg, a new trajectory)
  */
int
  get_double_list_1p1(fp,data_dim,p)
FILE 		*fp;
int 		data_dim;
double 		*p;
{
  int		i;
  char		word[SIZE_OF_GEN_INPUT];
/*   double	atof(); */
  
  if ( (data_dim==0) || (fscanf(fp,"%s",word)==EOF)  || (!strcmp(word,"#")) ) /* new text line begins with pound sign */
    return(FALSE);
  *p = atof(word); 
  for (i=1; i<data_dim; i++)
    fscanf(fp,"%lg", (p + i) );
  return(TRUE);
}


/*	
  set_pm_dvalue() sets the appropriate postmaster double precision variable.
  This is not intended to be a general purpose function, just a convenient way to 
  read in many different variable types with minimal effort.
  */
int set_pm_dvalue(FILE 		* fp, int data_dim, int cur_dim, char * pm_label)
{
  double		value;
  int 		i;
  
  for (i=0;i< min(data_dim,cur_dim);i++)
    {						  /* allow for data varbs to be different dim than current system */
      if (fscanf(fp,"%lg",&value)==1)
	pm(PUT, pm_label, i, value, 0);
    }
}

/*	
  set_pm_ivalue() sets the appropriate postmaster integer variable.
  This is not intended to be a general purpose function, just a convenient way to 
  read in many different variable types with minimal effort.
  */
int set_pm_ivalue(FILE 		* fp, int data_dim, int cur_dim, char * pm_label)
{
  int 		i, value;
  
  for (i=0;i< min(data_dim,cur_dim);i++)
    {						  /* allow for data varbs to be different dim than current system */
      if (fscanf(fp,"%d",&value)==1)
	pm(PUT, pm_label, i, value, 0);
    }
}

/*     
  old_keyword() returns NO_KEY if the given string is not in a list of
  key words.  If the word is in the list, it returns the integer position of the word on the list
  */
int
  old_keyword(w)
char    *w;
{
  int		i;
  char		*allupper();
  
  allupper(w);
  for (i=0; i< N_OLDKEYWORD; i++)
    if (!strcmp(w,old_key[i].word))
      return(old_key[i].index);
  return(NO_KEY);
}

/*
  allupper() converts a string to all uppercase characters.  The original string is
  lost in the process.  Ex: if word points to "HelLo WorLd!#1" then allupper(word)
  returns a pointer to "HELLO WORLD!#1".
  */
char *
  allupper(w)
char *w;
{
  char *p;
  
  p = w;
  while ( (*w++ = toupper(*w)) != '\0')
    ;
  return( p );
}

int
    load_model_if_new()
{
  char      	load_name[MAX_LEN_DS_TITLE], *model_name,
  		*get_ds_name();

  int		i;

  pm(GET,"Load.Model_Name",load_name,0);
  model_name = get_ds_name();

  if (! strncmp(load_name,model_name,strlen(model_name)))
      return NO_ERROR;
  for( i=0; i<N_DS; i++) {
      model_name = DS_Sel[i].DS_Name;
      if ( !strncmp( load_name, model_name, strlen(model_name) )) /* compare at most strlen(modelname) chars. */
	  pm(PUT, "Model.Load_Number", i,
	     EXEC, "Model.Load", 
	     0);
/*	  load_model();*/	
  }
  return -2;
      
}
