/*
  Navigator  -  controller for global navigation through interface space

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file navigator.cpp
 ** Implementation of global interface navigation mechanisms.
 ** Especially we implement the LocationQuery interface, which exposes the structures
 ** of the UI as an abstracted, tree-shaped topology. This task adds up to levelling all the
 ** specifics of accessing individual components and to assemble them into a virtual component tree.
 ** The actual details of component access are thereby delegated to the ViewLocator, which is a sibling
 ** service maintained by the InteractionDirector.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** @todo WIP 1/2018 integrating the concept of UI-Coordinate navigation and resolution. Still WIP-WIP-WIP...
 */


#include "stage/interact/navigator.hpp"
#include "stage/interact/spot-locator.hpp"
//#include "stage/ctrl/global-ctx.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;


namespace stage {
namespace interact {
  
  
  
  // dtors via smart-ptr invoked from here...
  Navigator::~Navigator()
    { }
  
  
  Navigator::Navigator (SpotLocator& spotLoc, ViewLocator& viewLoc)
    : spotLocator_{spotLoc}
    , viewLocator_{viewLoc}
    { }
  
  
  /* ==== implementing the LocationQuery API ==== */
  
  Literal
  Navigator::determineAnchor (UICoord const& path)
  {
    UNIMPLEMENTED ("LocationQuery in real UI: resolve anchor point of given UI-Coordinates");
  }
  
  
  size_t
  Navigator::determineCoverage (UICoord const& path)
  {
    UNIMPLEMENTED ("LocationQuery in real UI: determine explicit coverage of given UI-Coordinates");
  }
  
  
  LocationQuery::ChildIter
  Navigator::getChildren (UICoord const& path, size_t pos)
  {
    UNIMPLEMENTED ("LocationQuery in real UI: build child iterator rooted at given point in the UI tree");
  }
  
  
  
  /** */
  
  
  
}}// namespace stage::interact
