/*
  INTERFACE-DESCRIPTOR.h  -  Metadata interface for Lumiera interfaces

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file interface-descriptor.h
 ** A data record to describe interface, interface instances and plug-in
 ** instances. It is used by the _interface system_ to register, open
 ** and manage global extension points and separation interfaces between
 ** the components of the application.
 ** 
 */


#ifndef COMMON_INTERFACEDESCRIPTOR_H
#define COMMON_INTERFACEDESCRIPTOR_H

#include "common/interface.h"


/**
 * Release state of an interface implementation.
 * The interface subsystem must be able to categorise implementations to present possible
 * upgrade paths to the user. This is done by the tagging it to a certain state in concert
 * with the version and the user supplied version compare function. The respective numbers
 * are chosen in a way that a higher value indicates precedence when selecting an implementation.
 * Note that 'BETA' is higher than 'DEPRECATED', we make the assumption that BETA is at least
 * maintained code and something gets deprecated for some reason. For common practice it is
 * suggested to make a stable release before declaring its predecessor version as deprecated.
 */
enum lumiera_interface_state {
  /** some known bugs exist which won't be fixed, don't use this */
  LUMIERA_INTERFACE_BROKEN = -1,
  /** Not finished development code */
  LUMIERA_INTERFACE_EXPERIMENTAL = 0,
  /** Old implementation which is now unmaintained and will be removed soon */
  LUMIERA_INTERFACE_DEPRECATED = 1,
  /** Finished but not finally released implementation for open testing */
  LUMIERA_INTERFACE_BETA = 2,
  /** Finished, released and maintained implementation */
  LUMIERA_INTERFACE_STABLE = 3
};


/**
 * Interface descriptor.
 * This defines an interface for querying metadata common to all interface implementations
 */
LUMIERA_INTERFACE_DECLARE (lumieraorg_interfacedescriptor, 0,
                           /* The following slots are some human-readable descriptions of certain properties */
                           LUMIERA_INTERFACE_SLOT (const char*, name, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, brief, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, homepage, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, version, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, author, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, email, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, copyright, (LumieraInterface)),
                           LUMIERA_INTERFACE_SLOT (const char*, license, (LumieraInterface)),

                           /* some flags for properties */
                           LUMIERA_INTERFACE_SLOT (int, state, (LumieraInterface)),

                           /* compare 2 version strings in a custom way */
                           LUMIERA_INTERFACE_SLOT (int, versioncmp, (const char*, const char*))
                           );


#if 0
/**
 * For convenience: a copy'n'paste descriptor
 */
LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0,
                            /*IDENTIFIER*/,
                            NULL, NULL, NULL,
                            LUMIERA_INTERFACE_INLINE (name, LUIDGEN,
                                                      const char*, (LumieraInterface iface),
                                                      {return /*NAME*/;}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (brief, LUIDGEN,
                                                      const char*, (LumieraInterface iface),
                                                      {return /*BRIEF*/;}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (homepage, LUIDGEN,
                                                      const char*, (LumieraInterface iface),
                                                      {return /*HOMEPAGE*/;}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (version, LUIDGEN,
                                                      const char*, (LumieraInterface iface),
                                                      {return /*VERSION*/;}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (author, LUIDGEN,
                                                      const char*, (LumieraInterface iface),
                                                      {return /*AUTHOR*/;}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (email, LUIDGEN,
                                                      const char*, (LumieraInterface iface),
                                                      {return /*EMAIL*/;}
                                                      ),
                            LUMIERA_INTERFACE_INLINE (copyright, LUIDGEN,
                                                      const char*, (LumieraInterface iface),
                                                      {
                                                        return
                                                          "Copyright (C)        "/*ORGANIZATION*/"\n"
                                                          "  "/*YEARS*/"               "/*AUTHOR*/" <"/*EMAIL*/">";
                                                      }
                                                      ),
                            LUMIERA_INTERFACE_INLINE (license, LUIDGEN,
                                                      const char*, (LumieraInterface iface),
                                                      {
                                                        return
                                                          /*LICENSE*/;
                                                      }
                                                      ),

                            LUMIERA_INTERFACE_INLINE (state, LUIDGEN,
                                                      int, (LumieraInterface iface),
                                                      {return /*LUMIERA_INTERFACE_EXPERIMENTAL*/;}
                                                      ),

                            LUMIERA_INTERFACE_INLINE (versioncmp, LUIDGEN,
                                                      int, (const char* a, const char* b),
                                                      {return /*COMPARERESULT*/;}
                                                      )
                            );
#endif


#endif /* COMMON_INTERFACEDESCRIPTOR_H */
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
