/*
  Binding  -  pattern defining a specific attachment to the Advice system

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


/** @file advice/binding.cpp
 ** Implementation of a binding record to represent a match between two patterns.
 ** This is used for the Advice System, to record existing connections between
 ** advice providers and consumers. But as such, Binding is a generic mechanism
 ** and looks like it could be of wider use within the Lumiera application.
 ** This is the reason why Binding got a separate implementation `cpp` file.
 */


#include "lib/util.hpp"
#include "lib/symbol.hpp"
#include "common/advice/binding.hpp"

#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>


using lib::Literal;
using util::isnil;

using std::regex;
using std::smatch;
using std::sregex_iterator;
using std::regex_constants::match_continuous;
using boost::hash_combine;
using boost::lexical_cast;


namespace lumiera{
namespace advice {
  
  LUMIERA_ERROR_DEFINE (BINDING_PATTERN_SYNTAX, "Unable to parse the given binding pattern definition");
  
  
  
  
  
                                                                      /////////////////////TICKET #613 : centralise generally useful RegExps
  namespace{  // Implementation details
    
    const string matchSym = "(\\w+(?:[\\.\\-]\\w+)*)";
    const string matchArg = "\\(\\s*"+matchSym+"?\\s*\\)"; 
    regex findPredicate ("\\s*"+matchSym+"("+matchArg+")?\\s*,?");    ///< \c sym(arg), groups: [symbol, parenthesis, argument symbol]
    
    /** detect the _arity_ of an predicate, as matched by #findPredicate.
     *  Currently, we don't really parse predicate logic notation and thus we
     *  just distinguish nullary predicates (no argument) and predicates with
     *  one single constant argument. */
    inline uint
    detectArity (smatch const& match)
    {
      if (!match[2].matched) return 0;  // no parenthesis at all
      if (!match[3].matched) return 0;  // empty parenthesis
      
      // later we could analyse the argument in detail here...
      return 1;  // but now we just accept a single constant symbol
    }
  }
  
  
  void
  Binding::parse_and_append (Literal lit)
  {      
    string def(lit);
    string::const_iterator end_of_last_match = def.begin();
    
    sregex_iterator end;
    sregex_iterator pos (def.begin(),def.end(), findPredicate, 
                                                match_continuous);    // continuous: don't allow garbage *not* matched by the RegExp
    while (pos != end)
      {
        smatch match = *pos;
        atoms_.insert (Atom (match[1], detectArity(match), match[3]));
        end_of_last_match = match[0].second;
        ++pos;
      }
    
    if (  end_of_last_match !=def.end()
       && *end_of_last_match !='.'
       ) // if the match did *not stop at the end of the pattern definition list 
      throw lumiera::error::Invalid ("Trailing garbage in binding pattern definition"                 ///////////////TICKET #197  should include the garbage, i.e. where the parsing stops
                                    , LUMIERA_ERROR_BINDING_PATTERN_SYNTAX);
  }
  
  
  Binding::Binding ()
    : atoms_()
  { }
  
  Binding::Binding (Literal spec)
  {
    if (!isnil(spec))
      parse_and_append (spec);
  }
  
  
  void
  Binding::addPredicate (Literal spec)
  {
    REQUIRE (spec);
    parse_and_append (spec);
  }
  
  
  
  Binding::operator string()  const
  {
    string repr("Binding[");
    typedef NormalisedAtoms::const_iterator AIter;
    AIter end = atoms_.end();
    AIter pos = atoms_.begin();
    for ( ; pos!=end ; ++pos)
      repr += string(*pos)+", ";
    
    if (0 < atoms_.size())
      repr.resize(repr.size()-2);
    
    repr += "]";
    return repr;
  }
  
  
  Binding::Atom::operator string()  const
  {
    return sym_+"/"+lexical_cast<string> (ari_)
               +"("+arg_+")";
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
        hash_combine (hash, pos->arg());              //////////////TICKET #615 : not in final version with variable arguments
      }
    
    return hash;
  }
  
  
  /** bindings are considered equivalent if,
   *  after normalisation, their respective definitions are identical.
   *  @note for bindings without variable arguments, equivalence and matching
   *        always yield the same results. Contrary to this, two bindings with
   *        some variable arguments could match, without being defined identically.
   *        For example \c pred(X) matches \c pred(u) or any other binding of the
   *        form \c pred(<constant_value>)  ////TICKET #615 not yet implemented
   */
  bool
  operator== (Binding const& b1, Binding const& b2)
  {
    if (b1.atoms_.size() != b2.atoms_.size())
      return false;
    
    ASSERT (b1.atoms_.size() == b2.atoms_.size());
    
    typedef Binding::NormalisedAtoms::const_iterator Iter;
    Iter end = b1.atoms_.end();
    Iter p1 = b1.atoms_.begin();
    Iter p2 = b2.atoms_.begin();
    
    for ( ; p1!=end;  ++p1, ++p2 )
      if (! p1->identical(*p2))
        return false;
    
    return true;
  }
  
  
  
}} // namespace lumiera::advice
