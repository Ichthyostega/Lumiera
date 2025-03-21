/*
  ImplFacadeGAVL  -  facade for integrating the GAVL media handling library

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file libgavl.cpp
 ** Implementation details of using the GAVL library for processing
 ** @todo 2016 evaluate if libGAVL is still active and maintained
 */


#include "steam/common.hpp"
#include "steam/external/libgavl.hpp"
#include "steam/control/stypemanager.hpp"

extern "C" {
#include <gavl/gavl.h>
}


namespace steam {
namespace external {
    
    using control::STypeManager;
    using control::ON_STREAMTYPES_RESET;
    using lumiera::LifecycleHook;
    
    void
    provide_GAVL_stream_implementation_types ()
      {
        STypeManager& typeManager = STypeManager::instance();
        UNIMPLEMENTED ("wire up a ImplFacade for GAVL implemented media streams");
      }
    
    namespace { // internal functionality
    
        LifecycleHook _register_gavl_types_ (ON_STREAMTYPES_RESET, &provide_GAVL_stream_implementation_types);         
    }
  
  
  /** 
   * Use an type information struct, which actually has to be 
   * a GAVL frame type (TODO), to wire up an ImplFacade such
   * as to deal with GAVL data frames of this type.
   * @todo fill in the actual GAVL frame type
   * @todo how to distinguish the audio and the video case?
   */
  ImplFacadeGAVL const&
  LibGavl::getImplFacade (TypeTag&)
  {
    TODO ("any chance to verify that the TypeTag actually points to a GAVL frame type descriptor?");
    UNIMPLEMENTED ("wire up an impl facade with the correct GAVL lib functions for the data type in question");
  }
  
  
  
}} // namespace steam::external
