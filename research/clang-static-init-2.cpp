
#include "clang-static-init.hpp"

#include <iostream>

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
//  Holder<Subject> fab2;
    Factory fab2;
  }
  
  
  Subject&
  fabricate()
  {
    return fab2();
  }
  
  
} // namespace test
