/*
  VIEWER.hpp  -  asset corresponding to a viewer element in the GUI

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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

#include "steam/asset/struct.hpp"
//#include "steam/mobject/mobject.hpp"
//#include "steam/mobject/placement.hpp"
#include "steam/mobject/mobject-ref.hpp"
//#include "steam/mobject/session/binding.hpp"         ////TODO avoidable??
#include "lib/p.hpp"
#include "lib/element-tracker.hpp"


//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace steam {
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
  
  
}} // namespace steam::asset
#endif
