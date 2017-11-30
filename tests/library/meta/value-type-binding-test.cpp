/*
  ValueTypeBinding(Test)  -  verify handling of value_type definitions on custom containers

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
          // verify the type diagnostics helper...
          CHECK ("int"               == showType<int               >() );
          CHECK ("int&"              == showType<int&              >() );
          CHECK ("int &&"            == showType<int&&             >() );
          CHECK ("int const&"        == showType<int const&        >() );
          CHECK ("const int &&"      == showType<int const&&       >() );
          CHECK ("int *"             == showType<int       *       >() );
          CHECK ("const int *"       == showType<int const *       >() );
          CHECK ("const int * const" == showType<int const * const >() );
          CHECK ("int const*&"       == showType<int const *      &>() );
          CHECK ("int const* const&" == showType<int const * const&>() );
          
          
          // Test fixture: the template Outer<T> provides nested value type bindings
          using OuterSpace = Outer<Space>;
          using Join = ulong;
          
          CHECK ("Space"                == showType<OuterSpace::value_type>() );
          CHECK ("Outer<Space>::Inner&" == showType<OuterSpace::reference>() );
          CHECK ("shared_ptr<Space>"    == showType<OuterSpace::pointer>() );
          
          // ...such nested type bindings will be picked up
          CHECK ("Space"                == showType<TypeBinding<OuterSpace>::value_type>() );
          CHECK ("Outer<Space>::Inner&" == showType<TypeBinding<OuterSpace>::reference>() );
          CHECK ("shared_ptr<Space>"    == showType<TypeBinding<OuterSpace>::pointer>() );
          
          CHECK ("unsigned long"                == showType<TypeBinding<Outer<Join>>::value_type>() );
          CHECK ("Outer<unsigned long>::Inner&" == showType<TypeBinding<Outer<Join>>::reference>() );
          CHECK ("shared_ptr<unsigned long>"    == showType<TypeBinding<Outer<Join>>::pointer>() );
          
          // contrast this to a type without such nested bindings
          CHECK ("Space"   == showType<TypeBinding<Space>::value_type>() );
          CHECK ("Space&"  == showType<TypeBinding<Space>::reference>()  );
          CHECK ("Space *" == showType<TypeBinding<Space>::pointer>()    );
          
          // reference types will be levelled (reference stripped)
          CHECK ("Space"                == showType<TypeBinding<OuterSpace&>::value_type>() );
          CHECK ("Outer<Space>::Inner&" == showType<TypeBinding<OuterSpace&>::reference>()  );
          CHECK ("shared_ptr<Space>"    == showType<TypeBinding<OuterSpace&>::pointer>()    );
          
          CHECK ("Space"                == showType<TypeBinding<OuterSpace&&>::value_type>() );
          CHECK ("Outer<Space>::Inner&" == showType<TypeBinding<OuterSpace&&>::reference>()  );
          CHECK ("shared_ptr<Space>"    == showType<TypeBinding<OuterSpace&&>::pointer>()    );
          
          CHECK ("Space"                == showType<TypeBinding<OuterSpace const&>::value_type>() );
          CHECK ("Outer<Space>::Inner&" == showType<TypeBinding<OuterSpace const&>::reference>()  );
          CHECK ("shared_ptr<Space>"    == showType<TypeBinding<OuterSpace const&>::pointer>()    );
          
          // but a pointer counts as a different, primitive type. No magic here
          CHECK ("Outer<Space> *"  == showType<TypeBinding<OuterSpace*>::value_type>() );
          CHECK ("Outer<Space>*&"  == showType<TypeBinding<OuterSpace*>::reference>()  );
          CHECK ("Outer<Space>* *" == showType<TypeBinding<OuterSpace*>::pointer>()    );
          
          CHECK ("const Outer<Space> *"  == showType<TypeBinding<const OuterSpace*>::value_type>() );
          CHECK ("Outer<Space> const*&"  == showType<TypeBinding<const OuterSpace*>::reference>()  );
          CHECK ("Outer<Space> const* *" == showType<TypeBinding<const OuterSpace*>::pointer>()    );
          
          CHECK ("const Outer<Space> * const" == showType<TypeBinding<const OuterSpace * const>::value_type>() );
          CHECK ("Outer<Space> const* const&" == showType<TypeBinding<const OuterSpace * const>::reference>()  );
          CHECK ("Outer<Space> * const *"     == showType<TypeBinding<const OuterSpace * const>::pointer>()    );
          
          CHECK ("Outer<Space> * const"   == showType<TypeBinding<OuterSpace * const>::value_type>() );
          CHECK ("Outer<Space>* const&"   == showType<TypeBinding<OuterSpace * const>::reference>()  );
          CHECK ("Outer<Space> * const *" == showType<TypeBinding<OuterSpace * const>::pointer>()    );
          
          // similar for a type without nested type bindings: references are levelled
          CHECK ("unsigned long"   == showType<TypeBinding<Join>::value_type>() );
          CHECK ("unsigned long&"  == showType<TypeBinding<Join>::reference>()  );
          CHECK ("unsigned long *" == showType<TypeBinding<Join>::pointer>()    );
          
          CHECK ("unsigned long"   == showType<TypeBinding<Join&>::value_type>() );
          CHECK ("unsigned long&"  == showType<TypeBinding<Join&>::reference>()  );
          CHECK ("unsigned long *" == showType<TypeBinding<Join&>::pointer>()    );
          
          CHECK ("unsigned long"   == showType<TypeBinding<Join&&>::value_type>() );
          CHECK ("unsigned long&"  == showType<TypeBinding<Join&&>::reference>()  );
          CHECK ("unsigned long *" == showType<TypeBinding<Join&&>::pointer>()    );
          
          CHECK ("unsigned long"         == showType<TypeBinding<Join const&>::value_type>() );
          CHECK ("unsigned long const&"  == showType<TypeBinding<Join const&>::reference>()  );
          CHECK ("const unsigned long *" == showType<TypeBinding<Join const&>::pointer>()    );
          
          //... but pointer types are not treated special in any way
          CHECK ("unsigned long *"  == showType<TypeBinding<Join *>::value_type>() );
          CHECK ("unsigned long*&"  == showType<TypeBinding<Join *>::reference>()  );
          CHECK ("unsigned long* *" == showType<TypeBinding<Join *>::pointer>()    );
          
          CHECK ("const unsigned long *"  == showType<TypeBinding<const Join *>::value_type>() );
          CHECK ("unsigned long const*&"  == showType<TypeBinding<const Join *>::reference>()  );
          CHECK ("unsigned long const* *" == showType<TypeBinding<const Join *>::pointer>()    );
          
          CHECK ("const unsigned long * const" == showType<TypeBinding<const Join * const>::value_type>() );
          CHECK ("unsigned long const* const&" == showType<TypeBinding<const Join * const>::reference>()  );
          CHECK ("unsigned long * const *"     == showType<TypeBinding<const Join * const>::pointer>()    );
          
          CHECK ("unsigned long * const"   == showType<TypeBinding<Join * const>::value_type>() );
          CHECK ("unsigned long* const&"   == showType<TypeBinding<Join * const>::reference>()  );
          CHECK ("unsigned long * const *" == showType<TypeBinding<Join * const>::pointer>()    );
        }
    };
  
  LAUNCHER (ValueTypeBinding_test, "unit common");
  
  
}}} // namespace lib::meta::test

