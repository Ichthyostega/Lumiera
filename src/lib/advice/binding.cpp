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
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

using boost::algorithm::join;
using boost::lexical_cast;

using boost::hash_combine;


namespace lib {
namespace advice {

//  using std::tr1::hash;

  
  
  Binding::Atom::operator string()  const
  {
    return sym_+"/"+lexical_cast<string> (ari_)
               +"("+arg_+")";
  }
  
  
  void
  Binding::parse_and_append (string def)
  {
    UNIMPLEMENTED ("do the actual parsing by regexp, create an Atom for each match");
  }

  
  Binding::Binding ()
    : atoms_()
  { }
  
  Binding::Binding (Literal spec)
  {
    parse_and_append (spec);
  }
  
  
  void
  Binding::addPredicate (Literal spec)
  {
    parse_and_append (spec);
  }


  
  Binding::operator string()  const
  {
    return "Binding[" + join(atoms_, ", ") + "]";
  }
  
  
  HashVal
  Binding::calculateHash() const
  {
    HashVal hash=0;
    
    typedef NormalisedAtoms::const_iterator AIter;
    AIter pos = atoms_.begin();
    AIter end = atoms_.end();
    for ( ; pos!=end ; ++pos)
      {
        hash_combine (hash, pos->sym());
        hash_combine (hash, pos->arity());
        hash_combine (hash, pos->arg());              //////////////TODO: not in final version with variable arguments
      }
    
    return hash;
  }
  
  
  
}} // namespace lib::advice
