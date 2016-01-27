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
using lib::meta::Pick;
using lib::meta::IndexSeq;
using lib::meta::IndexIter;
using lib::meta::BuildIndexSeq;
using lib::meta::InstantiateChained;
using lib::meta::Filter;
using lib::time::TimeVar;
using lib::time::Time;

using std::string;
using std::tuple;

namespace error = lumiera::error;

using std::__not_;
using std::__and_;
using std::__or_;
using std::is_constructible;
using lib::meta::is_narrowingInit;
using lib::meta::Strip;


using DataCapPredicate = Variant<DataValues>::Predicate;


template<typename TAR>
struct GenNodeAccessor
  : boost::noncopyable
  {
    
    template<typename TY>
    struct allow_Conversion
      : __and_<is_constructible<TAR, TY const&>
              ,__not_<is_narrowingInit<typename Strip<TY>::TypePlain
                                      ,typename Strip<TAR>::TypePlain>>
              >
      { };
    
    using SupportedSourceTypes = typename Filter<DataValues::List, allow_Conversion>::List; 
    
    
    
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
    
    
    using ConversionBuffer = InstantiateChained< SupportedSourceTypes
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




template<class SRC, class TAR>
struct ElementExtractor;

template<typename...TYPES>
struct ElementExtractor<Rec, tuple<TYPES...>>
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






template< typename TYPES
        , template<class,class, size_t> class _ElmMapper_
        , class SEQ
        >
struct TupleConstructor;

template< typename TYPES
        , template<class,class, size_t> class _ElmMapper_
        , size_t...idx
        >
struct TupleConstructor<TYPES, _ElmMapper_, IndexSeq<idx...>>
  : Tuple<TYPES>
  {
    
  public:
    template<class SRC>
    TupleConstructor (SRC values)
      : Tuple<TYPES> (_ElmMapper_<SRC, Tuple<TYPES>, idx>{values}...)
      { }
  };


template<class SRC, class TAR, size_t i>
using PickArg = typename ElementExtractor<SRC, TAR>::template Access<i>;


template<typename TYPES, class SRC>
Tuple<TYPES>
buildTuple (SRC values)
{
  using IndexSeq = typename IndexIter<TYPES>::Seq;
  
  return TupleConstructor<TYPES, PickArg, IndexSeq> (values);
}


////////////////////////TODO reworked for function-closure.hpp

    template<typename SRC, typename TAR, size_t start>
    struct PartiallyInitTuple
      {
        template<size_t i>
        using DestType = typename std::tuple_element<i, TAR>::type;
        
        
        /**
         * define those index positions in the target tuple,
         * where init arguments shall be used on construction.
         * All other arguments will just be default initialised.
         */
        static constexpr bool
        useArg (size_t idx)
          {
            return (start <= idx)
               and (idx < start + std::tuple_size<SRC>());
          }
        
        
        
        template<size_t idx,   bool doPick = PartiallyInitTuple::useArg(idx)>
        struct IndexMapper
          {
            SRC const& initArgs;
            
            operator DestType<idx>()
              {
                return std::get<idx-start> (initArgs);
              }
          };
        
        template<size_t idx>
        struct IndexMapper<idx, false>
          {
            SRC const& initArgs;
            
            operator DestType<idx>()
              {
                return DestType<idx>();
              }
          };
      };


template<typename TYPES, typename ARGS, size_t start>
struct SomeArgs
  {
    
    template<class SRC, class TAR, size_t i>
    using IdxSelector = typename PartiallyInitTuple<SRC, TAR, start>::template IndexMapper<i>;
    
    static Tuple<TYPES>
    doIt (Tuple<ARGS> const& args)
    {
       using IndexSeq = typename IndexIter<TYPES>::Seq;
       
       return TupleConstructor<TYPES, IdxSelector, IndexSeq> (args);
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
    
    cout << SomeArgs<UgglyTypes,NiceTypes,1>::doIt(std::make_tuple("hui", 88)) <<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
