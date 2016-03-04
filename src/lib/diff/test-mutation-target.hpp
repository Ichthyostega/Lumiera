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
#include "lib/iter-adapter-stl.hpp"
#include "lib/diff/record.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/format-obj.hpp"
#include "lib/test/event-log.hpp"
#include "lib/util.hpp"
//#include "lib/format-string.hpp"

#include <boost/noncopyable.hpp>
//#include <functional>
#include <string>
#include <vector>
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
  using util::unConst;
  using util::isnil;
  
  using VecS = std::vector<string>;
  
  namespace {
    template<typename T>
    inline string
    identify (const T* const entity)
      {
        return lib::idi::instanceTypeID(entity);
      }
    
    
    string render (DataCap const&);
    
    
    inline string
    renderAttribute (GenNode const& elm)
    {
      return elm.idi.getSym()
           + " = "
           + render (elm.data);
    }
    
    inline string
    renderChild (GenNode const& elm)
    {
      return render (elm.data);
    }
    
    
    inline string
    renderRecord (Rec const& record)
      {
        using util::join;
        using lib::transformIterator;
        
        return "Rec("
             + (Rec::TYPE_NIL==record.getType()? "" : record.getType())
             + (isnil(record.attribs())? "" : "| "+join (transformIterator (record.attribs(), renderAttribute))+" ")
             + (isnil(record.scope())?   "" : "|{"+join (transformIterator (record.scope(), renderChild))+"}")
             + ")"
             ;
      }
    
    
    inline string
    render (DataCap const& content)
      {
        class StringRenderer
          : public string
          , public Variant<DataValues>::Visitor
          {
            void
            renderAs (string const& representation)
              {
                string::operator= (representation);
              }
            
#define STRINGIFY_CONTENT(_TY_) \
            virtual void handle  (_TY_& val) override { renderAs (util::toString (val)); }
            
            STRINGIFY_CONTENT (int)
            STRINGIFY_CONTENT (int64_t)
            STRINGIFY_CONTENT (short)
            STRINGIFY_CONTENT (char)
            STRINGIFY_CONTENT (bool)
            STRINGIFY_CONTENT (double)
            STRINGIFY_CONTENT (string)
            STRINGIFY_CONTENT (time::Time)
            STRINGIFY_CONTENT (time::Offset)
            STRINGIFY_CONTENT (time::Duration)
            STRINGIFY_CONTENT (time::TimeSpan)
            STRINGIFY_CONTENT (hash::LuidH)
            
            /** recursive simplified content-only rendering
             *  of an embedded Record<GenNode> */
            virtual void
            handle  (Rec & rec) override
              {
                renderAs (renderRecord (rec));
              }
            
            virtual void
            handle  (RecRef & ref) override
              {
                if (ref)
                  renderAs (renderRecord (ref));
                else
                  renderAs (util::BOTTOM_INDICATOR);
              }
          };
        
        StringRenderer visitor;
        unConst(content).accept (visitor);
        return string(visitor);
      }

  }
  
  
  /**
   * Test adapter to watch and verify how the
   * TreeMutator binds to custom tree data structures.
   * @todo WIP 2/2016
   */
  class TestMutationTarget
    : boost::noncopyable
    {
      
      EventLog log_{identify (this)};
      
      VecS      content_{};
      VecS prev_content_{};
      
    public:
      
      /* === Operation / Mutation API === */
      
      void
      initMutation (string mutatorID)
        {
          swap (content_, prev_content_);
          log_.event ("attachMutator "+mutatorID);
        }
      
      void
      injectNew (string elm)
        {
          content_.push_back (elm);
          log_.event ("injectNew", elm);
        }
      
      
      /* === Diagnostic / Verification === */
      
      bool
      empty()  const
        {
          return content_.empty();
        }
      
      bool
      emptySrc()  const
        {
          return prev_content_.empty();
        }
      
      /** check for recorded element */
      bool
      contains (string spec)  const
        {
          VecS const& currentValidContent{content_};
          return util::contains (currentValidContent, spec);
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
      
      
      using iterator       = typename iter_stl::_SeqT<VecS>::Range;
      using const_iterator = typename iter_stl::_SeqT<const VecS>::Range;
      
      const_iterator begin() const { return iter_stl::eachElm(content_); }
      const_iterator end()   const { return const_iterator(); }
      
      iterator srcIter()           {return iter_stl::eachElm(prev_content_); }
      
      
      friend const_iterator begin (TestMutationTarget const& target) { return target.begin(); }
      friend const_iterator end   (TestMutationTarget const& target) { return target.end(); }
      
      
      
    private:
    };
  
  
  
  namespace { // supply a suitable decorator for the TreeMutator
    
    template<class PAR>
    struct TestWireTap
      : public PAR
      {
        
        /* ==== re-Implementation of the operation API ==== */
      
        /** skip next src element and advance abstract source position */
        virtual void
        skipSrc ()
          {
            UNIMPLEMENTED();
          }
        
        /** record in the test taget
         *  that a new child element is
         *  being insertet at current position */
        virtual void
        injectNew (GenNode const& n)  override
          {
            target_.injectNew (n.isNamed()? renderAttribute(n)
                                          : renderChild(n));
          }
        
        /** ensure the next recorded source element
         *  matches on a formal level with given spec */
        virtual bool
        matchSrc (GenNode const& n)
          {
            if (target_.emptySrc())
              return false;
            string spec{n.isNamed()? renderAttribute(n)
                                   : renderChild(n) };
            return spec == *pos_;
          }
        
        /** accept existing element, when matching the given spec */
        virtual bool
        acceptSrc (GenNode const&)
          {
            UNIMPLEMENTED();
            return false;
          }
        
        /** locate designated element and accept it at current position */
        virtual bool
        findSrc (GenNode const&)
          {
            UNIMPLEMENTED();
            return false;
          }
        
        
        TestWireTap(TestMutationTarget& dummy, PAR const& chain)
          : PAR(chain)
          , target_(dummy)
          , pos_()
          {
            target_.initMutation (identify(this));
            pos_ = target_.srcIter();
          }
        
      private:
        TestMutationTarget& target_;
        TestMutationTarget::iterator pos_;
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
