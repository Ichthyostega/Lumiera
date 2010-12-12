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



#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "lib/scoped-holder.hpp"
#include "lib/scoped-holder-transfer.hpp"
#include "testdummy.hpp"

#include <iostream>
#include <vector>


namespace lib {
  namespace test {
    
    using ::Test;
    using util::isnil;
    
    using std::vector;
    using std::cout;
    
    namespace { // extending the Dummy for our special purpose....
      
      bool throw_in_transfer = false;
    
      class FixedDummy
        : public Dummy
        {
        public:
          FixedDummy()
            {
              TRACE  (test, "CTOR      FixedDummy() --> this=%p val=%d", this, getVal());
            }
          
          ~FixedDummy()
            {
              TRACE  (test, "DTOR     ~FixedDummy()  this=%p val=%d", this, getVal());
            }
          
          friend void
          transfer_control (FixedDummy& from, FixedDummy& to)
          {
            TRACE  (test, "TRANSFER  target=%p   <-- source=%p  (%d,%d)", &to,&from, to.getVal(),from.getVal());
            
            if (throw_in_transfer)
              throw to.getVal();
            
            swap (from,to);
            from.setVal(0); // remove the old Dummy from accounting (checksum) 
          }
          
        };
      
      
      typedef ScopedHolder<FixedDummy>    HolderD;
      typedef ScopedPtrHolder<FixedDummy> PtrHolderD;
      
      template<class HOL>
      struct Table
        {
          typedef Allocator_TransferNoncopyable<HOL> Allo; 
          typedef typename std::vector<HOL,Allo> Type;
          
        };
    }
      
      
    
    
    /**********************************************************************************
     *  @test growing a vector containing noncopyable objects wrapped into ScopedHolder
     *        instances. This requires the use of a custom allocator, invoking a 
     *        \c transfer_control() function to be provided for the concrete
     *        noncopyable class type, being invoked when the vector
     *        needs to reallocate.
     */
    class ScopedHolderTransfer_test : public Test
      {
        
        virtual void 
        run (Arg)
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
        
        void create_contained_object (HolderD&    holder) { holder.create();                }
        void create_contained_object (PtrHolderD& holder) { holder.reset(new FixedDummy()); }
        
        
        template<class HO>
        void
        buildVector()
          {
            ASSERT (0==checksum);
            {
              typedef typename Table<HO>::Type Vect;
              
              Vect table(50);
              ASSERT (0==checksum);
              
              for (uint i=0; i<10; ++i)
                create_contained_object (table[i]);
              
              ASSERT (0 < checksum);
              ASSERT ( table[9]);
              ASSERT (!table[10]);
              
              Dummy *rawP = table[5].get();
              ASSERT (rawP);
              ASSERT (table[5]);
              ASSERT (rawP == &(*table[5]));
              ASSERT (rawP->add(-555) == table[5]->add(-555));
            }
            ASSERT (0==checksum);
          }
        
        
        template<class HO>
        void
        growVector()
          {
            ASSERT (0==checksum);
            {
              typedef typename Table<HO>::Type Vect;
              
              Vect table;
              table.reserve(2);
              ASSERT (0==checksum);
              
              cout << ".\n..install one element at index[0]\n";
              table.push_back(HO());
              ASSERT (0==checksum);
              
              create_contained_object (table[0]); // switches into "managed" state
              ASSERT (0 < checksum);
              int theSum = checksum;
              
              cout << ".\n..*** resize table to 16 elements\n";
              for (uint i=0; i<15; ++i)
                table.push_back(HO());
              
              ASSERT (theSum==checksum);
            }
            ASSERT (0==checksum);
          }
        
        
        template<class HO>
        void
        checkErrorHandling()
          {
            ASSERT (0==checksum);
            {
              typedef typename Table<HO>::Type Vect;
              
              Vect table(5);
              table.reserve(5);
              ASSERT (0==checksum);
              
              create_contained_object (table[2]);
              create_contained_object (table[4]);
              ASSERT (0 < checksum);
              int theSum = checksum;
              
              cout << ".\n.throw some exceptions...\n";
              throw_in_ctor = true;
              try
                {
                  create_contained_object (table[3]);
                  NOTREACHED ();
                }
              catch (int val)
                {
                  ASSERT (theSum < checksum);
                  checksum -= val;
                  ASSERT (theSum==checksum);
                }
              ASSERT ( table[2]); 
              ASSERT (!table[3]); // not created because of exception 
              ASSERT ( table[4]); 
              
              throw_in_ctor = false;
              throw_in_transfer=true;  // can do this only when using ScopedHolder
              try
                {
                  table.resize(10);
                }
              catch (int val)
                {
                  ASSERT ( table.size() < 10);
                }
              ASSERT (theSum == checksum);
              throw_in_transfer=false;
            }
            ASSERT (0==checksum);
          }
        
      };
    
    LAUNCHER (ScopedHolderTransfer_test, "unit common");
    
    
  }// namespace test

} // namespace lib

