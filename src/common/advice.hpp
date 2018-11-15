/*
  ADVICE.hpp  -  generic loosely coupled interaction guided by symbolic pattern

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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
 ** \par interfaces and implementation
 ** Client code is assumed to interface solely through the advice::Request and advice::Provision classes,
 ** which both can be instantiated and copied freely, may be used as member or mixed in as baseclass.
 ** The AdviceSystem on the other hand is an implementation facility (actually a singleton) and lives
 ** in the advice.cpp translation unit. The interface entities inherit protected from AdviceLink,
 ** which is implemented in the same scope as the AdviceSystem and thus allowed to talk to it
 ** directly. The AdviceSystem in turn uses advice::Index to keep track of the collaboration
 ** partners, which, for this purpose, are handled as type-erased PointOfAdvice elements.
 ** The latter class contains 4 API functions used by the index to manage solutions. 
 ** 
 ** @note as of 6/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces and gain practical usage experiences. Ichthyo expects this collaboration
 **       service to play a central role at various places within steam-layer.
 ** @todo allow variables in binding patterns
 ** @todo use the lumiera MPool instead of heap allocations
 ** @todo consider to provide variations of the basic behaviour by policy classes
 ** @todo the implementation is generic/defensive, and could be improved and optimised
 ** 
 ** @see AdviceBasics_test usage example
 ** @see advice.cpp implementation
 ** @see advice::Index index datastructure
 ** @see binding.hpp
 ** 
 */


#ifndef LUMIERA_ADVICE_H
#define LUMIERA_ADVICE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/null-value.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include "common/advice/binding.hpp"


namespace lumiera{
namespace advice {
  
  using lib::Literal;
  using util::isSameObject;
  
  
  /**
   * Basic (abstracted) view of an advice collaboration partner,
   * as used internally by the AdviceSystem to manage the participants.
   * Each PointOfAdvice is characterised by a binding pattern, used to
   * pair up advice::Request and advice::Provision entries. Moreover,
   * each PointOfAdvice can refer to an existing advice solution 
   * provided elsewhere in the system. The specific type of advice
   * (and thus the storage requirements) are abstracted away,
   * as is the distinction between Request and Provision.
   * 
   * @see AdviceSystem
   * @see AdviceIndex_test
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
      
      void
      setSolution (PointOfAdvice* solution =0)
        {
          resolution_ = solution;
        }
        
      const PointOfAdvice*
      getSolution ()  const
        {
          return resolution_;
        }
      
      Binding::Matcher
      getMatcher ()  const
        {
          return pattern_;
        }
      
      friend HashVal
      hash_value (PointOfAdvice const& entry)
      {
        return hash_value (entry.pattern_);
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
      void publishProvision (PointOfAdvice*);
      void discardSolutions ();
      void publishRequestBindingChange(HashVal);
      
      void registerRequest();
      void deregisterRequest();
      
      static void* getBuffer(size_t);
      static void  releaseBuffer (void*, size_t);
      
      typedef void (DeleterFunc)(void*);
      static void manageAdviceData (PointOfAdvice*, DeleterFunc*);
      
    public:
      explicit
      AdviceLink (Binding const& binding)
        : PointOfAdvice(binding)
        { }
      
      // using default copy/assignment
    };
  
  
  
  
  
  /**
   * Access point for the advising entity (server).
   * This is the interface intended for client code to set and provide
   * concrete advice information of a specific type AD. Instantiating 
   * automatically creates a \em type-guard binding pattern, but client code
   * can (and typically should) provide additional predicates to define the
   * "topic" this advice belongs to. This allows advice::Request entries
   * to attach to the suitable advice "channels" and get the specific
   * piece of advice they're looking for.
   * 
   * Any advice::Provision remains inactive and thus invisible, until
   * \link #setAdvice setting the concrete advice data.\endlink After that,
   * the provided data is \em copied into the AdviceSystem and remains available
   * even after the original Provision goes out of scope. Consequently, it isn't
   * possible to \em modify advice data once set. But client code may retract
   * the provision or change the binding pattern.
   * 
   * @see AdviceBasics_test usage example
   */
  template<class AD>
  class Provision
    : protected AdviceLink
    {
      
      
      /* == policy definitions == */    ////TODO: extract into policy classes
      
      void deregistrate() { /* NOP */ }
      
      
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
          setSolution ( NULL );
        }
      
