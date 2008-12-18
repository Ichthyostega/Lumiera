/*
  RemoveFromSet(Test)  -  algorithm removing predicated elements from set
 
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

#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <iostream>
#include <string>
#include <set>

using std::cout;
using std::string;



namespace util
  {
  namespace test
    {
    using util::for_each;
    using boost::lambda::_1;
    using boost::lambda::bind;
    using boost::lexical_cast;
    
    
  
    template<class COLL>
    void 
    show (COLL const& coll)
    {
      cout << "[ ";
      for_each (coll, cout << _1 << ", ");
      cout << "]\n";
    }
    
    bool
    killerselector (string description, uint candidate)
    {
      return string::npos != description.find( lexical_cast<string> (candidate));
    }
    
    
    
    class RemoveFromSet_test : public Test
      {
        virtual void
        run (Arg arg)
          {
            test_remove (" nothing ");
            test_remove ("0");
            test_remove ("9");
            test_remove ("5");
            test_remove ("0   2   4   6   8  ");
            test_remove ("  1   3   5   7   9");
            test_remove ("0 1 2 3 4 5 6 7 8 9");
            test_remove ("0 1 2 3 4 5 6 7 8  ");
            test_remove ("  1 2 3 4 5 6 7 8 9");
            test_remove ("0 1 2 3 4   6 7 8 9");
          }
        
        
        /** @test populate a test set,
         *        remove the denoted elements
         *        and print the result...  */
        void
        test_remove (string elems_to_remove)
          {
            std::set<uint> theSet;
            for (int i=0; i<10; ++i)
              theSet.insert (i);
            
            util::remove_if (theSet, bind( killerselector, elems_to_remove, _1));
            
            cout << "removed " << elems_to_remove << " ---> ";
            show (theSet);
          }
        
      };
    
      
      LAUNCHER (RemoveFromSet_test, "unit common");

      
  } // namespace test
    
} // namespace util

