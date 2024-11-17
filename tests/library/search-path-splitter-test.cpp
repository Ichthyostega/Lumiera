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

#include "lib/searchpath.hpp"




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
          walk (":a:b:c:");
          walk (" d : e f");
          walk ("/usr/bin:/usr/lib");
          
          SearchPathSplitter sp("");
          VERIFY_ERROR (ITER_EXHAUST, sp.next() );
        }
      
      void
      walk (string spec)
        {
          SearchPathSplitter path(spec);
          while (path)
            cout << "➢➢" << path.next() << endl;
        }
      
      
      
      void
      resolveEmbeddedOriginToken ()
        {
          fsys::path exePath (findExePath());
          string expected = (exePath.remove_leaf() / "modules").string();
          
          SearchPathSplitter sp("xyz:$ORIGIN/modules:abc");
          CHECK ("xyz" == sp.next());
          CHECK (sp.next() == expected);
          CHECK ("abc" == sp.next());
          CHECK (!sp.isValid());
        }
    };
  
  
  LAUNCHER (SearchPathSplitter_test, "unit common");
  
  
}} // namespace lib::test
