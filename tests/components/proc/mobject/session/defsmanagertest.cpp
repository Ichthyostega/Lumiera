/*
  DefsManager(Test)  -  checking basic behaviour of the defaults manager
 
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


#include "common/test/run.hpp"
#include "common/util.hpp"

#include "proc/asset.hpp"
#include "proc/asset/pipe.hpp"
#include "common/query.hpp"
#include "proc/assetmanager.hpp"
#include "proc/mobject/session.hpp"

#include <boost/format.hpp>
#include <cstdlib>

using boost::format;
using util::isnil;
using std::string;


namespace asset
  {
  namespace test
    {
    using mobject::Session;
    using cinelerra::Query;
    using cinelerra::query::normalizeID;
    
    
    
    /***********************************************************************
     * @test basic behaviour of the defaults manager.
     *       <ol><li>retrieving a "default" object repeatedly</li>
     *           <li>retrieving a more constrained "default" object</li>
     *           <li>failure registers a new "default"</li>
     *       </ol>
     * Using pipe assets as an example. The defaults manager shouldn't
     * interfere with memory management (it holds weak refs).
     */
    class DefsManager_test : public Test
      {
        virtual void run(Arg arg) 
          {
            string pipeID   = isnil(arg)?  "Black Hole" : arg[1];
            string streamID = 2>arg.size()? "teststream" : arg[2] ;
            
            normalizeID (pipeID);
            normalizeID (streamID);
            
            retrieveSimpleDefault (pipeID);
            retrieveConstrainedDefault (pipeID, streamID);
            pipeID = failureCreatesNewDefault();
            verifyRemoval (pipeID);
          }
        
        
        
        
        void retrieveSimpleDefault(string pID)
          { 
            PPipe pipe1 = Pipe::query (""); // "the default pipe"
            PPipe pipe2;
            
            // several variants to query for "the default pipe"
            pipe2 = Pipe::query ("");
            ASSERT (pipe2 == pipe1);
            pipe2 = Pipe::query ("default(X)");
            ASSERT (pipe2 == pipe1);
            pipe2 = Session::current->defaults(Query<Pipe> ());
            ASSERT (pipe2 == pipe1);
            pipe2 = asset::Struct::create (Query<Pipe> ());
            ASSERT (pipe2 == pipe1);
            pipe2 = asset::Struct::create (Query<Pipe> ("default(X)"));
            ASSERT (pipe2 == pipe1);
          }
        
        
        void retrieveConstrainedDefault(string pID, string sID)
          { 
            PPipe pipe1 = Pipe::query (""); // "the default pipe"
            ASSERT (sID != pipe1->getProcPatt()->queryStreamID(),
                    "stream-ID \"%s\" not suitable for test, because "
                    "the default-pipe \"%s\" happens to have the same "
                    "stream-ID. We need it to be different", 
                    sID.c_str(), pID.c_str()
                   );
            
            string query_for_sID ("stream("+sID+")");
            PPipe pipe2 = Pipe::query (query_for_sID);
            ASSERT (sID == pipe2->getProcPatt()->queryStreamID());
            ASSERT (pipe2 != pipe1);
            ASSERT (pipe2 == Pipe::query (query_for_sID));   // reproducible
          }
        
        
        string failureCreatesNewDefault ()
          { 
            PPipe pipe1 = Session::current->defaults(Query<Pipe> ()); // "the default pipe"

            string new_pID (str (format ("dummy_%s_%i") 
                                 % pipe1->getPipeID()
                                 % std::rand()
                                ));     // make random new pipeID  
            string query_for_new ("pipe("+new_pID+")");
            PPipe pipe2 = Session::current->defaults(Query<Pipe> (query_for_new));
            
            TODO ("a way to do only a query, without creating. Use this to check it is really added as default");
            
            ASSERT (pipe1 != pipe2);
            ASSERT (pipe2 == Pipe::query (query_for_new));
            return new_pID;
          }
        
        
        void verifyRemoval(string pID)
          { 
            string query_for_pID ("pipe("+pID+")");
            size_t hash;
              {
                PPipe pipe1 = Pipe::query (query_for_pID);
                hash = pipe1->getID();
              }
             // now AssetManager should have the only ref
            ID<Asset> assetID (hash);
              
            AssetManager& aMang (AssetManager::instance());
            ASSERT ( aMang.known (assetID));
            aMang.remove (assetID);
            ASSERT (!aMang.known (assetID));
            
            TODO ("assure the bare default-query now fails...");
            PPipe pipe2 = Session::current->defaults(Query<Pipe> (query_for_pID));
            TODO ("assure the bare default-query now succeeds...");
          }
      };
    
    
    /** Register this test class... */
    LAUNCHER (DefsManager_test, "function session");
    
    
    
  } // namespace test

} // namespace asset
