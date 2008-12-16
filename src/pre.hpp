/*
  PRE.hpp  -  precompiled header collection
 
 
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

/** @file pre.hpp
 ** Precompiled Header Collection.
 ** Assortment of standard util, error handling and basic lib-boost components,
 ** frequently used in conjunction. Precompiling these can speedup building
 ** significantly. When used, this header should be included prior to any other
 ** headers (and it needs to be compiled by gcc into a "pre.gch" file prior to
 ** building the object files including this header).
 **
 ** @see mobject.hpp usage example
 ** @see pre_a.hpp precompiled header including Asset subsystem
 */

#ifndef LUMIERA_PRE_HPP
#define LUMIERA_PRE_HPP

#include <string>
#include <vector>
#include <map>
#include <tr1/memory>
#include <boost/function.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>

#include "proc/common.hpp"



#endif /*LUMIERA_PRE_HPP*/
