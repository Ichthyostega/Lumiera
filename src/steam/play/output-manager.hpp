/*
  OUTPUT-MANAGER.hpp  -  handling all the real external output connections

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file output-manager.hpp
 ** A global service to handle all external output connections.
 **
 ** @see output-manager-test.cpp  ////TODO
 */


#ifndef STEAM_PLAY_OUTPUT_MANAGER_H
#define STEAM_PLAY_OUTPUT_MANAGER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/play/output-slot.hpp"
#include "steam/mobject/model-port.hpp"

//#include <string>
//#include <vector>
#include <memory>


namespace lumiera {
namespace error {
  LUMIERA_ERROR_DECLARE (CANT_PLAY); ///< unable to build playback or render process for this configuration
}}

namespace steam {
namespace play {
  
//using std::string;
//using std::vector;
  using std::shared_ptr;
  
  
  
  
  /**************************************************//**
   * Management of external Output connections.
   * 
   * @todo need a concrete implementation
   * @todo need a unit-test
   * @todo write Type comment
   */
  class OutputManager
    : util::NonCopyable
    {
    public:
      virtual ~OutputManager() { }
      
      
      virtual OutputSlot& getOutputFor (mobject::ModelPort port)  =0; 
    };
  
  typedef shared_ptr<OutputManager> POutputManager;
  
  
}} // namespace steam::play
#endif
