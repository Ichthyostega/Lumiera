/*
  Effect(Asset)  -  Effect or media processing component

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file asset/effect.cpp
 ** Implementation level functionality regarding the Asset representation of effects
 ** @todo still unimplemented and stalled work as of 2016
 */


#include "steam/asset/effect.hpp"

namespace proc {
namespace asset {
  
  Proc::ProcFunc*
  Effect::resolveProcessor()  const
  {
    UNIMPLEMENTED ("do the actual resolution plugin -> callable processing function");
  }



}} // namespace asset
