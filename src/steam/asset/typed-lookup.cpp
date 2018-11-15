/*
  TypedLookup  -  implementing a registration service for ID to type association

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file typed-lookup.cpp
 ** Implementation backend for a registration service to associate IDs, types and object instances
 ** @todo this is placeholder code for an important facility, required once we engage into
 **       predicate rules based configuration. For now (as of 2016), efforts towards this important
 **       goal have been postponed, because we have to focus on a working engine first
 */


#include "steam/asset/typed-id.hpp"


namespace proc {
namespace asset {
  
  LUMIERA_ERROR_DEFINE (MISSING_INSTANCE, "Existing ID registration without associated instance");

  
  /* ohlolololohaha */
  
  
  
}} // namespace asset
