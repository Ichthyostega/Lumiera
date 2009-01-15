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
 */


#ifndef NOBUGCFG_H  /* ============= Part 1: DECLARATIONS ======== */
#define NOBUGCFG_H

#include <syslog.h>
#include <nobug.h>


#ifdef __cplusplus  /* ============= C++ ================ */

#endif /* =====================(End) C++ ================ */




  /* declare flags used throughout the code base */
  NOBUG_DECLARE_FLAG (all);
  NOBUG_DECLARE_FLAG (lumiera_all);
  NOBUG_DECLARE_FLAG (lumiera);        ///< master log, informative console output
  NOBUG_DECLARE_FLAG (operate);        ///< logging channel reporting what the application does
  NOBUG_DECLARE_FLAG (render);         ///< logging channel focusing on the render engine's workings
  NOBUG_DECLARE_FLAG (config);         ///< logging channel covering application and session configuration
  NOBUG_DECLARE_FLAG (memory);         ///< logging channel covering memory management issues
  NOBUG_DECLARE_FLAG (sync);           ///< especially for tracing synchronisation
  NOBUG_DECLARE_FLAG (test);


#endif /*NOBUGCFG_H    ======= (End) Part 1: DECLARATIONS ======== */






#ifdef NOBUG_INIT_DEFS_ /*========== Part 2: DEFINITIONS ========= */


  /* flags used throughout the code base... */
  NOBUG_CPP_DEFINE_FLAG              (all);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (lumiera_all, all);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (lumiera,     lumiera_all);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (config,      lumiera);
  NOBUG_CPP_DEFINE_FLAG_PARENT       (operate,     lumiera);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (render,      lumiera,    LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (memory,      lumiera,    LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (sync,        memory,     LOG_WARNING);
  NOBUG_CPP_DEFINE_FLAG_PARENT_LIMIT (test,        all,        LOG_ERR);




#endif /*NOBUG_INIT_DEFS_ ==== (End) Part 2: DEFINITIONS ========= */
