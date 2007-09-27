/*
  DeleteAsset(Test)  -  deleting and Asset with all dependencies
 
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
//#include "common/factory.hpp"
//#include "common/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
using std::string;
using std::cout;


namespace asset
  {
  namespace test
    {
    
    
    
    
    /*******************************************************************
     * @test deleting an Asset includes removing all dependant Assets
     *       and all MObjects relying on these. Especially this means
     *       breaking all links between the involved Objects, so the
     *       shared-ptrs can do the actual cleanup.
     * @see  asset::Asset#unlink
     * @see  mobject::MObject#unlink
     */
    class DeleteAsset_test : public Test
      {
        virtual void run(Arg arg) 
          {
            UNIMPLEMENTED ("delete asset and update all dependencies");
          } 
      };
    
    
    /** Register this test class... */
    LAUNCHER (DeleteAsset_test, "function asset");
    
    
    
  } // namespace test

} // namespace asset
