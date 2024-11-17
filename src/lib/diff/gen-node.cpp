/*
  GenNode  -  generic node element for tree like data representation

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file gen-node.cpp
 ** Generic node element (implementation parts).
 ** Some of the more technical details regarding value access and
 ** comparisons has been moved down within this compilation unit,
 ** to cut down compilation time.
 ** 
 ** \par comparison and match
 ** 
 ** The DataCap element provides a set of functions to check for \em equivalence
 ** or match. These are used to build a recursive containment check. To implement
 ** such predicates, we need to build a one-way off visitor for use with lib::Variant.
 ** These specifically tailored functors only define \c handle(TY) functions for the
 ** cases actually of interest. All other cases invoke the default handling, which
 ** returns \c false.
 ** 
 ** @see gen-node-test.cpp
 ** 
 */


#include "lib/error.hpp"
#include "lib/diff/diff-language.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/util-quant.hpp"
#include "lib/variant.hpp"

#include <boost/lexical_cast.hpp>


using util::join;
using boost::lexical_cast;
using lib::time::TimeValue;
using lib::transformIterator;
using util::almostEqual;
using lib::hash::LuidH;


namespace lib {
namespace diff{
  
  /* symbolic marker ID references
   * used within the tree diff language
   * to mark specific scopes and situations
   */
  const Ref Ref::I      ("_I_");
  const Ref Ref::NO     ("_NO_");
  const Ref Ref::END    ("_END_");
  const Ref Ref::THIS   ("_THIS_");     ////////TICKET #996 : Feature of questionable usefulness. Maybe dispensable?
  const Ref Ref::CHILD  ("_CHILD_");
  const Ref Ref::ATTRIBS("_ATTRIBS_");
  
  
  
  
  /** Implementation of content equality test, delgating to content
   * @throws error::Logic when the given other DataCap
   *         does not hold a value of the same type than
   *         this DataCap.
   * @remarks since the GenNode::ID is generated including a type hash,
   *         the equality operator of GenNode ensures this content test
   *         is only called on a compatible DataCap.
   */
  bool
  DataCap::matchData (DataCap const& o)  const
  {
    class EqualityTest
      : public Variant<DataValues>::Predicate
      {
        DataCap const& o_;

#define DERIVE_EQUALITY(_TY_) \
        virtual bool handle  (_TY_ const& val) override { return (o_.get<_TY_>() == val); }
        
        DERIVE_EQUALITY (int)
        DERIVE_EQUALITY (int64_t)
        DERIVE_EQUALITY (short)
        DERIVE_EQUALITY (char)
        DERIVE_EQUALITY (bool)
        DERIVE_EQUALITY (double)
        DERIVE_EQUALITY (string)
        DERIVE_EQUALITY (time::Time)
        DERIVE_EQUALITY (time::Offset)
        DERIVE_EQUALITY (time::Duration)
        DERIVE_EQUALITY (time::TimeSpan)
        DERIVE_EQUALITY (hash::LuidH)
        DERIVE_EQUALITY (Rec)
        
        /** special treatment to allow matching a RecRef
         * with an Record or RecRef on the other side */
        virtual bool
        handle  (RecRef const& val) override
          {
            return o_.matchRec(val);
          }
        
      public:
        EqualityTest(DataCap const& o)
          : o_(o)
          { }
      };
    
    EqualityTest visitor(o);
    return accept(visitor);
  }
  
  
  bool
  DataCap::matchNum (int64_t num)  const
  {
    class MatchNumber
      : public Variant<DataValues>::Predicate
      {
        int64_t num_;

#define MATCH_NUMBER(_TY_) \
        virtual bool handle  (_TY_ const& val) override { return val == num_; }
        
        MATCH_NUMBER (int)
        MATCH_NUMBER (int64_t)
        MATCH_NUMBER (short)
        MATCH_NUMBER (char)
        MATCH_NUMBER (double)
        
      public:
        MatchNumber(int64_t num)
          : num_(num)
          { }
      };
    
    MatchNumber visitor(num);
    return accept(visitor);
  }
  
  
  bool
  DataCap::matchDbl (double d)  const
  {
    class MatchDouble
      : public Variant<DataValues>::Predicate                    //////////////////////////////////TICKET #1360 floating-point precision
      {
        double num_;

#define MATCH_DOUBLE(_TY_) \
        virtual bool handle  (_TY_ const& val) override { return almostEqual (double(val), num_); }
        
        MATCH_DOUBLE (int)
        MATCH_DOUBLE (int64_t)
        MATCH_DOUBLE (short)
        MATCH_DOUBLE (char)
        MATCH_DOUBLE (double)
        
      public:
        MatchDouble(double d)
          : num_(d)
          { }
      };
    
    MatchDouble visitor(d);
    return accept(visitor);
  }
  
  
  bool
  DataCap::matchTxt (string const& text)  const
  {
    class MatchString
      : public Variant<DataValues>::Predicate
      {
        string const& txt_;

#define MATCH_STRING(_TY_) \
        virtual bool handle  (_TY_ const& val) override { return lexical_cast<string>(val) == txt_; }
        
        MATCH_STRING (int)
        MATCH_STRING (int64_t)
        MATCH_STRING (short)
        MATCH_STRING (double)
        MATCH_STRING (bool)
        
        virtual bool handle  (string const& str) override { return str == txt_; }
        virtual bool handle  (char   const& c  ) override { return 1 == txt_.length() && txt_.front() == c; }
        
      public:
        MatchString(string const& text)
          : txt_(text)
          { }
      };
    
    MatchString visitor(text);
    return accept(visitor);
  }
  
  
  bool
  DataCap::matchTime (TimeValue time)  const
  {
    class MatchTime
      : public Variant<DataValues>::Predicate
      {
        TimeValue& t_;

#define MATCH_TIME(_TY_) \
        virtual bool handle  (_TY_ const& val) override { return val == t_; }
        
        MATCH_TIME (time::Time)
        MATCH_TIME (time::Offset)
        MATCH_TIME (time::Duration)
        MATCH_TIME (time::TimeSpan)
        MATCH_TIME (hash::LuidH)
        
      public:
        MatchTime(TimeValue& t)
          : t_(t)
          { }
      };
    
    MatchTime visitor(time);
    return accept(visitor);
  }
  
  
  bool
  DataCap::matchBool (bool b)  const
  {
    bool* val = unConst(this)->maybeGet<bool>();
    return val && (b == *val);
  }
  
  
  bool
  DataCap::matchLuid (LuidH hash)  const
  {
    LuidH* val = unConst(this)->maybeGet<LuidH>();
    return val && (hash == *val);
  }
  
  
  bool
  DataCap::matchRec (RecRef const& ref)  const
  {
    if (ref)
      return matchRec (*ref.get());
    else
      {
        RecRef* val = unConst(this)->maybeGet<RecRef>();
        return val && val->empty();
      }
  }
  
  
  bool
  DataCap::matchRec (Rec const& rec)  const
  {
    Rec* val = unConst(this)->maybeGet<Rec>();
    if (!val)
      {
        RecRef* r = unConst(this)->maybeGet<RecRef>();
        if (r) val = r->get();
      }
    return val && (rec == *val);
  }
  
  
  
  
  DataCap::operator string()  const
  {
    return "DataCap|"+string(this->buffer());
  }
  
  
  
  
  
