/*
  MULTIFACT-ARG.hpp  -  variant of family-of-object factory, accepting fabrication arguments

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

*/

/** @file multifact.hpp
 ** Extension allowing the MultiFact to pass arguments to the fabrication.
 ** This extension is implemented by template specialisations and thus kicks in
 ** when specifying a function \em signature as factory "product type". The resulting
 ** factory class exposes a function call operator matching this signature, additionally
 ** expecting the ID (to select the specific fabrication function) as first parameter.
 ** 
 ** @note the function signature used for this variant of MultiFact should specify
 **       the raw/base (interface) type of the produced objects as a return type.
 **       Depending on the used wrapper, the actual fabrication functions indeed
 **       should yield the product in a form suitable to be accepted by the
 **       wrapper. E.g., when building smart-ptrs, the fabrication function
 **       should actually deliver a raw pointer to a heap allocated object.
 ** 
 ** @see multifact-argument-test.cpp
 ** @see query-resolver.cpp usage example
 ** @see multifact.hpp standard case
 */


#ifndef LIB_MULTIFACT_ARG_H
#define LIB_MULTIFACT_ARG_H


#include "lib/multifact.hpp"

#include "lib/meta/function.hpp"



namespace lib {
  namespace factory {
    
    
    using lib::meta::Types;
    using lib::meta::FunctionSignature;
    using lib::meta::FunctionTypedef;
    using std::tr1::function;
    
    
    /**
     * Extended MultiFact configuration for arbitrary fabrication functions.
     * Contrary to the (simple) standard case, such fabrication functions 
     * take additional arguments on each invocation. These arguments need
     * to be passed through by MultiFact. Moreover, the actual Wrapper
     * used may require these fabrication functions to deliver their
     * product in a specific form, e.g. as pointer or reference.
     * Thus, we have to re-build the actual signature of the
     * fabrication functions to be installed into MultiFact.
     */
    template< typename SIG
            , template<class> class Wrapper
            >
    struct FabWiring<function<SIG>, Wrapper>
      : Wrapper<typename FunctionSignature<function<SIG> >::Ret>
      {
        typedef typename FunctionSignature<function<SIG> >::Args Args;
        typedef typename FunctionSignature<function<SIG> >::Ret Element;
        typedef typename Wrapper<Element>::PType WrappedProduct;
        typedef typename Wrapper<Element>::RType FabProduct;
        typedef typename FunctionTypedef<FabProduct, Args>::Sig SIG_Fab;
      };
    
    
    
    
    /**
     * Variant of MultiFact, accepting an additional invocation argument.
     * Similar to the (simple) standard case, this MultiFact specialisation
     * allows to install suitable fabrication function(s) at runtime and
     * will select the function to use on invocation based on the given ID.
     * One additional argument will be passed on to this function.
     * @todo with a bit more metaprogramming it would be possible to
     *       accept multiple arguments; maybe we'll need this later on.
     */
    template< typename ELM, typename ARG
            , typename ID
            , template<class> class Wrapper
            >
    class MultiFact<ELM(ARG), ID, Wrapper>
      : public MultiFact<function<ELM(ARG)>,ID,Wrapper>
      {
        typedef MultiFact<function<ELM(ARG)>,ID,Wrapper> _Base;
        typedef typename _Base::Creator Creator;
        
      public:
        typedef typename _Base::Product Product;
        
        Product
        operator() (ID const& id, ARG arg)
          {
            Creator& func = selectProducer (id);
            return wrap (func(arg));
          }
        
      };
    
    
    
  } // namespace factory
  
  
  
  // TODO is there some suitable standard configuration we could provide here??
  
  
} // namespace lib
#endif
