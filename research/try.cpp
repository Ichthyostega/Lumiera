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
// 3/17  - generic function signature traits, including support for Lambdas
// 9/17  - manipulate variadic templates to treat varargs in several chunks
// 11/17 - metaprogramming to detect the presence of extension points
// 11/17 - detect generic lambda
// 12/17 - investigate SFINAE failure. Reason was indirect use while in template instantiation
// 03/18 - Dependency Injection / Singleton initialisation / double checked locking
// 04/18 - investigate construction of static template members
// 08/18 - Segfault when compiling some regular expressions for EventLog search
// 10/18 - investigate insidious reinterpret cast
// 12/18 - investigate the trinomial random number algorithm from the C standard lib
// 04/19 - forwarding tuple element(s) to function invocation


/** @file try.cpp
 * Research how to apply a tuple to a varargs function forwarder.
 * The recent standard library has a std::apply, which we can not yet use, unfortunately.
 * @note this research remains inconclusive. As far as I can see, the simplified setup
 *       exactly mimics the problematic call situation; however, in the real use case,
 *       we need to std::forward<Args> the argument tuple object field while here in
 *       this simplified case, it compiles just fine without -- as it should after all,
 *       since that is the whole point of perfect forwarding; std::get should expose
 *       a LValue reference to the tuple element, and we pass that through a forwarding
 *       function into the double dispatch to the receiving visitor.
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"
#include "lib/verb-visitor.hpp"

#include "lib/meta/variadic-helper.hpp"

#include <utility>
#include <string>
#include <tuple>

using lib::Literal;
using std::string;
using std::tuple;



#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;

template<typename FUN, typename...ARGS>
void
forwardInvoker (FUN& fun, ARGS&&... args)
{
  cout << "forwardInvoker...\n"
       << lib::test::showVariadicTypes(args...)
       << endl;
  fun (std::forward<ARGS>(args)...);
}

template<typename FUN, typename...ARGS>
struct Holder
  {
    using Args = tuple<ARGS...>;
    
    Args tup;
    
    Holder (Args& tup)
      : tup{tup}
      { }
    
template<size_t...idx>
void
unpack_and_forward (FUN& fun, lib::meta::IndexSeq<idx...>)
{
  cout << "unpack_and_forward...\n";
  SHOW_TYPE (Args)
  
  forwardInvoker (fun, std::get<idx> (tup)...);
}

void
applyTuple (FUN& fun)
{
  cout << "applyTuple...\n";
  SHOW_TYPE (Args)
  
  using SequenceIterator = typename lib::meta::BuildIdxIter<ARGS...>::Ascending;
  
  unpack_and_forward (fun, SequenceIterator());
}
  };



  ///////////////////////////TODO : Debugging
  struct Trackr
    {
      size_t num;
      
      Trackr (size_t val)
        : num(val)
        {
          cout <<"Trackr("<<val<<")"<<endl;
        }
     ~Trackr()
        {
          cout <<"~Trackr("<<num<<")"<<endl;
        }
      Trackr (Trackr const& lval)
        : num(lval.num)
        {
          cout <<"Trackr()<<-LVal"<<endl;
        }
      Trackr (Trackr && rval)
        : num(rval.num)
        {
          cout <<"Trackr()<<-RVal"<<endl;
        }
      Trackr&
      operator= (Trackr const& orig)
        {
          cout <<"Tracker = orig"<<endl;
          num = orig.num;
          return *this;
        }
    };
  ///////////////////////////TODO : Debugging

  struct Receiver
    {
      void
      grrrn (uint& x, Trackr y)
        {
          cout <<"grrrn()..."<< x<<"*Trckr("<<y.num<<")="<<(x*y.num)<<endl;
        }
    };
  
  template<class REC, class SIG>
  struct Hodler;
  
  template<class REC, class RET, typename... ARGS>
  struct Hodler<REC, RET(ARGS...)>
    {
      typedef RET (REC::*Handler) (ARGS...);
      
      Handler handler_;
      using Verb = lib::VerbToken<REC,RET(ARGS...)>;
      using Args = std::tuple<ARGS...>;
      
      /** meta-sequence to pick argument values from the storage tuple */
      using SequenceIterator = typename lib::meta::BuildIdxIter<ARGS...>::Ascending;
      
      Verb verb_;
      Args args_;
      
