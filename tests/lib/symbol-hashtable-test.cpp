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
#include "lib/util.hpp"

#include "lib/symbol.hpp"

#include <boost/functional/hash.hpp>
#include <tr1/unordered_map>
#include <string>

using util::isnil;
using util::contains;



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
    };
  
  LAUNCHER (SymbolHashtable_test, "function common");
  
  
}} // namespace lib::test

