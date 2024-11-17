/*
  ELEMENT-TRACKER.hpp  -  registry for tracking instances automatically

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file element-tracker.hpp
 ** Tracking instances automatically on creation and disposal.
 ** These helper templates can be used to provide a registry of all objects
 ** of some kind or type. The class to be used with instance tracking needs
 ** to cooperate, typically by inheriting from lib::AutoRegistered. Moreover,
 ** the instance lifecycle handling is assumed to utilise lumiera::P or a
 ** similar smart-ptr, which allows the registry to hold a safe reference.
 ** Thus, references won't be cleaned up automatically (as there is always
 ** at least one reference left) -- rather we rely on invoking a dispose or
 ** unlink function explicitly.
 ** 
 ** The standard use case for this feature is the management of the structural
 ** facade assets Timeline and Sequence within the session. They're created
 ** automatically to reflect some internal structures within the model; 
 ** with the help of lib::ElementTracker we can expose a list of
 ** all instances as part of the public Session interface.
 ** 
 ** @warning AutoRegistered does not check accessibility of the configured
 ** registry, when detaching an element. Especially, when detaching in turn
 ** gets automatically invoked from some kind of cleanup, care has to be taken
 ** to ensure the registry is still available. The Functions
 ** AutoRegistered::is_attached_to and AutoRegistered::deactivateRegistryLink
 ** can be used for detaching an Registry explicitly before destruction.
 ** Ignoring this might lead to segfault on application shutdown. /////////////////TICKET #574
 ** @warning be sure to understand the implications of object identity,
 ** especially when it comes to closing and opening the Session. Especially
 ** note that Assets use a systematic identity, not an instance identity,
 ** which might lead to removing an old instance from the new session.
 ** 
 ** @see session-element-tracker-test.cpp
 ** @see session-interface-modules.hpp
 ** @see Timeline
 ** @see Sequence
 **
 */


#ifndef LIB_ELEMENT_TRACKER_H
#define LIB_ELEMENT_TRACKER_H

#include "lib/p.hpp"
#include "lib/optional-ref.hpp"
#include "lib/util-foreach.hpp"
#include "lib/ref-array-impl.hpp"



namespace lib {
  
  /**
   * Registry for tracking object instances.
   * Custom implementation of the RefArray interface,
   * based on a vector. Especially used  by the Session
   * to keep track of all timelines and sequences.
   * Typically, ELM will inherit from AutoRegistered<ELM>,
   * which in turn will invoke the registration/deregistration.
   * Because of the smart-ptr-from-this problem, removal takes
   * a direct reference, as opposed to a smart-ptr.
   * @note ELM is required to provide an equality test.
   *       Depending on the semantics of this equality,
   *       registration might behave surprisingly, as
   *       previously registered \em equivalent instances
   *       will be deregistered prior to appending the 
   *       new instance. 
   */
  template<typename ELM>
  class ElementTracker
    : public lib::RefArrayVector<P<ELM>>
    {
      using  _Vec = std::vector<P<ELM>>;
      using  Iter = typename _Vec::iterator;
      using CIter = typename _Vec::const_iterator;
      
    public:
     ~ElementTracker()
        {
          try { clear(); }
          catch(...) {/*ignored*/}
        }
      
      void
      clear ()
        {
          _Vec toKill;
          toKill.reserve(_Vec::size());
          toKill.swap(*this);       // prevent quadratic detach()
          ASSERT (0 == _Vec::size());
          util::for_each (toKill, unlink_it);
        }
      
      void
      append (P<ELM> const& asset)
        {
          REQUIRE (asset, "Attempt to track a NIL element");
          remove (*asset);
          this->push_back (asset);
        }
      
      void
      remove (ELM const& asset)
        {
          for (Iter i = _Vec::begin();
               i != _Vec::end() ; ++i )
            if (asset == **i)       // _Vec contains smart-ptrs
              {                    //   ELM is required to define '=='
                this->erase (i);
                return;
              }
        }
      
      bool
      isRegistered (ELM const& asset)  const
        {
          for (CIter i = _Vec::begin();
               i != _Vec::end() ; ++i )
            if (asset == **i)
              return true;
          
          return false;
        }
      
    private:
      static void
      unlink_it (P<ELM>& elm)
        {
          REQUIRE (elm);
          try { elm->detach(); }
          catch(...)
            {
              WARN (common,"problems while clearing ElementTracker, ignored.");
            }
        }
    };
  
  
  
  /**
   * Helper mixin template for implementing a type
   * intended to participate in automatic element tracking.
   * - the element-tracking registry is accessed through
   *   the static functor #getRegistry
   * - a factory and a #detach operation is provided,
   *   automatically handling registration.
   * It is not mandatory to use this template, but
   * types participating in automatic element tracking
   * should provide equivalent functionality.
   */
  template<typename TAR>
  class AutoRegistered
    {
    public:
      typedef lib::ElementTracker<TAR> Registry;
      typedef lib::OptionalRef<Registry> RegistryLink;
      
      /** detach this element
       *  from the element-tracking registry.
       *  @note called when destroying an non-empty registry.
       */
      void
      detach()
        {
          if (!getRegistry) return;
          TAR& element = static_cast<TAR&> (*this);
          
          getRegistry().remove(element);
          ENSURE (!getRegistry().isRegistered(element));
        }
      
      
      typedef P<TAR> PTarget;
      
      /** factory for creating smart-ptr managed
       *  TAR instances, automatically registered
       *  with the element-tracking registry.
       */
      static PTarget
      create()
        {
          REQUIRE (getRegistry);
          
          PTarget newElement (new TAR());
          getRegistry().append (newElement);
          
          ENSURE (newElement);
          ENSURE (getRegistry().isRegistered(*newElement));
          return newElement;
        }
      
      
      static void
      setRegistryInstance (Registry& registry_to_use)
        {
          getRegistry.link_to (registry_to_use);
        }
      
      static void
      deactivateRegistryLink()
        {
          getRegistry.clear();
        }
      
      static bool
      is_attached_to (Registry const& someRegistry)
        {
          return getRegistry.points_to (someRegistry);
        }
      
    protected:
      static RegistryLink getRegistry;
    };
  
  /** storage for the functor to link an AutoRegistered entity
   *  to the corresponding registration service */
  template<typename TAR>
  typename AutoRegistered<TAR>::RegistryLink  AutoRegistered<TAR>::getRegistry;
  
  
  
} // namespace lib
#endif
