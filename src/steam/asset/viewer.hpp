/*
  VIEWER.hpp  -  asset corresponding to a viewer element in the GUI

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file viewer.hpp
 ** structural element corresponding to a viewer in the GUI.
 ** This Asset marks an attachment point, allowing other (output producing) 
 ** elements to be connected to ("viewer attachment"). The typical standard case
 ** is a Timeline: It features a set of global pipes (busses), which collect all
 ** produced data. Yet without an explicit output connection, this data can't be
 ** generated, because in Lumiera, actual output is always retrieved ("pulled")
 ** starting from an output sink. Thus, in order to \em perform (play, render)
 ** the timeline, an "view connection" needs to be established: this connection
 ** is represented by an session::BindingMO, linking the Timeline to an
 ** asset::Viewer. Consequently, the same output mapping and translation
 ** mechanism used for Sequence-Timeline- (and VirtualClip-)Bindings
 ** is employed in this situation to figure out the real output port.
 ** 
 ** @todo WIP-WIP-WIP as of 5/11
 ** 
 ** @see Session
 ** @see Timeline
 ** @see StructFactory
 **
 */


#ifndef ASSET_VIEWER_H
#define ASSET_VIEWER_H

#include "proc/asset/struct.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"
#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/session/binding.hpp"         ////TODO avoidable??
#include "lib/p.hpp"
#include "lib/element-tracker.hpp"


//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace proc {
namespace mobject {
namespace session {
  
  class Binding;
  typedef MORef<Binding> RBinding;
}}


namespace asset {
  
  
  class Viewer;
  typedef lib::P<Viewer> PViewer;
  
  
  /**
   * TODO type comment
   */
  class Viewer
    : public Struct
//  , public lib::AutoRegistered<Viewer>
    {
      
      Viewer (Ident const&); /////TODO ctor params????
      
    public:
      /** create and register a new Timeline instance */
//    static PTimeline create (Asset::Ident const& idi, RBinding const& sequenceBinding);
      
    protected:
      virtual void unlink ();
      
    };
    
  
  
  
///////////////////////////TODO currently just fleshing the API
  
  
}} // namespace proc::asset
#endif
