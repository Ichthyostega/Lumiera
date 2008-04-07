/*
  MockConfigRules  -  mock implementation of the config rules system
 
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


#include "common/query/mockconfigrules.hpp"

#include "proc/mobject/session/track.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"


#include "common/util.hpp"
#include "nobugcfg.h"

using util::isnil;


namespace lumiera
  {
  
  namespace query
    {
    using asset::Struct;
    using asset::Pipe;
    using asset::PPipe;
    
    using asset::ProcPatt;
    using asset::PProcPatt;
    
    namespace
      {
      typedef std::pair<const string, any> AnyPair;
      
      /** helper to simplify creating mock table entries, wrapped correctly */
      template<class TY>
      AnyPair entry (const string& query, typename WrapReturn<TY>::Wrapper& obj)
      {
        return AnyPair ( Query<TY> (query).asKey()
                       , any(obj)); 
      }
      
      /** helper especially for creating structural assets from a capability query */
      template<class STRU>
      AnyPair entry_Struct(Symbol caps)
      {
        typedef typename WrapReturn<STRU>::Wrapper Ptr;
        
        Query<STRU> query(caps); 
        Ptr obj = Struct::create (query);
        return AnyPair(query.asKey(), obj);
      }
      
      /** shortcut for simply accessing a table entry */ 
      template<class STRU, class PTAB>
      any&
      item (PTAB& table, const string& query)
      {
        return (*table)[Query<STRU>(query).asKey()];
      }
    
    }
    
    
    /** hard coded answers to configuration queries.
     *  @note while filling the table re-entrace
     *        will be quite common, so the order of
     *        creating the objects is important.
     */
    void
    MockTable::fill_mock_table ()
    {
      INFO (config, "creating mock answers for some config queries...");
      isInit_ = true; // allow re-entrance
      
      typedef const ProcPatt cPP;
      
      
      // for baiscpipetest.cpp ---------
      answer_->insert (entry_Struct<cPP> ("stream(video)"));
      answer_->insert (entry_Struct<cPP> ("stream(teststream)"));
      item<cPP> (answer_, "stream(default)")  = item<cPP> (answer_,"stream(video)"); //TODO killme
      
      answer_->insert (entry_Struct<Pipe> ("pipe(master), stream(video)"));
      item<Pipe> (answer_, "pipe(default)")   = item<Pipe>(answer_,"pipe(master), stream(video)"); //TODO killme
      TODO ("remove the default entries!!!  DefaultsManager should find them automatically");
    }
    
    
    
    
    /* under some circumstances we need to emulate the behaviour *
     * of a real resolution engine in a more detailed manner.    *
     * These case are hard wired in code below                   */
    
    /** special case: create a new pipe with matching pipe and stream IDs on the fly when referred... */
    bool 
    MockTable::fabricate_matching_new_Pipe (Query<Pipe>& q, string const& pipeID, string const& streamID)
    {
      typedef WrapReturn<Pipe>::Wrapper Ptr;
      
      Ptr newPipe (Struct::create (pipeID, streamID));
      answer_->insert (entry<Pipe> (q, newPipe));
      return true; // denotes query will now succeed...
    }
    /** special case: create/retrieve new rocessing pattern for given stream ID... */
    bool 
    MockTable::fabricate_ProcPatt_on_demand (Query<const ProcPatt>& q, string const& streamID)
    {
      typedef const ProcPatt cPP;
      typedef WrapReturn<cPP>::Wrapper Ptr;
      
      Ptr newPP (Struct::create (Query<cPP> ("make(PP), "+q)));
      answer_->insert (entry<cPP> (q, newPP));
      return true;
    }
    
    /** for entering "valid" solutions on-the-fly from tests */
    template<class TY>
    bool 
    MockTable::set_new_mock_solution (Query<TY>& q, typename WrapReturn<TY>::Wrapper& obj)
    {
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

} // namespace lumiera
