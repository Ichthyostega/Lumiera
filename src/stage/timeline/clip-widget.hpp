/*
  CLIP-WIDGET.hpp  -  display of a clip in timeline or media bin view

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file clip-widget.hpp
 ** This widget provides the concrete rendering of a clip-like entity.
 ** Such can appear in various contexts within the UI, be it a media bin
 ** in the asset management section, or rather the timeline display proper,
 ** which is at the heart of the Lumiera UI. Moreover, such a "clip" might
 ** represent a media clip as a whole, or just recursively some part of it.
 ** It might be a channel, or even an effect attached to some clip. Combined
 ** with the various temporal zoom levels, we thus need the ability to adapt
 ** this display to a wide variety of *clip appearance styles*. And another
 ** consequence is that the ClipWidget itself needs to be mostly passive,
 ** supervised by a dedicated ClipPresenter.
 ** 
 ** # Appearance styles
 ** 
 ** According to the requirements of the given context, the ClipWidget is
 ** able to conform to several styles of appearance. All of these can be
 ** implemented from a single common base structure, and thus it is possible
 ** to switch those styles dynamically, so to expand or collapse the visible
 ** entity in place
 ** 
 ** \par abridged form
 ** To start with, a clip can be rendered in *abridged form*, which means that the
 ** content is stylised and the temporal extension does not matter. In this form,
 ** the clip is reduced to an icon, an expand widget and a ID label. This is the
 ** standard representation encountered within the _media bins._ The intent of this
 ** representation is to save on screen estate, especially to minimise vertical extension.
 ** But as a derivative of this layout style, a clip may be shown in abridged form, but
 ** in fact _with proper representation of the temporal extension;_ to this end, the
 ** enclosing box is extended horizontally as needed, while the compound of icon,
 ** control and label is aligned such as to remain in sight.
 ** 
 ** \par compact form
 ** The next step in a series of progressively more detailed clip representations is the
 ** *compact form*, which still focuses on handling the clip as an unit, while at least
 ** indicating some of the inherent structuring. Essentially, the clip here is represented
 ** as a _strip of rendered preview content,_ decorated with some overlays. One of these
 ** overlays is the _ID pane,_ which resembles the arrangement known from the abridged form:
 ** The icon here is always the _placement icon,_ followed by the expand widget and the ID
 ** label. Again, this pane is aligned such as to remain in sight. Then, there is a pair
 ** of overlays, termed the _boundary panes,_ which indicate the begin and the end of the
 ** clip respectively. Graphically, these overlays should be rendered in a more subtle way,
 ** just enough to be recognisable. The boundary panes are the attachment areas for
 ** _trimming gestures,_ as opposed to moving and dragging the whole clip or shuffle editing
 ** of the content. Moreover, these boundary panes compensate for the alignment of the ID pane,
 ** which mostly keeps the latter in sight. As this might counterfeit the visual perception
 ** of scrolling, the boundary panes serve to give a clear visual clue when reaching the
 ** boundary of an extended clip. Optionally, another overlay is rendered at the upper side
 ** of the clip's area, to indicate attached effect(s). It is quite possible for these effect
 ** decorations not to cover the whole temporal span of the clip.
 ** 
 ** \par expanded form
 ** A yet more detailed display of the clip's internals is exposed in the *expanded form*.
 ** Here, the clip is displayed as an window pane holding nested clip displays, which in turn
 ** might again be abridged, compact or _(TODO not sure 11/16)_ even expanded. This enclosing
 ** clip window pane should be rendered semi transparent, just to indicate the enclosing whole.
 ** The individual clip displays embedded therein serve to represent individual media parts or
 ** channels, or individual attached effects. Due to the recursive nature of Lumiera's
 ** High-Level-Model, each of these parts exposes essentially the same controls, allowing to
 ** control the respective aspects of the part in question
 ** 
 ** \par degraded form
 ** Finally, there can be a situation where it is just not possible to render any of the
 ** aforementioned display styles properly, due to size constraints. Especially, this happens
 ** when zooming out such as to show a whole sequence or even timeline in overview. We need to
 ** come up with a scheme of ''graceful display degradation'' to deal with this situation --
 ** just naively attempting to render any form might easily send our UI thread into a minute long
 ** blocking render state, for no good reason. Instead, in such cases display should fall back to
 ** - showing just a placeholder rectangle, when the clip (or any other media element) will cover
 **   a temporal span relating to at least 1 pixel width (configurable trigger condition)
 ** - even further collapsing several entities into a strike of elements, to indicate at least
 **   that some content is present in this part of the timeline.
 ** 
 ** ## Structure of the clip representation
 ** Obviously, managing all these wildly different appearance styles incurs a lot of complexity,
 ** which needs to be decomposed to keep it manageable. Thus, we introduce several responsibilities
 ** - the ClipPresenter is what formally corresponds to the session::Clip, i.e. in a birds
 **   eyes view, it "is" the clip. However, in fact the ClipPresenter only manages the
 **   desired properties and delegates the actual realisation to "some widget"
 ** - and this "widget" is in fact a "PImpl", the ClipDelegate, which foremost exposes an interface
 **   to adapt and control the appearance style, while the actual clip widget remains a private detail
 **   and is only accessible through the ClipDelegate interface
 ** - behind the scenes, [within the implementation](\ref clip-widget.cpp), several implementation
 **   subclasses are available, to be installed and exchanged to accommodate the desired appearance
 **   style. These are managed semi-automatically and hooked into the appropriate display framework
 **   as required. And this attachment is itself again abstracted as CanvasHook -- which means the
 **   actual clip widget implements the interaction mechanics, and can draw itself on demand, while
 **   remaining agnostic about details regarding zoom management, scrolling and visibility of
 **   timeline contents, or the actual allocation of display extension, which happens within the
 **   stage::timeline::DisplayEvaluation pass.
 ** 
 ** @todo WIP-WIP as of 1/2021
 ** 
 */


