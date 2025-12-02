/*
  WorkSite  -  a place within interface space where work is done

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file work-site.cpp
 ** Implementation aspects of the WorkSite abstraction.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** 
 */


#include "stage/interact/work-site.hpp"
//#include "stage/ctrl/global-ctx.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;


namespace stage {
namespace interact {
  
  
  
  // dtors via smart-ptr invoked from here...
  WorkSite::~WorkSite()
    { }
  
  
  /**
   * Define a new coherent location within _"interface space"_ for future interaction.
   */
  WorkSite::WorkSite()
    { }
  
  
  /** */
  
  
  
}}// namespace stage::interact
