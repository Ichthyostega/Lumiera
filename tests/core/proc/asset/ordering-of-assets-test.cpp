/*
  OrderingOfAssets(Test)  -  equality and comparisons

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

/** @file ordering-of-assets-test.cpp
 ** unit test \ref OrderingOfAssets_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/proc.hpp"

#include "proc/asset/asset-diagnostics.hpp"
#include "backend/media-access-mock.hpp"
#include "lib/depend-inject.hpp"

using lib::test::Depend4Test;
using util::isnil;
using std::string;


namespace proc {
namespace asset{
namespace test {
  
  
  
  
  /**************************************************//**
   * @test validate the equality and order relations of 
   *       Asset::Ident and Asset objects.
   * @note a known problem is that only Asset smart ptrs
   *       are supported for comparison, not smartpointers
   *       of Asset subclasses. To solve this, we would
   *       either have to repeat the operator definitions,
   *       or resort to template metaprogramming tricks.
   *       Just providing templated comparison operators
   *       would generally override the behaviour of 
   *       std::shared_ptr, which is not desirable. 
   * @see  Asset::Ident#compare
   */
  class OrderingOfAssets_test : public Test
    {
      virtual void run(Arg) 
        {
          Depend4Test<backend::test::MediaAccessMock> within_this_scope;
          
          
          Asset::Ident key1("test-1", Category(AUDIO), "ichthyo", 5);
          PAsset mm1 = asset::Media::create(key1, "Name-1");
          
          Asset::Ident key2("test-1", Category(AUDIO), "ichthyo", 7);
          PAsset mm2 = asset::Media::create(key2, "Name-2");
          
          Asset::Ident key3("test-2", Category(AUDIO), "ichthyo", 5);
          PAsset mm3 = asset::Media::create(key3, "Name-3");
          
          Asset::Ident key4("test-2", Category(AUDIO), "stega", 5);
          PAsset mm4 = asset::Media::create(key4, "Name-4");
          
          Asset::Ident key5("test-1", Category(VIDEO), "ichthyo", 5);
          PAsset mm5 = asset::Media::create(key5, "Name-5");
          
          
          // ordering of keys
          CHECK (key1 == key2);
          CHECK (key2 != key3);
          CHECK (key3 != key4);
          CHECK (key4 != key5);
          CHECK (key1 != key5);
          
          CHECK ( 0 > key2.compare(key3));
          CHECK ( 0 < key3.compare(key2));
          
          CHECK ( 0 > key3.compare(key4));
          CHECK ( 0 > key4.compare(key5));
          CHECK ( 0 > key1.compare(key5));
          CHECK ( 0 > key2.compare(key5));
          CHECK ( 0 > key3.compare(key5));
          CHECK ( 0 > key1.compare(key3));
          CHECK ( 0 > key1.compare(key4));
          CHECK ( 0 > key2.compare(key4));
          
          
          // ordering of Asset smart ptrs
          CHECK (mm1 == mm2);
          CHECK (mm2 != mm3);
          CHECK (mm3 != mm4);
          CHECK (mm4 != mm5);
          CHECK (mm1 != mm5);
          
          CHECK (mm2 < mm3);
          CHECK (mm2 <= mm3);
          CHECK (mm3 > mm2);
          CHECK (mm3 >= mm2);
          
          CHECK (mm3 < mm4);
          CHECK (mm4 < mm5);
          CHECK (mm1 < mm5);
          CHECK (mm2 < mm5);
          CHECK (mm3 < mm5);
          CHECK (mm1 < mm3);
          CHECK (mm1 < mm4);
          CHECK (mm2 < mm4);
          
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OrderingOfAssets_test, "unit asset");
  
  
  
}}} // namespace proc::asset::test
