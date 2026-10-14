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
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/svrimage.h>
#include <xview/termsw.h>
#include <xview/text.h>
#include <xview/tty.h>
#include <xview/xv_xrect.h>
#include <xview/canvas.h>
#include <xview/cms.h>
#include <gdd.h>

#include <ui_init.h>
#include <constants.h>
#include <defaults.h>
#include "twoD.h"
#include "twoD_opt.h"
#include "twoD_ip.h"
#include <memory.h>
#include <symbols.h>



int
opt_data_refresh(int window_number)
{
  static char   dirname[SIZE_OF_FNAME], filename[SIZE_OF_FNAME];
  twoD_opt_win_objects    *ip = twoD_ip[window_number]->twoD_opt_win;

  set_dcoord_min( window_number, (double) atof( (char *)xv_get(ip->dcoord_min,PANEL_VALUE) ));
  set_dcoord_max( window_number, (double) atof( (char *)xv_get(ip->dcoord_max,PANEL_VALUE) ));

  set_sym( window_number, (int) xv_get(ip->sym, PANEL_VALUE ) );
  set_size( window_number, (int) xv_get(ip->size, PANEL_VALUE ) );
  set_bg_color(window_number, (int) xv_get(ip->bg, PANEL_VALUE ) );

  strcpy(dirname, (char *) xv_get(ip->cmap_dir, PANEL_VALUE) );
  set_cmap_dir( window_number, dirname);

  strcpy(filename, (char *) xv_get(ip->cmap_file, PANEL_VALUE) );
  set_cmap_fname(window_number, filename);

  set_showcol(window_number,  (int) xv_get(ip->showcol, PANEL_VALUE) );

  set_cmap_type( window_number, (int) xv_get(ip->cmap_use, PANEL_VALUE) );

  twoD_opt_win_refresh( window_number );
}



int opt_field_manager(int window_number)
{
  twoD_opt_dcrd_list( window_number );
  opt_data_refresh( window_number );
}

/* --------------------------------------------------------------------------------
   refreshes window and rebuilds new menu items if needed.
   
   -------------------------------------------------------------------------------- */
int
twoD_opt_win_refresh(int window_number)
{
  int    panel_index;
  twoD_opt_win_objects    *ip = twoD_ip[window_number]->twoD_opt_win;
  int		n_varb =  *((int *) pm( GET, "Model.Varb_Dim", 0 ));
  int		n_param = *((int *) pm( GET, "Model.Param_Dim", 0 ));
  int           format = *((int *) pm(GET, "Defaults.Precision", 0));
  char		strng[SIZE_OF_DIR_PLUS_FNAME];

  xv_set(ip->sym, PANEL_VALUE,
	 TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Symbol_Index, 0);

  xv_set(ip->size, PANEL_VALUE,
	 TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Sym_Size_Index, 0);

  xv_set(ip->bg, PANEL_VALUE,
	 TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Bg_Color_Index, 0);
        
  sprintf(strng,"%s",TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_Dir);
  xv_set(ip->cmap_dir, PANEL_VALUE,strng, 0);

  sprintf(strng,"%s",TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_File);
  xv_set(ip->cmap_file, PANEL_VALUE,strng, 0);

  xv_set(ip->showcol, PANEL_VALUE,
	 TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Show_Cmap, 0);

  xv_set(ip->cmap_use, PANEL_VALUE,
	 TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_Type_Index, 0);

  if( (int)xv_get(ip->cmap_use, PANEL_VALUE) != 2 ){ /* not depth coding */
    xv_set(ip->depthcoord, PANEL_INACTIVE, TRUE, 0);
    xv_set(ip->dcoord_min, PANEL_INACTIVE, TRUE, 0);
    xv_set(ip->dcoord_max, PANEL_INACTIVE, TRUE, 0);
  }
  else{
    xv_set(ip->depthcoord, PANEL_INACTIVE, FALSE, 0);
    xv_set(ip->dcoord_min, PANEL_INACTIVE, FALSE, 0);
    xv_set(ip->dcoord_max, PANEL_INACTIVE, FALSE, 0);
    if( TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type == PHASE_SPACE_VARB )
      panel_index = TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index;
    else if (TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type == PARAMETER_VARB )
      panel_index = TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index + n_varb;
    else
      panel_index = TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index + n_varb + n_param;
	      
    xv_set(ip->depthcoord, PANEL_VALUE, panel_index, 0);
    sprintf(strng,"%.*lg",format,TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Min);
    xv_set(ip->dcoord_min, PANEL_VALUE,strng, 0);
    sprintf(strng,"%.*lg",format,TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Max);
    xv_set(ip->dcoord_max, PANEL_VALUE,strng, 0);
  }
}



