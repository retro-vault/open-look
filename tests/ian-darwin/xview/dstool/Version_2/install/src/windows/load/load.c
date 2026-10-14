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
 * load.c - Notify and event callback functions.
 */
#include <stdio.h>
#include <math.h>
#include <sys/param.h>
#include <sys/types.h>
#include <xview/notice.h>
#include <gdd.h>

#include <ui_init.h>
#include <constants.h>
#include <pm.h>
#include <saveload.h>
#include <twoD.h>
#include <twoD_opt.h>
#include <memory.h>
#include <modellib.h>
#include "load_ui.h"

static load_win_objects *load_ip = 0;

/*
 * Menu handler for `filemenu (Load ...)'.
 */
Menu_item
load_handler(Menu_item item, Menu_generate op)
{
    int
	locn[4];
    if (op == MENU_NOTIFY)
	if (item != (Menu_item) 0)
	    load_open(DEFAULT_WIN_CONFIG,0,0,0,0);
	else {
	    pm(GET_LIST,"Win.Locn.Load",0,3,locn,0);
	    if ((locn[0] != NO_LOCATION))
		load_open(SET_WIN_CONFIG,locn[0],locn[1],locn[2],locn[3]);
	    else
		load_open(DEFAULT_WIN_CONFIG,0,0,0,0);
	}
  if(op == MENU_NOTIFY)
    load_open(DEFAULT_WIN_CONFIG,0,0,0,0);
  return item;
}


/* 
 * load_open()  displays the load window, creating it if necessary.
 */
int
  load_open(use_default,left,top,width,height)
int   use_default;    /* either SET_WIN_CONFIG  or  DEFAULT_WIN_CONFIG */
int   left;           /* position (on screen) of left edge of window.  */
int   top;            /* position (on screen) of top edge of window.   */
int   width;          /* width of window;  width <= 0 means use default width */
int   height;         /* height of window; height <= 0 means use default width */
{
  Rect  *rect;
  
  if (!load_ip)
    {
      load_ip = load_win_objects_initialize(0, cmd_ip->win);
      register_win_and_type("Load",load_ip->win,POPUP_WINDOW);
      pm(PUT_SAVABLE,"Win_Names.Load",SAVE_NONE,
	 0);
      load_field_manager();	/* calls load_init() */
    }
  if (use_default == SET_WIN_CONFIG)
    {
      rect = (Rect *)calloc(1,sizeof(Rect));
      frame_get_rect(load_ip->win,rect); /* get the current configuration */
      rect->r_left = (short) left;
      rect->r_top = (short) top;
      if (width>0) rect->r_width = (short) width;
      if (height>0) rect->r_height = (short) height;
      frame_set_rect(load_ip->win,rect); /* set the current configuration */
      free(rect);
    }
  load_data_refresh();
  mark_window_open("Load");
  xv_set(load_ip->win, FRAME_CMD_PUSHPIN_IN, TRUE, 0);
  xv_set(load_ip->win, WIN_SHOW, TRUE, WIN_FRONT, 0);
}

int
load_close(void)
{
    mark_window_closed("Load");
    if(load_ip) {
	xv_set(load_ip->win, FRAME_CMD_PUSHPIN_IN, FALSE, 0);
        xv_set(load_ip->win, XV_SHOW, FALSE, 0);	
    }
}

/*
 * load_data_refresh() uses postmaster info to update load window
 */
