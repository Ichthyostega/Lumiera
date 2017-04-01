/*
  TestClip  -  test clip (stub) for checking Model/Session functionality

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

/** @file testasset.cpp
 ** Implementation of a mocked Asset for unit tests
 */


#include "proc/asset/testasset.hpp"
#include "proc/assetmanager.hpp"

using std::static_pointer_cast;


namespace proc {
namespace asset{
namespace test {
  
  namespace 
    {
    uint counter (0);
    
    /** @internal helper generating continuously
     *            different new asset identities
     */
    Asset::Ident
    make_new_ident ()
    {
      return Asset::Ident ( string(_Fmt("TestAsset.%i") % counter)
                          , Category (META)
                          , "test"
                          , counter++
                          );
    }
    Asset::Ident
    make_new_ident (PAsset& ref)
    {
      return Asset::Ident ( string(_Fmt("%s-TestAsset.%i") % ref->ident.name
                                                           % counter)
                          , ref->ident.category
                          , "test"
                          , counter++
                          );
    }
  }
  
  
  template<class A>
  TestAsset<A>::TestAsset ()
    : A(make_new_ident ())
    { };
  
  
  template<class A>
  TestAsset<A>::TestAsset (PAsset& pRef)
    : A(make_new_ident (pRef)) 
  { 
    this->defineDependency(pRef); 
  };
  
  
  /** @internal helper for the create()-Functions
   *  retrieving the smart ptr created automatically
   *  within AssetManager by the Asset base class ctor
   */
  template<class A>
  lib::P<TestAsset<A>>
  TestAsset<A>::ptrFromThis ()
  {
    return static_pointer_cast<TestAsset<A>,Asset>
      (AssetManager::instance().getAsset (this->id));
  };
  
  
  
}}} // namespace proc::asset::test





   /*********************************************************/
   /* explicit template instantiations for some Asset Kinds */
   /*********************************************************/

#include "proc/asset/unknown.hpp"


namespace proc {
namespace asset{
namespace test {
  
  template TestAsset<Asset>::TestAsset ();
  template TestAsset<Unknown>::TestAsset ();
  
  template TestAsset<Asset>::TestAsset (PAsset& pRef);
  template TestAsset<Unknown>::TestAsset (PAsset& pRef);
  
  template lib::P<TestAsset<Asset>>   TestAsset<Asset>::ptrFromThis ();
  template lib::P<TestAsset<Unknown>> TestAsset<Unknown>::ptrFromThis ();
  
  
  
}}} // namespace proc::asset::test
