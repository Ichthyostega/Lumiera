/*
  TEMP-DIR.hpp  -  automatic allocation of a temporary working directory

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
#include "lib/util.hpp"

#include <fstream>
#include <string>


namespace lib {
namespace test{
  
  namespace error = lumiera::error;
  
  using util::_Fmt;
  using util::isnil;
  using std::string;
  
  namespace {
    Literal TEMPFILE_PREFIX = "Lux";
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
          if (fs::exists (loc_))
            destroyTempDirectory();
        }
      
      
      operator fs::path const& () const
        {
          return loc_;
        }
      
      fs::path
      makeFile (string name ="")
        {
          if (isnil (name))
            return establishNewFile (string{TEMPFILE_PREFIX});

          auto newFile = loc_ / name;
          if (fs::exists (newFile))
            return establishNewFile (name);
          
          std::ofstream{newFile};
          if (fs::exists (newFile) and fs::is_empty(newFile))
            return newFile;
          //
          throw error::Fatal{_Fmt{"Failed to create unique new file %s in TempDir."} % newFile};
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
      
      fs::path
      establishNewFile (string prefix)
        {
          for (uint attempt=0; attempt<LUMIERA_MAX_COMPETITION; ++attempt)
            {
              auto randName = prefix + "." + util::showHash (entropyGen.u64());
              auto newPath = loc_ / randName;
              //  attempt to create it....
              if (fs::exists(newPath))
                continue;
              std::ofstream{newPath};
              if (fs::exists(newPath) and fs::is_empty (newPath))
                return newPath;    // success!
            }
          throw error::Fatal{_Fmt{"Failed to create unique new file at %s after %d attempts."}
                                 % loc_ % LUMIERA_MAX_COMPETITION
                            ,error::LUMIERA_ERROR_SAFETY_LIMIT };
        }
      
      void
      destroyTempDirectory()
        try {
            fs::remove_all (loc_);
            ENSURE (not fs::exists(loc_));
          }
        ERROR_LOG_AND_IGNORE (filesys, "TempDir clean-up")
    };
  
  
}} // namespace lib::test
#endif /*LIB_TEST_TEMP_DIR_H*/
