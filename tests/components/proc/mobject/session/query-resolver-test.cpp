/*
  QueryResolver(Test)  -  issuing typed queries over a generic interface
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
//#include "lib/lumitime.hpp"
#include "lib/test/test-helper.hpp"
//#include "proc/mobject/placement-ref.hpp"
//#include "proc/mobject/session/test-scopes.hpp"
//#include "proc/mobject/placement-index.hpp"
#include "proc/mobject/session/query-resolver.hpp"
//#include "lib/symbol.hpp"
//#include "lib/access-casted.hpp"
//#include "lib/variant.hpp"
//#include "lib/meta/typelist.hpp"
//#include "lib/util.hpp"

#include <iostream>
#include <string>



namespace mobject {
namespace session {
namespace test    {
  
  using lib::test::showSizeof;
  //using util::isSameObject;
  //using lumiera::Time;
  using std::string;
  using std::cout;
  using std::endl;
  
  namespace { // a test query resolving facility
    
    
    template<typename TY>
    class DummySolution;
    
    class DummySolution<int>
      {
        int resNr_;
        
      public:
        DummySolution() : resNr_(7) {}
        
        int* next () { --resNr_; return &resNr_; }
        bool exhausted() { return bool(resNr_); }
      };
    
    class DummySolution<string>
      : DummySolution<int> 
      {
        string currentText_;
        
      public:
        string*
        next ()
          { 
            static char* lumi ="Lumiera";
            currentText_ = string (lumi[*DummySolution<int>::next()]);
            return &currentText_;
          }
      };
    
    template<typename TY>
    struct DummyResultSet
      : QueryResolver::Resolution
      {
        DummySolution<TY> solution_;

        typedef typename Query<TY>::Cursor Cursor;

        Result
        prepareResolution()
          {
            Cursor cursor;
            cursor.pointAt (solution_.next());
            return cursor;
          }
        
        void
        nextResult(Result& pos)
          {
            Cursor& cursor = static_cast<Cursor&> (pos);
            
            if (solution_.exhausted())
              cursor.point_at (0);
            else
              cursor.point_at (solution_.next());
          }
      };
      
    class TypeMatchFilter
      : public QueryResolver
      {
        bool
        canHandleQuery (Goal::QueryID qID)  const
          {
            return Goal::GENERIC == qID.kind 
                && (wantResultType<int> (qID)
                  ||wantResultType<string>(qID));
          }
        
        template<typename TY>
        bool
        wantResultType (Goal::QueryID qID)
          {
            return qID.type == ResultType::ID<TY>::get();
          }
        
      };
  
    QueryResolver&
    buildTestQueryResolver ()
    {
      UNIMPLEMENTED("build a special resolver just for this test and register it.");
    }
  }
  
  
  /***********************************************************************************
   * @test verify the mechanism for issuing typed queries through a generic interface,
   *       without disclosing the facility actually answering those queries. 
   *       Results are to be retrieved through a Lumiera Forward Iterator.
   *       
   * @see  mobject::session::QueryResolver
   * @see  mobject::session::ScopeLocate usage example
   * @see  mobject::session::ContentsQuery typed query example
   * @see  contents-query-test.cpp
   */
  class QueryResolver_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          QueryResolver& resolver = buildTestQueryResolver();
          Query<int> firstQuery;
          explore (firstQuery (resolver));
          
          Query<string> secondQuery;
          explore (secondQuery(resolver));
        }
      
      template<typename ELM>
      static void
      explore (typename Query<ELM>::iterator ii)
        {
          cout << "Query-Results: " << showSizeof(ii) << endl;;
          while (ii)
              cout << *ii << endl;
        }
          
    };
  
  
  /** Register this test class... */
  LAUNCHER (QueryResolver_test, "unit session");
  
  
}}} // namespace mobject::session::test
