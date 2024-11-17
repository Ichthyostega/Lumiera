/*
  FileSupport(Test)  -  verify additional filesystem helpers

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file file-support-test.cpp
 ** unit test \ref FileSupport_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/temp-dir.hpp"
#include "lib/stat/file.hpp"

#include <fstream>


namespace lib {
namespace stat{
namespace test{
  
  using lib::test::TempDir;
  
  
  
  
  /********************************************************************//**
   * @test verify supplemental helper functions for file-handling support,
   *       provided to complement the C++ `<filesystem>` library.
   * @see file.hpp
   * @see TempDir_test
   */
  class FileSupport_test : public Test
    {
      void
      run (Arg)
        {
          simplifiedPermissionAccess();
          homedirectoryExpansion();
        }
      
      
      void
      simplifiedPermissionAccess()
        {
          TempDir temp;
          fs::path f = temp.makeFile("Lumiera.nix");
          CHECK (fs::exists(f));
          CHECK (f.filename() == "Lumiera.nix");
          CHECK (f.parent_path() == temp);
          
          // enforce specific permissions...
          fs::permissions(f, fs::perms::owner_read | fs::perms::group_all | fs::perms::others_exec);
          
          CHECK (    fs::has_perm(f, fs::perms::owner_read));
          CHECK (not fs::has_perm(f, fs::perms::owner_write));
          CHECK (not fs::has_perm(f, fs::perms::owner_exec));
          CHECK (not fs::has_perm(f, fs::perms::owner_all));
          CHECK (    fs::has_perm(f, fs::perms::group_read));
          CHECK (    fs::has_perm(f, fs::perms::group_write));
          CHECK (    fs::has_perm(f, fs::perms::group_exec));
          CHECK (    fs::has_perm(f, fs::perms::group_all));
          CHECK (not fs::has_perm(f, fs::perms::others_read));
          CHECK (not fs::has_perm(f, fs::perms::others_write));
          CHECK (    fs::has_perm(f, fs::perms::others_exec));
          CHECK (not fs::has_perm(f, fs::perms::others_all));
          CHECK (not fs::has_perm(f, fs::perms::all));
          CHECK (    fs::can_read(f));
          CHECK (not fs::can_write(f));
          CHECK (not fs::can_exec(f));
          
          // and indeed: we can not write
          std::ofstream out{f};
          out << "outch";
          out.close();
          CHECK (not out.good());
          CHECK (0 == fs::file_size(f));
        }
      
      
      
      /** @test verify _consolidated path_
       *      - retrieves and expands the POSIX home directory
       *      - transforms into canonical, absolute path
       */
      void
      homedirectoryExpansion ()
        {
          fs::path sweetHome{"~"};
          CHECK ("~" == sweetHome.generic_string());
          CHECK (not sweetHome.empty());
          CHECK (not sweetHome.has_parent_path());
          CHECK (not sweetHome.is_absolute());
          
          sweetHome = fs::consolidated (sweetHome);
          CHECK (not util::startsWith (sweetHome.generic_string(), "~"));
          CHECK (    util::startsWith (sweetHome.generic_string(), "/"));
          CHECK (not sweetHome.empty());
          CHECK (    sweetHome.has_parent_path());
          CHECK (    sweetHome.is_absolute());
          CHECK (fs::is_directory(sweetHome));

          fs::path itFollows = fs::consolidated ("~/it/follows");
          CHECK (util::startsWith (itFollows.generic_string(), "/"));
          CHECK (util::endsWith (itFollows.generic_string(), "follows"));
          CHECK (itFollows.filename() == "follows");
          CHECK (itFollows.is_absolute());
          
          CHECK (fs::relative (itFollows, sweetHome) == "it/follows");
        }
    };
  
  LAUNCHER (FileSupport_test, "unit common");
  
  
}}} // namespace lib::stat::test

