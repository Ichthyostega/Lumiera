/*
  MEDIA-IMPL-LIB.hpp  -  facade interface to access an external media handling library

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef PROC_CONTROL_MEDIA_IMPL_LIB_H
#define PROC_CONTROL_MEDIA_IMPL_LIB_H


#include "proc/streamtype.hpp"



namespace proc {
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
  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_MEDIA_IMPL_LIB_H*/
