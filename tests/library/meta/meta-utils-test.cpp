/*
  MetaUtils(Test)  -  check some simple type trait helpers 

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file meta-utils-test.cpp
 ** unit test \ref MetaUtils_test
 */


#include "lib/symbol.hpp"
#include "lib/test/run.hpp"
#include "lib/meta/util.hpp"
#include "lib/meta/typelist.hpp"

#include <string>
#include <iostream>
using std::cout;
using std::endl;/////////////////////////TODO


namespace lib  {
namespace meta {
namespace test {
  
  using std::string;
  
  
  
  
  
  
  /*********************************************************************//**
   * @test verify basic type trait and metaprogramming helpers.
   *       - marker types to tell which overload the compiler picks
   *       - simple trait to detect the possibility of a string conversion
   *       - trait to detect a typelist type
   */
  class MetaUtils_test : public Test
    {
      void
      run (Arg) 
        {
          verify_basicTypeProbing();
          verify_genericTypeDisplay();
          
          detect_stringConversion();
          detect_typeList();
        }
      
      
      /** @test demonstrate the basic type trait detection technique:
       *  - we have two overloads with differing return type
       *  - we form a function call expression
       *  - by investigating the return type,
       *    we can figure out which overload the compiler picks.
       */
      void
      verify_basicTypeProbing()
        {
          CHECK (sizeof(Yes_t) != sizeof (No_t));
          
          CHECK (sizeof(Yes_t) == sizeof (probe (1)));
          CHECK (sizeof(Yes_t) == sizeof (probe (1L)));         // conversion long -> int
          CHECK (sizeof(Yes_t) == sizeof (probe ('a')));        // conversion char -> int
          CHECK (sizeof(No_t)  == sizeof (probe ("a")));        // char * can't be converted
        }
      
      static Yes_t probe (int);
      static No_t  probe (...);
      
      
      
      void
      verify_genericTypeDisplay()
        {
          cout << typeStr<SubString>() <<endl;
          
          struct Lunatic
            : Test
            {
              virtual void run (Arg) {}
            }
          lunatic;
          cout << typeStr(lunatic)     << endl;
          cout << typeStr(&lunatic)      << endl;
          cout << typeStr((Test &)lunatic) << endl;
          cout << typeStr((Test *) &lunatic) << endl;
          cout << typeStr(&Lunatic::run)       << endl;
        }
      
      
      
      //-------------------------------------------------TEST-types--
      class SubString : public string
        { 
        public:
            SubString() : string("sublunar") { }
        };
      
      class Something { };
      
      struct SomehowStringy
        {
          operator string() { return "No such thing"; }
        };
      
      struct SomehowSubtle
        {
          operator SubString() { return SubString(); }
        };
      
      class SomehowSubSub : public SomehowSubtle { };
      //-------------------------------------------------TEST-types--
      
      template<typename TY>
      static bool
      can_convert (TY const&)
        {
          return can_convertToString<TY>::value;
        }
      
      void
      detect_stringConversion()
        {
          CHECK ( can_convert (string("inline string")));
          CHECK ( can_convert ("char literal"));
          CHECK (!can_convert (23.34));
          CHECK (!can_convert (23));
          CHECK (!can_convert (1L));
          
          string str("mhm");
          string & str_ref (str);
          string const& str_const_ref (str);
          string * str_ptr = &str;
          
          CHECK ( can_convert (str));
          CHECK ( can_convert (str_ref));
          CHECK ( can_convert (str_const_ref));
          CHECK ( can_convert (*str_ptr));
          CHECK (!can_convert (str_ptr));
          
          SubString sub;
          Something thing;
          const SomehowStringy stringy = SomehowStringy();
          SomehowSubSub subsub;
          SubString const& subRef(subsub);
          
          CHECK ( can_convert (sub));
          CHECK (!can_convert (thing));
          CHECK ( can_convert (stringy));
          CHECK ( can_convert (subsub));
          CHECK ( can_convert (subRef));
        }
      
      
      
      //-------------------------------------------------TEST-types--
      typedef Types< int
                   , uint
                   , int64_t
                   , uint64_t
                   >::List     TheList;
      
      typedef Types<  >::List  EmptyList;
      //-------------------------------------------------TEST-types--
      
      
      void
      detect_typeList()
        {
          CHECK ( is_Typelist<TheList>::value);
          CHECK ( is_Typelist<EmptyList>::value);
          CHECK (!is_Typelist<Something>::value);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MetaUtils_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
