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

#include <tr1/functional>
//#include <boost/format.hpp>
//#include <iostream>
#include <string>

using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
using std::tr1::function;
//using boost::format;
using lumiera::Time;
//using util::contains;
using std::string;
//using std::cout;


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

  template<class LI>
  struct Tuple;

  template<>
  struct Tuple<NullType>
    {
      typedef Types<> Type;
    };
  
  template<class TY, class TYPES>
  struct Tuple<Node<TY,TYPES> >
    {
      typedef typename Prepend<TY, typename Tuple<TYPES>::Type >::Tuple Type;
    };
  
  
  
}} // namespace lumiera::typelist
  
namespace control {
namespace test    {

//  using session::test::TestClip;
  using lumiera::P;
  
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::FunctionTypedef;
  
  using lumiera::typelist::Tuple;
  using lumiera::typelist::Append;
  
  template<typename SIG, typename MEM>
  struct BuildUndoCapturing_Signature
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      typedef typename FunctionTypedef<MEM,Args>::Sig           type;
    };
  
  template<typename SIG, typename MEM>
  struct BuildUndoOperation_Signature
    {
      typedef typename FunctionSignature< function<SIG> >::Args::List Args;
      typedef typename Append<Args, MEM>::List                        ExtendedArglist;
      typedef typename Tuple<ExtendedArglist>::Type                   ExtendedArgs;
      typedef typename FunctionTypedef<void, ExtendedArgs>::Sig       type;
    };
  
  
  class CommDef
    {
      Symbol id_;
      
      template<typename SIG, typename MEM>
      struct UndoDefinition
        {
          typedef typename BuildUndoOperation_Signature<SIG,MEM>::type UndoSig; 
          
          UndoDefinition
          undoOperation (UndoSig& how_to_Undo)
            {
              function<UndoSig> opera3 (how_to_Undo);
              
            }
          
        };
      
      template<typename SIG>
      struct BasicDefinition
        {
          template<typename MEM>
          UndoDefinition<SIG,MEM>
          captureUndo (typename BuildUndoCapturing_Signature<SIG,MEM>::type& how_to_capture_UndoState)
            {
              typedef typename BuildUndoCapturing_Signature<SIG,MEM>::type UndoCapSig;
              
              function<UndoCapSig> opera2 (how_to_capture_UndoState);
              
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
        }
    };
  
  
  
  
  /////////////////////////////
  /////////////////////////////

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
        }
      
      void
      defineCommands ()
        {
          CommDef ("test.command1")
              .operation (command1::operate)
              .captureUndo<Time> (command1::capture)                 /////////////////TODO: can we get rid of the type hint? (i.e. derive the type <Time> automatically?)
              .undoOperation (command1::undoIt)
//              .bind (obj, randVal)
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
