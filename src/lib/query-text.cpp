/*
  QueryText  -  syntactical standard representation for queries

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file query-text.cpp
 ** Implementation bits regarding a syntactical standard representation of predicate queries
 */


#include "lib/query-text.hpp"

#include <boost/functional/hash.hpp>
#include <string>

using std::string;


namespace lib {
  
  
  /** Parse, verify and normalise the raw query definition
   * @warning right now (2012) we don't normalise at all
   * @todo when integrating a real resolution engine, we
   *       need to parse and verify the given string.
   */
  string
  QueryText::normalise (string const& rawDefinition)
  {
    return rawDefinition;
  }
  
  
  /** support using queries in hashtables. 
   * @warning right now (2012) the dummy implementation of QueryText
   *          doesn't normalise the query in any way, which makes the
   *          generated hash value dependent on the actual textual
   *          form used to build the QueryText. This is not how
   *          it's intended to work, it should rely on a normalised
   *          form after parsing the query definition.
   */
  HashVal
  hash_value (QueryText const& entry)
  {
    return boost::hash_value (entry.definition_);
  }
  

} // namespace lib
