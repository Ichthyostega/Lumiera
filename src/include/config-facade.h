/*
  CONFIGFACADE  -  C++ convenience wrapper and startup of the config system

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file config-facade.h
 ** The lumiera::Config wrapper class addresses two issues.
 ** First, it registers startup and shutdown hooks to bring up the config system
 ** as early as possible. Later, on application main initialisation, the global
 ** config interface is opened and wrapped for convenient access from C++ code.
 ** 
 ** @note This was created as part of a first draft towards an application
 **       wide configuration system. Later (around 2012) it became clear that
 **       we can not judge the requirements for such a system yet, so we deferred
 **       the topic altogether. Meanwhile, this facade is sporadically used to
 **       mark the necessity to retrieve some "parametrisation values".
 ** @warning as a preliminary solution, the original configuration system draft
 **       was detached and replaced by an _ini file solution_ based on lib Boost.
 ** 
 ** @todo there ought to be an external Interface for the Config subsystem.
 **       But the full-blown Config system isn't implemented yet anyway
 ** 
 ** @see config.h
 ** @see lumiera::AppState
 ** @see main.cpp
 */


#ifndef INTERFACE_CONFIGFACADE_H
#define INTERFACE_CONFIGFACADE_H





#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "lib/depend.hpp"
#include "lib/symbol.hpp"

#include <string>


namespace lumiera {
  
  using std::string;
  
  
  /*****************************************************************//**
   * C++ wrapper for convenient access to the Lumiera config system.
   * 
   * @warning Config system not fully implemented yet. Thus for now
   *          this facade is wired with the setup.ini and will just
   *          fetch the values from there.
   */
  class Config
    {
    public:
      static string get (lib::Literal key);
      
      static lib::Depend<Config> instance;
      
      
    private:
      Config();
     ~Config();
      friend class lib::DependencyFactory<Config>;
    };
  
  
  
} // namespace lumiera



#else /* =========================== C Interface ====================== */


/** retrieve the default plugin search path from the
 *  basic applications setup.ini
 *  @return a fully expanded string suitable to be fed
 *          to the lumiera_config_setdefault function
 *  @see lumiera_plugin_discover
 */
const char* lumiera_get_plugin_path_default ();


#endif
#endif
