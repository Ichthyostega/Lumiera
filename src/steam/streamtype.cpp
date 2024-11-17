/*
  StreamType  -  classification of media stream types 

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file streamtype.cpp
 ** Implementation of a _stream type_ framework
 */


#include "steam/streamtype.hpp"


namespace steam {

   /** @internal defined here non-inline place the vtable in this object file.*/
   StreamType::ImplFacade::ImplFacade (Symbol libID)
     : libraryID(libID)
     { }

  

} // namespace steam
