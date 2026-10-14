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
 * init_tcl.c
 *
 * Procedures:
 *   init_tcl()
 *   tcl_to_pmput()
 *   tcl_to_pmexec()
 *
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

#include <tcl.h>

#include <constants.h>

extern int tcl_to_pm();
extern int tcl_to_pmput();
extern int tcl_to_pmget();
extern int tcl_to_pmexec();
extern int tcl_to_dsput();
extern int tcl_SET();


void
  init_tcl(interp)
Tcl_Interp *interp;
{
  Tcl_CreateCommand(interp, "pm",tcl_to_pm,0,0);
  Tcl_CreateCommand(interp, "SET",tcl_SET,0,0);
/*  Tcl_CreateCommand(interp, "pmput",tcl_to_pmput,0,0);
  Tcl_CreateCommand(interp, "pmget",tcl_to_pmget,0,0);
  Tcl_CreateCommand(interp, "pmexec",tcl_to_pmexec,0,0);
  Tcl_CreateCommand(interp, "dsput",tcl_to_dsput,0,0); */
}


int
  tcl_SET(clientdata, interp, argc, argv)
ClientData clientdata;
Tcl_Interp *interp;
int argc;
char *argv[];
{
    int status = NO_ERROR;
    static char 
	empty_str[1] = "",
	set_str[5] = "set ",
	blank_str[2] = " ",
        quote_str[2] = "\"";
  

    if (argc != 3) 
	status = MINOR_ERROR;
    else {
	status = Tcl_VarEval(interp,set_str, argv[1], blank_str, quote_str,argv[2], quote_str,(char *) 0);
	if (status == TCL_OK) {
	    status = NO_ERROR;
/*	    if ((! strcmp(argv[1],"new_object")) || 
		(! strcmp(argv[1],"func_object")))
		load_set_from_str(argv[1],argv[2]); */
	}
	else
	    status = MINOR_ERROR;
	
    }

    if (status != NO_ERROR) {
	fprintf(stderr,"%s %s\n",argv[1],argv[2]);
	fprintf(stderr, "tcl_SET: usage: SET new_object list \n");
    }
/*    sprintf(interp.result,"%s",(char *) empty_str); */
    return TCL_OK;

}

int
  tcl_to_pm(clientdata, interp, argc, argv)
