/*
  TEST-DUMMY-MOBJECT.hpp  -  dummy MObject hierarchy for unit tests

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** @todo needed to move this temporarily into the core session tree. ////////////TICKET #532
 **
 ** @see mobject::session::BuilderTool_test
 ** @see mobject::session::PlacementHierarchy_test
 ** 
 */


#ifndef MOBJECT_TEST_DUMMY_MOBJECT_H
#define MOBJECT_TEST_DUMMY_MOBJECT_H

#include "lib/test/run.hpp"

#include "steam/mobject/builder/buildertool.hpp"
#include "steam/mobject/session/abstractmo.hpp"
#include "steam/mobject/placement.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <cstdlib>

using util::_Fmt;
using std::string;
using std::rand;



namespace steam    {
namespace mobject {
namespace test    {
  
  using lib::rani;
  using lib::Symbol;
  using builder::BuilderTool;
  
  
  /** 
   *  Test MObject subclass, which, contrary to any real MObject,
   *  can be created directly without involving MObjectFactory.
   */
  class DummyMO : public session::AbstractMO
    {
      int id_;
      
    public:
      DummyMO() : id_{rani(1000)} {}
      DummyMO(int i) : id_(i) {}
      
      DEFINE_PROCESSABLE_BY (BuilderTool);
      
      virtual bool isValid()       const   { return true;}
      virtual string initShortID() const   { return buildShortID("DummyMO"); }
      virtual operator string()    const   { return display("DummyMO"); }
      static void killDummy (MObject* dum) { delete (DummyMO*)dum; }
      
    protected:
      string
      display(Symbol name)  const
        {
          static _Fmt fmt{"%s(ID=%03d)"};
          return fmt % name % this->id_;
        }
    };
  
  /** 
   * Subclass-1 is \em not defined "processible",
   * thus will always be handled as DummyMO...
   */
  struct TestSubMO1 : DummyMO 
    {
      virtual operator string() const      { return display("TestSubMO1"); }
    };
  
  /** 
   * Subclass-2 \em is defined "processible", 
   * but we omit the necessary "applicable" definition in TestTool,
   * resulting in an invocation of the error (catch-all) function...
   */
  struct TestSubMO2 : DummyMO 
    { 
      DEFINE_PROCESSABLE_BY (BuilderTool);

      virtual operator string() const      { return display("TestSubMO2"); }
    };
  
  struct TestSubMO21 : TestSubMO2 
    {
      virtual operator string() const      { return display("TestSubMO21"); }
      virtual void specialAPI() const      { cout << "specialAPI()\n";}
    };

  
  
  template<class DMO=DummyMO, class B=DummyMO>
  class TestPlacement ;
  
  template<>
  class TestPlacement<> : public Placement<DummyMO>
    {
    public:
      TestPlacement(DummyMO& dummyObj) 
        : Placement<DummyMO>::Placement(dummyObj, &DummyMO::killDummy)
        { }
      
      // allowing all kinds of copy
      TestPlacement(Placement<DummyMO> const& refP)
        : Placement<DummyMO>::Placement(refP)
        { }
    };
  
  template<class DMO, class B>
  class TestPlacement : public TestPlacement<B>
    {
    public:
      TestPlacement(DMO& dummyObj) 
        : TestPlacement<B>(dummyObj)
        { }
      
      DMO *
      operator-> ()  const
        {
          ENSURE (INSTANCEOF (DMO, this->get()));
          return static_cast<DMO*>
            (Placement<MObject>::_SmartPtr::operator-> ());
        }
    };
  
  template class TestPlacement<TestSubMO21,TestSubMO2>;
  
  
  
}}} // namespace steam::mobject::test
#endif
