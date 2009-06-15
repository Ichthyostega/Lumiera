/*
  CommandBasic(Test)  -  checking simple ProcDispatcher command definition and execution
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
#include "lib/p.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/command-def.hpp"
#include "lib/lumitime.hpp"
//#include "lib/util.hpp"

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelistutil.hpp"
#include "lib/meta/generator.hpp"

#include <tr1/functional>
//#include <boost/format.hpp>
#include <iostream>
#include <string>

using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
using std::tr1::function;
//using boost::format;
using lumiera::Time;
//using util::contains;
using std::string;
using std::cout;
using std::endl;


namespace lumiera {
namespace typelist{


  ////////////////////////////////////////////TODO braindump
  
  template< typename SIG>
  struct FunctionSignature;
  
  template< typename RET>
  struct FunctionSignature< function<RET(void)> >
  {
    typedef RET Ret;
    typedef Types<> Args;
  };
  
  template< typename RET
          , typename A1
          >
  struct FunctionSignature< function<RET(A1)> >
  {
    typedef RET Ret;
    typedef Types<A1> Args;
  };
  
  template< typename RET
          , typename A1
          , typename A2
          >
  struct FunctionSignature< function<RET(A1,A2)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2> Args;
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          >
  struct FunctionSignature< function<RET(A1,A2,A3)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3> Args;
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          >
  struct FunctionSignature< function<RET(A1,A2,A3,A4)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3,A4> Args;
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          >
  struct FunctionSignature< function<RET(A1,A2,A3,A4,A5)> >
  {
    typedef RET Ret;
    typedef Types<A1,A2,A3,A4,A5> Args;
  };
  
  
  template<typename RET, typename LI>
  struct FunctionTypedef;
  
  template< typename RET>
  struct FunctionTypedef<RET, Types<> >
  {
    typedef function<RET(void)> Func;
    typedef          RET Sig();
  };
  
  template< typename RET
          , typename A1
          >
  struct FunctionTypedef<RET, Types<A1> >
  {
    typedef function<RET(A1)> Func;
    typedef          RET Sig(A1);
  };
  
  template< typename RET
          , typename A1
          , typename A2
          >
  struct FunctionTypedef<RET, Types<A1,A2> >
  {
    typedef function<RET(A1,A2)> Func;
    typedef          RET Sig(A1,A2);
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3> >
  {
    typedef function<RET(A1,A2,A3)> Func;
    typedef          RET Sig(A1,A2,A3);
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4> >
  {
    typedef function<RET(A1,A2,A3,A4)> Func;
    typedef          RET Sig(A1,A2,A3,A4);
  };
  
  template< typename RET
          , typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          >
  struct FunctionTypedef<RET, Types<A1,A2,A3,A4,A5> >
  {
    typedef function<RET(A1,A2,A3,A4,A5)> Func;
    typedef          RET Sig(A1,A2,A3,A4,A5);
  };
  
  
  
  /////////////////////////very basic facility: Typed tuples
  
  template<class T, class TYPES>
  struct Prepend;
  
  template< typename A1
          , typename A2
          , typename A3
          , typename A4
          , typename A5
          , typename IGN
          >
  struct Prepend<A1, Types<A2,A3,A4,A5,IGN> >
  {
    typedef Types<A1,A2,A3,A4,A5> Tuple;
  };

  template<class TYPES>
  struct Tuple;

  template<>
  struct Tuple<NullType>
    {
      typedef NullType HeadType;
      typedef Types<>  TailType;
      typedef Types<>  Type;
      
      typedef NullType ArgList_;
      typedef Tuple<Type> ThisTuple;
      typedef Tuple<NullType> Tail;
      enum { SIZE = 0 };
      
      NullType getHead() { return NullType(); }
      Tail&    getTail() { return *this;      }
      
      Tuple (HeadType const&, Tail const&) { }
      Tuple ()                             { }
    };
  
  template<class TY, class TYPES>
  struct Tuple<Node<TY,TYPES> >
    : Tuple<TYPES>
    {
      typedef TY                                   HeadType;
      typedef typename Tuple<TYPES>::Type          TailType;
      typedef typename Prepend<TY,TailType>::Tuple Type;
      
      typedef Node<TY,TYPES> ArgList_;
      typedef Tuple<Type> ThisTuple;
      typedef Tuple<TYPES> Tail;
      enum { SIZE = count<ArgList_>::value };
      
      Tuple ( TY a1 =TY()
            , Tail tail =Tail()
            )
        : Tuple<TYPES> (tail.getHead(), tail.getTail()),
          val_(a1)
        { }
      
      TY  & getHead() { return val_; }
      Tail& getTail() { return static_cast<Tail&> (*this); }
      
    private:
      TY val_;
    };
  
  ////TODO move in sub-scope
  template<class TUP,uint i>
  struct Shifted
    {
      typedef typename TUP::Tail Tail;
      typedef typename Shifted<Tail,i-1>::TupleType TupleType;
    };
  template<class TUP>
  struct Shifted<TUP,0>
    { 
      typedef Tuple<typename TUP::ArgList_> TupleType;
    };
      
  
  template< typename T1
          , typename T2
          , typename T3
          , typename T4
          , typename T5
          >
  struct Tuple<Types<T1,T2,T3,T4,T5> >
    : Tuple<typename Types<T1,T2,T3,T4,T5>::List>
    {
      typedef T1                          HeadType;
      typedef Types<T2,T3,T4,T5,NullType> TailType;
      typedef Types<T1,T2,T3,T4,T5>       Type;
      
      typedef typename Type::List ArgList_;
      typedef Tuple<Type> ThisTuple;
      typedef Tuple<TailType> Tail;
      enum { SIZE = count<ArgList_>::value };
      
      Tuple ( T1 a1 =T1()
            , T2 a2 =T2()
            , T3 a3 =T3()
            , T4 a4 =T4()
            , T5 a5 =T5()
            )
        : Tuple<ArgList_>(a1, Tuple<TailType>(a2,a3,a4,a5))
        { }
      
      using Tuple<ArgList_>::getHead;
      using Tuple<ArgList_>::getTail;
      
      template<uint i>
      typename Shifted<ThisTuple,i>::TupleType&
      getShifted ()
        {
          typedef typename Shifted<ThisTuple,i>::TupleType Tail_I;
          return static_cast<Tail_I&> (*this);
        }
      
      template<uint i>
      typename Shifted<ThisTuple,i>::TupleType::HeadType&
      getAt ()
        {
          return getShifted<i>().getHead();
        }
      
      NullType&
      getNull()
        {
          static NullType nix;
          return nix; 
        }
    };
  
  
  /**
   * Decorating a tuple type with auxiliary data access operations.
   * This helper template builds up a subclass of the given BASE type
   * (which is assumed to be a Tuple or at least need to be copy constructible
   * from \c Tuple<TYPES> ). The purpose is to use the Tuple as storage, but
   * to add a layer of access functions, which in turn might rely on the exact
   * type of the individual elements within the Tuple. To achieve this, for each
   * type within the Tuple, the BASE type is decorated with an instance of the
   * template passed in as template template parameter _X_. Each of these
   * decorating instances is provided with a member pointer to access "his"
   * specific element within the underlying tuple.
   * 
   * The decorating template _X_ need to take its own base class as template
   * parameter. Typically, operations on _X_ will be defined in a recursive fashion,
   * calling down into this templated base class. To support this, an instantiation
   * of _X_ with the 0 member ptr is generated for detecting recursion end
   * (built as innermost decorator, i.e. immediate subclass of BASE) 
   */
  template
    < typename TYPES
    , template<class,class,uint> class _X_
    , class BASE =Tuple<TYPES>
    , uint i = 0
    >
  class BuildTupleAccessor
    {
      typedef Tuple<TYPES> ArgTuple;
      typedef typename ArgTuple::HeadType Head;
      typedef typename ArgTuple::TailType Tail;
//    typedef Head ArgTuple::*getElm();
      typedef BuildTupleAccessor<Tail,_X_,BASE, i+1> NextBuilder;
      typedef typename NextBuilder::Accessor NextAccessor;
      
      ArgTuple& argData_;
      
    public:
      
      /** type of the product created by this template.
       *  Will be a subclass of BASE */
      typedef _X_<Head, NextAccessor, i> Accessor;
      
      BuildTupleAccessor (ArgTuple& tup)
        : argData_(tup)
        { }
      
      operator Accessor() { return Accessor(argData_); }
      
    };

  template
    < class BASE
    , template<class,class,uint> class _X_
    , uint i
    >
  class BuildTupleAccessor<Types<>, _X_, BASE, i>
    {
      typedef Tuple<Types<> > ArgTuple;
//    typedef NullType BASE::*getElm();
      
    public:
      typedef _X_<NullType, BASE, 0> Accessor;
    };
  
  
  ///////////////////////// creating functional closures
  
  namespace tuple {
      template<uint n>
      struct Apply;
      
      template<>       
      struct Apply<1>
        {
          template<class FUN, typename RET, class TUP>
          static RET
          invoke (FUN f, TUP & arg)
            {
              return f (arg.template getAt<1>()); 
            }
          
          template<class FUN, typename RET, class TUP>
          static RET
          bind (FUN f, TUP & arg)
            {
              return std::tr1::bind (f, arg.template getAt<1>()); 
            }
        };
      
      template<>       
      struct Apply<2>
        {
          template<class FUN, typename RET, class TUP>
          static RET
          invoke (FUN f, TUP & arg)
            {
              return f ( arg.template getAt<1>()
                       , arg.template getAt<2>()
                       ); 
            }
          
          template<class FUN, typename RET, class TUP>
          static RET
          bind (FUN f, TUP & arg)
            {
              return std::tr1::bind (f, arg.template getAt<1>() 
                                      , arg.template getAt<2>()
                                     );
            }
        };
  } // (END) sub-namespace 
  
  template<typename SIG>
  class TupleApplicator
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      typedef typename FunctionSignature< function<SIG> >::Ret  Ret;
      
      enum { ARG_CNT = count<typename Args::List>::value };
      
      
      /** storing a ref to the parameter tuple */
      Tuple<Args>& params_;
      
    public:
      TupleApplicator (Tuple<Args>& args)
        : params_(args)
        { }
      
      function<SIG>  bind (SIG& f)                 { return tuple::Apply<ARG_CNT>::bind (f, params_); }
      function<SIG>  bind (function<SIG> const& f) { return tuple::Apply<ARG_CNT>::bind (f, params_); }
      
      Ret      operator() (SIG& f)                 { return tuple::Apply<ARG_CNT>::invoke (f, params_); }
      Ret      operator() (function<SIG> const& f) { return tuple::Apply<ARG_CNT>::invoke (f, params_); }
    };
  
  
  /**
   * Closing a function over its arguments.
   * This is a small usage example or spin-off,
   * having almost the same effect than invoking tr1::bind.
   * The notable difference is that the function arguments for
   * creating the closure are passed in as one compound tuple.
   */
  template<typename SIG>
  class FunctionClosure
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      typedef typename FunctionSignature< function<SIG> >::Ret  Ret;
      
      function<Ret(void)> closure_;
      
    public:
      FunctionClosure (SIG& f, Tuple<Args>& arg)
        : closure_(TupleApplicator<SIG>(arg).bind(f))
        { }
      FunctionClosure (function<SIG> const& f, Tuple<Args>& arg)
        : closure_(TupleApplicator<SIG>(arg).bind(f))
        { }
      
      Ret operator() () { return closure_(); }
    };
  
  
