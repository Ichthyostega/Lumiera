/*
  VariadicArgumentPicker(Test)  -  access individual arguments from a variadic template

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file variadic-argument-picker-test.cpp
 ** verify metaprogramming manipulation on a variadic parameter pack.
 ** The unittest VariadicArgumentPicker_test calls a variadic function with different
 ** number of arguments and it employs variadic-helper.hpp to alter the order of passed arguments.
 ** 
 ** The primary difficulty when dealing with variadic templates is the fact that a variadic
 ** parameter pack is not a first class type, rather a special language construct which can only
 ** be used to perform a pattern match. For this reason, metaprogramming has to proceed "backwards",
 ** by invoking a delegate template, thereby dissecting the parameter pack by a pattern match.
 ** The key technique for more elaborate manipulation is to construct a variadic index sequence
 ** as a helper, and then to expand that index sequence to drive instantiation of a helper
 ** template on individual arguments.
 **
 ** @see variadic-helper.hpp
 ** @see path-array.hpp
 ** @see TypeSeqManipl_test
 ** @see TupleRecordInit_test
 ** @see TypeListManip_test
 **
 */


#include "lib/test/run.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"

#include <utility>

using util::_Fmt;
using util::join;
using util::toString;
using std::forward;
using std::string;


namespace lib  {
namespace meta {
namespace test {
  
  
  namespace { // test data
    
    template<int n>
    struct N
      {
        uint i_;
        static int instanceCnt;
        
        N (uint x = rani(1 + abs(n)))
          : i_{x}
          {
            ++instanceCnt;
          }
       ~N()
          {
            --instanceCnt;
          }
        N (N const& r)
          : i_{r.i_}
          {
            ++instanceCnt;
          }
        N (N&& rr)
          {
            std::swap (i_, rr.i_);
            ++instanceCnt;
          }
        
        // instanceCnt remains same...  
        N& operator= (N const&) = default;
        N& operator= (N&&)      = default;
        
        operator string()  const
          {
            static _Fmt format{"%s──%s─"};
            return format % typeStr(*this) % i_;
          }
        friend bool operator== (N const& l, N const& r) { return l.i_ == r.i_; } 
        friend bool operator!= (N const& l, N const& r) { return l.i_ != r.i_; } 
      };
    
