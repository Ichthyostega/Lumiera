/*
  ScopedPtrVect(Test)  -  holding and owning a collection of noncopyable objects
 
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
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/scoped-ptrvect.hpp"
#include "testdummy.hpp"


namespace lib {
namespace test{
  
  using ::Test;
  using util::isnil;
  using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;
  
  
  typedef ScopedPtrVect<Dummy> VectD;
  
  
  /********************************************************************
   *  @test ScopedPtrVect manages the lifecycle of a number of objects.
   *  @todo implement detaching of objects
   */
  class ScopedPtrVect_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          iterating();
//        detaching();
        }
      
      
      
      void
      simpleUsage()
        {
          ASSERT (0==checksum);
          {
            VectD holder;
            ASSERT (isnil (holder));
            ASSERT (0==checksum);
            
            Dummy* ptr = new Dummy();
            Dummy& ref = holder.manage (ptr);
            ASSERT (!isnil (holder));
            ASSERT (0!=checksum);
            ASSERT (&ref==ptr);
            
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            ASSERT (3 == holder.size());
            
            holder.clear();
            ASSERT (0==checksum);
            ASSERT (isnil (holder));
            
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            holder.manage (new Dummy);
            ASSERT (9 == holder.size());
            ASSERT (0!=checksum);
          }
          ASSERT (0==checksum);
        }
      
      
      void
      iterating()
        {
          ASSERT (0==checksum);
          {
            VectD holder;
            for (int i=0; i<16; ++i)
              holder.manage(new Dummy(i));
            
            int check=0;
            VectD::iterator ii = holder.begin();
            while (ii)
              {
                ASSERT (check == ii->getVal());
                ++check;
                ++ii;
              }
            
            
            // Test the const iterator
            check = 0;
            VectD::const_iterator cii = holder.begin();
            while (cii)
              {
                ASSERT (check == cii->getVal());
                ++check;
                ++cii;
              }
            
            
            // Verify correct behaviour of iteration end
            ASSERT (! (holder.end()));
            ASSERT (isnil (holder.end()));
            
            VERIFY_ERROR (ITER_EXHAUST, *holder.end() );
            VERIFY_ERROR (ITER_EXHAUST, ++holder.end() );
            
            ASSERT (ii == holder.end());
            ASSERT (cii == holder.end());
            VERIFY_ERROR (ITER_EXHAUST, ++ii );
            VERIFY_ERROR (ITER_EXHAUST, ++cii );
            
          }
          ASSERT (0==checksum);
        }
      
    };
  
  LAUNCHER (ScopedPtrVect_test, "unit common");
  
  
}} // namespace lib::test

