#include "lib/test/run.hpp"

#include "proc/config-resolver.hpp"
#include "clang-static-init.hpp"

#include <iostream>

using proc::ConfigResolver;

using ::test::Test;
using std::cout;
using std::endl;


int
main (int, char**)
  {
    cout <<  "\n.gulp.\n";
    
    ConfigResolver& ref1 = ConfigResolver::instance();
    
    ConfigResolver& sub2 = test::fabricate();
    
    cout << "sub1="<< &ref1 << " sub2="<< &sub2 <<"\n";
    
    
    return 0;
  }


