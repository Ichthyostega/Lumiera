/*
  Proc(Asset)  -  key abstraction: data-processing assets

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


/** @file proc.cpp
 ** Implementation details regarding the Asset representation of a media processing facility
 */


#include "proc/assetmanager.hpp"
#include "proc/asset/proc.hpp"
#include "lib/util.hpp"
#include "include/logging.h"


namespace proc {
namespace asset {
  
  
  ProcFactory Proc::create;  ///< storage for the static ProcFactory instance
  
  
  
  /** Factory method for Processor Asset instances. ....
   *  @todo actually define
   *  @return an Proc smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Proc subclass ctor.
   */
  ProcFactory::PType 
  ProcFactory::operator() (Asset::Ident& key) ////TODO
  {
    UNIMPLEMENTED ("Proc-Factory");
  }
  
  
  
}} // namespace asset
