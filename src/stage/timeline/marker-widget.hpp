/*
  MARKER-WIDGET.hpp  -  display of a marker in timeline or within clips

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file marker-widget.hpp
 ** Widget to show a marker at various places.
 ** Markers are attached by placement and serve as a reference point, for navigation,
 ** for special functions, but also to be referenced by other placements. The display
 ** of a marker is just a label, typically abridged. But the MarkerWidget itself
 ** participates in the structural model of the UI and thus is a model::Tangible and
 ** may receive messages over the UI-Bus.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#ifndef STAGE_TIMELINE_MARKER_WIDGET_H
#define STAGE_TIMELINE_MARKER_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/model/widget.hpp"
#include "stage/widget/element-box-widget.hpp"

//#include "lib/util.hpp"

//#include <memory>
#include <string>



namespace stage  {
namespace timeline {
  
  using std::string;
  
  enum MarkerKind {
    MARK,
    LOOP
  };
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   * @todo 9/2022 baseclass sigc::trackable is ambiguous, we inherit it from two chains (diamond of death)
   */
  class MarkerWidget
    : public model::Widget
    , public widget::ElementBoxWidget
    {
      MarkerKind kind_;
      string     name_;
      
    public:
      /**
       * @param identity used to refer to a corresponding marker entity in the Session
       * @param nexus a way to connect this Controller to the UI-Bus.
       */
      MarkerWidget (ID identity, ctrl::BusTerm& nexus);
      
     ~MarkerWidget();
      
      
      /** set up a binding to respond to mutation messages via UiBus */
      virtual void buildMutator (lib::diff::TreeMutator::Handle)  override;
      
      void relink();

    private:/* ===== Internals ===== */
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_MARKER_WIDGET_H*/
