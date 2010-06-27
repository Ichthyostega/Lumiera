/*
  BasicPipe(Test)  -  checking the basic properties of Pipe Assets
 
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


#include "include/logging.h"
#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/asset/category.hpp"
#include "proc/asset/pipe.hpp"
#include "lib/query.hpp"
#include "proc/assetmanager.hpp"
#include "proc/mobject/session.hpp"
#include "proc/asset/asset-diagnostics.hpp"

#include <boost/format.hpp>
#include <iostream>

using boost::format;
using util::contains;
using util::isnil;
using std::string;
using std::cout;


namespace asset
  {
  namespace test
    {
    using mobject::Session;
    using lumiera::Query;
    using lumiera::query::normaliseID;
    
    
    
    /***********************************************************************
     * @test basic properties of Pipe (structural) Assets.
     *       <ul><li>created by referral</li>
     *           <li>access existing pipe by referral</li>
     *           <li>create with full properties</li>
     *           <li>access ProcPatt</li>
     *           <li>check dependency</li>
     *       </ul>
     */
    class BasicPipe_test : public Test
      {
        virtual void run(Arg arg) 
          {
            string pipeID   = isnil(arg)?  "Black Hole" : arg[1];
            string streamID = 2>arg.size()? "teststream" : arg[2] ;
            
            createExplicit (pipeID,streamID);
            create_or_ref (pipeID);
            create_using_default ();
            dependProcPatt (pipeID);
          }
        
        
        
        
        void createExplicit (string pID, string sID)
          { 
            string pID_sane (pID);
            normaliseID (pID_sane);
            ASSERT (pID_sane != pID);
            
            PPipe thePipe = asset::Struct::create (pID,sID);
            
            ASSERT (thePipe);
            ASSERT (thePipe->getProcPatt());
            ASSERT (thePipe->getPipeID() == pID_sane);
            ASSERT (thePipe->getProcPatt()->queryStreamID() == sID);
            ASSERT (thePipe->shortDesc == pID_sane);
            
            Asset::Ident idi = thePipe->ident;
            ASSERT (idi.org == "lumi");
            ASSERT (contains (idi.name, thePipe->getPipeID()));
            ASSERT (contains (idi.name, thePipe->getProcPatt()->queryStreamID()));

            Category cat (idi.category);
            Category refcat (STRUCT,"pipes");
            ASSERT ( cat.hasKind(STRUCT) );
            ASSERT ( cat.isWithin(refcat) );
          }
        
        
        void create_or_ref(string pID)
          { 
            normaliseID (pID);
            
            PPipe pipe1 = Pipe::query ("pipe("+pID+")");
            ASSERT (pipe1);
            ASSERT (pipe1->getPipeID() == pID);
            
            string pID2 = "another-" + pID;
            PPipe pipe2 = Pipe::query ("pipe("+pID2+")");
            ASSERT (pipe2);
            ASSERT (pipe2 != pipe1);
            Category c1 = pipe1->ident.category;
            Category c2 = pipe2->ident.category;
            ASSERT (c1 == c2);
            
            PPipe pipe3 = Pipe::query ("pipe("+pID2+")");
//////////////////////////////////////////////////////////////TODO: that's broken; creating a new one instead to find the existing one, as it should be            
            ASSERT (pipe3 == pipe2);
          }
        
        
        void create_using_default()
          { 
            PPipe pipe1 = Pipe::query (""); // "the default pipe"
            PPipe pipe2;
            ASSERT (pipe1);
            ASSERT (pipe1 == Session::current->defaults (Query<Pipe>()));
            ASSERT (pipe1->ident.category.hasKind(VIDEO));
            ASSERT (pipe1->getProcPatt());
            PProcPatt popa = Session::current->defaults (Query<const ProcPatt>("pipe(default)"));
            ASSERT (popa == pipe1->getProcPatt());
            
            // several variants to query for "the default pipe"
            pipe2 = Session::current->defaults(Query<Pipe> ());
            ASSERT (pipe2 == pipe1);
            pipe2 = asset::Struct::create (Query<Pipe> ());
            ASSERT (pipe2 == pipe1);
            pipe2 = asset::Struct::create (Query<Pipe> ("pipe(default)"));
            ASSERT (pipe2 == pipe1);
            
            string sID = popa->queryStreamID(); // sort of a "default stream type"
            PPipe pipe3 = Pipe::query ("stream("+sID+")");
            ASSERT (pipe3);
            ASSERT (pipe3->getProcPatt()->queryStreamID() == sID);
            ASSERT (pipe3->getProcPatt() == Session::current->defaults (Query<const ProcPatt>("stream("+sID+")")));
          }
        
        
        void dependProcPatt(string pID)
          {
            typedef P<Pipe> PPipe;                                                         /////TODO: transition to P<>
            typedef P<const ProcPatt> PProcPatt;
            
            PPipe thePipe = Pipe::query ("pipe("+pID+")");
            ASSERT (thePipe);
            PProcPatt thePatt = thePipe->getProcPatt();
            ASSERT (thePatt);
            ASSERT (dependencyCheck (thePipe, thePatt));
            
            PProcPatt pattern2 = thePatt->newCopy("another");
            ASSERT (thePatt != pattern2);
            ASSERT (!dependencyCheck (thePipe, pattern2));
            TODO ("add something to the new pattern, e.g. an effect");

              // now querying for a pipe using this pattern (created on-the-fly)
             //  note: because the pattern is new, this new pipe will be used as
            //         default pipe for this pattern automatically
            PPipe pipe2x = Pipe::query ("pattern(another)");
            ASSERT (pattern2 == pipe2x->getProcPatt());
            ASSERT (pipe2x == Session::current->defaults (Query<Pipe>("pattern(another)")));
            
            thePipe->switchProcPatt(pattern2);
            ASSERT ( dependencyCheck (thePipe, pattern2));
            ASSERT (!dependencyCheck (thePipe, thePatt));
            
            AssetManager& aMang = AssetManager::instance();
            ASSERT ( aMang.known (thePipe->getID()));
            ASSERT ( aMang.known (thePatt->getID()));
            ASSERT ( aMang.known (pattern2->getID()));
            aMang.remove (pattern2->getID());
            ASSERT ( aMang.known (thePatt->getID()));
            ASSERT (!aMang.known (pattern2->getID()));
            ASSERT (!aMang.known (thePipe->getID()));  // has been unlinked too, because dependant on pattern2
            
            ASSERT (thePipe);
            PProcPatt pattern3 = thePipe->getProcPatt();                                   /////TODO: transition to P<>
            ASSERT (thePipe->getProcPatt());
            ASSERT (              pattern3 == pattern2); // but is still valid, as long as the ref is alive....
            
            PPipe pipe3x = Pipe::query ("pattern(another)");
            pattern3 = pipe3x->getProcPatt();                                              /////TODO: transition to P<>
            ASSERT (pattern3 != pattern2);  // because pattern2 is already unlinked...
            ASSERT (pipe3x == Session::current->defaults (Query<Pipe>("pattern(another)")));
            ASSERT (pipe3x != pipe2x);                 // ..we got a new default pipe for "pattern(another)" too!
            
            
            TRACE (asset_mem, "leaving BasicPipe_test::dependProcPatt()");
            // expect now pipe2x and pattern2 to be destroyed...
          }
      };
    
    
    /** Register this test class... */
    LAUNCHER (BasicPipe_test, "unit asset");
    
    
    
  } // namespace test

} // namespace asset
