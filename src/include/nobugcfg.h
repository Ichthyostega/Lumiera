/*
  NOBUGCFG.h  -  NoBug definitions and initialisation for the Proc-Layer
 
 
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

/** @file nobugcfg.h
 ** This header is for including and configuring NoBug.
 ** The idea is that configuration and some commonly used flag 
 ** declarations are to be kept in one central location. Subsystems
 ** are free to define and use additional flags for local use. Typically,
 ** this header will be included via some of the basic headers like error.hpp,
 ** which in turn gets included e.g. by proc/common.hpp
 **
 ** This header can thus be assumed to be effectively global. It should contain
 ** only declarations of global relevance, as any change causes the whole project 
 ** to be rebuilt. Moreover, for C++ this header assures automatic initialisation
 ** of NoBug by placing a static ctor call.
 **
 ** Besides the usual guarded declarations, this header contains one section
 ** with the corresponding <b>definitions</b>. This section is to be included once
 ** by some translation unit (currently this is lumiera/nobugcfg.cpp) in order to
 ** generate the necessary definitions.
 ** 
 ** @par Logging configuration
 ** By default, logging is configured such as to emit a small number of informative
 ** messages on the starting terminal and to report fatal errors. But besides the
 ** usual fine-grained tracing messages, we define a small number of distinct 
 ** thematic <b>Logging Channels</b> providing a consistent high-level view of
 ** what is going on with regards to a specific aspect of the application
 ** - \c operate documents a high-level overall view of what the application \em does
 ** - \c render focuses on the working of the render engine (without logging each frame)
 ** - \c config shows anything of relevance regarding the configured state of App and session
 ** - \c memory allows to diagnose a high-level view of memory management
 ** 
 ** Any log level can be overridden by an environment variable, for example
 ** \code NOBUG_LOG='operate:INFO' ./lumiera \endcode
 ** 
 ** @todo logging to files?
 */


#ifndef NOBUGCFG_H  /* ============= Part 1: DECLARATIONS ======== */
#define NOBUGCFG_H

#include <syslog.h>
#include <nobug.h>


#ifdef __cplusplus  /* ============= C++ ================ */

#include "include/lifecycle.h"
#include "lib/error.hpp"  ///< make assertions throw instead of abort()

namespace lumiera { 
  void initialise_NoBug ();
  namespace {
    LifecycleHook trigger_it_ (ON_BASIC_INIT, &initialise_NoBug);         
} }
#endif /* =====================(End) C++ ================ */




  /* declare flags used throughout the code base */
  NOBUG_DECLARE_FLAG (all);
  NOBUG_DECLARE_FLAG (lumiera_all);
  NOBUG_DECLARE_FLAG (lib_all);
  NOBUG_DECLARE_FLAG (lumiera);        ///< master log, informative console output
  NOBUG_DECLARE_FLAG (operate);        ///< logging channel reporting what the application does
  NOBUG_DECLARE_FLAG (render);         ///< logging channel focusing on the render engine's workings
  NOBUG_DECLARE_FLAG (config);         ///< logging channel covering application and session configuration
  NOBUG_DECLARE_FLAG (memory);         ///< logging channel covering memory management issues
  NOBUG_DECLARE_FLAG (test);


  /* further flags which don't fit into any specific translation unit */
  NOBUG_DECLARE_FLAG (sync);           ///< especially for tracing synchronisation
  
  
#endif /*NOBUGCFG_H    ======= (End) Part 1: DECLARATIONS ======== */






#ifdef NOBUG_INIT_DEFS_ /*========== Part 2: DEFINITIONS ========= */


  /* flags used throughout the code base... */
  NOBUG_CPP_DEFINE_FLAG              (all);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (lumiera_all, all);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (lib_all,     all);
  
  NOBUG_CPP_DEFINE_FLAG_PARENT       (lumiera,     lumiera_all);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (config,      lumiera);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (operate,     lumiera);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (render,      lumiera,    LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (memory,      lumiera,    LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (test,        all,        LOG_ERR);


  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (sync,        lib_all,    LOG_WARNING);
  


#endif /*NOBUG_INIT_DEFS_ ==== (End) Part 2: DEFINITIONS ========= */