ClientData clientdata;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  int pmtype, i=3, status=0, kw, idata, n, j, lower, upper;
  double ddata;
  char *str, *sdata, result[MAX_LONG_STR];

  if (argc<2) status = 1;
  else if ( (pmtype = pm_type((str=argv[2]), 0, 0)) != 0)
    {
      switch(kw = pm_keyword(argv[1]))
	{
	case EXEC:
	  pm(EXEC, str, 0);
	  break;
        case PUT:
	  switch (pmtype)
	    {
	    case INT:
	      idata = atoi(argv[i++]);
	      pm(PUT, str, idata, 0);
	      break;
	    case DBL:
	      ddata = atof(argv[i++]);
	      pm(PUT, str, ddata, 0);
	      break;
	    case STRNG:
	      sdata = argv[i++];
	      pm(INIT, str, strlen(sdata)+1, 
		 PUT, str, sdata, 0);
	      break;
	    case INT_LIST: 
	      j = atoi(argv[i++]);
	      idata = atoi(argv[i++]);
	      pm(PUT, str, j, idata, 0);
	      break;
	    case DBL_LIST:	/* Ditto */
	      j = atoi(argv[i++]);
	      ddata = atof(argv[i++]);
	      pm(PUT, str, j, ddata, 0);
	      break;
	    case STRNG_LIST:
	      system_mess_proc(0,"tcl_to_pm: this pm type not loadable yet.");
	      break;
	    case ADDRS:
	    case MEMRY:
	    case FNCT:
	      system_mess_proc(0,"tcl_to_pm: cannot transfer this data type.");
	      break;
	    case 0:
	      system_mess_proc(0,"tcl_to_pm: received an unknown pm object.");
	      break;
	    default:
	      system_mess_proc(0,"tcl_to_pm: this pm type is not recognized.");
	      break;
	    }
	  break;
        case INIT:
	  system_mess_proc(0, "Initializing not yet allowed in tcl commands.");
	  break;
        case CLEAR:
	  system_mess_proc(0, "Clearing not yet allowed in tcl commands.");
	  break;
        case CREATE_OBJ:
        case CREATE_ELEM:
	  /* not implemented - skip rest of line */
	  system_mess_proc(0, "Creation not yet allowed in tcl commands.");
	  break;
        case PUT_LIST:
	  switch (pmtype)
	    {
	    case INT_LIST: /* Isn't this really a PUT_LIST operation ? */
			   /* Put for an INT_LIST just sets one value as below.*/
	      /* n = argc-i;*/
		
	      lower = atoi(argv[i++]);
	      upper = atoi(argv[i++]);
	      pm(INIT, str, upper + 1, 0); 
	      for (j=lower; j<=upper; j++) 
		{
		  idata = atoi(argv[i++]);
		  pm(PUT, str, j, idata, 0);
		}
	      break;
	    case DBL_LIST:	/* Ditto */
	      n = argc-i;
	      lower = atoi(argv[i++]);
	      upper = atoi(argv[i++]);
	      pm(INIT, str, upper + 1, 0); 
	      /* pm(INIT, str, n, 0);*/
	      for (j=lower; j<=upper; j++)
		{
		  ddata = atof(argv[i++]);
		  pm(PUT, str, j, ddata, 0);
		}
	      break;
	    case STRNG_LIST:
	      system_mess_proc(0,"tcl_to_pm: this pm type not loadable yet.");
	      break;
	    case ADDRS:
	    case MEMRY:
	    case FNCT:
	      system_mess_proc(0,"tcl_to_pm: cannot transfer this data type.");
	      break;
	    case 0:
	      system_mess_proc(0,"tcl_to_pm: received an unknown pm object.");
	      break;
	    default:
	      system_mess_proc(0,"tcl_to_pm: this pm type is not recognized.");
	      break;
	    }
	  break;
        case GET:
	  switch (pmtype)
	    {
	    case INT:
	      idata = *((int *) pm(GET, str, 0));
	      sprintf(interp->result,"%d",idata);
	      /*Tcl_SetResult(interp,result,TCL_VOLATILE);*/
	      break;
	    case DBL:
	      ddata = *((double *) pm(GET, str, 0));
	      sprintf(result,"%g",ddata);
	      Tcl_SetResult(interp,result,TCL_VOLATILE);
	      break;
	    case STRNG:
	      sdata = argv[i++];
	      pm(GET, str, result,0);
	      Tcl_SetResult(interp,result,TCL_VOLATILE);
	      break;
	    case INT_LIST: 
	      j = atoi(argv[i++]);
	      idata = *((int *) pm(GET, str, j, 0));
	      sprintf(result,"%d",idata);
	      Tcl_SetResult(interp,result,TCL_VOLATILE);
	      break;
	    case DBL_LIST:	/* Ditto */
	      j = atoi(argv[i++]);
	      ddata = *((double *) pm(GET, str, j, 0));
	      sprintf(result,"%g",ddata);
	      Tcl_SetResult(interp,result,TCL_VOLATILE);
	      break;
	    case STRNG_LIST:
	      j = atoi(argv[i++]);
	      pm(GET, str,j, result,0);
	      Tcl_SetResult(interp,result,TCL_VOLATILE);
	      break;
	    case ADDRS:
	    case MEMRY:
	    case FNCT:
	      system_mess_proc(0,"tcl_to_pm: cannot GET this data type.");
	      break;
	    case 0:
	      system_mess_proc(0,"tcl_to_pm: received an unknown pm object.");
	      break;
	    default:
	      system_mess_proc(0,"tcl_to_pm: this pm type is not recognized.");
	      break;
	    }
	  break;
        case GET_LIST:
        default:
	  /* give error msg and skip rest of line */
	  system_mess_proc(0,"GET_LIST, or other not implemented.");
	  break;
	}
    }
  
  if (status != 0)
    { 
      fprintf(stderr, "tcl_to_pm: usage: pm [PUT | EXEC] object.element\n");
    }

  return TCL_OK;
}



