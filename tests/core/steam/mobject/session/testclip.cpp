/*
  TestClip  -  test clip (stub) for checking Model/Session functionality

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file testclip.cpp
 ** Implementation of a stubbed clip object to support unit testing
 */


#include "steam/mobject/session/testclip.hpp"
#include "vault/media-access-facade.hpp"
#include "vault/media-access-mock.hpp"
#include "lib/depend-inject.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/clip.hpp"
#include "lib/depend.hpp"
#include "lib/time/timevalue.hpp"

namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  using lib::time::Time;
  using lib::time::Duration;
  using asset::VIDEO;
  
  using PM = shared_ptr<asset::Media>;
  using MAF = backend_interface::MediaAccessFacade;
  
  using MediaAccessMock = lib::DependInject<MAF>::Local<vault::test::MediaAccessMock>;
  
  
  
  asset::Media & 
  createTestMedia ()
  {
    // install Mock-Interface to Lumiera Vault
    MediaAccessMock useMockMedia;
    
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
  
  
}}}} // namespace steam::mobject::session::test
