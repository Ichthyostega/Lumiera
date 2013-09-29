
#include "clang-static-init.hpp"

#include <iostream>

using proc::ConfigResolver;
using std::cout;
using std::endl;


namespace test {
  
  
  int Subject::cnt = 0;
  
  Subject::Subject()
    {
      ++cnt;
      std::cout << "Subject("<<cnt<<")\n";
    }
   
  
  namespace {
    Holder<Subject> fab2;
  }
  
  
  ConfigResolver&
  fabricate()
  {
    return ConfigResolver::instance();
  }
  
  
} // namespace test
