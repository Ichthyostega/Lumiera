/*
  MEDIA-IMPL-LIB.hpp  -  facade interface to access an external media handling library

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file media-impl-lib.hpp
 ** Abstraction to represent (metadata) of an external library to handle media data
 ** Lumiera delegates most actual media data processing to well established external
 ** libraries. While configuring the render process, a [stream type](\ref streamtype.hpp)
 ** framework is used to gain uniform access to heterogeneous media and processing
 ** facilities. At some point, we need to integrate the individual capabilities of
 ** the referenced libraries, though. This will be done with adapter implementations,
 ** where the interface MediaImplLip defines those aspects actually used and required
 ** by our internal processing.
 */


#ifndef STEAM_CONTROL_MEDIA_IMPL_LIB_H
#define STEAM_CONTROL_MEDIA_IMPL_LIB_H


#include "steam/streamtype.hpp"



namespace steam {
namespace control {
  
  using lib::Symbol;
  
  
  class MediaImplLib
    {
    protected:
      virtual ~MediaImplLib() {};
      
      typedef StreamType::ImplFacade ImplFacade;
      typedef StreamType::ImplFacade::TypeTag TypeTag;
      typedef StreamType::ImplFacade::DataBuffer DataBuffer;
      
    public:
      virtual Symbol getLibID()  const =0;
      
      virtual ImplFacade const&  getImplFacade (TypeTag&) =0;
    };
  
  
}} // namespace steam::control
#endif /*STEAM_CONTROL_MEDIA_IMPL_LIB_H*/
