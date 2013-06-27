/* _INSTANTIATE macro */
/* Copyright 2003-2010 IAR Systems AB.  */

#ifndef _INSTANTIATE_H
#define _INSTANTIATE_H

/* Avoid warning for extra semicolon. */
#pragma diag_suppress=Pe381

#define PRAGMA(X) _Pragma(#X)
#define _INSTANTIATE(X) PRAGMA(keep_definition = X)
#define _INSTANTIATE_TYPE(X, Y) \
  PRAGMA(keep_definition = static_cast<Y>(X))

#endif /* _INSTANTIATE_H */
