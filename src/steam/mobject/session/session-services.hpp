/*
  SESSION-SERVICES.hpp  -  accessing Steam-Layer internal session implementation services

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


/** @file session-services.hpp
 ** A mechanism for exposing and accessing implementation level
 ** services of the session. While any client code should always
 ** use the public Session API, some implementation level facilities
 ** within Steam-Layer need to cooperate with a wider SessionImpl API.
 ** On the other hand, we don't want to create coupling between the
 ** mentioned Steam internals and the session implementation. Another
 ** concern addressed by this mechanism is to assure consistency
 ** across all those implementation APIs. New APIs can be added
 ** just by extending a template definition and will automatically
 ** participate in the session management mechanisms, because any
 ** access is routed through the top level Session PImpl.
 ** 
 ** \par structure of session implementation-level services
 ** 
 ** Assumed any part of the Steam implementation needs to cooperate
 ** with the session implementation; the necessary link has to be
 ** abstracted into an implementation level API. Typically, this
 ** API provides an static access function, which is to be implemented
 ** "somewhere else", so the Steam implementation isn't required to 
 ** include anything of the session implementation level
 ** 
 ** In order to actually provide such a service, an specialisation of
 ** the ServiceAccessPoint template has to be defined, which may mix in
 ** the service API and implement it directly on top of SessionImpl.
 ** Note, mixing in the API isn't required -- alternatively the API might
 ** be completely bridged through the mentioned static access functions
 ** (i.e. such would be kind of an generic API, relying on convention
 ** rather than on a vtable)
 ** 
 ** When the SessManagerImpl creates the concrete session object,
 ** it doesn't use the bare SessionImpl class; rather, an inheritance
 ** chain is formed, starting with SessionImpl and stacking any of the
 ** configured ServiceAccessPoint instantiations on top of it. Thus,
 ** the public session access gets the concrete implementation of the
 ** Session API (through the vtable), while any service level access
 ** can use the corresponding service API directly. Service APIs have
 ** to care to avoid name clashes though.
 ** 
 ** The implementation of all the service API access functions is
 ** bundled within session-service.cpp -- where the full compound
 ** of SessionImpl and the ServiceAccessPoint specialisations has
 ** to be visible.
 ** 
 ** @see session-service-access-test.cpp simplified inline demo definition of this setup
 ** @see session.hpp public session API
 ** @see session-impl.hpp definition of ServiceAccessPoint specialisations
 ** @see session-impl.cpp session implementation internals
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICES_H
#define MOBJECT_SESSION_SESSION_SERVICES_H

#include "steam/mobject/session.hpp"
#include "lib/meta/generator.hpp"




namespace steam {
namespace mobject {
namespace session {
  
  using lib::meta::InstantiateChained;
  using lib::meta::Types;
  
  
  /**
   * Access point to a single implementation-level API.
   * For each concrete service to provide, an specialisation
   * of this template is assumed to exist which inherits from
   * IMPL; it will be used in an inheritance-chain on top of
   * SessionImpl and thus can access the latter (= session
   * implementation API) just through its parent class IMPL
   */
  template<class API, class IMPL>
  struct ServiceAccessPoint;
  
  
  /**
   * Collection of configured implementation-level services
   * to provide by the Session. An instance of this template
   * is created on top of SessionImpl, configured such as
   * to inherit from all the concrete services to be
   * exposed for use by Steam-Layer's internals.
   * 
   * @param APIS  sequence of API types to implement
   * @param FRONT type of the frontend used for access
   * @param SESS  the basic session implementation 
   */
  template< typename APIS
          , class FRONT
          , class SESS
          >
  class SessionServices
    : public InstantiateChained< typename APIS::List    // for each of the API types...
                               , ServiceAccessPoint    //  instantiate a service implementation
                               , SESS                 //   and stack all these on top of SessionImpl
                               >
    {
    public:
      static FRONT& current;                      ///< intended to be hard-wired to SessManagerImpl singleton 
      
      /** access an service by explicit downcast.
       *  @warning this function is dangerous; never store the
       *           returned reference, as the referred object 
       *           might go away due to session close/reset/load
       */
      template<class API>
      API& get() { return *this; }
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
