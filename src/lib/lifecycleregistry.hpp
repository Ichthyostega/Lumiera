/*
  LIFECYCLEREGISTRY.hpp  -  registry for application lifecycle callbacks 

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

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

/** @file lifecycleregistry.hpp
 ** Helper for registering lifecycle event callbacks, which are
 ** provided as a global service by lumiera::AppState. This service
 ** allows to enrol functions under a given label and then to call
 ** all those registered functions. 
 ** @note this is in fact an event mechanism, and if we start using
 ** more than just this basic functionality, we should switch to
 ** boost::signals. (which has the downside of being an binary
 ** dependency).
 **
 ** @see appstate.hpp
 */


#ifndef LUMIERA_LIFECYCLEREGISTRY_H
#define LUMIERA_LIFECYCLEREGISTRY_H


#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <functional>
#include <string>
#include <set>
#include <map>


namespace lumiera {
  
  using boost::noncopyable;
  using util::contains;
  using std::function;
  using std::string;


  
  /**
   * Registry of callback functions accessible by a label (ID)
   * provided at registration. Registered functions will be added
   * to a list, which can be triggered via label. Used by AppState
   * to implement the lumiera lifecycle (init, shutdown) hooks.
   */
  class LifecycleRegistry
    : private noncopyable
    {
    public:
      typedef void (*Hook)(void);
      typedef std::set<Hook> Callbacks;
      typedef Callbacks::iterator Iter;
      
      
      /** @note only one copy of each distinct callback remembered */ 
      bool enrol (Symbol label, Hook toCall)
        {
          return table_[label]
                        .insert(toCall)
                        .second;  // true if actually stored 
        }
      
      void execute (Symbol label)
        {
          Callbacks& cbs (table_[label]);
          Iter e = cbs.end();
          for (Iter p = cbs.begin(); 
               p != e; ++p) 
            (*p)(); // invoke callback
        }
      
      
      /** get the (single) LifecycleRegistry instance. 
       *  @warning don't use it after the end of main()! */
      static LifecycleRegistry& instance();
      
      
    private:
      std::map<Symbol, Callbacks> table_;
      
      LifecycleRegistry ()  {
        execute (ON_BASIC_INIT);   // just to be sure, typically a NOP, because nothing is registered yet 
      }
      
     ~LifecycleRegistry () { }
      
    };



} // namespace lumiera
#endif
