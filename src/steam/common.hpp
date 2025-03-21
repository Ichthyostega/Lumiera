/*
  COMMON.hpp  -  global definitions and common types for the Steam-Layer
 

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file steam/common.hpp
 ** Basic set of definitions and includes commonly used together.
 ** Including common.hpp gives you a common set of elementary declarations
 ** widely used within the C++ code of the Steam-Layer. Besides that, this
 ** header is used to attach the doxygen documentation comments for all
 ** the primary Stean-Layer namespaces
 ** 
 ** @see main.cpp
 ** 
 */



#ifndef STEAM_COMMON_H
#define STEAM_COMMON_H




/* ===== frequently used common types ===== */

#include "lib/p.hpp"
#include "lib/util.hpp"
#include "lib/nocopy.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/symbol.hpp"
#include "lib/error.hpp"   // pulls in NoBug via loggging.h



/**
 * Steam-Layer implementation namespace root.
 * Lumiera's middle layer contains the core models, both
 * high-level (session) and low-level (render nodes), together with
 * the Builder to translate between those two models, the command frontend,
 * session support system, the playback-and-render-control subsystem and
 * the API and backbone of the renderengine. Most render implementation
 * code resides in the vault layer or is loaded from plug-ins though.
 * @ingroup steam
 */
namespace steam {
  
  
  
  /**
   * The asset subsystem of the Steam-Layer.
   */
  namespace asset { }
  
  
  /**
   * Steam-Layer dispatcher, controller and administrative facilities.
   */
  namespace control { }
  
  
  /**
   * Playback and rendering control subsystem.
   * The so called "player" combines and orchestrates services from
   * the engine, session and vault to perform playback or rendering
   */
  namespace play {
    
  }
  
  
  /**
   * Lumiera's render engine core and operational control.
   * Backbone of the engine, render nodes base and cooperation.
   * A good deal of the active engine code is outside the scope of the
   * Steam-Layer, e.g. code located in vault services and plugins.
   */
  namespace engine {
    
    
    /**
     * Policies, definitions and tweaks to control the actual setup
     * and behaviour of the render engine, and the way, render nodes
     * are wired and instantiated
     */
    namespace config { }
  }
  
  
  
  /**
   * Media-Objects, edit operations and high-level session.
   * @todo is this interface or implementation ??
   */
  namespace mobject {
  
  
    /**
     * Namespace of Session and user visible high-level objects.
     */
    namespace session { }
  
  
    /**
     * Namespace of the Builder, transforming high-level into low-level.
     */
    namespace builder { }
  
  
  }//(End)namespace mobject
  
  
  /**
   * Fixture and low-level-Model.
   * The shared data structure used as render nodes network.
   * It is created or updated by the [Builder](\ref session::builder)
   * after any relevant change of \ref Session contents, while used
   * by the engine as work data structure for the render jobs.
   */
  namespace fixture {
    
  }
  
} //(End)namespace steam

#endif /*STEAM_COMMON_H*/
