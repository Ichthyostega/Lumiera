/*
  Meta(Asset)  -  key abstraction: metadata and organisational asset
 
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


#include "proc/assetmanager.hpp"
#include "proc/asset/meta.hpp"
#include "lib/util.hpp"
#include "common/logging.h"


namespace asset
  {
  
  namespace // Implementation details
  {
    /** helper: .....*/
  } 


  
  MetaFactory Meta::create;  ///< storage for the static MetaFactory instance
  
  
  
  /** Factory method for Metadata Asset instances. ....
   *  @todo actually define
   *  @return an Meta smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Meta subclass ctor.
   */
  MetaFactory::PType 
  MetaFactory::operator() (Asset::Ident& key) ////TODO
  {
    UNIMPLEMENTED ("Meta-Factory");
  }
  
  
  
} // namespace asset
