/*
  LILBGAVL.hpp  -  facade for integrating the GAVL media handling library
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef LIB_EXTERN_LIBGAVL_H
#define LIB_EXTERN_LIBGAVL_H


#include "proc/control/mediaimpllib.hpp"



namespace lib {
  namespace external {
  
  
  using lumiera::Symbol;
  
  using lumiera::StreamType;
  typedef StreamType::ImplFacade ImplFacade;
  typedef StreamType::ImplFacade::TypeTag TypeTag;
  
  
  class LibGavl;
  
  /**
   * Concrete media lib implementation facade
   * allowing to work with GAVL data frames and types
   * in an implementation agnostic way.
   * @note GAVL types are automagically registered into the
   * control::STypeManager on reset and thus are always available.
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
      
      virtual DataBuffer* createFrame ()  const;
    };
  
  
  class LibGavl
    : public control::MediaImplLib
    {
    protected:
      
    public:
      virtual Symbol getLibID()  const  { return "GAVL"; }
      
      virtual ImplFacadeGAVL const&  getImplFacade (TypeTag*);
    };
  
  
  
  } // namespace external
  
} // namespace lib
#endif
