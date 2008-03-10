/*
  LUMIERA.h  -  global definitions and common types
 
 
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

#ifndef LUMIERA_H
#define LUMIERA_H



#ifdef __cplusplus
extern "C" {
#endif /* ========================== common C Part ============ */


#ifdef __cplusplus
}
#endif /* ==================== (End) common C Part ============ */





#ifndef __cplusplus
#include "nobugcfg.h"

#else  /* ========================== C++-Part ================= */


    /* common types frequently used... */

#include "common/util.hpp"
#include "common/time.hpp"
#include "common/appconfig.hpp" // includes NoBug via "nobugcfg.h"
#include "common/error.hpp"


namespace lumiera
  {
    /* additional global configuration goes here... */
  
    
} // namespace lumiera

#endif /* ===================== (End) C++-Part ================= */

#endif /*LUMIERA_H*/
