/*
  MEDIA-ACCESS-MOCK.hpp  -  a test (stub) target object for testing the factories

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file media-access-mock.hpp
 ** Unit test helper to access an emulated media file.
 ** This can be used to inject specific metadata or media properties,
 ** or even to feed generated media content into the code to be tested.
 ** 
 ** @todo this facility was occasionally used until 2011, yet never really completed
 ** @todo the implementation is linked against steam-layer to use a constant definition
 **       from 'steam/mobject/session/testclip.hpp' — which highlights an conceptual
 **       ambiguity underlying this whole concept; at inception time, there was no
 **       clear notion pertaining the kind of structures related to the vault.
 */


#ifndef VAULT_TEST_MEDIA_ACCESS_MOCK_H
#define VAULT_TEST_MEDIA_ACCESS_MOCK_H


#include "vault/media-access-facade.hpp"




namespace vault {
namespace test {
  
  /**
   * Mock implementation of the MediaAccessFacade. 
   * Provides preconfigured responses for some Test-Filenames.
   */
  class MediaAccessMock
    : public MediaAccessFacade
    {
      MediaDesc& queryFile (string const& name)        const override;
      ChanDesc queryChannel (MediaDesc&, uint chanNo)  const override;
    };
  
  
}} // namespace vault::test
#endif
