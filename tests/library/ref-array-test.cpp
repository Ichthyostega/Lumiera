/*
  RefArray(Test)  -  unittest for wrapping with array-of-refs access

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file ref-array-test.cpp
 ** unit test \ref RefArray_test
 ** @deprecated 5/2024 rework underway
 ** @see several-buider-test.cpp
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "lib/ref-array-impl.hpp"

#include <vector>

using ::test::Test;
using std::vector;


namespace lib {
namespace test{
  
  namespace { // test types
    
    struct I
      {
        virtual int op(int i)  const =0;
        virtual ~I() {}
      };
    
    struct Sub1 : I
      {
        int offs_;
        
        Sub1 (int o=1) : offs_(o) {}
        
        int op (int i)  const { return i+offs_; }
      };
    
    struct Sub2 : I
      {
        const char* letterz_; 
        Sub2() : letterz_("ABCDEFGHIJKLMNOPQRSTUVWXYZ") {}
        
        int op (int i)  const { return (int)letterz_[i % 26]; }
      };
    
    struct Sub3 : I
      {
        int id_;
        static long sum;
        static long trigger;
        
        Sub3(int id) : id_(id)
          {
            sum +=id_;
            if ( id_ == trigger )
              throw trigger; // fail while in construction
          }
        ~Sub3()
          { 
            sum -=id_;
          }
        
        int op (int i)  const { return i + id_; }
      };
    
    long Sub3::sum = 0;
    long Sub3::trigger = -1;
    
    
  } // (END) test types
  
  
  
  
  
  
  
  /***************************************************************//**
   * @test build several wrappers, each based on a different storage,
   * all providing RefArray access to a given vector. The rationale
   * for RefArray is to expose just the interface: the data structure
   * within the actual implementation holds subclass instances of
   * the specified interface.
   * - RefArrayVectorWrapper is a ref to an existing vector
   * - RefArrayVector subclasses std::vector
   * - RefArrayTable holds a fix sized table, i.e. embedded storage
   * 
   * @see ref-array-impl.hpp
   */
  class RefArray_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          checkWrapper();
          checkVector();
          checkTable();
          checkTable_inplaceCreation();
          checkTable_errorHandling();
        }
      
      
      
      void
      checkWrapper()
        {
          vector<Sub2> subz(10);
          RefArrayVectorWrapper<I,Sub2> subWrap (subz);
          
          RefArray<I> & rArr (subWrap);
          
          CHECK (subWrap.size()==subz.size());
          CHECK (INSTANCEOF(I, &rArr[0]));
          for (size_t i=0; i<rArr.size(); ++i)
            {
              CHECK (&rArr[i] == &subz[i]);
              CHECK (rArr[i].op(i) == subz[i].op(i));
            }
        }
      
      void
      checkVector()
        {
          RefArrayVector<I,Sub2> subz(10);
          
          vector<Sub2> & vect (subz);
          RefArray<I> & rArr (subz);
          
          CHECK (vect.size()==subz.size());
          CHECK (INSTANCEOF(I, &rArr[0]));
          for (size_t i=0; i<rArr.size(); ++i)
            {
              CHECK (&rArr[i] == &vect[i]);
              CHECK (rArr[i].op(i) == vect[i].op(i));
            }
        }
      
      
#define ADR(OBJ) (ulong)&(OBJ)
      
      void
      checkTable()
        {
          RefArrayTable<I,20,Sub1> tab;  
          // creates 20 Sub1-objects in-place
          // which are indeed located within the object
          CHECK (sizeof(tab) >= 20 * sizeof(Sub1));
          CHECK (ADR(tab) < ADR(tab[19]) && ADR(tab[19]) < ADR(tab) + sizeof(tab));
          
          RefArray<I> & rArr (tab);
          
          CHECK (20 == tab.size());
          CHECK (INSTANCEOF(I, &rArr[0]));
          for (size_t i=0; i<rArr.size(); ++i)
            {
              CHECK (i*sizeof(Sub1) == ADR(rArr[i]) - ADR(rArr[0]) ); // indeed array-like storage
              CHECK (int(i+1) == rArr[i].op(i));                     //  check the known result
            }
        }
      
      template<class SUB>
      struct Fac ///< fabricating a series of subclass instances with varying ctor parameter
        {
          int offset_;
          Fac ( ) : offset_ (0) {}
          
          void operator() (void* place)
            {
              CHECK (place);
              new(place) SUB (offset_++); // note: varying ctor parameter
            }
        };
      
      
      void 
      checkTable_inplaceCreation()
        {
          Fac<Sub1> theFact;
          RefArrayTable<I,30,Sub1> tab (theFact);
          RefArray<I> & rArr (tab);
          CHECK (30 == tab.size());
          for (size_t i=0; i<rArr.size(); ++i)
            CHECK (int(i+i) == rArr[i].op(i)); // each one has gotten another offset ctor parameter
        }
      
      
      void 
      checkTable_errorHandling()
        {
          for (uint i=0; i<500; ++i)
            {
              Sub3::sum = 0;
              Sub3::trigger = rani(50);   // when hitting the trigger Sub3 throws
              try
                {
                  {
                    Fac<Sub3> factory;
                    RefArrayTable<I,30,Sub3> table (factory);
                    CHECK (Sub3::sum == (29+1)*29/2);
                  }
                  CHECK (Sub3::sum == 0);
                }
              
              catch(long id)
                {
                  CHECK (id == Sub3::trigger);
                  CHECK (Sub3::sum == id);
                  // meaning: all objects have been cleaned up,
                  // with the exception of the one hitting the trigger
            }   }
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (RefArray_test, "unit common");
  
  
  
}} // namespace lib::test
