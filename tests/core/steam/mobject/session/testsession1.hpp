/*
  TESTSESSION1.hpp  -  complete session configuration used for various tests

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file testsession1.hpp
 ** Dummy session content prepared for unit test
 ** @todo this is an idea from the early stages of the project and was never really implemented.
 **       But it still seems like a good idea (as of 2016), while likely the implementation will be
 **       completely different, based on JSON to be injected via the diff system
 */


#ifndef MOBJECT_SESSION_TESTSESSION1_H
#define MOBJECT_SESSION_TESTSESSION1_H


#include "steam/mobject/session.hpp"
#include "lib/error.hpp"
//#include "lib/util.hpp"

//#include <string>

//using std::string;


namespace steam {
namespace mobject {
namespace session {
  
  /**
   * Create a Test Session configuration usable for various Tests.
   * This Session holds two Clips and corresponds to "Example1"
   * in the UML design. All changes are done to the (global)
   * current session.
   */
  inline void 
  buildTestsession1 ()
    {
      UNIMPLEMENTED ("Test-Session 1");
    };
  
  
  /**
   * Analyse the current (global) Session to verify the
   * configuration of "Test-Session 1"
   */
  inline bool 
  checkTestsession1 ()
    {
      UNIMPLEMENTED ("Test-Session 1");
      return false;
    };
  
  
  const string SESSION1_CLIP("TODO: some sensible way to refer to a clip");
  
  
}}} // namespace steam::mobject::session
#endif
