#ifndef lint
static char sccsid[]="@(#)text.c	1.50 01/11/95 Copyright 1987-1992 Sun Microsystem, Inc." ;
#endif
/*  Copyright (c) 1988-1990  Sun Microsystems, Inc.
 *  All Rights Reserved.
 *
 *  Sun considers its source code as an unpublished, proprietary
 *  trade secret, and it is available only under strict license
 *  provisions.  This copyright notice is placed here only to protect
 *  Sun in the event the source is deemed a published work.  Dissassembly,
 *  decompilation, or other means of reducing the object code to human
 *  readable form is prohibited by the license agreement under which
 *  this code is provided to the user or company in posesion of this copy.
 *
 *  RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the
 *  Government is subject to restrictions as set forth in subparagraph
 *  (c)(1)(ii) of the Rights in Technical Data and Computer Software
 *  clause at DFARS 52.227-7013 and in similar clauses in the FAR and
 *  NASA FAR Supplement.
 */

#include "defs.h"

char *Str[MAX_MESSAGES] ;


void
init_text()         /* Setup text strings depending upon language. */
{
  Str[(int) M_WIN] =
    MGET("Can't create a new window; try quitting some windows") ;

  Str[(int) M_NOMEMORY] = MGET("Out of memory") ;
  Str[(int) M_CHDIR]    = MGET("Unable to read directory for folder '%s': %s") ;
  Str[(int) M_PRINT]    = MGET("Sent files to printer") ;
  Str[(int) M_NOCOPIES] = MGET("Please enter number of copies to print") ;
  Str[(int) M_CREAT]    = MGET("Unable to create '%s': %s") ;
  Str[(int) M_DELETE]   = MGET("Unable to remove '%s': %s") ;
  Str[(int) M_SELFIRST] = MGET("You must first select something.") ;
  Str[(int) M_LINK]     = MGET("Link") ;

/* STRING_EXTRACTION SUNW_DESKSET_FM_MSG : Abbreviation of "Move". */

  Str[(int) M_COPY]    = MGET("Copy") ;
  Str[(int) M_MOVE]    = MGET("Move") ;
  Str[(int) M_READSTR] = MGET("Unable to read '%s': %s") ;
  Str[(int) M_NOFLOPPY] = MGET("No floppy disk in drive") ;

  Str[(int) M_NO_INFONAME] = MGET("Please enter a name") ;
  Str[(int) M_CONFIRMDEL] =
    MGET("Are you sure you want to remove these file(s)?") ;

  Str[(int) M_DELFAILED] = MGET("Removing '%s' failed: %s") ;
  Str[(int) M_EXIST]     = MGET("'%s' already exists") ;
  Str[(int) M_RENAME]    = MGET("Renaming '%s'...") ;

  Str[(int) M_RENAMEPERM] =
    MGET("You don't have permission to rename '%s' in this folder") ;

  Str[(int) M_2MANYFILES] = MGET("Can't display more than %d files") ;
  Str[(int) M_FIND]       = MGET("Can't find '%s'") ;
  Str[(int) M_CANTRENAME] = MGET("Can't rename '%s': %s") ;

  Str[(int) M_DATE] =
    MGET("Invalid '%s' date format; use month/day/year, eg 12/25/88") ;

  Str[(int) M_OWNER]  = MGET("%s is not a valid owner name") ;
  Str[(int) M_SEARCH] = MGET("Will search from current folder: '%s'") ;
  Str[(int) M_PTY]    = MGET("Ran out of pty's:  Find aborted") ;

  Str[(int) M_HOWTOPASTE] =
    MGET("%s these file(s) by opening the target folder and selecting 'Paste'") ;
  Str[(int) M_HIDDEN] = MGET("To see .hidden files, change the View Properties") ;

  Str[(int) M_CMD] =
    MGET("The '%s' command failed; the Console may display more messages.") ;

  Str[(int) M_EXEC] =
    MGET("Opening '%s'; the Console may display more messages.") ;

  Str[(int) M_EXPANDALL_1]  = MGET("This operation may take some time; use the Stop key to stop the operation.") ;
  Str[(int) M_YES]          = MGET("Yes") ;
  Str[(int) M_NO]           = MGET("No") ;
  Str[(int) M_LINKEDTO]     = MGET("The folder %s is linked to the folder %s") ;
  Str[(int) M_SEARCHING]    = MGET("Searching...") ;
  Str[(int) M_NFILESFOUND]  = MGET("%d file(s) found") ;
  Str[(int) M_NOTHINGFOUND] = MGET("Nothing found") ;
  Str[(int) M_MAILAPPEND]   = MGET("Appended to folder %s") ;

  Str[(int) M_APPINSHELLTOOL] =
    MGET("Do you want to run the selected application in a Shell Tool or without one?") ;

  Str[(int) M_APP_SHELLYES]  = LGET("Shell Tool") ;
  Str[(int) M_APP_SHELLNO]   = LGET("No Shell Tool") ;
  Str[(int) M_CANCEL]        = MGET("Cancel") ;
  Str[(int) M_UNKNOWNOWNER]  = MGET("Unknown owner") ;
  Str[(int) M_UNKNOWNGROUP]  = MGET("Unknown group") ;

  Str[(int) M_NEED_HOME_VAR] = EGET("Need to set $HOME environment variable, see your system administrator") ;
  Str[(int) M_BAD_START_DIR] = EGET("Invalid Directory: File Manager cannot start with this directory.") ;
  Str[(int) M_DEBUGGING_ON]  = EGET("Debugging turned ON") ;

  Str[(int) M_BAD_INTERVAL] =
    EGET("Invalid file checking interval. Use a number between 0 and %d. Using default of %d") ;

  Str[(int) M_NO_INTERVAL] =
    EGET("No file checking interval given. Use a number between 0 and %d. Using default of %d") ;

  Str[(int) M_USAGE] =
    MGET("Usage %s: [-a] [-c] [-r] [-ver] [-d directory ] [-i interval]") ;

  Str[(int) M_NOSEL]          = EGET("No Files Selected") ;
  Str[(int) M_NOOPEN_INV_OBJ] = EGET("Cannot open; invalid file") ;
  Str[(int) M_NOGOTO_ARGS]    = EGET("No arguments found on goto line") ;
  Str[(int) M_NOOPEN_METHOD]  = EGET("No open method defined for %s") ;
  Str[(int) M_BADCHAR] = EGET("Filenames cannot contain the '%c' character") ;
  Str[(int) M_INVCHAR] = EGET("Invalid character used in filename") ;
  Str[(int) M_ENTER_PMETHOD]  = EGET("Please enter a print method.") ;
  Str[(int) M_INV_NOTICE]     = EGET("Invalid notice value returned") ;

  Str[(int) M_FULLPATH]    = EGET("Full pathname required") ;
  Str[(int) M_IGNORE_DND]  = EGET("You cannot drag & drop on same folder") ;
  Str[(int) M_NO_OPERATE1] =
    LGET("The sourcing application cannot \nsend data that filemgr can operate on. \nThe drop operation has been cancelled.") ;
  Str[(int) M_NOOPEN_PIPE] = EGET("Unable to initiate drag & drop") ;
  Str[(int) M_NOWRITE_DND] = EGET("You do not have permission to write to %s for drag & drop") ;
  Str[(int) M_NOCREATE_DND] = EGET("Unable to create destination file for drop.  Check permissions or disc space.") ;
  Str[(int) M_NOOPEN_FILE] = EGET("Unable to open %s file for drag and drop") ;
  Str[(int) M_NOCREATE_CATCH] = EGET("Drag & drop has failed.") ;
  Str[(int) M_NO_REALLOCATE]  = EGET("Drag & drop has failed.") ;
  Str[(int) M_ALLOC_ATOMS]    = EGET("Drag & drop has failed.") ;
  Str[(int) M_NO_FILTER]   = EGET("Unable to find drag & drop method for %s") ;
  Str[(int) M_NO_CONTEXT]  = EGET("Unable to get context of drop") ;
  Str[(int) M_NO_WASTE_MOVE] = EGET("The Wastebasket cannot be moved") ;
  Str[(int) M_NO_PLACE_WASTE] = EGET("Did not place '%s' in waste; the move failed.") ;
  Str[(int) M_NO_APP_DRAG] = EGET("You cannot drag folders to the other application") ;
  Str[(int) M_NO_SYS_DRAG] = EGET("You cannot drag system files to the other application") ;
  Str[(int) M_ONLY_DRAG] = EGET("You can only drag applications and documents to the other application") ;
  Str[(int) M_NO_TREE_DND] = EGET("Unable to setup folder view drag & drop selections") ;
  Str[(int) M_NO_FILE_DND] = EGET("Unable to setup file pane drag & drop selections") ;
  Str[(int) M_NOCREATE_OBJ] = EGET("Drag & drop failed, try again.") ;

  Str[(int) M_DND_ERROR] = EGET("File Manager: %s: dnd drop error") ;
  Str[(int) M_NOGET_DND] = EGET("Unable to get dnd selections") ;
  Str[(int) M_DND_TIMEOUT] = EGET("Drag and Drop: Timed Out") ;
  Str[(int) M_DND_ILLEGAL] = EGET("Drag and Drop: Illegal Target") ;
  Str[(int) M_DND_BADSEL]  = EGET("Drag and Drop: Bad Selection") ;
  Str[(int) M_DND_FAIL]    = EGET("Drag and Drop: Failed") ;
  Str[(int) M_DND_OLD]     = EGET("V2.0 and older drag and drop events are not supported\n") ;
  Str[(int) M_REPAINTS] = EGET("%d saved repaints this expose, %d total\n") ;
  Str[(int) M_NOT_FDR]  = EGET("Objects need to be dropped on folders, not in the pane area. Try again.") ;
  Str[(int) M_EXEC_FAIL] = EGET("Cannot run Find.") ;

  Str[(int) M_MAX_UNDELETE] = EGET("Only %d items can be undeleted at a time") ;
  Str[(int) M_NO_ORIG]      = EGET("Original location for %s not saved -- please restore manually.") ;
  Str[(int) M_ERR_EXIT]     = EGET("File Manager: %s(%d) %s\n") ;
  Str[(int) M_FM_MSG]       = EGET("File Manager: %s\n") ;
  Str[(int) M_OPEN_ERROR]   = EGET("File Manager cannot open the error message file. %s\n") ;
  Str[(int) M_NOPRINT_METHOD] = EGET("There is no print method defined for %s.") ;
  Str[(int) M_EMPTY_FILE] = EGET("This file has no path; it cannot be undeleted.") ;
  Str[(int) M_NO_DELETE]  = EGET("File Manager: Unable to remove old delete path %s\n") ;
  Str[(int) M_BAD_PROPERTY] = EGET("dnd receive error: Bad property");
  Str[(int) M_BAD_TIME] = EGET("dnd receive error: Bad time");
  Str[(int) M_BAD_WIN_ID] = EGET( "dnd receive error: Bad window id");
  Str[(int) M_TIMEDOUT] = EGET("dnd receive error: Timed out");
  Str[(int) M_PROPERTY_DELETED] = EGET("dnd receive error: Property deleted");
  Str[(int) M_BAD_PROPERTY_EVENT] = EGET("dnd receive error: Bad property event");
  Str[(int) M_BAD_FILENAME] = EGET("Bad filename received");
  Str[(int) M_SURE_DELFOLDER] =
    MGET("Are you sure you want to delete the folder \n%s\nand ALL of its contents?") ;
  Str[(int) M_SURE_DESFOLDER] =
    MGET("Are you sure you want to destroy the folder \n%s\nand ALL of its contents? \n\nWARNING: This operation is not undoable.") ;
  Str[(int) M_DELFOLDER]  = LGET("Delete Folder") ;
  Str[(int) M_DESFOLDER]  = LGET("Destroy Folder") ;

  Str[(int) M_INV_CE_VER] = EGET("Invalid CE database version") ;
  Str[(int) M_NO_CE_CONNECT] = EGET("Unable to connect with the CE database") ;
  Str[(int) M_CE_NAMESPACE] = EGET("Unable to find the File namespace") ;
  Str[(int) M_CE_TYPESPACE] = EGET("Unable to find the Type namespace") ;
  Str[(int) M_CE_NO_FILE] = EGET("Unable to find the file type handle") ;
  Str[(int) M_CE_NO_TYPE] = EGET("Unable to find the type name handle") ;
  Str[(int) M_CE_NO_OPEN] = EGET("Unable to find the open method handle") ;
  Str[(int) M_CE_NO_ICON] = EGET("Unable to find the icon handle") ;
  Str[(int) M_CE_NO_MASK] = EGET("Unable to find the icon mask handle") ;
  Str[(int) M_CE_NO_FG] = EGET("Unable to find the foreground color handle") ;
  Str[(int) M_CE_NO_BG] = EGET("Unable to find the background color handle") ;
  Str[(int) M_CE_NO_PRINT] = EGET("Unable to find the print method handle") ;
  Str[(int) M_CE_NO_TEM] = EGET("Unable to find the template handle") ;
  Str[(int) M_CE_NO_FDR] = EGET("Unable to find the generic folder entry") ;
  Str[(int) M_CE_NO_DOC] = EGET("Unable to find the generic document entry") ;
  Str[(int) M_CE_NO_APP] = EGET("Unable to find the generic application entry") ;
  Str[(int) M_CE_NO_DROP] = EGET("Unable to find the drop method handle") ;
  Str[(int) M_DND_SAME]  = EGET("Ignoring drag and drop on same object") ;
  Str[(int) M_NO_SOURCE] = EGET("Unable to read source file '%s'; drag & drop failed.") ;
  Str[(int) M_START_FILTER] = EGET("Unable to start drag & drop method '%s' for %s") ;
  Str[(int) M_NEED_FPATH] = EGET("File Manager needs the full pathname to Apply changes.") ;

  Str[(int) M_NO_FDROP] = EGET("Unable to drop on workspace; drag & drop failed.") ;
  Str[(int) M_NO_CMD]       = EGET("Unable to find the UNIX command to run") ;
  Str[(int) M_NO_ACT_NAME] =
    EGET("Select only one file at a time to rename. \nThe name field cannot be activated when \nmultiple files are selected.") ;
  Str[(int) M_NO_BACK]      = EGET("Unable to get background color") ;
  Str[(int) M_NO_FORE]      = EGET("Unable to get foreground color") ;
  Str[(int) M_CREATE_CMAP]  = EGET("Unable to create color map segment") ;
  Str[(int) M_NO_OPEN]      = EGET("Unable to open the error message file %s") ;
  Str[(int) M_NO_FORK] = EGET("File Manager: Unable to start process '%s'.") ;
  Str[(int) M_NO_DUP] = EGET("File Manager: Unable to start process '%s':%s.") ;
  Str[(int) M_NO_FIND] = EGET("File Manager: Unable to start process '%s'.") ;
  Str[(int) M_CHILD_STOP]   = EGET("File Manager: '%s' process stopped\n") ;
  Str[(int) M_CHILD_SIGNAL] = EGET("File Manager: '%s' process signaled\n") ;
  Str[(int) M_CHILD_EXIT]   = EGET("File Manager: '%s' process exited\n") ;
  Str[(int) M_CREATE_GEN_ICON] = EGET("Unable to create generic icon") ;
  Str[(int) M_CREATE_GEN_MASK] = EGET("Unable to create generic mask") ;
  Str[(int) M_UNCHECK_LICON] = EGET("Unchecked file found.\n") ;
  Str[(int) M_NO_ALLOC_TICON] = EGET("Unable to allocate icon tree entry") ;
  Str[(int) M_ERROR_MES] = EGET("File Manager: %s") ;

  Str[(int) M_NO_READ]  = EGET("Unable to read removable disk information.%s") ;
  Str[(int) M_CREATE_GC] = EGET("GC create failed") ;

  Str[(int) M_HOME]      = LGET("Home") ;
  Str[(int) M_CONTINUE]  = LGET("Continue") ;
  Str[(int) M_WASTE]     = LGET("Waste") ;
  Str[(int) M_VIEW]      = LGET("View") ;
  Str[(int) M_NO_PATH]   = LGET("Please type in a pathname and click Add.") ;
  Str[(int) M_LRENAME]   = LGET("rename %s to %s ") ;
  Str[(int) M_OUT_MEM]   = LGET("out of memory") ;
  Str[(int) M_NEWFOLDER] = LGET("NewFolder") ;
  Str[(int) M_KBYTES1]   = LGET("%d kbytes") ;
  Str[(int) M_BYTES]     = LGET("%s bytes") ;
  Str[(int) M_UNLABELED] = LGET("Unlabeled") ;
  Str[(int) M_EXISTS]    =
    LGET("A file named %s already exists. \nDo you want to overwrite the existing file %s \nor alter the name of the file you are moving?") ;
  Str[(int) M_EXISTS2]    =
    LGET("A file named %s already exists. \nDo you want to overwrite the existing file %s \nyou are moving or cancel the operation?") ;
  Str[(int) M_EX_OVERWRITE]   = LGET("Overwrite Existing File") ;
  Str[(int) M_EX_ALTER]       = LGET("Alter Name") ;
  Str[(int) M_FILEMGR]        = LGET("File Manager") ;
  Str[(int) M_ENTER_CMD_ARGS] = LGET("Enter any command arguments here:") ;

  Str[(int) M_CMD_ARGS] =
    DGET("ARG='%s';FILE='%s';FMPWD='%s';export ARG FILE FMPWD;eval cd \"$FMPWD\" && ") ;

  Str[(int) M_RUN]         = LGET("Run") ;
  Str[(int) M_RUNNING]     = MGET("Running %s") ;
  Str[(int) M_MENU_UPDATE] = LGET("Custom command menu updated") ;
  Str[(int) M_CONTAINS]    = LGET("Contains %1d items") ;
  Str[(int) M_UNDEFINED]   = LGET("undefined") ;
  Str[(int) M_AVAILK]      = LGET("%s kbytes (%d%%) available") ;
  Str[(int) M_AVAILM]      = LGET("%s Mbytes (%d%%) available") ;
  Str[(int) M_XXXXXXXXX]   = LGET("XXXXXXXXX bytes") ;
  Str[(int) M_UNKNOWN]     = LGET("Unknown") ;
  Str[(int) M_KBYTES2]     = LGET("%s kbytes (%d%%)") ;
  Str[(int) M_OK]          = LGET("Ok") ;
  Str[(int) M_FM]          = LGET("FM: %s %s %s:%s%s %s") ;
  Str[(int) M_WASTEBASKET] = LGET(" Waste") ;
  Str[(int) M_NEWDOCUMENT] = LGET("NewDocument") ;
  Str[(int) M_CLEANUP_SEL] = LGET("Clean up Selection") ;
  Str[(int) M_CLEANUP_ICONS] = LGET("Clean up Icons") ;
  Str[(int) M_OUT_PATH]      = MGET("Out of memory. Quitting folder view.") ;

  Str[(int) M_TRUNC_VIEW]    = LGET("Too many folders to display. Use Hide Sub Folders menu option.") ;

  Str[(int) M_IS_VIEWED]     = MGET("%s already open.") ;
  Str[(int) M_OPENING]  = MGET("Opening %s") ;
  Str[(int) M_MATCHING] = MGET("Matching %s") ;
  Str[(int) M_BUILDING] = MGET("Building %s") ;
  Str[(int) M_OPEN_ERRORS] = MGET("Opening '%s' - the Console may display more messages.") ;
  Str[(int) M_UNABLE_OPEN] = MGET("Unable to open %s") ;
  Str[(int) M_UNABLE_RENAME] = MGET("Unable to rename %s") ;
  Str[(int) M_DND_SOURCE] = MGET("Folder '%s' cannot be dropped onto itself.") ;
  Str[(int) M_OVER]       = MGET("Over %s") ;
  Str[(int) M_LUCIDA]     = MGET("Lucida font unavailable, using system font") ;
  Str[(int) M_LEVELS]     = MGET("Directory path too deep") ;
  Str[(int) M_LIST_EXIST] = MGET("The entry %s already exists in the list.") ;

  Str[(int) M_FILE1] =
    MGET("If you want this command to operate on the file(s) \nyou've selected you must include the word $FILE in \nyour command where the filename should appear. \n\nYou can ignore this warning and apply the command as \nyou wrote it; cancel the Apply; or have $FILE automatically \nappended to the end of your command.") ;
  Str[(int) M_FILE_APPLY] = MGET("Apply") ;
  Str[(int) M_FILE_ADD]   = MGET("Add $FILE") ;
  Str[(int) M_AFTER]     = MGET("After") ;
  Str[(int) M_BEFORE]    = MGET("Before") ;
  Str[(int) M_DO_DELETE] = MGET("Do you want to destroy the files in the waste?") ;
  Str[(int) M_NO_CREATE_WASTE] = MGET("Unable to create waste folder, cannot write to home directory. '%s' : %s") ;
  Str[(int) M_DIR_GONE] =
    MGET("%s no longer exists. Displaying home directory.") ;
  Str[(int) M_NO_VALID_DIR]    = MGET("%s is not a valid folder") ;
  Str[(int) M_REDRAWING]       = MGET("Redrawing folder pane...") ;
  Str[(int) M_UNDELETING]      = MGET("Undeleting %s") ;
  Str[(int) M_PRINTING] = MGET("Printing %s - the Console may display more messages.") ;
  Str[(int) M_UNABLE_PRINT] = MGET("Unable to print %s") ;
  Str[(int) M_MISSING]  = MGET("Missing %c") ;
  Str[(int) M_NO_APPLY] = MGET("Your changes in this category have not been applied. Do you want to apply the changes?") ;
  Str[(int) M_APPLY]     = MGET("Apply Changes") ;
  Str[(int) M_DISCARD]   = MGET("Discard Changes") ;
  Str[(int) M_CANT_DROP] = MGET("Folder '%s' cannot be dropped onto itself") ;
  Str[(int) M_DUPLICATE] = MGET("Duplicating %s") ;
  Str[(int) M_NO_IFNAME] = MGET("Unable to read icon filename (%s)\n") ;
  Str[(int) M_UNAPPLIED] = MGET("You have unapplied changes to your custom commands worksheet") ;
  Str[(int) M_APPLY_DISMISS]   = MGET("Apply and dismiss") ;
  Str[(int) M_DISCARD_DISMISS] = MGET("Discard changes and dismiss") ;
  Str[(int) M_UNIX_CMD]        = MGET("You need to enter a UNIX command") ;
  Str[(int) M_NO_CREATE]       = MGET("Unable to create '%s' : %s") ;
  Str[(int) M_NORESSAVE]       = MGET("Unable to save defaults") ;
  Str[(int) M_NOINFO_RENAME]   = MGET("Unable to move '%s' to %s by renaming its path.") ;
  Str[(int) M_GOTOAPP_TREE]    = LGET("Folder View") ;
  Str[(int) M_GOTOAPP_WASTE]   = LGET("Waste") ;
  Str[(int) M_PERM_DENIED]     = LGET("Permission denied\n") ;
  Str[(int) M_INFO_MORE]       = LGET("+") ;
  Str[(int) M_INFO_LESS]       = LGET("-") ;
  Str[(int) M_TREESEL]         = MGET("%s selected") ;

  Str[(int) M_UPDATING]        = LGET("Updating folder") ;
  Str[(int) M_UPDATEDNO]       =
    LGET("Updated folder:  no files added or deleted since last update.") ;
  Str[(int) M_UPDATEDASDS]     =
    MGET("Updated folder:  %d files added and %d files deleted since last update.") ;
  Str[(int) M_UPDATEDAD]       =
    MGET("Updated folder:  %d file added and %d file deleted since last update.") ;
  Str[(int) M_UPDATEDASD]      =
    MGET("Updated folder:  %d files added and %d file deleted since last update.") ;
  Str[(int) M_UPDATEDADS]      =
    MGET("Updated folder:  %d file added and %d files deleted since last update.") ;
  Str[(int) M_UPDATEDA]        =
    MGET("Updated folder:  %d file added since last update.") ;
  Str[(int) M_UPDATEDAS]       =
    MGET("Updated folder:  %d files added since last update.") ;
  Str[(int) M_UPDATEDD]        =
    MGET("Updated folder:  %d file deleted since last update.") ;
  Str[(int) M_UPDATEDDS]       =
    MGET("Updated folder:  %d files deleted since last update.") ;
  Str[(int) M_EDIT_DELETE]     = LGET("Delete") ;
  Str[(int) M_EDIT_DESTROY]    = LGET("Destroy") ;
  Str[(int) M_TREE_VERT]       = LGET("Show Vertical") ;
  Str[(int) M_TREE_HORZ]       = LGET("Show Horizontal") ;
  Str[(int) M_MAIN_TITLE]      = LGET("File Manager ") ;
  Str[(int) M_WASTE_TITLE]     = LGET("FM: Waste:") ;
  Str[(int) M_FV_TITLE]        = LGET("FM: Folder View:") ;
  Str[(int) M_NO_POS_VIEW] = LGET("No positional view for this display mode") ;
  Str[(int) M_CCMD_TITLE]      = LGET("Press return to quit.") ;
  Str[(int) M_NO_HOME_CHANGE]  = LGET("You cannot change the Home entry") ;
  Str[(int) M_NO_DISK_NAME]    = LGET("No disk name given") ;
  Str[(int) M_RECURSIVE]       = LGET("Recursive copy. Operation aborted.") ;
  Str[(int) M_STOP_ABORT]      = LGET("Operation aborted.") ;
  Str[(int) M_NSELECTED]       = MGET("%d files selected.") ;

  Str[(int) M_COM_TITLE]   = LGET("\nWhat is your job title?\n\n\n") ;
  Str[(int) M_COM_LONG]    = LGET("How long have you been using computers of any kind?\n\n\n") ;
  Str[(int) M_COM_TIME]    = LGET("What percentage of your time is spent working directly with computers?\n\n\n") ;
  Str[(int) M_COM_PRIMARY] = LGET("What type is your primary computer?\n\n\n") ;
  Str[(int) M_COM_OTHER]   = LGET("What other types of computers have you used?\n\n\n") ;
  Str[(int) M_COM_APPS]    = LGET("What are the three most common applications or uses of your computer?\n\n\n") ;
  Str[(int) M_COM_USAGE]   = LGET("What percentage of the time do you use:\n") ;
  Str[(int) M_COM_USAGE1]  = LGET("        OpenWindows?\n") ;
  Str[(int) M_COM_USAGE2]  = LGET("        the command-line interface, the Console, and the Command Tool?\n\n\n") ;
  Str[(int) M_COM_COM]     = LGET("Please add your comments below this line.\n") ;
  Str[(int) M_COM_DIVIDE]  = LGET("=========================================\n\n") ;
  Str[(int) M_QUIT_MES] = LGET("Are you sure you want to quit File Manager?") ;
  Str[(int) M_QUIT_FM]  = LGET("Quit File Manager") ;
  Str[(int) M_RCP_FAIL] =
    LGET("Remote Copy from '%s' to '%s' has failed. \nThe permissions may not be set up appropriately. \nSee your system administrator for more help.") ;
  Str[(int) M_NO_WRITE]  = LGET("Floppy is not writable") ;
}
