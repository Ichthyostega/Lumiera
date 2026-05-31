/*
  BUFFER-PROVIDER-SETUP.hpp  -  configuration of the actual BufferProvider implementation

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file buffer-provider-setup.hpp
 ** Helper template for installing a suitable BufferProvider setup.
 ** 
 ** @see engine-facilities.cpp
 ** @see buffer-provider.hpp
 */

#ifndef VAULT_MEM_BUFFR_PROVIDER_SETUP_H
#define VAULT_MEM_BUFFR_PROVIDER_SETUP_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/hash-value.h"
#include "vault/mem/buffer-provider.hpp"
#include "lib/nocopy.hpp"

#include <utility>
#include <memory>


namespace vault {
namespace mem   {
  
  using lib::Literal;
  using std::unique_ptr;
  using std::make_unique;
  using std::forward;
  using std::move;
  
  
  
  
  
  /**
   * Framework for configuring the actual BufferPovider backend implementation.
   * 
   * @todo WIP-WIP-WIP as of 2/2026 -- reworking BufferProvider implementation   ////////////////////////////TICKET #1410
   */
  class BufferProviderSetup
    : public BufferProvider
    {
    public:
      /**
       * Build a new BufferProvider setup to manage a number of buffers.
       * The metadata of these buffers is organised hierarchically based on
       * chained hash values, using an `implementationID` as a seed.
       * The \a CONF configuration record controls the actual implementation
       * of lifecycle transitions and storage management.
       */
      template<class CONF>
      BufferProviderSetup (CONF&& confRec)
        : BufferProvider{}
        {
          bufferStage_ = move (confRec.buildStage());
          bufferStore_ = move (confRec.buildStore());
        }
      
      
      /* ======== Extension points ======== */
      
      class Stage
        : public BufferStage
        { };
      
      class Store
        : public BufferStore
        { };

      
    protected:
      /**
       * Enable subclasses to layer a decorator on top of
       * an already instantiated implementation object
       * @tparam IMP concrete type of the implementation decorator; must be specified.
       * @tparam API the API to decorate; the decorator IMP must implement this API
       * @tparam ARGS (optionally) further constructor arguments for the decorator
       * @param rawImp the `uniqe_ptr` with an already instantiated implementation of API
       * @note the decorator subclass is assumed to incorporate an unique_ptr&&, holding
       *       and managing the underlying implementation from that point on
       * @warning the new decorator implementation is responsible for the overall sane state,
       *       starting from that point when it picks up the given unique_ptr. An exception
       *       emanating after that point will not leak memory, but destroy the original
       *       implementation and thus leave the whole BufferProvider in undefined state.
       */
      template<class IMP, class API, typename...ARGS>
      void
      decorate (unique_ptr<API>& rawImp, ARGS&& ...furtherDecoratorArgs)
        {
          unique_ptr<API> underlying;
          try {
              REQUIRE (rawImp);
              std::swap (rawImp, underlying);
              ENSURE (not rawImp);
              rawImp = make_unique<IMP> (move(underlying), forward<ARGS> (furtherDecoratorArgs)...);
              ENSURE (rawImp);
            }
          catch (...)
            {
              REQUIRE (not rawImp);
              REQUIRE (underlying);
              std::swap (rawImp, underlying);
            }
        }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_BUFFR_PROVIDER_SETUP_H*/
