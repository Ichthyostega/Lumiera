/*
  VirtualCopySupport(Test)  -  copy and clone type-erased objects

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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



#include "lib/test/run.hpp"
#include "lib/format-string.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/virtual-copy-support.hpp"
#include "lib/util.hpp"

#include <iostream>
#include <string>
#include <type_traits>

using util::_Fmt;
using util::isnil;
using std::string;
using std::cout;
using std::endl;


namespace lib  {
namespace meta {
namespace test {
  
  namespace { // Test fixture...
    
    class Interface;
    
    /** helper: shortened type display */
    string
    typeID(Interface const& obj)
    {
      string typeStr = lib::test::demangleCxx(
                       lib::test::showType(obj));
      size_t pos = typeStr.rfind("::");
      if (pos != string::npos)
        typeStr = typeStr.substr(pos+2);
      return typeStr;
    }
    
    int _CheckSum_ = 0;
    
    class Interface
      {
      public:
        virtual ~Interface()  { }
        virtual operator string()  const =0;
        virtual bool empty()       const =0;
      };
    
    template<uint i>
    class Sub
      : public Interface
      {
        static_assert (0 < i, "invalid construction");
        
        char storage_[i];
        
        char&
        access()
          {
            return storage_[i-1];
          }
        char const&
        access()  const
          {
            return storage_[i-1];
          }
        
      public:
        virtual operator string()  const override
          {
            return _Fmt("Sub|%s|%d|-%s")
                      % typeID(*this)
                      % i
                      % access();
          }
        
        virtual bool
        empty()  const
          {
            return !bool(access());
          }
        
       ~Sub()
          {
            _CheckSum_ -= access();
          }
        Sub()
          {
            access() = 'A' + rand() % 23;
            _CheckSum_ += access();
          }
        Sub (Sub const& osub)
          {
            access() = osub.access();
            _CheckSum_ += access();
          }
        Sub (Sub&& rsub)
          {
            access() = 0;
            std::swap(access(),rsub.access());
          }
        Sub&
        operator= (Sub const& osub)
          {
            if (!util::isSameObject (*this, osub))
              {
                _CheckSum_ -= access();
                access() = osub.access();
                _CheckSum_ += access();
              }
            return *this;
          }
        Sub&
        operator= (Sub&& rsub)
          {
            access() = 0;
            std::swap(access(),rsub.access());
            return *this;
          }
      };
    
    
    /* == create various flavours of copyable / noncopyable objects == */
    
    template<char c>
    class Regular
      : public Sub<c>
      { };
    
    template<char c>
    class UnAssignable
      : public Sub<c>
      {
        void operator= (UnAssignable const&);
      public:
        UnAssignable()                    = default;
        UnAssignable(UnAssignable&&)      = default;
        UnAssignable(UnAssignable const&) = default;
      };
    
    template<char c>
    class OnlyMovable
      : public UnAssignable<c>
      {
      public:
        OnlyMovable (OnlyMovable const&) = delete;
        OnlyMovable()                    = default;
        OnlyMovable(OnlyMovable&&)       = default;
      };
    
    template<char c>
    class Noncopyable
      : public OnlyMovable<c>
      {
      public:
        Noncopyable (Noncopyable&&) = delete;
        Noncopyable ()              = default;
      };
    
  }//(End)Test fixture
  
  
  
  
  
  
  /**********************************************************************************************//**
   * @test verify a mechanism to allow for cloning and placement new of opaque, type-erased entities.
   *       Basically we allow only assignments and copy between objects of the same concrete type,
   *       but we want to initiate those operations from the base interface, without any further
   *       knowledge about the actual types involved.
   */
  class VirtualCopySupport_test : public Test
    {
      virtual void
      run (Arg)
        {
          CHECK(0 == _CheckSum_);
          
          verify_TestFixture();
          
          CHECK(0 == _CheckSum_);
        }
      
      void
      verify_TestFixture()
        {
          /* == full copy, move and assignment == */
          Regular<'A'> a;
          Regular<'A'> aa(a);
          Regular<'A'> a1;
          
          cout << string(a) <<endl
               << string(aa)<<endl
               << string(a1)<<endl;
          
          a1 = a;
          
          CHECK (string(a) == string(aa));
          CHECK (string(a) == string(a1));
          
          
          /* == interface vs. concrete class == */
          Regular<'B'> b;
          Interface& ii = b;
          
          string prevID(b);
          ii = a;             // Copy operation of Base-Interface is NOP
          CHECK (string(b) == prevID);
          
          
          /* == assignment inhibited == */
          UnAssignable<'C'> c;
          UnAssignable<'C'> cc(c);
          
          CHECK (string(c) == string(cc));
          
          prevID = cc;
          UnAssignable<'C'> ccc(std::move(cc));
          
          cout << string(cc) <<endl
               << string(ccc)<<endl;
          
          CHECK (string(ccc) == prevID);
          CHECK (string(cc) != prevID);
          CHECK (!isnil(ccc));
          CHECK (isnil (cc));   // killed by moving away
          
          // c = cc;  // does not compile
          
          
          /* == only move construction allowed == */
          OnlyMovable<'D'> d;
          OnlyMovable<'D'> dd (std::move(d));
          
          cout << string(d) <<endl
               << string(dd)<<endl;
          
          CHECK (string(dd) != string(d));
          CHECK (!isnil(dd));
          CHECK (isnil(d));
          
          // OnlyMovable<'D'> d1 (dd);     // does not compile
          // d = dd;                       // does not compile;
          
          
          /* == all copy/assignment inhibited == */
          Noncopyable<'E'> e;
          // Noncopyable<'E'> ee (e);                // does not compile
          // Noncopyable<'E'> eee (std::move (e));   // does not compile
          // e = Noncopyable<'E'>();                 // does not compile
          
          CHECK (!isnil (e));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (VirtualCopySupport_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