int load_data_refresh(void)
{
  load_win_objects *ip = load_ip;
  char		strng[SIZE_OF_DIR_PLUS_FNAME];
  int		i, v_dim, p_dim, *v_box, *p_box, *ivector();

  if (!ip ) return 0; 

  pm(GET, "Load.Directory", strng, 0);
  xv_set(ip->directory, PANEL_VALUE, strng, 0);
  pm(GET, "Load.Filename", strng, 0);
  xv_set(ip->filename, PANEL_VALUE, strng, 0);
  pm(GET, "Load.Data_Type", strng, 0);
  if (strcmp(strng,"Memory.Traj"))
    xv_set(ip->type, PANEL_VALUE, 0, 0);
  else if (strcmp(strng,"Memory.Fixed"))
    xv_set(ip->type, PANEL_VALUE, 1, 0);
  else if (strcmp(strng,"Memory.Cont"))
    xv_set(ip->type, PANEL_VALUE, 2, 0);
  else if (strcmp(strng,"Memory.Param"))
    xv_set(ip->type, PANEL_VALUE, 3, 0);
  else if (strcmp(strng,"Memory.Sel_Pt"))
    xv_set(ip->type, PANEL_VALUE, 4, 0);
  else xv_set(ip->type, PANEL_VALUE, 0, 0);

  xv_set(ip->color,PANEL_VALUE, *((int *)pm(GET, "Load.Color_Flag", 0))? 0:1, 0);
  xv_set(ip->symbol,PANEL_VALUE, *((int *)pm(GET, "Load.Symbol_Flag", 0))? 0:1, 0);

  v_dim = *((int *)pm(GET, "Model.Varb_Dim", 0));
  p_dim = *((int *)pm(GET, "Model.Param_Dim", 0));
  v_box = ivector(0,v_dim-1); 
  p_box = ivector(0,p_dim-1); 
  pm(GET_LIST, "Load.Varb_Index", 0, v_dim-1, v_box, 0);
  pm(GET_LIST, "Load.Param_Index", 0, p_dim-1, p_box, 0);
  xv_set(ip->varb, XV_SHOW, FALSE, 0); /* so that refresh doesn't scroll the list */
  for(i=v_dim-1; i>=0; i--)	
    xv_set(ip->varb,PANEL_LIST_SELECT, i, v_box[i], 0);
  xv_set(ip->varb, XV_SHOW, TRUE, 0); 
  xv_set(ip->param, XV_SHOW, FALSE, 0); /* so that refresh doesn't scroll the list */
  for(i=p_dim-1; i>=0; i--)
    xv_set(ip->param,PANEL_LIST_SELECT, i, p_box[i], 0);
  xv_set(ip->param, XV_SHOW, TRUE, 0);

  i = *((int *)pm(GET, "Load.Format_Flag", 0));
  xv_set(ip->option,PANEL_VALUE, i, 0);

  if (i<2) 
    {
      xv_set(ip->win, XV_HEIGHT, 175, 0);     /* hide bottom of panel */
      xv_set(ip->color,  PANEL_INACTIVE, TRUE, 0);
      xv_set(ip->symbol, PANEL_INACTIVE, TRUE, 0);
      xv_set(ip->type,   PANEL_INACTIVE, TRUE, 0);
      xv_set(ip->varb,   PANEL_INACTIVE, TRUE, 0);
      xv_set(ip->param,  PANEL_INACTIVE, TRUE, 0);
    }
  else 
    {
      xv_set(ip->color,  PANEL_INACTIVE, FALSE, 0);
      xv_set(ip->symbol, PANEL_INACTIVE, FALSE, 0);
      xv_set(ip->type,   PANEL_INACTIVE, FALSE, 0);
      xv_set(ip->varb,   PANEL_INACTIVE, FALSE, 0);
      xv_set(ip->param,  PANEL_INACTIVE, FALSE, 0);
      window_fit_height(ip->pan);          
      window_fit_height(ip->win);
    }
  free_ivector(v_box, 0, v_dim-1);
  free_ivector(p_box, 0, p_dim-1);
}

/*
 * load_read_window() reads info from the load window into the postmaster
 */
