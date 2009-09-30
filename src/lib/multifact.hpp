/*
  MULTIFACT.hpp  -  flexible family-of-object factory template
 
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
 ** Building blocks of a configurable factory, generating families of related objects.
 ** 
 ** @todo WIP-WIP-WIP ... currently collecting various bits of implementation here. TICKET #277
 ** 
 ** @see SingletonFactory
 ** @see lib::factory::Factory
 */


#ifndef LIB_MULTIFACT_H
#define LIB_MULTIFACT_H


#include "lib/error.hpp"
//#include <tr1/memory>

#include <tr1/functional>



namespace lib {
  namespace factory {
    
    /**
     * Repository of registered production lines.
     * @todo write type comment
     */
    template<typename TY, typename ID>
    class Fab
      {
        typedef TY& RawProduct;
        typedef std::tr1::function<RawProduct(void)> & FactoryFunc;
        
      public:
        static FactoryFunc
        select (ID id)
          {
            UNIMPLEMENTED ("how to store/select the production line");
          }
        
        static void
        defineProduction (ID id, FactoryFunc fun)
          {
            UNIMPLEMENTED ("how to store/select the production line");
          }
      };
    
    
    
    /**
     * @todo write type comment
     */
    template< typename TY
            , typename ID
            , template<class> class Wrapper
            >
    class MultiFact
      : Wrapper<TY>
      {
        typedef Fab<TY,ID> _Fab;
        typedef typename _Fab::FactoryFunc Creator;
        typedef typename WR::PType Product;
        
      public:
        Product
        operator() (ID id)
          {
            Creator func = _Fab::select(id);
            return wrap (func());
          }
        
        /**
         * to set up a production line,
         * associated with a specific ID
         */
        struct Produce
          : Creator
          {
            template<typename FUNC>
            Produce (ID id, FUNC fun)
              : Creator(fun)
              {
                _Fab::defineProduction (id, *this);
              }
          };
        
      };
      
      
      
  } // namespace factory
  
//using factory::Factory;
  
  
} // namespace lib
#endif
