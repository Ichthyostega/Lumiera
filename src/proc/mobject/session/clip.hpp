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



namespace mobject
  {
  namespace session
    {


    /**
     * A user visible/editable Clip is a reference to a contiguous
     * sequence of media data loaded as Asset into the current Session.
     * As such, it is a virtual (non destructive) cut or edit of the 
     * source material and can be placed into the EDL to be rendered
     * into the ouput. The actual media type of a clip will be derived
     * at runtime by resolving this reference to the underlying Asset.
     * 
     * TODO: define how to denote Time positions /lengths. This is tricky,
     * because it depends on the actual media type, and we wand to encapsulate
     * all these details as much as possible. 
     */
    class Clip : public AbstractMO
      {
      protected:
        /** startpos in source */
        Time start;

        //TODO: where to put the duration ???

      };



  } // namespace mobject::session

} // namespace mobject
#endif
