/*
  FormatHelper(Test)  -  validate formatting and diagnostics helpers

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file format-helper-test.cpp
 ** unit test \ref FormatHelper_test
 */


#include "lib/test/run.hpp"
#include "lib/format-util.hpp"
#include "lib/format-string.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/error.hpp"

#include <vector>
#include <string>


using lib::transformIterator;
using lib::iter_stl::snapshot;
using lib::iter_stl::eachElm;
using lib::eachNum;
using util::_Fmt;

using std::vector;
using std::string;
using std::to_string;


namespace util {
namespace test {
  
  namespace { // test fixture...
    
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
  }
  
  
  
  /***************************************************************************//**
   * @test verifies the proper working of some string-formatting helper functions.
   *       - util::toString() provides a failsafe to-String conversion, preferring
   *         an built-in conversion, falling back to just a type string.
   *       - util::join() combines elements from arbitrary containers or iterators
   *         into a string, relying on aforementioned generic string conversion
   * @see format-util.hpp
   */
  class FormatHelper_test
    : public Test
    {
      void
      run (Arg)
        {
          check2String();
          checkStringify();
          checkStringJoin();
          checkPrefixSuffix();
        }
      
      
      /** @test verify a failsafe to-string conversion. */
      void
      check2String ()
        {
          Reticent closeLipped;
          UnReticent chatterer;
          
          CHECK (toString (closeLipped) == "«Reticent»"_expect);
          CHECK (toString (chatterer)   ==   "hey Joe!"_expect);
          
          CHECK (toString (&chatterer)  ==  "↗hey Joe!"_expect); // pointer indicated
          CHECK (toString (nullptr)     ==          "↯"_expect); // runtime exception, caught
          
          CHECK (toString (true)        ==       "true"_expect); // special handling for bool
          CHECK (toString (2+2 == 5)    ==      "false"_expect);
          CHECK (toString (12.34e55)    ==  "1.234e+56"_expect);
          
          CHECK (toString (short(12))
                +toString (345L)
                +toString ("67")
                +toString ('8')         ==   "12345678"_expect); // these go through lexical_cast<string>
        }
      
      
      /** @test inline to-string converter function
       *        - can be used as transforming iterator
       *        - alternatively accept arbitrary arguments
       */
      void
      checkStringify()
        {
          // use as transformer within an (iterator) pipeline
          auto ss = stringify (eachNum (1.11, 10.2));
          
          CHECK (ss);
          CHECK ("1.11" == *ss);
          ++ss;
          CHECK ("2.11" == *ss);
          
          string res{".."};
          for (auto s : ss)
            res += s;
          
          CHECK (res == "..2.113.114.115.116.117.118.119.1110.11"_expect);
          
          
          using VecS = vector<string>;
          
          // another variant: collect arbitrary heterogeneous arguments
          VecS vals = stringify (short(12), 345L, "67", '8');
          CHECK (vals == VecS({"12", "345", "67", "8"}));
          
          
          // stringify can both consume (RValue-Ref) or take a copy from its source
          auto nn = snapshot (eachNum (5, 10));
          CHECK (5 == *nn);
          ++nn;
          CHECK (6 == *nn);
          
          auto sn = stringify (nn);
          CHECK ("6" == *sn);
          ++sn;
          CHECK ("7" == *sn);
          CHECK (6 == *nn);
          ++++nn;
          CHECK (8 == *nn);
          CHECK ("7" == *sn);
          
          sn = stringify (std::move(nn));
          CHECK ("8" == *sn);
          CHECK (isnil (nn)); // was consumed by moving it into sn
          ++sn;
          CHECK ("9" == *sn);
          ++sn;
          CHECK (isnil (sn));
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
          
          
          CHECK (join (dubious, "--+--")
                 == "0--+--"
                    "1.1--+--"
                    "2.2--+--"
                    "3.3--+--"
                    "4.4--+--"
                    "5.5--+--"
                    "6.6--+--"
                    "7.7--+--"
                    "8.8--+--"
                    "9.9"_expect);
          CHECK (join (transformIterator(eachElm(dubious), justCount))
                 == "Nr.01(0.0), "
                    "Nr.02(2.2), "
                    "Nr.03(4.4), "
                    "Nr.04(6.6), "
                    "Nr.05(8.8), "
                    "Nr.06(11.0), "
                    "Nr.07(13.2), "
                    "Nr.08(15.4), "
                    "Nr.09(17.6), "
                    "Nr.10(19.8)"_expect);
        }
      
      
      /** @test convenience helpers to deal with prefixes and suffixes */
      void
      checkPrefixSuffix()
        {
          string abcdef{"abcdef"};
          CHECK (startsWith (abcdef, "abcdef"));
          CHECK (startsWith (abcdef, "abcde"));
          CHECK (startsWith (abcdef, "abcd"));
          CHECK (startsWith (abcdef, "abc"));
          CHECK (startsWith (abcdef, "ab"));
          CHECK (startsWith (abcdef, "a"));
          CHECK (startsWith (abcdef, ""));
          
          CHECK (endsWith (abcdef, "abcdef"));
          CHECK (endsWith (abcdef, "bcdef"));
          CHECK (endsWith (abcdef, "cdef"));
          CHECK (endsWith (abcdef, "def"));
          CHECK (endsWith (abcdef, "ef"));
          CHECK (endsWith (abcdef, "f"));
          CHECK (endsWith (abcdef, ""));
          
          CHECK (startsWith (string{}, ""));
          CHECK (endsWith   (string{}, ""));
          
          CHECK (not startsWith (string{"abc"}, "abcd"));
          CHECK (not startsWith (string{"a"}, "ä"));
          CHECK (not startsWith (string{"ä"}, "a"));
          
          CHECK (not endsWith (string{"abc"}, " abc"));
          CHECK (not endsWith (string{"a"},   "ä"));
          CHECK (not endsWith (string{"ä"},   "a"));
          
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

