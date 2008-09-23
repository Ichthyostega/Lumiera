/*
  STypeManager  -  entry point for dealing with media stream types
 
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
 
* *****************************************************/


#include "proc/lumiera.hpp"
#include "proc/control/stypemanager.hpp"

namespace control {
  
  using lumiera::StreamType;
  
  
  /** */
  StreamType const&
  STypeManager::getType (Symbol sTypeID) 
  {
    UNIMPLEMENTED ("get type just by symbolic ID (query defaults manager)");
  }
  
  
  StreamType const& 
  STypeManager::getType (StreamType::Prototype const& protoType)
  {
    UNIMPLEMENTED ("build complete StreamType based on prototype; may include querying defaults manager");
  }
  
  
  StreamType const&
  STypeManager::getType (StreamType::ImplFacade const& implType)
  {
    UNIMPLEMENTED ("build complete StreamType round the given implementation type");
  }
  
  
  StreamType::ImplFacade const&
  STypeManager::getImpl (Symbol libID, StreamType::Prototype const& protoType)
  {
    UNIMPLEMENTED ("wire up implementation in compliance to a prototype and using a specific MediaImplLib. really tricky.... ");
  }
  
  
  StreamType::ImplFacade const&
  STypeManager::getImpl (Symbol libID, StreamType::ImplFacade::TypeTag rawType)
  {
    UNIMPLEMENTED ("STypeManager basic functionality: wire up implementation facade (impl type) from given raw type of the library");
  }
  
  
  
} // namespace control
