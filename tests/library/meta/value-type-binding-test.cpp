/*
  ValueTypeBinding(Test)  -  human readable simplified display of C++ types

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file type-display-test.cpp
 ** unit test \ref ValueTypeBinding_test
 */


#include "lib/test/run.hpp"
#include "lib/meta/value-type-binding.hpp"
#include "lib/format-cout.hpp"
#include "lib/meta/util.hpp" ///////TODO RLY?

#include <string>
#include <memory>


using std::string;/////////////TODO


namespace lib {
namespace meta{
namespace test{

  namespace { // test fixture
    
    template<class T>
    struct Outer
      {
        struct Inner
          { 
            T val;
          };
        
        typedef T value_type;
        typedef Inner & reference;
        typedef std::shared_ptr<T> pointer;
      };
    
    struct Space { };
    using Join = ulong;
    
    template<typename X>
    struct Bugg
      {
        static_assert (not sizeof(X), "guggi");
      };

    template<typename X>
    struct TypeDiagnostics
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = "";
      };
    template<typename X>
    struct TypeDiagnostics<X&>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = "&";
      };
    template<typename X>
    struct TypeDiagnostics<X&&>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " &&";
      };
    template<typename X>
    struct TypeDiagnostics<X const&>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " const&";
      };
    template<typename X>
    struct TypeDiagnostics<X const&&>
      {
        using Type = X;
        static constexpr auto prefix  = "const ";
        static constexpr auto postfix = " &&";
      };
    template<typename X>
    struct TypeDiagnostics<X *>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " *";
      };
    template<typename X>
    struct TypeDiagnostics<const X *>
      {
        using Type = X;
        static constexpr auto prefix  = "const ";
        static constexpr auto postfix = " *";
      };
    template<typename X>
    struct TypeDiagnostics<const X * const>
      {
        using Type = X;
        static constexpr auto prefix  = "const ";
        static constexpr auto postfix = " * const";
      };
    template<typename X>
    struct TypeDiagnostics<X * const>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " * const";
      };
    template<typename X>
    struct TypeDiagnostics<X * const *>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " * const *";
      };
    
    template<typename X>
    inline string
    showType()
    {
      using Case = TypeDiagnostics<X>;
      using Type = typename Case::Type;
      
      return Case::prefix
           + humanReadableTypeID (typeid(Type).name())
           + Case::postfix;
    }
    
  }//(end)fixture
  
  
  
  
  /**********************************************************************************//**
   * @test verify result type rebinding for containers, iterators values and references.
   *  When augmenting, combining or decorating generic entities, there is often the need
   *  to find out about the `value_type`, a `reference` or `pointer` to such a value.
   *  Many STL compliant containers and iterators provide suitably nested type definitions
   *  to indicate those types. The meta::TypeBinding helper allows to pick up such
   *  definitions, and additionally it levels and unifies access for various combinations
   *  of primitive types, references and pointers. The purpose of this test is to verify
   *  and document this behaviour. 
   * 
   * @see value-type-binding.hpp
   * @see lib::RangeIter
   * @see lib::TreeExplorer::transform()
   */
  class ValueTypeBinding_test
    : public Test
    {
      void
      run (Arg)
        {
          using OuterSpace = Outer<Space>;
          
          cout << showType<OuterSpace::value_type>() <<endl;
          cout << showType<OuterSpace::reference>() <<endl;
          cout << showType<OuterSpace::pointer>() <<endl;
          
          cout << showType<TypeBinding<OuterSpace>::value_type>() <<endl;
          cout << showType<TypeBinding<OuterSpace>::reference>() <<endl;
          cout << showType<TypeBinding<OuterSpace>::pointer>() <<endl;
          
          cout << showType<TypeBinding<Outer<Join>>::value_type>() <<endl;
          cout << showType<TypeBinding<Outer<Join>>::reference>() <<endl;
          cout << showType<TypeBinding<Outer<Join>>::pointer>() <<endl;
          
          cout << showType<TypeBinding<Space>::value_type>() <<endl;
          cout << showType<TypeBinding<Space>::reference>() <<endl;
          cout << showType<TypeBinding<Space>::pointer>() <<endl;
          
          cout << showType<TypeBinding<OuterSpace&>::value_type>() <<endl;
          cout << showType<TypeBinding<OuterSpace&>::reference>() <<endl;
          cout << showType<TypeBinding<OuterSpace&>::pointer>() <<endl;
          
          cout << showType<TypeBinding<OuterSpace&&>::value_type>() <<endl;
          cout << showType<TypeBinding<OuterSpace&&>::reference>() <<endl;
          cout << showType<TypeBinding<OuterSpace&&>::pointer>() <<endl;
          
          cout << showType<TypeBinding<OuterSpace const&>::value_type>() <<endl;
          cout << showType<TypeBinding<OuterSpace const&>::reference>() <<endl;
          cout << showType<TypeBinding<OuterSpace const&>::pointer>() <<endl;
          
          cout << showType<TypeBinding<OuterSpace*>::value_type>() <<endl;
          cout << showType<TypeBinding<OuterSpace*>::reference>() <<endl;
          cout << showType<TypeBinding<OuterSpace*>::pointer>() <<endl;
          
          cout << showType<TypeBinding<const OuterSpace*>::value_type>() <<endl;
          cout << showType<TypeBinding<const OuterSpace*>::reference>() <<endl;
          cout << showType<TypeBinding<const OuterSpace*>::pointer>() <<endl;
          
          cout << showType<TypeBinding<const OuterSpace * const>::value_type>() <<endl;
          cout << showType<TypeBinding<const OuterSpace * const>::reference>() <<endl;
          cout << showType<TypeBinding<const OuterSpace * const>::pointer>() <<endl;
          
          cout << showType<TypeBinding<OuterSpace * const>::value_type>() <<endl;
          cout << showType<TypeBinding<OuterSpace * const>::reference>() <<endl;
          cout << showType<TypeBinding<OuterSpace * const>::pointer>() <<endl;
          
          cout << showType<TypeBinding<Join>::value_type>() <<endl;
          cout << showType<TypeBinding<Join>::reference>() <<endl;
          cout << showType<TypeBinding<Join>::pointer>() <<endl;
          
          cout << showType<TypeBinding<Join&>::value_type>() <<endl;
          cout << showType<TypeBinding<Join&>::reference>() <<endl;
          cout << showType<TypeBinding<Join&>::pointer>() <<endl;
          
          cout << showType<TypeBinding<Join&&>::value_type>() <<endl;
          cout << showType<TypeBinding<Join&&>::reference>() <<endl;
          cout << showType<TypeBinding<Join&&>::pointer>() <<endl;
          
          cout << showType<TypeBinding<Join const&>::value_type>() <<endl;
          cout << showType<TypeBinding<Join const&>::reference>() <<endl;
          cout << showType<TypeBinding<Join const&>::pointer>() <<endl;
          
          cout << showType<TypeBinding<Join *>::value_type>() <<endl;
          cout << showType<TypeBinding<Join *>::reference>() <<endl;
          cout << showType<TypeBinding<Join *>::pointer>() <<endl;
          
          cout << showType<TypeBinding<const Join *>::value_type>() <<endl;
          cout << showType<TypeBinding<const Join *>::reference>() <<endl;
          cout << showType<TypeBinding<const Join *>::pointer>() <<endl;
          
          cout << showType<TypeBinding<const Join * const>::value_type>() <<endl;
          cout << showType<TypeBinding<const Join * const>::reference>() <<endl;
          cout << showType<TypeBinding<const Join * const>::pointer>() <<endl;
          
          cout << showType<TypeBinding<Join * const>::value_type>() <<endl;
          cout << showType<TypeBinding<Join * const>::reference>() <<endl;
          cout << showType<TypeBinding<Join * const>::pointer>() <<endl;
          
          cout << showType<int>() <<endl;
          cout << showType<int&>() <<endl;
          cout << showType<int&&>() <<endl;
          cout << showType<int const&>() <<endl;
          cout << showType<int const&&>() <<endl;
          cout << showType<int       *>() <<endl;
          cout << showType<int const *>() <<endl;
          cout << showType<int const * const>() <<endl;
          cout << showType<int const *      &>() <<endl;
          cout << showType<int const * const&>() <<endl;
        }
    };
  
  LAUNCHER (ValueTypeBinding_test, "unit common");
  
  
}}} // namespace lib::meta::test

