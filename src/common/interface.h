/*
  INTERFACE.h  -  Lumiera interface macros and structures

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file interface.h
 ** @brief Lumiera interface macros and structures.
 **
 ** Lumiera uses a system of versioned interfaces as external extension points
 ** and for defining plug-ins. The interfaces defined here are C compatible and,
 ** thus, can be used by any language able to bind to C. The interfaces are
 ** versioned to provide forward and backward compatibility for both source and
 ** binary deployment of modules.
 **
 ** # Overview
 **
 ** To make an interface available to code so that the code can use the
 ** interface, the interface needs to be declared and then defined. We provide a
 ** number of macros here which ease this process.
 **
 ** The interface is declared by placing the following macro in a
 ** header file: 
 ** \code
 **  LUMIERA_INTERFACE_DECLARE(name, version,
 **                            LUMIERA_INTERFACE_SLOT(ret, name, params),
 **                            ...
 **                           )
 ** 
 ** \endcode
 ** Any code that wants to use this interface must then include the header file.
 **
 ** The interface is defined by mapping interface functions to slots, or
 ** providing inline definitions for slot functions. Defining the interface has
 ** the following form:
 ** \code
 **  LUMIERA_INTERFACE_INSTANCE(iname, version, name, descriptor, acquire, release,
 **                             LUMIERA_INTERFACE_MAP (slot, function),
 **                             LUMIERA_INTERFACE_INLINE (slot, ret, params, {body}),
 **                             ...
 **                            )
 ** \endcode
 **
 ** A collection of interfaces can be defined in 2 different ways depending on
 ** where whether the interface is exported by the core, or by a plugin:
 ** \code 
 ** LUMIERA_EXPORT(queryfunc,
 **                LUMIERA_INTERFACE_DEFINE(...),
 **                ...
 **                ) // Exporting from the core
 ** 
 **  LUMIERA_PLUGIN(descriptor, acquire, release,
 **                 LUMIERA_INTERFACE_DEFINE(...),
 **                 ...
 **                 ) // Exporting from an interface
 ** \endcode
 **
 ** # Naming and Versioning
 **
 ** Interfaces have unique names and a major and minor version. The name and the major version
 ** is used to construct a C identifier for the interface, the minor version is implicitly defined
 ** by the number of functions in the interface. Interface instances are not versioned by the
 ** interface system, versioning these will be defined somewhere else.
 **
 ** Slot names are normal C identifiers, how these shall be versioned has to be defined somewhere
 ** else and is not the subject of the interface system. Each function can have its own unique UUID.
 */

#ifndef LUMIERA_INTERFACE_H
#define LUMIERA_INTERFACE_H

#include "lib/ppmpl.h"
#include "lib/psplay.h"



/* ==== Macros to Declare an Interface */

/**
 * 
 * Construct a type identifier for an interface
 * @param name name of the interface
 * @param version major version of this interface
 */
#define LUMIERA_INTERFACE_INAME(name, version) name##_##version

/**
 * Construct a definition identifier for an interface
 * @param iname name of the interface
 * @param version major version of the interface
 * @param dname name for the instance
 * @return constructed identifier
 */
#define LUMIERA_INTERFACE_DNAME(iname, version, dname) PPMPL_CAT (LUMIERA_INTERFACE_INAME (iname, version), _##dname)

/**
 * Construct a definition string for an interface
 * @param iname name of the interface
 * @param version major version of the interface
 * @param dname name for the instance
 * @return constructed string
 */
#define LUMIERA_INTERFACE_DSTRING(iname, version, dname) PPMPL_STRINGIFY (LUMIERA_INTERFACE_DNAME (iname, version, dname))

/**
 * Return a reference (pointer) to an interface implementation
 * @param iname name of the interface
 * @param version major version of the interface
 * @param dname name for the instance
 */
#define LUMIERA_INTERFACE_REF(iname, version, dname) \
  (LumieraInterface)&LUMIERA_INTERFACE_DNAME(iname, version, dname)


/**
 * Construct the type of the interface
 * @param name name of the interface
 * @param version major version of this interface
 */
#define LUMIERA_INTERFACE_TYPE(name, version) struct LUMIERA_INTERFACE_INAME(name, version)


/**
 * Construct a cast to the target interface type
 * Used to cast a generic LumieraInterface to the real type
 * @param name name of the interface
 * @param version major version of this interface
 */
