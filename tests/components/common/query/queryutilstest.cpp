/*
  QueryUtils(Test)  -  checking various utils provided for dealing with config queries
 
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

#include "common/cmdline.hpp"
#include "common/query.hpp"
#include "common/query/querydiagnostics.hpp"

#include <boost/bind.hpp>
#include <iostream>
#include <cstdlib>

using lumiera::Query;
using util::Cmdline;
using util::isnil;
using util::contains;
using util::for_each;

using boost::bind;
using std::string;
using std::cout;



namespace lumiera
  {
  namespace query
    {
    namespace test
      {
      
      struct Thing
        {
          virtual ~Thing() {} // add RTTI for Query.asKey();
        };
      
      
      
      /************************************************************************
       * @test check the various small helpers and utilities we utilize
       *       for dealing with ConfigQuery 
       */
      class QueryUtils_test : public Test
        {
          
          virtual void
          run (Arg arg) 
            {
              if (isnil(arg))  arg = Cmdline ("Query normalizeID extractID countPraed");
              
              if (contains (arg, "Query"      ))   check_Query ();
              if (contains (arg, "normalizeID"))   check_normalizeID ();
              if (contains (arg, "extractID"  ))   check_extractID ();
              if (contains (arg, "countPraed" ))   check_countPraed ();
            }
          
          
          
          /** @test Query wrapper class basics */
          void
          check_Query ()
            {
              cout << Query<Thing> ("I am writing a test sentence.").asKey() << "\n";
            }
          
          
          
          /** @test sanitizing and normalizing various tokens */
          void
          check_normalizeID ()
            {
              Cmdline tokens ("a A AA dufte 1a _1 A_A BÄH");
              tokens.push_back ("");
              tokens.push_back ("  White  space ");
              tokens.push_back ("§&Ω%€GΩ%€ar  Ω  baäääääge!!!!! ");
              
              cout << "..original : " << tokens << " :\n";
              
              for_each (tokens, bind ( &normalizeID, _1 ));
              
              cout << "normalized : " << tokens << " :\n";
            }
          
          
          
          /** @test the simple regexp extracting a parameter token */
          void
          check_extractID ()
            {
              ASSERT ("tok" == extractID ("pred", "pred(tok)." ));
              ASSERT ("tok" == extractID ("pred", "    pred( tok )" ));
              ASSERT ("tok" == extractID ("pred", "pred(tok), pred(tux)." ));
              ASSERT ("tok" == extractID ("pred", "other(xyz) pred(tok) pred(tux)" ));
              ASSERT ("tok" == extractID ("pred", "some( pred(tok)" ));

              ASSERT (isnil (extractID ("pred", "pred (tok)")));
              ASSERT (isnil (extractID ("pred", "pred tok)" )));
              ASSERT (isnil (extractID ("pred", "pred(tok " )));
            }
          
          
          
          /** @test counting of predicates in a quiery
           *  (currently 4/08 regexp based...)
           */
          void
          check_countPraed ()
            {
              for (uint i=1; i <= 30; ++i)
                  ASSERT ( i == countPraed (garbage_query (i)));
            }
        };
      
      
      /** Register this test class... */
      LAUNCHER (QueryUtils_test, "unit query");
      
      
      
    } // namespace test
    
  } // namespace query

} // namespace lumiera
