/*
  LIFECYCLEREGISTRY.hpp  -  registry for application lifecycle callbacks 
 
  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file lifecycleregistry.hpp
 ** Helper for registering lifecycle event callbacks, which are
 ** provided as a global service by lumiera::Appconfig. This service
 ** allows to enroll functions under a given label and then to call
 ** all those registered functions. 
 ** @note this is in fact an event mechanism, and if we start using
 ** more than just this basic functionallity, we should switch to
 ** boost::signals. (which has the downside of being an binary
 ** dependency).
 **
 ** @see appconfig.hpp
 */


#ifndef LUMIERA_LIFECYCLE_H
#define LUMIERA_LIFECYCLE_H

#include <map>
#include <vector>
//#include <string>
//#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>



namespace lumiera
  {
//  using std::string;
//  using boost::scoped_ptr;
  using boost::noncopyable;
  using boost::function;

  typedef const char * const Symbol;  

  
  /**
   * Registry of callback functions accessable by a label (ID)
   * provided at registration. Registered functions will be added
   * to a list, which can be triggered via label. Used by Appconfig
   * to implement the lumiera lifecycle (init, shutdown) hooks.
   */
  class LifecycleRegistry
    : private noncopyable
    {
    public:
      typedef boost::function<void(void)> Hook;
      typedef std::vector<Hook> Callbacks;
      typedef Callbacks::iterator Iter;
      
      
      void enroll (Symbol label, Hook& toCall)
        {
          table_[label].push_back(toCall);
        }
      
      void execute (Symbol label)
        {
          Callbacks& cbs (table_[label]);
          Iter e = cbs.end();
          for (Iter p = cbs.begin(); 
               p != e; ++p) 
            (*p)(); // invoke callback
        }
      
      
    private:
      std::map<Symbol, Callbacks> table_;
      
      LifecycleRegistry ()  {}
      friend class Appconfig;
      
    };



} // namespace lumiera
#endif
