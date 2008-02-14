/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs


#include <nobug.h>
#include <iostream>

#include <limits>


using std::string;
using std::cout;

NOBUG_CPP_DEFINE_FLAG(test);


int main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    int64_t lol (1);
    cout << sizeof(lol)<< "\n";
    
    cout << "long: "<< std::numeric_limits<long>::max() 
         <<" 64: " <<  std::numeric_limits<int64_t>::max()
         <<"\n";
    
    return 0;
  }
