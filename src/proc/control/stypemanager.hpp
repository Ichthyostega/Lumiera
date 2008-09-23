/*
  STYPEMANAGER.hpp  -  entry point for dealing with media stream types
 
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


#ifndef CONTROL_STYPEMANAGER_H
#define CONTROL_STYPEMANAGER_H


#include "common/streamtype.hpp"



namespace control {
  
  using lumiera::Symbol;
  using lumiera::StreamType;
  
  
  class STypeManager
    {
      
    public:
      /** (re)-access a media stream type using
       *  just a symbolic ID. Effectively this queries a default */
      StreamType const& getType (Symbol sTypeID) ;
      
      /** build or retrieve a complete StreamType implementing the given Prototype */
      StreamType const& getType (StreamType::Prototype const& protoType) ;
      
      /** build or retrieve a complete StreamType incorporating the given implementation type */
      StreamType const& getType (StreamType::ImplFacade const& implType) ;

      /** build or retrieve an implementation (facade)
       *  utilizing a specific MediaImplLib and implementing the given Prototype.
       *  @todo find out if this one is really necessary, because it is especially tricky */
      StreamType::ImplFacade const& getImpl (Symbol libID, StreamType::Prototype const& protoType) ;
      
      /** build or retrieve an implementation (facade)
       *  wrapping up the actual implementation as designated by the rawType tag,
       *  which needs to be an implementation type of the mentioned MediaImplLib */
      StreamType::ImplFacade const& getImpl (Symbol libID, StreamType::ImplFacade::TypeTag rawType) ;
      
      
      
      ////////////////TODO: design a mechanism allowing to add MediaImplLib implementations by plugin......
    };
  
  
} // namespace control


namespace proc_interface {
  
  using control::STypeManager;


} // namespace proc_interface
#endif
