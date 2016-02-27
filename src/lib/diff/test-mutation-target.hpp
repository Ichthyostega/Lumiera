/*
  TEST-MUTATION-TARGET.hpp  -  diagnostic helper for TreeMutator bindings

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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

*/


/** @file test-mutation-target.hpp
 ** Diagnostic helper for unit tests regarding mutation of custom data.
 ** The TreeMutator provides a specialised adapter to attach to a TestMutationTarget.
 ** This adapter is optional and can be combined with any other binding for arbitrary
 ** hierarchical data structures. It operates in the way of a "wire tap", where the
 ** observed "mutation primitives" are recorded within the TestMutationTarget,
 ** which offers query functions for the unit test to verify what happened.
 ** 
 ** @remarks this facility was created during the attempt to shape the internal API
 **          of TreeMutator, including definition of the "mutation primitives";
 **          it might be helpful later to diagnose problems with data mutation. 
 ** 
 ** @todo WIP 2/2016
 ** 
 ** @see TreeManipulationBinding_test
 ** @see TreeMutator
 ** 
 */


#ifndef LIB_DIFF_TEST_MUTATION_TARGET_H
#define LIB_DIFF_TEST_MUTATION_TARGET_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/diff/record.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/test/event-log.hpp"
//#include "lib/util.hpp"
//#include "lib/format-string.hpp"

//#include <functional>
#include <string>
//#include <vector>
//#include <map>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
//using util::_Fmt;
  using lib::test::EventLog;
  using lib::test::EventMatch;
  using lib::Literal;
//  using std::function;
  using std::string;
  
  namespace {
  }
  
  
  /**
   * Test adapter to watch and verify how the
   * TreeMutator binds to custom tree data structures.
   * @todo WIP 2/2016
   */
  class TestMutationTarget
    {
      
      EventLog log_{this->identify()};
      
    public:
      
      /* === Diagnostic / Verification === */
      
      bool
      empty()  const
        {
          UNIMPLEMENTED ("NIL check");
        }
      
      bool
      contains (string spec)  const
        {
          UNIMPLEMENTED ("check for recorded element");
        }
      
      EventMatch
      verify (string match)  const
        {
          return getLog().verify(match);
        }
      
      EventMatch
      verifyMatch (string regExp)  const
        {
          return getLog().verifyMatch(regExp);
        }
      
      EventMatch
      verifyEvent (string match)  const
        {
          return getLog().verifyEvent(match);
        }
      
      EventMatch
      verifyEvent (string classifier, string match)  const
        {
          return getLog().verifyEvent (classifier,match);
        }
      
      EventMatch
      verifyCall (string match)  const
        {
          return getLog().verifyCall(match);
        }
      
      EventMatch
      ensureNot (string match)  const
        {
          return getLog().ensureNot(match);
        }
      
      EventLog const&
      getLog()  const
        {
          return log_;
        }
      
      
    private:
      string
      identify() const
        {
          return lib::idi::instanceTypeID(this);
        }
    };
  
  
  
  namespace { // supply a suitable decorator for the TreeMutator
    
    template<class PAR>
    struct TestWireTap
      : PAR
      {
        TestMutationTarget& target_;
        
        TestWireTap(TestMutationTarget& dummy, PAR const& chain)
          : PAR(chain)
          , target_(dummy)
          { }
      };
    
    template<class PAR>
    Builder<TestWireTap<PAR>>
    Builder<PAR>::attachDummy (TestMutationTarget& dummy)
    {
      return WireTap (dummy, *this);
    }

  }
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_H*/
