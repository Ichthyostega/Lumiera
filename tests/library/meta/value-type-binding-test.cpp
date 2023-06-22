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
#include "lib/test/test-helper.hpp"
#include "lib/meta/value-type-binding.hpp"
#include "lib/format-cout.hpp"
#include "lib/meta/util.hpp"

#include <memory>


namespace lib {
namespace meta{
namespace test{
  
  using lib::test::showType;
  

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
   * @see lib::IterExplorer::transform()
   */
  class ValueTypeBinding_test
    : public Test
    {
      void
      run (Arg)
        {
          // verify the type diagnostics helper...
          CHECK (showType<int               >() == "int"_expect               );
          CHECK (showType<int&              >() == "int&"_expect              );
          CHECK (showType<int&&             >() == "int &&"_expect            );
          CHECK (showType<int const&        >() == "int const&"_expect        );
          CHECK (showType<int const&&       >() == "const int &&"_expect      );
          CHECK (showType<int       *       >() == "int *"_expect             );
          CHECK (showType<int const *       >() == "const int *"_expect       );
          CHECK (showType<int const * const >() == "const int * const"_expect );
          CHECK (showType<int const *      &>() == "int const*&"_expect       );
          CHECK (showType<int const * const&>() == "int const* const&"_expect );
          
          
          // Test fixture: the template Outer<T> provides nested value type bindings
          CHECK (showType<Outer<Space>::value_type>() == "Space"_expect                );
          CHECK (showType<Outer<Space>::reference>()  == "Outer<Space>::Inner&"_expect );
          CHECK (showType<Outer<Space>::pointer>()    == "shared_ptr<Space>"_expect    );
          
          
          // ...such nested type bindings will be picked up
          CHECK (showType<ValueTypeBinding<Outer<Space>>::value_type>() == "Space"_expect                );
          CHECK (showType<ValueTypeBinding<Outer<Space>>::reference>()  == "Outer<Space>::Inner&"_expect );
          CHECK (showType<ValueTypeBinding<Outer<Space>>::pointer>()    == "shared_ptr<Space>"_expect    );
          
          CHECK (showType<ValueTypeBinding<Outer<short>>::value_type>() == "short"_expect                );
          CHECK (showType<ValueTypeBinding<Outer<short>>::reference>()  == "Outer<short>::Inner&"_expect );
          CHECK (showType<ValueTypeBinding<Outer<short>>::pointer>()    == "shared_ptr<short>"_expect    );
          
          // contrast this to a type without such nested bindings
          CHECK (showType<ValueTypeBinding<Space>::value_type>() == "Space"_expect   );
          CHECK (showType<ValueTypeBinding<Space>::reference>()  == "Space&"_expect  );
          CHECK (showType<ValueTypeBinding<Space>::pointer>()    == "Space *"_expect );
          
          // when checking for nested bindings, reference will be stripped and just the binding returned as-is
          CHECK (showType<ValueTypeBinding<Outer<Space>&>::_SrcType>()         == "Outer<Space>"_expect         );   // internal: this is the type probed for nested bindings
          CHECK (showType<ValueTypeBinding<Outer<Space>&>::value_type>()       == "Space"_expect                );
          CHECK (showType<ValueTypeBinding<Outer<Space>&>::reference>()        == "Outer<Space>::Inner&"_expect );
          CHECK (showType<ValueTypeBinding<Outer<Space>&>::pointer>()          == "shared_ptr<Space>"_expect    );
          
          CHECK (showType<ValueTypeBinding<Outer<Space>&&>::_SrcType>()        == "Outer<Space>"_expect         );   // likewise for &&
          CHECK (showType<ValueTypeBinding<Outer<Space>&&>::value_type>()      == "Space"_expect                );
          CHECK (showType<ValueTypeBinding<Outer<Space>&&>::reference>()       == "Outer<Space>::Inner&"_expect );
          CHECK (showType<ValueTypeBinding<Outer<Space>&&>::pointer>()         == "shared_ptr<Space>"_expect    );
          
          CHECK (showType<ValueTypeBinding<Outer<Space> const&>::value_type>() == "Space"_expect                );
          CHECK (showType<ValueTypeBinding<Outer<Space> const&>::reference>()  == "Outer<Space>::Inner&"_expect );
          CHECK (showType<ValueTypeBinding<Outer<Space> const&>::pointer>()    == "shared_ptr<Space>"_expect    );
          
          // but a pointer counts as different, primitive type. No magic here
          CHECK (showType<ValueTypeBinding<      Outer<Space> *      >::value_type>() == "Outer<Space> *"_expect  );
          CHECK (showType<ValueTypeBinding<      Outer<Space> *      >::reference>()  == "Outer<Space>*&"_expect  );
          CHECK (showType<ValueTypeBinding<      Outer<Space> *      >::pointer>()    == "Outer<Space>* *"_expect );
          
          CHECK (showType<ValueTypeBinding<const Outer<Space> *      >::value_type>() == "const Outer<Space> *"_expect  );
          CHECK (showType<ValueTypeBinding<const Outer<Space> *      >::reference>()  == "Outer<Space> const*&"_expect  );
          CHECK (showType<ValueTypeBinding<const Outer<Space> *      >::pointer>()    == "Outer<Space> const* *"_expect );
          
          CHECK (showType<ValueTypeBinding<const Outer<Space> * const>::value_type>() == "const Outer<Space> * const"_expect );
          CHECK (showType<ValueTypeBinding<const Outer<Space> * const>::reference>()  == "Outer<Space> const* const&"_expect );
          CHECK (showType<ValueTypeBinding<const Outer<Space> * const>::pointer>()    == "Outer<Space> * const *"_expect     );
          
          CHECK (showType<ValueTypeBinding<      Outer<Space> * const>::value_type>() == "Outer<Space> * const"_expect   );
          CHECK (showType<ValueTypeBinding<      Outer<Space> * const>::reference>()  == "Outer<Space>* const&"_expect   );
          CHECK (showType<ValueTypeBinding<      Outer<Space> * const>::pointer>()    == "Outer<Space> * const *"_expect );
          
          // yet for a type without nested type bindings: references are levelled
          CHECK (showType<ValueTypeBinding<short>::value_type>()        == "short"_expect   );
          CHECK (showType<ValueTypeBinding<short>::reference>()         == "short&"_expect  );
          CHECK (showType<ValueTypeBinding<short>::pointer>()           == "short *"_expect );
          
          CHECK (showType<ValueTypeBinding<short&>::value_type>()       == "short"_expect   );
          CHECK (showType<ValueTypeBinding<short&>::reference>()        == "short&"_expect  );
          CHECK (showType<ValueTypeBinding<short&>::pointer>()          == "short *"_expect );
          
          CHECK (showType<ValueTypeBinding<short&&>::value_type>()      == "short"_expect   );
          CHECK (showType<ValueTypeBinding<short&&>::reference>()       == "short&"_expect  );
          CHECK (showType<ValueTypeBinding<short&&>::pointer>()         == "short *"_expect );

          CHECK (showType<ValueTypeBinding<short const&>::value_type>() == "const short"_expect  );
          CHECK (showType<ValueTypeBinding<short const&>::reference>()  == "short const&"_expect  );
          CHECK (showType<ValueTypeBinding<short const&>::pointer>()    == "const short *"_expect );
          
          //... but pointer types are not treated special in any way
          CHECK (showType<ValueTypeBinding<      short *      >::value_type>() == "short *"_expect  );
          CHECK (showType<ValueTypeBinding<      short *      >::reference>()  == "short*&"_expect  );
          CHECK (showType<ValueTypeBinding<      short *      >::pointer>()    == "short* *"_expect );
          
          CHECK (showType<ValueTypeBinding<const short *      >::value_type>() == "const short *"_expect  );
          CHECK (showType<ValueTypeBinding<const short *      >::reference>()  == "short const*&"_expect  );
          CHECK (showType<ValueTypeBinding<const short *      >::pointer>()    == "short const* *"_expect );
          
          CHECK (showType<ValueTypeBinding<const short * const>::value_type>() == "const short * const"_expect );
          CHECK (showType<ValueTypeBinding<const short * const>::reference>()  == "short const* const&"_expect );
          CHECK (showType<ValueTypeBinding<const short * const>::pointer>()    == "short * const *"_expect     );
          
          CHECK (showType<ValueTypeBinding<      short * const>::value_type>() == "short * const"_expect   );
          CHECK (showType<ValueTypeBinding<      short * const>::reference>()  == "short* const&"_expect   );
          CHECK (showType<ValueTypeBinding<      short * const>::pointer>()    == "short * const *"_expect );
        }
    };
  
  LAUNCHER (ValueTypeBinding_test, "unit common");
  
  
}}} // namespace lib::meta::test

