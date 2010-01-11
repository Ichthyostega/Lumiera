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


//#include <nobug.h>
//#define LUMIERA_LOGGING_CXX
//#include "include/logging.h"
//#include "include/nobugcfg.h"


#include <iostream>
//#include <typeinfo>
#include <vector>
#include <cstdlib>
#include <cstdio>


//using std::rand;
using std::string;
using std::cout;
using std::endl;


struct A
  {
    int& funA ();
  };

struct B
  {
    void funA();
  };


  typedef char Yes_t;
  struct No_t { char padding[8]; };


  template<typename TY>
  class Detector1
    {
      template<typename X, int i = sizeof(&X::funA)>
      struct Probe
        { };
      
      template<class X>
      static Yes_t check(Probe<X>*);
      template<class>
      static No_t  check(...);
      
    public: 
      static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));
    };
  
  
  template<typename TY>
  class Detector2
    {
      template<typename X, int& (X::*)(void)> 
      struct Probe
        { };
      
      template<class X>
      static Yes_t check(Probe<X,&X::funA>*);
      template<class>
      static No_t  check(...);
      
    public: 
      static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));
    };
  
  
int 
main (int, char**)
  {
    
//  NOBUG_INIT;
    
    cout << "Detector1<A> = " << Detector1<A>::value << endl;
    cout << "Detector1<B> = " << Detector1<B>::value << endl;
    
    cout << "Detector2<A> = " << Detector2<A>::value << endl;
    cout << "Detector2<B> = " << Detector2<B>::value << endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