int load_read_window(void)
{
  load_win_objects *ip = load_ip;
  char *strng;
  int	temp, i, v_dim, p_dim, *v_box, *p_box, *ivector();

  if (!ip) return 0;

  strng = (char *) xv_get(ip->directory, PANEL_VALUE);		       /* Directory Name */
  pm(PUT, "Load.Directory", strng, 0);

  strng = (char *) xv_get(ip->filename, PANEL_VALUE);		       /* Filename */
  pm(PUT, "Load.Filename", strng, 0);

  temp = (int) xv_get(ip->option, PANEL_VALUE);
  pm(PUT, "Load.Format_Flag", temp, 0);

  temp = (int) xv_get(ip->type, PANEL_VALUE);
  switch( temp )
    {
    case 0:
      pm(PUT, "Load.Data_Type", "Memory.Traj", 0);
      break;
    case 1:
      pm(PUT, "Load.Data_Type", "Memory.Fixed", 0);
      break;
    case 2:
      pm(PUT, "Load.Data_Type", "Memory.Cont", 0);
      break;
    case 3:
      pm(PUT, "Load.Data_Type", "Memory.Param", 0);
      break;
    case 4:
      pm(PUT, "Load.Data_Type", "Memory.Sel_Pt", 0);
      break;
    }

  temp = (int) xv_get(ip->color, PANEL_VALUE);
  pm(PUT, "Load.Color_Flag", temp? FALSE: TRUE, 0);

  temp = (int) xv_get(ip->symbol, PANEL_VALUE);
  pm(PUT, "Load.Symbol_Flag", temp? FALSE: TRUE, 0);

  v_dim = *((int *)pm(GET, "Model.Varb_Dim", 0));
  p_dim = *((int *)pm(GET, "Model.Param_Dim", 0));
  v_box = ivector(0,v_dim-1); 
  p_box = ivector(0,p_dim-1); 
  for(i=v_dim-1; i>=0; i--)	/* go backwards so that refresh doesn't scroll down the listg */
    v_box[i] = (int) xv_get(ip->varb, PANEL_LIST_SELECTED, i);
  for(i=p_dim-1; i>=0; i--)
    p_box[i] = (int) xv_get(ip->param, PANEL_LIST_SELECTED, i);
  pm(PUT_LIST, "Load.Varb_Index", 0, v_dim-1, v_box,
     PUT_LIST, "Load.Param_Index", 0, p_dim-1, p_box,
     0);
  free_ivector(v_box, 0, v_dim-1);
  free_ivector(p_box, 0, p_dim-1);
}

/*
 * load_field_manager() updates info on load window.  Modifies labels on buttons
 */
int load_field_manager(void)
{
  int			i,v_dim,p_dim;
  char			name[MAX_LEN_VARB_NAME];
  static int		num_varb=0, num_param=0;
  load_win_objects  	*ip = load_ip;
  
  if (!ip) return 0;
  
  v_dim = *((int *) pm( GET, "Model.Varb_Dim", 0 ));	/* get new values */
  p_dim = *((int *) pm( GET, "Model.Param_Dim", 0 ));
  
  if ( num_varb > 0 ) 
    {						  /* destroy old objects */
      for (i=num_varb-1; i>=0; i--)
	xv_set(ip->varb, PANEL_LIST_DELETE, i, 0);
      num_varb = 0;
    }
  if ( num_param > 0 ) 
    {
      for (i=num_param-1; i>=0; i--)
	xv_set(ip->param, PANEL_LIST_DELETE, i, 0);
      num_param = 0;
    }

  for ( i=0; i<v_dim; i++) 
    {						  /* assign new labels */
      pm( GET, "Model.Varb_Names", i, name , 0);
      xv_set(ip->varb, PANEL_LIST_INSERT, i, PANEL_LIST_STRING,  i, name,  0);
    }   
  num_varb = v_dim;
  for ( i=0; i<p_dim; i++) 
    {						  /* assign new labels */
      pm( GET, "Model.Param_Names", i, name , 0);
      xv_set(ip->param, PANEL_LIST_INSERT, i, PANEL_LIST_STRING, i, name, 0);
    }   
  num_param = p_dim;
}


/*
 * load_from_batch() enables the user to load a dstool format file without having
 * windows open.  It is used upon startup of dstool to load data from a configuration file.
 */
int load_from_batch(void)
{
  char	*dir = (char *)calloc(SIZE_OF_DIR_PLUS_FNAME,sizeof(char));
  char	*test, *getcwd();
  char	*filename = (char sizeof(char));
  
  int   mode_flag = *((int *) pm(GET, "Control.Mode", 0));

  /*  getwd(dir); */			/* may be system specific */
  getcwd(dir, SIZE_OF_DIR_PLUS_FNAME);

  if ( mode_flag == WINDOWS_MODE ) 
    {
      pm(GET, "Control.Infile", filename, 0);
      test = (char *)calloc(SIZE_OF_DIR_PLUS_FNAME,sizeof(char));
      test = strcat(strcat(strcpy(test,dir), "/"), filename );
      if( check_file_to_read(test) ) /* Allow config file to not exist or to be unreadable. */
	{
	  pm(PUT, "Load.Directory", dir, /* set load filename = config filename based in CURRENT directory*/
	     PUT, "Load.Filename", filename, 0);
	  load_driver(1);		/* display errors on cmd window (always created) */
	}
      free(test);
    }
  else
    {
      pm(GET, "Control.Infile", filename, 0);
      pm(PUT, "Load.Directory", dir, /* set load filename = config filename based in CURRENT directory*/
	 PUT, "Load.Filename", filename, 0);
      load_driver(-1);		/* display errors to stderr */
    }
  free(dir);
  free(filename);
}

