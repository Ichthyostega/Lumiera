/*
  MakeClip(Test)  -  create a Clip from a Media Asset

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file make-clip-test.cpp
 ** unit test \ref MakeClip_test
 */


#include "include/logging.h"
#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "lib/time/timevalue.hpp"
#include "steam/assetmanager.hpp"
#include "steam/asset/media.hpp"
#include "steam/mobject/session/clip.hpp"
#include "steam/asset/asset-diagnostics.hpp"
#include "vault/media-access-mock.hpp"
#include "lib/depend-inject.hpp"

using util::contains;
using util::isnil;
using std::string;

using std::static_pointer_cast; //TODO only temporarily;

namespace steam {
namespace asset{
namespace test {
  
  using MediaAccessMock = lib::DependInject<vault::MediaAccessFacade>
                                ::Local<vault::test::MediaAccessMock>;
  
  
  
  
  /*******************************************************************//**
   * @test creating a Clip MObject and an associated Clip Asset from 
   *       a given asset::Media.
   * @see  asset::Media#createClip
   */
  class MakeClip_test : public Test
    {
      typedef lib::P<asset::Media> PM;
      typedef asset::Media::PClip PC;
          
      virtual void run (Arg) 
        {
          MediaAccessMock useMockMedia;
          
          
          PM mm = asset::Media::create("test-1", VIDEO);
          PC cc = mm->createClip();
          PM cm = cc->getMedia();
          
          CHECK (cm);
          CHECK (!isnil (cc->getLength()));
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
  
  
  
}}} // namespace steam::asset::test
