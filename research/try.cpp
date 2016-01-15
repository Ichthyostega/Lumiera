/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>
// 4/08  - conversions on the value_type used for boost::any
// 5/08  - how to guard a downcasting access, so it is compiled in only if the involved types are convertible
// 7/08  - combining partial specialisation and subclasses 
// 10/8  - abusing the STL containers to hold noncopyable values
// 6/09  - investigating how to build a mixin template providing an operator bool()
// 12/9  - tracking down a strange "warning: type qualifiers ignored on function return type"
// 1/10  - can we determine at compile time the presence of a certain function (for duck-typing)?
// 4/10  - pretty printing STL containers with python enabled GDB?
// 1/11  - exploring numeric limits
// 1/11  - integer floor and wrap operation(s)
// 1/11  - how to fetch the path of the own executable -- at least under Linux?
// 10/11 - simple demo using a pointer and a struct
// 11/11 - using the boost random number generator(s)
// 12/11 - how to detect if string conversion is possible?
// 1/12  - is partial application of member functions possible?
// 5/14  - c++11 transition: detect empty function object
// 7/14  - c++11 transition: std hash function vs. boost hash
// 9/14  - variadic templates and perfect forwarding
// 11/14 - pointer to member functions and name mangling
// 8/15  - Segfault when loading into GDB (on Debian/Jessie 64bit
// 8/15  - generalising the Variant::Visitor
// 1/16  - generic to-string conversion for ostream
// 1/16  - generate receiver function with arbitrary arguments


/** @file try.cpp
 ** Metaprogramming: how to generate a function to accept a fixed set
 ** of typed arguments, where the sequence of arguments is given as type sequence
 ** or as variadic parameter pack. Also how to bind concrete argument values to
 ** such a function, where the concrete arguments come as runtime collection
 ** of variant holders.
 ** 
 */

typedef unsigned int uint;

//#include "lib/diff/gen-node.hpp"

#include "lib/time/timevalue.hpp"
#include "proc/control/command-def.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"

#include <functional>
#include <vector>
#include <string>

//using lib::diff::GenNode;
using proc::control::CommandSignature;
using proc::control::CommandDef;
using lib::time::Time;
using util::stringify;
using util::join;

using std::function;
using std::vector;
using std::string;

using VecS = vector<string>;


template<typename...ARGS>
void
operate (ARGS const& ...args)
  {
    VecS strs = stringify<VecS> (args...);
    cout << join (strs);
  }

template<typename...ARGS>
string
capture (ARGS const& ...args)
  {
    VecS strs = stringify<VecS> (args...);
    return join (strs);
  }

template<typename...ARGS>
void
undo (ARGS const& ...args)
  {
    VecS strs = stringify<VecS> (args...);
    cout << "UNDO..." << join (strs);
  }





#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;


int
main (int, char**)
  {
    cout << capture ("lalü", string("lala"), 12, 34L, 56.78) <<endl;
    

    auto ops = operate<double,Time>;
    
    using FunnySIG = lib::meta::_Fun<typeof(ops)>::Sig;
    
    using SIG_Opr = CommandSignature<FunnySIG, string>::OperateSig;
    using SIG_Cap = CommandSignature<FunnySIG, string>::CaptureSig;
    using SIG_Udo = CommandSignature<FunnySIG, string>::UndoOp_Sig;
    
    SHOW_TYPE (SIG_Opr);
    SHOW_TYPE (SIG_Cap);
    SHOW_TYPE (SIG_Udo);
    
    function<SIG_Opr> funny;
    function<SIG_Cap> capy;
    function<SIG_Udo> undy;
    
    cout << "funny? " << bool(funny) <<endl;
    
    funny = operate<double,Time>;
    capy = capture<double,Time>;
    undy = undo<double,Time,string>;
    cout << "funny? " << bool(funny) <<endl;
    
    cout << capy (98.7654321987654321987654321, Time(1,2,3,4)) <<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
