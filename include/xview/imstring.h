/*
 * Declares public XView interfaces, types, and attributes for
 * imstring.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
typedef	struct _InputContext {
	Window		win;
	Xv_Window	xv_win;
	XrmDatabase	db;
}  InputContext;

typedef	enum {
	IMPlain		= 0,
	IMPrimary	= 1,
	IMSecondary	= 2,
}  IMTextAttrType;

typedef	struct	_IMString {
	int		len;
	IMTextAttrType	*attr;
	Bool		encoding_is_wchar;
	union {
		char	*text;
		wchar_t	*text_wc;
	} Text_data;
}  IMString;
