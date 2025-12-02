/*
  TempDir(Test)  -  verify automated temporary working directory

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file temp-dir-test.cpp
 ** unit test \ref TempDir_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/temp-dir.hpp"

#include <fstream>


namespace lib {
namespace test{
namespace test{
  
  
  /***************************************************************//**
   * @test validate proper behaviour of a temporary working directory,
   *       including automatic name allocation and clean-up.
   * @see temp-dir.hpp
   * @see DataCSV_test usage example
   */
  class TempDir_test : public Test
    {
      void
      run (Arg)
        {
          simpleUsage();
          verify_Lifecycle();
        }
      
      
      void
      simpleUsage()
        {
          TempDir temp;
          auto ff = temp.makeFile();
          CHECK (fs::exists (ff));
          CHECK (fs::is_empty (ff));
          
          std::ofstream out{ff, std::ios_base::out};
          auto scree = randStr(55);
          out << scree << std::endl;
          out.close();
          
          CHECK (fs::is_regular_file (ff));
          CHECK (not fs::is_empty (ff));
          
          std::ifstream in{ff};
          string readBack;
          in >> readBack;
          CHECK (readBack == scree);
        }
      
      
      
      /** @test automatic clean-up even in case of errors. */
      void
      verify_Lifecycle ()
        {
          fs::path d1;
          fs::path d2;
          {
            TempDir tt;
            d1 = tt;
            tt.makeFile("huibuh");
            tt.makeFile("huibuh");
            tt.makeFile("huibuh");
            std::ofstream boo{d1 / "huibuh"};
            boo << "boo";
            fs::create_directories(d1 / "bug/bear");
            fs::rename (d1 / "huibuh", d1 / "bug/bear/fray");
            
            auto scare = [&]{
                              TempDir tt;
                              d2 = tt;
                              tt.makeFile("Mooo");
                              CHECK (fs::exists(d2 / "Mooo"));
                              CHECK (not fs::is_empty(d2));
                              fs::create_directory(d2 / "Mooo"); // Booom!
                            };
            CHECK (d2.empty());
            CHECK (not d1.empty());
            
            VERIFY_FAIL ("File exists", scare() );
            // nested context was cleaned-up after exception
            CHECK (not fs::exists(d2));
            CHECK (    fs::exists(d1));
            CHECK (not d2.empty());
            CHECK (d1 != d2);
            
            boo << "moo";
            boo.close();
            CHECK (6 ==  fs::file_size(d1 / "bug/bear/fray"));
            // so bottom line: can do filesystem stuff for real...
          }
          // All traces are gone...
          CHECK (not fs::exists(d1));
          CHECK (not fs::exists(d2));
        }
    };
  
  LAUNCHER (TempDir_test, "unit common");
  
  
}}} // namespace lib::test::test
