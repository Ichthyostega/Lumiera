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
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"
#include "lib/test/event-log.hpp"
#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <utility>
#include <string>
#include <vector>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
  using lib::Literal;
  using lib::test::EventLog;
  using lib::test::EventMatch;
  using iter_stl::eachElm;
  using util::unConst;
  using util::isnil;
  using util::join;
  using util::_Fmt;
  
  using std::string;
  using std::forward;
  using std::move;
  
  
  namespace { // diagnostic helpers: render diff spec....
    
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
    renderNode (GenNode const& n)
    {
      return n.isNamed()? renderAttribute(n)
                        : renderChild(n);
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
    
  }//(End)diagnostic helpers
  
  
  
  
  
  /**
   * Test adapter to watch and verify how the
   * TreeMutator binds to custom tree data structures.
   * As a data structure, the TestMutationTarget builds an
   * »External Tree Description« reflecting the actual data structure,
   * as can be inferred through listening to all handled diff mutation primitives.
   * Besides, each of these primitives is recorded in the embedded \ref EventLog.
   * @see TreeManipulationBinding_test::mutateDummy()
   */
  class TestMutationTarget
    : boost::noncopyable
    {
      using VecG = std::vector<GenNode>;
      
      EventLog log_{identify (this)};
      
      VecG      content_{};
      VecG prev_content_{};
      
      
    public:
      using iterator       = typename iter_stl::_SeqT<VecG>::Range;
      using const_iterator = typename iter_stl::_SeqT<const VecG>::Range;
      
      const_iterator begin() const { return eachElm(content_); }
      const_iterator end()   const { return const_iterator(); }
      
      iterator srcIter()           { return eachElm(prev_content_); }
      iterator lastElm()           { return iterator(VecG::iterator(&content_.back()), content_.end());}
      
      
      friend const_iterator begin (TestMutationTarget const& target) { return target.begin(); }
      friend const_iterator end   (TestMutationTarget const& target) { return target.end(); }
      
      
      
      /* === Operation / Mutation API === */
      
      void
      initMutation (string mutatorID)
        {
          prev_content_.clear();
          swap (content_, prev_content_);
          log_.event ("attachMutator "+mutatorID);
        }
      
      void
      inject (GenNode&& elm, string operationID)
        {
          content_.emplace_back (forward<GenNode>(elm));
          log_.event (operationID, renderNode (content_.back()));
        }
      
      static iterator
      search (GenNode::ID const& targetID, iterator pos)
        {
          while (pos and not pos->matches(targetID))
            ++pos;
          return pos;
        }
      
      iterator
      locate (GenNode::ID const& targetID)
        {
          if (!empty() and content_.back().matches(targetID))
            return lastElm();
          else
            return search (targetID, eachElm(content_));
        }
      
      void
      logSkip (GenNode const& content)
        {
          log_.event ("skipSrc", isnil(content.idi.getSym())? util::BOTTOM_INDICATOR : renderNode(content));
        }
      
      void
      logAssignment (GenNode const& target, string oldPayload)
        {
          log_.event ("assignElm", _Fmt{"%s: %s ⤅ %s"}
                                       % target.idi.getSym()
                                       % oldPayload
                                       % render(target.data));
        }
      
      void
      logMutation (GenNode const& target)
        {
          log_.event ("mutateChild", _Fmt{"%s: start mutation...%s"}
                                       % target.idi.getSym()
                                       % render(target.data));
        }
      
      
      /* === Diagnostic / Verification === */
      
      bool
      empty()  const
        {
          return content_.empty();
        }
      
      /** render payload content for diagnostics */
      string
      showContent ()  const
        {
          return join (transformIterator (begin(), renderNode));
        }
      
      /** render elements waiting in source buffer to be accepted */
      string
      showSrcBuffer ()  const
        {
          return join (transformIterator (eachElm(prev_content_), renderNode));
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
    };
  
  
  
  namespace { // supply a suitable decorator for the TreeMutator
    
    template<class PAR>
    class TestWireTap
      : public PAR
      {
        using Target = TestMutationTarget;
        using Iter = TestMutationTarget::iterator;

        Target& target_;
        Iter    pos_;

      public:
        TestWireTap(Target& dummy, PAR const& chain)
          : PAR(chain)
          , target_(dummy)
          , pos_()
          {
            target_.initMutation (identify(this));
            pos_ = target_.srcIter();
          }
        
        
        
        /* ==== re-Implementation of the operation API ==== */
      
        /** skip next recorded src element
         * @remarks TestWireTap adapter together with TestMutationTarget
         *      maintain a "shaddow copy" of the data and apply the detected diff
         *      against this internal copy. This allows to verify what's going on
         */
        virtual void
        skipSrc ()  override
          {
            if (pos_)
              {
                GenNode const& skippedElm = *pos_;
                ++pos_;
                target_.logSkip (skippedElm);
              }
            PAR::skipSrc();
          }
        
        /** record in the test taget
         *  that a new child element is
         *  being insertet at current position
         */
        virtual void
        injectNew (GenNode const& n)  override
          {
            target_.inject (GenNode{n}, "injectNew");
            PAR::injectNew (n);
          }
        
        virtual bool
        emptySrc ()  override
          {
            return !pos_
               and PAR::emptySrc();
          }
        
        /** ensure the next recorded source element
         *  matches on a formal level with given spec */
        virtual bool
        matchSrc (GenNode const& n)  override
          {
            return PAR::matchSrc(n)
                or pos_? n.matches(*pos_)
                       : false;
          }
        
        /** accept existing element, when matching the given spec */
        virtual bool
        acceptSrc (GenNode const& n)  override
          {
            bool isSrcMatch = TestWireTap::matchSrc(n);
            if (isSrcMatch)             // NOTE: important to call our own method here, not the virtual function
              {
                target_.inject (move(*pos_), "acceptSrc");
                ++pos_;
              }
            return PAR::acceptSrc(n)
                or isSrcMatch;
          }
        
        /** locate designated element and accept it at current position */
        virtual bool
        findSrc (GenNode const& ref)  override
          {
            Iter found = TestMutationTarget::search (ref.idi, pos_);
            if (found)
              {
                target_.inject (move(*found), "findSrc");
              }
            return PAR::findSrc(ref)
                or found;
          }
        
        /** repeatedly accept, until after the designated location */
        virtual bool
        accept_until (GenNode const& spec)
          {
            bool foundTarget = true;
            
            if (spec.matches (Ref::END))
              for ( ; pos_; ++pos_)
                target_.inject (move(*pos_), "accept_until END");
            else
              {
                string logMsg{"accept_until "+spec.idi.getSym()};
                while (pos_ and not TestWireTap::matchSrc(spec))
                  {
                    target_.inject (move(*pos_), logMsg);
                    ++pos_;
                  }
                if (TestWireTap::matchSrc(spec))
                  {
                    target_.inject (move(*pos_), logMsg);
                    ++pos_;
                  }
                else
                  foundTarget = false;
              }
            return PAR::accept_until(spec)
                or foundTarget;
          }
        
        /** locate element already accepted into the target sequence
         *  and assign the designated payload value to it. */
        virtual bool
        assignElm (GenNode const& spec)
          {
            Iter targetElm = target_.locate (spec.idi);
            if (targetElm)
              {
                string logOldPayload{render(targetElm->data)};
                *targetElm = spec;
                target_.logAssignment (*targetElm, logOldPayload);
              }
            return PAR::assignElm(spec)
                or targetElm;
          }
        
        /** locate the designated target element and build a suitable
         *  sub-mutator for this element into the provided target buffer */
        virtual bool
        mutateChild (GenNode const& spec, TreeMutator::MutatorBuffer targetBuff)
          {
            if (PAR::mutateChild (spec, targetBuff))
              return true;
            else // Test mode only --
              { //  no other layer was able to provide a mutator
                Iter targetElm = target_.locate (spec.idi);
                if (targetElm)
                  {
                    targetBuff.create (TreeMutator::build());
                    target_.logMutation (*targetElm);
                    return true;
                  }
                return false;
              }
          }
        
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
