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

#include <boost/filesystem.hpp>
#include <string>
#include <regex>


namespace lib {
  
  using std::string;
  
  using SubMatch = std::smatch::value_type const&;
  
  namespace error = lumiera::error;
  namespace fsys = boost::filesystem;
  
  using LERR_(ITER_EXHAUST);
  
  
  /** retrieve the location of the executable */
  string findExePath();
  
  /** replace $ORIGIN tokens in the given string
   *  @return copy with expansions applied */
  string replaceMagicLinkerTokens (string const& src);
  
  
  /**
   * Helper: Access a path Specification as a sequence of filesystem Paths.
   * This iterator class dissects a ':'-separated path list. The individual
   * components may use the symbol \c $ORIGIN to refer to the directory
   * holding the current executable.
   * @note #next picks the current component and advances the iteration. 
   */
  class SearchPathSplitter
    : util::NonCopyable
    {
      string pathSpec_;
      std::sregex_iterator pos_,
                           end_;
      
      static const std::regex EXTRACT_PATHSPEC;
      
    public:
      SearchPathSplitter (string const& searchPath)
        : pathSpec_(replaceMagicLinkerTokens (searchPath))
        , pos_(pathSpec_.begin(),pathSpec_.end(), EXTRACT_PATHSPEC)
        , end_()
        { }
      
      explicit operator bool() const { return isValid(); }
      
      bool
      isValid()  const
        {
          return pos_ != end_;
        }
      
      string
      next ()
        {
          if (!isValid())
            throw error::Logic ("Search path exhausted."
                               ,LERR_(ITER_EXHAUST));
          
          string currentPathElement = pos_->str();
          ++pos_;
          return currentPathElement;
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
  string resolveModulePath (fsys::path moduleName, string searchPath = "");
  
  
  
} // namespace lib
#endif
