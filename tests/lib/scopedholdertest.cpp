/*
  ScopedHolder(Test)  -  holding and owning noncopyable objects
 
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
 
* *****************************************************/



#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "lib/scopedholder.hpp"
#include "testdummy.hpp"

#include <boost/noncopyable.hpp>
#include <iostream>
#include <map>


namespace lib {
  namespace test {
    
    using ::Test;
    using util::isnil;
    
    using std::map;
    using std::cout;
      
    typedef ScopedHolder<Dummy>    HolderD;
    typedef ScopedPtrHolder<Dummy> PtrHolderD;
    
    
    /**********************************************************************************
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
            ASSERT (0==checksum);
            {
              HO holder;
              ASSERT (!holder);
              ASSERT (0==checksum);
              
              create_contained_object (holder);
              ASSERT (holder);
              ASSERT (false!=holder);
              ASSERT (holder!=false);
              
              ASSERT (0!=checksum);
              ASSERT ( &(*holder));
              ASSERT (holder->add(2) == checksum+2);
              
              Dummy *rawP = holder.get();
              ASSERT (rawP);
              ASSERT (holder);
              ASSERT (rawP == &(*holder));
              ASSERT (rawP->add(-5) == holder->add(-5));
              
              TRACE (test, "holder at %x", &holder);
              TRACE (test, "object at %x", holder.get() );
              TRACE (test, "size(object) = %d", sizeof(*holder));
              TRACE (test, "size(holder) = %d", sizeof(holder));
            }
            ASSERT (0==checksum);
          }
        
        
        template<class HO>
        void
        checkErrorHandling()
          {
            ASSERT (0==checksum);
            {
              HO holder;
              
              throw_in_ctor = true;
              try
                {
                  create_contained_object (holder);
                  NOTREACHED ;
                }
              catch (int val)
                {
                  ASSERT (0!=checksum);
                  checksum -= val;
                  ASSERT (0==checksum);
                }
              ASSERT (!holder); /* because the exception happens in ctor
                                   object doesn't count as "created" */
              throw_in_ctor = false;
            }
            ASSERT (0==checksum);
          }
        
        
        template<class HO>
        void
        checkCopyProtocol()
          {
            ASSERT (0==checksum);
            {
              HO holder;
              HO holder2 (holder);
              holder2 = holder;
              // copy and assignment of empty holders is tolerated
              
              // but after enclosing an object it will be copy protected...
              ASSERT (!holder);
              create_contained_object (holder);
              ASSERT (holder);
              long currSum = checksum;
              void* adr = holder.get();
              try
                {
                  holder2 = holder;
                  NOTREACHED ;
                }
              catch (lumiera::error::Logic&)
                {
                  ASSERT (holder);
                  ASSERT (!holder2);
                  ASSERT (holder.get()==adr);
                  ASSERT (checksum==currSum);
                }
              
              try
                {
                  holder = holder2;
                  NOTREACHED ;
                }
              catch (lumiera::error::Logic&)
                {
                  ASSERT (holder);
                  ASSERT (!holder2);
                  ASSERT (holder.get()==adr);
                  ASSERT (checksum==currSum);
                }
              
              create_contained_object (holder2);
              ASSERT (holder2);
              ASSERT (checksum != currSum);
              currSum = checksum;
              try
                {
                  holder = holder2;
                  NOTREACHED ;
                }
              catch (lumiera::error::Logic&)
                {
                  ASSERT (holder);
                  ASSERT (holder2);
                  ASSERT (holder.get()==adr);
                  ASSERT (checksum==currSum);
                }
              
              try
                {
                  HO holder3 (holder2);
                  NOTREACHED ;
                }
              catch (lumiera::error::Logic&)
                {
                  ASSERT (holder);
                  ASSERT (holder2);
                  ASSERT (checksum==currSum);
                }
            }
            ASSERT (0==checksum);
          }
        
        
        /** @test collection of noncopyable objects
         *        maintained within a STL map
         */
        template<class HO>
        void
        checkSTLContainer()
          {
            typedef std::map<int,HO> MapHO;
            
            ASSERT (0==checksum);
            {
              MapHO maph;
              ASSERT (isnil (maph));
              
              for (uint i=0; i<100; ++i)
                {
                  HO & contained = maph[i];
                  ASSERT (!contained);
                }                      // 100 holder objects created by sideeffect
                                      
              ASSERT (0==checksum);  // ..... without creating any contained object!
              ASSERT (!isnil (maph));
              ASSERT (100==maph.size());
              
              for (uint i=0; i<100; ++i)
                {
                  create_contained_object (maph[i]);
                  ASSERT (maph[i]);
                  ASSERT (0 < maph[i]->add(12));
                }
              ASSERT (100==maph.size());
              ASSERT (0!=checksum);
              
              
              long value55 = maph[55]->add(0); 
              long currSum = checksum;
              
              ASSERT (1 == maph.erase(55));
              ASSERT (checksum == currSum - value55); // proves object#55's dtor has been invoked
              ASSERT (maph.size() == 99);
              
              maph[55];                            // create new empty holder by sideeffect...
              ASSERT (&maph[55]);
              ASSERT (!maph[55]);
              ASSERT (maph.size() == 100);
            }
            ASSERT (0==checksum);
          }
        
        
      };
    
    LAUNCHER (ScopedHolder_test, "unit common");
    
    
  }// namespace test

} // namespace lib

