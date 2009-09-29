/*
  DefsRegistryImpl(Test)  -  verifying correct behaviour of the defaults registry
 
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


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/mobject/session/defsregistry.hpp"
#include "lib/factory.hpp"
#include "lib/query.hpp"
#include "lib/p.hpp"

#include "../lib/query/querydiagnostics.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/format.hpp>
#include <map>

using lumiera::P;
using lumiera::Query;
using lumiera::query::test::garbage_query;
using util::isnil;

using boost::scoped_ptr;
using boost::format;
using std::string;
using std::rand;
using std::map;



namespace mobject {
  namespace session {
    namespace test  {
      
      format typePatt ("Dummy<%2i>");
      format instancePatt ("obj_%s_%i");
      format predicatePatt ("%s_%2i( %s )");
      
      
      
      /** create a random new ID */
      string 
      newID (string prefix)
      {
        return str (instancePatt % prefix % rand());
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
      string Dummy<I>::name = str (typePatt % I);
      
      
      
      
      
      
      /************************************************************************
       * @test build an registry table (just for this test) configured for
       *       some artificial test Types. Register some entries and verify
       *       the intended behaviour of the storage structure.
       * @see  DefsManagerImpl_test for checking the implementation details
       *       in the actual context used in Lumiera.
       */
      class DefsRegistryImpl_test : public Test
        {
          scoped_ptr<DefsRegistry> reg_;
          
          typedef lumiera::P<Dummy<13> > O;
          typedef lumiera::P<Dummy<23> > P;
          
          typedef Query<Dummy<13> > Q13;
          typedef Query<Dummy<23> > Q23;
          
          typedef DefsRegistry::Iter<Dummy<13> > Iter13;
          typedef DefsRegistry::Iter<Dummy<23> > Iter23;
          
          
          // fabricating Objects wrapped into smart-ptrs
          lib::factory::RefcountFac<Dummy<13> > oFac;
          lib::factory::RefcountFac<Dummy<23> > pFac;
          
          O o1, o2, o3;
          Q13 q1, q2, q3, q4, q5;
          map<Q23, P> ps;
          
        public:
          DefsRegistryImpl_test ()
            : o1 (oFac()), o2 (oFac()), o3 (oFac()), 
              q1 (garbage_query (1)),
              q2 (garbage_query (2)),
              q3 (garbage_query (3)),
              q4 (garbage_query (4)),
              q5 (garbage_query (5))
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
              ASSERT ( ! *(reg_->candidates(Q13 ("something"))) );
              
              reg_->put (o1, q5);
              reg_->put (o2, q4);
              reg_->put (o3, q3);
              reg_->put (o3, q2);
              reg_->put (o2, q1);
              reg_->put (o1, Q13()); // the empty query
             
              ps.clear();
              for (int i=0; i<100; ++i)
                {
                  P px (pFac());
                  Q23 qx (garbage_query());
                  ps[qx] = px;
                  reg_->put (px, qx);
                  px->instanceID = qx;
                }
            }
          
          
          void
          check_query ()
            {
              Iter13 i (reg_->candidates(Q13 ("irrelevant query")));
              ASSERT ( i.hasNext());
              ASSERT ( *i++ == o1);  // ordered according to the degree of the queries
              ASSERT ( *i++ == o2);
              ASSERT ( *i++ == o3);
              ASSERT ( *i++ == o3);
              ASSERT ( *i++ == o2);
              ASSERT ( *i   == o1);
              ASSERT (!i.hasNext());
              ASSERT (! *++i ); // null after end
              
              i = reg_->candidates(q3);
              ASSERT ( *i++ == o3);  // found by direct match
              ASSERT ( *i++ == o1);  // followed by the ordered ennumeration
              ASSERT ( *i++ == o2);
              ASSERT ( *i++ == o3);
              ASSERT ( *i++ == o3);
              ASSERT ( *i++ == o2);
              ASSERT ( *i++ == o1);
              ASSERT (!i.hasNext());
              
              i = reg_->candidates(Q13());
              ASSERT ( *i++ == o1);  // found by direct match to the empty query
              ASSERT ( *i++ == o1);
              ASSERT ( *i++ == o2);
              ASSERT ( *i++ == o3);
              ASSERT ( *i++ == o3);
              ASSERT ( *i++ == o2);
              ASSERT ( *i++ == o1);
              ASSERT (!i.hasNext());
              
              uint d=0;
              uint d_prev=0;
              Iter23 j = reg_->candidates(Q23 ("some crap"));
              for ( ; *j ; ++j )
                {
                  ASSERT ( *j );
                  Q23 qx ((*j)->instanceID);
                  ASSERT ( ps[qx] == (*j));
                  d = lumiera::query::countPraed (qx);
                  ASSERT ( d_prev <= d );
                  d_prev = d;
                }
              ASSERT (!j.hasNext());
              
              // calling with an arbitrary (registered) query
              // yields the corresponding object at start of the enumeration
              j = reg_->candidates(ps.begin()->first);
              ASSERT ( *j == ps.begin()->second);
              
            }
          
          
          void
          check_remove ()
            {
              reg_->forget (o2);
              
              Iter13 i (reg_->candidates(q4));
              ASSERT ( i.hasNext());
              ASSERT ( *i++ == o1);  // ordered according to the degree of the queries
                                     // but the o2 entries are missing
              ASSERT ( *i++ == o3);
              ASSERT ( *i++ == o3);
                                     // missing
              ASSERT ( *i   == o1);
              ASSERT (!i.hasNext());
              
              o3.reset(); // killing the only reference....
                         //  expires the weak ref in the registry

              i = reg_->candidates(Q13 ("something"));
              ASSERT ( i.hasNext());
              ASSERT ( *i++ == o1);  // ordered according to the degree of the queries
                                     // but now also the o3 entries are missing...
              ASSERT ( *i   == o1);
              ASSERT (!i.hasNext());
              
              ASSERT ( reg_->put (o1, q5));  // trying to register the same object at the same place
                                             // doesn't change anything (but counts as "success")
              i = reg_->candidates(q5);
              ASSERT ( *i++ == o1); // direct match
              ASSERT ( *i++ == o1);
              ASSERT ( *i++ == o1);
              ASSERT (!i.hasNext());
              
              ASSERT (!reg_->put (o2, q5));  // trying to (re)register o2 with a existing query
                                             // counts as failure (nothing changes)
              i = reg_->candidates(q5);
              ASSERT ( *i++ == o1); // direct match
              ASSERT ( *i++ == o1);
              ASSERT ( *i++ == o1);
              ASSERT (!i.hasNext());
              
              ASSERT ( reg_->put (o2, q2));  // trying to (re)register o2 with another query succeeds
              i = reg_->candidates(q2);
              ASSERT ( *i++ == o2); // direct match
              ASSERT ( *i++ == o1);
              ASSERT ( *i++ == o2); // inserted here in the dataset
              ASSERT ( *i++ == o1);
              ASSERT (!i.hasNext());
              
              ASSERT ( reg_->forget (o1));
              ASSERT (!reg_->forget (o1)); // failure, because it's already removed
              ASSERT ( reg_->forget (o2)); 

              o3 = oFac();        // another object is another object (it's irrelevant...) 
              
              i = reg_->candidates(q2);
              ASSERT (! (*i)); // empty
            }
          
        };
      
      
      /** Register this test class... */
      LAUNCHER (DefsRegistryImpl_test, "function session");
      
      
      
    } // namespace test
    
  } // namespace mobject

} // namespace mobject
