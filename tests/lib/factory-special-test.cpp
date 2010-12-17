/*
  Factory-Special(Test)  -  testing the more advanced features of factory

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


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "testtargetobj.hpp"
#include "lib/factory.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <iostream>

using boost::lexical_cast;
using boost::format;
using util::isnil;
using std::string;
using std::cout;


namespace lib {
namespace test{
  
  /**
   * Example Allocator using plain C memory management.
   */
  class MallocAllocator
    {
    public:
      void* operator new (size_t siz) { return malloc (siz); };
      void  operator delete (void* p) { if (p) free (p);     };
    };
  
  
  /** 
   * Simple subclass used for custom alloc and as Interface class
   */
  class TargetO : public TestTargetObj
    {
      long additional_member;
      
    public:
      TargetO (uint cnt) : TestTargetObj(cnt) {}
      virtual ~TargetO () {};
      
      /** Example Base/Interface function */
      virtual void funky()
        {
          cout << string(*this) << "\n";
        }
    };
  
  
  /**
   * Subclass of the Interface class TargetO, could be an implementation class.
   */
  class ImplObj : public TargetO
    {
    public:
      ImplObj () : TargetO(12) {}
      
      /** Example Implementation function */
      virtual void funky()
        {
          cout << ".....ImplObj::funky() called\n";
          TargetO::funky();
        }
    };
  
  
  /**
   * Another special Subclass, using custom allocation.
   */
  class MallocO : public TestTargetObj, public MallocAllocator
    {
    public:
      MallocO () : TestTargetObj (7) {}
    };
  
  
  
  class Factory2; 
    
  /**
   * Special Subclass prohibiting public use of ctor
   */
  class ParanoidObj : public TestTargetObj
    {
    private:
      ParanoidObj (uint cnt) : TestTargetObj(cnt) {}
      ~ParanoidObj () {}
      
      friend class Factory2;
    };
  
  
  
  
  /* ===== several specialised Factories ====== */
  
  using std::tr1::shared_ptr;
  
  /** 
   * Factory creating refcounting TargetO pointers 
   * and using placement Allocation. 
   */ 
  class Factory1 : public Factory<TargetO>
    {
    public:
      typedef shared_ptr<TargetO> PType;
      
      /** Specialised Factory method
       *  doing placement new and invoking
       *  a special constructor. */
      PType operator() (uint param) { return wrap (new(alloc()) TargetO(param) ); }
      
    protected:
      PType wrap (TargetO* tO) { return PType (tO, &destroy); } ///< note: providing custom deleter function
      
      static void destroy (TargetO* victim) { victim->~TargetO(); }  ///< call dtor but don't delete mem
      static void* alloc ()                 { return buff; }        ///<  returning raw mem for new object
      static char buff[];                                          ///<   we place our (single) instance here
    };
  char Factory1::buff[sizeof(TargetO)];
  
  
  
  /** 
   * Factory usable if object can be constructed only by friends
   */ 
  class Factory2 : public Factory<ParanoidObj>
    {
    public:
      typedef shared_ptr<ParanoidObj> PType;
      
      /** allowed to call ctor because 
       *  it's a friend of ParanoidObj. 
       *  Note passing custom deleter. 
       */
      PType operator() (uint param)   { return PType (new ParanoidObj(param), &destroy); }
      
    protected:
      /** custom deleter func is allowed to call 
       *  ~ParanoidObj() because of friendship 
       */
      static void destroy (ParanoidObj* victim) { delete victim; }
      
    };
  
  
  
  /* === Factory instances ==============*/
  
  typedef Factory<MallocO> FactoryM;
  typedef factory::PImplFac<TargetO,ImplObj> FactoryP;
  
  static Factory1  placement_fac;
  static Factory2  paranoid_fac;
  static FactoryM  malloc_fac;
  static FactoryP  pimpl_fac;
  
  
  
  
  
  
  /*******************************************************************
   * testing the more advanced Factory variants and possibilities.
   * We use several customised Factory subclasses supporting custom
   * allocation, placement allocation, private constructors and
   * the PIpmpl design pattern. All creating smart pointers.
   */
  class Factory_special_test : public Test
    {
      virtual void run(Arg arg) 
        {
          uint num= isnil(arg)? 1 : lexical_cast<uint>(arg[1]);
          
          checkPlacement (num);
          checkPrivate (num);
          checkMalloc ();
          checkPImpl () ;
        }
      
      
      /** @test using direct object placement instead of heap allocation.
       *  Factory1 will place every new object into the same static buffer
       *  and return a refcounting pointer
       */
      void checkPlacement (uint cnt)
        {
          cout << "checkPlacement--------\n";
          
          typedef Factory1::PType P;
          format msg ("created %d shared_ptrs to Object placed in static buffer.\n");
          void* raw (0);
          P pX;
          CHECK (0 == pX.use_count());
          
            {
              P p1 (placement_fac (cnt));
              P p2 (p1);
              P pX = p2;
              
              cout << msg % p2.use_count() 
                   << string (*pX) << "\n";
              
              raw = p1.get(); // remember raw mem address
            }
            
          CHECK (0 == pX.use_count());
          
            {
              P p1 (placement_fac (cnt+1));
              P p2 (p1);
              P p3 (p1);
              P pX = p2;
              
              cout << msg % p2.use_count();
              
              CHECK (raw == p1.get(), "explicit object placement at fixed buffer doesn't work.");
            }
            
          CHECK (0 == pX.use_count());
        } 
      
      
      
      
      /** @test simple factory creating std::auto_ptr wrapped instances
       *  of an object with only private ctor and dtor. 
       */
      void checkPrivate (uint cnt)
        {
          cout << "checkPrivate--------\n";
          
          typedef Factory2::PType P;
          format msg ("created %d shared_ptrs to paranoid Object.\n");
          P pX;
          
          CHECK (0 == pX.use_count());
            {
              P p1 (paranoid_fac (cnt));
              P p2 (p1);
              P pX = p2;
              
              cout << msg % p2.use_count() 
                   << string (*pX) << "\n";
            }
          CHECK (0 == pX.use_count());
        }
      
      
      
      
      /** @test simple factory creating std::auto_ptr wrapped instances,
       *  but of a class using a custom allocation scheme (here implemented
       *  by direct C-style malloc calls)
       */
      void checkMalloc (void)
        {
          cout << "checkMalloc--------\n";
          
          typedef FactoryM::PType P;
          
          P p1 (malloc_fac ());
          P p2 = p1;
          cout << ("created auto_ptr to malloc-ed Object.\n")
               << string (*p2) << "\n";
          
          CHECK (!p1.get());
        }
      
      
      
      
      /** @test using direct object placement instead of heap allocation.
       *  Factory1 will place every new object into the same static buffer
       *  and return a refcounting pointer
       */
      void checkPImpl (void)
        {
          cout << "checkPImpl--------\n";
          
          typedef FactoryP::PType P;
          
          P p1 (pimpl_fac ());
          P p2 = p1;
          cout << ("created auto_ptr to Interface Object.\n");
          p2->funky(); // call a Interface function
          
          CHECK (!p1.get());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Factory_special_test, "unit common");
  
  
  
}} // lib::test
