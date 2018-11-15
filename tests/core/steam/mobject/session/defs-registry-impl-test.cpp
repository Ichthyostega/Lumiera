/*
  DefsRegistryImpl(Test)  -  verifying correct behaviour of the defaults registry

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

/** @file defs-registry-impl-test.cpp
 ** unit test \ref DefsRegistryImpl_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "common/query.hpp"
#include "common/query/defs-registry.hpp"
#include "lib/format-string.hpp"
#include "lib/p.hpp"

#include "lib/query-diagnostics.hpp"

#include <memory>
#include <map>

using util::isnil;
using util::_Fmt;
using lib::P;

using std::unique_ptr;
using std::string;
using std::rand;
using std::map;



namespace lumiera {
namespace query   {
namespace test    {
  
  namespace { // Test helpers...
    
    _Fmt instancePatt ("obj_%s_%i");
    
    
    /** create a random new ID */
    string
    newID (string prefix)
    {
      return instancePatt % prefix % rand();
    }
    
    
    /** template for generating some different test types */
    template<int I>
    struct Dummy
      {
        static string name;
        string instanceID;
        operator string ()                  const { return instanceID; }
        bool operator== (const Dummy& odu)  const { return this == &odu; }
        
        
        Dummy () : instanceID (newID (name)) {}
      };
    
    template<int I>
    string Dummy<I>::name = _Fmt("Dummy<%2i>") % I;
    
    template<int I>
    inline P<Dummy<I>>
    fabricate()
    {
      return P<Dummy<I>>(new Dummy<I>);
    }
    
  }//(End)Test helpers
  
  using lib::query::test::garbage_query;
  
  
  
  
  
  /********************************************************************//**
   * @test build an registry table (just for this test) configured for
   *       some artificial test Types. Register some entries and verify
   *       the intended behaviour of the storage structure.
   * @see  DefsManagerImpl_test for checking the implementation details
   *       in the actual context used in Lumiera.
   */
  class DefsRegistryImpl_test : public Test
    {
      unique_ptr<DefsRegistry> reg_;
      
      typedef P<Dummy<13>> Obj;
      typedef P<Dummy<23>> Pra;
      
      typedef Query<Dummy<13>> Q13;
      typedef Query<Dummy<23>> Q23;
      
      typedef DefsRegistry::Iter<Dummy<13>> Iter13;
      typedef DefsRegistry::Iter<Dummy<23>> Iter23;
      
      
      // some test objects...
      Obj o1, o2, o3;
      Q13 q1, q2, q3, q4, q5;
      map<QueryKey, Pra> ps;
      
    public:
      DefsRegistryImpl_test ()
        : o1 (fabricate<13>())
        , o2 (fabricate<13>())
        , o3 (fabricate<13>())
        , q1 (garbage_query (1))
        , q2 (garbage_query (2))
        , q3 (garbage_query (3))
        , q4 (garbage_query (4))
        , q5 (garbage_query (5))
        { }
      
      
      virtual void
      run (Arg)
        {
          this->reg_.reset (new DefsRegistry);
          
          fill_table ();
          check_query ();
          check_remove ();
        }
      
      
      
      
      void
      fill_table ()
        {
           // at start the registry is indeed empty
          //  thus a query doesn't yield any results....
          CHECK ( ! *(reg_->candidates(Q13 ("something"))) );
          
          reg_->put (o1, q5);
          reg_->put (o2, q4);
          reg_->put (o2, q3);
          reg_->put (o3, q2);
          reg_->put (o2, q1);
          reg_->put (o1, Q13()); // the empty query
          
          ps.clear();
          for (int i=0; i<100; ++i)
            {
              Pra px (fabricate<23>());
              Q23 qx (garbage_query());
              reg_->put (px, qx);
              
              // store for verification....
              px->instanceID = QueryKey(qx).getQueryString();
              ps[qx] = px; 
            }
        }
      
      
      void
      check_query ()
        {
          Iter13 i (reg_->candidates(Q13 ("irrelevant query")));
          CHECK ( i.hasNext());
          CHECK ( *i == o1); ++i;   // ordered according to the degree of the queries
          CHECK ( *i == o2); ++i;   // degree == 1
          CHECK ( *i == o3); ++i;   // degree == 2
          CHECK ( *i == o2); ++i;   // ...
          CHECK ( *i == o2); ++i;
          CHECK ( *i == o1);
          CHECK (!i.hasNext());
          CHECK (! *++i ); // null after end
          
          i = reg_->candidates(q2);
          CHECK ( *i == o3); ++i;   // found by direct match
          CHECK ( *i == o1); ++i;   // followed by the ordered enumeration
          CHECK ( *i == o2); ++i;
          CHECK ( *i == o3); ++i;
          CHECK ( *i == o2); ++i;
          CHECK ( *i == o2); ++i;
          CHECK ( *i == o1); ++i;
          CHECK (!i.hasNext());
          
          i = reg_->candidates(Q13());
          CHECK ( *i == o1); ++i;   // found by direct match to the empty query
          CHECK ( *i == o1); ++i;
          CHECK ( *i == o2); ++i;
          CHECK ( *i == o3); ++i;
          CHECK ( *i == o2); ++i;
          CHECK ( *i == o2); ++i;
          CHECK ( *i == o1); ++i;
          CHECK (!i.hasNext());
          
          uint d=0;
          uint d_prev=0;
          Iter23 j = reg_->candidates(Q23 ("some crap"));
          for ( ; *j ; ++j )
            {
              CHECK ( *j );
              Q23 qx ((*j)->instanceID);
              CHECK ( ps[qx] == (*j));
              d = QueryKey(qx).degree();
              CHECK ( d_prev <= d );
              d_prev = d;
            }
          CHECK (!j.hasNext());
          
          // calling with an arbitrary (registered) query
          // yields the corresponding object at start of the enumeration
          Q23 someQuery(ps.begin()->first);
          j = reg_->candidates(someQuery);
          CHECK ( *j == ps.begin()->second);
          
        }
      
      
      void
      check_remove ()
        {
          reg_->forget (o2);
          
          Iter13 i (reg_->candidates(q4));
          CHECK ( i.hasNext());
          CHECK ( *i == o1); ++i;    // ordered according to the degree of the queries
                                     // but the o2 entries are missing
          CHECK ( *i == o3); ++i;
                                     // o2 missing
                                     // o2 missing
          CHECK ( *i == o1);
          CHECK (!i.hasNext());
          
          o3.reset(); // killing the only reference....
                     //  expires the weak ref in the registry
          
          i = reg_->candidates(Q13 ("something"));
          CHECK ( i.hasNext());
          CHECK ( *i == o1); ++i;    // ordered according to the degree of the queries
                                     // but now also the o3 entry is missing...
          CHECK ( *i == o1);
          CHECK (!i.hasNext());
          
          CHECK ( reg_->put (o1, q5));   // trying to register the same object at the same place
                                         // doesn't change anything (but counts as "success")
          i = reg_->candidates(q5);
          CHECK ( *i == o1); ++i; // direct match
          CHECK ( *i == o1); ++i;
          CHECK ( *i == o1); ++i;
          CHECK (!i.hasNext());
          
          CHECK (!reg_->put (o2, q5));   // trying to (re)register o2 with a existing query
                                         // counts as failure (nothing changes)
          i = reg_->candidates(q5);
          CHECK ( *i == o1); ++i; // direct match
          CHECK ( *i == o1); ++i;
          CHECK ( *i == o1); ++i;
          CHECK (!i.hasNext());
          
          CHECK ( reg_->put (o2, q2));   // trying to (re)register o2 with another query succeeds
          i = reg_->candidates(q2);
          CHECK ( *i == o2); ++i; // direct match
          CHECK ( *i == o1); ++i;
          CHECK ( *i == o2); ++i; // inserted here in the dataset, since q2 has degree 2
          CHECK ( *i == o1); ++i;
          CHECK (!i.hasNext());
          
          CHECK ( reg_->forget (o1));
          CHECK (!reg_->forget (o1)); // failure, because it's already removed
          CHECK ( reg_->forget (o2));
          
          o3 = fabricate<13>();    // another object is another object (it's irrelevant...)
          
          i = reg_->candidates(q2);
          CHECK (! (*i));  // empty
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (DefsRegistryImpl_test, "function session");
  
  
  
}}} // namespace lumiera::query::test
