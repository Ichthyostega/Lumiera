/*
  ZOMBIE-CHECK.hpp  -   flatliner self-detection

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** @see ZombieCheck_test
 ** @see sync-classlock.hpp
 ** @see depend.hpp
 */


#ifndef LIB_ZOMBIE_CHECK_H
#define LIB_ZOMBIE_CHECK_H

#include "lib/error.hpp"

#include <cstring>
#include <string>



namespace lib {
  namespace error = lumiera::error;
  
  using std::string;
  
  
  /**
   * Automatic lifecycle tracker, to produce an alarm when accessing objects after deletion.
   * @warning ensure the ZombieCheck instance lives in static memory, otherwise it won't work.
   */
  class ZombieCheck
    {
      bool deceased_ = false;
      char zombieID_[42]{};
      
    public:
      /**
       * install a zombie check, tagged with the given id.
       * When invoked after death, the raised error::Fatal
       * includes this ID in the diagnostic message.
       * @remark recommended to use util::typeStr 
       */
      ZombieCheck (string id)
        {
          std::strncpy(zombieID_, id.c_str(), 41);
        }
      
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
            throw error::Fatal(buildDiagnosticMessage()
                              ,error::LUMIERA_ERROR_LIFECYCLE);
        }
      
    private:
      string
      buildDiagnosticMessage()  const
        {
          string msg{"Already deceased object called out of order during Application shutdown. "
                     "Lumiera Policy violated: Dependencies must not be used from destructors."};
          if (zombieID_[0])
            msg += " Offender = "+string{zombieID_};
          return msg;
        }
    };
  
  
} // namespace lib
#endif /*LIB_ZOMBIE_CHECK_H*/
