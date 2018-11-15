/*
  MediaAccessMock  -  a test (stub) target object for testing the factories

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


/** @file media-acess-mock.cpp
 ** Mock implementation of the Interface normally used to query media file
 ** information from the data backend. The Mock implementation instead holds
 ** a map of fixed response which will be delivered when querying some magic
 ** filenames.
 ** @todo this facility was occasionally used until 2011, yet never really completed
 ** 
 ** @see mediaaccessmocktest.cpp validating the Mock
 ** @see MediaAccessFactory the real thing
 **
 */


#include "vault/media-access-mock.hpp"
#include "steam/mobject/session/testclip.hpp"

#include "lib/util.hpp"
#include "lib/depend.hpp"
#include "lib/time/mutation.hpp"
#include "lib/symbol.hpp"

#include <vector>
#include <map>

using lumiera::error::Invalid;
using lib::time::Mutation;
using lib::time::Duration;
using lib::Literal;
using util::isnil;
using std::string;
using std::vector;
using std::map;


namespace vault {
namespace test {
  
  typedef MediaAccessFacade::ChanHandle ChanHandle;
  
  
  namespace { // implementation details
    
    struct Response
      {
        MediaDesc        globalDesc;
        vector<ChanDesc> channels;
        
        Response&
        globalLength (Duration length)
          {
            globalDesc.length.accept (Mutation::changeDuration(length));
            return *this;
          }
        
        Response&
        channel (Literal name, Literal id)
          {
            channels.push_back (ChanDesc (name, id, genH()));
            return *this;
          }
        
      private:
        static int _i_;
        
        ChanHandle
        genH()
          {
            return reinterpret_cast<ChanHandle> (++_i_);
          }
      };
    int Response::_i_(0);
    const ChanDesc NULLResponse;
    using steam::mobject::session::test::LENGTH_TestClip;
    
    
    struct TestCases : map<string,Response>
      {
        TestCases ()
          {
            // ----------------------------------------------------------------------TESTCASES
            (*this)["test-1"].globalLength(LENGTH_TestClip).channel("video","ID");
            
            (*this)["test-2"].globalLength(LENGTH_TestClip).channel("video","H264")
                                                           .channel("audio-L","PCM")
                                                           .channel("audio-R","PCM");
            (*this)["test-3"].globalLength(LENGTH_TestClip).channel("audio","PCM");
            (*this)["test-4"].globalLength(LENGTH_TestClip).channel("audio-W","PCM")
                                                           .channel("audio-X","PCM")
                                                           .channel("audio-Y","PCM")
                                                           .channel("audio-Z","PCM");
            // ----------------------------------------------------------------------TESTCASES
          }
        
        bool
        known (string key)
          {
            const_iterator i = find (key);
            return (i != end());
          }
      };
      
      // instantiate TestCasses table
      lib::Depend<TestCases> testCases;
      
  } // (end) implementation namespace
  
  
  
  MediaDesc&
  MediaAccessMock::queryFile (string const& name)  const
  {
    if (isnil (name))
      throw Invalid ("empty filename passed to MediaAccessFacade.");
    
    if (!testCases().known(name))
      throw Invalid ("unable to use media file \""+name+"\"."
                     "Hint: you're using a test-mock file access, "
                     "which responds only to some magical names.");
    
    return testCases()[name].globalDesc;
  }
  
  
  ChanDesc
  MediaAccessMock::queryChannel (MediaDesc& h, uint chanNo)  const
  {
    Response const& res (*reinterpret_cast<Response*> (&h));
    
    if (res.channels.size() <= chanNo)
      return NULLResponse;
    else
      return res.channels[chanNo];
  }
  
  
}} // namespace vault::test
