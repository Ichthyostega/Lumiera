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

#include "proc/mobject/session/clip.hpp"
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
      class TestPlacement;
       
      /**
       * Sample or Test Clip for checking
       * various EDL, session and builder operations.
       * @todo make this usable as Mock object to record invoked operations.
       * 
       */
      class TestClip   : public mobject::session::Clip   /////////////TODO how this????
        {
          
          TestClip ();
          static void deleter (MObject* mo) { delete (TestClip*)mo; }

          friend class TestPlacement;  
          
        public:
          static Placement<Clip> create () ;
        };

        
      class TestPlacement : public Placement<Clip>
        {
          
        public:
          TestPlacement (TestClip & subject)
            : Placement<Clip> (subject, &TestClip::deleter)
            { }
        };

        
      inline Placement<Clip>
      TestClip::create()
      { 
        return TestPlacement (*new TestClip); 
      }
    
    
    
    } // namespace test
  
  } // namespace session

} // namespace mobject
#endif
