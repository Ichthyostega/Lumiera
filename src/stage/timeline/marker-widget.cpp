/*
  MarkerWidget  -  display of a marker in timeline or within clips

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


/** @file marker-widget.cpp
 ** Implementation of marker display.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#include "stage/gtk-base.hpp"
#include "include/ui-protocol.hpp"
#include "stage/timeline/marker-widget.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
using lib::diff::TreeMutator;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  
  
  
  
  MarkerWidget::MarkerWidget (ID identity, ctrl::BusTerm& nexus)
    : Widget{identity, nexus}
    , kind_{MARK}
    , name_{identity.getSym()}  // ID symbol as name fallback
    {
      UNIMPLEMENTED ("how to attach the Marker, depending on the actual context");
    }
  
  
  MarkerWidget::~MarkerWidget()
  {
  }
  
  
  
  
  void
  MarkerWidget::buildMutator (TreeMutator::Handle buffer)
  {
    buffer.create (
      TreeMutator::build()
        .change(ATTR_name, [&](string val)
            {
              name_ = val;
            })
        .change(META_kind, [&](string val)
            {
              if (val == "LOOP") kind_ = LOOP;
              else               kind_ = MARK;
            }));
  }
  
  /** update and re-attach the presentation widget into its presentation context. 
   *  Will be called during the "re-link phase" of DisplayEvaluation, after the
   *  timeline layout has been (re)established globally. Often, this incurs
   *  attaching the presentation widget (ClipDelegate) at a different actual
   *  position onto the drawing canvas, be it due to a zoom change, or
   *  as result of layout re-flow.
   */
  void
  MarkerWidget::relink()
  {
    UNIMPLEMENTED ("how to re-attach the Marker, depending on the actual context");
  }
  
  
  
  
}}// namespace stage::timeline
