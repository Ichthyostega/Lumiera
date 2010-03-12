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
 ** @see session-element-tracker-test.cpp
 ** @see session-interface-modules
 ** @see Timeline
 ** @see Sequence
 **
 */


#ifndef LIB_ELEMENT_TRACKER_H
#define LIB_ELEMENT_TRACKER_H

#include "lib/p.hpp"
#include "lib/ref-array-impl.hpp"




namespace lib {
  
  using lumiera::P;
  
  /**
   * Custom implementation of the RefArray interface,
   * used by the Session to keep track of all timelines
   * and sequences. The registration/deregistration functions
   * are accessible as SessionServices
   */
  template<typename ELM>
  class ElementTracker
    : public lib::RefArrayVector<P<ELM> >
    {
    public:
      void
      append (P<ELM> const& asset)
        {
          UNIMPLEMENTED ("attach entry to session");
        }
      
      void
      remove (ELM const& asset)
        {
          UNIMPLEMENTED ("detach entry from session");
        }
      
      bool
      isRegistered (ELM const& asset)
        {
          UNIMPLEMENTED ("detect if the given element is indeed registered within this");
        }
    };
  




} // namespace lib
#endif
