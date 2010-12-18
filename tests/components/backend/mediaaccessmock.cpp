/*
  MediaAccessMock.hpp  -  a test (stub) target object for testing the factories

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


/** @file mediaacessmock.cpp
 ** Mock implementation of the Interface normally used to query media file
 ** informations from the data backend. The Mock implementation instead holds
 ** a map of fixed response which will be delivered when querying some magic
 ** filenames.
 ** 
 ** @see mediaaccessmocktest.cpp validating the Mock
 ** @see MediaAccessFactory the real thing
 **
 */


#include "backend/mediaaccessmock.hpp"

#include "lib/util.hpp"
#include "lib/util-foreach.hpp"

#include <iostream>
#include <vector>
#include <map>

using lumiera::error::Invalid;
using util::for_each;
using util::isnil;
using std::cout;
using std::string;
using std::vector;
using std::map;


namespace backend_interface {
namespace test {
  
  typedef MediaAccessFacade::FileHandle FileHandle;
  typedef MediaAccessFacade::ChanHandle ChanHandle;
  
  
  namespace { // implementation details
      
    typedef vector<ChanDesc> Response;
    const ChanDesc NULLResponse;
    
    struct TestCases : map<string,Response>
      {
        TestCases ()
          {
            // ------------------------------------------------------------------TESTCASES
            (*this)["test-1"].push_back (ChanDesc ("video","ID", genH()));
            
            (*this)["test-2"].push_back (ChanDesc ("video","H264", genH()));
            (*this)["test-2"].push_back (ChanDesc ("audio-L","PCM", genH()));
            (*this)["test-2"].push_back (ChanDesc ("audio-R","PCM", genH()));
            // ------------------------------------------------------------------TESTCASES
          }
        
        bool known (string key)
          {
            const_iterator i = find (key);
            return (i != end());
          }
      private:
        int _i_;
        ChanHandle genH()
          {
            return reinterpret_cast<ChanHandle> (++_i_);
          }
      };
      
      // instantiate TestCasses table
      TestCases testCases;
      
  } // (end) implementation namespace
  
  
  FileHandle
  MediaAccessMock::queryFile (const char* name)  throw(Invalid)
  {
  if (isnil (name))
    throw Invalid ("empty filename passed to MediaAccessFacade.");
  
    if (!testCases.known(name))
      return 0;
    else
      return reinterpret_cast<void*> (&testCases[name]);
  }
  
  ChanDesc
  MediaAccessMock::queryChannel (FileHandle h, uint chanNo)  throw()
  {
    const Response* res (reinterpret_cast<Response*> (h));
    
    if (!res || res->size() <= chanNo) 
      return NULLResponse;
    else
      return (*res)[chanNo];
  }
  
  
}} // namespace backend_interface::test
