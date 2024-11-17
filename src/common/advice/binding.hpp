/*
  BINDING.hpp  -  pattern defining a specific attachment to the Advice system

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file advice/binding.hpp
 ** A pattern to define and identify a specific attachment to the Advice system.
 ** This pattern is comprised of a predicate list and intended to be matched or unified
 ** against a similar pattern associated with the attachment of a possible collaboration partner.
 ** Semantically, this list of atoms forms an conjunction of predicates to be resolved against 
 ** similar predicates of the partner. Informally, when two entities attach to the Advice system,
 ** each specifying a binding, they can be paired up if any condition included into the binding
 ** holds true for both sides.
 ** 
 ** Typically, a binding includes a _type-guard predicate_ `advice.type.xx` where `xx` is an
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
 ** # implementation notes
 ** Any binding will be normalised prior to further processing. This normalisation is based
 ** on ordering by predicate symbol and arity. Patterns just comprised of constant symbols
 ** (nullary atoms) can even be condensed into a single hash value, which allows for fast
 ** match checking. For each pattern, we provide a matcher functor, allowing to check
 ** a match against this pattern. In case of the mentioned symbol-only patterns,
 ** this matcher will just hold the hash value of the normalised pattern.
 ** 
 ** The advice system uses a binding index datastructure to keep track of any participating
 ** patterns and especially of the matching pairs. Actually, this datastructure needs to store
 ** only these matcher functors; thus, for a new binding to be used within the advice system,
 ** the symbolic definition is parsed, then normalised and finally, after creating the matcher
 ** functor, the full pattern definition can be discarded.
 ** 
 ** @remarks while binding is defined in the context of the advice system for now,
 **       obviously this is the foundation of a more generic system to deal with predicate terms.
 ** @note as of 4/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces. Ichthyo expects this collaboration service to play a central role
 **       at various places within steam-layer.
 ** @todo for now, \em only the case of a completely constant (ground) pattern is implemented.
 **       Later we may consider to extend the binding patterns to allow variables, which, on match
 **       could be fed as parameters to the bound advice. But this extension requires to extend
 **       the simple hash-based match check to an actual unification of the patterns. ///TICKET #615
 ** 
 ** @see advice.hpp
 ** @see advice-binding-pattern-test.cpp
 ** 
 */


#ifndef LUMIERA_ADVICE_BINDING_H
#define LUMIERA_ADVICE_BINDING_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/hash-value.h"
#include "lib/query-util.hpp"

#include <string>
#include <set>


namespace lumiera{
namespace advice {
  
  using lib::Literal;
  using std::string;
  using lib::HashVal;
  
  LUMIERA_ERROR_DECLARE (BINDING_PATTERN_SYNTAX); ///< Unable to parse the given binding pattern definition
  
  
  
  /**
   * Conjunction of predicates to be matched
   * against a collaboration partner for establishing
   * an Advice connection. The binding is defined by a
   * textual spec in prolog-like syntax. The internal
   * representation is immediately \em normalised.
   * Typically the goal is just to create a #Matcher
   * (Functor) to be stored for later match checks
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
          explicit
          Atom (string const& symbol ="nil", uint arity =0, string const& arg ="")
            : ari_(arity), sym_(symbol), arg_(arg)
            { }
          
          string const& sym()  const { return sym_; }
          string const& arg()  const { return arg_; }
          uint        arity()  const { return ari_; }
          
          operator string()  const;
          
          bool
          identical (Atom const& oa)  const
            {
              return ari_ == oa.ari_
                  && sym_ == oa.sym_
                  && arg_ == oa.arg_;
            }
          
          int
          compare (Atom const& oa)  const               ///< @note when #compare returns 0, the corresponding Atom counts as duplicate
            {
              int res;
              if (0 != (res=sym().compare (oa.sym())))  return res;
              if (0 != (res=arity() - (oa.arity())))    return res;
              return arg().compare (oa.arg());                        /////TODO: in the final version, when we'll allow for variable arguments
            }                                                         /////////: and unification, then variable arguments must not be part of
                                                                      /////////: the comparison, otherwise the matching by hash will break!
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
       * @note Matcher is POD, copyable, no-throw
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
      Binding const& addTypeGuard();
      
      
      Matcher buildMatcher()  const;
      HashVal calculateHash() const;
      
      operator string()  const;
      
      friend bool operator== (Binding const&, Binding const&);
      
      
    private:
      /** internal: parse into atoms, and insert them */
      void parse_and_append (Literal def);
    };
  
  
  template<typename TY>
  inline Binding const&
  Binding::addTypeGuard()
  {
    atoms_.insert (Atom ("advice.type."+lib::query::buildTypeID<TY>()));
    return *this;
  }
  
  
  
  
  /* === equality comparison and matching === */
  
  
  inline bool
  operator!= (Binding const& b1, Binding const& b2)
  {
    return not (b1 == b2);
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
  
  
  
  
  
  
}} // namespace lumiera::advice
#endif
