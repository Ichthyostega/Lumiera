/*
  LILBGAVL.hpp  -  facade for integrating the GAVL media handling library

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file libgavl.hpp
 ** Concrete implementation of the MediaImplLib facade to work with `libGAVL`
 ** @todo a draft and placeholder code from 2008, at which time the intention was
 **       to rely on libGAVL for processing of raw media data. This seemed like a
 **       good idea at that time, but we should re-evaluate if libGAVL is maintained
 **       and in actual use, before we start really relying on it
 */


#ifndef STEAM_EXTERNAL_LIBGAVL_H
#define STEAM_EXTERNAL_LIBGAVL_H


#include "steam/control/media-impl-lib.hpp"



namespace steam {
namespace external {
  
  
  using lib::Symbol;
  
  typedef StreamType::ImplFacade ImplFacade;
  typedef StreamType::ImplFacade::TypeTag TypeTag;
  
  
  class LibGavl;
  
  /**
   * Concrete media lib implementation facade
   * allowing to work with GAVL data frames and types
   * in an implementation agnostic way.
   * @note GAVL types are automagically registered into the
   * control::STypeManager on reset and thus are always available.
   * @todo 2016 evaluate if libGAVL is still active and maintained
   */
  class ImplFacadeGAVL
    : public ImplFacade
    {
    protected:
      ImplFacadeGAVL()
        : ImplFacade("GAVL")
        { }
      
      friend class LibGavl;
      
    public:
      virtual bool operator== (ImplFacade const& other)  const;
      virtual bool operator== (StreamType const& other)  const;
      
      virtual bool canConvert (ImplFacade const& other)  const;
      virtual bool canConvert (StreamType const& other)  const;
      
      virtual StreamType::MediaKind getKind()  const;
      virtual DataBuffer* createFrame ()  const;
    };
  
  
  class LibGavl
    : public control::MediaImplLib
    {
    protected:
      
    public:
      virtual Symbol getLibID()  const  { return "GAVL"; }
      
      virtual ImplFacadeGAVL const&  getImplFacade (TypeTag&);
    };
  
  
  
}} // namespace steam::external
#endif
