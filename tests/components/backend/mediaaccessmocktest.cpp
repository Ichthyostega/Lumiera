/*
  MediaAccessMock(Test)  -  checking our Test Mock replacing the MediaAccessFacade
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "backend/mediaaccessfacade.hpp"
#include "backend/mediaaccessmock.hpp"

#include "common/test/run.hpp"
//#include "common/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
//using util::isnil;
using std::string;
using std::cout;


namespace backend_interface
  {
  namespace test
    {
    
    

    
    
    /********************************************************************************
     * @test inject a Mock object replacing the backend_interface::MediaAccessFacade.
     *       Verify if the Mock object behaves as expected when calling the Facade.
     */
    class MediaAccessMock_test : public Test
      {
        typedef MediaAccessFacade MAF;
        
        virtual void run(Arg arg) 
          {
            MAF::instance.injectSubclass (new MediaAccessMock);
            
            queryScenario ("test-1");
            queryScenario ("test-2");
            
            MAF::instance.injectSubclass (0);
          }

        
        /** perform the test: query for an (alledged) file
         *  and retrieve the mock answer. 
         */
        void queryScenario (string filename)
          {
            MAF& maf (MAF::instance());
            MAF::FileHandle fhandle = maf.queryFile (filename.c_str());
            if (!fhandle)
              cout << "File \""<<filename<<"\" not accessible\n";
            else
              {
                cout << "accessing \""<<filename<<"\" ...\n";
                for (int i=0; true; ++i)
                  {
                    ChanDesc chan = maf.queryChannel (fhandle, i);
                    if (!chan.handle) break;
                    
                    cout << " Channel-" << i
                         << ": nameID=" << chan.chanID
                         << " codecID=" << chan.codecID
                         << "\n";
          }   }   }
        
        
      };
    
      
    
    /** Register this test class... */
    LAUNCHER (MediaAccessMock_test, "unit operate");
    
    
    
  } // namespace test

} // namespace backend_interface
