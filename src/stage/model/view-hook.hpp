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
 ** the actual implementation of the central facility. The attachment of a widget is thus
 ** modelled by a smart-handle stage::timeline::ViewHooked, which -- on destruction --
 ** automatically detaches the widget from the presentation.
 ** 
 ** As it turns out in practice, we get two flavours or _view attachment:_
 ** - Widgets just somehow placed into a grid or widget layout
 ** - Widgets attached to a canvas with positioning by coordinates
 ** Moreover, such a "hooked" widget will never exist apart from its attachment.
 ** Consequently, we locate the widget within the smart-handle itself, thus tightly
 ** linking together the lifecycle of the widget and the presentation attachment.
 ** However, this combined memory layout incurs some liabilities:
 ** - the combined `ViewHooked<W>` must be non-copyable, since it can be expected
 **   for the canvas to store some pointer to the attached widget.
 ** - moreover, the canvas/presentation need to be available and activated when
 **   constructing the widget(s) due to the interwoven lifecycle.
 ** - and, most notably, the presentation/canvas (the ViewHook) must be arranged to
 **   outlive the attached widgets, since they call back on destruction.
 ** In the typical usage situation these points can be ensured naturally by housing
 ** the widgets in some detail data structure owned by the top level presentation frame.
 ** 
 ** @see ViewHook_test
 ** 
 */


#ifndef STAGE_MODEL_VIEW_HOOK_H
#define STAGE_MODEL_VIEW_HOOK_H

#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <utility>


namespace stage  {
namespace model {
  
  
  /**
   * Interface to represent _"some presentation layout entity",_
   * with the ability to _attach_ widgets (managed elsewhere), and
   * to re-establish a different sequence of the widgets (whatever this means).
   * @remark some typical examples for the kind of collaboration modelled here:
   *         - a tree or grid control, allowing to populate some row with a given widget.
   *         - a _canvas widget,_ (e.g. `Gtk::Layout`), allowing to attach
   *           child widgets at specific positions, together with custom drawing.
   * @warning please ensure the ViewHook outlives any attached ViewHooked.
   * @see CanvasHook extended interface to support positioning by coordinates
   */
  template<class WID>
  class ViewHook
    {
    public:
      virtual ~ViewHook() { }    ///< this is an interface
      
      virtual void hook   (WID& widget)           =0;
      virtual void remove (WID& widget)           =0;
      virtual void rehook (WID& widget) noexcept  =0;
      
      template<class IT>
      void reOrder (IT newOrder);
    };
  
  
  
  /**
   * A widget attached onto a display canvas or similar central presentation context.
   * This decorator inherits from the widget to be attached, i.e. the widget itself becomes
   * embedded; moreover, the attachment is immediately performed at construction time and
   * managed automatically thereafter. When the `ViewHooked` element goes out of scope, it is
   * automatically detached from presentation. With the help of ViewHooked, a widget (or similar
   * entity) may control some aspects of its presentation placement, typically the order or
   * arrangement within a grid or layout, while remaining agnostic regarding the
   * implementation details of the canvas and its placement thereon.
   * 
   * The prominent usage example of ViewHooked` elements is in the stage::timeline::DisplayFrame,
   * maintained  by the TrackPresenter within the timeline UI. This connection entity allows to
   * attach \ref TrackHeaderWidget elements into the appropriate part of the patchbay, and to place
   * \ref ClipWidget elements into the appropriate display region for this track, without exposing
   * the actual stage::timeline::BodyCanvasWidget to each and every Clip or Label widget.
   * 
   * @tparam WID type of the embedded widget, which is to be hooked-up into the view/canvas.
   * @remark since ViewHooked represents one distinct attachment to some view or canvas,
   *         is has a clear-cut identity, linked to an allocation and must not be moved.
   * @warning since ViewHooked entities call back into the ViewHook on destruction,
   *         the latter still needs to be alive at that point. Which basically means
   *         you must ensure the ViewHooked "Widgets" are destroyed prior to the "Canvas".
   */
  template<class WID, class BASE =WID>
  class ViewHooked
    : public WID
    , util::NonCopyable
    {
      using View = ViewHook<BASE>;
      View* view_;
      
    protected:
      View&
      getView() const { return *view_; }
      
    public:
      template<typename...ARGS>
      ViewHooked (View& view, ARGS&& ...args)
        : WID{std::forward<ARGS>(args)...}
        , view_{&view}
        {
          view_->hook (*this);
        }
      
     ~ViewHooked()  noexcept
        {
          try {
              if (view_)
                view_->remove (*this);
            }
          ERROR_LOG_AND_IGNORE (progress, "Detaching of ViewHooked widgets from the presentation")
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
   *        and then immediately re-attach it _at the "front side"_ (whatever this means).
   *        The element as such, and all associated presentation entities are not destroyed,
   *        but continue to exist with the same identity (and possibly all signal wirings).
   *        Just they now appear as if attached with the new ordering.
   */
  template<class WID>
  template<class IT>
  void
  ViewHook<WID>::reOrder (IT newOrder)
  {
    for (WID& existingHook : newOrder)
      this->rehook (existingHook);
  }
  
  
  
}}// namespace stage::model
#endif /*STAGE_MODEL_VIEW_HOOK_H*/
