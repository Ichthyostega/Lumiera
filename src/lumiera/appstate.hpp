/*
  APPSTATE.hpp  -  application initialisation and behaviour 
 
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

/** @file appstate.hpp
 ** Registering and managing some application-global services.
 ** Besides encapsulating the logic for starting up the fundamental parts
 ** of the application, there is a mechanism for registering and firing off
 ** application lifecycle event callbacks. 
 **
 ** @see LifecycleHook
 ** @see main.cpp
 ** @see nobugcfg.h
 */


#ifndef LUMIERA_APPSTATE_H
#define LUMIERA_APPSTATE_H

#include "include/symbol.hpp"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>
#include <map>



namespace lumiera {
  
  using std::string;
  using boost::scoped_ptr;
  using boost::noncopyable;
  
  class LifecycleRegistry;
  
  
  /**
   * Singleton to hold global flags directing the overall application behaviour,
   * for triggering lifecycle events and performing early initialisation tasks.
   * AppState services are available already from static initialisation code.
   * @warning don't use AppState in destructors.
   */
  class AppState
    : private noncopyable
    {
    private:
      AppState ();
      
      ~AppState ();                 ///< deletion prohibited
      friend void boost::checked_delete<AppState>(AppState*);
      
      
    public:
      /** get the (single) AppState instance. 
       *  @warning don't use it after the end of main()! */
      static AppState& instance();
      
      
      /** fire off all lifecycle callbacks
       *  registered under the given label  */
      static void lifecycle (Symbol eventLabel);
      
      
      
    private:
      typedef scoped_ptr<LifecycleRegistry> PLife;
      
      PLife lifecycleHooks_;
      
      friend class LifecycleHook;
      
    };
  
  
} // namespace lumiera

#endif
