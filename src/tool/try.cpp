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


#include <nobug.h>
#define LUMIERA_LOGGING_CXX
#include "include/logging.h"
//#include "include/nobugcfg.h"

#include <tr1/functional>

#include <iostream>
//#include <typeinfo>
#include <vector>
#include <cstdlib>
#include <cstdio>


//using std::tr1::bind;
using std::tr1::placeholders::_1;

using std::rand;
using std::string;
using std::cout;


  
  template <typename SEQ, typename Oper>
  inline bool
  eat_all (SEQ& coll, Oper predicate)
  {
    typename SEQ::const_iterator e = coll.end();
    typename SEQ::const_iterator i = coll.begin();
    
    predicate(*i);
    
//    for ( ; i!=e; ++i )
//      if (!predicate(*i))
//        return false;
    
    return true;
  }
  
//  template < typename CON, typename FUN
//           , typename P1
//           , typename P2
//           >
//  inline bool
//  eat_all (CON& elements, FUN function, P1 bind1, P2 bind2)
//  {
//    return eat_all (elements, std::tr1::bind (function, bind1, bind2));
//  }
  
  
  namespace {
//    std::tr1::_Placeholder<1>        _1;
    
    bool
    plainFunc (int i, int j)
    {
      cout <<':'<< i+j;
      return i+j;
    }
  }
  
  
int 
main (int, char**)
  {
    
    NOBUG_INIT;
    
    typedef std::vector<int> VecI;
    
    uint count = 4;
    VecI numberz;
    while (count)
      numberz.push_back(count--);
    
//  eat_all (numberz, plainFunc, 10, _1 );
    eat_all (numberz, std::tr1::bind<bool> (plainFunc, 10, _1));
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
