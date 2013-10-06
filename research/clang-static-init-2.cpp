
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
      //
  }
  
  AccessPoint fab;
  
  
  Subject&
  fabricate()
  {
    return fab.get();
  }
  
  
} // namespace test
