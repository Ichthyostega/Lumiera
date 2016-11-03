/*
  DEFS-MANAGER.hpp  -  access to preconfigured default objects and definitions

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file §§§
 ** TODO §§§
 */


#ifndef LUMIERA_QUERY_DEFS_MANAGER_H
#define LUMIERA_QUERY_DEFS_MANAGER_H


#include "lib/p.hpp"
#include "common/query.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>



namespace lumiera{
namespace query  {
  
  
  using lumiera::Query;
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
   * 
   * @note while the logic of defaults handling can be considered
   *       roughly final, as of 12/09 most of the actual object
   *       handling is placeholder code.
   */
  class DefsManager : private boost::noncopyable
    {
      scoped_ptr<impl::DefsRegistry> defsRegistry_;
      
    public:
      
      DefsManager ()  throw();
     ~DefsManager ();
      
      /** common access point: retrieve the default object fulfilling
       *  some given conditions. May silently trigger object creation.
       *  @throw error::Config in case no solution is possible, which
       *         is considered \e misconfiguration. 
       */
      template<class TAR>
      lib::P<TAR> operator() (Query<TAR> const&);
      
      
      /** search through the registered defaults, never create anything.
       *  @return object fulfilling the query, \c empty ptr if not found. 
       */
      template<class TAR>
      lib::P<TAR> search  (Query<TAR> const&);
      
      /** retrieve an object fulfilling the query and register it as default.
       *  The resolution is delegated to the ConfigQuery system (which may cause
       *  creation of new object instances) 
       *  @return object fulfilling the query, \c empty ptr if no solution.
       */ 
      template<class TAR>
      lib::P<TAR> create  (Query<TAR> const&);
      
      /** register the given object as default, after ensuring it fulfils the
       *  query. The latter may cause some properties of the object to be set,
       *  trigger creation of additional objects, and may fail altogether.
       *  @return true if query was successful and object is registered as default
       *  @note only a weak ref to the object is stored
       */ 
      template<class TAR>
      bool define  (lib::P<TAR> const&, Query<TAR> const&  =Query<TAR>());
      
      /** remove the defaults registration of the given object, if there was such
       *  @return false if nothing has been changed because the object wasn't registered
       */
      template<class TAR>
      bool forget  (lib::P<TAR> const&);
      
      
      /** @internal for session lifecycle */
      void clear();
      
      
// Q: can we have something along the line of...?
//
//        template
//          < class TAR,                   ///< the target to query for 
//            template <class> class SMP  ///<  smart pointer class to wrap the result
//          >
//        SMP<TAR> operator() (const lumiera::Query<TAR>&);

// 12/09: according to my current knowledge of template metaprogramming, the answer is "no",
//        but we could use a traits template to set up a fixed association of smart pointers
//        and kinds of target object. This would allow to define a templated operator() returning
//        the result wrapped into the right holder. But currently I don't see how to build a sensible
//        implementation infrastructure backing such an interface.
//////////TICKET #452
      
    };
  
  
  
}} // namespace lumiera::query
#endif
