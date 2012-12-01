/*
  FakeConfigRules  -  dummy implementation of the config rules system

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


#include "proc/mobject/session/query/fake-configrules.hpp"

#include "proc/mobject/session.hpp"
#include "proc/mobject/session/track.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"
#include "common/query.hpp"


#include "include/logging.h"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

using lib::Literal;
using util::isnil;

//////////////////////////////////////////////////////////////////TICKET #710 : to be removed entirely in Alpha

namespace proc {
namespace mobject {
namespace session {
  
  namespace query {
    
    using asset::Struct;
    using asset::Pipe;
    using asset::PPipe;
    
    using asset::ProcPatt;
    using asset::PProcPatt;
    
//  using lumiera::query::extractID;
    using lumiera::query::removeTerm;
    
    
    namespace {
      
      typedef std::pair<const string, any> AnyPair;
      
      /** helper to simplify creating mock table entries, wrapped correctly */
      template<class TY>
      AnyPair entry (Query<TY> const& query, typename WrapReturn<TY>::Wrapper& obj)
      {
        UNIMPLEMENTED ("generic query-key");////////////////////////////////////////////////////////////////////////////////////////////TODO
        return AnyPair ( "TODO"//Query<TY> (query).asKey()////////////////////////////////////////////////////////////////////////////////////////////TODO
                       , any(obj)); 
      }
      
      /** helper especially for creating structural assets from a capability query */
      template<class STRU>
      AnyPair entry_Struct(Literal caps)
      {
        typedef typename WrapReturn<STRU>::Wrapper Ptr;
        
        string capabilities (caps);
        Query<STRU> query (capabilities); 
        Ptr obj = Struct::retrieve (query);
        UNIMPLEMENTED("generic query-key");
//      return AnyPair(query.asKey(), obj);////////////////////////////////////////////////////////////////////////////////////////////TODO
      }
      
      /** shortcut for simply accessing a table entry */ 
      template<class STRU, class PTAB>
      any&
      item (PTAB& table, const string& query)
      {
        UNIMPLEMENTED ("generic Query-key");////////////////////////////////////////////////////////////////////////////////////////////TODO
        //return (*table)[Query<STRU>(query).asKey()];////////////////////////////////////////////////////////////////////////////////////////////TODO
      }
    
    }
    
    
    /** hard coded answers to configuration queries.
     *  @note while filling the table re-entrance
     *        will be quite common, so the order of
     *        creating the objects is important.
     */
    void
    MockTable::fill_mock_table ()
    {
      INFO (config, "creating mock answers for some hard wired config queries...");
      isInit_ = true; // allow re-entrance
      
      typedef const ProcPatt cPP;
      
      
      // for baiscpipetest.cpp ---------
      answer_->insert (entry_Struct<cPP> ("stream(video)"));
      answer_->insert (entry_Struct<cPP> ("stream(teststream)"));
      item<cPP> (answer_, "stream(default)")  = item<cPP> (answer_,"stream(video)"); // set up a default stream
      
      answer_->insert (entry_Struct<Pipe> ("pipe(master), stream(video)"));
      item<Pipe> (answer_, "") = item<Pipe>(answer_,"pipe(master), stream(video)");//   use as default
      
      answer_->insert (entry_Struct<Pipe> ("pipe(ambiance)"));
    }
    
    
    
    
    /* under some circumstances we need to emulate the behaviour *
     * of a real resolution engine in a more detailed manner.    *
     * The code below implements these cases hard wired.         */
    
    /** special case: create a new pipe with matching pipe and stream IDs on the fly when referred... */
    bool 
    MockTable::fabricate_matching_new_Pipe (Query<Pipe>& q, string const& pipeID, string const& streamID)
    {
      typedef WrapReturn<Pipe>::Wrapper Ptr;
      
      Ptr newPipe (Struct::retrieve.newPipe (pipeID, streamID));
      answer_->insert (entry<Pipe> (q, newPipe));
      return true; // denotes query will now succeed...
    }
    
    /** special case: create a new pipe for a specific stream ID */
    bool 
    MockTable::fabricate_just_new_Pipe (Query<Pipe>& q )
    {
      typedef WrapReturn<Pipe>::Wrapper Ptr;
      
      Ptr newPipe (Struct::retrieve.made4fake (q));
      answer_->insert (entry<Pipe> (q, newPipe));
      return true;
    }
    
    /** special case: create/retrieve new processing pattern for given stream ID... */
    bool 
    MockTable::fabricate_ProcPatt_on_demand (Query<const ProcPatt>& q)
    {
      typedef const ProcPatt cPP;
      typedef WrapReturn<cPP>::Wrapper Ptr;
      
      Ptr newPP (Struct::retrieve.made4fake (q));
      answer_->insert (entry<cPP> (q, newPP));
      return true;
    }
    
    /** special case: fabricate new Timeline, maybe using specific sub-objects
     *  as hinted by the IDs given within the query. This might include searching
     *  the session's timelines / sequences to retrieve an existing object
     *  with matching ID... */
    bool 
    MockTable::fabricate_Timeline_on_demand (Query<asset::Timeline>& query)
    {
      typedef asset::Timeline aTl;
      typedef WrapReturn<aTl>::Wrapper Ptr;
  
      UNIMPLEMENTED ("generic query remolding");////////////////////////////////////////////////////////////////////////////////////////////TODO
      string nameID = "TODO";//removeTerm ("id", query);////////////////////////////////////////////////////////////////////////////////////////////TODO
      if (isnil (nameID))
        nameID = "TODO";//removeTerm ("timeline", query);////////////////////////////////////////////////////////////////////////////////////////////TODO
      if (isnil (nameID))
        nameID = "prime";
//    query.insert (0, "id("+nameID+"), ");
      
      // try to find an existing one with the desired id
      Ptr newTimeline;
      size_t i=0, cnt=Session::current->timelines.size();
      for ( ; i < cnt; ++i)
        if (nameID == Session::current->timelines[i]->ident.name)
          {
            newTimeline = Session::current->timelines[i];
            break;
          }
      
      if (!newTimeline)
        newTimeline = Struct::retrieve.made4fake (query);     // no suitable Timeline found: create and attach new one
      
      answer_->insert (entry<aTl> (query, newTimeline));    //   "learn" the found/created Timeline as new solution
      return true;
    }
    
    /** special case: fabricate new Timeline, maybe using ID specs from the query... */
    bool 
    MockTable::fabricate_Sequence_on_demand (Query<asset::Sequence>& query)
    {
      typedef asset::Sequence aSq;
      typedef WrapReturn<aSq>::Wrapper Ptr;
  
      UNIMPLEMENTED ("generic Query remolding");////////////////////////////////////////////////////////////////////////////////////////////TODO
      string nameID = "TODO";//removeTerm ("id", query);////////////////////////////////////////////////////////////////////////////////////////////TODO
      if (isnil (nameID))
        nameID = "TODO";//removeTerm ("sequence", query);////////////////////////////////////////////////////////////////////////////////////////////TODO
      if (isnil (nameID))
        nameID = "first";
//    query.insert (0, "id("+nameID+"), ");
      
      // try to find an existing sequence with the desired id
      Ptr newSequence;
      size_t i=0, cnt=Session::current->sequences.size();
      for ( ; i < cnt; ++i)
        if (nameID == Session::current->sequences[i]->ident.name)
          {
            newSequence = Session::current->sequences[i];
            break;
          }
      
      if (!newSequence)                                            
        newSequence = Struct::retrieve.made4fake (query);   // no suitable found: create and attach new Sequence
      
      answer_->insert (entry<aSq> (query, newSequence));
      return true;
    }
    
    
    /** for entering "valid" solutions on-the-fly from tests */
    template<class TY>
    bool 
    MockTable::set_new_mock_solution (Query<TY>& q, typename WrapReturn<TY>::Wrapper& obj)
    {
      UNIMPLEMENTED ("generic query-key");////////////////////////////////////////////////////////////////////////////////////////////TODO
//    answer_->erase (q.asKey());////////////////////////////////////////////////////////////////////////////////////////////TODO
      answer_->insert (entry<TY> (q, obj));
      return true;
    }
    // generate the necessary specialisations-----------------------------
    template bool MockTable::set_new_mock_solution (Query<Pipe>&, PPipe&);
    

    
    
    
    MockConfigRules::MockConfigRules () 
    {
      WARN (config, "using a mock implementation of the ConfigQuery interface");
    }
    
    
    MockTable::MockTable ()
      : answer_(new Tab()),
        isInit_(false)
    { }

    

    
    /** this is the (preliminary/mock) implementation
     *  handling queries for objects of a specific type
     *  and with capabilities or properties defined by
     *  the query. The real implementation would require
     *  a rule based system (Ichthyo plans to use YAP Prolog),
     *  while this dummy implementation simply relpies based
     *  on a table of pre-fabricated objects. Never fails.
     *  @return smart ptr (or similar) holding the object,
     *          maybe an empty smart ptr if not found
     */
    const any& 
    MockTable::fetch_from_table_for (const string& queryStr)
    {
      static const any NOTFOUND;
      if (!isInit_) fill_mock_table();
      
      Tab::iterator i = answer_->find (queryStr);
      if (i == answer_->end())
        return NOTFOUND;
      else
        return i->second;
    }
    

  
  } // namespace query
}}} // namespace proc::mobject::session
