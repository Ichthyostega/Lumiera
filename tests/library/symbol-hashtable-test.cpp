/*
  SymbolHashtable(Test)  -  building a hashtable with Symbol (id) keys

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file symbol-hashtable-test.cpp
 ** unit test \ref SymbolHashtable_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/symbol.hpp"

#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <cstring>
#include <string>

using util::contains;
using util::isnil;



namespace lib {
namespace test{
  
  using std::string;
  using boost::hash;
  
  
  namespace { // test data
    Symbol KEY1 ("Key1");
    Symbol KEY2 ("Key2");
    Symbol KEY3 ("Key3");
    Symbol KEY4 ("Key4");
    Symbol KEY5 ("Key5");
    
  }
  
  typedef std::unordered_map< Symbol, string, hash<Symbol>> HTable;
  
  
  /*********************************************************//**
   * @test build a hashtable using Symbol objects as Keys.
   *       Especially this verifies picking up a customised
   *       hash implementation via ADL
   * @see symbol-impl.cpp
   */
  class SymbolHashtable_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          checkHashFunction();
          
          HTable table;
          CHECK (isnil(table));
          
          table[KEY1] = string (KEY1);
          table[KEY2] = string (KEY2);
          table[KEY3] = string (KEY3);
          table[KEY4] = string (KEY4);
          
          CHECK (!isnil(table));
          CHECK (4 == table.size());
          CHECK (contains (table, KEY1));
          CHECK (contains (table, KEY2));
          CHECK (contains (table, KEY3));
          CHECK (contains (table, KEY4));
          CHECK (!contains (table, KEY5));
          
          CHECK (string (KEY1) == table[KEY1]);
          CHECK (string (KEY2) == table[KEY2]);
          CHECK (string (KEY3) == table[KEY3]);
          CHECK (string (KEY4) == table[KEY4]);
          
          table[KEY3] += "...";
          CHECK (string(KEY3)       != table[KEY3]);
          CHECK (string(KEY3)+"..." == table[KEY3]);
          
          CHECK (isnil (table[KEY5])); // adds a new empty value object as side effect
          CHECK (5 == table.size());
        }
      
      
      void
      checkHashFunction()
        {
          string random = randStr(STRING_MAX_RELEVANT+1);
          
          string copy1(random);
          copy1[5] = '\0';      // truncate the c-String to 5 chars
          
          string copy2(random);
          copy2[rani (STRING_MAX_RELEVANT)] = '*';  // modify a random position
          
          string copy3(copy2);
          copy3[STRING_MAX_RELEVANT]        = '*';  // modify behind observation limit
          
          Symbol  l0;
          Literal l51 (copy1.c_str());
          Literal l52 (random.substr(0,5).c_str());
          
          Literal l_1 (random.c_str());
          Literal l_2 (copy2.c_str());
          Literal l_3 (copy3.c_str());
          
          CHECK (isnil (l0));
          CHECK (l0  != l51);
          CHECK (l51 == l52);
          
          CHECK (l51 != l_1);
          CHECK (l_1 != l_2);
          CHECK (l_2 == l_3);  // difference not detected due to observation limit...
          CHECK (!std::strncmp (l_2, l_3, STRING_MAX_RELEVANT  ));
          CHECK ( std::strncmp (l_2, l_3, STRING_MAX_RELEVANT+1));
          
          size_t h0   = hash_value (l0);
          size_t h51  = hash_value (l51);
          size_t h52  = hash_value (l52);
          size_t h_1  = hash_value (l_1);
          size_t h_2  = hash_value (l_2);
          size_t h_3  = hash_value (l_3);
          
          CHECK (h0  == 0);
          CHECK (h51 != 0);
          CHECK (h52 != 0);
          CHECK (h_1 != 0);
          CHECK (h_2 != 0);
          CHECK (h_3 != 0);
          
          CHECK (h51 == h52);    // verify the hash function indeed stops at '\0'
          CHECK (h51 != h_1);    // share a common prefix, but the hash differs
          CHECK (h_1 != h_2);    // the single random modification is detected
          CHECK (h_2 == h_3);    // because l_2 and l_3 differ behind the fixed observation limit
          
          CHECK (h_1 == hash_value (l_1)); //reproducible
          CHECK (h_2 == hash_value (l_2));
          CHECK (h_3 == hash_value (l_3));
        }
    };
  
  LAUNCHER (SymbolHashtable_test, "function common");
  
  
}} // namespace lib::test
