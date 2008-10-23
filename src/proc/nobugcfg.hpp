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
 ** The idea is that configuration and some globally used flag 
 ** declarations are to be kept in one central location. Normally,
 ** this header will be included via some of the basic headers like
 ** error.hpp, which in turn gets included by proc/lumiera.hpp
 **
 ** @par Besides the usual guarded declarations, this header contains
 ** one section with the corresponding <b>definitions</b>. This section
 ** is to be included once by some translation unit (currently this is
 ** nobugcfg.cpp) in order to generate the necessary definitions.
 **
 ** @note this header assures automatic initialisation of NoBug
 **       by placing a static ctor call.
 **
 */


#ifndef NOBUGCFG_H  /* ============= Part 1: DECLARATIONS ======== */
#define NOBUGCFG_H

#include <syslog.h>
#include <nobug.h>
#include "lib/appconfig.hpp"
#include "common/error.hpp"  ///< make assertions throw instead of abort()


  /* declare flags used throughout the code base... */
  NOBUG_DECLARE_FLAG(config);
  NOBUG_DECLARE_FLAG(oper);
  NOBUG_DECLARE_FLAG(test);
  NOBUG_DECLARE_FLAG(singleton);
  NOBUG_DECLARE_FLAG(assetmem);
  NOBUG_DECLARE_FLAG(mobjectmem);
  NOBUG_DECLARE_FLAG(buildermem);
  
  
namespace lumiera { 
  void initialize_NoBug ();
  namespace {
    LifecycleHook schedule_ (ON_BASIC_INIT, &initialize_NoBug);         
} }  
#endif /*NOBUGCFG_H    ======= (End) Part 1: DECLARATIONS ======== */






#ifdef NOBUG_INIT_DEFS_ /*========== Part 2: DEFINITIONS ========= */


  /* flags used throughout the code base... */
  NOBUG_CPP_DEFINE_FLAG(config);
  NOBUG_CPP_DEFINE_FLAG(oper);
  NOBUG_CPP_DEFINE_FLAG(test);
  NOBUG_CPP_DEFINE_FLAG_LIMIT(singleton, LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_LIMIT(assetmem,  LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_LIMIT(mobjectmem, LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_LIMIT(buildermem, LOG_INFO);

  


#endif /*NOBUG_INIT_DEFS_ ==== (End) Part 2: DEFINITIONS ========= */
