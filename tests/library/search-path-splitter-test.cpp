/*
  SearchPathSplitter(Test)  -  iterating a search path specification

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file search-path-splitter-test.cpp
 ** unit test \ref SearchPathSplitter_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/file.hpp"
#include "lib/util.hpp"

#include "lib/searchpath.hpp"


using util::isnil;

namespace lib {
namespace test {
  
  
  /**************************************************//**
   * @test verify splitting a search path specification
   *       and retrieving the components by iteration.
   *       Embedded \c $ORIGIN tokens get resolved
   *       to the absolute path of this executable.
   */
  class SearchPathSplitter_test : public Test
    {
      void
      run (Arg)
        {
          walkSimplePaths ();
          resolveEmbeddedOriginToken ();
        }
      
      
      void
      walkSimplePaths ()
        {
          walk ("");
          walk (":");
          walk ("a:");
          walk (":a");
          walk ("a:b");
          walk (":a:b\n:c:");
          walk (" d : e f ");
          walk ("/usr/bin:/usr/lib");
          
          SearchPathSplitter sp{};
          CHECK (not sp);
          VERIFY_ERROR (ITER_EXHAUST, *sp );
        }
      
      void
      walk (string spec)
        {
          SearchPathSplitter path{spec};
          for (auto const& pathElm : path)
            cout <<"▶"<< pathElm <<"◀"<< endl;
        }
      
      
      
      void
      resolveEmbeddedOriginToken ()
        {
          fs::path exePath{findExePath()};
          string expected{exePath.parent_path() / "modules"};
          
          string searchSpec = "xyz:$ORIGIN/modules:abc";
          SearchPathSplitter sp{searchSpec};
          CHECK (*sp == "xyz"_expect);
          ++sp;
          CHECK (*sp == ExpectString{expected});
          ++sp;
          CHECK (*sp == "abc"_expect);
          ++sp;
          CHECK (isnil (sp));
        }
    };
  
  
  LAUNCHER (SearchPathSplitter_test, "unit common");
  
  
}} // namespace lib::test
