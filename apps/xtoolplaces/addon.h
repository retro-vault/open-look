/*
 * Declares the addon interfaces used by the xtoolplaces application.
 *
 * Copyright (c) 1993-1998 Xerox Corporation.  All Rights Reserved.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

/*Structure for items to addon*/
typedef struct _addon {
        char *program;                          /*Program to add onto*/
        char *cmmd_line;                        /*Command line to add*/
        struct _addon *next;                    /*Pointer to next link*/
} ADDON;

