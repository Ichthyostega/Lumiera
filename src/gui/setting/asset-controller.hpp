/*
  ASSET-CONTROLLER.hpp  -  asset management UI top level

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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
 ** @see proc::mobject::Session
 ** @see proc::asset::Asset
 ** @see timeline-controller.hpp
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI_SETTING_ASSET_CONTROLLER_H
#define GUI_SETTING_ASSET_CONTROLLER_H

//#include "gui/gtk-base.hpp"
#include "gui/model/controller.hpp"

//#include <string>
//#include <memory>


namespace gui {
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
      
      /** content population and manipulation via UI-Bus */
      void buildMutator (lib::diff::TreeMutator::Handle)  override;
    
    public:
      AssetController (ID identity, ctrl::BusTerm& nexus);
     ~AssetController();
      
    private:
      
    };
  
  
  
}}// namespace gui::setting
#endif /*GUI_SETTING_ASSET_CONTROLLER_H*/
