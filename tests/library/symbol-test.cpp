/*
  Symbol(Test)  -  verify basic properties of a Symbol datatype

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

/** @file symbol-test.cpp
 ** unit test \ref Symbol_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include "lib/symbol.hpp"

#include <string>

using util::typeStr;
using util::isSameObject;
using util::isnil;
using std::string;



namespace lib {
namespace test{
  
  
  /******************************************************//**
   * @test properties of Symbol data type. Currently this is
   *       just a thin wrapper for a const char *
   * @todo this test is very much WIP, as the implementation
   *       of a real symbol type and symbol table remains
   *       to be done. ///////////////////////////Ticket #157
   */
  class Symbol_test : public Test
    {
      
      virtual void
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
          CHECK (sizeof(Literal) == sizeof(char*));
          
          CHECK (li1 == li2);
          CHECK (!isSameObject (li1,li2));
          CHECK (li1 != li3);
          CHECK (li2 != li3);
          CHECK (li3 != li2);
          
          cout << typeStr (li1 + string("ce"))         << endl;
          cout << typeStr (string("minus " +li1))      << endl;
          cout << li2+string("..") << string("..")+li2 << endl;
          
          CHECK (hash_value(li1) == hash_value(li2));
          CHECK (hash_value(li2) != hash_value(li3));
        }
      
      
      void
      checkEmptyLiteral()
        {
          Literal nn1 (0);
          Literal nn2 ("");
          
          CHECK (isnil (nn1));
          CHECK (isnil (nn2));
          
          Literal nnn (" ");
          CHECK (!isnil (nnn));
        }
      
      
      void
      checkSymbolCreation()
        {
          Literal l1("1");
          Symbol sy1("1");
          Symbol sy2(l1);
          
          CHECK (sy1 == sy2);
          CHECK (not isSameObject (l1,sy1));
          CHECK (not isSameObject (sy1,sy2));
          CHECK (l1.c() == sy1.c());
          CHECK (sy1.c() == sy2.c());
          
          Symbol sy3;
          CHECK (isnil(sy3));
          CHECK (sy1 != sy3);
          
          sy3 = Symbol{l1};
          CHECK (!isnil(sy3));
          CHECK (sy1 == sy3);
          
          Symbol sy4{sy3, "11"};
          CHECK (sy4 == "1.11");
          CHECK (not isSameObject(sy4.c(), "1.11"));
          CHECK (sy4.c() == Symbol{"1.11"}.c());
          CHECK (sy4.c() == (const char*)sy4);
          CHECK (hash_value(sy4) == hash_value(Symbol{"1.11"}));
        }
    };
  
  LAUNCHER (Symbol_test, "unit common");
  
  
}} // namespace lib::test
