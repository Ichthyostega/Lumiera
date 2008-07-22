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
      
      
      template
        < class TYPES 
        , class BASE = NullType
        >
      struct Zoing;
      
      
      template<class BASE>
      struct Zoing<NullType, BASE>
        : BASE
        { 
        };
      
      
      template
        < class TY, typename TYPES
        , class BASE
        >
      struct Zoing<Node<TY, TYPES>, BASE> 
        :  TY,
           Zoing<TYPES, BASE>
        { 
        };
      
      
  }
  
  typedef Types< Num<1>
               , Num<2>
               , Num<3>
               >::List List1;
  
  struct B
    { typedef List1 Zeug; };
  struct D : B 
    {};


int 
main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    typedef Zoing<D::Zeug, NullType> Boing;
    
    Boing();
    
    cout <<  "\ngulp\n";
    
    
    return 0;
  }