int
  tcl_to_dsput(clientdata, interp, argc, argv)
ClientData clientdata;
Tcl_Interp interp;
int argc;
char *argv[];
{
  int i,j, nvars, nparams, obj_index;
  char name[MAX_LEN_VARB_NAME];
  char *obj_type;

  nvars = *((int *) pm(GET, "Model.Varb_Dim", 0));
  nparams = *((int *) pm(GET, "Model.Param_Dim", 0));

  for (i=1; i<argc; i++)
    {
      obj_index = -1;
      /* is it a variable ? */
      for (j=0; j<nvars && obj_index<0; j++)
	{
	  pm(GET, "Model.Varb_Names", j, name, 0);
	  if (strcmp(name,argv[i]) == 0)
	    {
	      obj_type = "Selected.Varb_Ic";
	      obj_index = j;
	    }
	}
      /* is it a parameter ? */
      for (j=0; j<nparams && obj_index<0; j++)
	{
	  pm(GET, "Model.Param_Names", j, name, 0);
	  if (strcmp(name,argv[i]) == 0)
	    {
	      obj_type = "Selected.Param_Ic";
	      obj_index = j;
	    }
	}
      if (obj_index <0)
	{
	  fprintf(stdout,"dstool: tcl_to_pm: Invalid name: %s\n",argv[i]);
	}
      else if (++i == argc)
	{
	  fprintf(stdout,"dstool: tcl_to_pm: No value supplied \n");
	}
      else
	/* fprintf(stdout,"dstool: tcl_to_pm: setting %s %d %d %lf\n",
		argv[i-1],obj_type, obj_index, atof(argv[i])); */
	pm(PUT, obj_type, obj_index, atof(argv[i]), 0);
    }

  return TCL_OK;
}


int
  tcl_to_pmexec(clientdata, interp, argc, argv)
ClientData clientdata;
Tcl_Interp interp;
int argc;
char *argv[];
{
  int i;

  for (i=1; i<argc; i++)
    {
      pm(EXEC, argv[i], 0);
    }
  return TCL_OK;
}


int
  tcl_to_pmput(clientdata, interp, argc, argv)
ClientData clientdata;
Tcl_Interp interp;
int argc;
char *argv[];
{
  int i=0, j, pmtype, n;
/*  double atof(); */
    static char 
	empty_str[1] = "";
  char *str;

  while (argc > (++i)+1)
    {
      str = argv[i++];
      pmtype = pm_type(str, 0, 0);
      switch(pmtype)
	{
	case INT:
	  pm(PUT, str, atoi(argv[i]), 0);
	  break;
	case DBL:
	  pm(PUT, str, atof(argv[i]), 0);
	  break;
	case STRNG:
	  pm(PUT, str, argv[i], 0);
	  break;
	case INT_LIST:
	  n = argc -i-1;
	  pm(INIT, str, n, 0);
	  for (j=0; j<n; j++, i++)
	    pm(PUT, str, j, atoi(argv[i]), 0);
	  break;
	case DBL_LIST:
	  n = argc -i-1;
	  pm(INIT, str, n, 0);
	  for (j=0; j<n; j++, i++)
	    pm(PUT, str, j, atof(argv[i]), 0);
	  break;
	case STRNG_LIST:
	  n = argc -i-1;
	  pm(INIT, str, n, 0);
	  for (j=0; j<n; j++, i++)
	    pm(PUT, str, j, argv[i], 0);
	  break;
	case ADDRS:
	case MEMRY:
	case FNCT:
	  system_mess_proc(0,"rcv_pm: cannot transfer this type of data!");
	  break;
	case 0:
	  system_mess_proc(0,"rcv_pm: received an unknown postmaster oject.");
	  break;
	default:
	  system_mess_proc(0,"rcv_pm: this pm type is not recognized.");
	  break;
	}
    }
      sprintf(interp.result,"%s",empty_str);
      return TCL_OK;
}


int
  tcl_to_pmget(clientdata, interp, argc, argv)
ClientData clientdata;
Tcl_Interp interp;
int argc;
char *argv[];
{
 return TCL_OK;
}
