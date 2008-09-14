/*
  interface.h  -  Lumiera interface macros and structures

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef LUMIERA_INTERFACE_H
#define LUMIERA_INTERFACE_H

#include "lib/luid.h"
#include "lib/ppmpl.h"

/*
  Interface declaration macros
 */

#define LUMIERA_INTERFACE_INAME(name, version) lumiera_interface_##name##_##version
#define LUMIERA_INTERFACE_DNAME(iname, dname, version) dname##_##version##_interface

#define LUMIERA_INTERFACE_TYPE(name, version) struct LUMIERA_INTERFACE_INAME(name, version)
#define LUMIERA_INTERFACE_CAST(name, version) (LUMIERA_INTERFACE_TYPE(name, version)*)



#define PPMPL_FOREACH_LUMIERA_INTERFACE_SLOT(ret, name, params)  ret (*name) params; lumiera_uid name##_uid;


#define LUMIERA_INTERFACE_DECLARE(name, version, ...)   \
LUMIERA_INTERFACE_TYPE(name, version)                   \
{                                                       \
  lumiera_interface interface_header_;                  \
  PPMPL_FOREACH(_, __VA_ARGS__)                         \
}


/*
  Interface definition macros
 */


#ifdef __cplusplus
#define PPMPL_FOREACH_LUMIERA_INTERFACE_MAP(slot, function, uid) \
  function, LUMIERA_UID_INITIALIZER (uid),
#else
#define PPMPL_FOREACH_LUMIERA_INTERFACE_MAP(slot, function, uid) \
  .slot = function, .slot##_uid = LUMIERA_UID_INITIALIZER (uid),
#endif


/**
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
#define LUMIERA_INTERFACE_INITIALIZER(iname, version, name, descriptor, data, acquire, release, ...)    \
{                                                                                                       \
  {                                                                                                     \
    #name,                                                                                              \
    version,                                                                                            \
    sizeof (LUMIERA_INTERFACE_TYPE(iname, version)),                                                    \
    descriptor,                                                                                         \
    data,                                                                                               \
    acquire,                                                                                            \
    release                                                                                             \
  },                                                                                                    \
  PPMPL_FOREACH(_, __VA_ARGS__)                                                                         \
}


#define LUMIERA_INTERFACE_DEFINE(iname, version, name, descriptor, data, acquire, release, ...)         \
LUMIERA_INTERFACE_TYPE(iname, version) LUMIERA_INTERFACE_DNAME(iname, name, version) =                  \
  LUMIERA_INTERFACE_INITIALIZER(iname, version, name, descriptor, data, acquire, release, __VA_ARGS__);



#define PPMPL_FOREACH_L1_P1_LUMIERA_INTERFACE(iname, version, name, descriptor, data, acquire, release, ...)    \
LUMIERA_INTERFACE_DEFINE (iname, version,                                                                       \
                          name,                                                                                 \
                          descriptor,                                                                           \
                          data,                                                                                 \
                          acquire,                                                                              \
                          release,                                                                              \
                          __VA_ARGS__                                                                           \
                          );


#define PPMPL_FOREACH_L1_P2_LUMIERA_INTERFACE(iname, version, name, descriptor, data, acquire, release, ...) \
  &LUMIERA_INTERFACE_DNAME(iname, name, version).interface_header_,


#define LUMIERA_PLUGIN(descriptor, data, acquire, release, luid, ...)                           \
PPMPL_FOREACH_L1(_P1_, __VA_ARGS__)                                                             \
static const LumieraInterface*                                                                  \
plugin_interfaces (void)                                                                        \
{                                                                                               \
 static const LumieraInterface interfaces[] =                                                   \
  {                                                                                             \
    PPMPL_FOREACH_L1(_P2_, __VA_ARGS__)                                                         \
    NULL                                                                                        \
  };                                                                                            \
  return interfaces;                                                                            \
}                                                                                               \
LUMIERA_INTERFACE_DEFINE (plugin, 0,                                                            \
                          lumiera_plugin,                                                       \
                          NULL,                                                                 \
                          NULL,                                                                 \
                          NULL,                                                                 \
                          NULL,                                                                 \
                          LUMIERA_INTERFACE_MAP (plugin_interfaces, plugin_interfaces, luid)    \
                          )



#define LUMIERA_EXPORT(queryfunc, ...)                                  \
PPMPL_FOREACH_L1(_P1_, __VA_ARGS__)                                     \
static const LumieraInterface*                                          \
queryfunc (void)                                                        \
{                                                                       \
 static const LumieraInterface interfaces[] =                           \
  {                                                                     \
    PPMPL_FOREACH_L1(_P2_, __VA_ARGS__)                                 \
    NULL                                                                \
  };                                                                    \
  return interfaces;                                                    \
}                                                                       \


typedef struct lumiera_interfaceslot_struct lumiera_interfaceslot;
typedef lumiera_interfaceslot* LumieraInterfaceslot;

typedef struct lumiera_interface_struct lumiera_interface;
typedef lumiera_interface* LumieraInterface;

/**
 * This is just a placeholder for an entry in a interface table.
 * It consists of one here generic, later correctly prototyped functionppointer and
 * a unique identifier which is associated with this function.
 */
struct lumiera_interfaceslot_struct
{
  void (*func)(void);
  lumiera_uid uid;
};


/**
 * Header for an interface, just the absolute necessary metadata.
 */
struct lumiera_interface_struct
{
  /** name of the interface */
  const char* name;
  /** major version, 0 means experimental */
  unsigned version;
  /** size of the whole interface structure (minor version) */
  size_t size;

  /** metadata descriptor, itself a interface (or NULL) */
  LumieraInterface descriptor;

  /** generic data to be used by the implementation */
  void* data;

  /** must be called before this interface is used, might be nested */
  LumieraInterface (*acquire)(LumieraInterface self);
  /** called when finished with this interface, must match the acquire calls */
  void (*release)(LumieraInterface self);

  /** placeholder array for the following function slots, C++ doesn't support flexibe arrays */
#ifndef __cplusplus
  lumiera_interfaceslot functions[];
#endif
};


#endif /* LUMIERA_INTERFACE_H */
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
