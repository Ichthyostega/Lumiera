/*
  RECORD.hpp  -  collection to represent object-like data

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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

*/


/** @file record.hpp
 ** Special collection to represent object-like data.
 ** To be used in a context where introspection, open, extensible definitions
 ** and loose coupling of data representation matters. Typically, structures
 ** defined in terms of Record elements are linked to the actual \em core
 ** representation of the same entities relying on
 ** \link diff-language.hpp diff messages. \endlink
 ** Record is one of the supported flavours within the DataCap of GenNode elements,
 ** which in turn serve as the standard handle to refer to other elements, entities,
 ** attributes or references within the "backbone" of the Lumiera GUI.
 ** 
 ** \par design decisions
 ** The Record type is shaped from its intended use: It serves to symbolically represent
 ** \em objects in the "external tree description". Here, \em objects means objects for
 ** real, i.e. with types, fields and an enclosed scope. But \em external means that we
 ** do not work on these objects right here, we only represent them, for later referral,
 ** \em symbolically.
 ** 
 ** This leads to the following decisions
 ** - the Record entity is itself an object and thus has an inner side, privately.
 **   The entrails of the Record can be reworked and tuned for performance
 ** - yet the Record has an external appearance, which makes it look flat and passive.
 **   This is to say, a Record has no visible functionality.
 ** - the parts or \em realms within this symbolic representation are distinguished
 **   by convention solely
 **   
 **   * metadata is very limited and boils down to magic attributes known by name
 **   * children (scope contents) can be recognised by \em not bearing a name
 ** 
 ** Record entities are meant to be immutable. The proper way to alter a Record is
 ** to apply a \link tree-diff.hpp diff \endlink
 ** 
 ** \par rationale
 ** The underlying theme of this design is negative, dialectical: we do not want to
 ** build yet another object system. The object model of C++ is deemed adequate.  
 ** 
 ** @see GenericRecordRepresentation_test
 ** 
 */


#ifndef LIB_DIFF_RECORD_H
#define LIB_DIFF_RECORD_H


#include "lib/error.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/itertools.hpp"

//#include "lib/util.hpp"
//#include "lib/format-string.hpp"
#include <boost/noncopyable.hpp>

