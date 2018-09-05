/*
  ZOMBIE-CHECK.hpp  -   flatliner self-detection

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file zombie-check.hpp
 ** Detector to set off alarm when (re)using deceased objects.
 ** When implementing services based on static fields or objects,
 ** an invocation after static shutdown can not be precluded -- be it by
 ** re-entrance, be through indirect reference to some dependency within a
 ** static function residing in another translation unit. Since typically the
 ** values in static storage are not overwritten after invoking the destructor,
 ** we may plant an automatic "zombie detector" to give a clear indication of
 ** such a policy violation (Lumiera forbids to use dependencies from dtors).
 ** 
 ** @see sync-classlock.hpp
 ** @see depend.hpp
 */


#ifndef LIB_ZOMBIE_CHECK_H
#define LIB_ZOMBIE_CHECK_H

#include "lib/error.hpp"



namespace lib {
  namespace error = lumiera::error;
  
  
  /**
   * Automatic lifecycle tracker, to produce an alarm when accessing objects after deletion.
   */
  class ZombieCheck
    {
      bool deceased_ = false;
      
    public:
      ZombieCheck() = default;
     ~ZombieCheck()
        {
          deceased_ = true;
        }
      
      operator bool()  const
        {
          return deceased_;
        }
      
      void
      operator() ()  const
        {
          if (deceased_)
            throw error::Fatal("Already deceased object called out of order during Application shutdown. "
                               "Lumiera Policy violated: Dependencies must not be used from destructors."
                              ,error::LUMIERA_ERROR_LIFECYCLE);
        }
    };
  
  
} // namespace lib
#endif /*LIB_ZOMBIE_CHECK_H*/
