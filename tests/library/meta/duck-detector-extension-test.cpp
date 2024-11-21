/*
  DuckDetectorExtension(Test)  -  detecting support for extension points at compile time

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file duck-detector-extension-test.cpp
 ** unit test \ref DuckDetectorExtension_test
 */


#include "lib/test/run.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/util.hpp"

#include <string>


namespace lib {
namespace meta{
namespace test{
  
  using std::string;
  
  
  namespace { // test types and definitions to explore....
    
    void
    fun (long)
      { }
    
    int
    fun (string, long)
      {
        return 12;
      }
    
    void
    fun ()
      { }
    
    
    class Cheesy
      {
      public:
        double funny (char, char, string);
        void funky()  const;
        short fuzzy (float, float);
        llong fuzzy();
        double fully;
      };
    
    class Fishy
      {
        /** @note private function can never be detected */
        llong fuzzy();
        
        /** @note type Fishy exposes an extension point `fun` */
        friend void fun (Fishy&);
      };
    
    
  }//(End) test definitions
  
  
  
  
  /***********************************************************************************//**
   * @test demonstrate some details regarding detection of functions within a type.
   *  The purpose of these metaprogramming techniques is to write generic containers,
   *  handlers, iterators etc, which automatically adapt themselves to the abilities
   *  of their payload type. To give an example, we may investigate the argument type
   *  of a functor or lambda, and then compose it with a suitable adapter or converter
   *  to work with the given data.
   *  
   *  However, when it comes to detecting the presence of a specific function, there
   *  are some complexities and variations to consider. Sometimes we only want to check
   *  for the presence of some function, while in other cases we also want to verify
   *  the exact signature of that function. Moreover, some of these detection techniques
   *  break down whenever there is overload ambiguity; thus we might need to resort to
   *  an alternative, not so strict test to get past such limitations.
   */
  class DuckDetectorExtension_test : public Test
    {
      
      META_DETECT_EXTENSION_POINT (funZ);
      META_DETECT_EXTENSION_POINT (fun);
      
