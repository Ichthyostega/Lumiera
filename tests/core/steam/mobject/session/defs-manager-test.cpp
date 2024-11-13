/*
  DefsManager(Test)  -  checking basic behaviour of the defaults manager

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

/** @file defs-manager-test.cpp
 ** unit test \ref DefsManager_test
 */


#include "lib/test/run.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"
#include "lib/format-string.hpp"
#include "lib/query-util.hpp"
#include "common/query.hpp"

#include "steam/asset.hpp"
#include "steam/asset/pipe.hpp"
#include "steam/asset/struct.hpp"
#include "steam/assetmanager.hpp"
#include "steam/mobject/session.hpp"
#include "steam/streamtype.hpp"

using util::_Fmt;
using util::isnil;
using std::string;



namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  using lib::Symbol;
  using asset::ID;
  using asset::Asset;
  using asset::AssetManager;
  using asset::Pipe;
  using asset::PPipe;
  using asset::Struct;
  using lumiera::Query;
  using lib::query::normaliseID;
  
  
  /** shortcut: run just a query
   *  without creating new instances
   */
  bool
  find (Query<Pipe>& q)
  {
    return bool(Session::current->defaults.search (q));
  }
  
  
  
  
  /*******************************************************************//**
   * @test basic behaviour of the defaults manager ("the big picture")
   *       - retrieving a "default" object repeatedly
   *       - retrieving a more constrained "default" object
   *       - failure registers a new "default"
   *       - instance management
   * 
   * Using pipe assets as an example. The defaults manager shouldn't
   * interfere with memory management (it holds weak refs).
   */
  class DefsManager_test : public Test
    {
      virtual void
      run (Arg arg)
        {
          seedRand();
          string pipeID   = isnil(arg)?  "Black Hole" : arg[1];
          string streamID = 2>arg.size()? "teststream" : arg[2];
          
          normaliseID (pipeID);
          normaliseID (streamID);
          
          retrieveSimpleDefault (pipeID);
          retrieveConstrainedDefault (pipeID, streamID);
          failureCreatesNewDefault();
          verifyRemoval();
        }
      
      
      
      
      void
      retrieveSimpleDefault (string)
        {
          PPipe pipe1 = Pipe::query (""); // "the default pipe"
          PPipe pipe2;
          
          // several variants to query for "the default pipe"
          pipe2 = Pipe::query ("");
          CHECK (pipe2 == pipe1);
          pipe2 = Pipe::query ("default(X)");
          CHECK (pipe2 == pipe1);
          pipe2 = Session::current->defaults(Query<Pipe> (""));
          CHECK (pipe2 == pipe1);
          pipe2 = asset::Struct::retrieve (Query<Pipe> (""));
          CHECK (pipe2 == pipe1);
          pipe2 = asset::Struct::retrieve (Query<Pipe> ("default(P)"));
          CHECK (pipe2 == pipe1);
        }
      
      
      void
      retrieveConstrainedDefault (string pID, string sID)
        {
          PPipe pipe1 = Pipe::query (""); // "the default pipe"
          CHECK ( pipe1->getStreamID() != StreamType::ID{sID},
                  "stream-ID \"%s\" not suitable for test, because "
                  "the default-pipe \"%s\" happens to have the same "
                  "stream-ID. We need it to be different",
                  sID.c_str(), pID.c_str()
                 );
          
          string query_for_sID{"stream("+sID+")"};
          PPipe pipe2 = Pipe::query (query_for_sID);
          CHECK (pipe2->getStreamID() == StreamType::ID{sID});
          CHECK (pipe2 != pipe1);
          CHECK (pipe2 == Pipe::query (query_for_sID));   // reproducible
        }
      
      
      void
      failureCreatesNewDefault()
        {
          PPipe pipe1 = Session::current->defaults(Query<Pipe>{""}); // "the default pipe"
          
          string new_pID = _Fmt{"dummy_%s_%i"}
                               % pipe1->getPipeID()
                               % rani(10'000)
                               ;  // make random new pipeID
          Query<Pipe> query_for_new{"pipe("+new_pID+")"};
          
          CHECK (!find (query_for_new));                              // check it doesn't exist
          PPipe pipe2 = Session::current->defaults (query_for_new);   // triggers creation
          CHECK ( find (query_for_new));                              // check it exists now
          
          CHECK (pipe1 != pipe2);
          CHECK (pipe2 == Session::current->defaults (query_for_new));
        }
      
      
      /** verify the defaults manager holds only weak refs,
       *  so if an object goes out of scope, any defaults entries
       *  are purged silently
       */
      void
      verifyRemoval()
        {
          Symbol pID{"some_pipe"};
          Query<Pipe> query_for_pID{"pipe("+pID+")"};
          size_t hash;
            {
              // create new pipe and declare it to be a default
              PPipe pipe1 = Struct::retrieve.newInstance<Pipe> (pID);
              Session::current->defaults.define (pipe1);
              
              CHECK (2 == pipe1.use_count());                         // the pipe1 smart-ptr and the AssetManager
              hash = pipe1->getID();
            }
            // pipe1 out of scope....
           //  AssetManager now should hold the only ref
          ID<Asset> assetID (hash);
          
          AssetManager& aMang (AssetManager::instance());
          CHECK ( aMang.known (assetID));
          aMang.remove (assetID);
          CHECK (!aMang.known (assetID));
          
          
          CHECK (!find(query_for_pID));                               // bare default-query should fail...
          PPipe pipe2 = Session::current->defaults (query_for_pID);   // triggers re-creation
          CHECK ( find(query_for_pID));                               // should succeed again
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (DefsManager_test, "function session");
  
  
}}}} // namespace steam::mobject::session::test
