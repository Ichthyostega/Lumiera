/*
  DUMMY-FUNCTIONS  -  mock functions for checking functor utils

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef LIB_META_DUMMY_FUNCTIONS_H
#define LIB_META_DUMMY_FUNCTIONS_H


//#include <boost/format.hpp>
//#include <string>

//using std::string;
//using boost::format;


namespace lib  {
namespace meta {
namespace test {
  
  
  namespace { // test helpers
  
    int _sum_ =0;  ///< used to verify the effect of testFunc
  
    void
    testFunc (int i, char c)
    {
      _sum_ += i + c;
    }
    
    int
    returnIt ()
    {
      return _sum_;
    }
  }
  
  
  
}}} // namespace lib::meta::test
#endif
