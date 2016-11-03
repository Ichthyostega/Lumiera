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

/** @file §§§
 ** unit test TODO §§§
 */


#ifndef BACKEND_TEST_MEDIA_ACCESS_MOCK_H
#define BACKEND_TEST_MEDIA_ACCESS_MOCK_H


#include "backend/media-access-facade.hpp"




namespace backend {
namespace test {
  
  /**
   * Mock implementation of the MediaAccessFacade. 
   * Provides preconfigured responses for some Test-Filenames.
   */
  class MediaAccessMock : public MediaAccessFacade
    {
      MediaDesc& queryFile (string const& name)        const;
      ChanDesc queryChannel (MediaDesc&, uint chanNo)  const;
      
    public:
      typedef MediaAccessFacade ServiceInterface;
    };
  
  
}} // namespace backend::test
#endif
