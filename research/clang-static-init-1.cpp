#include "lib/test/run.hpp"
#include "proc/hh.hpp"

#include "proc/config-resolver.hpp"

#include <iostream>

using proc::ConfigResolver;

using ::test::Test;
using std::cout;
using std::endl;


namespace proc {
namespace test {

  class StaticInstance_test : public Test
    {
      virtual void
      run (Arg) 
        {
          ConfigResolver& ref1 = ConfigResolver::instance();
          
          ConfigResolver& sub2 = fabricate();
          
          cout << "sub1="<< &ref1 << " sub2="<< &sub2 <<"\n";
          
        }
  
    };
  
  /** Register this test class... */
  LAUNCHER (StaticInstance_test, "unit bug");
      
  
  
}} // namespace proc::test