/* --------------------------------------------------------------------------------
   sets up depth coordinate selection panel item               
   
   -------------------------------------------------------------------------------- */

int
twoD_opt_dcrd_list(int window_number)
{
  int		i;
  int	        n_total;
  twoD_opt_win_objects    *ip = twoD_ip[window_number]->twoD_opt_win;
  int		n_varb =  *((int *) pm( GET, "Model.Varb_Dim", 0 ));
  int		n_param = *((int *) pm( GET, "Model.Param_Dim", 0 ));
  int		n_funct = *((int *) pm( GET, "Model.Funct_Dim", 0 ));
  char		name[MAX_LEN_VARB_NAME];

  n_total = n_varb + n_param + n_funct;
  xv_destroy(ip->depthcoord);
  ip->depthcoord = twoD_opt_win_depthcoord_create(ip, ip->pan, n_total);
  xv_set(ip->depthcoord,PANEL_VALUE,0,0);
  xv_set(ip->depthcoord,PANEL_CHOICE_NROWS,n_total,0);
  for(i=0;i<n_varb;i++){
    pm( GET, "Model.Varb_Names", i, name , 0);
    xv_set(ip->depthcoord,PANEL_CHOICE_STRING,i,name,0);
  }
  for(i=n_varb;i<n_varb+n_param;i++){
    pm( GET, "Model.Param_Names", i-n_varb , name , 0);
    xv_set(ip->depthcoord,PANEL_CHOICE_STRING,i,name,0);
  }
  for(i=n_varb+n_param;i<n_total;i++){
    pm( GET, "Model.Funct_Names", i-n_varb-n_param , name , 0);
    xv_set(ip->depthcoord,PANEL_CHOICE_STRING,i,name,0);
  }
}


/* --------------------------------------------------------------------------------
   adds a pt to the selected point or paramater data set

   ------------------------------------------------------------------------------- */
int
add_sel_pt(int window_number)
{

  int	get_num_param(), get_symbol_code();
  int	symbol, size, color[3], status = 0;
  double	*parameters, *state, *dvector();
  memory	mem_obj;
  twoD_opt_win_objects    *ip = twoD_ip[window_number]->twoD_opt_win;
	
  if(get_num_func(window_number) != 0)
    {error_notice(ip->pan,"Cannot Select Points From A Function Display Window!");
     return(-1);}

  if(get_num_param(window_number) == 2)
    mem_obj = (memory) pm( GET, "Memory.Param", 0);
  else
    mem_obj = (memory) pm( GET, "Memory.Sel_Pt", 0);

  state = dvector(0,*((int *) pm( GET, "Model.Varb_Dim", 0 ))-1);
  parameters = dvector(0,*((int *) pm( GET, "Model.Param_Dim", 0 ))-1);
  pm( GET_LIST, "Selected.Param_Ic", 0, 
     *((int *) pm( GET, "Model.Param_Dim", 0 ))-1,
     parameters, 0);
  pm( GET_LIST, "Selected.Varb_Ic", 0, 
     *((int *) pm( GET, "Model.Varb_Dim", 0 ))-1,
     state, 0);

  symbol = TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Symbol_Index;
  size = TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Sym_Size_Index;
  color[0] = color[1] = *((int *) pm( GET, "Color.Pick_Color_Choice", 0));
  color[2] = get_symbol_code(symbol,size);
	
  if (!(memory_exists_current_flow(mem_obj)))
    memory_start_new_flow( mem_obj, 1, 0, 0, 2, 0, 0);
  status = memory_add_point(mem_obj, state, parameters, color, 0, 0, 0, 0);
  if (status == 0) mem_all_win(mem_obj);
  else system_mess_proc(1,"add_sel_pt: Error, point not saved.");

  free_dvector(state,0,*((int *) pm( GET, "Model.Varb_Dim", 0 ))-1);
  free_dvector(parameters,0,*((int *) pm( GET, "Model.Param_Dim", 0 ))-1);

  return(0);
}


