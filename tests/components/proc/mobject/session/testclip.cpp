/*
  TestClip  -  bookkeeping (asset) view of a media clip.
 
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
 
* *****************************************************/


#include "proc/mobject/session/testclip.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/clip.hpp"

namespace mobject
  {
  namespace session
    {
    namespace test
      {
      typedef shared_ptr<mobject::session::Clip> PC;
      typedef shared_ptr<asset::Media> PM;
      typedef MediaAccessFacade MAF;
        

      /** @todo find a way to link to an existing clip object.
       *        Idea: use the clip's copy operation, i.e. feed it
       *        to mobject::session::clip copy ctor
       */
      TestClip::TestClip ()
      {
        // install Mock-Interface to cinelerra backend
        MAF::instance.injectSubclass (new MediaAccessMock);
        
        PM media = asset::Media::create("test-2", VIDEO); // query magic filename
        PC clip = media->createClip();
        //TODO how to link to *this ???
        
        MAF::instance.injectSubclass (0); // remove Mock-Interface
      }
      
      /** storage for the TestClip-Factory */
      TestClip::Factory TestClip::create;


    } // namespace test
  
  } // namespace session

} // namespace mobject
