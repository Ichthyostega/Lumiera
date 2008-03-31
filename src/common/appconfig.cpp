/*
  Appconfig  -  for global initialization and configuration 
 
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
 
* *****************************************************/


#include "common/appconfig.hpp"
#include "common/error.hpp"
#include "common/util.hpp"

#define NOBUG_INIT_DEFS_
#include "nobugcfg.h"
#undef NOBUG_INIT_DEFS_

#include <exception>


using util::isnil;

namespace lumiera
  {
  

#ifndef LUMIERA_VERSION
#define LUMIERA_VERSION 3++devel
#endif


  /** perform initialization on first access. 
   *  A call is placed in static initialization code
   *  included in lumiera.h; thus it will happen
   *  probably very early.
   */
  Appconfig::Appconfig()
    : configParam_ (new Configmap)
  {
    ////////// 
    NOBUG_INIT;
    //////////
    
    INFO(config, "Basic application configuration triggered.");
    
    // install our own handler for undeclared exceptions
    std::set_unexpected (lumiera::error::lumiera_unexpectedException);
    
    (*configParam_)["version"] = STRINGIFY (LUMIERA_VERSION);
  }
  
  
  
  
  
  /** access the configuation value for a given key.
   *  @return empty string for unknown keys, else the corresponding configuration value
   */
  const string &
  Appconfig::get (const string & key)  throw()
  {
    try
      {
        const string& val = (*instance().configParam_)[key];
        WARN_IF( isnil(val), config, "undefined config parameter \"%s\" requested.", key.c_str());
        return val;
      }
    catch (...)
      {
        ERROR(config, "error while accessing configuration parameter \"%s\".", key.c_str());
        throw lumiera::error::Fatal ();
  }   }

  


} // namespace lumiera
