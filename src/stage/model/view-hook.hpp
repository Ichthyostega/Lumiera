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

#include "lib/nocopy.hpp"
#include "lib/util.hpp"


namespace stage  {
namespace model {
  
  
  template<class ELM>
  class ViewHook;
  
  
  /**
   * Interface to represent _"some presentation layout entity",_
   * with the ability to _attach_ widgets managed elsewhere, to
   * relocate those widgets to another position, and to re-establish
   * a different sequence of the widgets (whatever this means).
   * @remark some typical examples for the kind of facility modelled here:
   *         - a _canvas widget,_ (e.g. `Gtk::Layout`), allowing to attach
   *           child widgets at specific positions, together with custom drawing.
   *         - a tree or grid control, allowing to populate some lines a given widget.
   */
  template<class ELM>
  class ViewHookable
    {
    public:
      virtual ~ViewHookable() { }    ///< this is an interface
      
      virtual ViewHook<ELM> hook (ELM& elm, int xPos, int yPos)  =0;
      virtual void move (ELM& elm, int xPos, int yPos)           =0;
      virtual void remove (ELM& elm)   noexcept                  =0;
      virtual void rehook (ViewHook<ELM>& hook)  noexcept        =0;
      
      template<class IT>
      void reOrder (IT newOrder);
    };
  
  
  
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
   * @remark since ViewHook represents one distinct attachment to some view or canvas,
   *         is has a clear-cut identity and thus may be moved, but not copied.
   * @todo WIP-WIP as of 11/2019
   */
  template<class ELM>
  class ViewHook
    : util::MoveOnly
    {
      using View = ViewHookable<ELM>;
      
      ELM* widget_;
      View* view_;
      
    public:
      ViewHook (ELM& elm, View& view)
        : widget_{&elm}
        , view_{&view}
        { }
      
      ViewHook (ViewHook&& existing)  noexcept
        : widget_{nullptr}
        , view_{nullptr}
        {
          swap (*this, existing);
        }
      
     ~ViewHook()  noexcept
        {
          if (widget_ and view_)
            view_->remove (*widget_);
        }
      
      ELM&
      operator* ()  const
        {
          return *widget_;
        }
      
      void
      moveTo (int xPos, int yPos)
        {
          view_->move (*widget_, xPos,yPos);
        }
      
      
      friend void
      swap (ViewHook& a, ViewHook& b)  noexcept
      {
        std::swap (a.widget_, b.widget_);
        std::swap (a.view_, b.view_);
      }
    };
  
  
  
  /**
   * re-attach elements in a given, new order.
   * @param newOrder a Lumiera Forward Iterator to yield a reference to
   *        all attached elements, and in the new order to be established.
   * @remark this operation becomes relevant, when "attaching an element" also
   *        constitutes some kind of arrangement in the visual presentation, like
   *        e.g. a stacking order, or by populating some table cells in sequence.
   *        The expected semantics is for this operation to detach each given element,
   *        and then immediately re-attach it _at the "beginning"_ (whatever this means).
   *        The element as such, and all associated presentation entities are not destroyed,
   *        but continue to exist with the same identity (and possibly all signal wirings).
   *        Just they now appear as if attached with the new ordering.
   */
  template<class ELM>
  template<class IT>
  void
  ViewHookable<ELM>::reOrder (IT newOrder)
  {
    for (ViewHook<ELM>& existingHook : newOrder)
      this->rehook (existingHook);
  }
  
  
  
}}// namespace stage::model
#endif /*STAGE_MODEL_VIEW_HOOK_H*/
