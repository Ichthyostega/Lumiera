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
#include "lib/util.hpp"

#include <iostream>
#include <cstdarg>
#include <string>

using util::unConst;
using std::string;
using std::cout;
using std::endl;


template<typename RET>
struct VFunc
  {

    template<class VAL>
    struct ValueAcceptInterface
      {
        virtual RET handle(VAL&) { /* do nothing */ return RET(); };
      };
    
    template<typename TYPES>
    using VisitorInterface
        = lib::meta::InstantiateForEach<typename TYPES::List, ValueAcceptInterface>;
    
  };

using lib::meta::NullType;
using lib::meta::Node;

template<typename TYPES>
struct ConstAll;

template<>
struct ConstAll<NullType>
  {
    typedef NullType List;
  };

template<typename TY, typename TYPES>
struct ConstAll<Node<TY,TYPES>>
  {
    typedef Node<const TY, typename ConstAll<TYPES>::List> List;
  };



template<class A, class B>
struct Var
  {
    A a;
    B b;
    
    using TYPES = lib::meta::Types<A,B>;
    
    template<typename RET>
    using VisitorFunc      = typename VFunc<RET>::template VisitorInterface<TYPES>;
    template<typename RET>
    using VisitorConstFunc = typename VFunc<RET>::template VisitorInterface<ConstAll<typename TYPES::List>>;
    
    using Visitor = VisitorFunc<void>;
    using Predicate = VisitorConstFunc<bool>;
    
    template<typename RET>
    RET
    accept (VisitorFunc<RET>& visitor)
      {
        typename VFunc<RET>::template ValueAcceptInterface<A>& visA = visitor;
        typename VFunc<RET>::template ValueAcceptInterface<B>& visB = visitor;
        visA.handle (a);
        return visB.handle (b);
      }
    
    void
    accept (Visitor& visitor)
      {
        accept<void> (visitor);
      }
    
    bool
    accept (Predicate& visitor)  const
      {
        typename VFunc<bool>::template ValueAcceptInterface<const A>& visA = visitor;
        typename VFunc<bool>::template ValueAcceptInterface<const B>& visB = visitor;
        return visA.handle (a)
            && visB.handle (b);
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


using V = Var<int, string>;

class Visi
  : public V::Visitor
  {
    virtual void handle(int& i)    { ++i; }
    virtual void handle(string& s) { s += "."; }
  };

class Predi
  : public V::Predicate
  {
    virtual bool handle(int const& i)    { return 0 == i % 2; }
    virtual bool handle(string const& s) { return 0 == s.length() % 2; }
  };


int
main (int, char**)
  {
    
    V var{12, "huii"};
    cout <<  string(var)<<endl;
    
    Visi visi;
    Predi predi;
    
    cout << var.accept(predi) <<endl;
    var.accept(visi);
    cout << var.accept(predi) <<endl;
    cout <<  string(var)<<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
