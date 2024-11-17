/*
  MediaAccessMock(Test)  -  checking our Test Mock replacing the MediaAccessFacade

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file media-access-mock-test.cpp
 ** unit test \ref MediaAccessMock_test
 **
 ** @todo this test must be linked against the Core (including Steam),
 **       yet conceptually it belongs into the Vault, indicating a design mismatch.
 **       The test support facility drafted here was not used much since then.
 */


#include "vault/media-access-facade.hpp"
#include "vault/media-access-mock.hpp"
#include "lib/depend-inject.hpp"
#include "lib/format-cout.hpp"

#include "lib/test/run.hpp"
#include "lib/symbol.hpp"

#include <string>

using lib::DependInject;
using lib::Literal;
using std::string;


namespace vault {
namespace test {
  
  
  
  
  /****************************************************************************//**
   * @test inject a Mock object replacing the backend_interface::MediaAccessFacade.
   *       Verify if the Mock object behaves as expected when calling the Facade.
   */
  class MediaAccessMock_test : public Test
    {
      typedef MediaAccessFacade MAF;
      
      virtual void run(Arg) 
        {
          DependInject<MediaAccessFacade>::Local<MediaAccessMock> useMockMedia;
          
          queryScenario ("test-1");
          queryScenario ("test-2");
        }
      
      
      /** perform the test: query for an (alleged) file
       *  and retrieve the mock answer. 
       */
      void queryScenario (string const& filename)
        {
          MAF& maf (MAF::instance());
          MediaDesc& mhandle = maf.queryFile (filename);
          cout << "accessing \""<<filename<<"\" (dur="<<mhandle.length<<")...\n";
          for (int i=0; true; ++i)
            {
              ChanDesc chan = maf.queryChannel (mhandle, i);
              if (!chan.handle) break;
              
              cout << " Channel-" << i
                   << ": nameID=" << chan.chanID
                   << " codecID=" << chan.codecID
                   << "\n";
        }   }
      
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (MediaAccessMock_test, "unit integration");
  
  
  
}} // namespace vault::test
