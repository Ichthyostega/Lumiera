
#include "clang-static-init.hpp"


test::Subject&
localFunction()
{
  return test::fab.get();
}


int
main (int, char**)
  {
    cout <<  "\nStart Testcase: invoking two instances of the configurable singleton factory...\n\n";
    
    test::Subject& ref1 = test::fab.get();
    test::Subject& sub2 = test::fabricate();  ///NOTE: invoking get() from within another compilation unit reveales the problem
    test::Subject& sub3 = localFunction();
    
    cout << "sub1="  << &ref1
         << "\nsub2="<< &sub2
         << "\nsub3="<< &sub3
         << "\n";
    
    
    return 0;
  }
