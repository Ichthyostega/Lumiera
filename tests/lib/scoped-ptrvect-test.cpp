/*
  ScopedPtrVect(Test)  -  holding and owning a collection of noncopyable objects

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file scoped-ptrvect-test.cpp
 ** unit test \ref ScopedPtrVect_test
 */



#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "test/tracking-dummy.hpp"

#include "lib/scoped-ptrvect.hpp"
#include "lib/util.hpp"


using test::Dummy;
using util::isnil;
using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;

namespace lib {
namespace test{
  
  using VectD = ScopedPtrVect<Dummy>;
  
  
  /****************************************************************//**
   *  @test ScopedPtrVect manages the lifecycle of a number of objects.
   *        The API is similar to a vector and allows for element access
   *        and iteration. Individual elements can be detached and thus
   *        removed from the responsibility of the container.
   */
  class ScopedPtrVect_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          iterating();
          detaching();
          moving();
        }
      
      
      
      void
      simpleUsage()
        {
          CHECK (0 == Dummy::checksum());
          {
            VectD holder;
            CHECK (isnil (holder));
            CHECK (0 == Dummy::checksum());
            
            Dummy* ptr = new Dummy();
            Dummy& ref = holder.manage (ptr);
            CHECK (!isnil (holder));
            CHECK (0 != Dummy::checksum());
            CHECK (&ref==ptr);
            
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            CHECK (3 == holder.size());
            
            holder.clear();
            CHECK (0 == Dummy::checksum());
            CHECK (isnil (holder));
            
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            CHECK (9 == holder.size());
            CHECK (0 < Dummy::checksum());
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      iterating()
        {
          CHECK (0 == Dummy::checksum());
          {
            VectD holder;
            for (int i=0; i<16; ++i)
              holder.manage(new Dummy(i));
            
            int check=0;
            VectD::iterator ii = holder.begin();
            while (ii)
              {
                CHECK (check == ii->getID());
                ++check;
                ++ii;
              }
            
            
            // Test the const iterator
            check = 0;
            VectD::const_iterator cii = holder.begin();
            while (cii)
              {
                CHECK (check == cii->getID());
                ++check;
                ++cii;
              }
            
            
            // Verify correct behaviour of iteration end
            CHECK (not holder.end());
            CHECK (isnil (holder.end()));
            
            VERIFY_ERROR (ITER_EXHAUST, *holder.end() );
            VERIFY_ERROR (ITER_EXHAUST, ++holder.end() );
            
            CHECK (ii == holder.end());
            CHECK (cii == holder.end());
            VERIFY_ERROR (ITER_EXHAUST, ++ii );
            VERIFY_ERROR (ITER_EXHAUST, ++cii );
            
          }
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      detaching()
        {
          int id2, id3;
          Dummy* extracted{nullptr};
          CHECK (0 == Dummy::checksum());
          {
            VectD holder;
            CHECK (0 == Dummy::checksum());
            CHECK (isnil (holder));
            
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            CHECK (5 == holder.size());
            CHECK (0 < Dummy::checksum());
            
            id2 = holder[2].getID();
            id3 = holder[3].getID();
            
            extracted = holder.detach(& holder[2]);
            CHECK (id2 == extracted->getID());
            CHECK (id3 == holder[2].getID());
            CHECK (4 == holder.size());
          }
          CHECK (0 < Dummy::checksum());     // not all dummies are dead
          CHECK (id2 == Dummy::checksum()); //  #2 is alive!
          
          extracted->setID (id2+id3);
          CHECK (id2+id3 == Dummy::checksum());
          
          delete extracted;
          CHECK (0 == Dummy::checksum());
        }
      
      
      void
      moving()
        { {
            VectD org;
            VectD left;
            CHECK (0 == Dummy::checksum());
            
            org.manage (new Dummy);
            org.manage (new Dummy);
            org.manage (new Dummy);
            
            CHECK (not isnil (org));
            CHECK (    isnil (left));
            auto sum = Dummy::checksum();
            CHECK (sum > 0);
            int id0 = org[0].getID(),
                id1 = org[1].getID(),
                id2 = org[2].getID();
            
            // create by move
            VectD right{std::move (org)};
            CHECK (    isnil (org));
            CHECK (    isnil (left));
            CHECK (not isnil (right));
            CHECK (sum == Dummy::checksum());
            
            // move-assignment
            left = std::move (right);
            CHECK (    isnil (org));
            CHECK (not isnil (left));
            CHECK (    isnil (right));
            CHECK (sum == Dummy::checksum());
            CHECK (id0 == left[0].getID());
            CHECK (id1 == left[1].getID());
            CHECK (id2 == left[2].getID());
            
          }
          CHECK (0 == Dummy::checksum());
        }
    };
  
  
  LAUNCHER (ScopedPtrVect_test, "unit common");
  
  
}} // namespace lib::test

