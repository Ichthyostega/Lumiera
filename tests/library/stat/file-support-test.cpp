/*
  FileSupport(Test)  -  verify additional filesystem helpers

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

* *****************************************************/

/** @file file-support-test.cpp
 ** unit test \ref FileSupport_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/temp-dir.hpp"
#include "lib/stat/file.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/error.hpp"
//#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"

//#include <boost/algorithm/string.hpp>
#include <fstream>
//#include <functional>
//#include <string>

//using util::for_each;
//using lumiera::Error;
//using lumiera::LUMIERA_ERROR_EXCEPTION;
//using lumiera::error::LUMIERA_ERROR_ASSERTION;
//using lib::time::TimeVar;
//using lib::time::Time;

//using boost::algorithm::is_lower;
//using boost::algorithm::is_digit;
//using std::function;
//using std::string;


namespace lib {
namespace stat{
namespace test{
  
  using lib::test::TempDir;
  
  
  
  
  /***************************************************************//**
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
SHOW_EXPR(temp)
SHOW_EXPR(fs::path{temp})
          fs::path f = temp.makeFile("Lumiera.nix");
SHOW_EXPR(f);
SHOW_EXPR(fs::exists(f));
          std::ofstream out{f};
SHOW_EXPR(fs::exists(f));
SHOW_EXPR(fs::status(f).permissions())
SHOW_EXPR(fs::has_perm(temp, fs::perms::owner_read));
SHOW_EXPR(fs::has_perm(temp, fs::perms::owner_write));
SHOW_EXPR(fs::has_perm(temp, fs::perms::owner_exec));
SHOW_EXPR(fs::has_perm(temp, fs::perms::owner_all));
SHOW_EXPR(fs::has_perm(temp, fs::perms::group_read));
SHOW_EXPR(fs::has_perm(temp, fs::perms::group_write));
SHOW_EXPR(fs::has_perm(temp, fs::perms::group_exec));
SHOW_EXPR(fs::has_perm(temp, fs::perms::group_all));
SHOW_EXPR(fs::has_perm(temp, fs::perms::others_read));
SHOW_EXPR(fs::has_perm(temp, fs::perms::others_write));
SHOW_EXPR(fs::has_perm(temp, fs::perms::others_exec));
SHOW_EXPR(fs::has_perm(temp, fs::perms::others_all));
SHOW_EXPR(fs::has_perm(temp, fs::perms::all));
SHOW_EXPR(fs::can_read(temp));
SHOW_EXPR(fs::can_write(temp));
SHOW_EXPR(fs::can_exec(temp));
        }
      
      
      
      /** @test prints "sizeof()" including some type name. */
      void
      homedirectoryExpansion ()
        {
        }
    };
  
  LAUNCHER (FileSupport_test, "unit common");
  
  
}}} // namespace lib::stat::test

