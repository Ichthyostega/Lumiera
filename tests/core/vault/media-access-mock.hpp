/*
  MEDIA-ACCESS-MOCK.hpp  -  a test (stub) target object for testing the factories

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

/** @file media-access-mock.hpp
 ** Unit test helper to access an emulated media file.
 ** This can be used to inject specific metadata or media properties,
 ** or even to feed generated media content into the code to be tested.
 ** 
 ** @todo this facility was occasionally used until 2011, yet never really completed
 */


#ifndef VAULT_TEST_MEDIA_ACCESS_MOCK_H
#define VAULT_TEST_MEDIA_ACCESS_MOCK_H


#include "vault/media-access-facade.hpp"




namespace backend {
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
  
  
}} // namespace backend::test
#endif
