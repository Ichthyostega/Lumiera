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


#include "lib/idi/genfunc.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <string>
#include <boost/functional/hash.hpp>


using util::_Fmt;
using std::string;


namespace lib {
namespace test{ // see test-helper.cpp
    std::string demangleCxx (lib::Literal rawName);
}
namespace idi {
  
  
  namespace format { // generic entry points / integration helpers...
    
    using lib::test::demangleCxx;
    
    string
    demangled_innermost_component (const char* rawName)
    {
      string typeStr = demangleCxx (rawName);
      size_t pos = typeStr.rfind("::");
      if (pos != string::npos)
        typeStr = typeStr.substr(pos+2);
      return typeStr;
    }
    
    string
    demangled_sanitised_name (const char* rawName)
    {
      return util::sanitise (test::demangleCxx (rawName));
    }
    
    
    string
    instance_formatter (string const& prefix, long instanceNr)
    {
      return _Fmt("%s.%03d")
                % prefix % instanceNr; 
    }
    
  } //(End)integration helpers...
  
  
  
  
}} // namespace lib::test