/*
  template<typename TYPES>
  struct BuildClosure
    : InstantiateWithIndex<TYPES, Accessor, I>
    {
      
    };
*/
  
  ///////////////////////// additional typelist manipulators
  
  template<class TYPES>
  struct SplitLast;
  
  template<>
  struct SplitLast<NullType>
    {
      typedef NullType Type;
      typedef NullType Prefix;
    };
  
  template<class TY>
  struct SplitLast<Node<TY,NullType> >
    {
      typedef TY Type;
      typedef NullType Prefix;
    };
  
  template<class TY, class TYPES>
  struct SplitLast<Node<TY,TYPES> >
    {
      typedef typename SplitLast<TYPES>::Type Type;
      typedef typename Append<TY, typename SplitLast<TYPES>::Prefix>::List Prefix;
    };

  
  
}} // namespace lumiera::typelist
  
namespace control {
namespace test    {

  using lib::test::showSizeof;

//  using session::test::TestClip;
  using lumiera::P;
  
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::FunctionTypedef;
  
  using lumiera::typelist::Types;
  using lumiera::typelist::NullType;
  using lumiera::typelist::Tuple;
  using lumiera::typelist::Append;
  using lumiera::typelist::SplitLast;
  
  using lumiera::typelist::BuildTupleAccessor;
  