/*
 * load_driver() calls load_go() and monitors errors.  Errors are displayed depending on mode:
 *    mode = 0  <==> display errors on load_win 
 *    mode > 0  <==> display errors on cmd_win 
 *    mode < 0  <==> write errors to stderr
 */
int load_driver(int mode)
{
  Panel		pan;
  int			status;
  int			keep_going=TRUE, force_varb=FALSE, force_param=FALSE;

  if (mode > 0)
    pan = (Panel) cmd_ip->pan;
  else if (mode == 0)
    pan = (Panel) load_ip->pan;

  while( keep_going && (status = load_go(&force_varb, &force_param)) )
    switch (status)
      {
      case -1:
	if(mode >= 0) 
	  error_notice(pan, "Cannot find or open file. Check directory and file names.");
	else
	  system_mess_proc(1, "Cannot find or open input file. Check directory and file names.");
	keep_going = FALSE;
	break;
      case -2:
	if(mode >= 0)
	  error_notice(pan, "Unformatted trajectory data cannot contain parameters.");
	keep_going = FALSE;
	break;
      case -3:
	if (mode >= 0)
	  error_notice(pan, "Loading not successful.  Cannot initialize memory!");
	else
	  system_mess_proc(1,"Loading not successful.  Cannot initialize memory!");
 	keep_going = FALSE;
	break;
      case -4:
	if (mode >= 0)
	  if ( !error_notice_option(pan, "Dimension of file variables different from current variable dimension", 
				    "Cancel Load","Try to Continue"))
	    {
	      force_varb = TRUE;
	      keep_going = TRUE;
	    }
	  else
	    keep_going = FALSE;
	else
	  {
	    system_mess_proc(1, "Dim of file variables different from current dim: Aborting load!");
	    keep_going = FALSE;
	  }
	break;
      case -5:
	if (mode >= 0)
	  if ( !error_notice_option(pan, "Dimension of file parameters different from current parameter dimension", 
				    "Cancel Load","Try to Continue"))
	    {
	      force_param = TRUE;
	      keep_going = TRUE;
	    }
	  else
	    keep_going = FALSE;
	else
	  {
	    system_mess_proc(1, "Dim of file parameters different from current dim: Aborting load!");
	    keep_going = FALSE;
	  }
	break;
      } 
  cmd_data_refresh();		/* update showing number of points */
  return 0;
}


/*
 * load_config()   loads dstool configuration data from specified file.  
 * This  routine essentially restores the windowing system to a previous state.
 */
