/*
  TupleRecordInit(Test)  -  to build a tuple from a GenNode sequence

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


/** @file tuple-record-init-test.cpp
 ** Metaprogramming: how to unload the contents of a runtime typed variant sequence
 ** into ctor arguments of a (compile time typed) tuple. This involves two problems
 ** - how to combine iteration, compile-time indexing and run-time access.
 ** - how to overcome the runtime-to-compiletime barrier, using a pre-generated
 **   double-dispatch (visitor).
 ** 
 ** The concrete problem prompting this research is the necessity to receive
 ** a command invocation parameter tuple from a Record<GenNode>
 ** 
 */

typedef unsigned int uint;

#include "lib/test/run.hpp"
#include "lib/symbol.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/meta/tuple-record-init.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"

#include <boost/noncopyable.hpp>
#include <string>

using lib::Literal;
using lib::Variant;
using lib::idi::EntryID;
using lib::diff::Rec;
using lib::diff::MakeRec;
using lib::diff::GenNode;
using lib::meta::Types;
using lib::meta::Tuple;
using lib::meta::buildTuple;
using lib::time::TimeVar;
using lib::time::Time;

using std::string;
using std::tuple;

namespace lib  {
namespace meta {
namespace test {

////////////////////////TODO reworked for function-closure.hpp
template<typename TYPES, typename ARGS, size_t start>
struct SomeArgs
  {
    
    template<class SRC, class TAR, size_t i>
    using IdxSelector = typename lib::meta::func::PartiallyInitTuple<SRC, TAR, start>::template IndexMapper<i>;
    
    static Tuple<TYPES>
    doIt (Tuple<ARGS> const& args)
    {
       return lib::meta::TupleConstructor<TYPES, IdxSelector> (args);
    }
  };
////////////////////////TODO reworked for function-closure.hpp




#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;

#define EVAL_PREDICATE(_PRED_) \
    cout << STRINGIFY(_PRED_) << "\t : " << _PRED_ <<endl;

void
verifyConversions()
  {
    using lib::meta::GenNodeAccessor;
    using std::is_arithmetic;
    using std::is_floating_point;
    using lib::meta::is_nonFloat;
    using lib::hash::LuidH;
    
    
    EVAL_PREDICATE(is_arithmetic<int>       ::value)
    EVAL_PREDICATE(is_arithmetic<size_t>    ::value)
    EVAL_PREDICATE(is_floating_point<size_t>::value)
    EVAL_PREDICATE(is_nonFloat<size_t>      ::value)
    
    EVAL_PREDICATE(GenNodeAccessor<int>  ::allow_Conversion<size_t>    ::value)
    EVAL_PREDICATE(GenNodeAccessor<int64_t>::allow_Conversion<long int>::value)
    EVAL_PREDICATE(GenNodeAccessor<double>::allow_Conversion<int64_t>::value)
    EVAL_PREDICATE(GenNodeAccessor<LuidH>::allow_Conversion<int64_t> ::value)
    EVAL_PREDICATE(GenNodeAccessor<LuidH>::allow_Conversion<int16_t> ::value)
    EVAL_PREDICATE(GenNodeAccessor<LuidH>::allow_Conversion<uint16_t>::value)
    EVAL_PREDICATE(GenNodeAccessor<LuidH> ::allow_Conversion<LuidH>  ::value)
    EVAL_PREDICATE(GenNodeAccessor<int64_t> ::allow_Conversion<LuidH>::value)
    EVAL_PREDICATE(GenNodeAccessor<uint64_t>::allow_Conversion<LuidH>::value)
    EVAL_PREDICATE(GenNodeAccessor<uint32_t>::allow_Conversion<LuidH>::value)
    EVAL_PREDICATE(GenNodeAccessor<int32_t> ::allow_Conversion<LuidH>::value)
    
    cout <<endl<<endl;
  }



  class TupleRecordInit_test : public Test
    {
      virtual void
      run (Arg)
  {
    verifyConversions();
    
    using NiceTypes = Types<string, int>;
    using UgglyTypes = Types<EntryID<long>, string, int, int64_t, double, TimeVar>;
    
    Rec args = MakeRec().scope("lalü", 42);
    Rec urgs = MakeRec().scope("lalü", "lala", 12, 34, 5.6, Time(7,8,9));
    
    cout << args <<endl;
    cout << urgs <<endl;
    
    cout << buildTuple<NiceTypes> (args) <<endl;
    cout << buildTuple<UgglyTypes> (urgs) <<endl;
    
    cout << SomeArgs<UgglyTypes,NiceTypes,1>::doIt(std::make_tuple("hui", 88)) <<endl;
    
    cout <<  "\n.gulp.\n";
    
  }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TupleRecordInit_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
