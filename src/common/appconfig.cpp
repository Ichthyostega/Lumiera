/*
  Appconfig  -  for global initialization and configuration 
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


using util::isnil;

namespace cinelerra
  {
  
  /** This internal pointer to the single instance is deliberately
   *  not initialized (i.e. rely on implicit initialisation to 0),
   *  because when static init reaches this definition, the
   *  Appconfig::instance() probably already has been called
   *  by another compilation unit. This is ugliy, but preferable
   *  to beeing dependant on inclusion order of headers. */
  Appconfig* Appconfig::theApp_ ;

#ifndef CINELERRA_VERSION
#define CINELERRA_VERSION "3++devel"
#endif


  /** perform initialization on first access. 
   *  A call is placed in static initialization code
   *  included in cinelerra.h; thus it will happen
   *  probably very early.
   */
  Appconfig::Appconfig()
    : configParam_ (new Configmap)
    {
      ////////// 
      NOBUG_INIT;
      //////////
      
      INFO(config, "Basic application configuration triggered.");
      (*configParam_)["version"] = CINELERRA_VERSION;
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
        }
    }

  


} // namespace cinelerra
