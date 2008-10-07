/*
  RefArray(Test)  -  unittest for wrapping with array-of-refs access
 
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

#include "lib/refarrayimpl.hpp"

//#include <boost/lexical_cast.hpp>
//#include <boost/format.hpp>
#include <iostream>
#include <vector>

//using boost::lexical_cast;
//using boost::format;
//using util::isnil;
//using std::string;
using test::Test;
using std::cout;
using std::vector;


namespace lib {
  namespace test {
    
    namespace { // test types
    
      struct I
        {
          virtual int op(int i)  const =0;
          virtual ~I() {}
        };
      
      struct Sub1 : I
        {
          int op (int i) { return i+1; }
        };
      
      struct Sub2 : I
        {
          const char* letterz_; 
          Sub2() : letterz_("ABCDEFGHIJKLMNOPQRSTUVWXYZ") {}
          
          int op (int i)  const { return (int)letterz_[i % 26]; }
        };
      
    } // (END) test types
    
    
    
    
    
    
    
    /*******************************************************************
     * @test build a wrapper providing RefArray access to a given vector
     * @see refarrayimpl.hpp
     */
    class RefArray_test : public Test
      {
        
        virtual void
        run (Arg arg) 
          {
            vector<Sub2> subz(10);
            RefArrayVectorWrapper<I,Sub2> subWrapz (subz);
            
            RefArray<I> & rArr (subWrapz);
            
            ASSERT (subWrapz.size()==subz.size());
            ASSERT (INSTANCEOF(I, &rArr[0]));
            for (size_t i=0; i<subz.size(); ++i)
              ASSERT (rArr[i].op(i) == subz[i].op(i));
          }

        
      };
    
      
    
    /** Register this test class... */
    LAUNCHER (RefArray_test, "unit common");
    
    
    
  } // namespace test

} // namespace lib
