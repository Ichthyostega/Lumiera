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


/** @file try.cpp
 ** Research: perform a partial application of a member function.
 ** The result of this partial application should be a functor expecting the remaining arguments.
 ** The idea was to use this at various library functions expecting a functor or callback, so to
 ** improve readability of the client code: clients could then just pass a member pointer, without
 ** the need to use any tr1::bind expression.
 ** 
 ** \par Costs in code size
 ** While this turned out to be possible, even without much work, just based on the existing
 ** templates for partial functor application (function-closure.hpp), the resulting code size
 ** is rather sobering. Especially in debug mode, quite some overhead is created, which makes
 ** usage of this convenience feature in general purpose library code rather questionable.
 ** When compiling with -O3 though, most of the overhead will be removed
 ** 
 ** The following numbers could be observed:
 ** \code
 **                                  debug / stripped   // debug-O3 / stripped
 ** just using a member pointer:     39013 /  7048            42061 /  7056
 ** using tr1::bind and function:    90375 / 15416            65415 /  9376
 ** partial apply, passing functor: 158727 / 23576            97479 / 11296
 ** partial apply with mem pointer: 119495 / 17816            78031 /  9440
 ** \endcode
 */


#include "lib/meta/tuple.hpp"
#include "lib/meta/function-closure.hpp"

//#include <tr1/functional>
#include <iostream>

using lib::meta::Types;
using lib::meta::Tuple;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
using std::tr1::function;
using std::tr1::bind;

using std::string;
using std::cout;
using std::endl;


namespace lib {
namespace meta{
namespace func{


template<typename SIG, uint num>
struct _PupS
  {
    typedef typename _Fun<SIG>::Ret Ret;
    typedef typename _Fun<SIG>::Args::List Args;
    typedef typename Splice<Args,NullType,num>::Front ArgsFront;
    typedef typename Splice<Args,NullType,num>::Back  ArgsBack;
    typedef typename Types<ArgsFront>::Seq            ArgsToClose;
    typedef typename Types<ArgsBack>::Seq             ArgsRemaining;
    typedef typename _Sig<Ret,ArgsRemaining>::Type    ReducedSignature;
    
    typedef function<ReducedSignature>                Function;
  };

template<typename SIG, typename A1>
inline 
typename _PupS<SIG,1>::Function
papply (SIG f, A1 a1)
{
  typedef typename _PupS<SIG,1>::ArgsToClose ArgsToClose;
  typedef Tuple<ArgsToClose>        ArgTuple;
  ArgTuple val(a1);
  return PApply<SIG,ArgsToClose>::bindFront (f, val);
}

template<typename SIG, typename A1, typename A2>
inline 
typename _PupS<SIG,2>::Function
papply (SIG f, A1 a1, A2 a2)
{
  typedef typename _PupS<SIG,2>::ArgPrefix ArgsToClose;
  typedef Tuple<ArgsToClose>        ArgTuple;
  ArgTuple val(a1,a2);
  return PApply<SIG,ArgsToClose>::bindFront (f, val);
}


}}} // namespace lib::meta::func

class Something
  {
    int i_;
    
    void
    privateFun(char a)
      {
        char aa(a + i_);
        cout << "Char-->" << aa <<endl;
      }
    
  public:
    Something(int ii=0)
      : i_(ii)
      { }
    
    typedef function<void(char)> FunP;
    
    FunP
    getBinding()
      {
//        function<void(Something*,char)> memf = bind (&Something::privateFun, _1, _2);
//        return lib::meta::func::papply (memf, this);
        return lib::meta::func::papply (&Something::privateFun, this);
      }
    
//    typedef void (Something::*FunP) (char);
//    
//    FunP
//    getBinding()
//      {
//        return &Something::privateFun;
//      }
  };





int 
main (int, char**)
  {
    Something some(23);
    Something::FunP fup = some.getBinding();
    
    fup ('a');
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
