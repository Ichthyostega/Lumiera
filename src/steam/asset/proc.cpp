/*
  Proc(Asset)  -  key abstraction: data-processing assets

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file proc.cpp
 ** Implementation details regarding the Asset representation of a media processing facility
 */


#include "steam/assetmanager.hpp"
#include "steam/asset/proc.hpp"
#include "lib/util.hpp"
#include "include/logging.h"


namespace steam {
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
