/*
  ELEM-ACCESS-DIR.hpp  -  service to access generic elements in the UI

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file elem-access-dir.hpp
 ** A service to discover and access raw UI elements in a cross cutting way.
 ** This is the actual low-level implementation of the model::ElementAccess interface
 ** Tightly coupled to the internals of Lumiera's GTK UI, this implementation embodies
 ** all the inside knowledge necessary to navigate over the various levels (Windows,
 ** panels, views, specific components) to get at those elements abstracted as
 ** UI-Coordinates.
 ** 
 ** @note as of 4/2018 this is a first draft and will remain unimplemented for the time being
 ** @todo WIP-WIP-WIP need to learn more about the concrete UI implementation  ///////////////////////TICKET #1134
 ** 
 ** @see view-locator.hpp
 ** @see navigator.hpp
 ** 
 */


#ifndef STAGE_CTRL_ELEM_ACCESS_DIR_H
#define STAGE_CTRL_ELEM_ACCESS_DIR_H


#include "lib/error.hpp"
#include "stage/model/element-access.hpp"
#include "stage/ctrl/window-locator.hpp"
#include "stage/ctrl/panel-locator.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


  
namespace stage {
namespace ctrl{
  
//  using util::isnil;
//  using std::string;
  using interact::UICoord;
  
  
  /**
   * Low-level service to navigate the internals of the Lumiera GTK UI.
   * @todo mostly not yet implemented as of 4/2018 -- need to learn more about aforementioned internals.
   */
  class ElemAccessDir
    : public model::ElementAccess
    {
      WindowLocator& windowLoc_;
      PanelLocator& panelLoc_;
      
    public:
      explicit
      ElemAccessDir (WindowLocator& windowLocatorService, PanelLocator& panelLocatorService)
        : windowLoc_{windowLocatorService}
        , panelLoc_{panelLocatorService}
        { }
      
      
      
      /* == ElementAccess interface == */
      
      RawResult
      performAccessTo (UICoord::Builder & target, size_t limitCreation)  override
        {
          UNIMPLEMENTED ("implementation of access function based on real UI widgets");
        }
      
      
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1134
          if (isDeaf())
            this->transmogrify (solution);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1134
      
      
    protected:
    };
  
  
  
  
  
  
  /** @internal in case
   */
  
  
  
  
  
}} // namespace stage::ctrl
#endif /*STAGE_CTRL_ELEM_ACCESS_DIR_H*/
