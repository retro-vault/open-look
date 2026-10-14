#include <X11/Wc/COPY>

/*
* SCCS_data: @(#) XtMacros.c 1.2 92/03/18 11:02:29
*
* Some old implementations of Xt provide some of the Xt functions only as
* macros, in violation of the Xt spec.  Here are implementations of the ones
* not provided by SCO Open Desk Top (Motif 1.0).
*/

#include <X11/IntrinsicP.h>
#include <X11/Wc/WcCreateP.h>

#ifdef XtMapWidget
#undef XtMapWidget
#endif
#ifdef XtUnmapWidget
#undef XtUnmapWidget
#endif

void XtMapWidget(Widget widget)
{
    XMapWindow(XtDisplay(widget), XtWindow(widget));
}

void XtUnmapWidget(Widget widget)
{
    XUnmapWindow(XtDisplay(widget), XtWindow(widget));
}

char* XtName(Widget w)
{
    if (XtIsWidget(w))
	return w->core.name;
    else
	return XrmQuarkToString(w->core.xrm_name);
}
