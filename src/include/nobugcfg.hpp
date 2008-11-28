/*
  NOBUGCFG.hpp  -  NoBug definitions and initialisation for the Proc-Layer
 
 
  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file nobugcfg.hpp
 ** This header is for including and configuring NoBug.
 ** The idea is that configuration and some commonly used flag 
 ** declarations are to be kept in one central location. Subsystems
 ** are free to define and use additional flags for local use. Typically,
 ** this header will be included via some of the basic headers like error.hpp,
 ** which in turn gets included e.g. by proc/common.hpp
 **
 ** @par This header can thus be assumed to be effectively global. It should contain
 ** only declarations of global relevance, as any change causes the whole project 
 ** to be rebuilt. Moreover, for C++ this header assures automatic initialisation
 ** of NoBug by placing a static ctor call.
 **
 ** @par Besides the usual guarded declarations, this header contains one section
 ** with the corresponding <b>definitions</b>. This section is to be included once
 ** by some translation unit (currently this is lumiera/nobugcfg.cpp) in order to
 ** generate the necessary definitions. 
 **
 */


#ifndef NOBUGCFG_H  /* ============= Part 1: DECLARATIONS ======== */
#define NOBUGCFG_H

#include <syslog.h>
#include <nobug.h>


#ifdef __cplusplus  /* ============= C++ ================ */

#include "lumiera/appconfig.hpp"
#include "include/error.hpp"  ///< make assertions throw instead of abort()

namespace lumiera { 
  void initialise_NoBug ();
  namespace {
    LifecycleHook schedule_ (ON_BASIC_INIT, &initialise_NoBug);         
} }
#endif /* =====================(End) C++ ================ */




  /* declare flags used throughout the code base... */
  NOBUG_DECLARE_FLAG (all);
  NOBUG_DECLARE_FLAG (lumiera_all);
  NOBUG_DECLARE_FLAG (lumiera);
  NOBUG_DECLARE_FLAG (operate);
  NOBUG_DECLARE_FLAG (render);
  NOBUG_DECLARE_FLAG (config);
  NOBUG_DECLARE_FLAG (memory);
  NOBUG_DECLARE_FLAG (test);


#endif /*NOBUGCFG_H    ======= (End) Part 1: DECLARATIONS ======== */






#ifdef NOBUG_INIT_DEFS_ /*========== Part 2: DEFINITIONS ========= */


  /* flags used throughout the code base... */
  NOBUG_CPP_DEFINE_FLAG              (all);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (lumiera_all, all);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (lumiera,     lumiera_all);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (operate,     lumiera,    LOG_INFO);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (render,      lumiera,    LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (config,      lumiera,    LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (memory,      lumiera,    LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (test,        all,        LOG_ERR);




#endif /*NOBUG_INIT_DEFS_ ==== (End) Part 2: DEFINITIONS ========= */
