/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>


#include <nobug.h>
#include <iostream>
#include <typeinfo>

#include "proc/asset/media.hpp"


using std::string;
using std::cout;


namespace lumiera
  {
  using std::tr1::shared_ptr;
  using std::tr1::weak_ptr;
  
  template<class TAR, class BASE=std::tr1::shared_ptr<TAR> >
  class P
    : public BASE
    {
    public:
      P() : BASE() {}
      template<class Y> explicit P (Y* p) : BASE(p) {}
      template<class Y, class D> P (Y* p, D d) : BASE(p,d) {}

      P (P const& r) : BASE(r) {}
      template<class Y> P (shared_ptr<Y> const& r) : BASE(r) {}
      template<class Y> explicit P (weak_ptr<Y> const& wr) : BASE(wr) {}
      template<class Y> explicit P (std::auto_ptr<Y> & ar) : BASE(ar) {}      

    private:
      // friend operators injected into enclosing namespace and found by ADL:
      template<typename _O_>
      friend inline bool
      operator== (P const& p, P<_O_> const& q) { return (p && q)? (*p == *q) : (!p && !q); }
  
      template<typename _O_>
      friend inline bool
      operator!= (P const& p, P<_O_> const& q) { return (p && q)? (*p != *q) : !(!p && !q); }
  
      template<typename _O_>
      friend inline bool
      operator<  (P const& p, P<_O_> const& q) { return (p && q) && (*p < *q); }
  
      template<typename _O_>
      friend inline bool
      operator>  (P const& p, P<_O_> const& q) { return (p && q) && (*q < *p); }

      template<typename _O_>
      friend inline bool
      operator<= (P const& p, P<_O_> const& q) { return (p && q)? (*p <= *q) : (!p && !q); }

      template<typename _O_>
      friend inline bool
      operator>= (P const& p, P<_O_> const& q) { return (p && q)? (*p >= *q) : (!p && !q); }
      
    };
}
  
namespace asset
  {
  using lumiera::P;
  
  
  void 
  doIt()
  {
    Asset::Ident key2("Try-1", Category(VIDEO));
    P<Asset> a1 = asset::Media::create(key2, "Name-1");
    
    Asset::Ident key3("Try-2", Category(VIDEO));
    P<Media> a2 = asset::Media::create(key3, "Name-2");
    
    cout << "a1 = " << str(a1);
    
    bool yes = (a1 >= a2);
    enable_if<is_pAsset<PAsset>, string>::type yup;
    
    cout << "\n yes=" << yes << " yup=" << typeid(*a1).name();
  }
}


int main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    asset::doIt();
    
    cout <<  "\ngulp\n";
    
    
    return 0;
  }
