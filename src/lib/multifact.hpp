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
#include "lib/singleton.hpp"
#include "util.hpp"

#include <tr1/functional>
#include <map>



namespace lib {
  namespace factory {
    
    using util::contains;
    
    /**
     * Dummy "wrapper",
     * just returning a target-ref
     */
    template<typename TAR>
    struct PassReference
      {
        typedef TAR& PType;
        
        PType wrap (TAR& object) { return object; }
      };
    
    /**
     * Repository of registered production lines.
     * @todo write type comment
     */
    template<typename TY, typename ID>
    struct Fab
      {
        typedef TY& RawProduct;
        typedef std::tr1::function<RawProduct(void)> FactoryFunc;
        
        
        FactoryFunc&
        select (ID id)
          {
            if (!contains (producerTable_,id))
              throw lumiera::error::Invalid("unknown factory product requested.");
            
            return producerTable_[id];
          }
        
        void
        defineProduction (ID id, FactoryFunc fun)
          {
            producerTable_[id] = fun;
          }
        
      private:
        std::map<ID, FactoryFunc> producerTable_;
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
        typedef typename Wrapper<TY>::PType Product;
        
        _Fab funcTable_;
        
      public:
        Product
        operator() (ID id)
          {
            Creator& func = funcTable_.select(id);
            return wrap (func());
          }
        
        /** to set up a production line,
         *  associated with a specific ID
         */
        template<typename FUNC>
        void
        defineProduction (ID id, FUNC fun)
          {
            funcTable_.defineProduction (id, fun);
          }
        
        /**
         * Convenience shortcut for automatically setting up
         * a production line, fabricating a singleton instance
         * of the given target type (TAR)
         */
        template<class TAR>
        class Singleton
          : lib::Singleton<TAR>
          {
            typedef lib::Singleton<TAR> SingFac;
//            typedef std::tr1::function<TAR&(void)> AccessSingleton_Func;
            
            Creator
            createSingleton_accessFunction()
              {
                return std::tr1::bind (&SingFac::operator()
                                      , static_cast<SingFac*>(this));
              }
            
          public:
            Singleton (MultiFact& factory, ID id)
              {
                factory.defineProduction(id, createSingleton_accessFunction());
              }
          };
      };
    
    
    
  } // namespace factory
  
//using factory::Factory;
  
  
} // namespace lib
#endif
