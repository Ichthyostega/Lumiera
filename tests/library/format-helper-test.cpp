/*
  FormatHelper(Test)  -  validate formatting and diagnostics helpers

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/format-util.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-string.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/error.hpp"

#include <iostream>
#include <vector>
#include <string>


using lib::transformIterator;
using lib::iter_stl::eachElm;
using util::_Fmt;

using std::vector;
using std::string;
using std::to_string;
using std::cout;
using std::endl;


namespace util {
namespace test {
  
  class Reticent 
    { };
  
  class UnReticent
    : public Reticent 
    {
    public:
      operator string()  const { return "hey Joe!"; }
    };
  
  
  
  class AutoCounter
    {
      static uint cnt;
      
      uint id_;
      double d_;
      
    public:
      AutoCounter(double d)
        : id_(++cnt)
        , d_(d*2)
        { }
      
      operator string()  const
        {
          return _Fmt("Nr.%02d(%3.1f)") % id_ % d_;
        }
    };
  uint AutoCounter::cnt = 0;
  
  
  
  /***************************************************************************//**
   * @test verifies the proper working of some string-formatting helper functions.
   *       - util::toString() provides a failsafe to-String conversion, preferring
   *         an built-in conversion, falling back to just a type string.
   *       - util::join() combines elements from arbitrary containers or iterators
   *         into a string, relyint on aforementioned generic string conversion
   * @see format-util.hpp
   */
  class FormatHelper_test
    : public Test
    {
      void
      run (Arg)
        {
          check2String();
          checkStringJoin();
          checkPrefixSuffix();
        }
      
      
      /** @test verify a failsafe to-string conversion. */
      void
      check2String ()
        {
          std::cout << "Displaying some types....\n";
          
          Reticent closeLipped;
          UnReticent chatterer;
          
          cout << toString (closeLipped) << endl;
          cout << toString (chatterer) << endl;
          
          cout << toString (false) << endl;
          cout << toString (12.34e55) << endl;
          cout << toString (short(12))
               << toString (345L)
               << toString ('X')
               << endl;
        }
      
      
      /** @test verify delimiter separated joining of arbitrary collections.
       *        - the first test uses a STL container, which means we need to wrap
       *          into a lib::RangeIter. Moreover, lexical_cast is used to convert
       *          the double numbers into strings.
       *        - the second test uses an inline transforming iterator to build a
       *          series of AutoCounter objects, which provide a custom string
       *          conversion function. Moreover, since the transforming iterator
       *          conforms to the Lumiera Forward Iterator concept, we can just
       *          move the rvalue into the formatting function without much ado
       */
      void
      checkStringJoin()
        {
          vector<double> dubious;
          for (uint i=0; i<10; ++i)
            dubious.push_back(1.1*i);
          
          std::function<AutoCounter(double)> justCount = [](double d){ return AutoCounter(d); };
          
          
          cout << join(dubious, "--+--") << endl;
          cout << join(transformIterator(eachElm(dubious)
                                        ,justCount)) << endl;
        }
      
      
      /** @test convenience helpers to deal with prefixes and suffixes */
      void
      checkPrefixSuffix()
        {
          CHECK (startsWith ("abcdef", "abcdef"));
          CHECK (startsWith ("abcdef", "abcde"));
          CHECK (startsWith ("abcdef", "abcd"));
          CHECK (startsWith ("abcdef", "abc"));
          CHECK (startsWith ("abcdef", "ab"));
          CHECK (startsWith ("abcdef", "a"));
          CHECK (startsWith ("abcdef", ""));
          CHECK (startsWith ("", ""));
          
          CHECK (not startsWith ("abc", "abcd"));
          CHECK (not startsWith ("a", "ä"));
          CHECK (not startsWith ("ä", "a"));
          
          CHECK (endsWith ("abcdef", "abcdef"));
          CHECK (endsWith ("abcdef", "bcdef"));
          CHECK (endsWith ("abcdef", "cdef"));
          CHECK (endsWith ("abcdef", "def"));
          CHECK (endsWith ("abcdef", "ef"));
          CHECK (endsWith ("abcdef", "f"));
          CHECK (endsWith ("abcdef", ""));
          CHECK (endsWith ("", ""));
          
          CHECK (not endsWith ("abc", " abc"));
          CHECK (not endsWith ("a", "ä"));
          CHECK (not endsWith ("ä", "a"));
          
          string abc{"abcdef"};
          removePrefix(abc, "ab");
          CHECK ("cdef" == abc);
          removeSuffix(abc, "ef");
          CHECK ("cd" == abc);
          
          abc = "bcdef";
          removePrefix(abc, "ab");
          CHECK ("bcdef" == abc);
          removeSuffix(abc, "abcdef");
          CHECK ("bcdef" == abc);
          removeSuffix(abc, "bcdef");
          CHECK (isnil (abc));
        }
    };
  
  LAUNCHER (FormatHelper_test, "unit common");
  
  
}} // namespace util::test