#ifndef STAGE_TIMELINE_CLIP_WIDGET_H
#define STAGE_TIMELINE_CLIP_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/model/canvas-hook.hpp"
#include "lib/time/timevalue.hpp"

//#include "lib/util.hpp"

#include <optional>
#include <memory>
#include <string>
//#include <vector>



namespace stage  {
namespace timeline {
  
  using std::string;
  using std::nullopt;
  using std::optional;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::Duration;
  using lib::time::TimeSpan;
  
  using WidgetHook = model::CanvasHook<Gtk::Widget>;
  
  class ClipDelegate;
  using PDelegate = std::unique_ptr<ClipDelegate>;
  
  
  /**
   * @todo WIP-WIP as of 2/2021
   */
  class ClipDelegate
    {
      TimeSpan timing_;
      
    public:
      virtual ~ClipDelegate();   ///< this is an interface
      ClipDelegate();
      
      
      /** desired appearance style for the clip */
      enum Appearance {PENDING, SYMBOLIC, DEGRADED, ABRIDGED, COMPACT, EXPANDED};
      
      
      /** vertical offset below the track start */
      static const int defaultOffsetY;
      
      /** placeholder name -- typically overridden from the model */
      static const string defaultName;
      
      /** presentation mode and style currently employed */
      virtual Appearance currentAppearance()  const  =0;
      
      /** alter appearance style, to the degree possible for this delegate.
       * @return the new #Appearance style acquired */
      virtual Appearance changeAppearance (Appearance desired)  =0;
      
      /** human readable rendering of the clip's name or identity */
      virtual cuString getClipName()    const =0;
      virtual void setClipName (cuString&)    =0;
      
      virtual TimeVar& accessStartTime()      =0;
      virtual TimeVar& accessDuration()       =0;

      virtual uint getVerticalOffset()  const =0;
      
      virtual WidgetHook& getCanvas()   const =0;
      virtual void updatePosition()           =0;
      
      virtual uint calcRequiredHeight() const =0;
      

      /** (re)establish current canvas attachment coordinates,
       *  thereby possibly switching to a new canvas implementation
       * @param newView (optional) new canvas; use existing if not given
       * @return record defining the canvas and the coordinates thereon
       */
      WidgetHook::Pos establishHookPoint (WidgetHook* newView);
      
      
      /** request to change the clip delegate's appearance style, if possible.
       * @param existing entity to hold and maintain this specific appearance state.
       * @param desired (optionally) the intended style or mode to acquire
       * @param newView (optionally) a different view to hook up the delegate.
       * @param timing position and duration of the clip, required when `existing` is empty
       * @return the actual mode the presentation was switched to
       * @remark switching the appearance style is a state transition; sometimes
       *         this change also implies switching the actual implementation of
       *         the delegate. And thus there can be prerequisites for achieving
       *         a specific appearance and presentation mode. Most notably, to
       *         reach any style beyond `SYMBOLIC`, we need to know the temporal
       *         extension of the clip. If such requirements can not be fulfilled,
       *         presentation stays or drops to the most elaborate state possible
       *         with the current configuration.
       * @note This function can build a new delegate from scratch, when the `existing`
       *         smart-ptr is empty. However, in this case it is mandatory to pass `newView`
       * @note Default and fallback appearance style is `PENDING`, which turns the
       *         delegate into a mere data record without visual representation.
       * @note whenever a WidgetHook (instance) different than the existing one
       *         is given, the existing widget / delegate will be destroyed and
       *         replaced by a suitable copy hooked up into the new display.
       *         The base ctor #ClipData(WidgetHook&) ensures there is
       *         always a display_ (WidgetHook) to refer to.
       */
      static Appearance selectAppearance (PDelegate& existing,
                                          Appearance desired  =PENDING,
                                          WidgetHook* newView =nullptr,
                                          optional<TimeSpan> const& timing =nullopt);
      
      /**
       * Wrapper to safely expose the actual clip implementation widget.
       * @throws error::State if the current presentation / appearance style
       *         does not actually use a dedicated clip widget.
       * @remark the assumption is for the caller to arrange the calling context
       *         such that existence of an actual widget can safely be assumed.
       */
      static Gtk::Widget& expect_and_expose_Widget (PDelegate& manager);
      
      
    private:/* ===== Internals ===== */
      
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_CLIP_WIDGET_H*/
