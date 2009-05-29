/*
  TEST-DUMMY-MOBJECT.hpp  -  dummy MObject hierarchy for unit tests
 
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
 
*/

/** @file test-dummy-mobject.hpp
 ** A hierarchy of simple dummy-Media-Objects for easy unit testing. 
 ** Normally, the creation of MObjects and corresponding Placements is defined to be
 ** very restricted. Any MObjects are supposed to be created by the MObjectFactory,
 ** which provided very specialised factory functions. When writing unit tests to
 ** check the basic MObject properties and behaviour, this can be an obstacle. 
 ** Thus we create a separate branch in the MObject hierarchy with relaxed
 ** requirements and some dummy/test API functions.
 **
 ** @see mobject::session::BuilderTool_test
 ** @see mobject::session::PlacementHierarchy_test
 ** 
 */


#ifndef MOBJECT_TEST_DUMMY_MOBJECT_H
#define MOBJECT_TEST_DUMMY_MOBJECT_H

#include "lib/test/run.hpp"

#include "proc/mobject/builder/buildertool.hpp"
#include "proc/mobject/session/abstractmo.hpp"
#include "proc/mobject/placement.hpp"
#include "lib/util.hpp"

#include <iostream>

using util::cStr;
using std::string;
using std::cout;



namespace mobject {
namespace test    {
  
  using builder::BuilderTool;
  
  
  /** 
   *  Test MObject subclass, which, contrary to any real MObject,
   *  can be created directly without involving MObjectFactory.
   */
  class DummyMO : public session::AbstractMO
    {
    public:
      DummyMO() { };
      
      DEFINE_PROCESSABLE_BY (BuilderTool);
      
      virtual bool isValid()  const        { return true;}
      static void killDummy (MObject* dum) { delete (DummyMO*)dum; }
    };
  
  /** 
   * Subclass-1 is \em not defined "processible",
   * thus will always be handled as DummyMO...
   */
  class TestSubMO1 : public DummyMO 
    { };
  
  /** 
   * Subclass-2 \em is defined "processible", 
   * but we omit the necessary "applicable" definition in TestTool,
   * resulting in an invocation of the error (catch-all) function...
   */
  class TestSubMO2 : public DummyMO 
    { 
      DEFINE_PROCESSABLE_BY (BuilderTool);
    };
  
  
  template<class MO>
  class TestPlacement : public Placement<MO>
    {
    public:
      TestPlacement(DummyMO& testObject) 
        : Placement<MO>::Placement(testObject, &DummyMO::killDummy)
        { }
    };
  
  
  
}} // namespace mobject::test
#endif
