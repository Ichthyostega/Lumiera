/*
  BranchCase(Test)  -  verify parsing textual specifications

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file branch-case-test.cpp
 ** unit test \ref BranchCase_test
 */



#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/branch-case.hpp"
#include "lib/format-obj.hpp"
#include "lib/test/diagnostic-output.hpp"//////////////////TODO


namespace lib {
namespace test{
  
  
  /********************************************************//**
   * @test verify a _Sum Type_ to hold alternative model types
   *       for several result branches of an evaluation.
   * @see parse.hpp "usage example"
   */
  class BranchCase_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          demonstrateStorage();
          verifyCopyAssignment();
        }
      
      
      /** @test create one alternative and access embedded model value. */
      void
      simpleUsage()
        {
          using Branch = BranchCase<char,ushort>;
          Branch branch{1, 42};                  // construct for second branch (#1) to hold ushort(42)
          CHECK (1 == branch.selected());
          CHECK (42 == branch.get<1>());         // direct access with known branch-nr
          CHECK ('*' == branch.get<0>());        // Warning: no protection against accessing the wrong branch
          
          int val{-5};
          auto visitor = [&](auto& it){ val = it;};
          branch.accept (visitor);
          CHECK (42 == val);
        }
      
      
      /** @test demonstrate expected storage layout...
       *      - the selector field always coincides with the object itself
       *      - the storage buffer starts after the `size_t` selector
       */
      void
      demonstrateStorage()
        {
          using Branch = BranchCase<ushort,double>;
          CHECK (sizeof(double)+sizeof(size_t) <= sizeof(Branch));
          CHECK (sizeof(double) == Branch::SIZ);
          
          double phi{(1+sqrt(5))/2};
          Branch b1{1,phi};
          CHECK (1   == b1.selected());
          CHECK (phi == b1.get<1>());
          
          auto p = reinterpret_cast<size_t*> (&b1);
          CHECK (1   == *p);
          CHECK (phi == * reinterpret_cast<double*>(p+1));
          
          // force-place a differently constructed object at the same location
          new(p) Branch{0,42};
          CHECK (0   == b1.selected());
          CHECK (42  == b1.get<0>());
          CHECK (0   == *p);
          CHECK (42  == * reinterpret_cast<ushort*>(p+1));
        }
      
      
      
      /** @test verify selector and payload instances
       *        are properly handled on copy, clone, assignment and swap.
       */
      void
      verifyCopyAssignment()
        {
          using Branch = BranchCase<char,string>;
          CHECK (sizeof(string)+sizeof(size_t) <= sizeof(Branch));
          
          // use generic to-String visitor to display contents
          auto render = [](auto const& it) -> string { return util::toString(it); };
          
          Branch b1{1, "evil"};
          CHECK ( 1 == b1.TOP );
          CHECK ( 1 == b1.selected());
          CHECK ("evil" == b1.get<1>());
          CHECK ("evil" == b1.accept(render));
          
          Branch b2{0,42};
          CHECK ( 0  == b2.selected());
          CHECK ('*' == b2.get<0>());
          CHECK ("*" == b2.accept(render));
          
          Branch b3{b1};
          CHECK (1      == b3.selected());
          CHECK ("evil" == b3.accept(render));
          
          b3 = b2;
          CHECK ( 0     == b3.selected());
          CHECK ("*"    == b3.accept(render));
          CHECK ("*"    == b2.accept(render));
          CHECK ("evil" == b1.accept(render));
          
          b3 = move(b1);
          CHECK ( 1     == b3.selected()    );
          CHECK ( 0     == b2.selected()    );
          CHECK ("evil" == b3.accept(render));
          CHECK ("*"    == b2.accept(render));
          CHECK (""     == b1.accept(render));   // ◁——————————— warning: moved-away string is "implementation defined"
          
          swap (b3,b2);
          CHECK ( 0     == b3.selected());
          CHECK ( 1     == b2.selected());
          CHECK ("*"    == b3.accept(render));
          CHECK ("evil" == b2.accept(render));
          CHECK (""     == b1.accept(render));
          
           //_______________________________
          // verify proper payload lifecycle
          seedRand();
          Dummy::checksum() = 0;
          {  //  track instances by checksum...
            Dummy dummy;
            auto rr = dummy.getVal();
            CHECK (rr == Dummy::checksum());
            CHECK (rr > 0);
            
            using BB = BranchCase<string,Dummy>;
            BB bb1{1, dummy};
            CHECK (bb1.get<1>().getVal() == rr);
            CHECK (2*rr == Dummy::checksum());   // got two instances due to copy-init

            BB bb2{0, "dummy"};
            CHECK (2*rr == Dummy::checksum());
            
            swap (bb1,bb2);
            CHECK (bb1.get<0>() == "dummy");
            CHECK (bb2.get<1>().getVal() == rr);
            CHECK (2*rr == Dummy::checksum());
            
            bb1 = bb2;
            CHECK (bb1.get<1>().getVal() == rr);
            CHECK (3*rr == Dummy::checksum());   // assignment by copy
            
            bb2 = move(bb1);                     // move-assignment
            CHECK (2*rr == Dummy::checksum());   // existing instance destroyed properly
            CHECK (bb2.get<1>().getVal() == rr);
            CHECK (bb1.get<1>().getVal() == Dummy::DEFUNCT);
            
            bb2 = BB{1,Dummy()};                 // wipes out the other copy
            auto rr2 = bb2.get<1>().getVal();    // but implants a different one
            CHECK (rr+rr2 == Dummy::checksum());
            CHECK (rr     == dummy.getVal());
          }// leave scope: invoke dtors here
          
          CHECK (0 == Dummy::checksum());
        }
    };
  
  LAUNCHER (BranchCase_test, "unit common");
  
  
}} // namespace lib::test

