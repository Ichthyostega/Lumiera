/*
  AssetController  -  asset management UI top level

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file asset-controller.cpp
 ** Implementation of the asset management UI top level.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** 
 */


#include "stage/setting/asset-controller.hpp"
//#include "stage/ctrl/global-ctx.hpp"
#include "lib/diff/tree-mutator.hpp"
//#include "lib/util.hpp"

//using util::cStr;
using lib::diff::TreeMutator;
//using util::isnil;


namespace stage {
namespace setting {
  
  
  
  // dtors via smart-ptr invoked from here...
  AssetController::~AssetController()
    { }
  
  
  /**
   * Establish the UI section to deal with _asset management_.
   * AssetController is connected to the UI-Bus and will be populated
   * with content from the session via _diff messages._
   * @see tree-mutator.hpp
   */
  AssetController::AssetController (ID identity, ctrl::BusTerm& nexus)
    : model::Controller{identity, nexus}
    { }
  
  
  void
  AssetController::buildMutator (TreeMutator::Handle buffer)
  {
    buffer.emplace(
      TreeMutator::build()
    );
    UNIMPLEMENTED ("create a sensible binding between AssetManager in the section and AssetController in the UI");
  }
  
  
  /** */
  
  
  
}}// namespace stage::setting
