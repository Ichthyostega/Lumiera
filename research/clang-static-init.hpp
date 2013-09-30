
#include "lib/error.hpp"

#include "lib/singleton-subclass.hpp"


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
  
  typedef lib::SingletonSub<Subject> Factory;
  
  extern Factory fab;
  
  
  
  Subject& fabricate();
  
  
  
  
} // namespace test
