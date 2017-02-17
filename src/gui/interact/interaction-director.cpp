/*
  interactionDirector  -  Global UI Manager

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

* *****************************************************/


/** @file interaction-director.cpp
 ** Implementation parts of the top-level controller within the UI.
 */


#include "gui/gtk-base.hpp"
#include "gui/ctrl/global-ctx.hpp"
#include "gui/interact/interaction-director.hpp"
#include "gui/ui-bus.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "proc/mobject/session/root.hpp"
#include "lib/diff/tree-mutator.hpp"
//#include "gui/ui-bus.hpp"
//#include "lib/util.hpp"

//#include <memory>
//#include <list>

//using util::isnil;
//using std::list;
//using std::shared_ptr;
using lib::diff::TreeMutator;


namespace gui {
namespace interact {
  
  using ctrl::GlobalCtx;
  
  
  // emit VTables here...
  InteractionDirector::~InteractionDirector()
    { }
  
  
  InteractionDirector::InteractionDirector (GlobalCtx& globals)
    : model::Controller(proc::mobject::session::Root::getID(), globals.uiBus_.getAccessPoint())
    , globalCtx_(globals)
    { }
  
  
  void
  InteractionDirector::buildMutator (TreeMutator::Handle buffer)
  {
//  using Attrib = std::pair<const string,string>;
//  using lib::diff::collection;
    
    buffer.create (
      TreeMutator::build()
    );
    UNIMPLEMENTED ("create a sensible binding between root-controller and root-model element");
  }

  
}}// namespace gui::interact
