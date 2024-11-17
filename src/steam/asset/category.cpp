/*
  Category  -  tree like classification of Assets

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file category.cpp
 ** Implementation parts regarding Asset categorisation
 */


#include "steam/asset/category.hpp"
#include "lib/util.hpp"
#include "include/logging.h"

#include <boost/algorithm/string.hpp>

using boost::algorithm::starts_with;
using util::isnil;

namespace steam {
namespace asset {
  
  /** human readable representation of the asset::Category.
   *  @todo to be localized.
   */
  Category::operator string ()  const
  {
    typedef const char * const SymID;  

    SymID kinds[6] = { "AUDIO"
                     , "VIDEO"
                     , "EFFECT"
                     , "CODEC"
                     , "STRUCT"
                     , "META"
                     };
    REQUIRE ( 0<=kind_ && kind_< 6 );
    string str (kinds[kind_]);
    if (!isnil (path_))
      str += "/"+path_;
    return str;
  }
  
  
  
  /** hierarchical inclusion test. 
   *  @return true if \c this can be considered 
   *          a subcategory of the given reference
   */
  bool 
  Category::isWithin (const Category& ref) const
  {
    return ( ref.hasKind (kind_)
          && starts_with (path_, ref.path_) 
           );
  }


}} // namespace asset
