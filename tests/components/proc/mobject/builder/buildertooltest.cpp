/*
  BuilderTool(Test)  -  specialized visitor used within the builder for processing Placements
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "common/test/run.hpp"
#include "proc/mobject/builder/buildertool.hpp"
#include "proc/asset/category.hpp"
#include "proc/asset/media.hpp"
#include "proc/mobject/session/clip.hpp"

#include "proc/asset.hpp"
#include "common/p.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/explicitplacement.hpp"   //////////TODO

#include "common/typelistutil.hpp"

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
#include <boost/type_traits/is_base_of.hpp>

//#include <boost/variant.hpp>
#include <iostream>
using std::string;
using std::cout;


namespace mobject
  {
  
  DEFINE_SPECIALIZED_PLACEMENT (session::AbstractMO);
  
  
  namespace builder
    {
    namespace test
      {
      
      using session::Clip;
      using session::AbstractMO;

/////////////////////////////////////////////////////TODO: move to buildertool.hpp

      using lumiera::P;

// Problem
/*
      - brauche Laufzeittyp des Zielobjektes
      - an wen wird die Dispatcher-Tabelle gebunden?
        - falls an den Wrapper: Gefahr, zur Laufzeit nicht die Tabelle zu bekommen, in die das Trampolin registriert wurde
        - falls an das Zielobjekt: wie gebe ich Referenz und konkreten Typ des Wrappers an den Funktionsaufruf im Tool?
      - vieleicht einen allgemeinen Argument-Adapter nutzen?
      - Zielobjekt oder Wrapper<Zielobjekt> als Argumenttyp? 
*/

    using boost::enable_if;
    using boost::is_convertible;
    
    using lumiera::typelist::Types;
    using lumiera::typelist::Node;
    using lumiera::typelist::NullType;
