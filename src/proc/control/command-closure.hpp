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
#include "lib/bool-checkable.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/meta/function-erasure.hpp"
#include "lib/meta/tuple.hpp"
#include "lib/format.hpp"
#include "lib/util.hpp"
#include "proc/control/argument-tuple-accept.hpp"  ////TODO better factor out struct TypedArguments
#include "proc/control/typed-allocation-manager.hpp"

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
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::Tuple;
  using lumiera::typelist::BuildTupleAccessor;
  using lumiera::typelist::func::TupleApplicator;
  using lumiera::typelist::FunErasure;
  using lumiera::typelist::StoreFunction;
  using lumiera::typelist::NullType;

  using util::unConst;

  using std::tr1::function;
  using std::ostream;
  using std::string;
  
  
  
  LUMIERA_ERROR_DECLARE (UNBOUND_ARGUMENTS);  ///< Command functor not yet usable, because arguments aren't bound
  
  
  /** 
   * A neutral container internally holding 
   * the functor used to implement the Command
   */
  typedef FunErasure<StoreFunction> CmdFunctor;
  
  class CmdClosure;
  typedef std::tr1::shared_ptr<CmdClosure> PClo;
  
  
  
  /** Interface */
  class CmdClosure
    : public lib::BoolCheckable<CmdClosure>
    {
    public:
      virtual ~CmdClosure() {}
      
      virtual operator string() const =0;
      
      virtual bool isValid ()   const =0;
      
      virtual void bindArguments (Arguments&) =0;
      
      virtual CmdFunctor closeArguments (CmdFunctor const&) =0;
      
      virtual PClo createClone (TypedAllocationManager&) =0;
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
      
      
      ////////////////////TODO the real access operations (e.g. for serialising) go here
      
      
      ostream&
      dump (ostream& output)  const
        {
          return BASE::dump (output << util::str (element()) << ',');
        }
      
      friend bool
      compare (ParamAccessor const& p1, ParamAccessor const& p2)
        {
          return (p1.element() == p2.element())
              && compare ( static_cast<BASE>(p1)
                         , static_cast<BASE>(p2) );
        }
    };
    
  template<class TUP>
  class ParamAccessor<NullType, TUP, TUP, 0>   ///< used for recursion end of implementation functions
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
      
      friend bool
      compare (ParamAccessor const&, ParamAccessor const&)
        {
          return true;;
        }
    };
  
  
  
  template<typename SIG>
  class Closure
    : public CmdClosure
    {
      typedef typename FunctionSignature< function<SIG> >::Args Args;
      
      
      typedef BuildTupleAccessor<Args,ParamAccessor> BuildAccessor;
      typedef typename BuildAccessor::Accessor ParamStorageTuple;
      
      ParamStorageTuple params_;
      
    public:
      typedef Tuple<Args> ArgTuple;
      
      Closure (ArgTuple const& args)
        : params_(BuildAccessor(args))
        { }
      
      /** create a clone copy of this, without disclosing the exact type */
      PClo
      createClone (TypedAllocationManager& storageManager)
        {
          return storageManager.create<Closure> (*this);
        }
      
      /** assign a new parameter tuple to this */
      void
      bindArguments (Arguments& args)
      {
        params_ = args.get<ArgTuple>();
      }
      
      
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
      closeArguments (CmdFunctor const& unboundFunctor)
        {
          return CmdFunctor (TupleApplicator<SIG> (params_)
                               .bind ( unboundFunctor.getFun<SIG>()) );
        }
      
      
      
      operator string()  const
        {
          std::ostringstream buff;
          params_.dump (buff << "Closure(" );
          
          string dumped (buff.str());
          if (8 < dumped.length())
            // remove trailing comma...
            return dumped.substr (0, dumped.length()-1) +")";
          else
            return dumped+")";
        }
      
      
      bool isValid ()  const { return true; }
      
      
      /// Supporting equality comparisons...
      friend bool operator== (Closure const& c1, Closure const& c2)  { return compare (c1.params_, c2.params_); }
      friend bool operator!= (Closure const& c1, Closure const& c2)  { return ! (c1 == c2); }
    };
    
    
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