      Provision&
      operator= (Provision const& o)
        {
          if (!isSameObject(*this, o))
            {
              AdviceLink::operator= (o);
              setSolution ( NULL );
            }
          return *this;
        }
      
      
      void setAdvice (AD const& pieceOfAdvice)
        {
          publishProvision(
            storeCopy (pieceOfAdvice));
        }
      
      void retractAdvice()
        {
          discardSolutions();
        }
      
      void
      defineBinding (Literal topic)
        {
          setBindingPattern (Binding(topic).addTypeGuard<AD>());
          maybe_rePublish();
        }
      
    private:
      PointOfAdvice* storeCopy (AD const& advice_given);
      static void releaseAdviceData (void*);
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
    , util::NonCopyable
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
          this->setSolution (this); // not used currently (5/10)
        }
      
      friend class Provision<AD>;
    };
  
  
  /*  ==== memory management for Provision data ===== */
  
  /** @internal function to copy advice into an internal buffer.
      @return type erased pointer to the data holder created
      @throw  error::Fatal on allocation problems, plus anything
              the advice data may throw during copy construction. */
  template<class AD>
  inline PointOfAdvice*
  Provision<AD>::storeCopy (AD const& advice_given)
  {
    typedef ActiveProvision<AD> Holder;
    void* storage = getBuffer(sizeof(Holder));
    try
      {
        Holder* storedProvision = new(storage) Holder (*this, advice_given);
        manageAdviceData (storedProvision, &releaseAdviceData);
        return storedProvision;
      }
    catch(...)
      {
        Literal errID = lumiera_error();
        releaseBuffer (storage, sizeof(Holder));
        throw lumiera::error::Fatal ("Failure to store advice data", errID);
      }
  }
  
  
  /** @internal assist the AdviceSystem with deallocating buffer storage.
   *  Problem is we need to know the exact size of the advice value holder,
   *  which information is available only here, in the fully typed context.
   */
  template<class AD>
  inline void
  Provision<AD>::releaseAdviceData (void* entry)
  {
    typedef ActiveProvision<AD> Holder;
    if (entry)
      {
        Holder* obj = static_cast<Holder*> (entry);
        obj->~Holder();
        releaseBuffer (entry, sizeof(Holder));
      }
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
    AdviceProvision* solution = static_cast<AdviceProvision*> (getSolution());
    
    if (solution)    // create copy of the data holder, using the new binding 
      publishProvision(
        storeCopy (solution->getAdvice()));
  }
  
  
  
  
  
  
  
  /**
   * Access point for the advised entity (client).
   * This is the interface intended for client code to request advice
   * of a specific type and optionally limited to a special topic (binding).
   * Instantiating an \c Request<AD> object automatically entails a registration
   * with the AdviceSystem behind the scenes, and deleting it causes deregistration.
   * Request objects may be instantiated and copied freely, and the binding pattern
   * may be changed. The actual advice is accessed through the member function
   * #getAdvice, which might return a default-constructed piece of advice data
   * in case no specific advice has been provided yet. Accessing advice can be
   * considered a lightweight operation, while creating/destroying advice
   * causes an index operation and thus requires a lock.
   * 
   * Creating an Request and changing the binding might fail, while the dtor
   * is protected against failure (as you'd expect). Accessing advice can also
   * be considered safe, given the concrete advice type can be default constructed
   * without failure in case there isn't any advice data provided yet.
   */
  template<class AD>
  class Request
    : protected AdviceLink
    {
      typedef const ActiveProvision<AD> AdviceProvision;
      
      
      /* == policy definitions == */    ////TODO: extract into policy classes
      
      AD const&
      handleMissingSolution()  const  ///< @warning might segfault when used during shutdown
        {
          return lib::NullValue<AD>::get();
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
          AdviceProvision* solution = static_cast<AdviceProvision*> (this->getSolution());
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
  
  
  
  
  
}} // namespace lumiera::advice
#endif
