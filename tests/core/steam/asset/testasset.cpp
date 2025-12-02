/*
  TestClip  -  test clip (stub) for checking Model/Session functionality

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file testasset.cpp
 ** Implementation of a mocked Asset for unit tests
 */


#include "steam/asset/testasset.hpp"
#include "steam/assetmanager.hpp"

using std::static_pointer_cast;


namespace steam {
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
  
  
  
}}} // namespace steam::asset::test





   /*********************************************************/
   /* explicit template instantiations for some Asset Kinds */
   /*********************************************************/

#include "steam/asset/unknown.hpp"


namespace steam {
namespace asset{
namespace test {
  
  template TestAsset<Asset>::TestAsset ();
  template TestAsset<Unknown>::TestAsset ();
  
  template TestAsset<Asset>::TestAsset (PAsset& pRef);
  template TestAsset<Unknown>::TestAsset (PAsset& pRef);
  
  template lib::P<TestAsset<Asset>>   TestAsset<Asset>::ptrFromThis ();
  template lib::P<TestAsset<Unknown>> TestAsset<Unknown>::ptrFromThis ();
  
  
  
}}} // namespace steam::asset::test
