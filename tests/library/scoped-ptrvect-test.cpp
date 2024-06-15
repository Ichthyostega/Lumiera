/*
  ScopedPtrVect(Test)  -  holding and owning a collection of noncopyable objects

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

/** @file scoped-ptrvect-test.cpp
 ** unit test \ref ScopedPtrVect_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/scoped-ptrvect.hpp"
#include "lib/test/tracking-dummy.hpp"


namespace lib {
namespace test{
  
  using ::Test;
  using util::isnil;
  using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;
  
  
  typedef ScopedPtrVect<Dummy> VectD;
  
  
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
                CHECK (check == ii->getVal());
                ++check;
                ++ii;
              }
            
            
            // Test the const iterator
            check = 0;
            VectD::const_iterator cii = holder.begin();
            while (cii)
              {
                CHECK (check == cii->getVal());
                ++check;
                ++cii;
              }
            
            
            // Verify correct behaviour of iteration end
            CHECK (! (holder.end()));
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
          Dummy* extracted(0);
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
            
            id2 = holder[2].getVal();
            id3 = holder[3].getVal();
            
            extracted = holder.detach(& holder[2]);
            CHECK (id2 == extracted->getVal());
            CHECK (id3 == holder[2].getVal());
            CHECK (4 == holder.size());
          }
          CHECK (0 < Dummy::checksum());     // not all dummies are dead
          CHECK (id2 == Dummy::checksum()); //  #2 is alive!
          
          extracted->setVal(id2+id3);
          CHECK (id2+id3 == Dummy::checksum());
          
          delete extracted;
          CHECK (0 == Dummy::checksum());
        }
    };
  
  
  LAUNCHER (ScopedPtrVect_test, "unit common");
  
  
}} // namespace lib::test

