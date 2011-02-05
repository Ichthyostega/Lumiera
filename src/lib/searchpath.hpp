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


#ifndef COMMON_SEARCHPATH_H
#define COMMON_SEARCHPATH_H

#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"

#include <boost/program_options.hpp>
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
  
  
  /**
   * Helper: Access a path Specification as a sequence of filesystem Paths.
   * This iterator class dissects a ':'-separated path list. The individual
   * components may use the symbol \c $ORIGIN to denote the directory holding
   * the current executable. After resolving this symbol, a valid absolute or
   * relative filesystem path should result, which must not denote an existing
   * file (directory is OK).
   * @note #fetch picks the current component and advances the iteration. 
   */
  class SearchPathSplitter
    : public BoolCheckable<SearchPathSplitter
      ,                    boost::noncopyable>
    {
      string pathSpec_;
      sregex_iterator pos_,
                      end_;
      
      static regex EXTRACT_PATHSPEC;
      
    public:
      SearchPathSplitter (string const& searchPath)
        : pathSpec_(searchPath)
        , pos_(pathSpec_.begin(),pathSpec_.end(), EXTRACT_PATHSPEC)
        , end_()
        { }
      
      bool
      isValid()  const
        {
          return pos_ != end_;
        }
      
      string
      fetch ()
        {
          if (!isValid())
            throw error::Logic ("Search path exhausted."
                               ,LUMIERA_ERROR_ITER_EXHAUST);
          
          string currentPathElement = resolveRelative();
          ++pos_;
          return currentPathElement;
        }
      
    private:
      /** maybe resolve a path spec given relative to
       *  the current Executable location ($ORIGIN) */
      string
      resolveRelative ()
        {
          if (containsORIGINToken())
            return asAbsolutePath();
          else
            return getFullPath();
        }
      
      SubMatch found(int group=0) { return (*pos_)[group]; }
      
      bool containsORIGINToken() { return found(1).matched; }
      string getRelativePath()  { return found(2);  }
      string getFullPath()     { return found(); }
      
      string
      asAbsolutePath()
        {
          fsys::path exePathName (findExePath());
          fsys::path modPathName (exePathName.remove_leaf() / getRelativePath());
          
          if (fsys::exists(modPathName) && !fsys::is_directory (modPathName))
            throw error::Invalid ("Error in search path: component \""+modPathName.string()+"\" is not a directory"
                                 ,LUMIERA_ERROR_FILE_NOT_DIRECTORY);
          
          return modPathName.directory_string();
        }
    };
  
  
  
  /** helper to establish the location to search for loadable modules.
   *  This is a simple demonstration of the basic technique used in the
   *  real application source to establish a plugin search path, based
   *  on the actual executable position plus compiled in and configured
   *  relative and absolute path specifications.
   */
  string resolveModulePath (string moduleName, string searchPath = "");
  
  
  
} // namespace lib
#endif
