/* try.cpp  -  to try out and experiment with new features....
 *             scons will create the binary bin/try
 */
// 06/25 - investigate function type detection of std::bind Binders
// 12/24 - investigate problem when perfect-forwarding into a binder
// 12/24 - investigate overload resolution on a templated function similar to std::get
// 11/24 - how to define a bare object location comparison predicate
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Investigate ambiguities regarding the function type of standard binders.
 * The Binder objects returned from `std::bind` provide a set of overloaded
 * function call `operator()` (with variants for c/v). Unfortunately this defeats
 * the common techniques to detect a function signature from a callable, when
 * only a concrete instance of such a binder is given. Furthermore, looking
 * at the definition of `class _Bind_result<_Result, _Functor(_Bound_args...)>`
 * in my implementation of the C++ Stdlib, it seems we are pretty much out
 * of luck, and even `std::function` fails with the template argument detection.
 * A possible workaround could be to wrap the Binder object immediately into
 * a lambda, but only if the actual types for the argument list can be
 * provided directly to a template to generate this λ-wrapper.
 * @note there is a nasty twist regarding const-correctness, which almost made
 *       this workaround fail altogether. The overloaded operator() from std::bind
 *       serves the same purpose (to deal with const/volatile), and this is the
 *       very reason that defeats the detection of the function signature.
 *       The workaround attempts to expose precisely one function call operator,
 *       and this becomes problematic as soon as the resulting object is processed
 *       further, and maybe bound into another lambda capture. Thus we define the
 *       wrapper class explicitly, so that any const-ness can be cast away.
 *       This turns out to be necessary in tuple-closure.hpp.
 */


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/variadic-rebind.hpp"
#include "lib/util.hpp"

#include <functional>

using std::forward;
using std::placeholders::_1;
using lib::meta::_Fun;


void
fun (int& a)
  {
    std::cout << a << std::endl;
  }

short
fup (long l, long long ll)
  {
    return short(l - ll);
  }



/** WORKAROUND: wrap a binder to yield clear function signature */
template<typename...ARGS>
struct AdaptInvokable
  {
    template<class FUN>
    static auto
    buildWrapper (FUN&& fun)
      {

        struct Wrap
          {
            FUN fun_;
            
            Wrap(FUN&& f) : fun_{forward<FUN>(f)} { }
            
            auto
            operator() (ARGS... args)
              {
                return fun_(forward<ARGS>(args)...);
              }
          };
        
        return Wrap{forward<FUN>(fun)};
///////////////////////////////////////////////////////////// NOTE
///////////////////////////////////////////////////////////// can not use a Lambda, since we're then trapped        
///////////////////////////////////////////////////////////// in an unsurmountable mixture of const and non-const        
//      return [functor = forward<FUN>(fun)]
//             (ARGS... args) mutable
//              {
//                return functor (forward<ARGS> (args)...);
//              };
      }
  };

template<class TYPES, class FUN>
auto
buildInvokableWrapper (FUN&& fun)
  {
    using ArgTypes = typename TYPES::Seq;
    using Builder = typename lib::meta::RebindVariadic<AdaptInvokable, ArgTypes>::Type;
    
    return Builder::buildWrapper (forward<FUN> (fun));
  }



int
main (int, char**)
  {
    SHOW_EXPR (fup(2,3))
    auto bup = std::bind (fup, _1, 5);
    SHOW_EXPR (bup)
    using Bup = decltype(bup);
    using Fub = _Fun<Bup>;
    SHOW_TYPE (Bup)
    SHOW_TYPE (Fub)
//  using Sub = Fub::Sig;    ////////////////Problem: does not compile
    
    using Fut = decltype(fun);
    SHOW_TYPE (_Fun<Fut>::Sig)
    
    auto wup = buildInvokableWrapper<lib::meta::Types<int>>(bup);
    
    using Wup = decltype(wup);
    using WupSig = _Fun<Wup>::Sig;
    SHOW_TYPE (WupSig);
    SHOW_EXPR (wup(3))
    SHOW_EXPR (sizeof(bup))
    SHOW_EXPR (sizeof(wup))
    
    auto waua = buildInvokableWrapper<lib::meta::Types<>> (std::bind (fun, 55));
    waua ();
    SHOW_TYPE (_Fun<decltype(waua)>::Sig)
    
    cout <<  "\n.gulp." <<endl;
    return 0;
  }
