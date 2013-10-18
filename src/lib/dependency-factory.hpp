/*
  DEPENDENCY-FACTORY.hpp  -  managing the lifecycle of singletons and dependencies

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/




#ifndef LIB_DEPENDENCY_FACTORY_H
#define LIB_DEPENDENCY_FACTORY_H



#include "lib/nobug-init.hpp"
//#include "lib/sync-classlock.hpp"

namespace lib {
  
  /** 
   * Factory to generate and manage service objects classified by type. 
   */
  class DependencyFactory
    {
    public:
      void*
      buildInstance()
        {
          UNIMPLEMENTED("invoke the ctor function");
        }
      
      void
      deconfigure (void* existingInstance)
        {
          UNIMPLEMENTED("deregister and destroy a managed product");
        }
      
      template<class TAR>
      void
      takeOwnership (TAR* newInstance)
        {
          UNIMPLEMENTED("enrol existing instance and prepare deleter function");
        }
      
      template<class TAR>
      void
      restore (TAR* volatile & activeInstance)
        {
          UNIMPLEMENTED("disable and destroy temporary shadowing instance and restore the dormant original instance");
        }
      
      template<class TAR>
      void
      shaddow (TAR* volatile & activeInstance)
        {
          UNIMPLEMENTED("set up a temporary replacement, allowing to restore the original later");
        }
      
      
      typedef void* (*InstanceConstructor)(void);
      
      void
      installConstructorFunction (InstanceConstructor ctor)
        {
          UNIMPLEMENTED("set up constructor function, unless already configured");
        }
      
      template<class TAR>
      static void*
      createSingletonInstance()
        {
          UNIMPLEMENTED("trampoline function to create a singleton");
        }
      
      
      template<class TAR>
      friend InstanceConstructor
      buildSingleton()
      {
        return & createSingletonInstance<TAR>;
      }
      
    };
  
  
} // namespace lib
#endif
