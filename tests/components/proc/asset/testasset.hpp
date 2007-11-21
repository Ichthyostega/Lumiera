/*
  TESTASSET.hpp  -  test asset (stub) for checking internal asset functionality
 
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


#ifndef ASSET_TESTASSET_H
#define ASSET_TESTASSET_H


#include "proc/asset.hpp"
//#include "common/util.hpp"


#include <boost/format.hpp>

using std::tr1::shared_ptr;
using boost::format;
using std::string;


namespace asset
  {
  namespace test
    {
    
    namespace 
      {
      uint counter (0);
      
      /** @internal helper generating continuosely
       *            different new asset identities 
       */
      Asset::Ident
      make_ident ()  
      {
        return Asset::Ident ( str(format("TestAsset.%i") % counter)
                            , Category (META)
                            , "test"
                            , counter++
                            );
      }
      Asset::Ident
      make_ident (PAsset& ref)
      {
        return Asset::Ident ( str(format("%s-TestAsset.%i") % ref->ident.name 
                                                            % counter)
                            , ref->ident.category
                            , "test"
                            , counter++
                            );
      }
    }
    
    
    
    /**
     * Test(mock) asset subclass usable for hijacking a given
     * asset class (template parameter) and subsequently accessing
     * internal facillities for writing unit tests. Prerequisite
     * for using this template is that the used asset base class
     * has a (protected) ctor taking an Asset::Ident....
     */
    template<class A>
    class TestAsset : public A
      {
        TestAsset ()             : A(make_ident ())     { };
        TestAsset (PAsset& pRef) : A(make_ident (pRef)) { this->defineDependency(pRef); };
        
        static void deleter (TestAsset<A>* aa) { delete aa; }
        
      public:
        typedef shared_ptr<TestAsset<A> > PA;
        
        static PA create ()             { return PA (new TestAsset<A>,        &deleter); }
        static PA create (PAsset& pRef) { return PA (new TestAsset<A> (pRef), &deleter); }
        
        /* === interesting asset features we want to access for tests === */
        void call_unlink ()             { this->unlink (); }
        void call_unlink (IDA target)   { this->unlink (target); }
        void set_depend (PAsset parent) { this->defineDependency (parent); }
        
      };
  
  
  
  } // namespace test
  
} // namespace asset
#endif
