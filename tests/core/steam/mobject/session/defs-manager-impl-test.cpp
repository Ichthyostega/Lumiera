/*
  DefsManagerImpl(Test)  -  checking implementation details of the defaults manager

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

/** @file defs-manager-impl-test.cpp
 ** unit test \ref DefsManagerImpl_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "steam/asset.hpp"
#include "steam/asset/pipe.hpp"
#include "steam/asset/struct.hpp"
#include "steam/config-resolver.hpp"
#include "steam/assetmanager.hpp"
#include "steam/mobject/session.hpp"
#include "steam/streamtype.hpp"
#include "lib/format-string.hpp"
#include "lib/query-util.hpp"
#include "common/query.hpp"

using util::_Fmt;
using util::isnil;
using std::string;


namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using lib::Symbol;
  using asset::Asset;
  using asset::AssetManager;
  using asset::Pipe;
  using asset::PPipe;
  using asset::Struct;
  
  using lumiera::Query;
  using lumiera::query::QueryHandler;
  using lib::query::normaliseID;
  
  using proc::ConfigResolver;
  
  
  namespace { // test fixture...
    
    /** shortcut: query for given Pipe-ID */
    bool
    find (const string& pID)
    {
      return bool(Session::current->defaults.search (Query<Pipe> ("pipe("+pID+")")));
    }
    
    
    _Fmt pattern ("dummy_%s_%i");
    
    /** create a random new ID */
    string
    newID (Symbol prefix)
    {
      return pattern % prefix % std::rand();
    }
  }//(end)test fixture
  
  
  
  
  /********************************************************************//**
   * @test verify some general assumptions regarding implementation details
   *       of the the defaults manager.
   * @see  DefsManager_test for the "big picture"
   */
  class DefsManagerImpl_test : public Test
    {
      virtual void
      run(Arg)
        {
          define_and_search();
          string pipeID = create();
          forget(pipeID);
        }
      
      
      
      
      void
      define_and_search ()
        {
          string sID = newID ("stream");
          StreamType::ID stID (sID);
          
           // create Pipes explicitly 
          //  (without utilising default queries)
          PPipe pipe1 = Struct::retrieve.newPipe (newID("pipe"), newID("stream"));
          PPipe pipe2 = Struct::retrieve.newPipe (newID("pipe"), sID            );
          
          CHECK (pipe1 != pipe2);
          CHECK (stID == pipe2->getStreamID());
          
          CHECK (!find (pipe1->getPipeID()), "accidental clash of random test-IDs");
          CHECK (!find (pipe2->getPipeID()), "accidental clash of random test-IDs");
          
          // now declare that these objects should be considered "default"
          Query<Pipe> justAnyPipe ("");
lumiera::query::setFakeBypass(justAnyPipe);  /////////////////////////////////////////////////TODO mock resolution
          CHECK (Session::current->defaults.define (pipe1, justAnyPipe));   // unrestricted default
          
          Query<Pipe> pipeWithSpecificStream("stream("+sID+")");
lumiera::query::setFakeBypass(pipeWithSpecificStream); ///////////////////////////////////TODO mock resolution
          CHECK (Session::current->defaults.define (pipe2, pipeWithSpecificStream));
          
          CHECK ( find (pipe1->getPipeID()), "failure declaring object as default");
          CHECK ( find (pipe2->getPipeID()), "failure declaring object as default");
          
          CHECK (stID != pipe1->getStreamID(), "accidental clash");
          CHECK (!Session::current->defaults.define (pipe1, Query<Pipe> ("stream("+sID+")")));
                  // can't be registered with this query, due to failure caused by wrong stream-ID
        }
      
      
      const string&
      create()
        {
          string sID = newID ("stream");
          Query<Pipe> query_for_streamID ("stream("+sID+")");
          
          // issue a ConfigQuery directly, without involving the DefaultsManager
          QueryHandler<Pipe>& typeHandler = ConfigResolver::instance();
          PPipe pipe1;
          typeHandler.resolve (pipe1, query_for_streamID);
          CHECK (pipe1);
          
          CHECK (!find (pipe1->getPipeID()));
          PPipe pipe2 = Session::current->defaults.create (query_for_streamID);
          CHECK (pipe2);
          CHECK (pipe2 == pipe1);
          CHECK ( find (pipe1->getPipeID())); // now declared as "default Pipe" for this stream-ID
          
          return pipe1->getPipeID();
        }
      
      
      void
      forget (string pID)
        {
          PPipe pipe = Pipe::query ("pipe("+pID+")");
          REQUIRE (find (pipe->getPipeID()), "test assumes pre-registered default pipe");
          long cnt = pipe.use_count();
          
          // now de-register the pipe as "default Pipe"
          CHECK (Session::current->defaults.forget (pipe));
          CHECK (!find (pipe->getPipeID()));
          CHECK (cnt == pipe.use_count());   // indicates DefaultsManager holding only a weak ref.
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DefsManagerImpl_test, "function session");
  
  
  
}}}} // namespace proc::mobject::session::test
