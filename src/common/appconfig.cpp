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


NOBUG_CPP_DEFINE_FLAG(config);


namespace cinelerra
  {
  
  /** holds the single instance and triggers initialization */
  auto_ptr<Appconfig> Appconfig::theApp_ (0);

#ifndef VERSION
#define VERSION 3++devel
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
      
      (*configParam_)["version"] = "VERSION";
    }
  
  
          ////////////////////////////TODO: ein richtiges utility draus machen....
          bool isnil(string val)
            {
              return 0 == val.length();
            };
          ////////////////////////////TODO  
            
            
            
  /** access the configuation value for a given key.
   *  @return empty string for unknown keys, else the corresponding configuration value
   */
  string
  Appconfig::get (const string & key)  throw()
    {
          
      try
        {
          string& val = (*instance().configParam_)[key];
          WARN_IF( isnil(val), config, "undefined config parameter \"%s\" requested.", key.c_str());
          
        }
      catch (...)
        {
          ERROR(config, "error while accessing configuration parameter \"%s\".", key.c_str());
        }
    }

  


} // namespace cinelerra
