/*
  SEARCHPATH.hpp  -  helpers for searching directory lists and locating modules

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file searchpath.hpp
 ** Helpers to handle directory search paths.
 ** The SerachPathSplitter allows to evaluate a "path" like specification
 ** with colon separated components. It is complemented by some magic convenience
 ** functions to self-discover the currently running executable and to resolve
 ** the `$ORIGIN` pattern similar to what is known from linker `rpath` / `runpath`
 */


#ifndef COMMON_SEARCHPATH_H
#define COMMON_SEARCHPATH_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/regex.hpp"
#include "lib/file.hpp"

#include <string>
#include <boost/algorithm/string.hpp>


namespace lib {
  namespace error = lumiera::error;
  
  using std::string;
  
  using SubMatch = std::smatch::value_type const&;
  
  
  using LERR_(ITER_EXHAUST);
  
  
  /** retrieve the location of the executable */
  fs::path findExePath();
  
  /** replace $ORIGIN tokens in the given string
   *  @return copy with expansions applied */
  string replaceMagicLinkerTokens (string const& src);
  
  
  /**
   * Helper: Access a path Specification as a sequence of filesystem Paths.
   * This iterator class dissects a ':'-separated path list. The individual
   * components may use the symbol \c $ORIGIN to refer to the directory
   * holding the current executable.
   */
  class SearchPathSplitter
    : public util::RegexSearchIter
    {
      static const regex ACCEPT_PATHELEMENT;
      
    public:
      SearchPathSplitter() = default;
      SearchPathSplitter (string& searchPath)         ///< @warning search path string must exist somewhere else
        : RegexSearchIter{searchPath, ACCEPT_PATHELEMENT}
        { }
      
      LIFT_PARENT_INCREMENT_OPERATOR (std::sregex_iterator);
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (SearchPathSplitter);
      
      using value_type = std::string;
      using reference = value_type&;
      using pointer =   value_type*;
      
      string
      operator*()  const
        {
          if (!isValid())
            throw error::Logic ("Search path exhausted."
                               ,LERR_(ITER_EXHAUST));
          
          string pathElm = util::RegexSearchIter::operator*()[2];
          pathElm = boost::algorithm::trim_right_copy(pathElm);
          return replaceMagicLinkerTokens (pathElm);
        }
    };
  
  
  
  /** helper to establish the location to search for loadable modules,
   *  configuration files, icons and further resources. After first trying
   *  the moduleName directly, the given search path is walked using the
   *  SearchPathSplitter, until encountering an existing file with the
   *  given name.
   *  @return the absolute pathname of the module file found
   *  @throws error::Config when the resolution fails
   */
  string resolveModulePath (fs::path moduleName, string searchPath = "");
  
  
  
} // namespace lib
#endif
