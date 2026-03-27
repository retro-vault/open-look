/*
 * Implements the p compat routines used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_compat_.h>
#include <xview/pkg.h>
#include <xview_private/portable.h>

/*
 * routines for compatibility with SunView 1.n
 */


Sv1_public      Panel_item
#ifdef ANSI_FUNC_PROTO
_panel_create_item(Panel client_panel, Xv_pkg *item_type, ...)
#else
_panel_create_item(client_panel, item_type, va_alist)
    Panel           client_panel;
    Xv_pkg *item_type;
va_dcl
#endif
{
    AVLIST_DECL;
    va_list         valist;

    VA_START(valist, item_type);
    MAKE_AVLIST( valist, avlist );
    va_end(valist);

    /*
     * convert row/column units to pixels now. This is provided for
     * compatibility with ATTR_ROW/COL().
     */
    window_rc_units_to_pixels(client_panel, avlist);

    return xv_create_avlist(client_panel, item_type, avlist);
}


Sv1_public      Panel_attribute_value
#ifdef ANSI_FUNC_PROTO
_panel_get(Panel client_object, Panel_attr attr, ...)
#else
_panel_get(client_object, attr, va_alist)
    Panel           client_object;
    Panel_attr      attr;
va_dcl
#endif
{
    va_list valist;
    Panel_attribute_value getvalue;

    VA_START( valist, attr );
    getvalue = (Panel_attribute_value)xv_get_varargs(client_object, attr,
                                                     valist);
    va_end( valist );
    return getvalue;
}


Sv1_public int
#ifdef ANSI_FUNC_PROTO
_panel_set(Panel client_object, ...)
#else
_panel_set(client_object, va_alist)
    Panel           client_object;
va_dcl
#endif
{
    Item_info      *object = ITEM_PRIVATE(client_object);
    Panel_info     *panel;
    AVLIST_DECL;
    va_list         valist;

    VA_START(valist, client_object);
    MAKE_AVLIST( valist, avlist );
    va_end(valist);

    if (is_panel(object)) {
	return window_set(client_object, ATTR_LIST, avlist, 0);
    }
    /*
     * convert row/column units to pixels now. This is provided for
     * compatibility with ATTR_ROW/COL().
     */
    panel = ((Item_info *) object)->panel;

    window_rc_units_to_pixels(PANEL_PUBLIC(panel), avlist);

    (void) xv_set_avlist(client_object, avlist);
    return 1;
}

Sv1_public void
panel_destroy_item(client_item)
    Panel_item      client_item;
{
    xv_destroy(client_item);
}


Sv1_public void
panel_free(client_object)
    Panel           client_object;
{
    /* Must be immediate destroy for SunView 1.X compatibility. */
    xv_destroy_immediate(client_object);
}
