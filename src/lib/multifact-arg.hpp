/*
  MULTIFACT-ARG.hpp  -  variant of family-of-object factory, accepting fabrication arguments
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
 ** @todo WIP-WIP-WIP ... does this work out as intended?? TICKET #377
 ** 
 ** @see multifact-argument-test.cpp
 ** @see query-resolver.cpp usage example
 ** @see multifact.hpp standard case
 */


#ifndef LIB_MULTIFACT_ARG_H
#define LIB_MULTIFACT_ARG_H


#include "lib/multifact.hpp"

#include <tr1/functional>



namespace lib {
  namespace factory {
    
    
    using std::tr1::function;
    
    
    template<typename TY, typename ARG>
    struct FabTraits<TY(ARG)>
      {
        typedef TY RawProduct;
        typedef RawProduct FabSig(ARG);
        typedef ARG Argument;
      };
    
    
    
    
    /**
     * Variant of MultiFact, selecting a creator function by ID
     * with the ability to pass additional argument(s) to this
     * creator function on invocation.
     * TODO type comment
     */
    template< typename SIG
            , typename ID
            , template<class> class Wrapper
            >
    class MultiFact<function<SIG>, ID, Wrapper>
      : public MultiFact<SIG,ID,Wrapper>
      {
        typedef MultiFact<SIG,ID,Wrapper> _Base;
        typedef typename FabTraits<SIG>::Argument ArgType;
        
        typedef typename _Base::Product Product;
        typedef typename _Base::Creator Creator;
        
      public:
        Product
        operator() (ID const& id, ArgType arg)
          {
            Creator& func = selectProducer (id);
            return wrap (func(arg));
          }
        
      };
    
    
    
  } // namespace factory
  
  
  
  // TODO is there some suitable standard configuration we could provide here??
  
  
} // namespace lib
#endif
