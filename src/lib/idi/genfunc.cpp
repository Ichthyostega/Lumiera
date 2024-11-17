/*
  GenFunc  -  generic identification functions (raw)

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


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
