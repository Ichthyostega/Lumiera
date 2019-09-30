/*
  VIEW-HOOK.hpp  -  abstracted attachment to a canvas or display facility

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file view-hook.hpp
 ** Allow widgets to connect to a common shared presentation context.
 ** This is an abstraction to overcome the problem of cross-cutting a complex
 ** hierarchical widget structure in order to maintain a connection to some central
 ** presentation entity or canvas. We do not want a central "God class" to manage and
 ** remote-control the widgets, nor do we want the widgets to be aware of the hierarchical
 ** control structure they are part of. Yet still, widgets typically require to have some
 ** access to those shared central structures, especially if they need to "draw themselves".
 ** A widget must be able to attach itself to a presentation canvas, and it must be able
 ** to control its position thereon. As usual, we solve this problem by abstracting away
 ** the actual implementation of the central facility. So widgets get a stage::timeline::ViewHook
 ** as access point, which also manages the _lifecycle of this attachment:_ whenever the
 ** `ViewHook` is destroyed, the attachment is automatically untied and the widget
 ** is deregistered from the central canvas. Widgets thus may want to store the
 ** `ViewHook` as member.
 ** 
 ** @see ViewHook_test
 ** @todo WIP-WIP-WIP as of 9/2019
 ** 
 */


#ifndef STAGE_MODEL_VIEW_HOOK_H
#define STAGE_MODEL_VIEW_HOOK_H

//#include "stage/gtk-base.hpp"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"

//#include <memory>
//#include <utility>
//#include <vector>



namespace stage  {
namespace model {
  
//  using lib::Literal;
//  using util::isnil;
//  using std::forward;

  
  /**
   * Abstracted attachment onto a display canvas or similar central presentation context.
   * A `ViewHook` represents the connection of a Widget into the presentation facility, like
   * e.g. placing the widget onto a _canvas_ (`Gtk::Layout`). This way, the widget may control
   * details of its placements, while remaining agnostic regarding the implementation details
   * of the presentation context.
   * 
   * The prominent example for using a `ViewHook` is the stage::timeline::DisplayFrame maintained
   * by the TrackPresenter within the timeline UI. This connection entity allows to place ClipWidget
   * elements into the appropriate display region for this track, without exposing the actual
   * stage::timeline::BodyCanvasWidget to each and every Clip or Label widget.
   * 
   * @todo WIP-WIP as of 4/2019
   * @todo the number of pinned elements should be a member field, instead of sneaking it into the prelude element...
   */
  template<class ELM>
  class ViewHook
    {
      public:
      ELM&
      operator* ()  const
        {
          UNIMPLEMENTED ("mart-ptr like access to the attached Element");
        }
    };
  
  
  
  /**
   */
//inline void
//TrackProfile::performWith ()
//{
//}
  
  
  
}}// namespace model::timeline
#endif /*STAGE_MODEL_VIEW_HOOK_H*/
