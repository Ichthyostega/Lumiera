/*
  ScopedHolderTransfer(Test)  -  managing noncopyable objects within a growing vector
 
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



#include "common/test/run.hpp"
#include "common/util.hpp"

#include "lib/scopedholder.hpp"
#include "lib/scopedholdertransfer.hpp"
#include "testdummy.hpp"

#include <iostream>
#include <vector>


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
    class ScopedHolderTransfer_test : public Test
      {
        
        virtual void 
        run (Arg arg)
          {
            
            cout << "checking ScopedHolder<Dummy>...\n";
            buildVector<HolderD>();
            growVector<HolderD>();
            checkErrorHandling<HolderD>();
            
            cout << "checking ScopedPtrHolder<Dummy>...\n";
            buildVector<PtrHolderD>();
            growVector<PtrHolderD>();
            checkErrorHandling<PtrHolderD>();
          }
        
        void create_contained_object (HolderD&    holder) { holder.create();           }
        void create_contained_object (PtrHolderD& holder) { holder.reset(new Dummy()); }
        
        
        template<class HO>
        void
        buildVector()
          {
            ASSERT (0==checksum);
            {
              UNIMPLEMENTED ("create constant sized vector holding noncopyables");
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
        growVector()
          {
            ASSERT (0==checksum);
            {
              UNIMPLEMENTED ("check growing a vector holding noncopyables");
            }
            ASSERT (0==checksum);
          }
        
        
        template<class HO>
        void
        checkErrorHandling()
          {
            UNIMPLEMENTED ("throw an error while growing");
            ASSERT (0==checksum);
            {
              HO holder;
              
              magic = true;
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
              magic = false;
            }
            ASSERT (0==checksum);
          }
        
      };
    
    LAUNCHER (ScopedHolderTransfer_test, "unit common");
    
    
  }// namespace test

} // namespace lib

