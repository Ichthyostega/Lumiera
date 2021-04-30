/*
  CANVAS-HOOK.hpp  -  abstracted attachment to a canvas with free positioning

  Copyright (C)         Lumiera.org
    2020,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file canvas-hook.hpp
 ** Specialised (abstracted) presentation context with positioning by coordinates.
 ** This expands the idea behind the ViewHook abstraction, and works in a similar way,
 ** in close collaboration with the corresponding CanvasHooked entity (abstraction).
 ** Elements relying on those abstractions maintain an attachment to "their view",
 ** while remaining agnostic about the view's implementation details. However,
 ** the key point with this extended variant of the abstraction is that elements
 ** can be placed onto a coordinate system or canvas, and they can be moved to a
 ** different position.
 ** 
 ** A CanvasHooked element is basically a decorator directly attached to the
 ** element, adding automatic detachment on destruction, similar to a smart-ptr.
 ** So the "hooked" widget will live within the common allocation, together with
 ** its attachment; the whole arrangement must be set up at construction time
 ** - the combined `CanvasHooked<W>` must be non-copyable, since it can be expected
 **   for the canvas to store some pointer to the attached widget.
 ** - moreover, the canvas/presentation need to be available and activated when
 **   constructing the widget(s) due to the interwoven lifecycle.
 ** - and, most notably, the presentation/canvas (the CanvasHook) must be arranged
 **   such as to outlive the attached widgets, since they call back on destruction.
 ** In the typical usage situation these points can be ensured naturally by housing
 ** the widgets in some detail data structure owned by the top level presentation frame.
 ** 
 ** @see CanvasHook_test
 ** 
 */


#ifndef STAGE_MODEL_CANVAS_HOOK_H
#define STAGE_MODEL_CANVAS_HOOK_H

#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <utility>


namespace stage  {
namespace model {
  
  using lib::time::Time;
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  
  
  
  /**
   * Mix-in interface to allow for concrete CanvasHooked widgets
   * to adapt themselves to the metric currently employed on the canvas.
   * In some cases, most notably when a timeline canvas is calibrated to
   * represent temporal extension precisely, the widgets (clips, effects)
   * within such a display need to adjust themselves. A relevant special
   * case is when such a widget _is dragged_ -- receiving mouse move events
   * in screen coordinates -- which need to be translated into a resulting
   * temporal offset or change as a result of this _interaction gesture._
   */
  class DisplayMetric
    {
    public:
      virtual ~DisplayMetric() { }    ///< this is an interface
      
      /** the overall time Duration covered by this timeline canvas */
      virtual TimeSpan coveredTime()  const                =0;
      
      /** extension point for time axis zoom management. */
      virtual int translateTimeToPixels (TimeValue)  const =0;
    };
  
  
  
  /**
   * Interface to represent _"some presentation layout entity",_
   * with the ability to _place_ widgets (managed elsewhere) onto it,
   * to relocate those widgets to another position.
   * @remark the canonical example is a _canvas widget,_ (e.g. `Gtk::Layout`),
   *    allowing to attach child widgets at specific positions, together with
   *    custom drawing.
   * @warning please ensure the CanvasHook outlives any attached CanvasHooked.
   * @see ViewHook embodies the same scheme for widgets just "added" into
   *    the presentation without the notion of explicit coordinates.
   */
  template<class WID>
  class CanvasHook
    {
    public:
      virtual ~CanvasHook() { }    ///< this is an interface
      
      virtual void hook   (WID& widget, int xPos, int yPos)  =0;
      virtual void move   (WID& widget, int xPos, int yPos)  =0;
      virtual void remove (WID& widget)                      =0;

      /** access the component to handle layout metric */
      virtual DisplayMetric& getMetric()  const              =0;
      
      /** Anchor point to build chains of related View Hooks */
      virtual CanvasHook<WID>&
      getAnchorHook()  noexcept
        {
          return *this;
        }
      
      struct Pos
        {
          CanvasHook* view;
          int x,y;
        };
      
      Pos
      hookedAt (int x, int y)
        {
          return Pos{this, x,y};
        }
      
      /** build the "construction hook" for a \ref ViewHooked element,
       *  which is to be attached to some timeline canvas view.
       * @param start anchor point / nominal start point of the element
       * @param downshift (optional) vertical shift down from the baseline
       * @return opaque registration argument for the CanvasHooked ctor */
      Pos
      hookedAt (Time start, int downshift=0)
        {
          return hookedAt (getMetric().translateTimeToPixels (start), downshift);
        }
    };
  
  
  
  /**
   * A widget attached onto a display canvas or similar central presentation context.
   * This decorator is a variation of the ViewHooked decorator, and likewise embodies
   * the widget to be attached; moreover, the attachment is immediately performed at
   * construction time and managed automatically thereafter. When the `CanvasHooked`
   * element goes out of scope, it is automatically detached from presentation.
   * With the help of the CanvasHooked API, a widget (or similar entity) may control the coordinates
   * of its placement onto some kind of _canvas_ (-> `Gtk::Layout`), while remaining agnostic
   * regarding any further implementation details of the canvas and its placement thereon.
   * 
   * The canonical example of a `CanvasHooked` element is the stage::timeline::ClipWidget, as created
   * and managed by the TrackPresenter within the timeline UI. This connection entity allows to place
   * ClipWidget elements into the appropriate display region for this track, without exposing the actual
   * stage::timeline::BodyCanvasWidget to each and every Clip or Label widget.
   * 
   * @tparam WID type of the embedded widget, which is to be hooked-up into the view/canvas.
   * @remark since CanvasHooked represents one distinct attachment to some view or canvas,
   *         is has a clear-cut identity and and will be identified by its allocation address.
   * @warning since ViewHooked entities call back into the ViewHook on destruction,
   *         the latter still needs to be alive at that point. Which basically means
   *         you must ensure the ViewHooked "Widgets" are destroyed prior to the "Canvas".
   */
  template<class WID, class BASE =WID>
  class CanvasHooked
    : public WID
    , util::NonCopyable
    {
      using Canvas = CanvasHook<BASE>;
      Canvas* view_;
      
    protected:
      Canvas& getCanvas()  const { return *view_; }
      
    public:
      template<typename...ARGS>
      CanvasHooked (typename Canvas::Pos attachmentPos, ARGS&& ...args)
        : WID{std::forward<ARGS>(args)...}
        , view_{attachmentPos.view}
        {
          getCanvas().hook (*this, attachmentPos.x, attachmentPos.y);
        }
      
     ~CanvasHooked()  noexcept
        {
          try {
              if (view_)
                view_->remove (*this);
            }
          ERROR_LOG_AND_IGNORE (progress, "Detaching of CanvasHooked widgets from the presentation")
        }
      
      void
      moveTo (int xPos, int yPos)
        {
          getCanvas().move (*this, xPos,yPos);
        }
    };
  
  
  
}}// namespace stage::model
#endif /*STAGE_MODEL_CANVAS_HOOK_H*/
