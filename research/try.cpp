/* try.cpp  -  to try out and experiment with new features....
 *             scons will create the binary bin/try
 */
// 12/24 - investigate problem when perfect-forwarding into a binder
// 12/24 - investigate overload resolution on a templated function similar to std::get
// 11/24 - how to define a bare object location comparison predicate
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Partially binding / closing arguments of a function with _perfect forwarding_ can be problematic.
 * The problem was encountered in the steam::engine::TypeHandler::create() - function with additional
 * constructor arguments. Obviously, we want these to be _perfect forwarded_ into the actual constructor,
 * but the binding must store a captured copy of these values, because the handler can be used repeatedly.
 * 
 * The actual problem is caused by the instantiation of the target function, because the arguments are
 * also passed into the binding mechanism by _perfect forwarding._ The target function template will thus
 * be instantiated to expect RValues, but the binder can only pass a copy by-reference. At this point then
 * the problem materialises (with a rather confusing error message).
 * 
 * The Problem was already discussed on [Stackoverflow]
 * 
 * A simple workaround is to change the types in the instantiation into references;
 * obviously this can not work for some argument types; if a more elaborate handling is necessary,
 * the [handling of bound arguments] should be considered in detail.
 * 
 * [Stackoverflow]: https://stackoverflow.com/q/30968573/444796
 * [handling of bound arguments]: http://en.cppreference.com/w/cpp/utility/functional/bind#Member_function_operator.28.29
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/util.hpp"

#include <functional>

using std::cout;
using std::endl;
using std::forward;
using std::placeholders::_1;

template<typename...ARGS>
inline void
dummy (int extra, ARGS&& ...args)
  {
    cout << extra <<"▷";
    ((cout << forward<ARGS>(args) << "•"), ...)
           << endl;
  }

template<typename...ARGS>
auto
bound (ARGS&& ...args)
  {
    return std::bind (dummy<ARGS&...>, _1, forward<ARGS>(args) ...);
  }

void
fun (int&& a)
  {
    std::cout << a << std::endl;
  }



int
main (int, char**)
  {
    dummy (55,2,3,5,8);
    
    auto bun = bound (2,3,5);
    using Bun = decltype(fun);
SHOW_TYPE(Bun)
    bun (55);

    auto bi = std::bind (fun, 55);
//  bi();                       /////////// this invocation does not compile, because the Binder passes a copy to the RValue-Ref
    
    cout <<  "\n.gulp." <<endl;
    return 0;
  }
