/*
  TESTASSET.hpp  -  test asset (stub) for checking internal asset functionality

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

/** @file testasset.hpp
 ** A mock asset to support unit testing
 */


#ifndef ASSET_TESTASSET_H
#define ASSET_TESTASSET_H


#include "proc/asset.hpp"
#include "lib/format-string.hpp"



namespace proc {
namespace asset{
namespace test {
  
  using util::_Fmt;
  
  
  /**
   * Test(mock) asset subclass usable for hijacking a given
   * asset class (template parameter) and subsequently accessing
   * internal facilities for writing unit tests. Prerequisite
   * for using this template is that the used asset base class
   * has a (protected) ctor taking an Asset::Ident....
   */
  template<class A>
  class TestAsset : public A
    {
      TestAsset () ;
      TestAsset (PAsset&);  ///< declared dependent on the given Asset
      
      static void deleter (TestAsset<A>* aa) { delete aa; }
      
    public:
      using PA = lib::P<TestAsset<A>>;
      
      static PA create ()             { return (new TestAsset<A>       )->ptrFromThis(); }
      static PA create (PAsset& pRef) { return (new TestAsset<A> (pRef))->ptrFromThis(); }
      
      /* === interesting asset features we want to access for tests === */
      void call_unlink ()             { this->unlink (); }
      void call_unlink (IDA target)   { this->unlink (target); }
      void set_depend (PAsset parent) { this->defineDependency (parent); }
      
    private:
      PA ptrFromThis ();
    };
  
  
  
}}} // namespace proc::asset::test
#endif
