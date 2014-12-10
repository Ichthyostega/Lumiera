/*
  DiffListApplication(Test)  -  demonstrate linearised representation of list diffs

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/verb-token.hpp"
#include "lib/util.hpp"
#include "lib/iter-adapter-stl.hpp"
//#include "lib/format-string.hpp"

//#include <iostream>
#include <boost/noncopyable.hpp>
#include <string>
#include <vector>
#include <tuple>

using util::isnil;
using std::string;
//using util::_Fmt;
using std::vector;
//using std::cout;


namespace lib {
namespace test{
  
  template<typename E>
  struct DiffLanguage
    {
      class Receiver
        {
        public:
          virtual ~Receiver() { } ///< this is an interface
          
          virtual void ins(E e)    =0;
          virtual void del(E e)    =0;
          virtual void pick(E e)   =0;
          virtual void push(E e)   =0;
        };
      
      using DiffVerb = VerbToken<Receiver, void(E)>;
      using VerbTok = std::tuple<DiffVerb, E>;
      
      struct DiffStep
        : VerbTok
        {
          DiffVerb& verb() { return std::get<0>(*this); }
          E         elm()  { return std::get<1>(*this); }
          
          DiffStep(DiffVerb verb, E e)
            : VerbTok(verb,e)
            { }
          
          operator string()  const
            {
              return string(verb()) + "("+string(elm())+")";
            }
          
          void
          applyTo (Receiver& receiver)
            {
              verb().applyTo (receiver, elm());
            }
        };
      
#define DiffStep_CTOR(_ID_) \
      static DiffStep _ID_(E e) { return {DiffVerb(&Receiver::_ID_, STRINGIFY(_ID_)), e }; }
      
      DiffStep_CTOR(ins)
      DiffStep_CTOR(del)
      DiffStep_CTOR(pick)
      DiffStep_CTOR(push)
      
    };
  
  template<class CON>
  class DiffApplicationStrategy;
  
  template<typename E, typename...ARGS>
  class DiffApplicationStrategy<vector<E,ARGS...>>
    : public DiffLanguage<E>::Receiver
    {
      using Vec = vector<E,ARGS...>;
      
      Vec& seq_;
      
      void
      ins(E e)
        {
          UNIMPLEMENTED("insert new element into target");
        }
      void
      del(E e)
        {
          UNIMPLEMENTED("insert given element from target");
        }
      void
      pick(E e)
        {
          UNIMPLEMENTED("pick denoted element from source");
        }
      void
      push(E e)
        {
          UNIMPLEMENTED("push next element behind denoted in source");
        }
      
    public:
      explicit
      DiffApplicationStrategy(vector<E>& targetVector)
        : seq_(targetVector)
        { }
    };
  
  
  template<class SEQ>
  class DiffApplicator
    : boost::noncopyable
    {
      using Val  = typename SEQ::value_type;
      using Diff = DiffLanguage<Val>;
      
      using Receiver = DiffApplicationStrategy<SEQ>;
      
      Receiver target_;
      
    public:
      explicit
      DiffApplicator(SEQ& targetSeq)
        : target_(targetSeq)
        { }
      
      template<class DIFF>
      void
      consume (DIFF diff)
        {
          for ( ; diff; ++diff )
            diff->applyTo(target_);
        }
      
    protected:
    };
  namespace {
    template<typename SEQ>
    struct _OnceT
      {
        typedef typename SEQ::value_type value_type;
        typedef iter_stl::IterSnapshot<value_type> iterator;
      };
  }
  
  template<class CON>
  inline typename _OnceT<CON>::iterator
  onceEach(CON const& con)
    {
      using OnceIter = typename _OnceT<CON>::iterator;
      return OnceIter(con);
    }
  
  template<class VAL>
  inline iter_stl::IterSnapshot<VAL>
  onceEach(std::initializer_list<VAL> const& ili)
    {
      using OnceIter = iter_stl::IterSnapshot<VAL>;
      return OnceIter(begin(ili), end(ili));
    }
    
  namespace {
    
    using DataSeq = vector<string>;
    
    #define TOK(id) id(STRINGIFY(id))
    
    string TOK(a1), TOK(a2), TOK(a3), TOK(a4), TOK(a5);
    string TOK(b1), TOK(b2), TOK(b3), TOK(b4);
    
    struct TestDiff
      : DiffLanguage<string>
      {
        using DiffSeq = typename _OnceT<std::initializer_list<DiffStep>>::iterator;
        
        static DiffSeq
        generate()
          {
            return onceEach({del(a1)
                           , del(a2)
                           , ins(b1)
                           , pick(a3)
                           , push(a5)
                           , pick(a5)
                           , ins(b2)
                           , ins(b3)
                           , pick(a4)
                           , ins(b4)
                           });
            
          }
      };
  }
  
  
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration/Concept: a description language for list differences.
   *       The representation is given as a linearised sequence of verb tokens.
   *       This test demonstrates the application of such a diff representation
   *       to a given source list, transforming this list to hold the intended
   *       target list contents.
   *       
   * @see session-structure-mapping-test.cpp
   */
  class DiffListApplication_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          DataSeq src({a1,a2,a3,a4,a5});
          auto diff = TestDiff::generate();
          CHECK (!isnil (diff));
          
          DataSeq target = src;
          DiffApplicator<DataSeq> application(target);
          application.consume(diff);
          
          CHECK (isnil (diff));
          CHECK (!isnil (target));
          CHECK (src != target);
          CHECK (target == DataSeq({b1,a3,a5,b2,b3,a4,b4}));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffListApplication_test, "unit common");
  
  
  
}} // namespace lib::test