//    using lumiera::typelist::count;
//    using lumiera::typelist::maxSize;
    using lumiera::typelist::InstantiateChained;
    
    template
      < class TYPES                           // List of Types
      , template<class,class,uint> class _X_ //  your-template-goes-here
      , class BASE = NullType               //   Base class at end of chain
      , uint i = 0                         //    incremented on each instantiaton
      >
    class InstantiateWithIndex;
    
    
    template< template<class,class,uint> class _X_
            , class BASE
            , uint i
            >
    class InstantiateWithIndex<NullType, _X_, BASE, i>
      : public BASE
      { 
      public:
        typedef BASE     Unit;
        typedef NullType Next;
        enum{ COUNT = i };
      };
    
      
    template
      < class TY, typename TYPES
      , template<class,class,uint> class _X_
      , class BASE
      , uint i
      >
    class InstantiateWithIndex<Node<TY, TYPES>, _X_, BASE, i> 
      : public _X_< TY
                  , InstantiateWithIndex<TYPES, _X_, BASE, i+1 >
                  , i
                  >
      { 
      public:
        typedef InstantiateWithIndex<TYPES,_X_,BASE,i+1> Next;
        typedef _X_<TY,Next,i> Unit;
        enum{ COUNT = Next::COUNT };
      };

    template<class TYPES>
    struct count;
    template<>
    struct count<NullType>
      {
        enum{ value = 0 };
      };
    template<class TY, class TYPES>
    struct count<Node<TY,TYPES> >
      {
        enum{ value = 1 + count<TYPES>::value };
      };
                  
    template<class TYPES>
    struct maxSize;
    template<>
    struct maxSize<NullType>
      {
        enum{ value = 0 };
      };
    template<class TY, class TYPES>
    struct maxSize<Node<TY,TYPES> >
      {
        enum{ nextval = maxSize<TYPES>::value
            , thisval = sizeof(TY)
            , value   = nextval > thisval?  nextval:thisval
            };
      };

    
    
    template<class T>
    struct Holder
      {
        T content;
        
        Holder (T const& src) : T(src) {}
        
        template<typename TAR>
        TAR get () { return static_cast<TAR> (content); }
      };

    typedef Types < Placement<MObject>*
                  , P<asset::Asset>*
                  > ::List
                    WrapperTypes;
    
    template<typename X>
    struct EmptyVal
      {
        static X create() 
          {
            return X(); 
          }
      };
    template<typename X>
    struct EmptyVal<X*&>
      {
        static X*& create() 
          {
            static X* null(0);
            return null; 
          }
      };
                  
    
    const uint TYPECNT = count<WrapperTypes>::value;
    const size_t SIZE  = maxSize<WrapperTypes>::value;
    
    struct Buffer
      {
        char buffer_[SIZE];
        uint which;
        
        Buffer() : which(TYPECNT) {}
        
        void* 
        put (void)
          {
            deleteCurrent();
            return 0;
          }
        
        void
        deleteCurrent ();  // depends on the Deleter, see below
      };
    
    template<typename T, class BASE, uint idx>
    struct Storage : BASE
      {
        T& 
        put (T const& toStore)
          {
            BASE::deleteCurrent();  // remove old content, if any
            
            T& storedObj = *new(BASE::buffer_) T (toStore);
            this->which = idx;   //    remember the actual type selected
            return storedObj;
          }
        
        using BASE::put;
      };
    
    
    
    template<class FUNCTOR>
    struct CaseSelect
      {
        typedef typename FUNCTOR::Ret Ret;
        typedef Ret (*Func)(Buffer&);
        
        Func table_[TYPECNT];
        
        CaseSelect ()
          {
            for (uint i=0; i<TYPECNT; ++i)
              table_[i] = 0;
          }
        
        template<typename T>
        static Ret
        trampoline (Buffer& storage)
          {
            T& content = reinterpret_cast<T&> (storage.buffer_);
            return FUNCTOR::access (content);
          }
        
        template<typename T>
        void
        create_thunk (uint idx)
          {
            Func  thunk = &trampoline<T>;
            table_[idx] = thunk;
          }
        
        Ret
        invoke (Buffer& storage)
          {
            if (TYPECNT <= storage.which)
              return FUNCTOR::ifEmpty ();
            else
              return (*table_[storage.which]) (storage);
          }
      };
    
    
    template< class T, class BASE, uint i >
    struct CasePrepare
      : BASE
      {
        CasePrepare () : BASE()
          {
//            BASE::table_[i] = &(BASE::template trampoline<T>);
            BASE::template create_thunk<T>(i);
          }
      };
    
      
    template<class FUNCTOR>
    typename FUNCTOR::Ret
    access (Buffer& buf)
    {
      typedef InstantiateWithIndex< WrapperTypes
                                  , CasePrepare
                                  , CaseSelect<FUNCTOR>
                                  > 
                                  Accessor;
      static Accessor select_case;
      return select_case.invoke(buf);
    }
    
    
    struct Deleter
      {
        typedef void Ret;
        
        template<typename T>
        static void access (T& elem) { elem.~T(); }
        
        static void ifEmpty () { }
      };

    
    void
    Buffer::deleteCurrent ()
    {
      access<Deleter>(*this); // remove old content, if any
      which = TYPECNT;       //  mark as empty
    }
    
        
    

    
   
    using boost::remove_pointer;
    using boost::remove_reference;
    using boost::is_convertible;
    using boost::is_polymorphic;
    using boost::is_base_of;
    using boost::enable_if;
    
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
            return dynamic_cast<TAR> (elem); 
          }
        
        template<typename ELM>
        static  typename enable_if< use_static_downcast<ELM&,TAR>, TAR>::type
        access (ELM& elem) 
          { 
            return static_cast<TAR> (elem); 
          }
        
        template<typename ELM>
        static  typename enable_if< use_conversion<ELM&,TAR>, TAR>::type
        access (ELM& elem) 
          { 
            return elem; 
          }
            
      };
        
        
    
    
    
    
    
    
    /** 
     * helper to treat various sorts of smart-ptrs uniformly.
     * Implemented as a variant-type value object, it is preconfigured
     * with the possible hierarchy-base classes used within this application.
     * Thus, when passing in an arbitrary smart-ptr, the best fitting smart-ptr
     * type pointing to the corresponding base class is selected for internal storage.
     * Later on, stored values can be retrieved either utilitzing static or dynamic casts;
     * error reporting is similar to the bahaviour of dynamic_cast<T>: when retrieving
     * a pointer, in case of mismatch NULL is returned.
     */
    class WrapperPtr         // NONCOPYABLE!!
      {
        

      
      private:
        typedef InstantiateWithIndex< WrapperTypes
                                    , Storage
                                    , Buffer
                                    > 
                                    VariantHolder;
        
        /** storage: buffer holding either and "empty" marker,
         *  or one of the configured pointer to wrapper types */ 
        VariantHolder holder_;
        
      public:
        void 
        reset () 
          { 
            access<Deleter> (holder_);
            holder_.which = TYPECNT;
          }
        
        template<typename WRA>
        WrapperPtr&
        operator= (WRA* src)      ///< store a ptr to the given wrapper, after casting to base
          {
            if (src) holder_.put (src);
            else     reset();
            return *this;
          }
        
        template<typename WRA>
        WRA*
        get ()                    ///< retrieve ptr and try to downcast to type WRA   
          {
            typedef AccessCasted<WRA*> AccessWraP;
            WRA* res = access<AccessWraP>(this->holder_);
            return res;
          }
      };
    
    
    class BuTuul
      : public lumiera::visitor::Tool<void, InvokeCatchAllFunction> 
      {
        WrapperPtr currentWrapper_;
        
      public:
        
      template<template<class> class WRA, class TAR>
      void rememberWrapper (WRA<TAR>* argument)
        {
          currentWrapper_ = argument;
        }
      
      void forgetWrapper ()
        {
          currentWrapper_.reset();
        }
      
      
      template<class TAR>
      Placement<TAR>&
      getPlacement ()
        {
          Placement<TAR>* pPlacement = currentWrapper_.get<Placement<TAR> >(); 
          return *pPlacement;
        }
      ExplicitPlacement
      getExplicitPlacement ()
        {
          return getPlacement<MObject>().resolve();
        }
      template<class TAR>
      P<TAR>
      getPtr ()
        {
          P<TAR>* pP = currentWrapper_.get<P<TAR> >(); 
          return *pP;
        }
      };
      
        
        

      
      
    
    
    template
      < class TOOLImpl,  // concrete BuilderTool implementation
        class TYPELIST  //  list of all concrete Buildables to be treated
      >
    class Appli
      : public lumiera::visitor::Applicable<TOOLImpl, TYPELIST, BuTuul>
      { }
      ;
      
    using lumiera::typelist::Types;   // convienience for the users of "Applicable"

    class BDable : public lumiera::visitor::Visitable<BuTuul>
      {
        
      };
