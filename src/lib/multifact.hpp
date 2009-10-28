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
 ** @see multifact-test.cpp
 ** @see SingletonFactory
 ** @see lib::factory::Factory
 */


#ifndef LIB_MULTIFACT_H
#define LIB_MULTIFACT_H


#include "lib/error.hpp"
#include "lib/singleton.hpp"
#include "util.hpp"

#include <tr1/functional>
#include <tr1/memory>
#include <map>



namespace lib {
  namespace factory {
    
    
    /**
     * Dummy "wrapper",
     * just returning a target-ref
     */
    template<typename TAR>
    struct PassReference
      {
        typedef TAR& RType;
        typedef TAR& PType;
        
        PType wrap (RType object) { return object; }
      };
    
    
    /**
     * Wrapper taking ownership,
     * by wrapping into smart-ptr
     */
    template<typename TAR>
    struct BuildRefcountPtr
      {
        typedef TAR*                      RType;
        typedef std::tr1::shared_ptr<TAR> PType;
        
        PType wrap (RType ptr) { return PType (ptr); }
      };
    
    
    
    template<typename TY>
    struct FabTraits
      {
        typedef TY RawProduct;
        typedef RawProduct FabSig(void);
      };
    
    
    
    /**
     * Table of registered production functions for MultiFact.
     * Each stored function can be accessed by ID and is able
     * to fabricate a specific object, which is assignable to TY
     */
    template<typename TY, typename ID>
    struct Fab
      {
        typedef typename FabTraits<TY>::FabSig Signature;
        typedef std::tr1::function<Signature> FactoryFunc;
        
        
        FactoryFunc&
        select (ID const& id)
          {
            if (!contains (id))
              throw lumiera::error::Invalid("unknown factory product requested.");
            
            return producerTable_[id];
          }
        
        void
        defineProduction (ID const& id, FactoryFunc fun)
          {
            producerTable_[id] = fun;
          }
        
        
        /* === diagnostics === */
        
        bool empty ()         const { return producerTable_.empty(); }
        bool contains (ID id) const { return util::contains (producerTable_,id); }
        
      private:
        std::map<ID, FactoryFunc> producerTable_;
      };
    
    
    
    /**
     * Factory for creating a family of objects by ID.
     * The actual factory functions are to be installed
     * from the usage site through calls to #defineProduction .
     * Each generated object will be treated by the Wrapper template,
     * allowing for the generation of smart-ptrs. The embedded class
     * Singleton allows to build a family of singleton objects; it is
     * to be instantiated at the call site and acts as singleton factory,
     * accessible through a MultiFact instance as frontend.
     */
    template< typename TY
            , typename ID
            , template<class> class Wrapper
            >
    class MultiFact
      : Wrapper<typename FabTraits<TY>::RawProduct>
      {
        typedef Wrapper<typename FabTraits<TY>::RawProduct> _Wrap;
        typedef typename FabTraits<TY>::RawProduct RawType;
        typedef typename _Wrap::PType WrappedProduct;
        typedef typename _Wrap::RType FabProduct;
        typedef Fab<FabProduct,ID> _Fab;
        
        _Fab funcTable_;
        
        
      protected:
        typedef typename _Fab::FactoryFunc Creator;
        typedef WrappedProduct Product;
        
        Creator&
        selectProducer (ID const& id)
          {
            return funcTable_.select(id);
          }
        
        
      public:
        Product
        operator() (ID const& id)
          {
            Creator& func = selectProducer (id);
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
         * of the given target type (IMP)
         */
        template<class IMP>
        class Singleton
          : lib::Singleton<IMP>
          {
            typedef lib::Singleton<IMP> SingFac;
            
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
          
        
        /* === diagnostics === */
          
        bool empty ()         const { return funcTable_.empty();       }
        bool contains (ID id) const { return funcTable_.contains (id); }
      };
    
    
    
  } // namespace factory
  
  
  
  /** 
   * Standard configuration of the family-of-object factory
   * @todo this is rather guesswork... find out what the best and most used configuration could be....
   */
  template< typename TY
          , typename ID
          >
  class MultiFact
    : public factory::MultiFact<TY,ID, factory::PassReference>
    { };
  
  
} // namespace lib
#endif
