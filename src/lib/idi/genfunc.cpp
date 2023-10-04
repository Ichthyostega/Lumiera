/*
  GenFunc  -  generic identification functions (raw)

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file genfunc.cpp
 ** Implementation of generic functions to build identification schemes.
 */


#include "lib/idi/genfunc.hpp"
#include "lib/format-string.hpp"
#include "lib/format-obj.hpp"
#include "lib/util.hpp"

#include <string>
#include <boost/functional/hash.hpp>


using util::_Fmt;
using std::string;


namespace lib {
namespace idi {
  
  
  namespace format { // generic entry points / integration helpers...
    
    string
    instance_format (string const& prefix, size_t instanceNr)
    {
      return _Fmt("%s.%03d")
                % prefix % instanceNr;
    }
    
    string
    instance_hex_format (string const& prefix, size_t instanceNr)
    {
      return _Fmt("%s.%04X")
                % prefix % instanceNr;
    }
    
  } //(End)integration helpers...
  
  
  TypedCounter&
  sharedInstanceCounter()
  {        // Meyer's Singleton
    static TypedCounter instanceCounter;
    return instanceCounter;
  }
  
}} // namespace lib::idi
