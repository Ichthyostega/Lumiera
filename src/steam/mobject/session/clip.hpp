/*
  CLIP.hpp  -  a Media Clip

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file session/clip.hpp
 ** MObject in the Session to represent a clip on the timeline
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#ifndef STEAM_MOBJECT_SESSION_CLIP_H
#define STEAM_MOBJECT_SESSION_CLIP_H

#include "steam/mobject/session/abstractmo.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/p.hpp"


namespace steam {
namespace asset {
  class Media;
  class Clip;
}

namespace mobject {
namespace session {
  
  using asset::Media;
  using lib::time::TimeVar;
  
  class Clip;
  
  using PMedia = lib::P<Media>;
  using PClipAsset = lib::P<asset::Clip>;
  
  using PClip = Placement<Clip>;
  
  
  /**
   * A user visible/editable Clip is a reference to a contiguous
   * sequence of media data loaded as Asset into the current Session.
   * As such, it is a virtual (non destructive) cut or edit of the 
   * source material and can be placed into the Session to be rendered
   * into the output. The actual media type of a clip will be derived
   * at runtime by resolving this reference to the underlying Asset.
   * 
   * @todo define how to denote Time positions /lengths. This is tricky,
   * because it depends on the actual media type, and we want to encapsulate
   * all these details as much as possible.
   */
  class Clip
    : public AbstractMO
    {
      string
      initShortID()  const
        {
          return buildShortID("Clip");
        }
      
      void setupLength();
      
      
      
    protected:
      /** start position in source */
      TimeVar start_;
      
      /** @todo using a mere ref here is against the scheme and only
          done as temporal solution, until we work out how to handle
          multichannel clips. It should be a smart pointer of some kind
          and the unlink() function of the asset should take it into
          account when breaking circular references.
       */
      
      const Media & mediaDef_;
      const asset::Clip & clipDef_;
      
      Clip (const asset::Clip&, const Media&);
      friend class MObjectFactory;
      
      
    public:
      bool isValid()  const;
      
      /** access the underlying media asset */
      PMedia getMedia ()  const;
      
      /** locate the corresponding asset
       *  representing this clip or the whole
       *  compound in case of a multichannel clip
       */ 
      PClipAsset findClipAsset ()  const;
      
      DEFINE_PROCESSABLE_BY (builder::BuilderTool);
      
    };
  
  
  
  
}}} // namespace steam::mobject::session
#endif
