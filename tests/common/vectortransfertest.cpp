/*
  VectorTransfer(Test)  -  intercept object copying when a STL vector grows
 
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

#include "lib/scopedholdertransfer.hpp"
#include "testdummy.hpp"

#include <iostream>
#include <vector>


namespace lib {
  namespace test {
    
    using ::Test;
    using std::vector;
    using std::cout;
    
    namespace { // extending the Dummy for our special purpose....
    
      class TransDummy
        : public Dummy
        {
        public:
          TransDummy()
            {
              TRACE  (test, "CTOR      TransDummy() --> this=%x", this);
              setVal(0); // we use val_==0 to mark the "empty" state 
            }
          
          ~TransDummy()
            {
              TRACE  (test, "DTOR     ~TransDummy()  this=%x", this);
            }
          
          /* to make Dummy usable within vector, we need to provide
           * \em special copy operations, an operator bool() and
           * a transfer_control friend function to be used by
           * our special allocator.
           */
          
          TransDummy (const TransDummy& o)
            {
              TRACE  (test, "COPY-ctor TransDummy( ref=%x ) --> this=%x", &o,this);
              ASSERT (!o, "protocol violation: real copy operations inhibited");
            }
          
          TransDummy&
          operator= (TransDummy const& ref)
            {
              TRACE  (test, "COPY      target=%x   <-- source=%x", this,&ref);
              ASSERT (!(*this));
              ASSERT (!ref, "protocol violation: real copy operations inhibited");
              return *this;
            }
          
          void
          setup (int x=0)
            {
              setVal (x? x : (rand() % 10000));
              TRACE  (test, "CREATE    val=%d ---> this=%x", getVal(),this);
            }
          
          
          // define implicit conversion to "bool" the naive way...
          operator bool()  const
            {
              return 0!=getVal();
            }
          
          
          friend void transfer_control (TransDummy& from, TransDummy& to);
          
        };
      
      
      void
      transfer_control (TransDummy& from, TransDummy& to)
      {
        TRACE  (test, "TRANSFER  target=%x   <-- source=%x", &to,&from);
        ASSERT (!to, "protocol violation: target already manages another object");
        to.setVal (from.getVal());
        from.setVal(0);
      }
      
      typedef Allocator_TransferNoncopyable<TransDummy> Allo; 
      typedef vector<TransDummy, Allo> TransDummyVector;
    }
    
    
    
    
    /**********************************************************************************
     *  @test ScopedHolder and ScopedPtrHolder are initially empty and copyable. 
     *        After taking ownership, they prohibit copy operations, manage the
     *        lifecycle of the contained object and provide smart-ptr like access.
     *        A series of identical tests is conducted both with the ScopedPtrHolder
     *        (the contained objects are heap allocated but managed by the holder)
     *        and with the ScopedHolder (objects placed inline)
     */
    class VectorTransfer_test : public Test
      {
        
        virtual void 
        run (Arg arg)
          {
            cout << "\n..setup table space for 2 elements\n";
            TransDummyVector table;
            table.reserve(2);
            ASSERT (0==checksum);
            
            cout << "\n..install one element at index[0]\n";
            table.push_back(TransDummy());
            ASSERT (0==checksum);
            
            table[0].setup(); // switches into "managed" state
            ASSERT (0 < checksum);
            int theSum = checksum;
            
            cout << "\n..*** resize table to 5 elements\n";
            table.resize(5);
            ASSERT (theSum==checksum);
            
            cout << "\n..install another element\n";
            table[3].setup(375);
            ASSERT (theSum+375==checksum);
            
            cout << "\n..kill all elements....\n";
            table.clear();
            ASSERT (0==checksum); 
          }
        
      };
    
    LAUNCHER (VectorTransfer_test, "unit common");
    
    
  }// namespace test

} // namespace lib

