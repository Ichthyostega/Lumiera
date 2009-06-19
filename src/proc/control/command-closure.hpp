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
#include "lib/meta/tuple.hpp"

#include <tr1/memory>
#include <tr1/functional>
#include <iostream> 
#include <string>


#include "lib/test/test-helper.hpp"  /////////////////TODO remove this
using lib::test::showSizeof;

using std::cout;    //////////////////////////////////TODO remove this
using std::endl;


namespace control {
  
//  using lumiera::Symbol;
//  using std::tr1::shared_ptr;
  using std::string;
  using std::ostream;
  using std::tr1::function;
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::Tuple;
  using lumiera::typelist::BuildTupleAccessor;
  
  using lumiera::typelist::NullType;
  
  class CmdClosure;
  typedef std::tr1::shared_ptr<CmdClosure> PClo;  ///< smart-ptr type used for handling concrete closures
  
  
  /** Interface */
  class CmdClosure
    {
    public:
      virtual ~CmdClosure() {}
      
      virtual PClo clone()  const =0;
      
      virtual operator string()  const =0;
      
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
      TY& element() { return BASE::template getAt<idx>(); }
      
    public:
        
      ParamAccessor(TUP const& tuple)
        : BASE(tuple)
        { 
          cout << showSizeof(element()) << endl;   //////////TODO remove this test code
        }
      
      string
      dump (string const& prefix = "(")  const
        {
          return BASE::dump (prefix+element());
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
      
    protected:
      string
      dump (string const& prefix)  const
        { 
          return prefix+")";
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
      
      
      /** create a clone copy of this concrete closure,
       *  hidden behind the generic CmdClosure interface
       *  and owned by a shared_ptr PClo.
       */
      PClo
      clone()  const
        {
          return PClo (new Closure (this->params_));
        }
      
      operator string()  const
        {
          UNIMPLEMENTED ("how to do a string conversion on the variable argument tuple??");
        }
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
} // namespace control
#endif
