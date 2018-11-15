/*
  TESTCLIP.hpp  -  test clip (stub) for checking Model/Session functionality

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

*/

/** @file testclip.hpp
 ** A stub implementation of the Clip interface for unit testing
 */


#ifndef MOBJECT_SESSION_TESTCLIP_H
#define MOBJECT_SESSION_TESTCLIP_H


#include "proc/mobject/session/clip.hpp"
#include "lib/time/timevalue.hpp"



using std::shared_ptr;
using std::string;


namespace proc {
namespace mobject {
namespace session {
namespace test    {
  
  
  class TestPlacement;
  
  /**
   * Sample or Test Clip for checking
   * various model, session and builder operations.
   * @todo maybe use this as Mock object to record invoked operations?
   * 
   */
  class TestClip   : public mobject::session::Clip
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
  
  
  /* == some test data to check == */
  extern const lib::time::Duration LENGTH_TestClip;
  
  
}}}} // namespace proc::mobject::session::test
#endif
