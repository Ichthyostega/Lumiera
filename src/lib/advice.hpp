/*
  ADVICE.hpp  -  generic loosely coupled interaction guided by symbolic pattern
 
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
 
*/


/** @file advice.hpp
 ** Expecting Advice and giving Advice: a cross-cutting collaboration of loosely coupled participants.
 ** This header exposes the basics of the advice system and the public access points. The advice system
 ** is a system wide singleton service, but clients never talk directly to this singleton; rather they
 ** use advice::Provision and advice::Request as access point.
 ** 
 ** \par Advice collaboration pattern
 ** Advice collaboration is a special pattern of interaction extracted from multiple use cases within
 ** Lumiera. Creating this abstraction was partially inspired by aspect oriented programming, especially
 ** the idea of cross-cutting the primary dependency hierarchy. Another source of inspiration where the
 ** various incarnations of properties with dynamic binding. For defining the actual binding, we rely
 ** on predicate notation and matching (planned: unification) as known from rule based systems.
 ** 
 ** <b>Definition</b>: Advice is an optional, mediated collaboration between entities taking on
 ** the roles of advisor and advised, thereby passing a custom piece of advice data, managed by
 ** the advice support system. The possibility of advice is created by both of the collaborators
 ** entering the system, where the advised entity exposes a point of advice, while the advising
 ** entity provides an actual advice value.
 ** 
 ** \par Collaborators
 ** - the advised entity
 ** - the advisor
 ** - point of advice
 ** - advice system
 ** - the binding
 ** - the advice
 ** 
 ** Usually, the \em advised entity opens the collaboration by requesting an advice. The \em advice itself
 ** is a piece of data of a custom type, which needs to be copyable. Obviously, both the advised and the
 ** advisor need to share knowledge about the meaning of this advice data. The actual advice collaboration
 ** happens at a \em point-of-advice, which needs to be derived first. To this end, the advised puts up an
 ** \em request by providing his \em binding, which is a pattern for matching. An entity about to give advice
 ** opens possible advice \em channels by putting up an advisor binding, which similarly is a pattern. The
 ** advice \em system as mediator resolves both sides, by matching (which in the most general case could be
 ** an unification). This process creates an advice point \em solution -- allowing the advisor to fed the
 ** piece of advice into the advice channel, causing it to be placed into the point of advice. After passing
 ** a certain (implementation defined) break point, the advice leaves the influence of the advisor and gets
 ** exposed to the advised entities. Especially this involves copying the advice data into a location managed
 ** by the advice system. In the standard case, the advised entity accesses the advice synchronously and
 ** non-blocking. Typically, the advice data type is default constructible and thus there is always a basic
 ** form of advice available, thereby completely decoupling the advised entity from the timings related
 ** to this collaboration.
 ** 
 ** @note as of 6/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces and gain practical usage experiences. Ichthyo expects this collaboration
 **       service to play a central role at various places within proc-layer.
 ** @todo allow variables in binding patterns
 ** @todo use the lumiera MPool instead of heap allocations
 ** @todo consider to provide variations of the basic behaviour by policy classes
 ** @todo the implementation is generic/defensive, and could be improved and optimised
 ** 
 ** @see configrules.hpp
 ** @see typed-lookup.cpp corresponding implementation
 ** @see typed-id-test.cpp
 ** 
 */


#ifndef LIB_ADVICE_H
#define LIB_ADVICE_H


#include "lib/error.hpp"
#include "lib/null-value.hpp"
#include "lib/advice/binding.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/noncopyable.hpp>

using util::isSameObject;


namespace lib    {
namespace advice {
  
  /**
   * TODO type comment
   */
  class PointOfAdvice
    {
      Binding::Matcher pattern_;
      PointOfAdvice* resolution_;
      
    protected:
      /** define or re-define the binding, which
       *  specifically labels this attachment to the advice system.
       *  @note issuing this on an existing connection is equivalent
       *        to re-connecting with the new binding.
       */
      void setBindingPattern (Binding const& binding)
        {
          pattern_ = binding.buildMatcher();
        }
      
    public:
      explicit
      PointOfAdvice (Binding const& binding)
        : pattern_(binding.buildMatcher())
        , resolution_(0)
        { }
      
      // using default copy/assignment
      
      
      
      /* == Adapter interface for use within the Index == */
        
      friend HashVal
      hash_value (PointOfAdvice const& entry)
      {
        return hash_value (entry.pattern_);
      }
      
      friend const Binding::Matcher
      getMatcher (PointOfAdvice const& entry)
      {
        return entry.pattern_;
      }
      
      friend const PointOfAdvice*
      getSolution (PointOfAdvice const& entry)
      {
        return entry.resolution_;
      }
      
      friend void
      setSolution (PointOfAdvice* entry, PointOfAdvice* solution =0)
      {
        REQUIRE (entry);
        entry->resolution_ = solution;
      }
    };
  
  
  
  
  /**
   * Advice Collaboration partner, internally connected to the AdviceSystem.
   * Both advice::Request and advice::Provision are linked in this way
   * to an internal index datastructure, which allows to carry out
   * the actual advice exchange and collaboration.
   */
  class AdviceLink
    : public PointOfAdvice
    {
    protected:
      const PointOfAdvice* publishProvision (PointOfAdvice*);
      const PointOfAdvice* discardSolutions ();
      void publishRequestBindingChange(HashVal);
      
      void registerRequest();
      void deregisterRequest();
      
      void* getBuffer(size_t);
      void  releaseBuffer (const void*, size_t);
      
    public:
      explicit
      AdviceLink (Binding const& binding)
        : PointOfAdvice(binding)
        { }
      
      // using default copy/assignment
    };
  
  
  
  
  
