
#include "lib/error.hpp"

#include "proc/config-resolver.hpp"

namespace proc {
namespace test {

  template<typename T>
  struct Holder
    {
      static T* instance;
      
      T&
      get()
        {
          if (!instance)
            {
              instance = new T();
            }
          return *instance;
        }
    };
  
  
  template<typename T>
  T* Holder<T>::instance;
  
  struct Subject
    {
      static int cnt;
      
      Subject();
      
    };
  
  
  proc::ConfigResolver& fabricate();
  
  
  
  
}} // namespace proc::test
  
