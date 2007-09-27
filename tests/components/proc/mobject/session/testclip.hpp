/*
  TESTCLIP.hpp  -  test clip (stub) for checking EDL/Session functionality
 
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
 
*/


#ifndef MOBJECT_SESSION_TESTCLIP_H
#define MOBJECT_SESSION_TESTCLIP_H


#include "common/test/run.hpp"
#include "common/factory.hpp"
//#include "common/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

using std::tr1::shared_ptr;
//using boost::format;
using std::string;
using std::cout;


namespace mobject
  {
  namespace session
    {
    namespace test
      {
        
      /**
       * Sample or Test Clip for checking
       * various EDL, session and builder operations.
       * Can be used as Mock object to record invoked operations.
       * 
       */
      class TestClip   ////TODO inherit from mobject::session::Clip
        {
          
          /** smart ptr factory allowed to invoke TestClip's ctor */
          struct Factory : cinelerra::Factory<TestClip>
            {
              typedef shared_ptr<TestClip> PType;
              PType operator() () { return PType (new TestClip, &destroy); }
            protected:
              static void destroy (TestClip* tc) { delete tc; }
            };
          
          
          TestClip ();  
          friend class Factory;  
          
        public:
          static Factory create;
        };
        
    
    
    
    } // namespace test
  
  } // namespace session

} // namespace mobject
#endif
