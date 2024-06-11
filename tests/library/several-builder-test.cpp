/*
  SeveralBuilder(Test)  -  building a limited fixed collection of elements

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/

/** @file several-builder-test.cpp
 ** unit test \ref SeveralBuilder_test
 */


#include "lib/test/run.hpp"
#include "lib/test/testdummy.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"////////////////TODO
#include "lib/iter-explorer.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include "lib/several-builder.hpp"

#include <vector>
#include <array>

using ::test::Test;
using std::vector;
using std::rand;

using lib::explore;
using util::join;


namespace lib {
namespace test{
  
  namespace { // diagnostic test types
    
    /**
     * Instance tracking sub-dummy
     * - implements the Dummy interface
     * - holds additional storage
     * - specific implementation of the virtual operation
     * - includes content of the additional storage into the
     *   checksum calculation, allowing to detect memory corruption
     */
    template<uint i>
    class Num
      : public test::Dummy
      {
        std::array<int,i> ext_;
        
      public:
        Num (uint seed=i)
          : Dummy{seed}
          {
            ext_.fill(seed);
            setVal ((i+1)*seed);
          }
       ~Num()
          {
            setVal (getVal() - explore(ext_).resultSum());
          }
          
        long
        acc (int ii)  override
          {
            return i+ii + explore(ext_).resultSum();
          }
      };
    
  } // (END) test types
  
  
  
  
  
  
  
  /***************************************************************//**
   * @test use lib::Several to establish small collections of elements,
   *       possibly with sub-classing and controlled allocation.
   *     - the container is populated through a separate builder
   *     - the number of elements is flexible during population
   *     - the actual container allows random-access via base interface
   * @see several-builder.hpp
   */
  class SeveralBuilder_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          check_Builder();
          check_ErrorHandling();
          check_ElementAccess();
          check_CustomAllocator();
        }
      
      
      
      /** @test demonstrate basic behaviour
       * @todo WIP 6/24 ✔ define ⟶ ✔ implement
       */
      void
      simpleUsage()
        {
          auto elms = makeSeveral({1,1,2,3,5,8,13}).build();
          CHECK (elms.size() == 7);
          CHECK (elms.back() == 13);
          CHECK (elms[3] == 3);
          CHECK (join (elms,"-") == "1-1-2-3-5-8-13"_expect);
        }
      
      
      /** @test TODO various ways to build an populate the container
       * @todo WIP 6/24 🔁 define ⟶ implement
       */
      void
      check_Builder()
        {
        }
      
      
      /** @test TODO proper handling of exceptions during population
       * @todo WIP 6/24 🔁 define ⟶ implement
       */
      void
      check_ErrorHandling()
        {
        }
      
      
      /** @test TODO verify access operations on the actual container
       * @todo WIP 6/24 🔁 define ⟶ implement
       */
      void 
      check_ElementAccess()
        {
        }
      
      
      /** @test TODO demonstrate integration with a custom allocator
       * @todo WIP 6/24 🔁 define ⟶ implement
       */
      void 
      check_CustomAllocator()
        {
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SeveralBuilder_test, "unit common");
  
  
  
}} // namespace lib::test
