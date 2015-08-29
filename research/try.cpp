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


/** @file try.cpp
 ** Design: how to generalise the Variant::Visitor to arbitrary return values.
 ** 
 ** Our Variant template allows either for access by known type, or through accepting
 ** a classic GoF visitor. Problem is that in many extended use cases we rather want
 ** to apply \em functions, e.g. for a monadic flatMap on a data structure built from
 ** Variant records. (see our \link diff::GenNode external object representation \endlink).
 ** Since our implementation technique relies on a template generated interface anyway,
 ** a mere extension to arbitrary return values seems feasible.
 **
 */

typedef unsigned int uint;

#include "lib/meta/typelist.hpp"
#include "lib/meta/generator.hpp"
#include "lib/format-util.hpp"

//#include <functional>
#include <iostream>
#include <cstdarg>
#include <string>

using std::string;
using std::cout;
using std::endl;

    
    
    template<class VAL>
    struct ValueAcceptInterface
      {
        virtual void handle(VAL&) { /* NOP */ };
      };
    
    template<typename TYPES>
    using VisitorInterface
        = lib::meta::InstantiateForEach<typename TYPES::List, ValueAcceptInterface>;


template<class A, class B>
struct Var
  {
    A a;
    B b;
    
    using TYPES = lib::meta::Types<A,B>;
    using Visitor = VisitorInterface<TYPES>;
    
    void
    accept (Visitor& visitor)
      {
        ValueAcceptInterface<A>& visA = visitor;
        ValueAcceptInterface<B>& visB = visitor;
        visA.handle (a);
        visB.handle (b);
      }

    
    operator string()  const
      {
        return "Var("
             + util::str(a)
             + "|"
             + util::str(b)
             + ")";
      }
  };


class Visi
  : public Var<int,string>::Visitor
  {
    
    virtual void handle(int& i) { ++i; }
    virtual void handle(string& s) { s += ".."; }
  };


int
main (int, char**)
  {
    using V = Var<int, string>;
    
    V var{12, "hui"};
    cout <<  string(var)<<endl;
    
    Visi visi;
    var.accept(visi);
    cout <<  string(var)<<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
