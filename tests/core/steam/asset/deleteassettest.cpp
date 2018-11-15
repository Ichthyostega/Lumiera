/*
  DeleteAsset(Test)  -  deleting and Asset with all dependencies

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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