      META_DETECT_FUNCTION (double, funny, (char, char, string));
      META_DETECT_FUNCTION (llong, fuzzy, (void));
      META_DETECT_FUNCTION_NAME (funny);
      META_DETECT_FUNCTION_NAME (funky);
      META_DETECT_FUNCTION_NAME (fuzzy);
      META_DETECT_FUNCTION_NAME (fully);
      META_DETECT_MEMBER(funny);
      META_DETECT_MEMBER(funky);
      META_DETECT_MEMBER(fuzzy);
      META_DETECT_MEMBER(fully);
      META_DETECT_FUNCTION_ARGLESS (funny);
      META_DETECT_FUNCTION_ARGLESS (funky);
      META_DETECT_FUNCTION_ARGLESS (fuzzy);
      
      
      void
      run (Arg)
        {
          detect_freeFunctionADL_ExtensionPoint();
          detect_memberFunctionVariations();
        }
      
      
      /** @test detect the presence of a free function extension point at compile time.
       * It is a common idiom in C++ to expose an extension point through a free function,
       * which is expected to be picked up by ADL. To mention a prominent example, any type
       * can offer the ability to be _iterated_ by injecting free functions `begin(TY)` and
       * `end(TY)`, to yield a STL compatible iterator.
       * 
       * Since such an extension point is used just by invoking the _unqualified_ function
       * with the target type, we can build a meta predicate based on the fact if such an
       * function invocation expression can be formed for the type in question.
       * 
       * @note the test really hinges on the ability to form the extension point call.
       *       For this reason, some implicit conversions might be involved, and some
       *       other conversions won't work (like passing a value to an extension point
       *       taking a reference).
       */
      void
      detect_freeFunctionADL_ExtensionPoint()
        {
          fun ();
          fun (23);
          fun ("FUN", 45);
          
          CHECK ( not HasExtensionPoint_funZ<long>::value );
          
          CHECK (     HasExtensionPoint_fun<long>        ::value );
          CHECK (     HasExtensionPoint_fun<long&>       ::value );
          CHECK (     HasExtensionPoint_fun<long&&>      ::value );
          CHECK (     HasExtensionPoint_fun<char>        ::value );
          CHECK (     HasExtensionPoint_fun<char&>       ::value );
          CHECK (     HasExtensionPoint_fun<char&&>      ::value );
          CHECK ( not HasExtensionPoint_fun<string>      ::value );
          CHECK ( not HasExtensionPoint_fun<void>        ::value );
          
          CHECK ( not HasExtensionPoint_fun<Cheesy>      ::value );
          CHECK ( not HasExtensionPoint_fun<Fishy>       ::value );
          CHECK (     HasExtensionPoint_fun<Fishy&>      ::value );
          CHECK ( not HasExtensionPoint_fun<Fishy&&>     ::value );
          CHECK ( not HasExtensionPoint_fun<Fishy const&>::value );
          
          CHECK ( not HasExtensionPoint_fun<Fishy const&>::value );
        }
      
      
      /** @test cover variations of member function detection
       *      - detect an explicitly given full signature
       *      - detect just the presence of a function _name_
       *      - distinguish this from detecting a _member_
       *      - detect especially a member function without arguments
       *  @note some corner cases are demonstrated here as well:
       *      - private functions can not be detected
       *      - detection fails on ambiguity due to overloads
       */
      void
      detect_memberFunctionVariations()
        {
          CHECK (     HasFunSig_funny<Cheesy>        ::value );  // explicit function signature detected
          CHECK (     HasFunSig_funny<Cheesy const>  ::value );  // const qualifier is irrelevant
          CHECK ( not HasFunSig_funny<Cheesy const&> ::value );  // but reference does not work, obviously
          
          CHECK (     HasFunSig_fuzzy<Cheesy>        ::value );  // explicit function signature detected, overload is irrelevant
          
          CHECK (     HasFunName_funny<Cheesy>       ::value );  // function name detected (arguments irrelevant)
          CHECK (     HasFunName_funky<Cheesy>       ::value );  // detected irrespective of const modifier
          CHECK ( not HasFunName_fuzzy<Cheesy>       ::value );  // function name fuzzy *not* detected due to overload ambiguity
          CHECK ( not HasFunName_fully<Cheesy>       ::value );  // name fully is a member, not a function
          
          CHECK (     HasMember_funny<Cheesy>        ::value );  // 'funny' is not only a function, it is also a member
          CHECK (     HasMember_funky<Cheesy>        ::value );
          CHECK ( not HasMember_fuzzy<Cheesy>        ::value );  // WARNING: member 'fuzzy' *not* detected due to overload ambiguity
          CHECK (     HasMember_fully<Cheesy>        ::value );  // 'fully' is not a function, but it is detected as member here
          
          CHECK ( not HasArglessFun_funny<Cheesy>    ::value );  // there is no argument less function 'funny' (it takes arguments)
          CHECK (     HasArglessFun_funky<Cheesy>    ::value );  // but an argument-less 'funky'
          CHECK (     HasArglessFun_fuzzy<Cheesy>    ::value );  // and one of the 'fuzzy' overloads also takes no arguments
          
          CHECK ( not HasFunSig_fuzzy<Fishy>         ::value );  // Fishy::fuzzy() is private and can thus never be detected
          CHECK ( not HasFunSig_funny<Fishy>         ::value );  // and no fun with Fishy beyond that...
          CHECK ( not HasFunName_funny<Fishy>        ::value );
          CHECK ( not HasFunName_funky<Fishy>        ::value );
          CHECK ( not HasFunName_fuzzy<Fishy>        ::value );
          CHECK ( not HasFunName_fully<Fishy>        ::value );
          CHECK ( not HasMember_funny<Fishy>         ::value );
          CHECK ( not HasMember_funky<Fishy>         ::value );
          CHECK ( not HasMember_fuzzy<Fishy>         ::value );
          CHECK ( not HasMember_fully<Fishy>         ::value );
          CHECK ( not HasArglessFun_funny<Fishy>     ::value );
          CHECK ( not HasArglessFun_funky<Fishy>     ::value );
          CHECK ( not HasArglessFun_fuzzy<Fishy>     ::value );
          CHECK ( not HasFunSig_fuzzy<short>         ::value );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DuckDetectorExtension_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
