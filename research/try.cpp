/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>
// 4/08  - conversions on the value_type used for boost::any
// 5/08  - how to guard a downcasting access, so it is compiled in only if the involved types are convertible
// 7/08  - combining partial specialisation and subclasses 
// 10/8  - abusing the STL containers to hold noncopyable values
// 6/09  - investigating how to build a mixin template providing an operator bool()
// 12/9  - tracking down a strange "warning: type qualifiers ignored on function return type"
// 1/10  - can we determine at compile time the presence of a certain function (for duck-typing)?
// 4/10  - pretty printing STL containers with python enabled GDB?
// 1/11  - exploring numeric limits
// 1/11  - integer floor and wrap operation(s)
// 1/11  - how to fetch the path of the own executable -- at least under Linux?
// 10/11 - simple demo using a pointer and a struct
// 11/11 - using the boost random number generator(s)
// 12/11 - how to detect if string conversion is possible?
// 1/12  - is partial application of member functions possible?
// 5/14  - c++11 transition: detect empty function object
// 7/14  - c++11 transition: std hash function vs. boost hash
// 9/14  - variadic templates and perfect forwarding
// 11/14 - pointer to member functions and name mangling
// 8/15  - Segfault when loading into GDB (on Debian/Jessie 64bit
// 8/15  - generalising the Variant::Visitor
// 1/16  - generic to-string conversion for ostream
// 1/16  - build tuple from runtime-typed variant container


/** @file try.cpp
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



int
main (int, char**)
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
    
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
