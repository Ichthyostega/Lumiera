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


#include <nobug.h>
#include <iostream>
#include <typeinfo>
#include <boost/format.hpp>

using std::string;
using std::cout;
using std::ostream;
using boost::format;


#include <boost/utility/enable_if.hpp>
using boost::enable_if;
#include <boost/type_traits/is_base_of.hpp>
using boost::is_base_of;
#include <boost/type_traits/is_abstract.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
  
  
#include "common/typelistutil.hpp"
using lumiera::typelist::NullType;
using lumiera::typelist::Node;
using lumiera::typelist::Types;
  
  
  namespace {
      
      boost::format fmt ("<%2i>");
      
      /** constant-wrapper type for debugging purposes,
       *  usable for generating lists of distinghishable types
       */
      template<int I>
      struct Num
        {
          enum{ VAL=I };
          static string str () { return boost::str (fmt % I); }
          Num()
            {
              cout << Num::str();
            }
        };
      
      
    
    template<template<class> class _CandidateTemplate_>
    class Instantiation
      {
        template<class ARG>
        struct If_possibleArgument : _CandidateTemplate_<ARG>
          {
            typedef void Type;
          };
        
      public:
          
        template<class X, class TEST=void>
        struct Test
          : boost::false_type {};
        
        template<class X>
        struct Test<X, typename If_possibleArgument<X>::Type >
          : boost::true_type {};
        
      };
      
  }
  
  struct Boing { typedef boost::true_type is_defined; };
  
  template<int>  struct Zoing ;
  
  template<>     struct Zoing<2> : Boing { enum{wau = 2}; };
  template<>     struct Zoing<5> : Boing { enum{wau = 5}; };

  typedef char yes_type;
  struct no_type
  {
     char padding[8];
  };

  template<class U>
  yes_type check(typename U::is_defined *);
  template<class U>
  no_type check(...);

  
int 
main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    typedef Zoing<2> Z2;
    typedef Zoing<3> Z3;
    typedef Zoing<5> Z5;
  
    cout << sizeof(check<Z2>(0)) << " / "
         << sizeof(check<Z3>(0)) << " / "
         << sizeof(check<Z5>(0)) ;
    
    
    cout <<  "\ngulp\n";
    
    
    return 0;
  }