/////////////////////////////////////////////////////TODO: move to buildertool.hpp
      
      class DummyMO : public AbstractMO, public BDable
        {
        public:
          DummyMO() { };
          virtual bool isValid()  const { return true;}
//          DEFINE_PROCESSABLE_BY (BuilderTool);
          
          static void killDummy (MObject* dum) { delete (DummyMO*)dum; }
          
          virtual void
          apply (BuTuul& tool) 
            { 
              return BDable::dispatchOp (*this, tool); 
            }
          
        };
      
      class Clop : public Clip, public BDable
        {
          Clop (Clip& base) : Clip (base.clipDef_, base.mediaDef_) {}
          
          virtual void
          apply (BuTuul& tool) 
            { 
              return BDable::dispatchOp (*this, tool); 
            }
        };
      
      
      template<typename WRA>
      inline BDable::ReturnType
      apply (BuTuul& tool, WRA& wrappedTargetObj)
      {
        WRA* ptr = &wrappedTargetObj;
        (*ptr)->isValid();
        tool.rememberWrapper(ptr);
        wrappedTargetObj->apply (tool);
        tool.forgetWrapper();
      }
      
  //////////////////////////////////////////////////////TODO: wip-wip
      
      class DummyPlacement : public Placement<AbstractMO>
        {
        public:
          DummyPlacement() 
            : Placement<AbstractMO>::Placement(*new DummyMO, &DummyMO::killDummy)
            { }
        };
  //////////////////////////////////////////////////////TODO: wip-wip
      
      

      class TestTool 
        : public Appli<TestTool, Types<Clip, AbstractMO>::List>
        {
        public:
          void treat (Clip& c)    
            { 
              Placement<Clip>& pC = getPlacement<Clip>();
              cout << "media is: "<< str(pC->getMedia()) <<"\n" 
                   << "Placement(adr) " << &pC <<"\n"; 
            }
          void treat (AbstractMO&)
            {
              Placement<AbstractMO>& placement = getPlacement<AbstractMO>();
              cout << "unspecific MO; Placement(adr) " << &placement <<"\n"; 
            }
          void onUnknown (Buildable&)
            { 
              cout << "catch-all-function called.\n"; 
            }
        };


      
      
      /*******************************************************************
       * @test the generic visitor pattern specialized for treating
       *       MObjects in the builder. Because the generic visitor 
       *       implementation is already covered by
       *       \link VisitingTool_test, it is sufficient to test
       *       the specialisation to the builder.
       * \par
       * Besides using existing MObject types (at the moment session::Clip),
       * we create inline a yet-unknown new MObject subclass. When passing
       * such to any BuilderTool subclass, the compiler enforces the definition
       * of a catch-all function, which is called, when there is no other
       * applicable \c treat(MO&) function. Note further, whithin the specific
       * treat-functions we get direct references, whithout interfering with
       * Placements and memory management. (Is this a good idea? it allows
       * client code to bypass the Placement (Handle). At least, I think
       * it is not really a problem...) 
       */
      class BuilderTool_test : public Test
        {
          virtual void run(Arg arg) 
            {
                          
              TestTool t1;
              BuTuul& tool (t1);
                                
              DummyPlacement dumm;
              Placement<AbstractMO>& dummy(dumm);
              
              Placement<Clip> clip = asset::Media::create("test-1", asset::VIDEO)->createClip();
              

              cout << "Placement(adr) " << &clip <<"\n"; 
              apply (tool, clip);
              cout << "Placement(adr) " << &dumm <<"\n"; 
              apply (tool, dummy);
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (BuilderTool_test, "unit builder");
      
      
      
    } // namespace test
    
  } // namespace builder
  
  //////////////////////////////////////////////////////TODO: wip-wip
/*  template<>
  ExplicitPlacement
  Placement<session::AbstractMO>::resolve ()  const 
    { 
      UNIMPLEMENTED ("just a test");
    }
*/  //////////////////////////////////////////////////////TODO: wip-wip


} // namespace mobject
