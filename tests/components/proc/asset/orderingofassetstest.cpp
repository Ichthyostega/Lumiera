/*
  OrderingOfAssets(Test)  -  equality and comparisons
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "common/test/run.hpp"
#include "common/util.hpp"

#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/proc.hpp"

#include "proc/asset/assetdiagnostics.hpp"

using util::isnil;
using std::string;


namespace asset
  {
  namespace test
    {
    
    
    
    
    /******************************************************
     * @test validate the equality and order relations of 
     *       Asset::Ident and Asset objects.
     * @note a known problem is that only Asset smart ptrs
     *       are supported for comparison, not smartptrs
     *       of Asset subclasses. To solve this, we would
     *       either have to repeat the operator definitions,
     *       or resort to template metaprogramming tricks.
     *       Just providing templated comparison operators
     *       would generally override the behaviour of 
     *       boost::shared_ptr, which is not desirable. 
     * @see  Asset::Ident#compare
     */
    class OrderingOfAssets_test : public Test
      {
        virtual void run(Arg arg) 
          {
            typedef shared_ptr<Asset> PA;
            
            Asset::Ident key1("Au-1", Category(AUDIO), "ichthyo", 5);
            PA mm1 = asset::Media::create(key1, "Name-1");
            
            Asset::Ident key2("Au-1", Category(AUDIO), "ichthyo", 7);
            PA mm2 = asset::Media::create(key2, "Name-2");
            
            Asset::Ident key3("Au-2", Category(AUDIO), "ichthyo", 5);
            PA mm3 = asset::Media::create(key3, "Name-3");
            
            Asset::Ident key4("Au-2", Category(AUDIO), "stega", 5);
            PA mm4 = asset::Media::create(key4, "Name-4");
            
            Asset::Ident key5("Au-1", Category(VIDEO), "ichthyo", 5);
            PA mm5 = asset::Media::create(key5, "Name-5");
            
            
            // ordering of keys
            ASSERT (key1 == key2);
            ASSERT (key2 != key3);
            ASSERT (key3 != key4);
            ASSERT (key4 != key5);
            ASSERT (key1 != key5);

            ASSERT ( 0 > key2.compare(key3));
            ASSERT ( 0 < key3.compare(key2));

            ASSERT ( 0 > key3.compare(key4));
            ASSERT ( 0 > key4.compare(key5));
            ASSERT ( 0 > key1.compare(key5));
            ASSERT ( 0 > key2.compare(key5));
            ASSERT ( 0 > key3.compare(key5));
            ASSERT ( 0 > key1.compare(key3));
            ASSERT ( 0 > key1.compare(key4));
            ASSERT ( 0 > key2.compare(key4));
            
            
            // ordering of Asset smart ptrs
            ASSERT (mm1 == mm2);
            ASSERT (mm2 != mm3);
            ASSERT (mm3 != mm4);
            ASSERT (mm4 != mm5);
            ASSERT (mm1 != mm5);

            ASSERT (mm2 < mm3);
            ASSERT (mm2 <= mm3);
            ASSERT (mm3 > mm2);
            ASSERT (mm3 >= mm2);

            ASSERT (mm3 < mm4);
            ASSERT (mm4 < mm5);
            ASSERT (mm1 < mm5);
            ASSERT (mm2 < mm5);
            ASSERT (mm3 < mm5);
            ASSERT (mm1 < mm3);
            ASSERT (mm1 < mm4);
            ASSERT (mm2 < mm4);

          }
      };
    
    
    /** Register this test class... */
    LAUNCHER (OrderingOfAssets_test, "unit asset");
    
    
    
  } // namespace test

} // namespace asset
