/*
  COMMAND-CLOSURE.hpp  -  defining execution targets and parameters for commands
 
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
 
*/


/** @file command-closure.hpp
 ** A closure enabling self-contained execution of commands within the ProcDispatcher.
 ** //TODO
 **  
 ** @see Command
 ** @see ProcDispatcher
 **
 */



#ifndef CONTROL_COMMAND_CLOSURE_H
#define CONTROL_COMMAND_CLOSURE_H

//#include "pre.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/meta/function-erasure.hpp"
#include "lib/meta/tuple.hpp"
#include "lib/util.hpp"

#include <tr1/memory>
#include <tr1/functional>
#include <iostream>
#include <sstream>
#include <string>


#include "lib/test/test-helper.hpp"  /////////////////TODO remove this
using lib::test::showSizeof;

using std::cout;    //////////////////////////////////TODO remove this
using std::endl;


namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  using util::unConst;
  using std::string;
  using std::ostream;
  using std::tr1::function;
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::Tuple;
  using lumiera::typelist::BuildTupleAccessor;
  using lumiera::typelist::func::TupleApplicator;
  using lumiera::typelist::FunErasure;
  using lumiera::typelist::StoreFunction;
  
  using lumiera::typelist::NullType;
    
  
  /** 
   * A neutral container internally holding 
   * the functor used to implement the Command
   */
  typedef FunErasure<StoreFunction> CmdFunctor;
  
  
  class CmdClosure;
  typedef std::tr1::shared_ptr<CmdClosure> PClosure;  ///< smart-ptr type used for handling concrete closures
  
  
  /** Interface */
  class CmdClosure
    {
    public:
      virtual ~CmdClosure() {}
      
      virtual PClosure clone()  const =0;
      
      virtual operator string()  const =0;
      
      virtual CmdFunctor bindArguments (CmdFunctor&) =0;
    };
  
  
  inline ostream& operator<< (ostream& os, const CmdClosure& clo) { return os << string(clo); }
  
  
  
  template
    < typename TY
    , class BASE
    , class TUP
    , uint idx
    >
  class ParamAccessor
    : public BASE
    {
      TY      & element()        { return          BASE::template getAt<idx>(); }
      TY const& element()  const { return unConst(this)->template getAt<idx>(); }
      
    public:
        
      ParamAccessor(TUP const& tuple)
        : BASE(tuple)
        { }
      
      ostream&
      dump (ostream& output)  const
        {
          return BASE::dump (output << element() << ',');
        }
      
      
      ////////////////////TODO the real access operations (e.g. for serialising) go here
    };
    
  template<class TUP>
  class ParamAccessor<NullType, TUP, TUP, 0>
    : public TUP
    {
    public:
      ParamAccessor(TUP const& tuple)
        : TUP(tuple)
        { }
      
      ////////////////////TODO the recursion-end of the access operations goes here
      
      ostream&
      dump (ostream& output)  const
        { 
          return output;
        }
      
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
      Closure (ArgTuple const& args)
        : params_(BuildAccessor(args))
        { }
      
      
      /** Core operation: use the embedded argument tuple
       *  to close a given functor over its arguments.
       *  @param unboundFunctor an function object, whose
       *         function arguments are required to match 
       *         the types of the embedded ParamStorageTuple
       *  @return new functor object containing the function<void()>,
       *         which is created by binding all arguments of the
       *         input functor.
       *  @note  ASSERTION failure if the function signature 
       *         doesn't match the argument types tuple.
       *  @note  when finally invoked, the functor, which is
       *         bound here to the argument tuple, might actually
       *         \em modify the param values. Thus this function
       *         can't be const.
       */
      CmdFunctor
      bindArguments (CmdFunctor& unboundFunctor)
        {
          return CmdFunctor (TupleApplicator<SIG> (params_)
                               .bind ( unboundFunctor.getFun<SIG>()) );
        }
      
      
      /** create a clone copy of this concrete closure,
       *  hidden behind the generic CmdClosure interface
       *  and owned by a shared_ptr PClo.
       */
      PClosure
      clone()  const
        {
          return PClosure (new Closure (this->params_));
        }
      
      operator string()  const
        {
          std::ostringstream buff;
          params_.dump (buff << "Closure(" );
          
          string dumped (buff.str());
          if (1 < dumped.length())
            // remove trailing comma...
            return dumped.substr (0, dumped.length()-1) +")";
          else
            return dumped+")";
        }
    };
    
    
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
