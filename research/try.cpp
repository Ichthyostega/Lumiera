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


/** @file try.cpp
 ** Investigation: how to supply a hash function for custom types.
 ** This example defines two custom types, each of which provides a way
 ** to calculate hash values. But in one case, we use the new \c std::hash
 ** as a framework, in the other case we use the extension mechanism for
 ** \c boost::hash. The latter has the benefit of being simpler and less verbose
 ** to write, since a simple ADL function is sufficient as extension point
 ** 
 ** Now it would be desirable to bridge automatically between the two systems of
 ** defining hash functions. Unfortunately, the standard library since GCC 4.7
 ** contains a default implementation of std::hash to trigger a static assertion.
 ** Probably this was meant to educate people, but has the adverse side effect to
 ** prohibit any metaprogramming through SFINAE
 ** 
 ** This mistake was corrected somewhere in the 4.8.x series, but in the meantime
 ** we'll use the dirty trick proposed by "enobayram" to hijack and fix the problematic
 ** definition from the standard library.
 ** http://stackoverflow.com/questions/12753997/check-if-type-is-hashable
 **
 */

#include <cstddef>

namespace std {
  
  template<typename Result, typename Arg>
  struct __hash_base;
  
  /**
   * Primary class template for std::hash.
   * We provide no default implementation, but a marker type
   * to allow detection of custom implementation through metaprogramming
   */
  template<typename TY>
  struct hash
    : public __hash_base<size_t, TY>
    {
      
/////// deliberately NOT defined to allow for SFINAE...
//    
//    static_assert (sizeof(TY) < 0, "std::hash is not specialized for this type");
//    size_t operator()(const _Tp&) const noexcept;
      
      typedef int NotHashable;
    };
  
}



#define hash hash_HIDDEN
#define _Hash_impl _Hash_impl_HIDDEN
#include <functional>
#undef hash
#undef _Hash_impl


namespace std {
  
  struct _Hash_impl
    : public std::_Hash_impl_HIDDEN
    {
      template<typename ... ARGS>
      static auto
      hash (ARGS&&... args) -> decltype(hash_HIDDEN (std::forward<ARGS>(args)...))
        {
          return hash_HIDDEN (std::forward<ARGS>(args)...);
        }
    };

#define STD_HASH_IMPL(_TY_) \
  template<> struct hash<_TY_> : public hash_HIDDEN<_TY_> { };

  STD_HASH_IMPL (bool)
  STD_HASH_IMPL (char)
  STD_HASH_IMPL (signed char)
  STD_HASH_IMPL (unsigned char)
  STD_HASH_IMPL (wchar_t)
  STD_HASH_IMPL (char16_t)
  STD_HASH_IMPL (char32_t)
  STD_HASH_IMPL (short)
  STD_HASH_IMPL (int)
  STD_HASH_IMPL (long)
  STD_HASH_IMPL (long long)
  STD_HASH_IMPL (unsigned short)
  STD_HASH_IMPL (unsigned int)
  STD_HASH_IMPL (unsigned long)
  STD_HASH_IMPL (unsigned long long)
  STD_HASH_IMPL (float)
  STD_HASH_IMPL (double)
  STD_HASH_IMPL (long double)
  
#undef STD_HASH_IMPL
}

#include <boost/functional/hash.hpp>

//#include <type_traits>
#include <iostream>
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::cout;
using std::endl;

class S
  {
    string s_;
    
    friend std::hash<S>;
    
  public:
    S(string ss ="")
      : s_(ss)
      { }
  };

namespace std {
  template<>
  struct hash<S>
    {
      size_t
      operator() (S const& val)  const noexcept
        {
          hash<string> string_hasher;
          return string_hasher(val.s_);
        }
    };
}

class V
  {
    vector<string> v_;
    
  public:
    V(string ss ="")
      {
        v_.push_back(ss);
      }
    
    friend size_t
    hash_value (V const& v)
    {
      return boost::hash_value(v.v_);
    }
  };


int
main (int, char**)
  {
    string p("Путин"), pp(p);
    S s(p), ss(pp);
    V v(p), vv(pp);
    
    std::hash<string>   std_stringHasher;
    boost::hash<string> boo_stringHasher;
    
    std::hash<S>   std_customHasher;
    boost::hash<V> boo_customHasher;
    
    std::hash<V>   boo2std_crossHar;
    boost::hash<S> std2boo_crossHar;
    
    cout << "raw hash(std) =      "   << std_stringHasher(p) <<"|"<< std_stringHasher(pp)
         << "\n      (boost) =      " << boo_stringHasher(p) <<"|"<< boo_stringHasher(pp)
         << "\n custom hash (std)   " << std_customHasher(s) <<"|"<< std_customHasher(ss)
         << "\n custom hash (boost) " << boo_customHasher(v) <<"|"<< boo_customHasher(vv)
//       << "\n use std from boost: " << std2boo_crossHar(s) <<"|"<< std2boo_crossHar(ss)
//       << "\n use boost from std: " << boo2std_crossHar(v) <<"|"<< boo2std_crossHar(vv)
         ;
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
