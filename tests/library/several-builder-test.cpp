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
#include "lib/test/test-coll.hpp"
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
using std::array;
using std::rand;

using lib::explore;
using util::isLimited;
using util::isnil;
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
          : Dummy(seed)
          {
            ext_.fill(seed);
            setVal ((i+1)*seed);
          }
       ~Num()
          {
            setVal (getVal() - explore(ext_).resultSum());
          }
          
        long
        calc (int ii)  override
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
          check_ElementStorage();
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
      
      
      /** @test various ways to build an populate the container
       *      - with a defined interface type \a I, instances of arbitrary subclasses
       *        can be added, assuming there is sufficient pre-allocated buffer space;
       *        all these subclass instances are accessed through the common interface.
       *      - yet the added elements can also be totally unrelated, in which case an
       *        *unchecked wild cast* will happen on access; while certainly dangerous,
       *        this behaviour allows for special low-level data layout tricks.
       *      - the results from an iterator can be used to populate by copy
       * @todo WIP 6/24 🔁 define ⟶ ✔ implement
       */
      void
      check_Builder()
        {
          { // Scenario-1 : Baseclass and arbitrary subclass elements
            SeveralBuilder<Dummy> builder;
            CHECK (isnil (builder));
            
            builder.emplace<Num<3>>()
                   .emplace<Num<2>>(1);
            CHECK (2 == builder.size());
            builder.fillElm(2);
            CHECK (4 == builder.size());
            builder.fillElm(3, 5);
            CHECK (7 == builder.size());
            
            Several<Dummy> elms = builder.build();
            CHECK (    isnil(builder));
            CHECK (not isnil(elms));
            CHECK (7 == elms.size());
            CHECK (elms[0].getVal() == (3+1)*3);                         // indeed a Num<3> with default-seed ≡ 3
            CHECK (elms[0].calc(1)  == 3 + 1 + (3+3+3));                 // indeed called the overridden calc() operation
            CHECK (elms[1].getVal() == (2+1)*1);                         // indeed a Num<2> with seed ≡ 1
            CHECK (elms[1].calc(1)  == 2 + 1 + (1+1));                   // indeed the overridden calc() picking from the Array(1,1)
            CHECK (isLimited (1, elms[2].getVal(), 100'000'000));        // indeed a Dummy with default random seed
            CHECK (isLimited (1, elms[3].getVal(), 100'000'000));        // and this one too, since we filled in two instances
            CHECK (elms[4].getVal() == 5);                               // followed by tree instances Dummy(5)
            CHECK (elms[5].getVal() == 5);
            CHECK (elms[6].getVal() == 5);
            CHECK (elms[6].calc(1)  == 5+1);                             // indeed invoking the base implementation of calc()
          }
          
          { // Scenario-2 : unrelated element types
            SeveralBuilder<uint32_t> builder;
            
            auto urgh = array<char,5>{"Urgh"};
            auto phi  = (1+sqrtf(5))/2;
            
            builder.append (urgh, phi, -1);                              // can emplace arbitrary data
            CHECK (3 == builder.size());
            
            Several<uint32_t> elms = builder.build();                    // WARNING: data accessed by wild cast to interface type
            CHECK (3 == elms.size());
            CHECK (elms[0] == * reinterpret_cast<const uint32_t*> ("Urgh"));
            CHECK (elms[1] == * reinterpret_cast<uint32_t*> (&phi));
            CHECK (elms[2] == uint32_t(-1));
          }
          
          { // Scenario-3 : copy values from iterator
            SeveralBuilder<int> builder;
            
            VecI seq = getTestSeq_int<VecI> (10);
            builder.appendAll (seq);
            CHECK (10 == builder.size());
            
            auto elms = builder.build();
            CHECK (10 == elms.size());
            CHECK (join (elms,"-") == "0-1-2-3-4-5-6-7-8-9"_expect);
          }
        }
      
      
      /** @test TODO proper handling of exceptions during population
       * @todo WIP 6/24 🔁 define ⟶ implement
       */
      void
      check_ErrorHandling()
        {
          { // Scenario-1 : Baseclass and arbitrary subclass elements
            SeveralBuilder<Dummy> builder;
            
            // The first element will _prime_ the container for a suitable usage pattern
            builder.emplace<Num<1>>();
            CHECK (1 == builder.size());
            
            // Notably the first element established the _spread_ between index positions,
            // which effectively limits the size of objects to be added. Moreover, since
            // the element type was detected to be non-trivial, we can not correct this
            // element spacing by shifting existing allocations (`memmove()` not possible)
            CHECK (sizeof(Num<1>) < sizeof(Num<5>));
            VERIFY_FAIL ("Unable to place element of type Num<5u> (size="
                        , builder.emplace<Num<5>>() );
            CHECK (1 == builder.size());
            
            // Furthermore, the first element was detected to be a subclass,
            // and the interface type `Dummy` has a virtual destructor;
            // all added elements must comply to this scheme, once established
            VERIFY_FAIL ("Unable to handle (trivial-)destructor for element type long, "
                         "since this container has been primed to use virtual-baseclass-destructors."
                        , builder.emplace<long>(55) );
            CHECK (1 == builder.size());
            
            // the initial allocation added some reserve buffer space (for 10 elements)
            // and we can fill that space with arbitrary subclass instances
            builder.fillElm (5);
            CHECK (6 == builder.size());
            
            // But the initial allocation can not be increased, since that would require
            // a re-allocation of a larger buffer, followed by copying the elements;
            // but since the established scheme allows for _arbitrary_ subclasses,
            // the builder does not know the exact type for safe element relocation.
            VERIFY_FAIL ("Unable to accommodate further element of type Dummy"
                        , builder.fillElm (20) );
            CHECK (10 == builder.size());
          }
        }
      
      
      /** @test TODO verify correct placement of instances within storage
       * @todo WIP 6/24 🔁 define ⟶ implement
       */
      void 
      check_ElementStorage()
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
