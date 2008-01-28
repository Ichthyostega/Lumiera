/*
  DefsManager  -  access to preconfigured default objects and definitions
 
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
 
* *****************************************************/


#include "proc/mobject/session/defsmanager.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/port.hpp"
#include "common/configrules.hpp"
#include "common/query/mockconfigrules.hpp"  // TODO: better way to handle the includes (rework singleton template?)
#include "common/error.hpp"

#include <boost/format.hpp>

using boost::format;

using asset::Query;
using asset::Port;
using asset::ProcPatt;
using asset::PProcPatt;

using cinelerra::ConfigRules;
using cinelerra::query::CINELERRA_ERROR_CAPABILITY_QUERY;

namespace mobject
  {
  namespace session
    {
    
    /** initialize the most basic internal defaults. */
    DefsManager::DefsManager ()  throw()
    {
      
    }

    
    template<class TAR>
    shared_ptr<TAR> 
    DefsManager::operator() (const Query<TAR>& capabilities)
    {
      shared_ptr<TAR> res = cinelerra::ConfigRules::instance().resolve (capabilities);
      
      if (!res)
        throw cinelerra::error::Config ( str(format("The following Query could not be resolved: %s.") 
                                                   % capabilities)
                                       , CINELERRA_ERROR_CAPABILITY_QUERY );
      else
        return res;
    }

    
    
   /***************************************************************/
   /* explicit template instantiations for querying various Types */
   /***************************************************************/

    template shared_ptr<Port> DefsManager::operator ()(const Query<Port>&); 
    template PProcPatt        DefsManager::operator ()(const Query<const ProcPatt>&); 

  } // namespace mobject::session

} // namespace mobject

