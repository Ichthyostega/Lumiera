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
#include <map>

using util::typeStr;
using util::isSameObject;
using util::isnil;
using std::string;
using std::map;



namespace lib {
namespace test{
  
  
  /***********************************************************//**
   * @test properties of Literal and Symbol data types.
   *     - a lib::Literal is just a thin wrapper for a `const char *`
   *     - lib::Symbol uses the same implementation, but relies on
   *       character string constants _interned_ into a symbol table.
   * @todo 2023 this test is very much in preliminary shape, as the
   *       implementation of a real symbol table remains was lacking.
   *       At some point, a simplistic implementation of »interned strings«
   *       was added (TICKET #157) — yet still the actual relevance of
   *       unique Symbols remains nebulous
   */
  class Symbol_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          checkLiteral();
          checkSymbolCreation();
          checkComparisons();
          use_as_map_key();
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
          
          CHECK ("string" == typeStr (li1 + string{"night"}));
          CHECK ("string" == typeStr (string{"minus " +li1}));
          cout << li1 + string{"night"} << endl;
          cout << string{"minus " +li1} << endl;
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
          CHECK (l1.c() != sy1.c());
          CHECK (sy1.c() == sy2.c());
          
          Symbol sy3;
          CHECK (not sy3);
          CHECK (sy3 == "⟂");
          CHECK (isnil(sy3));
          CHECK (sy1 != sy3);
          
          CHECK (not Symbol{"⟂"});
          CHECK (sy3 == Symbol{"⟂"});
          CHECK (sy3.c() == Symbol{"⟂"}.c());
          CHECK (Symbol{}.c() == Symbol{"⟂"}.c());
          
          // EMPTY and BOTTOM are distinct Symbols, yet both count as "empty"
          CHECK (Symbol::EMPTY == Symbol{""});
          CHECK (Symbol::BOTTOM == Symbol{"⟂"});
          CHECK (Symbol::EMPTY != Symbol::BOTTOM);
          CHECK (Symbol{nullptr} == Symbol::BOTTOM);
          CHECK (Symbol::EMPTY.empty());
          CHECK (Symbol::BOTTOM.empty());
          CHECK (not Symbol::FAILURE.empty());
          CHECK (isnil (Symbol{"⟂"}));
          CHECK (isnil (Symbol{""}));
          
          // re-assignment
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
      
      
      void
      checkComparisons()
        {
          const char* s1 = "1";
          const char* s3 = "11";
          const char* s2 = s3+1;
          
          CHECK (s1 != s2);
          CHECK (s1 != s3);
          CHECK (s2 != s3);
          
          Literal l1(s1);
          Literal l2(s2);
          Literal l3(s3);
          
          CHECK (l1 == l2);
          CHECK (l1 != l3);
          CHECK (l3 != l1);
          CHECK (l2 != l3);
          CHECK (l3 != l2);
          
          CHECK (l1 == s1);
          CHECK (s1 == l1);
          CHECK (l1 == s2);
          CHECK (s2 == l1);
          CHECK (l1 != s3);
          CHECK (s3 != l1);
          CHECK (not isSameObject(l1, l2));
          CHECK (not isSameObject(l1.c(), l2.c()));
          
          Symbol y1{s1};
          Symbol y2{l2};
          Symbol y3{"11"};
          
          CHECK (y1 == y2);
          CHECK (y1.c() == y2.c());
          CHECK (not isSameObject (y1.c(), y2.c()));
          CHECK (    isSameObject (*y1.c(), *y2.c()));
          CHECK (y1 != y3);
          CHECK (y3 != y1);
          CHECK (y2 != y3);
          CHECK (y3 != y2);
          
          CHECK (y1 == l1);  CHECK (l1 == y1);
          CHECK (y1 == s1);  CHECK (s1 == y1);
          CHECK (y1 == l2);  CHECK (l2 == y1);
          CHECK (y1 == s2);  CHECK (s2 == y1);
          CHECK (y3 != l1);  CHECK (l1 != y3);
          CHECK (y3 != s1);  CHECK (s1 != y3);
          CHECK (y3 != l2);  CHECK (l2 != y3);
          CHECK (y3 != s2);  CHECK (s2 != y3);
        }
      
      
      /** @test use Literal and Symbol as keys in a tree map.
       *  @note neither Literal, nor Symbol defines an `operator<`
       *        so the map specialisation has to fall back on const char*,
       *        which are compared based on pointer identity. Contrast this
       *        with std::string, which does define its own ordering.
       */
      void
      use_as_map_key()
        {
          map<Literal, int> mli;
          map<Symbol, int>  myi;
          map<string, int>  msi;
          
          Literal l1{"1"}, l2{"2"};
          Symbol  y1{l1}, y2{l2};
          string  s1{y1}, s2{"2"};
          
          mli[l1] = 1;  myi[y1] = 1;  msi[s1] = 1;
          mli[l2] = 2;  myi[y2] = 2;  msi[s2] = 2;
          
          CHECK (mli[l1] == 1);
          CHECK (mli[l2] == 2);
          CHECK (myi[y1] == 1);
          CHECK (myi[y2] == 2);
          CHECK (msi[s1] == 1);
          CHECK (msi[s2] == 2);
          
          const char* x = "11";
          ++x;
          CHECK (x != l1.c());
          CHECK (x == l1);
          CHECK (x == y1);
          CHECK (x == s1);
          
          CHECK (mli[x] == 0);  // Note: not found, since it is a different pointer
          CHECK (myi[x] == 1);  // Note: same string mapped to same ptr in Symbol
          CHECK (msi[x] == 1);
        }
    };
  
  LAUNCHER (Symbol_test, "unit common");
  
  
}} // namespace lib::test
