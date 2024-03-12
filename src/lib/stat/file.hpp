/*
  FILE.hpp  -  Filesystem access and helpers

  Copyright (C)         Lumiera.org
    2022,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file file.hpp
 ** Includes the C++ Filesystem library and provides some convenience helpers.
 ** The `std::filesystem` library allows for portable access to file and directory handling
 ** functions; this header maps these functions with an concise `fs::` namespace prefix,
 ** and offers some convenience extensions, which are _„slightly non-portable“_ (meaning they
 ** were developed on Linux and „should“ work on Unix like systems; adapters for exotic operating
 ** systems could be added here when necessary...)
 ** 
 ** @todo 3/2024 should be part of generic utilities
 */



#ifndef LIB_STAT_FILE_H
#define LIB_STAT_FILE_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/meta/util.hpp"

#include <filesystem>
#include <cstdlib>


namespace fs = std::filesystem;
namespace std::filesystem {
  
  extern const string UNIX_HOMEDIR_SYMBOL;
  extern lib::Literal UNIX_HOMEDIR_ENV;
  
  
  inline fs::path
  getHomePath()
  {
    auto home = std::getenv(UNIX_HOMEDIR_ENV);
    if (not home)
        throw lumiera::error::Config{"Program environment doesn't define $HOME (Unix home directory)."};
    return fs::path{home};
  }
  
  
  /** resolves symlinks, `~` (Unix home dir) and relative specs
   * @return absolute canonical form if the path exists;
   *         otherwise only the home directory is expanded */
  inline fs::path
  consolidated (fs::path rawPath)
  {
      if (rawPath.empty())
          return rawPath;
      if (UNIX_HOMEDIR_SYMBOL == *rawPath.begin())
          rawPath = getHomePath() / rawPath.lexically_proximate(UNIX_HOMEDIR_SYMBOL);
  
      return fs::exists(rawPath)? fs::absolute(
                                  fs::canonical(rawPath))
                                : rawPath;
  }
}//(End)namespace fs



namespace util {
  
  /** specialisation: render filesystem path as double quoted string */
  template<>
  struct StringConv<fs::path, void>
    {
      static std::string
      invoke (fs::path path) noexcept
      try {
          return "≺"+std::string{path}+"≻";
        }
      catch(...)
        { return lib::meta::FAILURE_INDICATOR; }
    };
  
}//(End)namespace util
#endif /*LIB_STAT_FILE_H*/
