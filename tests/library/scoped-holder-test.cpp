/*
  ScopedHolder(Test)  -  holding and owning noncopyable objects

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file scoped-holder-test.cpp
 ** unit test \ref ScopedHolder_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"
#include "lib/error.hpp"

#include "lib/scoped-holder.hpp"
#include "lib/test/testdummy.hpp"

#include <map>


namespace lib {
namespace test{
  
  using ::Test;
  using util::isnil;
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  using std::map;
  
  typedef ScopedHolder<Dummy>    HolderD;
  typedef ScopedPtrHolder<Dummy> PtrHolderD;
  
  
  /******************************************************************************//**
   *  @test ScopedHolder and ScopedPtrHolder are initially empty and copyable.
   *        After taking ownership, they prohibit copy operations, manage the
   *        lifecycle of the contained object and provide smart-ptr like access.
   *        A series of identical tests is conducted both with the ScopedPtrHolder
   *        (the contained objects are heap allocated but managed by the holder)
   *        and with the ScopedHolder (objects placed inline)
   */
  class ScopedHolder_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          
          cout << "checking ScopedHolder<Dummy>...\n";
          checkAllocation<HolderD>();
          checkErrorHandling<HolderD>();
          checkCopyProtocol<HolderD>();
          checkSTLContainer<HolderD>();
          
          cout << "checking ScopedPtrHolder<Dummy>...\n";
          checkAllocation<PtrHolderD>();
          checkErrorHandling<PtrHolderD>();
          checkCopyProtocol<PtrHolderD>();
          checkSTLContainer<PtrHolderD>();
        }
      
      void create_contained_object (HolderD&    holder) { holder.create();           }
      void create_contained_object (PtrHolderD& holder) { holder.reset(new Dummy()); }
      
      
      template<class HO>
      void
      checkAllocation()
        {
          CHECK (0 == Dummy::checksum());
          {
            HO holder;
            CHECK (!holder);
            CHECK (0 == Dummy::checksum());
            
            create_contained_object (holder);
            CHECK (holder);
            CHECK (false != bool(holder));
            CHECK (bool(holder) != false);
            
            CHECK (0 < Dummy::checksum());
            CHECK ( &(*holder));
            CHECK (holder->calc(2) == 2 + Dummy::checksum());
            
            Dummy *rawP = holder.get();
            CHECK (rawP);
            CHECK (holder);
            CHECK (rawP == &(*holder));
            CHECK (rawP->calc(-5) == holder->calc(-5));
            
            TRACE (test, "holder at %p", &holder);
            TRACE (test, "object at %p", holder.get() );
            TRACE (test, "size(object) = %zu", sizeof(*holder));
            TRACE (test, "size(holder) = %zu", sizeof(holder));
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      template<class HO>
      void
      checkErrorHandling()
        {
          CHECK (0 == Dummy::checksum());
          {
            HO holder;
            
            Dummy::activateCtorFailure();
            try
              {
                create_contained_object (holder);
                NOTREACHED ("expect failure in ctor");
              }
            catch (int val)
              {
                CHECK (0 != Dummy::checksum());
                Dummy::checksum() -= val;
                CHECK (0 == Dummy::checksum());
              }
            CHECK (!holder);  /* because the exception happens in ctor
                                 object doesn't count as "created" */
            Dummy::activateCtorFailure(false);
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      template<class HO>
      void
      checkCopyProtocol()
        {
          CHECK (0 == Dummy::checksum());
          {
            HO holder;
            HO holder2 (holder);
            holder2 = holder;
            // copy and assignment of empty holders is tolerated
            
            // but after enclosing an object it will be copy protected...
            CHECK (!holder);
            create_contained_object (holder);
            CHECK (holder);
            long currSum = Dummy::checksum();
            void* adr = holder.get();
            
            VERIFY_ERROR(LOGIC, holder2 = holder );
            CHECK (holder);
            CHECK (!holder2);
            CHECK (holder.get()==adr);
            CHECK (Dummy::checksum()==currSum);
            
            VERIFY_ERROR(LOGIC, holder = holder2 );
            CHECK (holder);
            CHECK (!holder2);
            CHECK (holder.get()==adr);
            CHECK (Dummy::checksum()==currSum);
            
            create_contained_object (holder2);
            CHECK (holder2);
            CHECK (Dummy::checksum() != currSum);
            currSum = Dummy::checksum();
            
            VERIFY_ERROR(LOGIC, holder = holder2 );
            CHECK (holder);
            CHECK (holder2);
            CHECK (holder.get()==adr);
            CHECK (Dummy::checksum()==currSum);
            
            VERIFY_ERROR(LOGIC, HO holder3 (holder2) );
            CHECK (holder);
            CHECK (holder2);
            CHECK (Dummy::checksum()==currSum);
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      /** @test collection of noncopyable objects
       *        maintained within a STL map
       */
      template<class HO>
      void
      checkSTLContainer()
        {
          typedef std::map<int,HO> MapHO;
          
          CHECK (0 == Dummy::checksum());
          {
            MapHO maph;
            CHECK (isnil (maph));
            
            for (uint i=0; i<100; ++i)
              {
                HO & contained = maph[i];
                CHECK (!contained);
              }                      // 100 holder objects created by sideeffect
                                    // ..... without creating any contained object!
            CHECK (0 == Dummy::checksum());
            CHECK (!isnil (maph));
            CHECK (100==maph.size());
            
            for (uint i=0; i<100; ++i)
              {
                create_contained_object (maph[i]);
                CHECK (maph[i]);
                CHECK (0 < maph[i]->calc(12));
              }
            CHECK (100==maph.size());
            CHECK (0 != Dummy::checksum());
            
            
            long value55 = maph[55]->calc(0);
            long currSum = Dummy::checksum();
            
            CHECK (1 == maph.erase(55));
            CHECK (Dummy::checksum() == currSum - value55); // proves object#55's dtor has been invoked
            CHECK (maph.size() == 99);
            
            maph[55];                              // create new empty holder by sideeffect...
            CHECK (&maph[55]);
            CHECK (!maph[55]);
            CHECK (maph.size() == 100);
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
    };
  
  LAUNCHER (ScopedHolder_test, "unit common");
  
  
}} // namespace lib::test
