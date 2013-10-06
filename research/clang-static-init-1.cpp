
#include "clang-static-init.hpp"



int
main (int, char**)
  {
    cout <<  "\nStart Testcase: invoking two instances of the configurable singleton factory...\n";
    
    test::Subject& ref1 = test::fab.get();
    test::Subject& sub2 = test::fabricate();
    
    cout << "sub1="<< &ref1 << " sub2="<< &sub2 <<"\n";
    
    
    return 0;
  }
