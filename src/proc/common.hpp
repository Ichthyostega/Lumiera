/*
  LUMIERA.hpp  -  global definitions and common types for the Proc-Layer
 
 
  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file lumiera.hpp
 ** Basic set of definitions and includes commonly used together.
 ** Including lumiera.hpp gives you a common set of elementary declarations
 ** widely used within the C++ code of the Proc-Layer.
 ** 
 ** @see main.cpp
 ** 
 */



#ifndef LUMIERA_H
#define LUMIERA_H




    /* common types frequently used... */

#include "lib/p.hpp"
#include "lib/util.hpp"
#include "lib/lumitime.hpp"
#include "include/symbol.hpp"
#include "include/error.hpp"   ///< pulls in NoBug via nobugcfg.h


/**
 * Lumiera public interface.
 * Global interfaces and facilities accessible from plugins and scripts.
 * It's probably a good idea to pull it in explicitly and to avoid nesting
 * implementation namespaces within \c lumiera::
 */
namespace lumiera {

  /* additional global configuration goes here... */
  
    
} // namespace lumiera


/**
 * Implementation namespace for support and library code.
 */
namespace lib { 

}


/**
 * The asset subsystem of the Proc-Layer.
 * @todo refactor proc namespaces
 */
namespace asset { }


/**
 * Proc-Layer dispatcher, controller and administrative facilities.
 * @todo refactor proc namespaces
 */
namespace control { }


/**
 * Render engine code as part of the Proc-Layer.
 * Backbone of the engine, render nodes base and cooperation.
 * A good deal of the active engine code is outside the scope of the
 * Proc-Layer, e.g. code located in backend services and plugins.
 * @todo refactor proc namespaces
 */
namespace engine { }



/**
 * Media-Objects, edit operations and high-level session.
 * @todo is this interface or implementation ??
 * @todo refactor proc namespaces
 */
namespace mobject {


  /**
   * Namespace of Session, EDL and user visible high-level objects.
   */
  namespace session { }


  /**
   * Namespace of the Builder, transforming high-level into low-level.
   */
  namespace builder { }

}

#endif /*LUMIERA_H*/
