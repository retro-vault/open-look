/*
 * Implements the p msg routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_msg_.h>
#include <xview_private/p_utl_.h>

static void msg_accept_preview(Panel_item item_public, Event *event);
static void msg_paint(Panel_item item_public);

static Panel_ops ops = {
    panel_default_handle_event,		/* handle_event() */
    NULL,				/* begin_preview() */
    NULL,				/* update_preview() */
    NULL,				/* cancel_preview() */
    msg_accept_preview,			/* accept_preview() */
    NULL,				/* accept_menu() */
    NULL,				/* accept_key() */
    panel_default_clear_item,		/* clear() */
    msg_paint,				/* paint() */
    NULL,				/* resize() */
    NULL,				/* remove() */
    NULL,				/* restore() */
    NULL,				/* layout() */
    NULL,				/* accept_kbd_focus() */
    NULL,				/* yield_kbd_focus() */
    NULL				/* extension: reserved for future use */
};


/* ========================================================================= */

/* -------------------- XView Functions  -------------------- */
/* ARGSUSED */
Pkg_private int
panel_message_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);

    ip->ops = ops;
    if (panel->event_proc)
	ip->ops.panel_op_handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_MESSAGE_ITEM;

    return XV_OK;
}


/* --------------------  Panel Item Operations  -------------------- */
static void
msg_accept_preview(item_public, event)
    Panel_item	    item_public;
    Event          *event;
{
    Item_info      *ip = ITEM_PRIVATE(item_public);

    (*ip->notify) (item_public, event);
}


static void
msg_paint(item_public)
    Panel_item	    item_public;
{
    Item_info      *ip = ITEM_PRIVATE(item_public);

    panel_paint_image(ip->panel, &ip->label, &ip->label_rect, inactive(ip),
		      ip->color_index);
}
