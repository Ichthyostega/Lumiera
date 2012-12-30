/*
  DefsManager  -  access to preconfigured default objects and definitions

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

* *****************************************************/


#include "proc/mobject/session/defs-manager.hpp"
#include "proc/mobject/session/defs-registry.hpp"
#include "common/configrules.hpp"
#include "lib/error.hpp"

#include <boost/format.hpp>

using boost::format;

using lumiera::ConfigRules;
using lumiera::query::QueryHandler;
using lumiera::query::LUMIERA_ERROR_CAPABILITY_QUERY;


namespace proc {
namespace mobject {
namespace session {
  
  
  
  
  /** initialise the most basic internal defaults. */
  DefsManager::DefsManager ()  throw()
    : defsRegistry(new DefsRegistry)
  {
    TODO ("setup basic defaults of the session");
  }
  
  
  
  /** @internal causes boost::checked_delete from \c scoped_ptr<DefsRegistry>
   *  to be placed here, where the declaration of DefsRegistry is available.*/
  DefsManager::~DefsManager() {}
  
  
  
  template<class TAR>
  P<TAR>
  DefsManager::search  (const Query<TAR>& capabilities)
  {
    P<TAR> res;
    QueryHandler<TAR>& typeHandler = ConfigRules::instance();
    for (DefsRegistry::Iter<TAR> i = defsRegistry->candidates(capabilities);
         res = *i ; ++i )
      {
        typeHandler.resolve (res, capabilities);
        if (res)
          return res;
      }
    return res; // "no solution found"
  }
  
  
  template<class TAR>
  P<TAR> 
  DefsManager::create  (const Query<TAR>& capabilities)
  {
    P<TAR> res;
    QueryHandler<TAR>& typeHandler = ConfigRules::instance();
    typeHandler.resolve (res, capabilities);
    if (res)
      defsRegistry->put (res, capabilities);
    return res;
  }
  
  
  template<class TAR>
  bool 
  DefsManager::define  (const P<TAR>& defaultObj, const Query<TAR>& capabilities)
  {
    P<TAR> candidate (defaultObj);
    QueryHandler<TAR>& typeHandler = ConfigRules::instance();  
    typeHandler.resolve (candidate, capabilities);
    if (!candidate)
      return false;
    else
      return defsRegistry->put (candidate, capabilities);
  }
  
  
  template<class TAR>
  bool 
  DefsManager::forget  (const P<TAR>& defaultObj)
  {
    return defsRegistry->forget (defaultObj);
  }
  
  
  template<class TAR>
  P<TAR>
  DefsManager::operator() (const Query<TAR>& capabilities)
  {
    P<TAR> res (search (capabilities));
    if (res) 
      return res;
    else
      res = create (capabilities); // not yet known as default, create new
    
    if (!res)
      throw lumiera::error::Config ( str(format("The following Query could not be resolved: %s.")
                                               % capabilities.asKey())
                                   , LUMIERA_ERROR_CAPABILITY_QUERY );
    else
      return res;
  }
  
}}} // namespace mobject::session






   /***************************************************************/
   /* explicit template instantiations for querying various Types */
   /***************************************************************/

#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "proc/mobject/session/track.hpp"

namespace proc {
namespace mobject {
namespace session {
  
  
  using asset::Pipe;
  using asset::PPipe;
  using asset::ProcPatt;
  using asset::PProcPatt;
  using asset::Timeline;
  using asset::PTimeline;
  using asset::Sequence;
  using asset::PSequence;
  
  using mobject::session::Track;
  using mobject::session::PTrack;
  
  template PPipe       DefsManager::operator() (Query<Pipe>     const&);
  template PProcPatt   DefsManager::operator() (Query<const ProcPatt> const&);
  template PTrack      DefsManager::operator() (Query<Track>    const&);
  template PTimeline   DefsManager::operator() (Query<Timeline> const&);
  template PSequence   DefsManager::operator() (Query<Sequence> const&);
  
  template bool DefsManager::define (PPipe const&, Query<Pipe> const&);
  template bool DefsManager::forget (PPipe const&);
  
  
}}} // namespace proc::mobject::session
