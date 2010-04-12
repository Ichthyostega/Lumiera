/*
  Binding  -  pattern defining a specific attachment to the Advice system
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/


#include "lib/advice/binding.hpp"

//#include <tr1/functional_hash.h>


namespace lib {
namespace advice {

//  using std::tr1::hash;

//  LUMIERA_ERROR_DEFINE (MISSING_INSTANCE, "Existing ID registration without associated instance");

  
  /* ohlolololohaha */
  Binding::Binding ()
  {
    UNIMPLEMENTED ("create a new empty binding");
  }
  
  Binding::Binding (Literal spec)
  {
    UNIMPLEMENTED ("parse the spec and create a new binding");
  }
  
  
  void
  Binding::addPredicate (Literal spec)
  {
    UNIMPLEMENTED ("parse the given spec and create an additional predicte, then re-normalise"); 
  }


  
  Binding::operator string()  const
  {
    UNIMPLEMENTED ("diagnostic string representation of an advice binding");
  }
  
  
  HashVal
  Binding::calculateHash() const
  {
    UNIMPLEMENTED ("calculate the hash for a normalised advice binding");
  }
  
  
  
}} // namespace lib::advice
