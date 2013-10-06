

namespace test {

  template<typename T
          ,template <class> class Fac
          >
  struct Holder
    {
      static T* instance;
      
      T&
      get()
        {
          if (!instance)
            {
              instance = Fac<T>::create();
            }
          return *instance;
        }
    };
  
  
  template<typename T
          ,template <class> class F
          >
  T* Holder<T,F>::instance;
  
  
  
  template<typename T>
  struct Factory
    {
      static T*
      create()
        {
          return new T();
        }
    };
  
  
  
  struct Subject
    {
      static int cnt;
      
      Subject();
      
    };
  
  typedef Holder<Subject,Factory> AccessPoint;
  
  extern AccessPoint fab;
  
  
  
  Subject& fabricate();
  
  
  
  
} // namespace test
