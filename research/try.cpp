/* try.cpp  -  to try out and experiment with new features....
 *             scons will create the binary bin/try
 */
// 09/26 - ways to build a cyclically wrapping number domain
// 03/26 - runtime index access and iteration over tuple-likes
// 06/25 - provide a concept to accept _tuple-like_ objects
// 06/25 - investigate function type detection of std::bind Binders
// 12/24 - investigate problem when perfect-forwarding into a binder
// 12/24 - investigate overload resolution on a templated function similar to std::get
// 11/24 - how to define a bare object location comparison predicate
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Investigate techniques for creating a cyclically wrapping numeric domain.
 * Actually this could be achieved with my `floordiv()` utility, yet the ramifications
 * were not clear -- so I settled to build a stand-alone solution, based on the naive modulus
 * technique, attempting to fix the negative offsets. It became clear that this approach would
 * have to consider the domain boundaries and identify cases that could not be handlet. At that
 * point, I used Claude to scrutinise and improve my draft, leading to a generic tool that is
 * now relocated into the \ref util-quant.hpp header
 * @see lib::par::Scale
 */


#include "lib/format-cout.hpp"
#include "lib/util-quant.hpp"
#include "lib/util.hpp"

#include <cmath>
    
    template<typename NUM>
    constexpr NUM _MAX = std::numeric_limits<NUM>::max();
    
    template<typename NUM>
    constexpr NUM _MIN = std::numeric_limits<NUM>::lowest();
  
  
  
  
template<typename N>
void
showCase (N val, N minVal, N maxVal)
{
  cout << "val:"<<val<<" (min:"<<minVal<<" , max:"<<maxVal<<") ->"<< util::cyclicWrap (val,minVal,maxVal)<<endl;
}

template<typename N>
void
showFloorDiv (N num, N den)
{
  cout << "FloorDiv:"<<num<<" / "<<den<<"  ->"<< util::floordiv(num,den)<<endl;
}

template<typename N>
void
showFloorWrap (N num, N den)
{
  auto fwrap = util::floorwrap (num,den);
  cout << "FloorWrap:"<<num<<" / "<<den<<"  -> q="<<fwrap.quot<<" r="<<fwrap.rem<<endl;
}


int
main (int, char**)
  {
    const int PER = 3;
    int i = -5;
    int d = i / PER;
    cout << "d="<<d<<endl;
    cout << "fd="<<util::floordiv(i,PER)<<endl;
    
    uint u = _MAX<uint>;
    
    cout << u << " fd="<<util::floordiv(u,uint(PER))<<" ...re="<<PER*util::floordiv(u,uint(PER))<<endl;
    
    showFloorDiv (7,5);
    showFloorDiv (-7,5);
    showFloorDiv (7,-5);
    showFloorDiv (-7,-5);
    
    showFloorWrap (7,5);
    showFloorWrap (-7,5);
    showFloorWrap (7,-5);
    showFloorWrap (-7,-5);

    showFloorWrap (_MAX<int>,1);
    showFloorWrap (_MAX<int>,2);
    showFloorWrap (_MAX<int>,5);
    
    showFloorWrap (_MIN<int>,1);
    showFloorWrap (_MIN<int>,2);
    showFloorWrap (_MIN<int>,5);
    
    showFloorWrap (_MAX<int>,-1);
    showFloorWrap (_MAX<int>,-2);
    showFloorWrap (_MAX<int>,-5);
    
    showFloorDiv (_MAX<int>,1);
    showFloorDiv (_MAX<int>,2);
    showFloorDiv (_MAX<int>,5);
    
    showFloorDiv (_MIN<int>,1);
    showFloorDiv (_MIN<int>,2);
    showFloorDiv (_MIN<int>,5);
    
    showFloorDiv (_MAX<int>,-1);
    showFloorDiv (_MAX<int>,-2);
    showFloorDiv (_MAX<int>,-5);
    
    showFloorWrap (_MAX<uint>,1u);
    showFloorWrap (_MAX<uint>,2u);
    showFloorWrap (_MAX<uint>,5u);
    
    showFloorDiv (_MAX<uint>,1u);
    showFloorDiv (_MAX<uint>,2u);
    showFloorDiv (_MAX<uint>,5u);
    
    showCase<int>(-7,-5,5);
    showCase<int>(-6,-5,5);
    showCase<int>(-5,-5,5);
    showCase<int>(-4,-5,5);
    showCase<int>(+5,-5,5);
    showCase<int>(+6,-5,5);
    showCase<int>(+7,-5,5);
    showCase<int>(+8,-5,5);
    showCase<int>(+9,-5,5);
    showCase<int>(+10,-5,5);
    showCase<int>(+11,-5,5);
    showCase<int>(+12,-5,5);
    showCase<int>(+14,-5,5);
    showCase<int>(+15,-5,5);
    showCase<int>( -7,-10,-7);
    showCase<int>( -8,-10,-7);
    showCase<int>( -9,-10,-7);
    showCase<int>(-10,-10,-7);
    showCase<int>(-11,-10,-7);
    showCase<int>(-12,-10,-7);
    showCase<int>(-13,-10,-7);
    showCase<int>(-14,-10,-7);
    showCase<int>(-15,-10,-7);
    
    showCase<uint>(5,1000,1003);

    showCase<float>(0.2 ,-2,-1.8);
    showCase<float>(0   ,-2,-1.8);
    showCase<float>(-0.8,-2,-1.8);
    showCase<float>(-1.79,-2,-1.8);
    showCase<float>(-1.8,-2,-1.8);
    showCase<float>(-1.81,-2,-1.8);
    showCase<float>(-1.99,-2,-1.8);
    showCase<float>(-2   ,-2,-1.8);
    showCase<float>(-2.01,-2,-1.8);
    
    
    cout <<  "\n.gulp." <<endl;
    return 0;
  }
