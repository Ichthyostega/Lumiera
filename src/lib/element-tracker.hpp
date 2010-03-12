/*
  ELEMENT-TRACKER.hpp  -  registry for tracking instances automatically
 
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
 ** to ensure the registry is still available. Ignoring this might lead to
 ** segfault on application shutdown. ///TICKET #574
 ** 
 ** @see session-element-tracker-test.cpp
 ** @see session-interface-modules
 ** @see Timeline
 ** @see Sequence
 **
 */


#ifndef LIB_ELEMENT_TRACKER_H
#define LIB_ELEMENT_TRACKER_H

#include "lib/p.hpp"
#include "lib/util-foreach.hpp"
#include "lib/ref-array-impl.hpp"




namespace lib {
  
  using lumiera::P;
  
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
    : public lib::RefArrayVector<P<ELM> >
    {
      typedef std::vector<P<ELM> > _Vec;
      typedef typename _Vec::iterator Iter;
      typedef typename _Vec::const_iterator CIter;
      
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
          toKill.swap(*this);
          ASSERT (0 == _Vec::size());
          util::for_each (toKill, unlink_it);
        }
      
      void
      append (P<ELM> const& asset)
        {
          REQUIRE (asset, "Attempt to track a NIL element");
          remove (*asset);
          push_back (asset);
        }
      
      void
      remove (ELM const& asset)
        {
          for (Iter i = _Vec::begin();
               i != _Vec::end() ; ++i )
            if (asset == **i)       // _Vec contains smart-ptrs
              {                    //   ELM is required to define '=='
                erase (i);
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
              WARN (common,"ignoring problems while clearing ElementTracker");
            }
        }
    };
  




} // namespace lib
#endif