  /** 
   * Type analysis helper template. 
   * Used for dissecting a given type signature to derive
   * the related basic operation signature, the signature of a possible Undo-function
   * and the signature necessary for capturing undo information. The implementation
   * relies on re-binding an embedded type defining template, based on the actual
   * case, as identified by the structure of the given parameter signature.
   */
  template<typename SIG>
  struct UndoSignature
    {
    private:
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      typedef typename FunctionSignature< function<SIG> >::Ret  Ret;
      
      /** Case1: defining the Undo-Capture function */
      template<typename RET, typename ARG>
      struct Case
        {
          typedef RET Memento;
          typedef typename Append<ARG, Memento>::List ExtendedArglist;
          typedef typename Tuple<ExtendedArglist>::Type ExtendedArgs;
          
          typedef typename FunctionTypedef<void, ARG>::Sig           OperateSig;
          typedef typename FunctionTypedef<Ret,ARG>::Sig             CaptureSig;
          typedef typename FunctionTypedef<void, ExtendedArgs>::Sig  UndoOp_Sig;
        };
      /** Case2: defining the actual Undo function */
      template<typename ARG>
      struct Case<void,ARG>
        {
          typedef typename ARG::List Args;
          
          typedef typename SplitLast<Args>::Type Memento;
          typedef typename SplitLast<Args>::Prefix OperationArglist;
          typedef typename Tuple<OperationArglist>::Type OperationArgs;
          