#define LUMIERA_INTERFACE_CAST(name, version) (LUMIERA_INTERFACE_TYPE(name, version)*)


/**
 * Declare an interface.
 * @param name name of the interface
 * @param version major version of this interface declaration. 0 denotes an experimental interface,
 * otherwise this will begin at 1 onwards for each new (incompatible) change to an interface.
 * The older interface declarations may still be maintained in parallel (backwards compatibility!).
 * @param ... Slot declarations for the functions provided by this interface @see LUMIERA_INTERFACE_SLOT
 * The number of Slots in an interface defines its 'minor' version.
 * New slots must be added at the end. The prototype and order of existing slots must not be changed.
 * Slots may be renamed, for example a slot 'foo' can be renamed to 'foo_old' when a new 'foo' slot is
 * added. Binary modules will then still use the 'foo_old' slot which was the 'foo' slot at the
 * compile time while compiling modules from source will use the new 'foo' slot. This may be
 * intentionally used to break compilation and force the update of modules to a new api.
 */
#define LUMIERA_INTERFACE_DECLARE(name, version, ...)   \
LUMIERA_INTERFACE_TYPE(name, version)                   \
{                                                       \
  lumiera_interface interface_header_;                  \
  PPMPL_FOREACH(_, __VA_ARGS__)                         \
}

/**
 * Declare a function slot inside an interface.
 * @param ret return type of the function
 * @param name name of this slot
 * @param params parenthesised list of parameters for the function
 */
#define PPMPL_FOREACH_LUMIERA_INTERFACE_SLOT(ret, name, params)  \
        ret (*name) params; \


/*
  Macros to Define an Interface
 */

/**
 * Define an interface instance.
 * @param iname name of the interface to instance
 * @param version major version of the interface to instance
 * @param name name of the instance
 * @param descriptor pointer to an interface instance which provides a description of this interface, might be NULL
 * @param acquire a function which is called whenever this interface is opened for using, might be NULL
 * @param release a function which is called whenever this interface is closed after use, might be NULL
 * @param ... map functions to interface slots @see LUMIERA_INTERFACE_MAP
 */
#define LUMIERA_INTERFACE_INSTANCE(iname, version, name, descriptor, acquire, release, ...)     \
PPMPL_FOREACH(_P1_, __VA_ARGS__)                                                                \
LUMIERA_INTERFACE_TYPE(iname, version) LUMIERA_INTERFACE_DNAME(iname, version, name) =          \
{                                                                                               \
{                                                                                               \
  #iname,                                                                                       \
  version,                                                                                      \
  #name,                                                                                        \
  sizeof (LUMIERA_INTERFACE_TYPE(iname, version)),                                              \
  descriptor,                                                                                   \
  acquire,                                                                                      \
  release                                                                                       \
},                                                                                              \
PPMPL_FOREACH(_P2_, __VA_ARGS__)                                                                \
}


/**
 * Map a function to a interface slot
 * @param slot name of the slot to be mapped
 * @param function name of the function to be mapped on slot
 *
 * @note C++ requires that all mappings are in the same order than defined in the interface declaration,
 * this would be good style for C too anyways
 */
#define PPMPL_FOREACH_P1_LUMIERA_INTERFACE_MAP(slot, function)
#ifdef __cplusplus
#define PPMPL_FOREACH_P2_LUMIERA_INTERFACE_MAP(slot, function) \
    function,\

#else
#define PPMPL_FOREACH_P2_LUMIERA_INTERFACE_MAP(slot, function) \
    .slot = function,\

#endif


/**
 * Map a inline defined function to a interface slot
 * @param slot name of the slot to be mapped
 * @param ret return type of the inline function
 * @param params parenthesised list of parameters given to the function
 * @param ... braced function body
 *
 * @note C++ requires that all mappings are in the same order than defined in the interface declaration,
 * this would be good style for C too anyways
 */
#define PPMPL_FOREACH_P1_LUMIERA_INTERFACE_INLINE(slot, ret, params, ...) \
  static ret                                                                    \
  LUMIERA_INTERFACE_INLINE_NAME(slot) params                                    \
  __VA_ARGS__

#ifdef __cplusplus
#define PPMPL_FOREACH_P2_LUMIERA_INTERFACE_INLINE(slot, ret, params, ...) \
    LUMIERA_INTERFACE_INLINE_NAME(slot),\

#else
#define PPMPL_FOREACH_P2_LUMIERA_INTERFACE_INLINE(slot, ret, params, ...) \
    .slot = LUMIERA_INTERFACE_INLINE_NAME(slot),\

