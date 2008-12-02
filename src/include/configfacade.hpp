/*
  CONFIGFACADE  -  C++ convenience wrapper and startup of the config system
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file configfacade.hpp
 ** The lumiera::Config wrapper class addresses two issues.
 ** First, it registers startup and shutdown hooks to bring up the config system
 ** as early as possible. Later, on application main initialisation, the global
 ** config interface is opened and wrapped for convenient access from C++ code.
 **
 ** @see config.h
 ** @see lumiera::AppState
 ** @see main.cpp
 */


#ifndef INTERFACE_CONFIGFACADE_H
#define INTERFACE_CONFIGFACADE_H


#include "common/singleton.hpp"

#include <string>


namespace lumiera {
  
  using std::string;
  
  
  /*********************************************************************
   * C++ wrapper for convenient access to the Lumiera config system.
   */
  struct Config
    {
      
      static const string get (string const& key);
      
      
      static lumiera::Singleton<Config> instance;
      
    private:
      Config();
      ~Config();
      
      friend class lumiera::singleton::StaticCreate<Config>;
      
      //////////////////TODO: define startup/shutdown and loading of the config interface
      
    };
  
  
  
} // namespace lumiera
#endif
