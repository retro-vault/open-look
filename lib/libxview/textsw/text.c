/*
 * Implements the text routines used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/text_.h>
#include <xview_private/attr_.h>
#include <xview_private/txt_once_.h>
#include <xview_private/txt_menu_.h>
#include <xview/attrol.h>
#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>
#include <xview/textsw.h>
#include <xview/win_struct.h>
#include <xview/window.h>
#include <xview/text.h>

/* Pkg_private */ int	text_notice_key;

Pkg_private int
textsw_init(parent, textsw_public, avlist)
    Xv_Window       parent;
    Textsw          textsw_public;
    Attr_attribute  avlist[];
{
    Attr_avlist     attrs;
    Textsw_status   dummy_status;
    Textsw_status  *status = &dummy_status;
    Xv_textsw      *textsw_object = (Xv_textsw *) textsw_public;
    Textsw_folio    folio = NEW(struct textsw_object);

    if (!text_notice_key)  {
	text_notice_key = xv_unique_key();
    }

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int)*attrs) {
	  case TEXTSW_STATUS:
	    status = (Textsw_status *) attrs[1];
	    break;
	  default:
	    break;
	}
    }

    if (!folio) {
	*status = TEXTSW_STATUS_CANNOT_ALLOCATE;
	return (XV_ERROR);
    }
    /* link to object */
    textsw_object->private_data = (Xv_opaque) folio;
    folio->public_self = textsw_public;

    folio = textsw_init_internal(folio, status, textsw_default_notify, (Textsw_attribute *)avlist);

    /*
     * BUG: Note the folio is not really initialized until the first view is
     * created.
     */
    return (folio ? XV_OK : XV_ERROR);
}
