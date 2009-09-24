/*
  Symbol(Test)  -  verify basic properties of a Symbol datatype
 
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
#include "lib/test/test-helper.hpp"

#include "lib/symbol.hpp"


#include <iostream>

using std::cout;
using std::endl;
using util::isnil;
using util::isSameObject;



namespace lib {
namespace test{
  
  
  /**********************************************************
   * @test properties of Symbol data type. Currently this is
   *       just a thin wrapper for a const char *
   * @todo this test is very much WIP, as the implementation
   *       of a real symbol type and symbol table remains 
   *       to be done. See Ticket #157
   */
  class Symbol_test : public Test
    {
      
      void
      run (Arg)
        {
          checkLiteral();
          checkSymbolCreation();
        }
      
      
      void
      checkLiteral()
        {
          Literal li1 ("one");
          Literal li2 (li1);
          Literal li3 ("one ");
          
          cout << li1 << endl;
          cout << showSizeof(li1) << endl;
          ASSERT (sizeof(Literal) == sizeof(char*));
          
          ASSERT (li1 == li2);
          ASSERT (!isSameObject (li1,li2));
          ASSERT (li1 != li3);
          ASSERT (li2 != li3);
          ASSERT (li3 != li2);
          
          cout << showType(li1 + string("ce"))         << endl;
          cout << showType(string("minus " +li1))      << endl;
          cout << li2+string("..") << string("..")+li2 << endl;
          
          ASSERT (hash_value(li1) == hash_value(li2));
          ASSERT (hash_value(li2) != hash_value(li3));
        }
      
      
      void
      checkEmptyLiteral()
        {
          Literal nn1 (0);
          Literal nn2 ("");
          
          ASSERT (isnil (nn1));
          ASSERT (isnil (nn2));
          
          Literal nnn (" ");
          ASSERT (!isnil (nnn));
        }
      
      
      void
      checkSymbolCreation()
        {
          Literal l1("1");
          Symbol sy1("1");
          Symbol sy2(l1);
          
          ASSERT (sy1 == sy2);
          ASSERT (!isSameObject (l1,sy1));
          ASSERT (!isSameObject (sy1,sy2));
          
          Symbol sy3;
          ASSERT (isnil(sy3));
          ASSERT (sy1 != sy3);
          
          sy3 = l1;
          ASSERT (!isnil(sy3));
          ASSERT (sy1 == sy3);
          
          TODO ("real functionality of Symbol data type");
        }
    };
  
  LAUNCHER (Symbol_test, "unit common");
  
  
}} // namespace lib::test

