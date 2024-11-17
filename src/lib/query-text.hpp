/*
  QUERY-TEXT.hpp  -  syntactical standard representation for queries

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file query-text.hpp
 ** A generic syntactical representation for all kinds of queries.
 ** Within Lumiera, its a common pattern to query for parts to be combined,
 ** instead of using a hard wired builder logic. Consequently, there are various
 ** flavours of queries used by different subsystems. As a common denominator,
 ** we use a syntactical query representation, based on predicate notation
 ** (mathematical logic, using prolog syntax). While subsystems typically
 ** might resolve a specialised query directly, as a fallback this
 ** syntactical representation allows for \em generic query dispatch.
 ** And it can be used as intermediary format for remolding queries.
 ** 
 ** @note as of 12/2012 this AST-representation is not defined at all;
 **       instead we use a plain string as placeholder for the "real thing"
 ** @todo actually build the term representation      /////////////////////////////TICKET #899
 ** 
 ** @see QueryText_test usage example
 ** 
 */


#ifndef LIB_QUERY_TEXT_H
#define LIB_QUERY_TEXT_H

#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "lib/query-util.hpp"
#include "lib/util.hpp"

#include <string>

namespace lib {
  
  using lib::HashVal;
  using std::string;
  
  /**
   * Syntactical query representation.
   * Used as a backbone to allow for generic queries and to
   * enable programmatically rebuilding and remolding of queries.
   * 
   * @todo this is placeholder code and just embeds a string
   *       with the raw query definition, instead of parsing
   *       the definition and transforming it into an AST
   * 
   * @see Query
   */
  class QueryText
    {
      string definition_;
      
      
    public:
      QueryText() { }
      
      QueryText (string const& syntacticRepr)
        : definition_ (normalise (syntacticRepr))
        { }
      
      // using default copy/assignment
      
      operator string()  const
        {
          return definition_;
        }
      
      
      bool
      empty()  const
        {
          return definition_.empty();
        }
      
      bool
      hasAtom (string const& predSymbol)
        {
          return util::contains (definition_, predSymbol);
        }
      
      /** synthetic total order to classify query definitions.
       *  Queries with more specific conditions should yield larger values.
       * @warning this is rather a design idea and it's not clear
       *          if this metric can be made to work in practice
       * @todo using a rather deaf placeholder implementation
       *       based just on counting the top level predicates.
       */
      uint
      degree_of_constriction()  const
        {
          return query::countPred (definition_);
        }
      
    private:
      string normalise (string const& rawDefinition);
      
            
      friend bool
      operator== (QueryText const& q1, QueryText const& q2)
      {
        return q1.definition_ == q2.definition_;
      }
      friend bool
      operator<  (QueryText const& q1, QueryText const& q2)
      {
        return q1.definition_ < q2.definition_;
      }
      
      friend HashVal hash_value (QueryText const& entry);
    };
  
  
} // namespace lib
#endif /*LIB_QUERY_TEXT_H*/
