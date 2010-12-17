/*
  MakeClip(Test)  -  create a Clip from a Media Asset

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

* *****************************************************/


#include "include/logging.h"
#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/asset/asset-diagnostics.hpp"

using util::contains;
using util::isnil;
using std::string;

using std::tr1::static_pointer_cast; //TODO only temporarily;

namespace asset {
  namespace test {
    
          //using mobject::NOBUG_FLAG(mobject_mem);
    
    
    
    /***********************************************************************
     * @test creating a Clip MObject and an associated Clip Asset from 
     *       a given asset::Media.
     * @see  asset::Media#createClip
     */
    class MakeClip_test : public Test
      {
        typedef P<asset::Media> PM;
        typedef asset::Media::PClipMO PC;
            
        virtual void run (Arg) 
          {
            
            PM mm = asset::Media::create("test-1", VIDEO);
            PC cc = mm->createClip();
            PM cm = cc->getMedia();
            
            CHECK (cm);
            CHECK (Time(0) < cc->getLength());
            CHECK (cm->ident.category.hasKind (VIDEO));
            CHECK (cm->getFilename() == mm->getFilename());
TODO ("implement Processing Pattern!!!");   
//            CHECK (cm->howtoProc() == mm->howtoProc());
            CHECK (cm->ident.org == mm->ident.org);
            CHECK (dependencyCheck (cm,mm));

            TRACE (asset_mem,   "leaving MakeClip_test::run()");
            TRACE (mobject_mem, "leaving MakeClip_test::run()");
          }
        
        
      };
    
    
    /** Register this test class... */
    LAUNCHER (MakeClip_test, "function asset");
    
    
    
  } // namespace test

} // namespace asset
