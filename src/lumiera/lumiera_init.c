/*
  lumiera_init.c  -  Lumiera initialization and shutdowen and global state

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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

//TODO: Support library includes//


//TODO: Lumiera header includes//
#include "include/nobugcfg.hpp"
#include "lumiera/lumiera.h"
#include "lumiera/config_interface.h"

//TODO: internal/static forward declarations//


//TODO: System includes//

/**
 * @file
 *
 */



//code goes here//

/**
 * Early initialization
 * Sets up some important basic stuff, this is run as *very* first before anything
 * else got initialized. Used for NoBug and other kinds of hooks. There is no matching
 * shutdown function for this, consider moving as much as possible to lumiera_init()
 */
void
lumiera_preinit (void)
{
  TRACE (lumiera, "booting up");
}


/**
 * Standard initialization procedure
 * Boots all buildin subsystems up as described in DesignProcess/GlobalInitilaization.
 * The configuration and interfaces/plugin subsystems are already started at this point and
 * commandline arguments are parsed.
 */
void
lumiera_init (void)
{
  TRACE (lumiera, "initializing");

  lumiera_config_interface_init ();
}


/**
 * Shutdown procedure
 * This shuts all subsystem which got initialized in lumiera_init down.
 * Shutdowns should be arranged in reverse initialization order.
 */
void
lumiera_shutdown (void)
{
  TRACE (lumiera, "shutdown");

  lumiera_config_interface_destroy ();
}





/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
