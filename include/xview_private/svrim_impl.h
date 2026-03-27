/*
 * Declares private XView interfaces, types, and macros for svrim impl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef _xview_server_image_impl_h_already_included
#define _xview_server_image_impl_h_already_included

#include <xview/svrimage.h>
#include <xview/screen.h>
#include <xview/pixwin.h>
#include <xview_private/draw_impl.h>
#include <xview_private/pw_impl.h>

typedef struct {
    Server_image	public_self; /* Back pointer */
    Xv_Screen		screen; /* screen for the server_image */
    short		save_pixmap;     
}   Server_image_info;

#define SERVER_IMAGE_PRIVATE(image) \
	XV_PRIVATE(Server_image_info, Xv_server_image, image)
#define SERVER_IMAGE_PUBLIC(image)  XV_PUBLIC(image)

/* default values for server image attributes */
#define  SERVER_IMAGE_DEFAULT_DEPTH	1
#define  SERVER_IMAGE_DEFAULT_WIDTH	16
#define  SERVER_IMAGE_DEFAULT_HEIGHT	16

#endif  /* _xview_server_image_impl_h_already_included */
