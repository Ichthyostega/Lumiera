/* try.cpp  -  to try out and experiment with new features....
 *             scons will create the binary bin/try
 */
// 11/24 - how to define a bare object location comparison predicate
// 11/23 - prototype for grouping from iterator


/** @file try.cpp
 * Attempt to generalise the util::isSameObject, to support a mix of pointers and references.
 * This is a long standing requirement, yet I made several failed attempts in the past,
 * due to difficulties detecting a pointer after perfect-forwarding the argument. However,
 * it is not even necessary to perfect-forward, if all we want is to extract the raw address.
 * 
 * To my defence, I should add that I never took the time to conduct a proper experiment
 * in a stand-alone setup; investigating directly in "lib/util.hpp" is a waste of time.
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>

using lib::rani;
using std::move;
using std::string;

  template<class X>
  inline const void*
  getAd (X& x)
  {
    return static_cast<const void*> (std::addressof(x));
  }
  template<class X>
  inline const void*
  getAd (X* x)
  {
    return static_cast<const void*> (x);
  }

  template<class A, class B>
  inline bool
  isSameOb (A const& a, B const& b)
  {
    return getAd(a)
        == getAd(b);
  }
  

  struct Boo
    {
      string moo;
      short goo;
      
      Boo(short uh)
        : moo{util::toString(uh-1)}
        , goo(uh+1)
        { }
      
      operator string()  const
        {
          return moo+util::toString(goo);
        }
    };
  
  struct SuBoo : Boo
    {
      long poo = rani(goo);
      
      using Boo::Boo;
    };

    
    
    inline Boo*
    asBoo (void* mem)
      {// type tag to mark memory address as Buffer
        return static_cast<Boo*> (mem);
      }
  

int
main (int, char**)
  {
    Boo boo(23);
    Boo booo(23);
    Boo* boop = &boo;
    Boo const* const beep = boop;
    cout << boo << endl;
SHOW_EXPR(util::showAdr(getAd(boo )))
SHOW_EXPR(util::showAdr(getAd(&boo)))
SHOW_EXPR(util::showAdr(getAd(boop)))
SHOW_EXPR(util::showAdr(getAd(beep)))
SHOW_EXPR(isSameOb(boop, beep))
SHOW_EXPR(isSameOb(&boop, &beep))
SHOW_EXPR(isSameOb(boo, beep))
SHOW_EXPR(isSameOb(*beep, booo))
SHOW_EXPR(isSameOb(boo, boo.moo))
SHOW_EXPR(isSameOb(boo, &boo.moo))
SHOW_EXPR(isSameOb(boo.moo, booo))
SHOW_EXPR(isSameOb(booo, asBoo(&booo.moo)))
SHOW_EXPR(isSameOb(booo, asBoo(&booo.goo)))

    const void* voo = boop;
SHOW_EXPR(isSameOb(voo, boo))
SHOW_EXPR(isSameOb(voo, boop))
SHOW_EXPR(isSameOb(voo, asBoo(&boo.moo)))
SHOW_EXPR(isSameOb(voo, asBoo(&booo.moo)))
SHOW_EXPR(isSameOb(voo, asBoo(&boo.goo)))
    
    Boo&& roo = move(boo);
SHOW_EXPR(isSameOb(roo, boo))
SHOW_EXPR(isSameOb(voo, roo))
SHOW_EXPR(isSameOb(voo, Boo{roo}))

    SuBoo* suBoo = static_cast<SuBoo*>(&boo);
SHOW_EXPR(isSameOb(boo, suBoo))
SHOW_EXPR(isSameOb(boo, suBoo->moo))
SHOW_EXPR(isSameOb(voo, suBoo->moo))
SHOW_EXPR(isSameOb(voo, suBoo->poo))
    
    SuBoo sudo{*suBoo};
SHOW_EXPR(isSameOb(sudo, suBoo))
    suBoo = &sudo;
SHOW_EXPR(isSameOb(sudo.poo, suBoo->poo))
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
