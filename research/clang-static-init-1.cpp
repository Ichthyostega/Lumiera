#include "lib/test/run.hpp"

#include "clang-static-init.hpp"

#include <iostream>


using ::test::Test;
using std::cout;
using std::endl;

using namespace test;

int
main (int, char**)
  {
    cout <<  "\n.gulp.\n";
    
    Factory fab1;
    Subject& ref1 = fab1();
    
    Subject& sub2 = test::fabricate();
    
    cout << "sub1="<< &ref1 << " sub2="<< &sub2 <<"\n";
    
    
    return 0;
  }