  /**
   * Access point for the advising entity (server).
   * TODO type comment
   * 
   * @todo currently leaking buffer storage for all the advice data which isn't explicitly retracted.
   */
  template<class AD>
  class Provision
    : public AdviceLink
    {
      
      
      /* == policy definitions == */    ////TODO: extract into policy classes
      
      void
      deregistrate()
        {
          TODO ("hand-over deallocation information to the AdviceSystem");
        }
      
      
    public:
      explicit
      Provision (Literal bindingSpec =0)
        : AdviceLink (Binding(bindingSpec).addTypeGuard<AD>())
        { }
      
     ~Provision()
        {
          this->deregistrate();
        }
      
      Provision (Provision const& o)
        : AdviceLink(o)
        {
          setSolution (this, NULL );
        }
      
      Provision&
      operator= (Provision const& o)
        {
          if (!isSameObject(*this, o))
            {
              AdviceLink::operator= (o);
              setSolution (this, NULL );
            }
        }
      
      
      void setAdvice (AD const& pieceOfAdvice)
        {
          maybe_deallocateOld (
              publishProvision(
                  storeCopy (pieceOfAdvice)));
        }
      
      void retractAdvice()
        {
          maybe_deallocateOld(
              discardSolutions());
        }
      
      void
      defineBinding (Literal topic)
        {
          setBindingPattern (Binding(topic).addTypeGuard<AD>());
          maybe_rePublish();
        }
      
    private:
      PointOfAdvice* storeCopy (AD const& advice_given);
      void maybe_deallocateOld(const PointOfAdvice*);
      void maybe_rePublish ();
    };
  
  
  /**
   * Piece of Advice as incorporated into the AdviceSystem.
   * This holder-object contains a copy of the advice data
   * and is placed into the internal storage buffer; the
   * advice index keeps a (type erased) pointer to serve
   * any requests which happen to match the binding.
   * 
   * @note the ptr-to-solution in the inherited PointOfAdvice
   *       is currently (5/10) not used, because this \em is
   *       already the solution.
   */
  template<class AD>
  class ActiveProvision
    : public PointOfAdvice
    , boost::noncopyable
    {
      AD theAdvice_;
      
    public:
      AD const&
      getAdvice()  const
        {
          return theAdvice_;
        }
      
    protected:
      ActiveProvision (PointOfAdvice const& refPoint, AD const& advice_given)
        : PointOfAdvice(refPoint)
        , theAdvice_(advice_given)
        {
          setSolution (this, this); // not used currently (5/10)
        }
      
      friend class Provision<AD>;
    };
  
  
  /*  ==== memory management for Provision data ===== */
  
  /** function to copy advice into an internal buffer.
      @return type erased pointer to the data holder created
      @throw  error::Fatal on allocation problems, plus anything
              the advice data may throw during copy construction. */
  template<class AD>
  inline PointOfAdvice*
  Provision<AD>::storeCopy (AD const& advice_given)
  {
    typedef ActiveProvision<AD> Holder;
    return new(getBuffer(sizeof(Holder))) Holder (*this, advice_given);
  }
  
  
  /** @internal assist the AdviceSystem with deallocating buffer storage.
   *  Problem is we need to know the exact size of the advice value holder,
   *  which information is available only here, in the fully typed context.
   *  @note the assumption is that \em any binding created will automatically
   *        contain a type guard, which ensures the existingEntry passed in here
   *        originally was allocated by #storeCopy within the same typed context.
   */
  template<class AD>
  inline void
  Provision<AD>::maybe_deallocateOld (const PointOfAdvice* existingEntry)
  {
    typedef ActiveProvision<AD> Holder;
    if (existingEntry)
      releaseBuffer (existingEntry, sizeof(Holder));
  }
  
  
  /** @internal in case we've already published this provision,
   *  we temporarily need a new provision entry, to allow the
   *  AdviceSystem implementation to rewrite the internal index
   */
  template<class AD>
  inline void
  Provision<AD>::maybe_rePublish ()
  {
    typedef const ActiveProvision<AD> AdviceProvision;
    AdviceProvision* solution = static_cast<AdviceProvision*> (getSolution (*this));
    
    if (solution)    // create copy of the data holder, using the new binding 
      maybe_deallocateOld (
          publishProvision(
              storeCopy (solution->getAdvice())));
  }
  
  
  
  
  
  
  
  /**
   * Access point for the advised entity (client).
   * TODO type comment
   */
  template<class AD>
  class Request
    : public AdviceLink
    {
      typedef const ActiveProvision<AD> AdviceProvision;
      
      
      /* == policy definitions == */    ////TODO: extract into policy classes
      
      AD const&
      handleMissingSolution()  const  ///< @warning might segfault when used during shutdown
        {
          return NullValue<AD>::get();
        }
      
      
    public:
      explicit
      Request (Literal bindingSpec =0)
        : AdviceLink (Binding(bindingSpec).addTypeGuard<AD>())
        {
          registerRequest();
        }
      
     ~Request()
        {
          deregisterRequest();
        }
      
      // copying Requests is allowed, using default
      
      
      AD const&
      getAdvice()  const
        {
          AdviceProvision* solution = static_cast<AdviceProvision*> (getSolution (*this));
          if (!solution)
            return this->handleMissingSolution();
          else
            return solution->getAdvice();
        }
      
      
      void
      defineBinding (Literal topic)
        {
          HashVal previous_bindingKey (hash_value(*this));
          setBindingPattern (Binding(topic).addTypeGuard<AD>());
          publishRequestBindingChange (previous_bindingKey);
        }
    };
  
  
  
  
  
}} // namespace lib::advice
#endif
