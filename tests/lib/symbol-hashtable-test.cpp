/*
  SymbolHashtable(Test)  -  building a hashtable with Symbol (id) keys
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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

#include "lib/symbol.hpp"

#include <boost/functional/hash.hpp>
#include <tr1/unordered_map>
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
  
  typedef std::tr1::unordered_map< Symbol, string, hash<Symbol> > HTable;
  
  
  /*************************************************************
   * @test build a hashtable using Symbol objects as Keys.
   *       Especially this verifies picking up a customised
   *       hash implementation via ADL
   * @see symbol-impl.cpp
   */
  class SymbolHashtable_test : public Test
    {
      
      void
      run (Arg)
        {
          checkHashFunction();
          
          HTable table;
          ASSERT (isnil(table));
          
          table[KEY1] = string (KEY1);
          table[KEY2] = string (KEY2);
          table[KEY3] = string (KEY3);
          table[KEY4] = string (KEY4);
          
          ASSERT (!isnil(table));
          ASSERT (4 == table.size());
          ASSERT (contains (table, KEY1));
          ASSERT (contains (table, KEY2));
          ASSERT (contains (table, KEY3));
          ASSERT (contains (table, KEY4));
          ASSERT (!contains (table, KEY5));
          
          ASSERT (string (KEY1) == table[KEY1]);
          ASSERT (string (KEY2) == table[KEY2]);
          ASSERT (string (KEY3) == table[KEY3]);
          ASSERT (string (KEY4) == table[KEY4]);
          
          table[KEY3] += "..."; 
          ASSERT (string(KEY3)       != table[KEY3]);
          ASSERT (string(KEY3)+"..." == table[KEY3]);
          
          ASSERT (isnil (table[KEY5])); // adds a new empty value object as side effect
          ASSERT (5 == table.size());
        }
      
      
      void
      checkHashFunction()
        {
          string random = randStr(STRING_MAX_RELEVANT+1);
          
          string copy1(random);
          copy1[5] = '\0';      // truncate the c-String to 5 chars
          
          string copy2(random);           
          copy2[rand() % STRING_MAX_RELEVANT] = '*';  // modify a random position
          
          string copy3(copy2);
          copy3[STRING_MAX_RELEVANT]          = '*';  // modify behind observation limit
          
          Symbol  l0;
          Literal l51 (copy1.c_str());
          Literal l52 (random.substr(0,5).c_str());
          
          Literal l_1 (random.c_str());
          Literal l_2 (copy2.c_str());
          Literal l_3 (copy3.c_str());
          
          ASSERT (isnil (l0));
          ASSERT (l0  != l51);
          ASSERT (l51 == l52);
          
          ASSERT (l51 != l_1);
          ASSERT (l_1 != l_2);
          ASSERT (l_2 == l_3);  // difference not detected due to observation limit...
          ASSERT (!std::strncmp (l_2, l_3, STRING_MAX_RELEVANT  ));
          ASSERT ( std::strncmp (l_2, l_3, STRING_MAX_RELEVANT+1));
          
          size_t h0   = hash_value (l0);
          size_t h51  = hash_value (l51);
          size_t h52  = hash_value (l52);
          size_t h_1  = hash_value (l_1);
          size_t h_2  = hash_value (l_2);
          size_t h_3  = hash_value (l_3);
          
          ASSERT (h0  == 0);
          ASSERT (h51 != 0);
          ASSERT (h52 != 0);
          ASSERT (h_1 != 0);
          ASSERT (h_2 != 0);
          ASSERT (h_3 != 0);
          
          ASSERT (h51 == h52);    // verify the hash function indeed stops at '\0'
          ASSERT (h51 != h_1);    // share a common prefix, but the hash differs
          ASSERT (h_1 != h_2);    // the single random modification is detected
          ASSERT (h_2 == h_3);    // because l_2 and l_3 differ behind the fixed observation limit
          
          ASSERT (h_1 == hash_value (l_1)); //reproducible
          ASSERT (h_2 == hash_value (l_2));
          ASSERT (h_3 == hash_value (l_3));
        }
    };
  
  LAUNCHER (SymbolHashtable_test, "function common");
  
  
}} // namespace lib::test

