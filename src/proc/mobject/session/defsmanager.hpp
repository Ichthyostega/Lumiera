/*
  DEFSMANAGER.hpp  -  access to preconfigured default objects and definitions
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "common/query.hpp"

#include <tr1/memory>



namespace mobject
  {
  namespace session
    {
    using std::tr1::shared_ptr;


    /**
     * Organize a collection of preconfigured default objects.
     * For various kinds of objects we can tweek the default parametrisation
     * as part of the general session configuration. A ref to an instance of
     * this class is accessible through the current session and can be used
     * to fill in parts of the configuration of new objects, if the user 
     * code didn't give more specific parameters. Necessary sub-objects 
     * will be created on demand, and any default configuration, once
     * found, will be remembered and stored with the current session.
     */
    class DefsManager
      {
      protected:
        
        DefsManager ()  throw();
        friend class SessManagerImpl;
        
      public:
        /** common access point: retrieve the default object fulfilling
         *  some given conditions. May silently trigger object creation.
         *  @throw error::Config in case no solution is possible, which
         *         is considered \e misconfiguration. 
         */
        template<class TAR>
        shared_ptr<TAR> operator() (const cinelerra::Query<TAR>&);
        
        /** search through the registered defaults, never create anything.
         *  @return object fulfilling the query, \c empty ptr if not found. 
         */
        template<class TAR>
        shared_ptr<TAR> search  (const cinelerra::Query<TAR>&);
        
        /** retrieve an object fulfilling the query and register it as default.
         *  The resolution is delegated to the ConfigQuery system (which may cause
         *  creation of new object instances) 
         *  @return object fulfilling the query, \c empty ptr if no solution.
         */ 
        template<class TAR>
        shared_ptr<TAR> create  (const cinelerra::Query<TAR>&);
        
        /** register the given object as default, after ensuring it fulfills the
         *  query. The latter may cause some properties of the object to be set,
         *  trigger creation of additional objects, and may fail altogether.
         *  @return true if query was successfull and object is registered as default
         *  @note only a weak ref to the object is stored
         */ 
        template<class TAR>
        bool define  (shared_ptr<TAR>&, const cinelerra::Query<TAR>&);
        
        /** remove the defaults registration of the given object, if there was such
         *  @return false if nothing has been changed because the object wasn't registered
         */
        template<class TAR>
        bool forget  (shared_ptr<TAR>&);
        
        
// Q: can we have something along the line of...?
//
//        template
//          < class TAR,                   ///< the target to query for 
//            template <class> class SMP  ///<  smart pointer class to wrap the result
//          >
//        SMP<TAR> operator() (const cinelerra::Query<TAR>&);
        
      };

      

  } // namespace mobject::session

} // namespace mobject
#endif
