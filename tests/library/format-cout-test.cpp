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


#include "lib/test/run.hpp"
/** @file try.cpp
 ** How to build generic string conversion into `ostream::operator<< `.
 ** 
 ** This task is actually a conglomerate of several chores:
 ** - sanitise and segregate the type-traits usage
 ** - disentangle the existing toString conversion helper
 ** - extract a basic form from this helper, which can be placed
 **   into a header with minimal dependencies. After some consideration,
 **   I decided to allow `<typeinfo>` in this category, which allows us
 **   at least to show a type name as fallback
 ** - distill an essential version of `enable_if`, which can be inlined.
 **   This allows us to get rid of `boost::enable_if` finally.
 ** - build a sensible `operator string()` for our `lib::P` based on this
 ** - and _finally_, to come up with a templated version of the `ostream`
 **   inserter `operator<<`, which does not cause too much havoc when
 **   used by default. The greatest challenge here is to avoid ambiguous
 **   overloads, yet also to deal with references, `void` and arrays.
 **
 ** \par policy
 ** What shall be expected from such a generic toString conversion?
 ** It should be _minimal_, it should be _transparent_ and it should
 ** always work and deliver a string, irrespective of the circumstances.
 ** By extension, this means that we do not want to differentiate much
 ** between values, references and pointers, which also means, we do
 ** not want to indicate pointers explicitly (just signal NULL, when
 ** encountered). The situation is slightly different for the `ostream`
 ** inserter; in a modern GUI application, there isn't much use for
 ** STDOUT and STDERR, beyond error messages and unit testing.
 ** Thus, we can strive at building a more convenient flavour
 ** here, which does indeed even shows the address of pointers.
 ** 
 */

typedef unsigned int uint;

#include "lib/p.hpp"
#include "lib/diff/gen-node.hpp"

#include "lib/meta/util.hpp"
#include "lib/meta/trait.hpp"
#include "lib/format-cout.hpp"

#include <string>

using lib::P;
using lib::newP;
using lib::diff::GenNode;
using lib::meta::is_basically;
using lib::meta::is_StringLike;
using lib::meta::can_lexical2string;
using lib::meta::can_convertToString;
using lib::meta::use_StringConversion4Stream;

using std::string;







namespace util {
namespace test {
  
  class Reticent 
  {
    uint neigh_ = 42;
  };



template<typename T>
using BasicallyString = is_basically<T, string>;
template<typename T>
using BasicallyChar = std::is_convertible<T, const char*>;


void
showTypes()
  {
    
#define SHOW_CHECK(_EXPR_) cout << STRINGIFY(_EXPR_) << "\t : " << (_EXPR_::value? "Yes":"No") << endl;
#define ANALYSE(_TYPE_)                 \
    cout << "Type: " STRINGIFY(_TYPE_) " ......"<<endl;   \
    SHOW_CHECK (is_StringLike<_TYPE_>);   \
    SHOW_CHECK (BasicallyChar<_TYPE_>);    \
    SHOW_CHECK (BasicallyString<_TYPE_>);   \
    SHOW_CHECK (std::is_arithmetic<_TYPE_>); \
    SHOW_CHECK (can_lexical2string<_TYPE_>);  \
    SHOW_CHECK (can_convertToString<_TYPE_>);  \
    SHOW_CHECK (use_StringConversion4Stream<_TYPE_>);
    
    
    using CharLit = typeof("bla");
    using CharPtr = typeof(const char*);
    using GenNodePtr = typeof(GenNode*);
    using GenNodeRef = typeof(GenNode&);
    
    ANALYSE (string);
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



  /***************************************************************************//**
   * @test verifies the proper working of some string-formatting helper functions.
   *       - util::toString() provides a failsafe to-String conversion, preferring
   *         an built-in conversion, falling back to just a type string.
   *       - util::join() combines elements from arbitrary containers or iterators
   *         into a string, relyint on aforementioned generic string conversion
   * @see format-util.hpp
   */
  class FormatCOUT_test
    : public Test
    {
      void
      run (Arg)
  {
    showTypes();
    
    auto psss = newP<Reticent>();
    auto gnng = newP<GenNode>("Hui", "Buh");
    
    cout << "mauu..." << psss <<endl;
    cout << "wauu..." << gnng <<endl;
    
    cout << "mauuu.." << *psss <<endl;
    cout << "wauuu.." << *gnng <<endl;
    cout << "wauuup." << gnng.get() <<endl;
    
    gnng.reset();
    cout << "aauu..." << gnng       <<endl;
    cout << "aauu..." << gnng.get() <<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
    };
  
  LAUNCHER (FormatCOUT_test, "unit common");
  
  
}} // namespace util::test

