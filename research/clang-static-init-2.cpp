
#include "clang-static-init.hpp"



namespace test {
  
  
  int Subject::creationCount = 0;
  
  Subject::Subject()
    {
      ++creationCount;
      std::cout << "Subject("<<creationCount<<")\n";
    }
  
  
  
  namespace {
      TypeInfo<Subject> shall_build_a_Subject_instance;
  }
  
  /**
   * instance of the singleton factory
   * @note especially for this example we're using just \em one
   *       shared instance of the factory.
   *       Yet still, the two (inlined) calls to the get() function
   *       access different addresses for the embedded singleton instance
   */
  AccessPoint fab(shall_build_a_Subject_instance);
  
  
  Subject&
  fabricate()
  {
    return fab.get();
  }
  
  
} // namespace test
