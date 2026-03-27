/*
 * Declares internal interfaces used by the XView base module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(MACROS_H)
#define MACROS_H

#include <boost/preprocessor.hpp>

#define ARG(z, n, array) \
    BOOST_PP_COMMA() \
    (Attr_attribute)(BOOST_PP_ARRAY_ELEM(n, array))

#define ARGS(array) \
    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE(array), ARG, array)

#define MACRO_DEF1(f, t1, ...) \
    MACRO_DEF1_(f, t1, BOOST_PP_VARIADIC_TO_ARRAY(__VA_ARGS__))
    
#define MACRO_DEF1_(f, t1, array) \
    f((t1)(BOOST_PP_ARRAY_ELEM(0, array)) \
        ARGS(BOOST_PP_ARRAY_POP_FRONT(array)))
    
#define MACRO_DEF2(f, t1, t2, ...) \
    MACRO_DEF2_(f, t1, t2, BOOST_PP_VARIADIC_TO_ARRAY(__VA_ARGS__))
    
#define MACRO_DEF2_(f, t1, t2, array) \
    f((t1)(BOOST_PP_ARRAY_ELEM(0, array)), (t2)(BOOST_PP_ARRAY_ELEM(1, array)) \
        ARGS(BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_ARRAY_POP_FRONT(array))))
    
#define MACRO_DEF3(f, t1, t2, t3, ...) \
    MACRO_DEF3_(f, t1, t2, t3, BOOST_PP_VARIADIC_TO_ARRAY(__VA_ARGS__))
    
#define MACRO_DEF3_(f, t1, t2, t3, array) \
    f((t1)(BOOST_PP_ARRAY_ELEM(0, array)), (t2)(BOOST_PP_ARRAY_ELEM(1, array)) , (t3)(BOOST_PP_ARRAY_ELEM(2, array)) \
        ARGS(BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_ARRAY_POP_FRONT(array)))))

#define MACRO_DEF4(f, t1, t2, t3, t4, ...) \
    MACRO_DEF4_(f, t1, t2, t3, t4, BOOST_PP_VARIADIC_TO_ARRAY(__VA_ARGS__))
    
#define MACRO_DEF4_(f, t1, t2, t3, t4, array) \
    f((t1)(BOOST_PP_ARRAY_ELEM(0, array)), (t2)(BOOST_PP_ARRAY_ELEM(1, array)) , (t3)(BOOST_PP_ARRAY_ELEM(2, array)), \
        (t4)(BOOST_PP_ARRAY_ELEM(3, array)) \
        ARGS(BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_ARRAY_POP_FRONT(array))))))

#endif
