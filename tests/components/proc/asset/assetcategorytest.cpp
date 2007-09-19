/*
  AssetCategory(Test)  -  verifying Asset category tuple functions
 
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

#include "proc/asset/category.hpp"

#include <boost/format.hpp>
#include <iostream>

using boost::format;
using util::isnil;
using std::string;
using std::cout;


namespace asset
  {
  namespace test
    {
    
    
    
    
    /***********************************************************************
     * @test checking the properties of Asset Category structs.
     *       They are included in the Asset::Ident identification tuple
     *       of Assets and can be used to organize Assets into a tree 
     *       like structure
     */
    class AssetCategory_test : public Test
      {
        virtual void run(Arg arg) 
          {
            createCategory();
            containmentQuery();
            ordering();
          }
        
        
        
        
        void createCategory()
          { 
            Category c1 (AUDIO);
            Category c2 (VIDEO,"bin1");
            Category c3 (VIDEO,"bin1/subbin");
            Category c4 (EFFECT,"some_kind");
            
            format fmt ("Category: %s");
            
            cout << fmt % c1 << "\n";
            cout << fmt % c2 << "\n";
            cout << fmt % c3 << "\n";
            cout << fmt % c4 << "\n";
          }
        
        
        void containmentQuery()
          { 
            Category c1 (VIDEO);
            Category c2 (VIDEO,"bin1");
            Category c3 (VIDEO,"bin1/subbin");
            Category c4 (EFFECT,"some_kind");

            ASSERT ( c1.hasKind(VIDEO) );
            ASSERT (!c1.hasKind(AUDIO) );
            ASSERT ( c2.isWithin(c1) );
            ASSERT ( c3.isWithin(c2) );
            ASSERT ( c3.isWithin(c1) );
            ASSERT (!c1.isWithin(c2) );
            ASSERT (!c2.isWithin(c3) );
            ASSERT (!c1.isWithin(c3) );
            ASSERT (!c3.isWithin(c4) );
            ASSERT (!c4.isWithin(c3) );
          }
        
        
        void ordering()
          { 
            Category c1 (AUDIO);
            Category c2 (VIDEO);
            Category c3 (EFFECT);
            Category c4 (CODEC);
            Category c5 (STRUCT);
            Category c6 (META);
            
            ASSERT (0 > c1.compare(c2));
            ASSERT (0 > c2.compare(c3));
            ASSERT (0 > c3.compare(c4));
            ASSERT (0 > c4.compare(c5));
            ASSERT (0 > c5.compare(c6));
            
            ASSERT (0 ==c1.compare(c1));
            ASSERT (0 > c1.compare(c6));
        
            Category c21 (VIDEO,"bin1");
            Category c22 (VIDEO,"bin2");
            Category c23 (VIDEO,"bin2/sub");

            ASSERT (0 > c1.compare(c21));
            ASSERT (0 > c2.compare(c21));
            ASSERT (0 < c22.compare(c21));
            ASSERT (0 < c23.compare(c22));
            ASSERT (0 < c23.compare(c21));
            ASSERT ( 0==c22.compare(c22));

            
            ASSERT ( c2 == c2 );
            ASSERT ( c2 != c22 );
            ASSERT ( c2 != c3 );
          }
      };
    
    
    /** Register this test class... */
    LAUNCHER (AssetCategory_test, "unit asset");
    
    
    
  } // namespace test

} // namespace asset
