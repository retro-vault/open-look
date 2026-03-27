/*
 * Implements the om compat routines used by the XView menu module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/om_compat_.h>
#include <xview_private/om_impl.h>
#include <xview_private/portable.h>

/* ------------------------------------------------------------------- */

Sv1_public      Menu
#ifdef ANSI_FUNC_PROTO
_menu_create(Attr_attribute attr1, ...)
#else
_menu_create(attr1, va_alist)
    Attr_attribute attr1;
va_dcl
#endif
{
    va_list         args;
    Attr_attribute  avarray[ATTR_STANDARD_SIZE];
    Attr_avlist     avlist = avarray;
    Menu            menu;

    if( attr1 )
    {
        VA_START(args, attr1);
        copy_va_to_av( args, avlist, attr1 );
        va_end(args);
    }
    else
        avlist[0] = (Attr_attribute)NULL;

    menu = (Menu) xv_create_avlist((Xv_object)NULL, MENU, avlist);
    return menu;
}

Sv1_public      Menu_item
#ifdef ANSI_FUNC_PROTO
_menu_create_item(Attr_attribute attr1, ...)
#else
_menu_create_item(attr1, va_alist)
    Attr_attribute attr1;
va_dcl
#endif
{
    Attr_attribute  avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    if( attr1 )
    {
        VA_START(valist, attr1);
        copy_va_to_av( valist, avlist, attr1 );
        va_end(valist);
    } 
    else 
        avlist[0] = (Attr_attribute)NULL; 

    return (Menu_item) xv_create_avlist((Xv_object)NULL, MENUITEM, avlist);
}

Sv1_public      Xv_opaque
#ifdef ANSI_FUNC_PROTO
_menu_set(Menu menu_public, ...)
#else
_menu_set(menu_public, va_alist)
    Menu            menu_public;
va_dcl
#endif
{
    AVLIST_DECL;
    va_list         valist;

    VA_START(valist, menu_public);
    MAKE_AVLIST( valist, avlist );
    va_end(valist);
    return xv_set_avlist(menu_public, avlist);
}

/*ARGSUSED*/
Sv1_public      Xv_opaque
menu_get(menu_public, attr, v1)
    Menu            menu_public;
    Xv_opaque       attr, v1;
{

    return xv_get(menu_public, attr, v1);

}

/*
 * for compatibility. BUG: note that this avoids the normal destroy
 * interposition chain, since there is no way to pass destroy_proc to the
 * low-level destroy routine.  Maybe the proc should be stashed in the menu
 * or item struct.
 */
Sv1_public void
menu_destroy_with_proc(m_public, destroy_proc)
    Menu            m_public;	/* menu or menu_item */
    void            (*destroy_proc) ();
{
    Xv_menu_info   *menu;
    Xv_menu_item_info *item;

    if (xv_get(m_public, XV_IS_SUBTYPE_OF, MENUITEM)) {
	item = MENU_ITEM_PRIVATE(m_public);
	item->extra_destroy_proc = destroy_proc;
    } else if (xv_get(m_public, XV_IS_SUBTYPE_OF, MENU)) {
	menu = MENU_PRIVATE(m_public);
	menu->extra_destroy_proc = destroy_proc;
    }
    xv_destroy(m_public);
}
