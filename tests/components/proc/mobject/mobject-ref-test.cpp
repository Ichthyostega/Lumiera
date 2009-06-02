/*
  MObjectRef(Test)  -  validating basic properties of the external MObject reference tag
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "lib/test/run.hpp"
//#include "lib/lumitime.hpp"
#include "proc/mobject/mobject.hpp"
#include "proc/mobject/mobject-ref.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"

#include <iostream>

//using lumiera::Time;
using std::string;
using std::cout;


namespace mobject {
namespace test    {
  
  using namespace mobject::test;

  
  /***************************************************************************
   * @test properties and behaviour of the external reference-mechanism for
   *       MObjects placed into the session. 
   * @see  mobject::Placement
   * @see  mobject::MObject
   * @see  mobject::PlacementRef_test
   */
  class MObjectRef_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MObjectRef_test, "unit session");
  
  
}} // namespace mobject::test
