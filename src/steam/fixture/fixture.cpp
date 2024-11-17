/*
  Fixture  -  the (low level) representation of the Session with explicit placement data

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file fixture.cpp
 ** Implementation of the Fixture datastructure
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "steam/fixture/fixture.hpp"
#include "include/logging.h"

namespace steam {
namespace fixture {
  
  
  
  /** TODO: a placeholder for the Operation needed for
   *  wiring the Automation providers in the Build process
   */
  Auto<double>*
  Fixture::getAutomation ()
  {
    UNIMPLEMENTED ("getAutomation from Fixture");
    return 0;
  }
  
  
  /** @todo self-verification of the fixture? necessary? */
  bool
  Fixture::isValid()  const
  {
    TODO ("actually do a self-verification of the Fixture");
    return true;
  }
  
  
  
  
}} // namespace steam::fixture
