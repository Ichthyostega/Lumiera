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


#include <nobug.h>
//#include "include/nobugcfg.h"
#include "lib/meta/function-erasure.hpp"

#include <iostream>
//#include <typeinfo>
#include <boost/format.hpp>
#include <cstdlib>


using std::rand;
using std::string;
using std::cout;
using boost::format;


  namespace {
      boost::format fmt ("<%2i>");
      long checksum = 0;
  }
  
  struct TestIt1
    : lumiera::typelist::BoolCheckable<TestIt1>
    {
      
      int val_;
      
      TestIt1 (int v = (rand() % 10))
        : val_(v)
        { }
      
      bool
      isValid()  const
        {
          return true;
        }
      
    };
    
  
  
int 
main (int, char**) //(int argc, char* argv[])
  {
    
    NOBUG_INIT;
   
    TestIt1 testrosteron (22);
    
    bool boo = testrosteron;
    
    cout <<  "\n.gulp.\n";
    
    if (boo)
      cout << "size=" << sizeof(TestIt1) <<"\n";
    
    
    return 0;
  }
