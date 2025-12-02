/*
  ASSET-CONTROLLER.hpp  -  asset management UI top level

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file asset-controller.hpp
 ** User interface for asset management.
 ** Aside of the media objects to be edited, the session holds a section dedicated
 ** to the bookkeeping aspects, where all of the assets relevant to the project are
 ** listed, catalogued and tracked. There is a corresponding section in the UI to
 ** offer this bookkeeping view -- which includes the tremendously important hierarchy
 ** of media and clip bins, where all of the material is organised and prepared for
 ** editing. Another topic subsumed under _asset management,_ yet presented separately
 ** is anything related to settings and project setup.
 ** 
 ** As with any part of the UI, there is some correspondence to session model components
 ** and a connection via UI-Bus and diff framework. We get one top-level UI element, the
 ** AssetController, to correspond to the asset::AssetManager in the session. The
 ** basically fixed structure of asset categories and subsections is reflected in
 ** likewise fixed access operations in this controller, allowing to enter a
 ** dedicated UI for each of the asset subsections...
 ** 
 ** @todo WIP 2/2017 early draft / a new UI backbone
 ** 
 ** @see steam::mobject::Session
 ** @see steam::asset::Asset
 ** @see timeline-controller.hpp
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef STAGE_SETTING_ASSET_CONTROLLER_H
#define STAGE_SETTING_ASSET_CONTROLLER_H

//#include "stage/gtk-base.hpp"
#include "stage/model/controller.hpp"

//#include <string>
//#include <memory>


namespace stage {
namespace setting {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  
  
  
  /**
   * Top level controller for the asset management section in the UI.
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class AssetController
    : public model::Controller
    {
      ///////TODO create UI representation for the asset subsections
    
    public:
      AssetController (ID identity, ctrl::BusTerm& nexus);
     ~AssetController();
      
      /** content population and manipulation via UI-Bus */
      void buildMutator (lib::diff::TreeMutator::Handle)  override;
      
    private:
      
    };
  
  
  
}}// namespace stage::setting
#endif /*STAGE_SETTING_ASSET_CONTROLLER_H*/
