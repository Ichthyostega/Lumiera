/*
  MakeClip(Test)  -  create a Clip from a Media Asset
 
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


#include "common/test/run.hpp"
#include "common/util.hpp"

#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/mobject/session/clip.hpp"

//#include "proc/asset/assetdiagnostics.hpp"

using util::contains;
using util::isnil;
using std::string;


namespace asset
  {
  namespace test
    {
    
    
    
    
    /***********************************************************************
     * @test creating a Clip MObject and an associated Clip Asset from 
     *       a given asset::Media.
     * @see  asset::Media#createClip
     */
    class MakeClip_test : public Test
      {
        typedef shared_ptr<asset::Media> PM;
        typedef shared_ptr<mobject::session::Clip> PC;
            
        virtual void run (Arg arg) 
          {
            
            PM mm = asset::Media::create("test-1", VIDEO);
            PC cc = mm->createClip();
            PM cm = cc->getMedia();
            
            ASSERT (cm);
            ASSERT (0 < cc->length);
            ASSERT (cm->ident.category.hasKind (VIDEO));
            ASSERT (cm->getFilename() == mm->getFilename());
            ASSERT (cm->howtoProc() == mm->howtoProc());
            ASSERT (cm->ident.org == mm->ident.org);
            ASSERT (dependencyCheck (mm,cm));

            TRACE (assetmem,   "leaving MakeClip_test::run()");
            TRACE (mobjectmem, "leaving MakeClip_test::run()");
          }
        
        bool dependencyCheck (PM media, PM clip)
        {
          TODO ("check asset dependencies, when this feature is implemented");
          return true; //TODO
          
          return (0 < clip->getParents().size())
              && (media == clip->getParents()[0])
              && (contains (media->getDependant(), clip));
              ;
        }
        
      };
    
    
    /** Register this test class... */
    LAUNCHER (MakeClip_test, "function asset");
    
    
    
  } // namespace test

} // namespace asset
