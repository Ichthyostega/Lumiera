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
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/meta/tuple.hpp"


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
          typedef typename SplitLast<Args>::List OperationArglist;
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
  
  
  template
    < typename TY
    , class BASE
    , class TUP
    , uint idx
    >
  struct ParamAccessor
    : BASE
    {
      ParamAccessor(TUP& tuple)
        : BASE(tuple)
        { 
           cout << showSizeof(tuple.template getAt<idx>()) << endl;
        }
      
      ////////////////////TODO the real access operations (e.g. for serialising) go here
    };
    
  template<class TUP>
  struct ParamAccessor<NullType, TUP, TUP, 0>
    : TUP
    {
      ParamAccessor(TUP& tuple)
        : TUP(tuple)
        { }
      
      ////////////////////TODO the recursion-end of the access operations goes here
    };
  
  
  
  template<typename SIG>
  class Closure
    : public CmdClosure
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      
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
