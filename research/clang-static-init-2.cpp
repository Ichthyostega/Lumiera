
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
      lib::singleton::UseSubclass<Subject> typeID;
  }
  
  Factory fab(typeID);
  
  
  Subject&
  fabricate()
  {
    return fab();
  }
  
  
} // namespace test
