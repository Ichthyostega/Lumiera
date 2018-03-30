/*
  TestClip  -  test clip (stub) for checking Model/Session functionality

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

/** @file testclip.cpp
 ** Implementation of a stubbed clip object to support unit testing
 */


#include "proc/mobject/session/testclip.hpp"
#include "backend/media-access-facade.hpp"
#include "backend/media-access-mock.hpp"
#include "lib/depend-inject.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/clip.hpp"
#include "lib/depend.hpp"
#include "lib/time/timevalue.hpp"

namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using lib::time::Time;
  using lib::time::Duration;
  typedef shared_ptr<asset::Media> PM;
  typedef backend_interface::MediaAccessFacade MAF;
  using backend::test::MediaAccessMock;
  using asset::VIDEO;
  
  
  
  asset::Media & 
  createTestMedia ()
  {
    // install Mock-Interface to Lumiera backend
    lib::test::Depend4Test<MediaAccessMock> withinThisScope;
    
    return *asset::Media::create("test-2", VIDEO); // query magic filename
  }
  
  
  asset::Clip & 
  createTestClipAsset (asset::Media& media)
  {
    return *(asset::Media::create(media));
  }
  
  
  struct Testbed
    {
      asset::Media & media_;
      asset::Clip & clipA_;
      
      Testbed() 
        : media_ (createTestMedia()),
          clipA_ (createTestClipAsset(media_))
        { }
    };
      
  lib::Depend<Testbed> testbed_1; // invoke ctor when creating first TestClip...
  
  
  
  
  
  TestClip::TestClip ()
    : Clip(testbed_1().clipA_,
           testbed_1().media_)
  {
    CHECK (isValid());
  }
  
  
  /* == define some data for verification in unit tests == */
  
  const Duration LENGTH_TestClip(Time(0,25,0,0));   //////TODO hard wired as of (1/10). See MediaFactory::operator()  in media.cpp / mediaaccessmock.cpp
  
  
}}}} // namespace proc::mobject::session::test