  /** compact textual representation of a Record<GenNode> (»object«). */
  string renderCompact (Rec const& rec)
  {
    auto renderChild  = [](diff::GenNode const& n){ return renderCompact(n); };
    auto renderAttrib = [](diff::GenNode const& n){
                                                    return (n.isNamed()? n.idi.getSym()+"=" : "")
                                                         +  renderCompact(n);
                                                  };
    
    return (Rec::TYPE_NIL==rec.getType()? "" : rec.getType())
         + "{"
         +  join (transformIterator (rec.attribs(), renderAttrib))
         + (isnil(rec.scope())?   "" : "|")
         +  join (transformIterator (rec.scope()  , renderChild))
         + "}"
         ;
  }
  
  string
  renderCompact (RecRef const& ref)
  {
    return "Ref->" + (ref.empty()? util::BOTTOM_INDICATOR
                                 : renderCompact (*ref.get()));
  }
  
  /** @remark presentation is oriented towards readability
   *   - numbers are slightly rounded (see \ref util::showDouble() )
   *   - time values are displayed timecode-like
   *   - nested scopes are displayed recursively, enclosed in curly brackets
   * @see text-template-gen-node-binding.hpp
   */
  string
  renderCompact (GenNode const& node)
  {
        class Renderer
          : public Variant<diff::DataValues>::Renderer
          {
    #define RENDER_CONTENT(_TY_) \
            virtual string handle  (_TY_ const& val) override { return util::toString(val); }
            
            RENDER_CONTENT (int)
            RENDER_CONTENT (int64_t)
            RENDER_CONTENT (short)
            RENDER_CONTENT (char)
            RENDER_CONTENT (double)
            RENDER_CONTENT (bool)
            RENDER_CONTENT (time::Time)
            RENDER_CONTENT (time::Offset)
            RENDER_CONTENT (time::Duration)
            RENDER_CONTENT (time::TimeSpan)
     #undef RENDER_CONTENT
            
            virtual string handle (string const& val) override { return val; }
            virtual string handle (LuidH const& val)  override { return util::showHash(val, 2);}
            virtual string handle (RecRef const& ref) override { return renderCompact(ref); }
            virtual string handle (Rec const& rec)    override { return renderCompact(rec); }
          };
    
    Renderer visitor;
    return node.data.accept (visitor);
  }
  
  
}} // namespace lib::diff
