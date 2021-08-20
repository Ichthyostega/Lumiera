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
 ** ## The View-Spec DSL
 ** 
 ** The goal for the Lumiera UI is not to totally hard-wire the overall structure. Rather, it shall be
 ** possible to "allocate" a view based of rules where to place it within the existing UI structure.
 ** This is especially important, since we expect much editing work to be done in a multi-monitor setup,
 ** where the UI has to span several top-level windows, and allow for location of views in accordance
 ** to the individual editor's habits.
 ** 
 ** In the current state (2017/2021) the foundation for such a rule based view allocation was worked out,
 ** but all the possibly quite elaborated details of hooking into the actual widget tree was postponed,
 ** until we have a more mature implementation of actual presentation widgets in place.
 ** 
 ** @todo as of 2017 .. 2021, this is a half-finished draft and postponed in favour of building the actually visible timeline display first
 ** 
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 ** @see tangible.hpp
 ** @see bus-term.hpp
 ** @see entry-id.hpp
 ** 
 */

#ifndef STAGE_ID_SCHEME_H
#define STAGE_ID_SCHEME_H


#include "lib/symbol.hpp"
#include "lib/idi/entry-id.hpp"
#include "stage/interact/view-spec-dsl.hpp"


/* === forward declarations === */
namespace steam {
  namespace asset {
    class Timeline;
    
    namespace meta {
      class ErrorLog;
    }
  }
}

namespace stage {
  namespace timeline {
    class TimelineController;
  }
  namespace widget {
    class ErrorLogDisplay;
  }
  namespace idi {
    
    using lib::Literal;
    using lib::idi::EntryID;
    using interact::UICoord;
    
    
    using ID = lib::idi::BareEntryID const&;
    
    /* === primary component view types === */
    using TimelineView = timeline::TimelineController;
    using ErrorLogView = widget::ErrorLogDisplay;
    
    
    /**
     * Descriptor for the Timeline UI
     */
    template<>
    struct Descriptor<TimelineView>
      {
        using Model = steam::asset::Timeline;
      };
    
    
    /**
     * Descriptor for error log display within the UI
     */
    template<>
    struct Descriptor<ErrorLogView>
      {
        using Model = steam::asset::meta::ErrorLog;
        
        ///////////////////////////////////////////////////////////////////////////////////////////TICKET #1105 : consider use of a DSL to configure component view access 
        //
        // alloc = onePerWindow
        // locate = within(InfoBoxPanel)
      };
    
/*
///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1105 : DSL design draft... 
// Timeline
// add to group of timelines within the timelinePanel

alloc = unlimited
locate = perspective(edit).panel(timeline)
          or panel(timeline)
          or currentWindow().panel(timeline).create()

// Viewer
// here multiple alternatives are conceivable
// - allow only a single view instance in the whole application

alloc = onlyOne
locate = external(beamer)
          or view(viewer)
          or perspective(mediaView).panel(viewer)
          or panel(viewer)
          or firstWindow().panel(viewer).view(viewer).create()

// - allow two viewer panels (the standard layout of editing applications)

alloc = limitPerWindow(2)
locate = perspective(edit).panel(viewer)
          or currentWindow().panel(viewer)
          or panel(viewer)
          or currentWindow().panel(viewer).create()

// (Asset)Bin
// within the dedicated asset panel, add to the appropriate group for the kind of asset

alloc = unlimited
locate = currentWindow().perspective(edit).tab(assetType())
          or perspective(asset).view(asset)
          or tab(assetType())
          or view(asset).tab(assetType()).create()
          or firstWindow().panel(asset).view(asset).create()

// Error-Log
// use the current `InfoBoxPanel` if such exists, fall back to using a single view on the primary window

alloc = limitPerWindow(1)
locate = currentWindow().panel(infobox)
          or view(error)
          or panel(infobox)
          or firstWindow().panel(infobox).view(error).create()

*/    
    
    
  }//namespace stage::idi
} // namespace stage
#endif /*STAGE_ID_SCHEME_H*/
