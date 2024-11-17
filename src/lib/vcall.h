/*
  vcall.h  -  helper macros for virtual (method) calls in C

  Copyright (C)                 Lumiera.org
     2010,                    Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/

#ifndef LUMIERA_VCALL_H
#define LUMIERA_VCALL_H

#include <nobug.h>

/**
 * @file
 * This allows one to do polymorphic programming in C by referencing a vtable
 * member which contains function pointers to a structure and then calling
 * this 'virtual' functions through the VCALL macro.
 */

/*
  TODO usage example, see btree.h
*/

/**
 * helper macro for calling vtable functions
 * just adds syntacic sugar VCALL(obj, func, params...)
 * translates to obj->vtable->func(obj, params...) plus some saftey checks
 */
#define LUMIERA_VCALL(self, function, ...)              \
  ({                                                    \
    REQUIRE (self);                                     \
    REQUIRE (self->vtable->function);                   \
    self->vtable->function (self, ## __VA_ARGS__);      \
  })

#ifndef VCALL
#define VCALL LUMIERA_VCALL
#else
#error "WTF! someone defined VCALL!"
#endif

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
