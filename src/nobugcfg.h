/*
  NOBUGCFG.h  -  global configuration and definitions for NoBug
 
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
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
 ** The idea is that configuration and some globally used flag 
 ** declarations are to be kept in one central location. Normally,
 ** this header will be included by appconfig.hpp, which in turn gets
 ** included by cinelerra.h
 ** @par Besides the usual guarded declarations, this header contains
 ** one section with the corresponding <b>definitions</b>. This section
 ** is to be called by appconfig.cpp only, which deals with application
 ** wide configuration values contained in the Singleton class Appconfig.
 ** Incidentally, the constructor of Appconfig issues the NOBUG_INIT call
 **
 */


#ifndef NOBUGCFG_H  /* ============= Part 1: DECLARATIONS ======== */
#define NOBUGCFG_H

#include <syslog.h>

  /* configuration of NoBug goes here... */

#include <nobug.h>


  /* declare flags used throughout the code base... */
  NOBUG_DECLARE_FLAG(config);
  NOBUG_DECLARE_FLAG(oper);
  NOBUG_DECLARE_FLAG(test);
  NOBUG_DECLARE_FLAG(singleton);
  NOBUG_DECLARE_FLAG(assetmem);
  NOBUG_DECLARE_FLAG(mobjectmem);


#endif /*NOBUGCFG_H    ======= (End) Part 1: DECLARATIONS ======== */






#ifdef NOBUG_INIT_DEFS_ /*========== Part 2: DEFINITIONS ========= */

/* ================================= common C Part ========= */






#ifdef __cplusplus /* ============== C++-Part ============== */


  /* flags used throughout the code base... */
  NOBUG_CPP_DEFINE_FLAG(config);
  NOBUG_CPP_DEFINE_FLAG(oper);
  NOBUG_CPP_DEFINE_FLAG(test);
  NOBUG_CPP_DEFINE_FLAG_LIMIT(singleton, LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_LIMIT(assetmem,  LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_LIMIT(mobjectmem, LOG_WARNING);

#include "common/error.hpp"
  

#endif /* ===================== (End) C++-Part ============= */



#endif /*NOBUG_INIT_DEFS_ ==== (End) Part 2: DEFINITIONS ========= */
