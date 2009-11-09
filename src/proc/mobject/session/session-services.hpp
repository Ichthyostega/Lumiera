/*
  SESSION-SERVICES.hpp  -  accessing Proc-Layer internal session implementation services
 
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


/** @file session-services.hpp
 ** A mechanism for exposing and accessing implementation level
 ** services of the session. 
 ** TODO TODO
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICES_H
#define MOBJECT_SESSION_SESSION_SERVICES_H

#include "proc/mobject/session.hpp"
#include "lib/meta/generator.hpp"

//#include <boost/scoped_ptr.hpp>
//#include <vector>




namespace mobject {
namespace session {
  
//using std::vector;
//using boost::scoped_ptr;
//using std::tr1::shared_ptr;
  using lumiera::typelist::InstantiateChained;
  using lumiera::typelist::InheritFrom;
  using lumiera::typelist::NullType;
  
  
  
  template<class API, class IMPL>
  struct ServiceAccessPoint;
  
  
  /**
   * Collection of configured implementation-level services
   * to provide by the Session. An instance of this template
   * is created on top of SessionImpl, configured such as
   * to inherit from all the concrete services to be
   * exposed for use by Proc-Lyer's internals. 
   */
  template< typename IMPS
          , class FRONT
          , class BA =NullType
          >
  class SessionServicesX
    : public InstantiateChained<typename IMPS::List, ServiceAccessPoint, BA>
    {
      static FRONT& entrance_;
    };
  
  
  
}} // namespace mobject::session
#endif
