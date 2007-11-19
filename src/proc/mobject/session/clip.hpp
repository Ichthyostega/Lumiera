/*
  CLIP.hpp  -  a Media Clip
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef MOBJECT_SESSION_CLIP_H
#define MOBJECT_SESSION_CLIP_H

#include "proc/mobject/session/abstractmo.hpp"


namespace asset
  {
    class Media;
    class Clip;
  }

namespace mobject
  {
  namespace session
    {
    using asset::Media;
    typedef shared_ptr<Media> PMedia;


    /**
     * A user visible/editable Clip is a reference to a contiguous
     * sequence of media data loaded as Asset into the current Session.
     * As such, it is a virtual (non destructive) cut or edit of the 
     * source material and can be placed into the EDL to be rendered
     * into the ouput. The actual media type of a clip will be derived
     * at runtime by resolving this reference to the underlying Asset.
     * 
     * @todo define how to denote Time positions /lengths. This is tricky,
     * because it depends on the actual media type, and we want to encapsulate
     * all these details as much as possible. 
     */
    class Clip : public AbstractMO
      {
      protected:
        /** startpos in source */
        Time start_;

        const Media & mediaDef_;
        const asset::Clip & clipDef_;

        Clip (const asset::Clip&, const Media&);
        friend class MObjectFactory;
        
        
        virtual void setupLength();
        
      public:
        virtual bool isValid()  const;
        
        /** access the underlying media asset */
        PMedia getMedia ()  const;
        
      };
      
    typedef Placement<Clip> PClipMO;



  } // namespace mobject::session
  
  
  ///////////////////////////TODO use macro for specialsation...
  template<>
  class Placement<session::Clip> : public Placement<MObject>
    { 
    protected:
      Placement (session::Clip & m, void (*moKiller)(MObject*)) 
        : Placement<MObject>::Placement (m, moKiller) 
        { };
      friend class session::MObjectFactory;
        
    public:
      virtual session::Clip*
      operator-> ()  const 
        { 
          ENSURE (INSTANCEOF(session::Clip, &(*this)));
          return static_cast<session::Clip*> (shared_ptr<MObject>::operator-> ()); 
        }      
    };
    

} // namespace mobject
#endif
