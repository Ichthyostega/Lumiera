/*
  StreamTypeBasics(Test)  -  check the fundamentals of stream type information
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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

#include "proc/control/stypemanager.hpp"
#include "teststreamtypes.hpp"

#include <iostream>
using std::cout;



namespace lumiera {
  namespace test {

    using control::STypeManager;
  

    /*******************************************************************
     * @test check the basic workings of the stream type handling.
     *       create some stream implementation data, build a 
     *       StreamType::ImplFacade from this, and derive a prototype
     *       and a full StreamType based on this information.
     */
    class StreamTypeBasics_test : public Test
      {
        virtual void run (Arg arg)
          {
            buildImplType ();
            basicImplTypeProperties ();
          }
        
        void buildImplType ()
          {
            STypeManager& typeManager = STypeManager::instance();
        
            TODO ("set up a GAVL frame type");
            TODO ("use this to retrieve an ImplFacade from the STypeManager");
            UNIMPLEMENTED ("at least preliminary implementation of the MediaImplLib interface for lib GAVL");
            TODO ("how to do a simple consistency check on the returned ImplFacade? can we re-create the GAVL frame type?");
          }
        
        void basicImplTypeProperties ()
          {
            StreamType::ImplFacade& iType = createImplType ();
            
            UNIMPLEMENTED ("get a lib descriptor"); 
            UNIMPLEMENTED ("check the lib of the type"); 
            UNIMPLEMENTED ("compare two types"); 
          }
      };
    
      LAUNCHER (StreamTypeBasics_test, "unit common");

      
  } // namespace test
    
} // namespace lumiera

