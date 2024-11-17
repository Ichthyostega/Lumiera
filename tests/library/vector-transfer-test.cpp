/*
  VectorTransfer(Test)  -  intercept object copying when a STL vector grows

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file vector-transfer-test.cpp
 ** unit test \ref VectorTransfer_test
 */



#include "lib/test/run.hpp"

#include "lib/scoped-holder-transfer.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/format-cout.hpp"

#include <vector>


namespace lib {
namespace test {
  
  using ::Test;
  using std::vector;
  
  namespace { // extending the Dummy for our special purpose....
    
    class TransDummy
      : public Dummy
      {
      public:
        TransDummy()
          {
            TRACE  (test, "CTOR      TransDummy() --> this=%p", this);
            setVal(0); // we use val_==0 to mark the "empty" state
          }
        
       ~TransDummy()
          {
            TRACE  (test, "DTOR     ~TransDummy()  this=%p", this);
          }
        
        /* to make Dummy usable within vector, we need to provide
         * \em special copy operations, an operator bool() and
         * a transfer_control friend function to be used by
         * our special allocator.
         */
        
        TransDummy (const TransDummy& o)
          : Dummy()
          {
            TRACE  (test, "COPY-ctor TransDummy( ref=%p ) --> this=%p", &o,this);
            CHECK (!o, "protocol violation: real copy operations inhibited");
          }
        
        TransDummy&
        operator= (TransDummy const& ref)
          {
            TRACE  (test, "COPY      target=%p   <-- source=%p", this,&ref);
            CHECK (!(*this));
            CHECK (!ref, "protocol violation: real copy operations inhibited");
            return *this;
          }
        
        void
        setup (int x=0)
          {
            setVal (x? x : rani (10000));
            TRACE  (test, "CREATE    val=%d ---> this=%p", getVal(),this);
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
      TRACE  (test, "TRANSFER  target=%p   <-- source=%p", &to,&from);
      CHECK (!to, "protocol violation: target already manages another object");
      to.setVal (from.getVal());
      from.setVal(0);
    }
    
    typedef Allocator_TransferNoncopyable<TransDummy> Allo;
    typedef vector<TransDummy, Allo> TransDummyVector;
  }
  
  
  
  
  /******************************************************************************//**
   *  @test growing (re-allocating) a vector with noncopyable objects, with the
   *        help of a special Allocator and a custom \c transfer_control operation
   *        provided by the contained objects. The idea is to allow some special
   *        copy-operations for the purpose of re-allocations within the vector,
   *        without requiring the object to be really copyable.
   */
  class VectorTransfer_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          cout << "\n..setup table space for 2 elements\n";
          TransDummyVector table;
          table.reserve(2);
          CHECK (0 == Dummy::checksum());
          
          cout << "\n..install one element at index[0]\n";
          table.push_back(TransDummy());
          CHECK (0 == Dummy::checksum());
          
          table[0].setup(); // switches into "managed" state
          CHECK (0 < Dummy::checksum());
          int theSum = Dummy::checksum();
          
          cout << "\n..*** resize table to 5 elements\n";
          table.resize(5);
          CHECK (theSum==Dummy::checksum());
          
          cout << "\n..install another element\n";
          table[3].setup(375);
          CHECK (theSum+375==Dummy::checksum());
          
          cout << "\n..kill all elements....\n";
          table.clear();
          CHECK (0 == Dummy::checksum());
        }
      
    };
  
  LAUNCHER (VectorTransfer_test, "unit common");
  
  
}} // namespace lib::test