/* --------------------------------------------------------------------------
   set the plotting symbol

   ------------------------------------------------------------------------ */
int
set_sym(int window_number, int symbol)
{
  TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Symbol_Index = symbol;
}



/* --------------------------------------------------------------------------
   set the size of plotting symbols 
   
   ------------------------------------------------------------------------- */
int
set_size(int window_number, int size)
{
  TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Sym_Size_Index = size;
}


/* -------------------------------------------------------------------------
   set the background color for a view window
   
   ------------------------------------------------------------------------- */
int
set_bg_color(int window_number, int bg_color)
{
  TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Bg_Color_Index = bg_color;

}

/* --------------------------------------------------------------------------
   changes the background color
   
   ------------------------------------------------------------------------ */
int
bg_color_refresh(int window_number, int bg_color)
{
  Canvas		canvas;

  canvas = (Canvas) get_twoD_handle(window_number,CANVAS_HANDLE);
  if (get_display_type(window_number) == TRUE)
      xv_set( canvas, WIN_BACKGROUND_COLOR, (int) bg_color, 0 );   
  refresh_win_id(window_number);  
}

 

/* -------------------------------------------------------------------------
   sets the colormap directory
   
   --------------------------------------------------------------------- */
int
set_cmap_dir(int window_number, char		* dirname)
{
  /* temp storage allocation -- fjw 9/2/91 */
  if ( TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_Dir == 0 )
    TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_Dir = (char *)malloc(SIZE_OF_DIR_PLUS_FNAME * sizeof(char));

  strcpy(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_Dir,dirname);
}


/* --------------------------------------------------------------------------------
   sets the file containing the colormap filename
   
   -------------------------------------------------------------------------------- */
int
set_cmap_fname(int window_number, char		* fname)
{
  /* temp storage allocation -- fjw 9/2/91 */
  if ( TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_File == 0 )
    TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_File = (char *)malloc(SIZE_OF_FNAME * sizeof(char));

  strcpy(TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_File,fname);
}



/* --------------------------------------------------------------------------------
   shows or hides the colormap strip
   
   -------------------------------------------------------------------------------- */
int
set_showcol(int window_number, int value)
{
  TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Show_Cmap= value;
  if(!value && get_display_type(window_number))	/* don't do it on monochrome! */
    show_colorbar(window_number);
  else
    hide_colorbar(window_number);
}


/* --------------------------------------------------------------------------------
   sets the colormap type (alt, pick, depth,...)
   
   -------------------------------------------------------------------------------- */
int
set_cmap_type(int window_number, int value)
{
  TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Cmap_Type_Index = value;
  clear_cbar(window_number);
  paint_cbar_win_id( window_number);
}


/* --------------------------------------------------------------------------------
   sets the depth coordinate for a given window
   
   -------------------------------------------------------------------------------- */
int
set_depthcoord(int window_number, int value)
{
  int	n_total;
  double	min_value, max_value;
  int 	n_varb =  *((int *) pm( GET, "Model.Varb_Dim", 0 ));
  int 	n_param = *((int *) pm( GET, "Model.Param_Dim", 0 ));
  int 	n_funct = *((int *) pm( GET, "Model.Funct_Dim", 0 ));

  n_total = n_varb + n_param + n_funct;

  if( value < n_varb) 
    {TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type = PHASE_SPACE_VARB;
     TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index = value;
     min_value = *((double *) pm( GET, "Defaults.Varb_Min", value, 0));
     max_value = *((double *) pm( GET, "Defaults.Varb_Max", value, 0)); }
  else if (n_varb<=value && value<n_varb+n_param)
    {TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type = PARAMETER_VARB;
     TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index = value-n_varb;
     min_value = *((double *) pm( GET, "Defaults.Param_Min", value-n_varb, 0));
     max_value = *((double *) pm( GET, "Defaults.Param_Max", value-n_varb, 0)); }
  else if (n_varb+n_param<=value && value<n_total)
    {TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Active_Depth_Type = FUNCTION_VARB;
     TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Index = value-n_varb-n_param;
     min_value = *((double *) pm( GET, "Defaults.Funct_Min", value-n_varb-n_param, 0));
     max_value = *((double *) pm( GET, "Defaults.Funct_Max", value-n_varb-n_param, 0)); }

  set_dcoord_min(window_number, min_value);
  set_dcoord_max(window_number, max_value);
}


