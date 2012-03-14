/*
  MEDIAIMPLLIB.hpp  -  interface providing a facade to an media handling library

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


#ifndef CONTROL_MEDIAIMPLLIB_H
#define CONTROL_MEDIAIMPLLIB_H


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
#endif