int load_config(FILE    * fp)
{
  char		label[SIZE_OF_GEN_INPUT],word[SIZE_OF_GEN_INPUT];
  Rect		*rect;
  int		window_number, wn, ijunk, djunk;
  int		saved_list_number; /* If the win nums are (say) 2,4,5 then the list_number's associated with them are 0,1,2 */
  int		v_dim, p_dim, f_dim, list_number = 0;  
  
  if( (*((int *) pm(GET, "Control.Mode", 0))) != WINDOWS_MODE)
    return 0;

  v_dim = *((int *) pm(GET, "Model.Varb_Dim", 0));
  p_dim = *((int *) pm(GET, "Model.Param_Dim", 0));
  f_dim = *((int *) pm(GET, "Model.Funct_Dim", 0));

  rect = (Rect *)calloc(1,sizeof(Rect));
  fscanf(fp,"%s",label);
  fscanf(fp,"%hd %hd %hd %hd",
	 &(rect->r_left), &(rect->r_top), &(rect->r_width), &(rect->r_height));
  if (!strcmp(label,"Cmd_Win")) 
    cmd_open(SET_WIN_CONFIG,rect->r_left, rect->r_top, rect->r_width, rect->r_height);
  else if (!strcmp(label,"Orbit_Win"))
    orbit_open(SET_WIN_CONFIG,rect->r_left, rect->r_top, rect->r_width, rect->r_height);
  else if (!strcmp(label,"Periodic_Win"))
    periodic_open(SET_WIN_CONFIG,rect->r_left, rect->r_top, rect->r_width, rect->r_height);
  else if (!strcmp(label,"Selected_Win"))
    sel_open(SET_WIN_CONFIG,rect->r_left, rect->r_top, rect->r_width, rect->r_height);
  else if (!strcmp(label,"Function_Win"))
    funct_open(SET_WIN_CONFIG,rect->r_left, rect->r_top, rect->r_width, rect->r_height);
  else if (!strcmp(label,"Defaults_Win"))
    def_open(SET_WIN_CONFIG,rect->r_left, rect->r_top, rect->r_width, rect->r_height);
  else if (!strcmp(label,"Mult_Win"))
    mult_open(SET_WIN_CONFIG,rect->r_left, rect->r_top, rect->r_width, rect->r_height);
  else if (!strcmp(label,"TwoD_Win"))		  /* handle twoD windows separately */
    {
      fscanf(fp, "%s %d", word, &saved_list_number);
      for ( wn=0; wn <=get_max_twoD(); wn++)
	{
	  if ( valid_twoD_id( wn ) )
	    {
	      if (list_number == saved_list_number)   /* an open window can be moved */
		{
		  window_number = twoD_open(wn,SET_WIN_CONFIG,rect->r_left,rect->r_top,rect->r_width,rect->r_height);
		}
	      list_number++;
	    }
	}				
      if (list_number <= saved_list_number)	      /* not enough windows; open one */
	{
	  window_number = get_max_twoD() + 1;
	  window_number = twoD_open(window_number,SET_WIN_CONFIG,rect->r_left,rect->r_top,rect->r_width,rect->r_height);
	}
      
      if (get_display_type(window_number)) /* this is a color display */
	{
	  fscanf(fp, "%s %d %s %d %s %d %s %d %s %d %s %d %s %lg %s %lg %s %s", 
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Symbol_Index),
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Sym_Size_Index),
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Bg_Color_Index),
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Show_Cmap),
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_Type_Index),
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index),
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Min),
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Max),
		 word, TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_File
		 );
	  if (TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index < v_dim)	/* correct for indexing - paw */
	    TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type = PHASE_SPACE_VARB;
	  else if ( (TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index -= v_dim) < p_dim)
	    TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type = PARAMETER_VARB;
	  else if ( (TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index -= p_dim) < f_dim)
	    TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type = FUNCTION_VARB;
	  else
	    {
	      TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index = 0;
	      TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type = PHASE_SPACE_VARB;
	    }
	}		
      else	   /* this is a B/W display; don't change TwoD_Opt_Ds fields having to do with color */
	{
	  fscanf(fp, "%s %d %s %d %s %d %s %d %s %d %s %d %s %lg %s %lg %s %s", 
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Symbol_Index),
		 word, &(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Sym_Size_Index),
		 word, &ijunk, word, &ijunk, word, &ijunk, word, &ijunk, word, &djunk,
		 word, &djunk, word, word );
	}
      
      fscanf(fp, "%s %lg %s %lg %s %lg %s %lg %s %d %s %d %s %d %s %d", 
	     word, &(TwoD_Ds[window_number]->TwoD_Win_Ds->Hor_Min),
	     word, &(TwoD_Ds[window_number]->TwoD_Win_Ds->Hor_Max),
	     word, &(TwoD_Ds[window_number]->TwoD_Win_Ds->Vert_Min),
	     word, &(TwoD_Ds[window_number]->TwoD_Win_Ds->Vert_Max),
	     word, &(TwoD_Ds[window_number]->TwoD_Win_Ds->Active_Hor_Type),
	     word, &(TwoD_Ds[window_number]->TwoD_Win_Ds->Active_Ver_Type),
	     word, &(TwoD_Ds[window_number]->TwoD_Win_Ds->Active_Hor_Index),
	     word, &(TwoD_Ds[window_number]->TwoD_Win_Ds->Active_Ver_Index)
	     );
      twoD_opt_win_refresh( window_number );
      twoD_data_refresh( window_number );
      colortable_refresh( window_number );
      bg_color_refresh( window_number, TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Bg_Color_Index );
    }
  free(rect);
}

