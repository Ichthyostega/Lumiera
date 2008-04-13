/*
  APPCONFIG.hpp  -  for global initialization and configuration 
 
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

/** @file appconfig.hpp
 ** Registering and managing some application-global services.
 ** Besides \link Appconfig::get querying \endlink for some 
 ** "Application property" constants, there is a mechanism for
 ** registering and firing off application lifecycle event hooks.
 ** The implementation of some subsystem can define a static instance
 ** variable of class LifecycleHook, which will place the provided
 ** callback function into a central registry accessable through
 ** the Appconfig singleton instance.
 **
 ** @see lumiera.cpp
 ** @see nobugcfg.cpp
 ** @see sessmanagerimpl.cpp
 */


#ifndef LUMIERA_APPCONFIG_H
#define LUMIERA_APPCONFIG_H

#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include "common/lifecycleregistry.hpp"

#include "nobugcfg.h"



namespace lumiera
  {
  using std::string;
  using boost::scoped_ptr;
  using boost::noncopyable;


  /**
   * Singleton to hold inevitable global flags and constants 
   * and for performing early (static) global initialization tasks.
   * Appconfig services are available already from static 
   * initialsation code.
   * @warning don't use Appconfig in destuctors.
   */
  class Appconfig
    : private noncopyable
    {
    private:
      /** perform initialization on first access.
       *  @see #instance()              */
      Appconfig ();
      
      ~Appconfig ()  throw()   {};  ///< deletion prohibited
      friend void boost::checked_delete<Appconfig>(Appconfig*);

      
    public:
      /** get the (single) Appconfig instance. 
       *  Implemented as Meyers singleton.
       *  @warning don't use it in destruction code!
       */
      static Appconfig& instance()
      {
        static scoped_ptr<Appconfig> theApp_ (0);
        if (!theApp_) theApp_.reset (new Appconfig ());
        return *theApp_;
      }
      
      
      /** access the configuation value for a given key.
       *  @return empty string for unknown keys, config value else
       */
      static const string & get (const string& key);  // never throws
      
      /** fire off all lifecycle callbacks
       *  registered under the given label */
      static void lifecycle (Symbol eventLabel);
      
      // note: if necessary, we can add support 
      // for querying the current lifecycle phase...
      
    private:
      typedef std::map<string,string> Configmap; 
      typedef scoped_ptr<Configmap> PConfig;
      typedef scoped_ptr<LifecycleRegistry> PLife;
      
      PConfig configParam_;
      PLife lifecycleHooks_;
      
      friend class LifecycleHook;
      
    };
    
  
  Symbol ON_BASIC_INIT      ("ON_BASIC_INIT");         ///< automatic static init. treated specially
  Symbol ON_GLOBAL_INIT     ("ON_GLOBAL_INIT");        ///< to be triggered in main()             @note no magic!
  Symbol ON_GLOBAL_SHUTDOWN ("ON_GLOBAL_SHUTDOWN");    ///< to be triggered at the end of main()  @note no magic!
  
  // client code is free to register and use additional lifecycle events
  
  
  /**
   *  define and register a callback for some lifecycle event.
   *  The purpose of this class is to be defined as a static variable
   *  in the implementation of some subsystem (i.e. in the cpp file),
   *  providing the ctor with the pointer to a callback function.
   *  Thus the callback gets enrolled when the corresponding object
   *  file is loaded. The event ON_BASIC_INIT is handled specifically,
   *  firing off the referred callback function as soon as possible.
   *  All other lables are just arbitrary (string) constants and it
   *  is necessary that "someone" cares to fire off the lifcycle events
   *  at the right place. For example, lumiera-main (and the test runner)
   *  calls \c Appconfig::instance().execute(ON_GLOBAL_INIT) (and..SHUTDOWN) 
   */
  class LifecycleHook
    : private noncopyable
    {
    public:
      LifecycleHook (Symbol eventLabel, LifecycleRegistry::Hook callbackFun);
      
      LifecycleHook& add (Symbol eventLabel, LifecycleRegistry::Hook callbackFun); ///< for chained calls (add multiple callbacks)
    };



} // namespace lumiera


extern "C" {  //TODO provide a separate header if some C code happens to need this...

  void lumiera_LifecycleHook_add (const char* eventLabel, void callbackFun(void));
  void lumiera_Lifecycle_execute (const char* eventLabel);
  const char* lumiera_Appconfig_get (const char* key);
  
}


#endif
