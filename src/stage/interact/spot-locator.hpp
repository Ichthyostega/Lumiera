/*
  SPOT-LOCATOR.hpp  -  conduct and direct a global centre of activity

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file spot-locator.hpp
 ** Navigating a centre of user activity globally.
 ** When the user works with the application, we conceive this activity to be happening
 ** _"somewhere",_ at a _location_ within user interface space. We call this place where the
 ** user "is" and does its work *the Spot* -- to be seen both as a location and some part
 ** of the interface lit up by a spotlight beam. The SpotLocator implements the technical
 ** gear necessary to move this spot from one place in the interface to another.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** 
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef STAGE_INTERACT_SPOT_LOCATOR_H
#define STAGE_INTERACT_SPOT_LOCATOR_H

#include "stage/gtk-base.hpp"
#include "lib/nocopy.hpp"

//#include <string>
//#include <memory>


namespace stage {
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  
  
  
  /**
   * Control and navigate the global centre of activity
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class SpotLocator
    : util::NonCopyable
    {
      
    public:
      SpotLocator();
     ~SpotLocator();
      
    private:
      
    };
  
  
  
}}// namespace stage::interact
#endif /*STAGE_INTERACT_SPOT_LOCATOR_H*/
