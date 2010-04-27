/*
  AdviceIndex(Test)  -  cover the index datastructure used to implement Advice dispatch
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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
//#include "lib/test/test-helper.hpp"

#include "lib/advice/index.hpp"
//#include "lib/p.hpp"
//#include "proc/assetmanager.hpp"
//#include "proc/asset/inventory.hpp"
//#include "proc/mobject/session/clip.hpp"
//#include "proc/mobject/session/track.hpp"
//#include "lib/meta/trait-special.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/symbol.hpp"

//#include <iostream>
#include <vector>
//#include <string>

//using lib::test::showSizeof;
//using lib::test::randStr;
//using util::isSameObject;
//using util::and_all;
//using util::for_each;
//using util::isnil;
using lib::Literal;
//using lib::Symbol;
//using lumiera::P;
//using std::string;
//using std::cout;
//using std::endl;



namespace lib {
namespace advice {
namespace test {
  
  using lib::Literal;
  
  namespace { // test support definitions
    
    struct TestPOA
      {
        TestPOA* solution_;
        Binding::Matcher pattern_;
        
        explicit
        TestPOA(Literal spec="missing")
          : solution_(0)
          , pattern_(Binding(spec).buildMatcher())
          { }
        
        bool
        matches (Literal refSpec)  const
          {
            return pattern_.matches (Binding(refSpec));
          }
        
        
        /* == Adapter interface for use within the Index == */
        
        friend HashVal
        hash_value (TestPOA const& entry)
        {
          return hash_value (entry.pattern_);
        }
        
        friend const Binding::Matcher
        getMatcher (TestPOA const& entry)
        {
          return entry.pattern_;
        }
        
        friend TestPOA*
        getSolution (TestPOA& entry)
        {
          return entry.solution_;
        }
        
        friend void
        setSolution (TestPOA* entry, TestPOA* solution =0)
        {
          REQUIRE (entry);
          entry->solution_ = solution;
        }
      };
    
    
    
    
    const uint MAX_TEST_ENTRIES = 10;
    
    std::vector<TestPOA> testEntries(MAX_TEST_ENTRIES);
    
    
    
    /** convenience shortcut for writing testcases inline */
    inline TestPOA&
    _entry (uint id, Literal spec)
    {
      REQUIRE (id < testEntries.size());
      
      if (!testEntries[id].matches(spec))
        testEntries[id] = TestPOA(spec);
      
      return testEntries[id];
    }
    
    /** check if the given request got the denoted solution */
    inline bool
    _hasSolution (uint req, uint prov)
    {
      REQUIRE (req < testEntries.size());
      REQUIRE (prov < testEntries.size());
      
      return testEntries[req].solution_ == & testEntries[prov]; 
    }
    
    /** check if the given request holds a default solution */
    inline bool
    _hasDefault (uint req)
    {
      REQUIRE (req < testEntries.size());
      return NULL == testEntries[req].solution_;
    }
  }
  
  typedef Index<TestPOA> Index;
  
  
  
  /******************************************************************************************
   * @test the Advice system uses an index datastructure to support matching the bindings
   *       to get pairs of participants to connect by an individual advice channel.
   *       
   *       This test covers the properties of this implementation datastructure in isolation.
   *       We employ special test entries, different from what is used in the advice system,
   *       and we create a specific instantiation of the advice::Index template solely
   *       for this test
   * 
   * @see advice.hpp
   * @see AdviceBasics_test
   * @see AdviceBindingPattern_test
   */
  class AdviceIndex_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          Index idx;
          buildIndex (idx);
          
          addRequest (idx);
          addProvision (idx);
          removeRequest (idx);
          retractProvision (idx);
          modifyRequest (idx);
          modifyProvision (idx);
          
          clearIndex (idx);
        }
      
      
      void
      buildIndex (Index& idx)
        {
          CHECK (idx.isValid());
          CHECK (0 == idx.size());
          
          idx.addRequest (_entry (1,"cat"));
          idx.addRequest (_entry (2,"cat"));
          idx.addRequest (_entry (3,"dog"));
          CHECK (3 == idx.size());
          CHECK (3 == idx.request_count());
          CHECK (0 == idx.provision_count());
          
          idx.addProvision (_entry (4,"dog"));
          CHECK (4 == idx.size());
          CHECK (3 == idx.request_count());
          CHECK (1 == idx.provision_count());
          
          CHECK (_hasDefault (1));
          CHECK (_hasDefault (2));
          CHECK (_hasSolution (3,4));
          CHECK (idx.isValid());
        }
      
      
      void
      addRequest (Index& idx)
        {
          CHECK (idx.isValid());
          uint req_cnt = idx.request_count();
          
          idx.addRequest (_entry (5,"dog"));
          idx.addRequest (_entry (6,"cat"));
          
          CHECK (idx.hasRequest (_entry (5,"dog")));
          CHECK (idx.hasRequest (_entry (6,"cat")));
          
          CHECK (_hasDefault (6));
          CHECK (_hasSolution (5,4));
          CHECK (idx.isValid());
          CHECK (2 + req_cnt == idx.request_count());
        }
      
      
      void
      addProvision (Index& idx)
        {
          CHECK (idx.isValid());
          uint r_cnt = idx.request_count();
          uint p_cnt = idx.provision_count();
          
          CHECK (_hasDefault (1));
          CHECK (_hasDefault (2));
          CHECK (_hasDefault (6));
          CHECK (_hasSolution (3,4));
          CHECK (_hasSolution (5,4));
          
          idx.addProvision (_entry (7,"cat"));
          CHECK (idx.hasProvision (_entry (7,"cat")));
          
          CHECK (_hasSolution (1,7));                   // all cats got the cat solution
          CHECK (_hasSolution (2,7));
          CHECK (_hasSolution (6,7));
          CHECK (_hasSolution (3,4));                   // dogs unaltered
          CHECK (_hasSolution (5,4));
          CHECK (idx.isValid());
          CHECK (1 + p_cnt == idx.provision_count());
          CHECK (0 + r_cnt == idx.request_count());
          
          idx.addProvision (_entry (8,"dog"));
          
          CHECK (_hasSolution (1,7));                   // cats remain unaffected
          CHECK (_hasSolution (2,7));
          CHECK (_hasSolution (6,7));
          CHECK (_hasSolution (3,8));                   // all dogs got the new solution
          CHECK (_hasSolution (5,8));
          CHECK (idx.isValid());
          CHECK (2 + p_cnt == idx.provision_count());
          CHECK (0 + r_cnt == idx.request_count());
        }
      
      
      void
      removeRequest (Index& idx)
        {
          CHECK (idx.isValid());
          uint r_cnt = idx.request_count();
          uint p_cnt = idx.provision_count();
          CHECK (_hasSolution (1,7));
          CHECK (_hasSolution (2,7));
          CHECK (_hasSolution (6,7));
          CHECK (_hasSolution (3,8));
          CHECK (_hasSolution (5,8));
          
          CHECK ( idx.hasRequest (_entry (2,"cat")));
          
          idx.removeRequest (_entry (2,"cat"));
          
          CHECK (!idx.hasRequest (_entry (2,"cat")));
          CHECK (p_cnt   == idx.provision_count());
          CHECK (r_cnt-1 == idx.request_count());
          
          CHECK (_hasSolution (1,7));                  // no effect on the other requests
          CHECK (_hasSolution (6,7));
          CHECK (_hasSolution (3,8));
          CHECK (_hasSolution (5,8));
          
          idx.removeRequest (_entry (2,"cat"));        // idempotent
          CHECK (!idx.hasRequest (_entry (2,"cat")));
          CHECK (p_cnt   == idx.provision_count());
          CHECK (r_cnt-1 == idx.request_count());
          CHECK (idx.isValid());
        }
      
      
      void
      retractProvision (Index& idx)
        {
          CHECK (idx.isValid());
          uint r_cnt = idx.request_count();
          uint p_cnt = idx.provision_count();
          CHECK (_hasSolution (1,7));
          CHECK (_hasSolution (6,7));
          CHECK (_hasSolution (3,8));
          CHECK (_hasSolution (5,8));
          
          CHECK ( idx.hasProvision (_entry (4,"dog")));
          
          idx.removeProvision (_entry (4,"dog"));
          
          CHECK (!idx.hasProvision (_entry (4,"dog")));
          CHECK (p_cnt-1 == idx.provision_count());
          CHECK (r_cnt   == idx.request_count());
          
          CHECK (_hasSolution (1,7));                  // no effect on the solutions, because of the more recent dog solution 8
          CHECK (_hasSolution (6,7));
          CHECK (_hasSolution (3,8));
          CHECK (_hasSolution (5,8));
          
          CHECK ( idx.hasProvision (_entry (8,"dog")));
          
          idx.removeProvision (_entry (8,"dog"));
          
          CHECK (!idx.hasProvision (_entry (8,"dog")));
          CHECK (p_cnt-2 == idx.provision_count());
          CHECK (r_cnt   == idx.request_count());
          
          CHECK (_hasSolution (1,7));                  // no effect on the cat solutions
          CHECK (_hasSolution (6,7));
          CHECK (!_hasSolution (3,8));
          CHECK (!_hasSolution (5,8));
          CHECK (_hasDefault  (3));                    // but all dog requests reverted to default
          CHECK (_hasDefault  (5));
          
          idx.removeProvision (_entry (8,"dog"));      // idempotent
          CHECK (!idx.hasProvision (_entry (8,"dog")));
          CHECK (p_cnt-2 == idx.provision_count());
          CHECK (r_cnt   == idx.request_count());
          CHECK (idx.isValid());
        }
      
      
      void
      modifyRequest (Index& idx)
        {
          CHECK (idx.isValid());
          uint r_cnt = idx.request_count();
          uint p_cnt = idx.provision_count();
          CHECK (_hasSolution (1,7));
          CHECK (_hasSolution (6,7));
          CHECK (_hasDefault  (3));
          CHECK (_hasDefault  (5));
          
          CHECK (!idx.hasRequest (_entry (2,"cat")));
          CHECK ( idx.hasRequest (_entry (5,"dog")));
          
          idx.modifyRequest (_entry (5,"dog"), _entry (2,"cat"));
          
          CHECK ( idx.hasRequest (_entry (2,"cat")));
          CHECK (!idx.hasRequest (_entry (5,"dog")));
          CHECK (p_cnt == idx.provision_count());
          CHECK (r_cnt == idx.request_count());
          CHECK (_hasSolution (1,7));
          CHECK (_hasSolution (6,7));
          CHECK (_hasDefault  (3));
          CHECK (_hasSolution (2,7));                  // automatically got the current cat solution
        }
      
      
      void
      modifyProvision (Index& idx)
        {
          CHECK (idx.isValid());
          uint r_cnt = idx.request_count();
          uint p_cnt = idx.provision_count();
          CHECK (_hasSolution (1,7));
          CHECK (_hasSolution (2,7));
          CHECK (_hasSolution (6,7));
          CHECK (_hasDefault  (3));
          
          CHECK ( idx.hasProvision (_entry (7,"cat")));
          CHECK (!idx.hasProvision (_entry (8,"dog")));
          
          idx.modifyProvision (_entry (7,"cat"), _entry (8,"dog"));
          CHECK (!idx.hasProvision (_entry (7,"cat")));
          CHECK ( idx.hasProvision (_entry (8,"dog")));
          CHECK (p_cnt == idx.provision_count());
          CHECK (r_cnt == idx.request_count());
          CHECK (_hasDefault  (1));
          CHECK (_hasDefault  (2));
          CHECK (_hasDefault  (3));
          CHECK (_hasSolution (3,8));
          
          idx.addProvision (_entry (7,"cat"));
          idx.addProvision (_entry (9,"cat"));
          CHECK (idx.hasProvision (_entry (7,"cat")));
          CHECK (idx.hasProvision (_entry (9,"cat")));
          CHECK (_hasSolution (1,9));                  // all cats got the second cat solution
          CHECK (_hasSolution (2,9));
          CHECK (_hasSolution (6,9));
          CHECK (_hasSolution (3,8));                  // the dog is unaffected

          CHECK ( idx.hasProvision (_entry (7,"cat")));
          CHECK (!idx.hasProvision (_entry (4,"dog")));
          
          idx.modifyProvision (_entry (7,"cat"), _entry (4,"dog"));
          
          CHECK (!idx.hasProvision (_entry (7,"cat")));
          CHECK ( idx.hasProvision (_entry (4,"dog")));
          CHECK (p_cnt == idx.provision_count());
          CHECK (r_cnt == idx.request_count());
          CHECK (_hasSolution (1,9));                  // cats unaffected, because we're changing a shadowed cat solution
          CHECK (_hasSolution (2,9));
          CHECK (_hasSolution (6,9));
          CHECK (_hasSolution (3,4));                  // but the dog got switched to the transmogrified-into-dog solution,
                                                       // because it was added later than the existing solution 8
          CHECK (p_cnt == idx.provision_count());
          CHECK (r_cnt == idx.request_count());
          CHECK (idx.isValid());
        }
      
      
      void
      clearIndex (Index& idx)
        {
          idx.clear();
          CHECK (idx.isValid());
          CHECK (0 == idx.size());
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceIndex_test, "function common");
  
  
}}} // namespace lib::advice::test
