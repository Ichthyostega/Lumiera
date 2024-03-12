/*
  TEMP-DIR.hpp  -  automatic allocation of a temporary working directory

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file temp-dir.hpp
 ** Manage a temporary directory for storage, with automated clean-up.
 ** @see TempDir_test
 */


#ifndef LIB_TEST_TEMP_DIR_H
#define LIB_TEST_TEMP_DIR_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/random.hpp"
#include "lib/stat/file.hpp"
#include "include/limits.hpp"
#include "lib/format-string.hpp"
//#include <unordered_map>
//#include <iostream>
//#include <vector>
#include <string>
//#include <map>


namespace lib {
namespace test{
  
  namespace error = lumiera::error;
  
  using util::_Fmt;
  
  namespace {
    Literal TEMPFILE_PREFIX = "Lux";
  }
  inline bool
  has_perm (fs::path const& p, fs::perms permissionMask)
  {
    return (fs::status(p).permissions() & permissionMask) == permissionMask;
  }
  inline bool
  can_read (fs::path const& p)
  {
    return has_perm (p, fs::perms::owner_read);
  }
  inline bool
  can_write (fs::path const& p)
  {
    return has_perm (p, fs::perms::owner_write);
  }
  inline bool
  can_exec (fs::path const& p)
  {
    return has_perm (p, fs::perms::owner_exec);
  }
  
  
  /**
   * A RAII style temporary directory.
   */
  class TempDir
    : util::MoveOnly
    {
      fs::path loc_;
      
    public:
      TempDir()
        : loc_{establishNewDirectory()}
        { }
      
     ~TempDir()
        {
          destroyTempDirectory();
        }
      
      
      operator fs::path const& () const
        {
          return loc_;
        }
      
      fs::path
      makeFile()
        {
          UNIMPLEMENTED ("make temporary file");
        }
      
      
    private:
      static fs::path
      establishNewDirectory()
        {
          auto tmpDir = fs::temp_directory_path();
          for (uint attempt=0; attempt<LUMIERA_MAX_COMPETITION; ++attempt)
            {
              auto randName = TEMPFILE_PREFIX + util::showHash (entropyGen.u64());
              auto newPath = tmpDir / randName;
              //  attempt to create it....
              if (fs::create_directory (newPath)
                  and has_perm (newPath, fs::perms::owner_all)
                  and fs::is_empty (newPath)
                 )     // success!
                return newPath;
            }
          throw error::Fatal{_Fmt{"Failed to create unique new TempDir after %d attempts."}
                                 % LUMIERA_MAX_COMPETITION
                            ,error::LUMIERA_ERROR_SAFETY_LIMIT };
        }
      void
      destroyTempDirectory()
        {
          UNIMPLEMENTED ("destroy");
        }
    };
  
  
}} // namespace lib::test
#endif /*LIB_TEST_TEMP_DIR_H*/
