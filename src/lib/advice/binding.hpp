/*
  BINDING.hpp  -  pattern defining a specific attachment to the Advice system
 
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
 
*/


/** @file binding.hpp
 ** A pattern to define and identify a specific attachment to the Advice system.
 ** This pattern is comprised of a predicate list and intended to be matched or unified
 ** against a similar pattern associated with the attachment of a possible collaboration partner.
 ** Semantically, this list of atoms forms an conjunction of predicates to be resolved against 
 ** similar predicates of the partner. Informally, when two entities attach to the Advice system,
 ** each specifying a binding, they can be paired up if, when combining, the expressions in their
 ** bindings all evaluate to true.
 ** 
 ** Typically, a binding includes a \em type-guard predicate \c adviceType(xx) where \c xx is an 
 ** identifier denoting a type used within an instantiation of the Advice collaboration, i.e. a type
 ** used as advice value in a instantiation of the PointOfAdvice<AD> template. Besides the type guard,
 ** a binding may narrow down the topic of the advice by providing further predicates. This allows for
 ** Advice collaborations targeted at a more specific topic. The goal and intention behind this Advice
 ** system is to allow collaboration of entities without requiring them to be tightly coupled. Indeed,
 ** the only dependency besides the common type used as advice is to know any specific topic used
 ** within the binding. Thus, and advisor entity could put up a piece of advice, i.e. a value of
 ** the advice type, and another client entity (the advised entity) could pick up this value
 ** without the need to now anything about the advisor.
 ** 
 ** Any binding can be normalised into a hash value, which plays a crucial role within the
 ** implementation of the advice system.
 ** 
 ** TODO WIP-WIP
 ** 
 ** @note as of 4/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces. Ichthyo expects this collaboration service to play a central role
 **       at various places within proc-layer.
 ** 
 ** @see configrules.hpp
 ** @see typed-lookup.cpp corresponding implementation
 ** @see typed-id-test.cpp
 ** 
 */


#ifndef LIB_ADVICE_BINDING_H
#define LIB_ADVICE_BINDING_H


#include "lib/error.hpp"
//#include "proc/asset.hpp"
//#include "proc/asset/struct-scheme.hpp"
//#include "lib/hash-indexed.hpp"
//#include "lib/util.hpp"
#include "lib/symbol.hpp"

//#include <boost/operators.hpp>
//#include <tr1/memory>
#include <iostream>
#include <string>
#include <set>

namespace lib    {
namespace advice {
  
  using std::string;
  
  typedef size_t HashVal;
  
  
  
  /**
   * Conjunction of predicates to be matched
   * against a collaboration partner for establishing
   * an Advice connection.
   * TODO type comment
   */
  class Binding
    {
      
      /** 
       *  single predicate
       *  as part of an advice binding pattern
       */
      class Atom
        {
          uint   ari_;
          string sym_;
          string arg_;
          
        public:
          Atom (uint arity, string const& symbol, string const& arg ="")
            : ari_(arity)
            , sym_(symbol)
            , arg_(arg)
            { }
          
          string const& sym()  const { return sym_; }
          string const& arg()  const { return arg_; }
          uint        arity()  const { return ari_; }
          
          operator string()  const;
          
          int 
          compare (Atom const& oa)  const
            { 
              int res;
              if (0 != (res=sym().compare (oi.sym())))  return res;
              if (0 != (res=arity() - (oi.arity())))    return res;
              return arg().compare (oi.arg());                        /////TODO: in the final version, when we'll allow for variable arguments
            }                                                         /////////  and unification, then variable arguments must not be part of
                                                                      /////////  the comparison, otherwise the matching by hash will break!
          friend bool
          operator< (Atom const& a1, Atom const& a2)
          {
            return a1.compare(a2) < 0; 
          }
        };
      
      
      typedef std::set<Atom> NormalisedAtoms;
      
      NormalisedAtoms atoms_;
      
      
    public:
      /** 
       * Functor object for matching against another Binding.
       * Contains precompiled information necessary for
       * determining a match.
       */
      class Matcher
        {
          HashVal bindingHash_;
          
          
          Matcher (HashVal ha)
            : bindingHash_(ha)
            { }
          
          friend class Binding;
          
          
        public:
          bool matches (Binding const& obi)  const;
          bool matches (Binding::Matcher const& oma)  const;
          
          friend HashVal hash_value (Matcher const&);
        };
      
      
      
      
      /** create the empty binding, equivalent to \c true */
      Binding();
    
      /** create the binding as defined by the given textual definition.
       *  @note implicit type conversion deliberately intended */
      Binding (Literal spec);
      
      /*-- Binding is default copyable --*/
      
      /** extend the definition of this binding
       *  by adding a predicate according to the
       *  given textual definition */
      void addPredicate (Literal spec);
      
      template<typename TY>
      void addTypeGuard();
      
      
      Matcher buildMatcher()  const;
      HashVal calculateHash() const;
      
      operator string()  const;
      
      
    private:
      /** internal: parse into atoms, and insert them */
      void parse_and_append (string def);
    };
  
  
  inline std::ostream&
  operator<< (std::ostream& os, Binding const& bi)
  {
    return os << string(bi); 
  }
      
  template<typename TY>
  inline void
  Binding::addTypeGuard()
  {
    UNIMPLEMENTED ("create a new predicate spec to denote that this binding is related to the given advice type");
  }

  
  
  
  /* === equality comparison and matching === */
  
  /** bindings are considered equivalent if, after normalisation,
   *  their respective definitions are identical.
   *  @note for bindings without variable arguments, equivalence and matching
   *        always yield the same results. Contrary to this, two bindings with
   *        some variable arguments could match, without being defined identically.
   *        For example \c pred(X) matches \c pred(u) or any other binding of the
   *        form \c pred(<constant_value>)
   */
  inline bool
  operator== (Binding const& b1, Binding const& b2)
  {
    UNIMPLEMENTED ("equality of bindings, based on term by term comparison");
  }
  
  inline bool
  operator!= (Binding const& b1, Binding const& b2)
  {
    return ! (b1 == b2);
  }
  
  
  inline bool
  matches (Binding const& b1, Binding const& b2)
  {
    return b1.buildMatcher().matches (b2);
  }
  
  inline bool
  matches (Binding::Matcher const& m1, Binding::Matcher const& m2)
  {
    return m1.matches (m2);
  }
  
  
  inline Binding::Matcher
  Binding::buildMatcher()  const
  {
    return Matcher (this->calculateHash());
  }
  
  
  
  /* == access hash values used for matching == */
  
  inline bool
  Binding::Matcher::matches (Binding const& obi)  const
  {
    return bindingHash_ == obi.calculateHash();
  }
          
  inline bool
  Binding::Matcher::matches (Binding::Matcher const& oma)  const
  {
    return bindingHash_ == hash_value(oma);
  }
          
  inline HashVal
  hash_value (Binding::Matcher const& bm)
  {
    return bm.bindingHash_;
  }
  
  inline HashVal
  hash_value (Binding const& bi)
  {
    return bi.calculateHash();
  }
  
  
  
  
  
  
}} // namespace lib::advice
#endif