    template<int n>
    int N<n>::instanceCnt = 0;
    
    
    /**
     * Variadic test function to invoke
     */
    template<class...ARGS>
    string
    fun (ARGS&& ...args)
    {
      static _Fmt format{"%2d╎%s┤"};
      return format % sizeof...(ARGS) % join({toString(args)...}, "┼");
    }
  } // (End) test data
  
  
  
  
  
  
  /****************************************************************************//**
   * @test check the basic utilities for manipulating variadic template arguments.
   *       - single out and pick an argument designated by index
   *       - pick an argument with fallback on default construction
   *       - demonstrate usage by reordering the position of arguments
   */
  class VariadicArgumentPicker_test : public Test
    {
      virtual void
      run (Arg) 
        {
          seedRand();
          verify_fixture();
          check_pickArg ();
          check_pickInit();
          check_reorderedArguments();
          
          CHECK (0 == N<0>::instanceCnt);
          CHECK (0 == N<1>::instanceCnt);
          CHECK (0 == N<2>::instanceCnt);
          CHECK (0 == N<3>::instanceCnt);
        }
      
      
      void
      verify_fixture ()
        {
          CHECK (0 == N<0>::instanceCnt);
          CHECK (0 == N<1>::instanceCnt);
          CHECK (0 == N<2>::instanceCnt);
          CHECK (0 == N<3>::instanceCnt);
          {
            N<1> n1;
            N<2> n2;
            N<3> n3;
            N<3> nn{n3};
            cout << fun (n1,n2,n3,nn) << endl;
            
            CHECK (0 == N<0>::instanceCnt);
            CHECK (1 == N<1>::instanceCnt);
            CHECK (1 == N<2>::instanceCnt);
            CHECK (2 == N<3>::instanceCnt);

          }
          CHECK (0 == N<0>::instanceCnt);
          CHECK (0 == N<1>::instanceCnt);
          CHECK (0 == N<2>::instanceCnt);
          CHECK (0 == N<3>::instanceCnt);
        }
      
      
      void
      check_pickArg ()
        {
          N<1> n1;
          N<2> n2;
          N<3> n3;
          
          CHECK (n1 == pickArg<0> (n1,n2,n3));
          CHECK (n2 == pickArg<1> (n1,n2,n3));
          CHECK (n3 == pickArg<2> (n1,n2,n3));

          // does not compile...
//        pickArg<3> (n1,n2,n3);
          
          N<0> n0{42};
          CHECK (n0 != pickArg<0> (N<0>{23}));
          CHECK (n0 == pickArg<0> (N<0>{n0}));
        }
      
      
      void
      check_pickInit ()
        {
          N<1> n1;
          N<2> n2;
          using N0 = N<0>;
          
          CHECK (1 == (pickInit<0,int> (1,2) ));
          CHECK (2 == (pickInit<1,int> (1,2) ));
          CHECK (0 == (pickInit<2,int> (1,2) ));
          
          CHECK (n1 == (pickInit<0,N0> (n1,n2) ));
          CHECK (n2 == (pickInit<1,N0> (n1,n2) ));
          
          CHECK ("N<0>" == typeStr(pickInit<3,N0> (n1,n2)));
          CHECK ("N<0>" == typeStr(pickInit<3,N0> (1,"2")));
          CHECK ("N<0>" == typeStr(pickInit<3,N0> ()));
        }
      
      
      
      /**
       * Demonstration of argument manipulation through metaprogramming.
       * This function invokes the ubiquitous `fun` test function with arbitrary arguments,
       * but it re-orders and even prunes arguments as dictated by the Index sequence parameter.
       * @remarks Note some fine points:
       *          - the value of the parameter `IndexSeq<idx...>` is irrelevant
       *          - rather, its sole purpose is to _pattern match_ against the type
       *          - we use `IndexSeq` just as an container to hold the sequence `idx...`
       *          - the expression within the function argument list expands the argument packs
       *          - and both of them are treated separately
       *          - the actual arguments are wrapped into a `std::forward` for _perfect forwarding_
       *          - but the key trick of the whole operation lies in the expansion of the `idx...` pack
       *          - note that `idx` without the `...` sits within the template list of `pickArg<idx>`
       *          - while the corresponding expansion operator is outmost and thus works on the whole expression
       *          - and thus it is the `idx...` pack which actually drives the generation of several `pickArg` instantiations
       *          - in the end the net effect is that the _Index sequence_ absolutely dictates which arguments are picked 
       */
      template<class...ARGS, size_t...idx>
      static auto
      call_with_reversed_arguments (IndexSeq<idx...>, ARGS&& ...args)
        {
          return fun (pickArg<idx> (forward<ARGS>(args)...) ...);
        }
      
      /** @test demonstrate reordering of arguments */
      void
      check_reorderedArguments ()
        {
          N<0> n0;
          N<1> n1;
          N<2> n2;
          N<3> n3;
          
          cout << fun (n0,n1,n2,n3) << endl;
          
          using Backwards = typename BuildIndexSeq<4>::Descending;    // 3,2,1,0
          using Back2     = typename BuildIndexSeq<2>::Descending;    // 1,0
          using After2    = typename BuildIndexSeq<4>::After<2>;      // 2,3
          
          cout << call_with_reversed_arguments (Backwards(), n0,n1,n2,n3)  <<endl;
          cout << call_with_reversed_arguments (Back2()    , n0,n1,n2,n3)  <<endl;
          cout << call_with_reversed_arguments (After2()   , n0,n1,n2,n3)  <<endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (VariadicArgumentPicker_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
