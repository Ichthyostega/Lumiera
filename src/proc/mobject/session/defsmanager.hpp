/*
  DEFSMANAGER.hpp  -  access to preconfigured default objects and definitions
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef MOBJECT_SESSION_DEFSMANAGER_H
#define MOBJECT_SESSION_DEFSMANAGER_H


#include "lib/p.hpp"
#include "lib/query.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>



namespace mobject {
  namespace session {
    
    
    using lumiera::P;
    using boost::scoped_ptr;
    
    namespace impl { class DefsRegistry; }


    /**
     * Organise a collection of preconfigured default objects.
     * For various kinds of objects we can tweak the default parametrisation
     * as part of the general session configuration. A ref to an instance of
     * this class is accessible through the current session and can be used
     * to fill in parts of the configuration of new objects, if the user 
     * code didn't give more specific parameters. Necessary sub-objects 
     * will be created on demand, and any default configuration, once
     * found, will be remembered and stored with the current session.
     */
    class DefsManager : private boost::noncopyable
      {
        scoped_ptr<impl::DefsRegistry> defsRegistry;
        
      protected:
        
        DefsManager ()  throw();
        friend class SessManagerImpl;
        
      public:
       ~DefsManager ();
       
        /** common access point: retrieve the default object fulfilling
         *  some given conditions. May silently trigger object creation.
         *  @throw error::Config in case no solution is possible, which
         *         is considered \e misconfiguration. 
         */
        template<class TAR>
        P<TAR> operator() (const lumiera::Query<TAR>&);
        
        
        /** search through the registered defaults, never create anything.
         *  @return object fulfilling the query, \c empty ptr if not found. 
         */
        template<class TAR>
        P<TAR> search  (const lumiera::Query<TAR>&);
        
        /** retrieve an object fulfilling the query and register it as default.
         *  The resolution is delegated to the ConfigQuery system (which may cause
         *  creation of new object instances) 
         *  @return object fulfilling the query, \c empty ptr if no solution.
         */ 
        template<class TAR>
        P<TAR> create  (const lumiera::Query<TAR>&);
        
        /** register the given object as default, after ensuring it fulfils the
         *  query. The latter may cause some properties of the object to be set,
         *  trigger creation of additional objects, and may fail altogether.
         *  @return true if query was successful and object is registered as default
         *  @note only a weak ref to the object is stored
         */ 
        template<class TAR>
        bool define  (const P<TAR>&, const lumiera::Query<TAR>&);
        
        /** remove the defaults registration of the given object, if there was such
         *  @return false if nothing has been changed because the object wasn't registered
         */
        template<class TAR>
        bool forget  (const P<TAR>&);
        
        
// Q: can we have something along the line of...?
//
//        template
//          < class TAR,                   ///< the target to query for 
//            template <class> class SMP  ///<  smart pointer class to wrap the result
//          >
//        SMP<TAR> operator() (const lumiera::Query<TAR>&);
        
      };

      

  } // namespace mobject::session

} // namespace mobject
#endif
