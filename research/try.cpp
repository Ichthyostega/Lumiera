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


/** @file try.cpp
 ** Investigation: pitfalls of "perfect forwarding".
 ** Find out about the corner cases where chained argument forwarding
 ** does not work as expected
 **
 */

#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include <cstddef>
#include <utility>
#include <iostream>
#include <functional>
#include <string>

using std::string;
using std::cout;
using std::endl;

class Interface
  {
  public:
    virtual ~Interface() { }
    virtual string op()  const  =0;
  };

class Impl
  : public Interface
  {
    string s_;
    
    string
    op()  const override
      {
        return s_;
      }
    
  public:
    Impl(string ss ="IMP")
      : s_(ss)
      { }
  };

template<typename X>
void
diagnostics (string id, const void* addr)
{
  cout << id << "\n"
       << "invoked with.. " << lib::test::showType<X>()
       << "\n Address ... " << addr
       << "\n is lRef ... " << std::is_lvalue_reference<X>::value
       << "\n is rRef ... " << std::is_rvalue_reference<X>::value
       << "\n"
       ;
}


void
invoke (Interface const& ref)
{
  using Ty = Interface const&;
  diagnostics<Ty> ("Invoke", &ref);
  cout << "instanceof Impl?" << bool(INSTANCEOF(Impl, &ref)) <<"\n";
  cout <<  ref.op();
}

template<class FUN, typename A>
void
indirect_1 (FUN fun, A&& a)
{
  diagnostics<A> ("Indirect-1", &a);
  fun (std::forward<A> (a));
}

template<class FUN, typename A>
void
indirect_2 (FUN fun, A&& a)
{
  diagnostics<A> ("Indirect-2", &a);
  indirect_1 (fun, std::forward<A> (a));
}


int
main (int, char**)
  {
    Impl obj;
    Interface const& ref = obj;
    
    cout << "before call. Address... "<<&ref<<"\n";
    
    std::function<void(Interface const&)> fun(invoke);
    
    indirect_2 (fun, ref);
    indirect_2 (fun, Impl("honk"));
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
