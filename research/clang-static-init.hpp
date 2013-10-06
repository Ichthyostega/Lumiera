
#include <iostream>

using std::cout;


namespace test {
  
  
  
  /* === Layer-1: a singleton factory based on a templated static variable === */
  
  template<typename I                     ///< Interface of the product type
          ,template <class> class Fac     ///< Policy: actual factory to create the instance
          >
  struct Holder
    {
      static I* instance;
      
      I&
      get()
        {
          if (!instance)
            {
              cout << "Singleton Factory: invoke Fabrication ---> instance="<<&instance<<"...\n";
              
              instance = Fac<I>::create();
            }
          return *instance;
        }
    };
  
  /**
   * allocate storage for the per-type shared
   * (static) variable to hold the singleton instance
   */
  template<typename I
          ,template <class> class F
          >
  I* Holder<I,F>::instance;
  
  
  
  
  template<typename C>
  struct Factory
    {
      static C*
      create()
        {
          return new C();
        }
    };
  
  
  
  
  
  /* === Layer-2: configurable product type === */
  
  template<typename I>
  struct Adapter
    {
      typedef I* FactoryFunction (void);
      
      static FactoryFunction* factoryFunction;
      
      
      template<typename C>
      static I*
      concreteFactoryFunction()
        {
          return static_cast<I*> (Factory<C>::create());
        }
      
      
      template<typename X>
      struct AdaptedConfigurableFactory
        {
          static X*
          create()
            {
              return (*factoryFunction)();
            }
        };
    };
  
  /** storage for the per-type shared function pointer to the concrete factory */
  template<typename I>
  typename Adapter<I>::FactoryFunction*  Adapter<I>::factoryFunction;
  
  
  
  template<typename C>
  struct TypeInfo { };
  
  
  
  /**
   * Singleton factory with the ability to configure the actual product type C
   * only at the \em definition site. Users get to see only the interface type T
   */
  template<typename T>
  struct ConfigurableHolder
    : Holder<T, Adapter<T>::template AdaptedConfigurableFactory>
    {
      /** define the actual product type */
      template<typename C>
      ConfigurableHolder (TypeInfo<C>)
        {
          Adapter<T>::factoryFunction = &Adapter<T>::template concreteFactoryFunction<C>;
        }
    };
  
  
  
  
  
  /* === Actual usage: Test case fabricating Subject instances === */
  
  struct Subject
    {
      static int creationCount;
      
      Subject();
      
    };
  
  typedef ConfigurableHolder<Subject> AccessPoint;
  
  extern AccessPoint fab;
  
  
  
  Subject& fabricate();
  
  
  
  
} // namespace test
