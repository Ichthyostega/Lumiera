/*
  FunctionSignature(Test)  -  metaprogramming to extract function signature type

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file function-signature-test.cpp
 ** unit test \ref FunctionSignature_test
 */


#include "lib/test/run.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/meta/function.hpp"

#include <functional>
#include <string>


using std::function;
using std::placeholders::_1;
using std::bind;
using std::string;
using std::tuple;
using std::move;


namespace lib  {
namespace meta {
namespace test {
  
  using lib::meta::_Fun;
  using lib::meta::typeStr;
  
  
  namespace { // test subjects
    
    int
    freeFun (uint i)
    {
      return -i+1;
    }
    
    struct Functor
      {
        int ii = 2;
        
        int
        fun (uint i2)
          {
            return ii + freeFun(i2);
          }
        
        int
        operator() (uint i2)
          {
            return 2*ii - fun(i2);
          }
        
        static int
        staticFun (uint i)
          {
            return 2*freeFun (i);
          }
      };
    
    
    /* ===== diagnostics helper ===== */
    
    template<typename F>
    string
    showSig (F)
    {
      return typeStr<typename _Fun<F>::Sig>();
    }
    
    template<typename F>
    string
    showSigRef (F&)
    {
      return typeStr<typename _Fun<F>::Sig>();
    }
    
    template<typename F>
    string
    showSigCRef (F&)
    {
      return typeStr<typename _Fun<F>::Sig>();
    }
    
    template<typename F>
    string
    showSigRRef (F&&)
    {
      return typeStr<typename _Fun<F>::Sig>();
    }
    
  } // (End) test subjects
  
  
  using Func = function<int(uint)>;
  using FuncF = function<int(Functor&, uint)>;
  
  
  
  
  
  
  /*********************************************************************//**
   * @test verify metaprogramming trait to pick up function signature types.
   *       - ability to handle _function like_ entities uniformly
   *       - can handle function references, function pointers,
   *         member pointer to function, functor objects,
   *         `std::function` and lambdas
   *       - supports arbitrary number of arguments
   * @see lib::meta::_Fun
   * @see typelist.hpp
   * @see FunctionClosure_test
   */
  class FunctionSignature_test : public Test
    {
      virtual void
      run (Arg) 
        {
          // this is how the key trick of the _Fun traits template works:
          // for anything "function like" we retrieve a member-pointer to the function call operator
          // which we then pass on to the dedicated overload for member pointers
          CHECK ("int (Functor::*)(unsigned int)" == typeStr<decltype(&Functor::operator())>());
          
          
          Func f1{freeFun};
          Func f2{&freeFun};
          
          Func f3{Functor::staticFun};
          Func f4{&Functor::staticFun};
          
          
          Functor funk;
          
          
          auto lambda = [&](uint ii) { return funk.fun(ii); };
          
          Func f5{lambda};
          
          CHECK ("int (unsigned int)" == showSig (freeFun));
          CHECK ("int (unsigned int)" == showSig (&freeFun));
          CHECK ("int (unsigned int)" == showSig (Functor::staticFun));
          CHECK ("int (unsigned int)" == showSig (lambda));
          CHECK ("int (unsigned int)" == showSig (f5));
          
          
          CHECK ("int (unsigned int)" == showSigRef (freeFun));
          CHECK ("int (unsigned int)" == showSigRef (lambda));
          CHECK ("int (unsigned int)" == showSigRef (f5));
          
          CHECK ("int (unsigned int)" == showSigCRef (freeFun));
          CHECK ("int (unsigned int)" == showSigCRef (lambda));
          CHECK ("int (unsigned int)" == showSigCRef (f5));
          
          CHECK ("int (unsigned int)" == showSigRRef (move(lambda)));
          CHECK ("int (unsigned int)" == showSigRRef (move(f5)));
          
          CHECK ("int (unsigned int)" == showSig (move(&freeFun)));
          CHECK ("int (unsigned int)" == showSig (move(lambda)));
          CHECK ("int (unsigned int)" == showSig (move(f5)));
          
          
          Func& funRef = f1;
          Functor& funkyRef = funk;
          Func const& funCRef = f1;
          Functor const& funkyCRef = funk;
          CHECK ("int (unsigned int)" == showSig (funRef));
          CHECK ("int (unsigned int)" == showSig (funkyRef));
          CHECK ("int (unsigned int)" == showSig (funCRef));
          CHECK ("int (unsigned int)" == showSig (funkyCRef));
          
          
          CHECK ("int (unsigned int)" == typeStr<_Fun<int(uint)>::Sig     >());
          CHECK ("int (unsigned int)" == typeStr<_Fun<Func&>::Sig         >());
          CHECK ("int (unsigned int)" == typeStr<_Fun<Func&&>::Sig        >());
          CHECK ("int (unsigned int)" == typeStr<_Fun<Func const&>::Sig   >());
          CHECK ("int (unsigned int)" == typeStr<_Fun<Functor&>::Sig      >());
          CHECK ("int (unsigned int)" == typeStr<_Fun<Functor&&>::Sig     >());
          CHECK ("int (unsigned int)" == typeStr<_Fun<Functor const&>::Sig>());
          
          using Siggy = _Fun<Func>::Sig;
          CHECK ("int (unsigned int)" == typeStr<_Fun<Siggy&>::Sig        >());
          CHECK ("int (unsigned int)" == typeStr<_Fun<Siggy&&>::Sig       >());
          CHECK ("int (unsigned int)" == typeStr<_Fun<Siggy const&>::Sig  >());
          
          
          auto memfunP = &Functor::fun;
          FuncF fM{memfunP};
          Func fMF{bind (fM, funk, _1)};
          
          CHECK ("int (unsigned int)"           == typeStr<_Fun<decltype(memfunP)>::Sig>());
          CHECK ("int (Functor&, unsigned int)" == typeStr<_Fun<decltype(fM)>::Sig     >());
          CHECK ("int (unsigned int)"           == typeStr<_Fun<decltype(fMF)>::Sig    >());
          
          
          // _Fun<F> can be used for metaprogramming with enable_if
          CHECK (    _Fun<Func>::value);      // yes : a Functor
          CHECK (    _Fun<int(long)>::value); // yes : a function type
          CHECK (not _Fun<int>::value);       // no  : a type without function call operator
          
          auto lambda1 = [](int i)  { return double(i) / (i*i); };
          auto lambda2 = [](auto i) { return double(i) / (i*i); };
          
          using TLamb1 = decltype(lambda1);
          using TLamb2 = decltype(lambda2);
          
          CHECK (    _Fun<TLamb1>::value);    // yes : detect signature of lambda
          CHECK (not _Fun<TLamb2>::value);    // no  : can not detect signature of a generic lambda!
          
          // but detection works, once the templated operator() has been instantiated to some fixed type
          auto stdFunction = function<double(float)> (lambda2);
          CHECK (    _Fun<decltype(stdFunction)>::value);
          
          CHECK ("double (int)"   == showSig (lambda1));
          CHECK ("double (float)" == showSig (stdFunction));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctionSignature_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
