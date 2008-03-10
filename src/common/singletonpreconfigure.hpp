/*
  SINGLETONPRECONFIGURE  -  declare the configuration of some Singleton types in advance
 
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

/** @file singletonpreconfigure.hpp
 ** Preconfiguration of some Singleton types, done by template specialisation.
 ** Typically the client code just includes singleton.h and uses the Singleton
 ** type. But in some cases, we want to configure specific (dependency injection)
 ** behaviour at a central location. At some point, we may well have a full blown
 ** Dependency Manager, but for the moment using just some specialized Singleton
 ** type for some instances seems sufficient.
 **
 ** One reason why one wants special Singleton behaviour is testing: Without
 ** altering the executable, for running some tests we need to inject a Test Mock
 ** in place of some service object, so we can verify the behaviour of the code
 ** <i>using</i> this service. For this, we mix cinelerra::test::MockInjector
 ** into the actual Singleton type.
 **
 ** @note we declare the specialisations into the target namespace
 **
 ** @see SingletonFactory
 ** @see singletontestmocktest.hpp
 */


#ifndef CINELERRA_SINGLETONPRECONFIGURE_H
#define CINELERRA_SINGLETONPRECONFIGURE_H

#include "common/test/mockinjector.hpp"


namespace cinelerra
  {
  /**
   * Default Singleton configuration
   * @note all Policy template parameters taking default values
   */
  template <class SI>
  class Singleton 
    : public SingletonFactory<SI>
    { }
    ;
    
    
  /* ********************************************************************** */
  /* Forward declarations of all Classes we want to specialize the template */
  /* ********************************************************************** */
    
  namespace test
    {
    class TestSingletonO;
    using cinelerra::Singleton;
    
  } // namespace test
}  // namespace cinelerra

namespace backend_interface
  {
  class MediaAccessFacade;
  using cinelerra::Singleton;
    
} // namespace backend_interface





  /* ************************** */
  /* Specialisation Definitions */
  /* ************************** */

namespace cinelerra
  {
  
  using test::MockInjector;
  
    
  template<>
  class Singleton<test::TestSingletonO>
    : public MockInjector<test::TestSingletonO>
    { };

    
  template<>
  class Singleton<backend_interface::MediaAccessFacade>
    : public MockInjector<backend_interface::MediaAccessFacade>
    { };

} // namespace cinelerra




#endif
