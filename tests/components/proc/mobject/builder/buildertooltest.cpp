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

#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include <iostream>
using std::string;
using std::cout;


namespace util {
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
  

  
  template<typename X>
  struct EmptyVal
    {
      static X create()    { return X(); }
    };
  template<typename X>
  struct EmptyVal<X*&>
    {
      static X*& create()  { static X* nullP(0); return nullP; }
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
  

}

namespace lumiera {

  namespace typelist {
  
    
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
  
  } // namespace typelist

  
  

  
  namespace variant {

    using lumiera::typelist::count;
    using lumiera::typelist::maxSize;
    using lumiera::typelist::InstantiateWithIndex;
  
    /** 
     * internal helper used to build a variant storage wrapper.
     * Parametrized with a collection of types, it provides functionality
     * to copy a value of one of these types into an internal buffer, while
     * remembering which of these types was used to place this copy.
     * The value can be later on extracted using a visitation like mechanism,
     * which takes a functor object and invokes a function \c access(T&) with
     * the type matching the current value in storage
     */ 
    template<typename TYPES>
    struct Holder
      {
        
        enum { TYPECNT = count<TYPES>::value
             , SIZE  = maxSize<TYPES>::value
             };
        
  
        /** Storage to hold the actual value */
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
        struct PlacementAdapter : BASE
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
        
        typedef InstantiateWithIndex< TYPES
                                    , PlacementAdapter
                                    , Buffer
                                    > 
                                    Storage;
        
        
        
        /** provide a dispatcher table based visitation mechanism */
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
                BASE::table_[i] = &BASE::template trampoline<T>;
              }
          };
        
          
        template<class FUNCTOR>
        static typename FUNCTOR::Ret
        access (Buffer& buf)
        {
          typedef InstantiateWithIndex< TYPES
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
      };
    
    
      template<typename TYPES>
      void
      Holder<TYPES>::Buffer::deleteCurrent ()
      {
        access<Deleter>(*this); // remove old content, if any
        which = TYPECNT;       //  mark as empty
      }
  
  } // namespace variant
  

  
 
      
      
  
  
  
  
  
  
  /** 
   * A variant wrapper (typesafe union) capable of holding a value of any
   * of a bounded collection of types. The value is stored in a local buffer
   * directly within the object and may be accessed by a typesafe visitation.
   * 
   * \par
   * This utility class is similar to boost::variant and indeed was implemented
   * (5/08) in an effort to replace the latter in a draft solution for the problem
   * of typesafe access to the correct wrapper class from within some builder tool.
   * Well -- after finisihng this "exercise" I must admit that it is not really
   * much more simple than what boost::variant does internally. At least we are
   * pulling in fewer headers and the actual code path is shorter compared with
   * boost::variant, at the price of beeing not so generic, not caring for
   * alignment issues within the buffer and being <b>not threadsafe</b>
   * 
   * @param TYPES   collection of possible types to be stored in this variant object
   * @param Access  policy how to access the stored value
   */
  template< typename TYPES
          , template<typename> class Access = util::AccessCasted  
          >                      
  class Variant 
    : boost::noncopyable
    {
      
      typedef variant::Holder<TYPES> Holder;
      typedef typename Holder::Deleter Deleter;
      
      
      /** storage: buffer holding either and "empty" marker,
       *  or one of the configured pointer to wrapper types */ 
      typename Holder::Storage holder_;
      
      
    public:
      void reset () { holder_.deleteCurrent();}
      
      /** store a copy of the given argument within the
       *  variant holder buffer, thereby typically casting 
       *  or converting the given source type to the best 
       *  suited (base) type (out of the collection of possible
       *  types for this Variant instance)
       */ 
      template<typename SRC>
      Variant&
      operator= (SRC src)
        {
          if (src) holder_.put (src);  // see Holder::PlacementAdaptor::put
          else     reset();
          return *this;
        }
      
      /** retrieve current content of the variant,
       *  trying to cast or convert it to the given type.
       *  Actually, the function \c access(T&) on the 
       *  Access-policy (template param) is invoked with the
       *  type currently stored in the holder buffer.
       *  May return NULL if conversion fails.
       */
      template<typename TAR>
      TAR
      get ()   
        {
          typedef Access<TAR> Extractor;
          return Holder::template access<Extractor> (this->holder_);
        }
    };
  
} // namespace lumiera 



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
      using lumiera::typelist::Types;


    
    typedef Types < Placement<MObject>*
                  , P<asset::Asset>*
                  > ::List
                    WrapperTypes;
                  
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
    typedef lumiera::Variant<WrapperTypes> WrapperPtr;
    
    
    
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
          Placement<TAR>* pPlacement = currentWrapper_.get<Placement<TAR>*>(); 
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
          P<TAR>* pP = currentWrapper_.get<P<TAR>*>(); 
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
