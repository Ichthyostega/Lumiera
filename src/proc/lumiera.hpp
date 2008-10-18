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

#include "proc/nobugcfg.hpp"


/**
 * Namespace for globals.
 * A small number of definitions and facilities of application wide relevance.
 * It's probably a good idea to pull it in explicitly and to avoid nesting
 * implementation namespaces within \c lumiera::
 */
namespace lumiera {

  /* additional global configuration goes here... */
  
    
} // namespace lumiera


/**
 * Namespace for support and library code.
 */
namespace lib { 

}


/**
 * The asset subsystem of the Proc-Layer.
 */
namespace asset { }


/**
 * Proc-Layer dispatcher, controller and administrative facilities.
 */
namespace control { }


/**
 * Render engine code as part of the Proc-Layer.
 * Backbone of the engine, render nodes base and cooperation.
 * A good deal of the active engine code is outside the scope of the
 * Proc-Layer, e.g. code located in backend services and plugins.
 */
namespace engine { }



/**
 * Media-Objects, edit operations and high-level session.
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
