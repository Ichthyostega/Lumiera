/*
  VirtualCopySupport(Test)  -  copy and clone type-erased objects

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file virtual-copy-support-test.cpp
 ** unit test \ref VirtualCopySupport_test
 */



#include "lib/test/run.hpp"
#include "lib/format-string.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/meta/virtual-copy-support.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <string>
#include <type_traits>

using util::_Fmt;
using util::isnil;
using std::string;

using lumiera::error::LERR_(LOGIC);
using lumiera::error::LERR_(WRONG_TYPE);


namespace lib  {
namespace meta {
namespace test {
  
  namespace { // Test fixture...
    
    int _CheckSum_ = 0;
    
    class Interface;
    
    
    /** Interface for the Virtual copy operations.
     * @remarks we define this explicitly here for the tests solely.
     *          In real use, you'd just mix in VirtualCopySupportInterface.
     *          But since we want to verify the text fixture in isolation,
     *          we use here empty base implementations instead of pure
     *          virtual functions, so we can always instantiate all
     *          test classes.
     */
    class CopyInterface
      {
      public:
        virtual void  copyInto (void*     )  const { /* NOP */ };
        virtual void  moveInto (void*     )        { /* NOP */ };
        virtual void  copyInto (Interface&)  const { /* NOP */ };
        virtual void  moveInto (Interface&)        { /* NOP */ };
      };
    
    
    /**
     * The official Interface for our test class hierarchy
     */
    class Interface
      : public CopyInterface
      {
      public:
        virtual ~Interface()  { }
        virtual operator string()  const =0;
        virtual bool empty()       const =0;
      };
    
    /**
     * implementation class with "special" memory layout
     */
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
        
      public: /* == Implementation of the Interface == */
        
        virtual operator string()  const override
          {
            return _Fmt("Sub|%s|%d|-%s")
                      % util::typeStr(this)
                      % i
                      % access();
          }
        
        virtual bool
        empty()  const
          {
            return not access();
          }
        
        
      public: /* == full set of copy and assignment operations == */
        
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
            _CheckSum_ -= access();
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
        void operator= (UnAssignable const&); // private and unimplemented
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
    
    
    
    /* == concrete implementation subclass with virtual copy support == */
    
    template<class IMP>
    class Opaque                          //-----Interface| CRTP-Impl  | direct Baseclass
      : public CopySupport<IMP>::template Policy<Interface, Opaque<IMP>, IMP>
      {
      public:
          static Opaque&
          downcast (Interface& bas)
            {
              Opaque* impl = dynamic_cast<Opaque*> (&bas);
              
              if (!impl)
                throw error::Logic("virtual copy works only on instances "
                                   "of the same concrete implementation class"
                                  ,error::LERR_(WRONG_TYPE));
              else
               return *impl;
            }
      };
    
    // == Test subject(s)==========================
    using RegularImpl  = Opaque<Regular<'a'>>;
    using ClonableImpl = Opaque<UnAssignable<'b'>>;
    using MovableImpl  = Opaque<OnlyMovable<'c'>>;
    using ImobileImpl  = Opaque<Noncopyable<'d'>>;
    
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
          
          verify_fullVirtualCopySupport();
          verify_noAssignementSupport();
          verify_onlyMovableSupport();
          verify_disabledCopySupport();
          