/* --------------------------------------------------------------------------------
   sets the lower range of the depth color-coding coordinate
   
   -------------------------------------------------------------------------------- */
int
set_dcoord_min(int window_number, double value)
{
  char		strng[80];
  int format = *((int *) pm(GET, "Defaults.Precision", 0));
  twoD_opt_win_objects    *ip = twoD_ip[window_number]->twoD_opt_win;

  TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Min = value;
  sprintf(strng,"%.*lg",format,TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Min);
  xv_set(ip->dcoord_min, PANEL_VALUE,strng, 0);
}


/* --------------------------------------------------------------------------------
   sets the upper range of the depth color-coding coordinate
   
   -------------------------------------------------------------------------------- */
int
set_dcoord_max(int window_number, double value)
{
  char		strng[80];
  int format = *((int *) pm(GET, "Defaults.Precision", 0));
  twoD_opt_win_objects    *ip = twoD_ip[window_number]->twoD_opt_win;

  TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Max = value;
  sprintf(strng,"%.*lg",format,TwoD_Opt_Ds[window_number]->TwoD_Opt_Win_Ds->Depth_Coord_Max);
  xv_set(ip->dcoord_max, PANEL_VALUE,strng, 0);
}


/* --------------------------------------------------------------------------------
   sets up default TwoD_Opt_Ds structure for a given twoD_opt window
   -------------------------------------------------------------------------------- */
int set_twoD_opt_fields(int window_number)
{
  static char	dirname[SIZE_OF_DIR_PLUS_FNAME];
  static char   fname[SIZE_OF_FNAME];

  get_dstool_path( dirname, DSTOOL_COLOR_DIR ); 
  pm( GET, "Defaults.Ctable_File", fname, 0 );
     
  set_sym( window_number, *((int *)pm( GET, "Defaults.Symbol_Index", 0 )) );
  set_size( window_number, *((int *)pm( GET, "Defaults.Size_Index", 0 )) );
  set_bg_color( window_number, *((int *)pm( GET, "Defaults.Bgnd_Color", 0)) );
  set_cmap_dir( window_number, dirname );
  set_cmap_fname( window_number, fname );
  set_showcol( window_number, *((int *)pm( GET, "Defaults.Show_Color", 0)) );
  twoD_opt_dcrd_list(window_number);
  set_depthcoord( window_number, *((int *)pm( GET, "Defaults.Depth_Coord", 0)) );
  set_cmap_type( window_number, *((int *)pm( GET, "Defaults.Cmap_Type", 0)) );
  twoD_opt_win_refresh( window_number );
}

/* --------------------------------------------------------------------------------
   proc used to switch off color controls on option panel

   -------------------------------------------------------------------------------- */
int
color_opt_off(int window_number)
{
  twoD_opt_win_objects    *ip = twoD_ip[window_number]->twoD_opt_win;

  xv_set(ip->win , XV_HEIGHT, 64, 0);
  xv_set(ip->bg , PANEL_INACTIVE, TRUE, 0);
  xv_set(ip->cmap_dir , PANEL_INACTIVE, TRUE, 0);
  xv_set(ip->cmap_file , PANEL_INACTIVE, TRUE, 0);
  xv_set(ip->showcol , PANEL_INACTIVE, TRUE, 0);
  xv_set(ip->cmap_use , PANEL_INACTIVE, TRUE, 0);
  xv_set(ip->depthcoord , PANEL_INACTIVE, TRUE, 0);
  xv_set(ip->dcoord_min , PANEL_INACTIVE, TRUE, 0);
  xv_set(ip->dcoord_max , PANEL_INACTIVE, TRUE, 0);
  xv_set(ip->dcoord_max , PANEL_INACTIVE, TRUE, 0);
}


/* --------------------------------------------------------------------------------
   sets the colormap and then changes the colormap on the 
   appropriate twoD  window.
   
   -------------------------------------------------------------------------------- */
int colortable_refresh(int window_number)
{
  Canvas		canvas;
  Window		paint_window;
  extern void twoD_cbar_repaint();

  set_colortable(window_number);

  canvas = (Canvas) get_twoD_handle(window_number,CBAR_CANVAS_HANDLE);
  paint_window = (Xv_window) get_twoD_handle(window_number,CBAR_PW_HANDLE);
  twoD_cbar_repaint(canvas,paint_window,0);
  refresh_win_id(window_number);
}


