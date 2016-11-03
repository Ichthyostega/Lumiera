/*
  SEARCHPATH.hpp  -  helpers for searching directory lists and locating modules

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

*/


/** @file searchpath.hpp
 ** TODO searchpath.hpp
 */


#ifndef COMMON_SEARCHPATH_H
#define COMMON_SEARCHPATH_H

#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"

#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <string>


namespace lib {
  
  using std::string;
  using boost::regex;
  using boost::smatch;
  using boost::regex_search;
  using boost::sregex_iterator;
  
  typedef smatch::value_type const& SubMatch;
  
  namespace error = lumiera::error;
  namespace fsys = boost::filesystem;
  
  LUMIERA_ERROR_DECLARE (FILE_NOT_DIRECTORY); ///< path element points at a file instead of a directory
  using error::LUMIERA_ERROR_ITER_EXHAUST;
  
  
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
    : public BoolCheckable<SearchPathSplitter
    , boost::noncopyable>
    {
      string pathSpec_;
      sregex_iterator pos_,
                      end_;
      
      static const regex EXTRACT_PATHSPEC;
      
    public:
      SearchPathSplitter (string const& searchPath)
        : pathSpec_(replaceMagicLinkerTokens (searchPath))
        , pos_(pathSpec_.begin(),pathSpec_.end(), EXTRACT_PATHSPEC)
        , end_()
        { }
      
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
                               ,LUMIERA_ERROR_ITER_EXHAUST);
          
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
