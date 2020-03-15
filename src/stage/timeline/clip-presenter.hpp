/*
  CLIP-PRESENTER.hpp  -  presentation control element for a clip within the timeline

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

*/


/** @file clip-presenter.hpp
 ** Presentation control element to model and manage a clip within the timeline UI.
 ** In the Lumiera timeline UI, we are mixing two different scope of concerns: For one,
 ** we have the globally tangible scope of actual session elements an operations performed
 ** on those. And then there are more local considerations regarding the "mechanics" of the
 ** UI elements, their state and immediate feedback to user interactions. The _Presenter_ --
 ** as known from the [MVP pattern](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93presenter) --
 ** serves as link between both levels. For the global angle of view, it is a model::Tangible and thus
 ** plays the role of the _View_, while the _Model_ and _Controler_ roles are mediated through the
 ** stage::UiBus, exchanging command, state and mutation messages. On the other hand, for the local
 ** angle of view, the _Presenter_ is a structural model element, kind of a _view model_, and corresponds
 ** to the respective element within the session. In addition, it manages actively the collaborative
 ** part of layout building, delegating to a mostly passive GTK widget for the actual display.
 ** This way it becomes possible to manage the actual UI resources on a global level, avoiding to
 ** represent potentially several thousand individual elements as GTK entities, while at any time
 ** only several can be visible and active as far as user interaction is concerned.
 ** 
 ** Especially the ClipPresenter plays a crucial role when it comes to deciding which *clip appearance style*
 ** to use for the actual display. Depending on the zoom level, the ClipPresenter may even decide to hide
 ** the individual clip and join it into a combined placeholder for degraded display of a hole strike of
 ** clips, just to indicate some content is actually present in this part of the timeline.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#ifndef STAGE_TIMELINE_CLIP_PRESENTER_H
#define STAGE_TIMELINE_CLIP_PRESENTER_H

#include "stage/gtk-base.hpp"
#include "stage/model/controller.hpp"
#include "stage/timeline/clip-widget.hpp"

//#include "lib/util.hpp"

//#include <memory>
#include <optional>
#include <vector>



namespace stage  {
namespace timeline {
  
  using std::vector;
  using std::optional;
  using std::unique_ptr;
  
  class MarkerWidget;
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class ClipPresenter
    : public model::Controller
    {
      vector<unique_ptr<ClipPresenter>> channels_;
      vector<unique_ptr<ClipPresenter>> effects_;
      vector<unique_ptr<MarkerWidget>>  markers_;
      
      unique_ptr<ClipDelegate> widget_;
      
      /** default level of detail presentation desired for each clip.
       * @note the actual appearance style is chosen based on this setting
       *       yet limited by the additional information necessary to establish
       *       a given level; e.g. name or content renderer must be available
       *       to allow for a detailed rendering of the clip in the timeline. */
      static const ClipDelegate::Appearance defaultAppearance = ClipDelegate::COMPACT;
      
    public:
      ClipPresenter (ID, ctrl::BusTerm&, WidgetViewHook&, optional<int> offsetX);
      
     ~ClipPresenter();
      
      
      /** set up a binding to respond to mutation messages via UiBus */
      virtual void buildMutator (lib::diff::TreeMutator::Handle)  override;
      
      
      /** find out the number of pixels necessary to render this clip properly,
       *  assuming its current presentation mode (abbreviated, full, expanded).
       */
      int determineRequiredVerticalExtension()  const;
      
    private:/* ===== Internals ===== */
      
      /** reevaluate desired presentation mode and available data,
       * possibly leading to a changed appearance style of the clip.
       * @remark a typical example would be, when a clip's temporal position,
       *         previously unspecified, now becomes defined through a diff message.
       *         With this data, it becomes feasible _actually to show the clip_ in
       *         the timeline. Thus the [Appearance style](\ref ClipDelegate::Appearance)
       *         of the presentation widget (delegate) can be switched up from `PENDING`
       *         to `ABRIDGED`.
       */
      void resetAppearanceStyle();
      
      WidgetViewHook& getClipContentCanvas();
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_CLIP_PRESENTER_H*/
