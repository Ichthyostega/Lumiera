/*
  BasicPort(Test)  -  checking the basic properties of Port Assets
 
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

#include "proc/asset/category.hpp"
#include "proc/asset/port.hpp"
#include "common/query.hpp"
#include "proc/assetmanager.hpp"
#include "proc/mobject/session.hpp"
#include "proc/asset/assetdiagnostics.hpp"

#include <boost/format.hpp>
#include <iostream>

using boost::format;
using util::contains;
using util::isnil;
using std::string;
using std::wstring;
using std::cout;


namespace asset
  {
  namespace test
    {
    using mobject::Session;
    using cinelerra::Query;
    using cinelerra::query::normalizeID;
    
    
    
    /***********************************************************************
     * @test basic properties of Port (structural) Assets.
     *       <ul><li>created by referral</li>
     *           <li>access existing port by referral</li>
     *           <li>create with full properties</li>
     *           <li>access ProcPatt</li>
     *           <li>check dependency</li>
     *       </ul>
     */
    class BasicPort_test : public Test
      {
        virtual void run(Arg arg) 
          {
            string portID   = isnil(arg)?  "blackHole" : arg[1];
            string streamID = 2>arg.size()? "teststream" : arg[2] ;
            
            createExplicit (portID,streamID);
            create_or_ref (portID);
            dependProcPatt (portID);
          }
        
        
        
        
        void createExplicit (string pID, string sID)
          { 
            string pID_sane (pID);
            normalizeID (pID_sane);
            
            PPort thePort = asset::Struct::create (pID,sID);
            
            ASSERT (thePort);
            ASSERT (thePort->getProcPatt());
            ASSERT (thePort->getPortID() == pID_sane);
            ASSERT (thePort->getProcPatt()->queryStreamID() == sID);
            ASSERT (thePort->shortDesc == wstring (pID_sane.begin(), pID_sane.end()));
            
            Asset::Ident idi = thePort->ident;
            ASSERT (idi.org == "cin3");
            ASSERT (contains (idi.name, thePort->getPortID()));
            ASSERT (contains (idi.name, thePort->getProcPatt()->queryStreamID()));

            Category cat (idi.category);
            Category refcat (STRUCT,"ports");
            ASSERT ( cat.hasKind(STRUCT) );
            ASSERT ( cat.isWithin(refcat) );
          }
        
        
        void create_or_ref(string pID)
          { 
            normalizeID (pID);
            
            PPort port1 = Port::query ("port("+pID+")");
            ASSERT (port1);
            ASSERT (port1->getPortID() == pID);
            
            string pID2 = "another-" + pID;
            PPort port2 = Port::query ("port("+pID2+")");
            ASSERT (port2);
            ASSERT (port2 != port1);
            Category c1 = port1->ident.category;
            Category c2 = port2->ident.category;
            ASSERT (c1 == c2);
            
            PPort port3 = Port::query ("port("+pID2+")");
            ASSERT (port3 == port2);
          }
        
        
        void create_using_default()
          { 
            PPort port1 = Port::query (""); // "the default port"
            PPort port2;
            ASSERT (port1);
            ASSERT (port1 == Session::current->defaults (Query<Port>()));
            ASSERT (port1->ident.category.hasKind(VIDEO));
            ASSERT (port1->getProcPatt());
            PProcPatt popa = Session::current->defaults (Query<ProcPatt>("port()"));
            ASSERT (popa == port1->getProcPatt());
            
            // several variants to query for "the default port"
            port2 = Session::current->defaults(Query<Port> ());
            ASSERT (port2 == port1);
            port2 = asset::Struct::create (Query<Port> ());
            ASSERT (port2 == port1);
            port2 = asset::Struct::create (Query<Port> ("port()"));
            ASSERT (port2 == port1);
            
            string sID = popa->queryStreamID(); // sort of a "default stream type"
            PPort port3 = Port::query ("stream("+sID+")");
            ASSERT (port3);
            ASSERT (port3->getProcPatt()->queryStreamID() == sID);
            ASSERT (port3->getProcPatt() == Session::current->defaults (Query<ProcPatt>("stream("+sID+")")));
          }
        
        
        void dependProcPatt(string pID)
          {
            PPort thePort = Port::query ("port("+pID+")");
            ASSERT (thePort);
            PProcPatt thePatt = thePort->getProcPatt();
            ASSERT (thePatt);
            ASSERT (dependencyCheck (thePort, thePatt));
            
            PProcPatt pattern2 = thePatt->newCopy("another");
            ASSERT (thePatt != pattern2);
            ASSERT (!dependencyCheck (thePort, pattern2));
            TODO ("add something to the new pattern, e.g. an effect");

              // now querying for a port using this pattern (created on-the-fly)
             //  note: because the pattern is new, this new port will be used as
            //         default port for this pattern automatically
            PPort port2x = Port::query ("pattern(another)");
            ASSERT (pattern2 == port2x->getProcPatt());
            ASSERT (port2x == Session::current->defaults (Query<Port>("pattern(another)")));
            
            thePort->switchProcPatt(pattern2);
            ASSERT ( dependencyCheck (thePort, pattern2));
            ASSERT (!dependencyCheck (thePort, thePatt));
            
            AssetManager& aMang = AssetManager::instance();
            ASSERT ( aMang.known (thePort->getID()));
            ASSERT ( aMang.known (thePatt->getID()));
            ASSERT ( aMang.known (pattern2->getID()));
            aMang.remove (pattern2->getID());
            ASSERT ( aMang.known (thePatt->getID()));
            ASSERT (!aMang.known (pattern2->getID()));
            ASSERT (!aMang.known (thePort->getID()));  // has been unlinked too, because dependant on pattern2
            
            ASSERT (thePort);
            ASSERT (thePort->getProcPatt());
            ASSERT (thePort->getProcPatt() == pattern2); // but is still valid, as long as the ref is alive....
            
            PPort port3x = Port::query ("pattern(another)");
            ASSERT (port3x->getProcPatt() != pattern2);  // because pattern2 is already unlinked...
            ASSERT (port3x == Session::current->defaults (Query<Port>("pattern(another)")));
            ASSERT (port3x != port2x);                 // ..we got a new default port for "pattern(another)" too!
            
            
            TRACE (assetmem, "leaving BasicPort_test::dependProcPatt()");
            // expect now port2x and pattern2 to be destroyed...
          }
      };
    
    
    /** Register this test class... */
    LAUNCHER (BasicPort_test, "unit asset");
    
    
    
  } // namespace test

} // namespace asset
