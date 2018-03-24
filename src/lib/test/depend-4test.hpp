/*
  DEPEND4TEST.hpp  -  inject test mock singletons and dependencies


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


/** @file depend-4test.hpp
 ** test support code to inject mock variants of dependencies
 */



#ifndef LIB_TEST_DEPEND_4TEST_H
#define LIB_TEST_DEPEND_4TEST_H


#include "lib/depend.hpp"
#include "lib/meta/duck-detector.hpp"

#include <memory>


namespace lib {
namespace test{
  
  namespace { ///< details: inject a mock automatically in place of a singleton
    
    using lib::meta::enable_if;
    using lib::meta::Yes_t;
    using lib::meta::No_t;
    
    /**
     * Metafunction: does the Type in question
     * give us a clue about what service interface to use?
     */
    template<class MOCK>
    class defines_ServiceInterface
      {
        META_DETECT_NESTED  (ServiceInterface);
        
      public:
        enum{ value = HasNested_ServiceInterface<MOCK>::value
            };
      };
    
    
    /**
     * Policy-Trait: determine the access point to install the mock.
     * @note either the mock service implementation needs to provide
     *       explicitly a typedef for the ServiceInterface, or we're
     *       just creating a separate new instance of the singleton service,
     *       while shadowing (but not destroying) the original instance.
     */
    template<class I, class YES =void>
    struct ServiceInterface
      {
        typedef I Type;
      };
    
    
    template<class MOCK>
    struct ServiceInterface<MOCK,   enable_if< defines_ServiceInterface<MOCK> >>
      {
        typedef typename MOCK::ServiceInterface Type;
      };
    
  }//(End) mock injection details
  
  
  
  
  
  /**
   * Scoped object for installing/deinstalling a mocked service automatically.
   * Placing a suitably specialised instance of this template into a local scope
   * will inject the corresponding mock installation and remove it when the
   * control flow leaves this scope.
   * @param TYPE the concrete mock implementation type to inject. It needs to
   *        be default constructible. If TYPE is a subclass of the service interface,
   *        it needs to expose a typedef \c ServiceInterface
   */
  template<class TYPE>
  struct Depend4Test
    : util::NonCopyable
    {
      typedef typename ServiceInterface<TYPE>::Type Interface;
      
      std::unique_ptr<TYPE> mock_;
      Interface* shadowedOriginal_;
      
      Depend4Test()
        : mock_{new TYPE}
        , shadowedOriginal_{Depend<Interface>::injectReplacement (mock_.get())}
        { }
      
     ~Depend4Test()
        {
          ENSURE (mock_);
          Depend<Interface>::injectReplacement (shadowedOriginal_);
        }
    };
  
  
}} // namespace lib::test
#endif
