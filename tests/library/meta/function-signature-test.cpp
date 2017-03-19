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

using lib::meta::_Fun;

using std::function;
using std::placeholders::_1;
using std::bind;
using std::string;
using std::tuple;
using std::move;


namespace lib  {
namespace meta {
namespace test {
    
    
    namespace { // test subjects
int
funny (uint i)
{
  return -i+1;
}

struct Funky
  {
    int ii = 2;
    
    int
    fun (uint i2)
      {
        return ii + funny(i2);
      }
    
    int
    operator() (uint i2)
      {
        return 2*ii - fun(i2);
      }
    
    static int
    notfunny (uint i)
      {
        return 2*funny (i);
      }
  };



#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;

#define EVAL_PREDICATE(_PRED_) \
    cout << STRINGIFY(_PRED_) << "\t : " << _PRED_ <<endl;


template<typename F>
void
showType (F)
  {
    using Sig = typename _Fun<F>::Sig;
    
    SHOW_TYPE (F);
    SHOW_TYPE (Sig);
  }

template<typename F>
void
showRef (F&)
  {
    using Sig = typename _Fun<F>::Sig;
    
    SHOW_TYPE (F);
    SHOW_TYPE (Sig);
  }

template<typename F>
void
showCRef (F&)
  {
    using Sig = typename _Fun<F>::Sig;
    
    SHOW_TYPE (F);
    SHOW_TYPE (Sig);
  }

template<typename F>
void
showRRef (F&&)
  {
    using Sig = typename _Fun<F>::Sig;
    
    SHOW_TYPE (F);
    SHOW_TYPE (Sig);
  }
    } // (End) test subjects


using Fun = function<int(uint)>;
using Fuk = function<int(Funky&, uint)>;
  
  
  
  
  
  
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
    Fun f1{funny};
    Fun f2{&funny};
    
    Fun f3{Funky::notfunny};
    Fun f4{&Funky::notfunny};
    
    auto memfunP = &Funky::fun;
    
    Fuk f5{memfunP};
    
    Funky funk;
    
    Fun f6{bind (f5, funk, _1)};
    
    auto lambda = [&](uint ii) { return funk.fun(ii); };
    
    Fun f7{lambda};
    
    showType (funny);
    showType (&funny);
    showType (Funky::notfunny);
    
    showType (memfunP);
    showType (lambda);
    showType (f7);
    
    cout << "\n\n-------\n";
    
    showRef (funny);
    showRef (lambda);
    showRef (f7);
    
    showCRef (funny);
    showCRef (lambda);
    showCRef (f7);
    
    showRRef (move(lambda));
    showRRef (move(f7));
    
    showType (move(&funny));
    showType (move(lambda));
    showType (move(f7));
    
    Fun& funRef = f1;
    Funky& funkyRef = funk;
    Fun const& funCRef = f1;
    Funky const& funkyCRef = funk;
    showType (funRef);
    showType (funkyRef);
    showType (funCRef);
    showType (funkyCRef);
    
    cout << "\n\n-------\n";
    
    SHOW_TYPE (decltype(&Funky::operator()));
    SHOW_TYPE (decltype(lambda));
    
    SHOW_TYPE (_Fun<int(uint)>::Sig);
    SHOW_TYPE (_Fun<Fun&>::Sig);
    SHOW_TYPE (_Fun<Fun&&>::Sig);
    SHOW_TYPE (_Fun<Fun const&>::Sig);
    SHOW_TYPE (_Fun<Funky&>::Sig);
    SHOW_TYPE (_Fun<Funky&&>::Sig);
    SHOW_TYPE (_Fun<Funky const&>::Sig);
    
    using Siggy = _Fun<Fun>::Sig;
    SHOW_TYPE (_Fun<Siggy&>::Sig);
    SHOW_TYPE (_Fun<Siggy&&>::Sig);
    SHOW_TYPE (_Fun<Siggy const&>::Sig);
    
    cout <<  "\n.gulp.\n";
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctionSignature_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
