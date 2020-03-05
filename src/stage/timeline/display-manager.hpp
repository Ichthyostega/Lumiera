/*
  DISPLAY-MANAGER.hpp  -  abstraction to translate model structure and extension into screen layout

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file display-manager.hpp
 ** Abstraction: service for the widgets to translate themselves into screen layout.
 ** 
 ** # Architecture
 ** 
 ** We strive to break up the whole process of controlling the layout into several
 ** local concerns, each of which can be made self contained. The backbone is formed by
 ** a recursive collaboration between two abstractions (interfaces)
 ** - the building blocks of the timeline expose the interface timeline::Element
 ** - the global timeline widget implements a timeline::LayoutManager interface
 ** 
 ** ## Display evaluation pass
 ** 
 ** Whenever the layout of timeline contents has to be (re)established, we trigger a recursive
 ** evaluation pass, which in fact is a tree walk. The layout manager maintains a DisplayEvaluation
 ** record, which is passed to the involved layout elements within the timeline. Each element in turn
 ** has the liability to walk its children and recursively initiate a nested evaluation. During that
 ** pass, we are able to transport and aggregate information necessary to give each element the
 ** necessary amount of screen real estate. 
 ** 
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** @todo WIP-WIP-WIP - drafting the DisplayEvaluation as of 3/2020
 ** 
 */


#ifndef STAGE_TIMELINE_DISPLAY_MANAGER_H
#define STAGE_TIMELINE_DISPLAY_MANAGER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "stage/model/view-hook.hpp"
#include "lib/util.hpp"

#include <sigc++/signal.h>



namespace stage  {
namespace timeline {
  
  using util::max;
  
  using model::ViewHooked;
  class TrackHeadWidget;
  class TrackBody;
  
  
  /**
   * Special ViewHook decorator to apply a (dynamic) offset
   * when attaching or moving Widgets on the shared canvas.
   */
  template<class WID>
  class ViewRefHook
    : public model::ViewHook<WID>
    {
      model::ViewHook<WID>& refHook_;
      
      
      /* ==== Interface: ViewHook ===== */
      
      void
      hook (WID& widget, int xPos=0, int yPos=0) override
        {
          refHook_.hook (widget, hookAdjX (xPos), hookAdjY (yPos));
        }
      
      void
      move (WID& widget, int xPos, int yPos)  override
        {
          refHook_.move (widget, hookAdjX (xPos), hookAdjY (yPos));
        }

      void
      remove (WID& widget)  override
        {
          refHook_.remove (widget);
        }
      
      void
      rehook (WID& hookedWidget)  noexcept override
        {
          refHook_.rehook (hookedWidget);
        }
      
      /** allow to build a derived ViewRefHook with different offset */
      model::ViewHook<WID>&
      getAnchorHook()  noexcept override
        {
          return this->refHook_;
        }
      
    protected: /* === extended Interface for relative view hook === */
      virtual int hookAdjX (int xPos)  =0;
      virtual int hookAdjY (int yPos)  =0;
      
    public:
      ViewRefHook(model::ViewHook<WID>& baseHook)
        : refHook_{baseHook.getAnchorHook()}
        { }
    };
  
  
  /**
   * Interface: a compound of anchoring facilities.
   * With the help of view-hooking, some detail presentation component
   * or widget can attach itself into the overarching view context or canvas
   * of the Timeline, while remaining agnostic about actual structure or implementation
   * of this »display umbrella«. The local presentation component itself is then model::ViewHooked,
   * thereby managing its attachment to the global context automatically. As it turns out, within
   * the timeline display, we typically need a specific combination of such model::ViewHook, and
   * we need them recursively: the actual timeline::DisplayFrame, while attaching below such
   * DisplayViewHooks, is in turn itself again such a sub-anchor, allowing to attach
   * child display frames recursively, which is required to display sub-tracks.
   */
  class DisplayViewHooks
    {
    public:
      virtual ~DisplayViewHooks() { }    ///< this is an interface
      
      virtual model::ViewHook<TrackHeadWidget>& getHeadHook()  =0;
      virtual model::ViewHook<TrackBody>&       getBodyHook()  =0;
      virtual model::ViewHook<Gtk::Widget>&     getClipHook()  =0;
    };
  
  
  
  /** @todo quick-n-dirty hack. Should consider the Range TS (C++20) */
  struct PixSpan
    {
      int b = 0;
      int e = 0;
      
      PixSpan() { }
      PixSpan(int begin, int end)
        : b{begin}, e{max (begin,end)}
        { }
      
      bool
      empty()  const
        {
          return e <= b;
        }
      
      int
      delta()  const
        {
          return e - b;
        }
    };
  
  /**
   * Interface used by the widgets to attach and translate themselves into screen layout.
   * @todo WIP-WIP as of 12/2019
   */
  class DisplayManager
    : util::NonCopyable
    , public DisplayViewHooks
    {
      
      
    public:
      virtual ~DisplayManager();    ///< this is an interface
      
      /** the overall horizontal pixel span to cover by this timeline */
      virtual PixSpan getPixSpan()         =0;
      
      /** cause a re-allocation of the complete layout */
      virtual void triggerDisplayEvaluation() =0;
      
      using SignalStructureChange = sigc::signal<void>;
      
      /**
       * signal to be invoked whenever the virtual structure of the
       * corresponding timeline changes, thus necessitating a new
       * arrangement of the timeline layout.
       */
      SignalStructureChange signalStructureChange_;
      
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_DISPLAY_MANAGER_H*/
