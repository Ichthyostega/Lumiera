/*
  RemoveFromSet(Test)  -  algorithm removing predicated elements from set

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


#include "lib/test/run.hpp"
#include "lib/util-foreach.hpp"

#include <boost/lexical_cast.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <set>

using std::cout;
using std::string;
using std::function;



namespace util {
namespace test {
  
  using util::for_each;
  using boost::lexical_cast;
  
  
  using IntSet = std::set<uint>;
  
  void 
  show (IntSet const& coll)
  {
    cout << "[ ";
    for_each (coll, [](uint elm) { cout << elm << ", "; });
    cout << "]\n";
  }
  
  function<bool(uint)>
  select_match (string description) 
  {
    return [&](uint candidate) 
            {
              return string::npos != description.find( lexical_cast<string> (candidate));
            };
  }
  
  
  
  class RemoveFromSet_test : public Test
    {
      virtual void
      run (Arg)
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
          IntSet theSet;
          for (int i=0; i<10; ++i)
            theSet.insert (i);
          
          util::remove_if (theSet, select_match(elems_to_remove));
          
          cout << "removed " << elems_to_remove << " ---> ";
          show (theSet);
        }
      
    };
  
  
  LAUNCHER (RemoveFromSet_test, "unit common");
  
  
}} // namespace util::test