          typedef typename FunctionTypedef<void, OperationArgs>::Sig OperateSig;
          typedef typename FunctionTypedef<Ret,OperationArgs>::Sig   CaptureSig;
          typedef typename FunctionTypedef<void, ARG>::Sig           UndoOp_Sig;
        };
      
    public:
      typedef typename Case<Ret,Args>::CaptureSig CaptureSig;
      typedef typename Case<Ret,Args>::UndoOp_Sig UndoOp_Sig;
      typedef typename Case<Ret,Args>::OperateSig OperateSig;
      typedef typename Case<Ret,Args>::Memento    Memento;
    };
  
  
  
  
  
  /** Interface */
  class CmdClosure
    {
    public:
      virtual ~CmdClosure() {}
    };
  
  
  template<typename TY, class BASE, uint idx>
  struct ParamAccessor
    : BASE
    {
      template<class TUP>
      ParamAccessor(TUP& tuple)
        : BASE(tuple)
        { 
           cout << showSizeof(tuple.template getAt<idx>()) << endl;
        }
    };
  template<class BASE>
  struct ParamAccessor<NullType, BASE, 0>
    : BASE
    {
      template<class TUP>
      ParamAccessor(TUP& tuple)
        : BASE(tuple)
        { }
    };
  
  template<typename SIG>
  class Closure
    : public CmdClosure
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
//    typedef typename FunctionSignature< function<SIG> >::Ret  Ret;
      
      typedef Tuple<Args> ArgTuple;
      
      typedef BuildTupleAccessor<Args,ParamAccessor> BuildAccessor;
      typedef typename BuildAccessor::Accessor ParamStorageTuple;
      
      ParamStorageTuple params_;
      
    public:
      Closure (ArgTuple& args)
        : params_(BuildAccessor(args))
        { }
    };
  
  
  
  
  /**
   * Helper class used solely for \em defining a Command-Object.
   * This technique is known as "fluent API", see http://en.wikipedia.org/wiki/Fluent_interface
   * The basic idea is for the user to create a disposable instance of this definition helper,
   * only for calling a chain of definition functions, which internally build the actual Command object.
   * Finally, the created Command object will be stored into a registry or handed over to the
   * ProcDispatcher. To give an example:
   * \code
   *    CommandDefinition ("test.command1")
   *           .operation (command1::operate)          // provide the function to be executed as command
   *           .captureUndo (command1::capture)        // provide the function capturing Undo state
   *           .undoOperation (command1::undoIt)       // provide the function which might undo the command
   *           .bind (obj, randVal)                    // bind to the actual command parameters
   *           .executeSync();                         // convenience call, forwarding the Command to dispatch.
   * \endcode
   * 
   * @todo of course, this needs to be extracted into command-definition.hpp 
   */
  class CommDef
    {
      Symbol id_;
      
      template<typename SIG, typename MEM>
      struct UndoDefinition
        {
          typedef typename FunctionSignature< function<SIG> >::Args BasicArgs;
          typedef typename FunctionTypedef<MEM,BasicArgs>::Sig      UndoCaptureSig;
          
