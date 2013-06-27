/* STM8-specific DLib configuration. */
/* Copyright 2010 IAR Systems AB.    */
/* $Revision: 1677 $                  */


#ifndef _DLIB_PRODUCT_H
#define _DLIB_PRODUCT_H

#ifndef _SYSTEM_BUILD
  #pragma system_include
#endif

/*
 * Avoid large data structures and too much precision...
 */
#define _DLIB_SMALL_TARGET 1

/*
 * Use function pointers directly in difunct table instead of offsets.
 */
#define _DLIB_STM8ABI_DIFUNC_CONTAINS_OFFSETS 0

/*
 * Use hand coded floating point support functions
 */
#define _FLOATING_LESS_INLINES 1

#endif /* _DLIB_PRODUCT_H */
