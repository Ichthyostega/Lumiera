/*
  Subsys  -  interface for describing an application part to be handled by main() 

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file subsys.cpp
 ** Implementation details of a »subsystem descriptor«.
 ** The application is split into various such subsystems,
 ** which are prompted by main() to start/stop.
 ** 
 ** @see subsystem-runner.hpp
 ** @see lumiera::AppState
 ** @see facade.cpp
 ** @see main.cpp
 ** 
 */


#include "common/subsys.hpp"


namespace lumiera {
  
  
  
  
  Subsys::~Subsys()  { }
  
  
  
  Subsys&
  Subsys::depends (Subsys& prereq)
  {
    prereq_.push_back(&prereq);
    return *this;
  }
  
  
  
  bool
  Subsys::isRunning()  noexcept
  {
    return checkRunningState();
  }
  
  
  
  
  
} // namespace lumiera