#endif

#define LUMIERA_INTERFACE_INLINE_NAME(slot) PPMPL_CAT(lumiera_##slot##_l, __LINE__)


#define PPMPL_FOREACH_L1_P1_LUMIERA_INTERFACE_DEFINE(iname, version, name, descriptor, acquire, release, ...)   \
LUMIERA_INTERFACE_INSTANCE (iname, version,                                                                     \
                            name,                                                                               \
                            descriptor,                                                                         \
                            acquire,                                                                            \
                            release,                                                                            \
                            __VA_ARGS__                                                                         \
                            );


#define PPMPL_FOREACH_L1_P2_LUMIERA_INTERFACE_DEFINE(iname, version, name, descriptor, acquire, release, ...)     \
  &LUMIERA_INTERFACE_DNAME(iname, version, name).interface_header_,


/**
 * Generate interface container suitable for enumerating interfaces.
 * This takes a list of interface definitions, instantiates them and places pointers to them
 * into a zero terminated array which address is returned by a static function named 'lumiera_plugin_interfaces'.
 * For interfaces generated by he core, the user is responsible to register these dynamically.
 * When LUMIERA_PLUGIN is defined, things change and an additional 'lumieraorg__plugin' interface is generated.
 * The plugin loader then uses this to register the provided interfaces automatically.
 * @param ... list of LUMIERA_INTERFACE_DEFINE()/LUMIERA_INTERFACE_INLINE() for all interfaces this plugin provides.
 */
#define LUMIERA_EXPORT(...)                     \
PPMPL_FOREACH_L1(_P1_, __VA_ARGS__)             \
static LumieraInterface*                        \
lumiera_plugin_interfaces (void)                \
{                                               \
 static LumieraInterface interfaces[] =         \
  {                                             \
    PPMPL_FOREACH_L1(_P2_, __VA_ARGS__)         \
    NULL                                        \
  };                                            \
  return interfaces;                            \
}                                               \
LUMIERA_PLUGININTERFACE


/**
 * Create a plugin interface when being compiled as plugin
 */
#ifdef LUMIERA_PLUGIN /* compile as plugin */
#define LUMIERA_PLUGIN_INTERFACEHANDLE static LUMIERA_INTERFACE_HANDLE(lumieraorg_interface, 0) lumiera_interface_handle
#define LUMIERA_PLUGIN_STORE_INTERFACEHANDLE(name)  lumiera_interface_handle = LUMIERA_INTERFACE_CAST (lumieraorg_interface, 0) name

#define LUMIERA_PLUGININTERFACE                                                         \
LUMIERA_INTERFACE_INSTANCE (lumieraorg__plugin, 0,                                      \
                            lumieraorg_plugin,                                          \
                            NULL,                                                       \
                            NULL,                                                       \
                            NULL,                                                       \
                            LUMIERA_INTERFACE_MAP (plugin_interfaces,                   \
                                                   lumiera_plugin_interfaces)           \
                            );
#define LUMIERA_INTERFACE_REGISTEREXPORTED
#define LUMIERA_INTERFACE_UNREGISTEREXPORTED

