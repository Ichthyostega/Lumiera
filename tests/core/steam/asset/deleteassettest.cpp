/*
  DeleteAsset(Test)  -  deleting and Asset with all dependencies

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file deleteassettest.cpp
 ** unit test \ref DeleteAsset_test
 */


#include "lib/test/run.hpp"
//#include "lib/util.hpp"


using std::string;


namespace steam {
namespace asset{
namespace test {
  
  
  
  
  /***************************************************************//**
   * @test deleting an Asset includes removing all dependent Assets
   *       and all MObjects relying on these. Especially this means
   *       breaking all links between the involved Objects, so the
   *       shared-ptrs can do the actual cleanup.
   * @see  asset::Asset#unlink
   * @see  mobject::MObject#unlink
   */
  class DeleteAsset_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("delete asset and update all dependencies");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (DeleteAsset_test, "function asset");
  
  
  
}}} // namespace steam::asset::test
