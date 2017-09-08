/*
  ID-SCHEME.hpp  -  naming and ID scheme definitions for the GUI

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


/** @file id-scheme.hpp
 ** Hard wired definitions for the object identification system used within the UI.
 ** 
 ** # Identities and element addressing
 ** 
 ** Within the UI-Layer, all components of global relevance are connected to the [UI-Bus](\ref ui-bus.hpp)
 ** and can thus be reached just by knowing their ID. This allows to address such UI components as
 ** [generic UI-Element](\ref model::Tangible), relying on a generic element manipulation protocol.
 ** Beyond this rather unspecific manipulation scheme, the UI relies on direct ownership relations.
 ** Typically, some element is created as result of another action and managed as child of some
 ** maintaining entity; generally speaking, UI elements live _free floating_ and are mostly
 ** interconnected by signals to react on user interaction. There is a rather limited
 ** [global UI-Context](\ref GlobalCtx) of directly interconnected backbone services,
 ** which allow to achieve activities cross-cutting the ownership relationship.
 ** 
 ** Element IDs are always formed as EntryID, typed to the corresponding type in the Session model.
 ** Thus, starting from a given model element, it is always possible to "cast" some message towards
 ** the corresponding UI view element, without actually knowing much about that element's implementation.
 ** 
 ** @note This header provides the basic definitions for easily accessing relevant UI elements. 
 ** 
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 ** @see tangible.hpp
 ** @see bus-term.hpp
 ** @see entry-id.hpp
 ** 
 */

#ifndef GUI_ID_SCHEME_H
#define GUI_ID_SCHEME_H


#include "lib/symbol.hpp"
#include "lib/idi/entry-id.hpp"


/* === forward declarations === */
namespace proc {
  namespace asset {
    class Timeline;
  }
}

namespace gui {
  namespace timeline {
    class TimelineController;
  }
  namespace widget {
    class ErrorLogWidget;
  }
  namespace idi {
    
    using lib::Literal;
    using lib::idi::EntryID;
    
    using ID = lib::idi::BareEntryID const&;
    
    /* === primary component view types === */
    using TimelineView = timeline::TimelineController;
    using ErrorLogView = widget::ErrorLogWidget;
    
    
    /** Generic Component View descriptors */
    template<class V>
    struct Descriptor
      {
        static_assert (not sizeof(V), "unknown generic view type");
      };
    
    
    /**
     * Descriptor for the Timeline UI
     */
    template<>
    struct Descriptor<TimelineView>
      {
        using Model = proc::asset::Timeline;
      };
    
    
    /**
     * Descriptor for error log display within the UI
     */
    template<>
    struct Descriptor<ErrorLogView>
      {
        ///////////////////////////////////////////////////////////////////////////////////////////TICKET #1105 : need a model placeholder to represent UI specific global entities
        
        ///////////////////////////////////////////////////////////////////////////////////////////TICKET #1105 : consider use of a DSL to configure component view access 
        //
        // count = onePerWindow
        // locate = within(InfoBoxPanel)
      };
    
/*
///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1105 : DSL design draft... 
// Timeline
// add to group of timelines within the timelinePanel

count = unlimited
locate = panel(timeline)

// Viewer
// here multiple alternatives are conceivable
// - allow only a single view instance in the whole application

count = onlyOne
locate = external(beamer)
                or perspective(mediaView), panel(viewer)
                or existingPanel(viewer)
                or firstWindow, panel(viewer)

// - allow two viewer panels (the standard layout of editing applications)

count = limitPerWindow(2)
locate = perspective(edit), existingPanel(viewer)
                or currentWindow, existingPanel(viewer)
                or existingPanel(viewer)
                or panel(viewer)

// (Asset)Bin
// within the dedicated asset panel, add to the appropriate group for the kind of asset

count = unlimited
locate = currentWindow, perspective(edit), existingPanel(asset), existingGroup
                or perspective(asset), panel(asset)
                or firstWindow, panel(asset)

// Error-Log
// use the current {{{InfoBoxPanel}}} if such exists, fall back to using a single view on the primary window

count = limitPerWindow(1)
locate = currentWindow, existingPanel(infobox)
                or firstWindow, panel(infobox)

*/    
    
    
  }//namespace gui::idi
} // namespace gui
#endif /*GUI_ID_SCHEME_H*/
