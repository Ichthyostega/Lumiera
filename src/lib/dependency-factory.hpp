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


/** @file dependency-factory.hpp
 ** Implementation of a singleton factory used to bring up services as dependency.
 ** @internal this implementation header belongs to our framework to deal with
 **           [service dependencies](\ref depend.hpp) and should not be used directly.
 ** @deprecated 3/18 rework of the singleton / dependency factory is underway
 */




#ifndef LIB_DEPENDENCY_FACTORY_H
#define LIB_DEPENDENCY_FACTORY_H



#include "lib/nobug-init.hpp"
#include "lib/error.hpp"


namespace lib {
  
  namespace error = lumiera::error;
  
  
  /**
   * @internal Factory to generate and manage service objects classified by type.
   * An instance of this factory is placed <i>once for each type</i> for use by
   * the lib::Depend<TY> front-end for dependency management. While the latter
   * provides the singleton-style initialisation patter, the DependencyFacotry
   * maintains a customisable factory function for instance creation. Moreover,
   * the embedded helper template DependencyFactory::InstanceHolder actually
   * creates and manages the singleton instances in default configuration;
   * it is placed into a function-scope static variable; consequently
   * the singleton instances are placed into static memory by default.
   * @deprecated 3/18 rework of the singleton / dependency factory is underway
   */
  class DependencyFactory
    {
    public:
      typedef void* (*InstanceConstructor)(void);
      
      /** ensure initialisation by installing a default constructor function,
       *  but don't change an explicitly installed different constructor function.
       * @remark deliberately this DependencyFactory has no constructor to
       *         initialise the object field \c ctorFunction_  to zero.
       *         The reason is, in the intended usage scenario, the
       *         DependencyFactory lives within a static variable,
       *         which might be constructed in no defined order
       *         in relation to the Depend<TY> instance.
       */
      void
      ensureInitialisation (InstanceConstructor defaultCtor)
        {
          if (!ctorFunction_)
            this->ctorFunction_ = defaultCtor;
          ENSURE (ctorFunction_);
        }
      
      
      /** explicitly set up constructor function, unless already configured
       *  In the default configuration, the template \c Depend<TY> installs a
       *  builder function to create a singleton instance in static memory.
       *  But specific instances might install e.g. a factory to create a
       *  implementation defined subclass; this might also be the place
       *  to hook in some kind of centralised service manager in future.
       * @param ctor a function to be invoked to create a new service instance
       * @throw error::Fatal when attempting to change an existing configuration.
       */
      void
      installConstructorFunction (InstanceConstructor ctor)
        {
          if (ctorFunction_ && ctor != ctorFunction_)
            throw error::Fatal ("DependencyFactory: attempt to change the instance builder function "
                                "after-the-fact. Before this call, a different function was installed "
                                "and possibly also used already. Hint: visit all code locations, which "
                                "actually create an instance of the Depend<TY> template."
                               ,error::LUMIERA_ERROR_LIFECYCLE);
          this->ctorFunction_ = ctor;
        }
      
      
      
      /** invoke the installed ctor function */
      void*
      buildInstance()
        {
          if (!ctorFunction_)
            throw error::Fatal ("lib::Depend: attempt to retrieve a service object prior to initialisation "
                                "of the DependencyFactory. Typically, this happens due to a misconfiguration "
                                "regarding static initialisation order. When lib::Depend<TY> is placed into "
                                "a class static variable, then the definition and initialisation of that "
                                "variable must happen prior to the call which caused this exception."
                               ,error::LUMIERA_ERROR_LIFECYCLE);
          return ctorFunction_();
        }
      
      
      
      
      
    private:
      /** pointer to the concrete function
       *  used for building new service instances */
      InstanceConstructor ctorFunction_;
      
      
      
      /** function to build service instances.
       *  A service class with private ctor can declare DependencyFactory as friend,
       *  to indicate this is the expected way to create instances */
      template<typename TAR>
      static TAR*
      create_in_buffer (void* buffer)
        {
          return new(buffer) TAR;
        }
      
      
      /**
       * @internal Helper to manage a service instance within an embedded buffer.
       * This helper and thus the service instance will be allocated into static memory.
       */
      template<typename TAR>
      class InstanceHolder
        : util::NonCopyable
        {
          /** storage for the service instance */
          char buff_[sizeof(TAR)];
          uint lifecycle_;
          
          
        public:
          InstanceHolder()
            : lifecycle_(0)
            { }
          
         ~InstanceHolder()
            {
              lifecycle_ |= 4;
              if (1 & lifecycle_)
                {
                  reinterpret_cast<TAR&> (buff_). ~TAR();
                  --lifecycle_;
            }   }
          
          
          TAR*
          buildInstance ()
            {
              if (0 < lifecycle_)
                throw error::Fatal("Attempt to double-create a singleton service. "
                                   "Either the application logic, or the compiler "
                                   "or runtime system is seriously broken"
                                  ,error::LUMIERA_ERROR_LIFECYCLE);
              
              // place new instance into embedded buffer
              TAR* newInstance = create_in_buffer<TAR>(buff_);
              ++lifecycle_;
              return newInstance;
            }
        };
      
      
      
      template<class TAR>
      static void*
      createSingletonInstance()
        {
          static InstanceHolder<TAR> storage;   // note: the singleton(s) live here
          return storage.buildInstance();
        }
      
      
      template<class TAR>
      friend InstanceConstructor buildSingleton();
      
    };
  
  
  
  /**
   * DSL-style marker function for client code
   * to configure the usage of a specific subclass.
   * Typically this function is used right within the
   * Constructor call for lib::Depend; this allows to
   * confine the actual service implementation class
   * to a single compilation unit, without the need
   * for clients of the respective service to know
   * the actual concrete implementation class
   */
  template<class TAR>
  inline DependencyFactory::InstanceConstructor
  buildSingleton()
  {
    return & DependencyFactory::createSingletonInstance<TAR>;
  }
  
  
} // namespace lib
#endif
