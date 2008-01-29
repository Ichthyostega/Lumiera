/*
  MockConfigRules  -  mock implementation of the config rules system
 
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


#include "common/query/mockconfigrules.hpp"

#include "proc/mobject/session/track.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/port.hpp"


//#include "common/util.hpp"
#include "nobugcfg.h"



namespace cinelerra
  {
  
  namespace query
    {
    
    MockConfigRules::MockConfigRules () 
    {
      
    }
    
    shared_ptr<Track> 
    MockConfigRules::resolve (Query<Track> q)
    {
      UNIMPLEMENTED ("solve for Track");
    }
    
    shared_ptr<Port>  
    MockConfigRules::resolve (Query<Port> q)
    {
      UNIMPLEMENTED ("solve for Port");
    }
    
    PProcPatt
    MockConfigRules::resolve (Query<const ProcPatt> q)
    {
      UNIMPLEMENTED ("solve for ProcPatt");
    }

  
  } // namespace query
    
  
  /** */


} // namespace cinelerra