#include <utility>
#include <vector>
#include <string>
//#include <map>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
//using util::_Fmt;
  using std::string;
  
  
  
  /** object-like record of data */
  template<typename VAL>
  class Record
    {
      using _Vec = std::vector<VAL>;
      using Attrib = std::pair<Symbol, VAL>;
      using Attribs = _Vec;
      using Children = _Vec;
      
      string type_;
      Attribs attribs_;
      Children children_;
      
    public:
      Record()
        : type_("NIL")
        { }
      
      template<typename A, typename C>
      Record(Symbol typeID, A&& att, C&& chi)
        : type_(typeID)
        , attribs_(std::forward<A> (att))
        , children_(std::forward<C> (chi))
        { }
      
      template<typename A, typename C>
      Record(Symbol typeID, std::initializer_list<A> const&& att
                          , std::initializer_list<C> const&& chi)
        : type_(typeID)
        , attribs_(att)
        , children_(chi)
        { }
      
      template<typename SEQ>
      explicit
      Record (SEQ const& con)
        : type_("NIL")
        {
          auto p = std::begin(con);
          auto e = std::end(con);
          if (p!=e && isTypeID (*p))
            type_ = extractTypeID(*(p++));
          for ( ; p!=e && isAttribute(*p); ++p)
            attribs_.push_back (*p);
          for ( ; p!=e; ++p)
            children_.push_back (*p);
        }
      
      Record (std::initializer_list<VAL> const&& ili)
        : Record(ili)
        { }
      
      // all default copy operations acceptable
      
      
      operator std::string()  const
        {
          return "nebbich";  ////TODO
        }
      
      
      bool
      empty()  const
        {
          return attribs_.empty()
              && children_.empty();
        }
      
      string
      getType()  const
        {
          return type_;
        }
      
      bool
      hasAttribute (string key)  const
        {
          return false; ////TODO
        }
      
      bool
      contains (VAL const& ref)  const
        {
          return false; ////TODO
        }
      
      VAL const&
      get (string key)  const
        {
          return "booo"; ////TODO
        }
      
      /**
       * While otherwise immutable,
       * a Record object can be remoulded
       * with the help of a Mutator object
       * @remarks a Mutator basically wraps a \em copy
       *          of the original object. After performing
       *          the desired changes, the altered copy can either
       *          be sliced out (by conversion), or moved overwriting
       *          an existing other Record instance (implemented as swap)
       */
      class Mutator;
      
      
      /**
       * copy-initialise (or convert) from the given Mutator instance.
       * @remarks need to code this explicitly, otherwise Record's
       *          build-from sequence templated ctor would kick in.
       */
      Record (Mutator const& mut)
        : Record((Record const&) mut)
        { }
      Record (Mutator && mut)
        : Record(std::move ((Record) mut))
        { }
      
      friend class Mutator;
      
      
      
      /* ==== Exposing scope and contents for iteration ====== */
      
      using iterator  = IterAdapter<typename _Vec::const_iterator, const Record*>;
      using scopeIter = typename iter_stl::_SeqT<const _Vec>::Range;
      using keyIter   = TransformIter<scopeIter, string>;
      using valIter   = TransformIter<scopeIter, VAL>;
      
      /** default iteration exposes all data within this "object", starting with the attributes */
      iterator  begin ()  const { return iterator(this, attribs_.begin()); }
      iterator  end ()    const { return iterator(); }
      
      scopeIter attribs() const { return iter_stl::eachElm(attribs_); }
      scopeIter scope()   const { return iter_stl::eachElm(children_); }
      
      keyIter   keys()    const { return transformIterator(attribs(), extractKey); }
      valIter   vals()    const { return transformIterator(attribs(), extractVal); }
      
    protected: /* ==== API for the IterAdapter ==== */
      
      /** Implementation of Iteration-logic: pull next element. */
      template<class ITER>
      friend void
      iterNext (const Record* src, ITER& pos)
        {
          ++pos;
          checkPoint (src,pos);
        }
      
      /** Implementation of Iteration-logic: detect iteration end.
       * @remarks seamless continuation of the iteration when reaching
       *    the end of the attribute collection. In this implementation,
       *    we use the default constructed \c ITER() to mark iteration end.
       */
      template<class ITER>
      friend bool
      checkPoint (const Record* src, ITER& pos)
        {
          REQUIRE (src);
          if ((pos != ITER()) && (pos != src->children_.end()))
            return true;
          else
            if (pos != ITER() && (pos == src->attribs_.end()) && !src->children_.empty())
              {
                pos = src->children_.begin();
                return true;
              }
            else
              {
                pos = ITER();
                return false;
        }     }
      
    private:
      static bool
      isAttribute (VAL const& v)
        {
          return false; ////TODO
        }
      
      static bool
      isTypeID (VAL const& v)
        {
          return false; ////TODO
        }
      
      static string
      extractTypeID (VAL const& v)
        {
          return "todo"; ////TODO
        }
      
      static VAL
      buildTypeAttribute (string const& typeID)
        {
          return VAL(); ///TODO
        }
      
      static string
      extractKey (VAL const& v)
        {
          return "todo"; ////TODO
        }
      
      static VAL
      extractVal (VAL const& v)
        {
          return VAL(); ///TODO
        }
      
      
      friend bool
      operator== (Record const& r1, Record const& r2)
      {
        return false; ////TODO
      }
      
      friend bool
      operator!= (Record const& r1, Record const& r2)
      {
        return ! (r1 == r2);
      }
    };
  
  
  
  template<typename VAL>
  class Record<VAL>::Mutator
    : boost::noncopyable
    {
      using Rec = Record<VAL>;
      
      Rec record_;
      
    public:
      explicit
      Mutator (Rec const& startingPoint)
        : record_(startingPoint)
        { }
      
      explicit
      Mutator (Rec && startingPoint)
        : record_(std::move (startingPoint))
        { }
      
      operator Rec&()
        {
          return record_;
        }
      
      void
      replace (Rec& existingInstance)  noexcept
        {
          std::swap (existingInstance, record_);
        }
      
      
      /* === functions to alter contents === */
      
      void
      setType (string const& newTypeID)
        {
          set ("type", Rec::buildTypeAttribute (newTypeID));
          record_.type_ = newTypeID;
        }
      
      void
      set (string const& key, VAL const& newValue)
        {
          ///////////TODO;
        }
      
      void
      appendChild (VAL const& newChild)
        {
          record_.children_.push_back (newChild);
        }
      
      void
      prependChild (VAL const& newChild)
        {
          record_.children_.insert (record_.children_.begin(), newChild);
        }
      
      bool
      empty()  const
        {
          return record_.empty();
        }
    };
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_GEN_NODE_H*/
