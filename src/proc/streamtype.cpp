/*
  StreamType  -  classification of media stream types 

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

* *****************************************************/


/** @file streamtype.cpp
 ** Implementation of a _stream type_ framework
 */


#include "proc/streamtype.hpp"


namespace proc {

   /** @internal defined here non-inline place the vtable in this object file.*/
   StreamType::ImplFacade::ImplFacade (Symbol libID)
     : libraryID(libID)
     { }

  

} // namespace proc
