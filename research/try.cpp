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
#include "lib/diff/gen-node.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/meta/generator.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/tuple-helper.hpp"
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
using lib::diff::DataValues;
using lib::meta::Types;
using lib::meta::Tuple;
using lib::meta::IndexSeq;
using lib::meta::BuildIndexSeq;
using lib::meta::InstantiateChained;
using lib::meta::Filter;
using lib::time::TimeVar;
using lib::time::Time;

using std::string;
using std::tuple;

namespace error = lumiera::error;


using DataCapPredicate = Variant<DataValues>::Predicate;


template<typename TAR>
class GenNodeAccessor
  : boost::noncopyable
  {
    struct ConverterBase
      : DataCapPredicate
      {
        char buffer[sizeof(TAR)];
      };
    
    template<typename TY, class BA>
    class Converter
      : public BA
      {
        virtual bool
        handle(TY const& srcElm)
          {
            new(&(BA::buffer)) TAR{srcElm};
            return true;
          };
      };
    
    template<typename TY>
    using can_Convert = std::is_constructible<TAR, TY const&>;
    
    using PossibleSourceTypes = typename Filter<DataValues::List, can_Convert>::List; 
    
    using ConversionBuffer = InstantiateChained< PossibleSourceTypes
                                               , Converter
                                               , ConverterBase
                                               >;
    
    ConversionBuffer converter_;
    
  public:
    GenNodeAccessor (GenNode const& node)
      : converter_()
      {
        if (not node.data.accept (converter_))
            throw error::Invalid ("Unable to build «" + util::typeStr<TAR>()
                                 +"» element from " + string(node));
      }
    
    operator TAR ()
      {
        return *reinterpret_cast<TAR*> (&converter_.buffer);
      }
  };


template<class, size_t>
struct Pick;

template<typename...TYPES, size_t i>
struct Pick<Types<TYPES...>, i>
  {
    using Type = typename lib::meta::Shifted<Types<TYPES...>, i>::Head;
  };



template<class SRC, class TAR>
struct ElementMapper;

template<typename...TYPES>
struct ElementMapper<Rec, Types<TYPES...>>
  {
    template<size_t i>
    using TargetType = typename Pick<Types<TYPES...>, i>::Type;
    
    
    template<size_t i>
    struct Access
      {
        Rec const& values;
        
        operator TargetType<i> ()
          {
            return GenNodeAccessor<TargetType<i>>(values.child(i));
          }

      };
    
  };



template<class REF>
struct IdxIter;

template<typename...TYPES>
struct IdxIter<Types<TYPES...>>
  {
    /////TODO as long as Types is not variadic (#987), we need to strip NullType here (instead of just using sizeof...(TYPES)
    enum {SIZ = lib::meta::count<typename Types<TYPES...>::List>::value };
    
    using Seq = typename BuildIndexSeq<SIZ>::Ascending;
  };



template<typename TYPES, class SEQ>
class TupleBuilder;

template<typename TYPES, size_t...idx>
class TupleBuilder<TYPES, IndexSeq<idx...>>
  : public Tuple<TYPES>
  {
    template<class SRC, size_t i>
    using PickArg = typename ElementMapper<SRC, TYPES>::template Access<i>;
    
    
  public:
    template<class SRC>
    TupleBuilder (SRC values)
      : Tuple<TYPES> (PickArg<SRC, idx>{values}...)
      { }
  };
  


template<typename TYPES, class SRC>
Tuple<TYPES>
buildTuple (SRC values)
{
  using IndexSeq = typename IdxIter<TYPES>::Seq;
  
  return TupleBuilder<TYPES, IndexSeq> (values);
}





#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;


int
main (int, char**)
  {
    using NiceTypes = Types<string, int>;
    using UgglyTypes = Types<EntryID<long>, string, short, long, float, TimeVar>;
    
    
    Rec args = MakeRec().scope("lalü", 42);
    Rec urgs = MakeRec().scope("lalü", "lala", 12, 34, 5.6, Time(7,8,9));

    cout << args <<endl;
    cout << urgs <<endl;
    
    cout << buildTuple<NiceTypes> (args) <<endl;
    cout << buildTuple<UgglyTypes> (urgs) <<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
