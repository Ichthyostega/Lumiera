/*
  FormatSupport(Test)  -  verify the configuration to support a specific format

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file format-support-test.cpp
 ** unit test \ref FormatSupport_test
 */


#include "lib/test/run.hpp"
#include "lib/time/formats.hpp"


namespace lib {
namespace time {
namespace format{
namespace test {
  
  
  
  /****************************************************//**
   * @test check how support for a specific timecode format
   *       can be enabled and detected. Actually this test
   *       verifies a simple metaprogramming facility,
   *       which allows to check type support at runtime.
   */
  class FormatSupport_test : public Test
    {
      virtual void
      run (Arg) 
        {
          SupportStandardTimecode just_fine;
          Supported just_smpte  = Supported::formats< Types<Smpte> >();
          Supported just_simple = Supported::formats< Types<Frames,Seconds> >();
          
          Supported& support1 (just_fine);
          Supported& support2 (just_smpte);
          Supported& support3 (just_simple);
          
          CHECK ( support1.check<Hms>());
          CHECK ( support1.check<Smpte>());
          CHECK ( support1.check<Frames>());
          CHECK ( support1.check<Seconds>());
          
          CHECK (!support2.check<Hms>());
          CHECK ( support2.check<Smpte>());
          CHECK (!support2.check<Frames>());
          CHECK (!support2.check<Seconds>());
          
          CHECK (!support3.check<Hms>());
          CHECK (!support3.check<Smpte>());
          CHECK ( support3.check<Frames>());
          CHECK ( support3.check<Seconds>());
          
          // format support descriptors are assignable
          just_smpte = just_simple;
          CHECK (support2.check<Hms>()     == support3.check<Hms>());  
          CHECK (support2.check<Smpte>()   == support3.check<Smpte>());
          CHECK (support2.check<Frames>()  == support3.check<Frames>());
          CHECK (support2.check<Seconds>() == support3.check<Seconds>());
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (FormatSupport_test, "unit common");
  
  
  
}}}} // namespace lib::time::format::test