          UndoDefinition (function<UndoCaptureSig>& undoCapOperation)
            {
              cout << showSizeof(undoCapOperation) << endl;
              UNIMPLEMENTED ("re-fetch command definition and augment it with Functor for capturing Undo information");
            }
          
          template<typename SIG2>
          UndoDefinition&
          undoOperation (SIG2& how_to_Undo)
            {
              typedef typename UndoSignature<SIG2>::UndoOp_Sig UndoSig;
              
              function<UndoSig> opera3 (how_to_Undo);
              
              UNIMPLEMENTED ("store actual Undo-Functor into the command definition held by the enclosing UndoDefinition instance");
              return *this;
            }
          
          template
            < typename T1
            , typename T2
            >
          UndoDefinition&    ///////TODO return here the completed Command
          bind ( T1& p1
               , T2& p2
               )
            {
              typedef Types<T1,T2> ArgTypes;
              Tuple<ArgTypes> params(p1,p2);
              Closure<SIG> clo (params);
              
              cout << showSizeof(clo) << endl;
              UNIMPLEMENTED ("complete Command definition by closing all functions");
              return *this;
            }
          
        };
      
      /** type re-binding helper: create a suitable UndoDefinition type,
       *  based on the UndoSignature template instance given as parameter */
      template<typename U_SIG>
      struct BuildUndoDefType
        {
          typedef UndoDefinition<typename U_SIG::OperateSig, typename U_SIG::Memento> Type;
        };
      
      template<typename SIG>
      struct BasicDefinition
        {
          BasicDefinition(function<SIG>& operation)
            {
              cout << showSizeof(operation) << endl;
              UNIMPLEMENTED ("create new command object an store the operation functor");
            }
          
          
          template<typename SIG2>
          typename BuildUndoDefType<UndoSignature<SIG2> >::Type
          captureUndo (SIG2& how_to_capture_UndoState)
            {
              typedef typename UndoSignature<SIG2>::CaptureSig UndoCapSig;
              typedef typename BuildUndoDefType<UndoSignature<SIG2> >::Type SpecificUndoDefinition;
              
              function<UndoCapSig> opera2 (how_to_capture_UndoState);
              return SpecificUndoDefinition (opera2);
            }
          
        };
      
    public:
      CommDef (Symbol cmdID)
        : id_(cmdID)
        { }
      
      template<typename SIG>
      BasicDefinition<SIG>
      operation (SIG& operation_to_define)
        {
          function<SIG> opera1 (operation_to_define);
          return BasicDefinition<SIG>(opera1);
        }
    };
  
  
  
  
  /////////////////////////////
  /////////////////////////////
  
  //////////////////////////// start of the actual Test....
  

/*  
    bind: opFunc(a,b,c) -> op(void)
    
    curry(opFunc) (a) (b) (c)
    
    pAppl(func, x) ->  func2 (b, c)
    
    return bind( recursion(), param)
     
*/  
  namespace command1 {
    void
    operate (P<Time> dummyObj, int randVal)
    {
      *dummyObj += Time(randVal);
    }
    
    Time
    capture (P<Time> dummyObj, int)
    {
      return *dummyObj;
    }
    
    void
    undoIt (P<Time> dummyObj, int, Time oldVal)
    {
      *dummyObj = oldVal;
    }
  
  }
  
  ////////////////////////////////////////////TODO braindump
  
  
  /***************************************************************************
   * @test basic usage of the Proc-Layer command dispatch system.
   *       Shows how to define a simple command inline and another
   *       simple command as dedicated class. Finally triggers
   *       execution of both commands and verifies the command
   *       action has been invoked.
   *       
   * @todo currently rather a scrapbook for trying out first ideas on the command system !!!!!!!!!!!!!!!
   *       
   * @see  control::Command
   * @see  control::CommandDef
   * @see  mobject::ProcDispatcher
   */
  class CommandBasic_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          /////////////////////////////////TODO
          defineCommands();
          checkExecution();
        }
      
      void
      defineCommands ()
        {
          P<Time> obj (new Time(5));
          int randVal ((rand() % 10) - 5);
          
          CommDef ("test.command1")
              .operation (command1::operate)
              .captureUndo (command1::capture)
              .undoOperation (command1::undoIt)
              .bind (obj, randVal)
              ;
        }
      
      void
      checkExecution ()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandBasic_test, "unit controller");
      
      
}} // namespace control::test