#define  LUMIERA_INTERFACE_OPEN(interface, version, minminor, name) \
  LUMIERA_INTERFACE_CAST(interface, version) lumiera_interface_handle->open (#interface, version, minminor, #name)

#define  LUMIERA_INTERFACE_CLOSE(handle) \
  lumiera_interface_handle->close (&(handle)->interface_header_)


#else /* compile as builtin */

#define LUMIERA_PLUGIN_INTERFACEHANDLE static LUMIERA_INTERFACE_HANDLE(lumieraorg_interface, 0) lumiera_interface_handle
#define LUMIERA_PLUGIN_STORE_INTERFACEHANDLE(name)  lumiera_interface_handle = LUMIERA_INTERFACE_CAST (lumieraorg_interface, 0) name

#define LUMIERA_PLUGININTERFACE
/**
 * Register all exported interfaces when not a plugin
 * This is a no-op when LUMIERA_PLUGIN is defined, since plugins are automatically registered
 */
#define LUMIERA_INTERFACE_REGISTEREXPORTED      \
  lumiera_interfaceregistry_bulkregister_interfaces (lumiera_plugin_interfaces(), NULL)
/**
 * Unregister all exported interfaces when not a plugin
 * This is a no-op when LUMIERA_PLUGIN is defined, since plugins are automatically registered
 */
#define LUMIERA_INTERFACE_UNREGISTEREXPORTED    \
  lumiera_interfaceregistry_bulkremove_interfaces (lumiera_plugin_interfaces())

#define  LUMIERA_INTERFACE_OPEN(interface, version, minminor, name) \
  LUMIERA_INTERFACE_CAST(interface, version) lumiera_interface_open (#interface, version, minminor, #name)

#define  LUMIERA_INTERFACE_CLOSE(handle) \
  lumiera_interface_close (&(handle)->interface_header_)

#endif



/**
 * create a handle for a interface (WIP)
 */

#define  LUMIERA_INTERFACE_HANDLE(interface, version) \
  LUMIERA_INTERFACE_TYPE(interface, version)*


typedef struct lumiera_interfaceslot_struct lumiera_interfaceslot;
typedef lumiera_interfaceslot* LumieraInterfaceslot;

typedef struct lumiera_interface_struct lumiera_interface;
typedef lumiera_interface* LumieraInterface;

/**
 * This is just a placeholder for an entry in a interface table.
 * It consists of one here generic, later correctly prototyped function pointer and
 * a unique identifier which is associated with this function.
 */
struct lumiera_interfaceslot_struct
{
  void (*func)(void);
};


/**
 * Header for an interface, just the absolute necessary metadata.
 */
struct lumiera_interface_struct
{
  /** name of the interface (type) */
  const char* interface;

  /** major version, 0 means experimental */
  unsigned version;

  /** name of this instance */
  const char* name;

  /** size of the whole interface structure (minor version) */
  size_t size;

  /** metadata descriptor, itself a interface (or NULL) */
  LumieraInterface descriptor;

  /**
   * Must be called before this interface is used.
   * might be nested.
   * @param self pointer to the interface to be acquired
   * @param interfaces pointer to a 'interfaces' interface giving plugins access to
   *        opening and closing interfaces, this is already opened and if a plugin
   *        wants to use other interfaces it has to store this pointer
   * @return pointer to the interface or NULL on error
   */
  LumieraInterface (*acquire)(LumieraInterface self, LumieraInterface interfaces);
  /**
   * called when finished using this interface
   * must match the acquire calls
   * @param self pointer to the interface to be released
   */
  void (*release)(LumieraInterface self);

#ifndef __cplusplus
  /** placeholder array for the following function slots, C++ doesn't support flexible arrays */
  lumiera_interfaceslot functions[];
#endif
};

/*
  API to handle interfaces
 */

/**
 * Open an interface by version and name.
 * Looks up the requested interface, possibly loading it from a plugin.
 * @param interface name of the interface definition
 * @param version major version of the interface definition
 * @param minminorversion required minor version (structure size)
 * @param name name of the interface implementation
 * @return the queried interface handle on success, else NULL
 */
LumieraInterface
lumiera_interface_open (const char* interface, unsigned version, size_t minminorversion, const char* name);

/**
 * Close an interface after use.
 * @param self interface to be closed
 * consider 'self' to be invalidated after this call
 */
void
lumiera_interface_close (LumieraInterface self);

/**
 * Runtime check for interface type and version
 * @param self interface to check
 * @param iname name of the interface
 * @return version of the interface or ~0 (-1) if iname doesn't match the requested name
 */
unsigned
lumiera_interface_version (LumieraInterface self, const char* iname);

/**
 * Define an interface for the above
 */
LUMIERA_INTERFACE_DECLARE (lumieraorg_interface, 0,
                           LUMIERA_INTERFACE_SLOT (LumieraInterface,
                                                   open,
                                                   (const char* interface, unsigned version, size_t minminorversion, const char* name)),
                           LUMIERA_INTERFACE_SLOT (void, close, (LumieraInterface self)),
                           LUMIERA_INTERFACE_SLOT (unsigned, version, (LumieraInterface self, const char* iname)),
);

/**
 * registering implementations of the above interface
 */
void
lumiera_interface_init (void);

/**
 * deregistering implementations of the above interface
 */
void
lumiera_interface_destroy (void);





/**
 * Plugin interface
 */
LUMIERA_INTERFACE_DECLARE (lumieraorg__plugin, 0,
                           LUMIERA_INTERFACE_SLOT (LumieraInterface*, plugin_interfaces, (void)),
);



#endif /* LUMIERA_INTERFACE_H */
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
