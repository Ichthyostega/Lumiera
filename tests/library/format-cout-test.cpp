/*
  FormatCOUT(Test)  -  validate automatic string conversion in output

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file format-cout-test.cpp
 ** unit test \ref FormatCOUT_test
 */


#include "lib/test/run.hpp"

#include "lib/p.hpp"
#include "lib/diff/gen-node.hpp"

#include "lib/meta/util.hpp"
#include "lib/meta/trait.hpp"
#include "lib/format-cout.hpp"

#include <string>

using lib::P;
using lib::makeP;
using lib::diff::GenNode;

using std::string;


namespace util {
namespace test {

  namespace { // test fixture
    
    /** opaque class without string conversion */
    class Reticent 
      {
        uint neigh_ = 42;
      };
    
    
    using lib::meta::is_basically;
    using lib::meta::is_StringLike;
    using lib::meta::can_lexical2string;
    using lib::meta::can_convertToString;
    using lib::meta::use_StringConversion4Stream;
    
    template<typename T>
    using BasicallyString = is_basically<T, string>;
    template<typename T>
    using BasicallyCString = std::is_convertible<T, const char*>;
    
    
#define SHOW_CHECK(_EXPR_) cout << STRINGIFY(_EXPR_) << "\t : " << (_EXPR_::value? "Yes":"No") << endl;
#define ANALYSE(_TYPE_)                     \
        cout << "Type: " STRINGIFY(_TYPE_) " ......"<<endl;   \
        SHOW_CHECK (is_StringLike<_TYPE_>);   \
        SHOW_CHECK (BasicallyString<_TYPE_>);  \
        SHOW_CHECK (BasicallyCString<_TYPE_>);  \
        SHOW_CHECK (std::is_arithmetic<_TYPE_>); \
        SHOW_CHECK (can_lexical2string<_TYPE_>);  \
        SHOW_CHECK (can_convertToString<_TYPE_>);  \
        SHOW_CHECK (use_StringConversion4Stream<_TYPE_>);
    
    
    void
    showTraits()
      {
        using CharLit = typeof("literal");
        using CharPtr = typeof(const char*);
        using StrCRef = typeof(string const&);
        using GenNodePtr = typeof(GenNode*);
        using GenNodeRef = typeof(GenNode&);
        
        ANALYSE (double);
        ANALYSE (string);
        ANALYSE (StrCRef);
        ANALYSE (CharLit);
        ANALYSE (CharPtr)
        ANALYSE (Reticent)
        ANALYSE (P<Reticent>)
        ANALYSE (GenNode)
        ANALYSE (GenNodePtr)
        ANALYSE (GenNodeRef)
        ANALYSE (P<GenNode>)
        cout << endl;
      }
  }//(end)fixture
  
  
  
  
  /***************************************************************************//**
   * @test How to build generic string conversion into `ostream::operator<< `.
   * This task (#985) was actually a conglomerate of several chores:
   * - sanitise and segregate the type-traits usage
   * - disentangle the existing util::str conversion helper
   * - extract a basic form from this helper, which can be placed
   *   into a header with minimal dependencies. After some consideration,
   *   I decided to allow `<typeinfo>` in this category, which allows us
   *   at least to show a type name as fallback
   * - distill an essential version of `enable_if`, which can be inlined.
   *   This allowed us to get rid of `boost::enable_if` finally.
   * - build a sensible `operator string()` for our `lib::P` based on this
   * - and _finally_, to come up with a templated version of the `ostream`
   *   inserter `operator<<`, which does not cause too much havoc when
   *   used by default. The greatest challenge here is to avoid ambiguous
   *   overloads, yet also to deal with references, `void` and arrays.
   * 
   * @see format-cout.hpp
   * @see FormatHelper_test
   */
  class FormatCOUT_test
    : public Test
    {
      void
      run (Arg)
        {
          showTraits();
          
          auto silent = makeP<Reticent>();
          auto chatty = makeP<GenNode>("Hui", "Buh");
          
          cout << "smart-ptr, no string conv..." << silent <<endl;
          cout << "smart-ptr, custom conv......" << chatty <<endl;
          
          cout << "reference, no string conv..." << *silent      <<endl;
          cout << "reference, custom conv......" << *chatty      <<endl;
          cout << "pointer,   custom conv......" << chatty.get() <<endl;
          
          chatty.reset();
          cout << "smart-ptr, NULL pointee....." << chatty       <<endl;
          cout << "pointer,   NULL pointee....." << chatty.get() <<endl;
        }
    };
  
  LAUNCHER (FormatCOUT_test, "unit common");
  
  
}} // namespace util::test