          CHECK(0 == _CheckSum_);
        }
      
      
      /** @test our test fixture is comprised of
       * - a common interface (#Interface)
       * - a implementation template #Sub to hold a buffer and
       *   manage a distinct random value at some position in that buffer,
       *   which depends on the concrete implementation type
       * - layered on top are adapters to make this implementation class
       *   either fully copyable, non-assignable, only movable or noncopyable.
       * - a global checksum, based on the random value of all instances,
       *   which are incremented on construction and decremented on destruction.
       *   After destroying everything this checksum should go to zero.
       * This test case just verifies this implementation mechanic.
       */
      void
      verify_TestFixture()
        {
          /* == full copy, move and assignment == */
          Regular<'A'> a;
          Regular<'A'> aa(a);
          Regular<'A'> a1;
          
          cout << a  <<endl
               << aa <<endl
               << a1 <<endl;
          
          a1 = a;
          
          CHECK (string(a) == string(aa));
          CHECK (string(a) == string(a1));
          CHECK (!isnil(a1));
          
          a = std::move(a1);
          
          CHECK (isnil(a1));
          CHECK (string(a) == string(aa));
          
          
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
          
          cout << cc  <<endl
               << ccc <<endl;
          
          CHECK (string(ccc) == prevID);
          CHECK (string(cc) != prevID);
          CHECK (!isnil(ccc));
          CHECK (isnil (cc));   // killed by moving away
          
          // c = cc;  // does not compile
          
          
          /* == only move construction allowed == */
          OnlyMovable<'D'> d;
          OnlyMovable<'D'> dd (std::move(d));
          
          cout << d  <<endl
               << dd <<endl;
          
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
      
      
      
      
      
      
      void
      verify_fullVirtualCopySupport()
        {
          RegularImpl a,aa,aaa;
          Interface& i(a);
          Interface& ii(aa);
          Interface& iii(aaa);
          
          char storage[sizeof(RegularImpl)];
          Interface& iiii (*reinterpret_cast<Interface*> (&storage));
          
          string prevID = a;
          CHECK (!isnil (a));
          
          i.moveInto(&storage);
          CHECK (string(iiii) == prevID);
          CHECK (!isnil(iiii));
          CHECK ( isnil(i));
          
          ii.copyInto(i);
          CHECK (!isnil(i));
          CHECK (!isnil(ii));
          CHECK (string(i) == string(ii));
          
          prevID = iii;
          iii.moveInto(ii);
          CHECK (!isnil(ii));
          CHECK ( isnil(iii));
          CHECK (string(ii) == prevID);
          
          // Verify that type mismatch in assignment is detected...
          Opaque<Regular<'!'>> divergent;
          Interface& evil(divergent);
          VERIFY_ERROR (WRONG_TYPE, evil.copyInto(i));
          VERIFY_ERROR (WRONG_TYPE, evil.moveInto(i));
          
          
          cout << "==fullVirtualCopySupport=="<<endl
               << i    <<endl
               << ii   <<endl
               << iii  <<endl
               << iiii <<endl;
          
          //need to clean-up the placement-new instance explicitly
          iiii.~Interface();
        }
      
      
      void
      verify_noAssignementSupport()
        {
          ClonableImpl b,bb,bbb;
          Interface& i(b);
          Interface& ii(bb);
          Interface& iii(bbb);
          
          char storage[sizeof(ClonableImpl)];
          Interface& iiii (*reinterpret_cast<Interface*> (&storage));
          
          string prevID = b;
          CHECK (!isnil (b));
          
          i.moveInto(&storage);
          CHECK (string(iiii) == prevID);
          CHECK (!isnil(iiii));
          CHECK ( isnil(i));
          
          iiii.~Interface(); //clean-up previously placed instance
          
          prevID = ii;
          ii.copyInto(&storage);
          CHECK (!isnil(ii));
          CHECK (!isnil(iiii));
          CHECK ( isnil(i));
          CHECK (string(iiii) == prevID);
          CHECK (string(ii) == prevID);
          
          prevID = iii;
          VERIFY_ERROR (LOGIC, iii.copyInto(ii));
          VERIFY_ERROR (LOGIC, iii.moveInto(ii));
          CHECK (string(iii) == prevID);
          CHECK (!isnil(iii));
          
          cout << "==noAssignementSupport=="<<endl
               << i    <<endl
               << ii   <<endl
               << iii  <<endl
               << iiii <<endl;
          
          //clean-up placement-new instance
          iiii.~Interface();
        }
      
      
      void
      verify_onlyMovableSupport()
        {
          MovableImpl c,cc;
          Interface& i(c);
          Interface& ii(cc);
          
          char storage[sizeof(MovableImpl)];
          Interface& iiii (*reinterpret_cast<Interface*> (&storage));
          
          string prevID = i;
          CHECK (!isnil (i));
          
          i.moveInto(&storage);
          CHECK (string(iiii) == prevID);
          CHECK (!isnil(iiii));
          CHECK ( isnil(i));
          
          prevID = ii;
          VERIFY_ERROR (LOGIC, ii.copyInto(&storage));
          VERIFY_ERROR (LOGIC, ii.copyInto(i));
          VERIFY_ERROR (LOGIC, ii.moveInto(i));
          CHECK (string(ii) == prevID);
          CHECK (!isnil(ii));
          CHECK ( isnil(i));
          
          cout << "==onlyMovableSupport=="<<endl
               << i    <<endl
               << ii   <<endl
               << iiii <<endl;
          
          //clean-up placement-new instance
          iiii.~Interface();
        }
      
      
      void
      verify_disabledCopySupport()
        {
          ImobileImpl d,dd;
          Interface& i(d);
          Interface& ii(dd);
          char storage[sizeof(ImobileImpl)];
          
          CHECK (!isnil (i));
          
          string prevID = ii;
          VERIFY_ERROR (LOGIC, ii.copyInto(&storage));
          VERIFY_ERROR (LOGIC, ii.moveInto(&storage));
          VERIFY_ERROR (LOGIC, ii.copyInto(i));
          VERIFY_ERROR (LOGIC, ii.moveInto(i));
          CHECK (string(ii) == prevID);
          CHECK (!isnil(ii));
          CHECK (!isnil (i));
          
          cout << "==disabledCopySupport=="<<endl
               << i  <<endl
               << ii <<endl;
          
          //no clean-up,
          //since we never created anything in the storage buffer
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (VirtualCopySupport_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
