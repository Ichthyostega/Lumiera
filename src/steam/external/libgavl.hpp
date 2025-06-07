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
 ** Concrete implementation of the MediaImplLib facade to work with [lib-GAVL],
 ** a library for handling basic raw video data formats.
 ** @todo a draft and placeholder code from 2008, at which time the intention was
 **       to rely on libGAVL for processing of raw media data. This seemed like a
 **       good idea at that time, but we should re-evaluate if libGAVL is maintained
 **       and in actual use, before we start really relying on it.
 ** @todo 2025 this code is a kind of _architecture placeholder_ — it is clear that
 **       a solution can can not be _that simple_, but any viable solution will
 **       rely onto an approach like outlined here: provide a façade interface,
 **       which has to be implemented by any plug-in component to provide access
 **       to the processing capabilities of a specific library. The access point
 **       functions exposed on the facade however must be shaped by the specific
 **       way how the builder and the render engine interacts with and delegates
 **       back to the media-handling library. Such an interface can not be based
 **       on first principles alone.
 ** @todo 2025 I still retain the dependency on lib-GAVL for now, even while we
 **       never got into actually using it. Simply because I still consider this
 **       library exemplary, and would love to use it for real. Yet it remains
 **       to be seen if the project actually gets a chance to do so, simply
 **       because FFmpeg roughly covers the same ground. Presumably it turns
 **       out to be a basic requirement to ship a plug-in with FFmpeg binding.
 **       Furthermore, one especially tricky aspect is how to support displays
 **       with extended dynamic range -- a requirement which has the potential
 **       to blur the notion of »raw video«...
 ** [Lib-GAVL]: https://github.com/bplaum/gavl
 */


#ifndef STEAM_EXTERNAL_LIBGAVL_H
#define STEAM_EXTERNAL_LIBGAVL_H


#include "steam/control/media-impl-lib.hpp"



namespace steam {
namespace external {
  
  
  using lib::Symbol;
  
  using ImplFacade = StreamType::ImplFacade;
  using    TypeTag = StreamType::ImplFacade::TypeTag;
  
  
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
