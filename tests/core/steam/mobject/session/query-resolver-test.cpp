/*
  QueryResolver(Test)  -  issuing typed queries over a generic interface

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file query-resolver-test.cpp
 ** unit test \ref QueryResolver_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/depend.hpp"

#include "common/query/query-resolver.hpp"

#include <string>



namespace lumiera {
namespace test{
  
  using lib::test::showSizeof;
  using std::string;
  
  
  
  
  namespace { // providing a test query resolving facility...
    
    typedef Goal::QueryID const& QID;
    
    /** an sequence of "solutions" to be "found" */
    template<typename TY>
    class DummySolutions;
    
    template<>
    class DummySolutions<int>
      {
        int resNr_;
        
      public:
        DummySolutions() : resNr_(7) {}
        
        int* next () { --resNr_; return &resNr_; }
        bool exhausted() { return !bool(resNr_); }
      };
    
    template<>
    class DummySolutions<string>
      : public DummySolutions<int> 
      {
        string currentText_;
        
      public:
        string*
        next ()
          { 
            static const char* lumi ="Lumiera";
            currentText_ = string (lumi + *DummySolutions<int>::next());
            return &currentText_;
          }
      };
    
    
    /**
     * a concrete "resolution" of the query
     * is a set of "solutions", which can be 
     * explored by iteration. Thus, the result set
     * has to implement the iteration control API
     * as required by IterAdapter
     */
    template<typename TY>
    struct DummyResultSet
      : Resolution
      {
        DummySolutions<TY> solutions_;
        
        typedef typename Query<TY>::Cursor Cursor;
        
        Result
        prepareResolution()
          {
            Cursor cursor;
            cursor.point_at (solutions_.next());
            return cursor;
          }
        
        void
        nextResult(Result& pos)
          {
            Cursor& cursor = static_cast<Cursor&> (pos);
            
            if (solutions_.exhausted())
              cursor.point_at (0);
            else
              cursor.point_at (solutions_.next());
          }
      };
    
    
    
    /**
     * a (dummy) concrete query resolution facility.
     * It is hard-wired to accept queries on int and string,
     * generating a sequence of results for both cases
     */
    class DummyTypedSolutionProducer
      : public QueryResolver
      {
        bool
        canHandleQuery (QID qID)  const
          {
            return Goal::GENERIC == qID.kind 
                && (wantResultType<int> (qID)
                  ||wantResultType<string>(qID));
          }
        
        template<typename TY>
        bool
        wantResultType (QID qID)  const
          {
            return qID.type == getResultTypeID<TY>();
          }
        
        
        template<typename TY>
        static Resolution*
        resolutionFunction (Goal const& goal)
          {
            QID qID = goal.getQID();
            REQUIRE (qID.kind == Goal::GENERIC
                  && qID.type == getResultTypeID<TY>());
            
            return new DummyResultSet<TY>(); 
          }
        
        operator string()  const { return "Test-DummyQueryResolver"; }
        
      public:
        DummyTypedSolutionProducer()
          : QueryResolver()
          {
            Goal::QueryID case1(Goal::GENERIC, getResultTypeID<int>());
            Goal::QueryID case2(Goal::GENERIC, getResultTypeID<string>());
            
            installResolutionCase(case1, &resolutionFunction<int> );
            installResolutionCase(case2, &resolutionFunction<string> );
          }
      };
    
    
    lib::Depend<DummyTypedSolutionProducer> testResolver;
    
    QueryResolver&
    buildTestQueryResolver ()
    {
      return testResolver();
    }
    
  } // (END) implementation of a test query resolving facility
  
  
  
  
  
  
  /*******************************************************************************//**
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
          Query<int> firstQuery("");
          explore (firstQuery.resolveBy (resolver));
          
          Query<string> secondQuery("");
          explore (secondQuery.resolveBy(resolver));
        }
      
      template<typename ITER>
      static void
      explore (ITER ii)
        {
          cout << "Query-Results: " << showSizeof(ii) << endl;
          for ( ; ii; ++ii )
              cout << *ii << endl;
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (QueryResolver_test, "unit session");
  
  
}} // namespace lumiera::test