//    Hodler (typename Verb::Handler handlerRef, Literal verbID, ARGS&&... args)
      Hodler (Handler handlerRef, Literal verbID, ARGS&&... args)
        : handler_{handlerRef} 
        , verb_{handlerRef, verbID}
        , args_{std::forward<ARGS> (args)...}
        { }
      
      RET
      applyTo (REC& receiver)
        {
          return invokeVerb (receiver, SequenceIterator());
        }
      
      template<size_t...idx>
      RET
      invokeVerb (REC& receiver, lib::meta::IndexSeq<idx...>)
        {                                                //////////////////////////////////////////TICKET #1006 | TICKET #1184 why do we need std::forward here? the target is a "perfect forwarding" function, which should be able to receive a LValue reference to the tuple element just fine...
//          lib::test::TypeDebugger<Args> buggy;
//          return verb_.applyTo (receiver, std::get<idx> (std::forward<Args>(args_))...);  /// <<------------this compiles, but consumes the tuple's content (move init)
//          return verb_.applyTo (receiver, std::get<idx> (args_)...);
//          return (receiver.*handler_)(std::get<idx> (args_)...);                          /// <<------------this works
//          return applyToVerb (receiver, std::get<idx> (args_)...);
//          return getVerbFun(receiver) (std::get<idx> (args_)...);                         /// <<------------this compiles, but creates a spurious copy
          return verb_.applyTo (receiver, forwardElm<idx> (args_)...);                    /// <<------------this compiles, but consumes the tuple's content (move init)
        }

      template<size_t idx>
      using TupleElmType = typename std::tuple_element<idx, Args>::type;
      
      template<size_t idx>
//      std::remove_reference_t<decltype(std::get<idx> (args))>&&
      TupleElmType<idx>&&      
      forwardElm (Args& args)
        {
          using ElmRef = decltype(std::get<idx> (args));
          using Elm = std::remove_reference_t<TupleElmType<idx>>;
          
          return std::forward<TupleElmType<idx>> (std::get<idx> (args));
        }
      
      RET
      applyToVerb (REC& receiver, ARGS&& ...args)
        {
//        REQUIRE ("NIL" != token_);
          return (receiver.*handler_)(std::forward<ARGS>(args)...);
        }
      
//      std::function<RET(ARGS...)>
      decltype(auto)
      getVerbFun(REC& receiver)
        {
          return [&](ARGS...args) -> RET
                    {
                      return (receiver.*handler_)(std::forward<ARGS>(args)...);
                    };
        }
    };

using lib::meta::Yes_t;
using lib::meta::No_t;

META_DETECT_FUNCTION(void, cloneInto, (void*) const);
META_DETECT_FUNCTION_NAME(cloneInto);


int
main (int, char**)
  {
    auto tup = std::make_tuple(1,2,3);
    auto fun = [](int a, int b, int c)
                  {
                    cout << a<<"+"<<b<<"+"<<c<<"="<<(a+b+c)<<endl;
                  };
    
    using Hol = Holder<decltype(fun), int, int, int>;
    Hol holder(tup);
    holder.applyTuple (fun);
    
    
    uint zwo{2};
    std::tuple<uint&, Trackr> trp{zwo,Trackr(3)};
    auto frn = [](uint& x, Trackr y)
                  {
                    cout << x<<"*Trckr("<<y.num<<")="<<(x*y.num)<<endl;
                  };
    using Hrl = Holder<decltype(frn), uint&, Trackr>;
    Hrl hrlder(trp);
    hrlder.applyTuple (frn);
    cout <<  "\n.ulps.\n";
    
    Hodler<Receiver, void(uint&,Trackr)> holyh(&Receiver::grrrn, "holyhandgrenade", zwo, Trackr(5));
    Receiver recy;
//    recy.grrrn (std::get<0>(trp), Trackr(5));
    holyh.applyTo (recy);
    
    cout << "---Types---"<<endl;
    using BussI = lib::VerbInvoker<Receiver, void>;
    using CloneI = lib::polyvalue::CloneValueSupport<lib::polyvalue::EmptyBase>;
    SHOW_TYPE(BussI);
    SHOW_EXPR (lib::polyvalue::exposes_CloneFunction<BussI>::value)
    SHOW_EXPR (HasFunSig_cloneInto<BussI>::value)
    SHOW_EXPR (HasFunSig_cloneInto<CloneI>::value)
    SHOW_EXPR (HasFunName_cloneInto<BussI>::value)
    SHOW_EXPR (HasFunName_cloneInto<CloneI>::value)
    using BussP = decltype(&BussI::cloneInto);
    SHOW_TYPE(BussP)
    
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
