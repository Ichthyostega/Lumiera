/*
  TESTTARGETOBJ.hpp  -  a test (stub) target object for testing the factories
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/


#ifndef CINELERRA_TEST_TESTTARGETOBJ_H
#define CINELERRA_TEST_TESTTARGETOBJ_H


#include "common/test/run.hpp"
#include "common/factory.hpp"
//#include "common/util.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <iostream>

using boost::algorithm::join;
using boost::lexical_cast;
using boost::format;
using std::string;
using std::cout;


namespace cinelerra
  {
  namespace test
    {
    /**
     * Target object to be created by Test-Factories or as Singleton.
     * Allocates a variable amount of additional heap memory
     * and prints diagnostic messages.
     */
    class TestTargetObj
      {
        uint cnt_;
        string* heapData_; 
        string* heapArray_; 
        
      public:
        TestTargetObj(uint num);
        ~TestTargetObj()  throw();
        
        operator string () const;
      };
    
    
    
    inline
    TestTargetObj::TestTargetObj(uint num)
      : cnt_ (num),
        heapData_ (new string(num,'*')),
        heapArray_ (new string[num])
    {
      for (uint i=0; i<cnt_; ++i)
        heapArray_[i] = lexical_cast<string>(i);
      cout << format("ctor TargetObj(%i) successfull\n") % cnt_;
    }
    
    
    inline
    TestTargetObj::~TestTargetObj()  throw()
    {
      delete heapData_;
      delete[] heapArray_;
      cout << format("dtor ~TargetObj(%i) successfull\n") % cnt_;
    }
    
    
    
    inline
    TestTargetObj::operator string () const
    {
      string array_contents = "{";
      for (uint i=0; i<cnt_; ++i)
        array_contents += heapArray_[i]+",";
      array_contents+="}";
      
      return str (format(".....TargetObj(%1%): data=\"%2%\", array[%1%]=%3%")
                        % cnt_ 
                        % *heapData_ 
                        % array_contents
                 );
    }
    
    
    
    
  } // namespace test

} // namespace cinelerra
#endif
