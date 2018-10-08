/*
  TimelineController  -  coordinate operation of timeline display

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file timeline-controller.cpp
 ** Implementation details of timeline operation management and control.
 ** - we build a binding to allow TimelineController to handle mutation messages
 **   on behalf of "the timeline". While the setup of a Timeline is quite flexible
 **   at the session level, here, when it comes to UI presentation, it can be
 **   boiled down to
 **   + a name
 **   + a single mandatory root track (which in turn could hold nested tracks)
 ** - thus we get a rather simple mapping, with some fixed attributes and no
 **   flexible child collection. The root track is implemented as TrackPresenter.
 ** 
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** @see TimelineWidget
 ** 
 */


#include "gui/gtk-base.hpp"
#include "include/ui-protocol.hpp"
#include "gui/timeline/timeline-controller.hpp"
#include "gui/timeline/track-presenter.hpp"
#include "gui/timeline/marker-widget.hpp"

//#include "gui/workspace/workspace-window.hpp"
//#include "gui/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"
//#include <algorithm>
//#include <cstdlib>



//using util::_Fmt;
using lib::diff::TreeMutator;
using lib::diff::collection;
using std::make_unique;
//using std::shared_ptr;
//using std::weak_ptr;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace gui {
namespace timeline {
  
//const int TimelineWidget::TrackPadding = 1;
//const int TimelineWidget::HeaderWidth = 150;
//const int TimelineWidget::HeaderIndentWidth = 10;
  
  
  
  
  TimelineController::TimelineController (ID identity, ID trackID, ctrl::BusTerm& nexus)
    : Controller{identity, nexus}
    , markers_{}
    , fork_{new TrackPresenter{trackID, nexus}}
    , name_{identity.getSym()}              // fallback initialise name from human-readable ID symbol 
    {
      UNIMPLEMENTED ("how to make the controller operative...");
    }
  
  
  TimelineController::~TimelineController()
  {
  }
  
  
  /**
   * @internal this method is invoked by the UI-Bus when dispatching a MutationMessage...
   * @remarks this is likely the first occasion a casual reader sees such a binding function,
   * thus some explanations might be helpful. This is part of the »diff framework«: we use
   * messages to _communicate changes on structured data._ We might as well just use a common
   * object model, but we refrain from doing so, to avoid tight coupling, here between the
   * core logic and the structures in the UI. Rather we assume that both sides share a
   * roughly compatible understanding regarding the structure of the session model.
   * Exchanging just diff messages allows us to use private implementation data structures
   * in the UI as we see fit, without the danger of breaking anything in the core. And vice
   * versa. You may see this as yet another way of data binding between model and view.
   * The TreeMutator helps to accomplish this binding between a generic structure description,
   * in our case based on GenNode elements, and the private data structure, here the private
   * object fields and the collection of child objects within TimelineController. To ease this
   * essentially "mechanic" and repetitive task, the TreeMutator offers some standard building
   * blocks, plus a builder DSL, allowing just to fill in the flexible parts with some lambdas.
   * Yet still, the technical details of getting this right can be tricky, especially since
   * it is very important to set up those bindings in the right order. Basically we build
   * a stack of decorators, so what is mentioned last will be checked first. Effectively
   * this creates a structure of "onion layers", where each layer handles just one aspect
   * of the binding. This works together with the convention that the diff message must
   * mention all changes regarding one group (or kind) of elements together and completely.
   * This is kind of an _object description protocol_, meaning that the diff has to mention
   * the metadata (the object type) first, followed by the "attributes" (fields) and finally
   * nested child objects. And nested elements can be handled with a nested diff, which
   * recurses into some nested scope. In the example here, we are prepared to deal with
   * two kinds of nested scope:
   * - the _fork_ (that is the tree of tracks) is a nested structure
   * - we hold a collection of marker child objects, each of which can be entered
   *   as a nested scope.
   * For both cases we prepare a way to build a _nested mutator_, and in both cases this
   * is simply achieved by relying on the common interface of all those "elements", which
   * is gui::model::Tangible and just happens to require each such "tangible" to offer
   * a mutation building method, just like this one here. Simple recursive programming.
   * @see DiffComplexApplication_test
   */
  void
  TimelineController::buildMutator (TreeMutator::Handle buffer)
  {
    using PMarker = unique_ptr<MarkerWidget>;
    
    buffer.create (
      TreeMutator::build()
        .attach (collection(markers_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {                                            // »Selector« : require object-like sub scope
                    return spec.data.isNested();
                  })
               .matchElement ([&](GenNode const& spec, PMarker const& elm) -> bool
                  {                                            // »Matcher« : how to know we're dealing with the right object
                    return spec.idi == ID(elm);
                  })
               .constructFrom ([&](GenNode const& spec) -> PMarker
                  {                                            // »Constructor« : what to do when the diff mentions a new entity
                    return make_unique<MarkerWidget>(spec.idi, this->uiBus_);
                  })
               .buildChildMutator ([&](PMarker& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {                                            // »Mutator« : how to apply the diff recursively to a nested scope
                    if (ID(target) != subID) return false;     //  - require match on already existing child object
                    target->buildMutator (buff);               //  - delegate to child to build nested TreeMutator
                    return true;
                  }))
        .mutateAttrib(ATTR_fork, [&](TreeMutator::Handle buff)
            {                                                  // »Attribute Mutator« : how enter an object field as nested scope
              REQUIRE (fork_);
              fork_->buildMutator(buff);
            })
        .change(ATTR_name, [&](string val)
            {                                                  // »Attribute Setter« : how assign a new value to some object field
              name_ = val;
            }));
  }
  
  
  
}}// namespace gui::timeline
