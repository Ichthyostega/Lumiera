/*
  AccessCasted(Test)  -  verify helper to cast or convert as appropriate

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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



#include "lib/test/run.hpp"

#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/enable_if.hpp>


using std::string;
using std::cout;
using std::ostream;

using boost::remove_pointer;
using boost::remove_reference;
using boost::is_convertible;
using boost::is_polymorphic;
using boost::is_base_of;
using boost::enable_if;


namespace lib  {
namespace meta {
namespace test {
  
  
    
    template <typename SRC, typename TAR>
    struct can_cast : boost::false_type {};

    template <typename SRC, typename TAR>
    struct can_cast<SRC*,TAR*>          { enum { value = is_base_of<SRC,TAR>::value };};

    template <typename SRC, typename TAR>
    struct can_cast<SRC*&,TAR*>         { enum { value = is_base_of<SRC,TAR>::value };};

    template <typename SRC, typename TAR>
    struct can_cast<SRC&,TAR&>          { enum { value = is_base_of<SRC,TAR>::value };};
    
    
    template <typename T>
    struct has_RTTI
      {
        typedef typename remove_pointer<
                typename remove_reference<T>::type>::type TPlain;
      
        enum { value = is_polymorphic<TPlain>::value };
      };
    
    template <typename SRC, typename TAR>
    struct use_dynamic_downcast
      {
        enum { value = can_cast<SRC,TAR>::value
                       &&  has_RTTI<SRC>::value
                       &&  has_RTTI<TAR>::value
             };
      };
    
    template <typename SRC, typename TAR>
    struct use_static_downcast
      {
        enum { value = can_cast<SRC,TAR>::value
                    && (  !has_RTTI<SRC>::value
                       || !has_RTTI<TAR>::value
                       )
             };
      };
    
    template <typename SRC, typename TAR>
    struct use_conversion
      {
        enum { value = is_convertible<SRC,TAR>::value
                    && !( use_static_downcast<SRC,TAR>::value
                        ||use_dynamic_downcast<SRC,TAR>::value
                        )
             };
      };
    
    
    template<typename X>
    struct EmptyVal
      {
        static X create() 
          {
            cout << " NULL() " << __PRETTY_FUNCTION__ <<"\n";
            return X(); 
          }
      };
    template<typename X>
    struct EmptyVal<X*&>
      {
        static X*& create() 
          {
            cout << " NULL & " << __PRETTY_FUNCTION__ <<"\n";
            static X* null(0);
            return null; 
          }
      };
    
    
    template<typename RET>
    struct NullAccessor
      {
        typedef RET Ret;
        
        static RET access  (...) { return ifEmpty(); }
        static RET ifEmpty ()    { return EmptyVal<RET>::create(); }
      };
    
    template<typename TAR>
    struct AccessCasted : NullAccessor<TAR>
      {
        using NullAccessor<TAR>::access;
        
        template<typename ELM>
        static  typename enable_if< use_dynamic_downcast<ELM&,TAR>, TAR>::type
        access (ELM& elem) 
          { 
            cout << " dynamic " << __PRETTY_FUNCTION__ <<"\n";
            return dynamic_cast<TAR> (elem); 
          }
        
        template<typename ELM>
        static  typename enable_if< use_static_downcast<ELM&,TAR>, TAR>::type
        access (ELM& elem) 
          { 
            cout << " static " << __PRETTY_FUNCTION__ <<"\n";
            return static_cast<TAR> (elem); 
          }
        
        template<typename ELM>
        static  typename enable_if< use_conversion<ELM&,TAR>, TAR>::type
        access (ELM& elem) 
          { 
            cout << " convert " << __PRETTY_FUNCTION__ <<"\n";
            return elem; 
          }
            
      };
  
  
  
  namespace { // Test fixture...
    
    struct B {};
    struct D : B {};
    
    struct E : D
      {
        virtual ~E() {};
      };
    struct F : E {};
    
    
    ostream& operator<< (ostream& s, const B& b) { return s << "B{} adr="<<&b; }
    ostream& operator<< (ostream& s, const D& d) { return s << "D{} adr="<<&d; }
    ostream& operator<< (ostream& s, const E& e) { return s << "E{} adr="<<&e; }
    ostream& operator<< (ostream& s, const F& f) { return s << "F{} adr="<<&f; }
    
  }//(End)Test fixture
  
  
  
  
  
  
  /*************************************************************************************************//**
   * @test verify a helper template for accessing values either through conversion or (dynamic) downcast.
   *       Typically, this helper is used in value holder containers or variant-like data structures,
   *       where the actual type is given at instantiation time of the template and possibly erased.
   */
  class AccessCasted_test : public Test
    {
      virtual void
      run (Arg)
        {
          
          D d;
          D* pD =&d;
          B* pB =pD;
          D& rD = *pD;
          B& rB = *pB;
          
          D*& rpD = pD;
          B*& rpB = pB;
          
          F f;
          E& rE = f;
          E* pE = &f;
          D* pDE = pE;
          
          cout <<  "is_base_of<B,D> = "   << is_base_of<B ,D >::value << "\n";
          cout <<  "is_base_of<B*,D*> = " << is_base_of<B*,D*>::value << "\n";
          cout <<  "is_base_of<B&,D&> = " << is_base_of<B&,D&>::value << "\n";
          
          cout <<  "can_cast<B,D> = " << can_cast<B,D>::value << "\n";
          cout <<  "can_cast<B*,D*> = " << can_cast<B*,D*>::value << "\n";
          cout <<  "can_cast<B&,D&> = " << can_cast<B&,D&>::value << "\n";
          cout <<  "can_cast<B&,D*> = " << can_cast<B&,D*>::value << "\n";
          cout <<  "can_cast<B*,D&> = " << can_cast<B*,D&>::value << "\n";
          cout <<  "can_cast<B*&,D*&> = " << can_cast<B*&,D*&>::value << "\n";
          cout <<  "can_cast<D*&,D*&> = " << can_cast<D*&,D*&>::value << "\n";
          
          cout <<  "can_cast<D*,E*> = " << can_cast<D*,E*>::value << "\n";
          cout <<  "can_cast<E*,F*> = " << can_cast<E*,F*>::value << "\n";
          
          cout <<  "has_RTTI<D*> = " << has_RTTI<D*>::value << "\n";
          cout <<  "has_RTTI<E*> = " << has_RTTI<E*>::value << "\n";
          cout <<  "has_RTTI<F*> = " << has_RTTI<F*>::value << "\n";
          
          
          cout <<  "use_dynamic_downcast<B*,D*> = " << use_dynamic_downcast<B*,D*>::value << "\n";
          cout <<  "use_static_downcast<B*,D*> = " << use_static_downcast<B*,D*>::value << "\n";
          cout <<  "use_conversion<D*,B*> = "      << use_conversion<D*,B*>::value << "\n";
          
          cout <<  "use_static_downcast<D*&,D*&> = " << use_static_downcast<D*&,D*&>::value << "\n";
          cout <<  "use_static_downcast<D*,E*> = " << use_static_downcast<D*,E*>::value << "\n";
          cout <<  "use_dynamic_downcast<D*&,E*> = " << use_dynamic_downcast<D*&,E*>::value << "\n";
          
          
          cout <<  "Access(D  as D&) --->" << AccessCasted<D&>::access(d)  << "\n";
          cout <<  "Access(D& as D&) --->" << AccessCasted<D&>::access(rD) << "\n";
          cout <<  "Access(B& as D&) --->" << AccessCasted<D&>::access(rB) << "\n";
          cout <<  "Access(D* as D*) --->" << AccessCasted<D*>::access(pD) << "\n";
          cout <<  "Access(B* as D*) --->" << AccessCasted<D*>::access(pB) << "\n";
          cout <<  "Access(D*& as D*&) --->" << AccessCasted<D*&>::access(rpD) << "\n";
          cout <<  "Access(B*& as D*&) --->" << AccessCasted<D*&>::access(rpB) << "\n";
          
          cout <<  "Access(D  as B&) --->" << AccessCasted<B&>::access(d)  << "\n";
          cout <<  "Access(D& as B&) --->" << AccessCasted<B&>::access(rD) << "\n";
          cout <<  "Access(B& as B&) --->" << AccessCasted<D&>::access(rB) << "\n";
          cout <<  "Access(D* as B*) --->" << AccessCasted<B*>::access(pD) << "\n";
          cout <<  "Access(B* as B*) --->" << AccessCasted<B*>::access(pB) << "\n";
          cout <<  "Access(D*& as B*&) --->" << AccessCasted<B*&>::access(rpD) << "\n";
          cout <<  "Access(B*& as B*&) --->" << AccessCasted<B*&>::access(rpB) << "\n";
          
          cout <<  "Access(D  as E&) --->" << AccessCasted<E&>::access(d) << "\n";
          cout <<  "Access(E& as F&) --->" << AccessCasted<F&>::access(rE) << "\n";
          cout <<  "Access(D(E)* as E*) --->" << AccessCasted<E*>::access(pDE) << "\n";
          cout <<  "Access(D(E)* as F*) --->" << AccessCasted<F*>::access(pDE) << "\n";
          cout <<  "Access(E* as F*) --->" << AccessCasted<F*>::access(pE) << "\n";
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (AccessCasted_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
