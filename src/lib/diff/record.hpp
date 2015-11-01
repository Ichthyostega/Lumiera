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
 **   * metadata is very limited and boils down to a type attribute known by name
 **   * children (scope contents) can be recognised by \em not bearing a name
 ** 
 ** Record entities are meant to be immutable. The proper way to alter a Record is
 ** to apply a \link tree-diff.hpp diff \endlink. Yet for the \em implementation
 ** of this diff handling, a Record::Mutator is provided, to allow controlled
 ** partial re-building of a given data element.
 ** 
 ** \par rationale
 ** The underlying theme of this design is negative, dialectical: we do not want to
 ** build yet another object system. The object model of C++ is deemed adequate.
 ** 
 ** @remarks
 **  - the implementation is focused on the intended primary use case,
 **    which is to exchange diff messages drawn against a symbolic representation
 **    of a typed object tree. Especially, we assume that there is only a small
 **    number of attributes (so linear search for access by key is adequate).
 **  - moreover, we assume that the value type allows for somehow embedding
 **    the key of each attribute; the implementation needs an explicit
 **    specialisation of the binding functions for each value type.
 **  - this header defines a specialisation for VAL = std::string --
 **    while the most relevant specialisation for GenNode is provided
 **    alongside with this special, monadic value type.
 **  - an alternative implementation approach would have been to use a
 **    dedicated helper type to represent the collection of attributes.
 **    This type might then be specialised, e.g. to utilise an index table
 **    for key-value lookup. However, in the light of the intended usage
 **    of Record entities as tree nodes within a GenNode monad, such a
 **    more elaborate approach was deemed unnecessary for the time being.
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
#include "lib/util.hpp"

#include "lib/diff/record-content-mutator.hpp"

#include <boost/noncopyable.hpp>

#include <algorithm>
#include <utility>
#include <vector>
#include <string>



namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
  using util::isnil;
  using std::string;
  
  template<typename VAL>
  struct RecordSetup;
  
  
  /**
   * object-like record of data.
   * For symbolic representation of "objects".
   * A Record holds both \em attributes (key-value data)
   * plus a list of \em enclosed children, which are conceived
   * to be within the "scope" of this Record. Optionally, a \em typeID
   * (metadata) may be defined. Otherwise, this typeID defaults to \c "NIL".
   * The representation of attributes depends on the actual value type, which
   * somehow need the ability to encode the keys within the value data.
   * By default, a specialisation is given for string, using the \c "key = val"
   * syntax. Yet the most relevant use case is \c Record<GenNode> -- using the
   * embedded name-ID of the GenNode elements as key for attributes.
   * 
   * Record elements are meant to be immutable; they can be created from a
   * defining collection. However, we provide a #Mutator mechanism to allow
   * for rebuilding and mutating symbolic data structures based on Records
   * and GenNode. Essentially, Lumiera's diff framework relies on this.
   */
  template<typename VAL>
  class Record
    {
      using Storage  = typename RecordSetup<VAL>::Storage;
      using ElmIter  = typename RecordSetup<VAL>::ElmIter;
      using Access   = typename RecordSetup<VAL>::Access;
      
      
      string type_;
      Storage attribs_;
      Storage children_;
      
    public:
      static const string TYPE_NIL;
      
      Record()
        : type_(TYPE_NIL)
        { }
      
      template<typename A, typename C>
      Record(Symbol typeID, A&& att, C&& chi)
        : type_(isnil(typeID)? TYPE_NIL:string(typeID))
        , attribs_(std::forward<A> (att))
        , children_(std::forward<C> (chi))
        { }
      
      template<typename A, typename C>
      Record(Symbol typeID, std::initializer_list<A> const&& att
                          , std::initializer_list<C> const&& chi)
        : type_(isnil(typeID)? TYPE_NIL:string(typeID))
        , attribs_(att)
        , children_(chi)
        { }
      
      template<typename SEQ>
      explicit
      Record (SEQ const& con)
        : type_(TYPE_NIL)
        {
          auto p = std::begin(con);
          auto e = std::end(con);
          for ( ; p!=e && isAttribute(*p); ++p)
            if (isTypeID (*p))
              type_ = extractTypeID(*p);
            else
              attribs_.push_back (*p);
          for ( ; p!=e; ++p)
            children_.push_back (*p);
        }
      
      Record (std::initializer_list<VAL> const&& ili)
        : Record(ili)
        { }
      
      // all default copy operations acceptable
      
      
      /** for diagnostic purpose: include format-util.hpp */
      operator std::string()  const;
      
      
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
          return attribs_.end() != findKey(key);
        }
      
      bool
      contains (VAL const& val)  const
        {
          return util::contains (children_, val);
        }
      
      Access
      get (string key)  const
        {
          ElmIter found = findKey (key);
          if (attribs_.end() == found)
            throw error::Invalid ("Record has no attribute \""+key+"\"");
          else
            return extractVal (*found);
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
      
      using ContentMutator = RecordContentMutator<Storage>;
      
      
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
      
      using iterator  = IterAdapter<ElmIter, const Record*>;
      using scopeIter = typename iter_stl::_SeqT<const Storage>::Range;
      using keyIter   = TransformIter<scopeIter, string>;
      using valIter   = TransformIter<scopeIter, Access>;
      
      /** default iteration exposes all data within this "object", starting with the attributes */
      iterator  begin ()  const { return iterator(this, attribs_.empty()? children_.begin() : attribs_.begin()); }
      iterator  end ()    const { return iterator(); }
      
      scopeIter attribs() const { return iter_stl::eachElm(attribs_); }
      scopeIter scope()   const { return iter_stl::eachElm(children_); }
      
      keyIter   keys()    const { return transformIterator(attribs(), extractKey); }
      valIter   vals()    const { return transformIterator(attribs(), extractVal); }
      
    protected: /* ==== API for the IterAdapter ==== */
      
      /** Implementation of Iteration-logic: pull next element. */
      friend void
      iterNext (const Record* src, ElmIter& pos)
        {
          ++pos;
          checkPoint (src,pos);
        }
      
      /** Implementation of Iteration-logic: detect iteration end.
       * @remarks seamless continuation of the iteration when reaching
       *     the end of the attribute collection. In this implementation,
       *     we use the default constructed \c ITER() to mark iteration end.
       */
      friend bool
      checkPoint (const Record* src, ElmIter& pos)
        {
          REQUIRE (src);
          static const ElmIter END;
          if (pos != END && pos == src->attribs_.end() && !src->children_.empty())
            {
              pos = src->children_.begin();
              return true;
            }
          else
            if (pos != END && (pos != src->children_.end()))
              return true;
            else
              {
                pos = END;
                return false;
        }     }
      
    private:
      /* === abstract attribute handling : needs specialisation === */
      static bool   isAttribute (VAL const& v);
      static bool   isTypeID (VAL const& v);
      static string extractTypeID (VAL const& v);
      static string renderAttribute (VAL const& a);
      static string extractKey (VAL const& v);
      static Access extractVal (VAL const& v);
      template<typename X>
      static VAL    buildAttribute (string const& key, X&& payload);
      
      
      ElmIter
      findKey (string key)  const
        {
          return std::find_if (attribs_.begin()
                              ,attribs_.end()
                              ,[=](VAL const& elm)
                                   {
                                     return key == extractKey(elm);
                                   });
        }
      
      
      friend bool
      operator== (Record const& r1, Record const& r2)
      {
        return r1.type_ == r2.type_
           and r1.attribs_ == r2.attribs_
           and r1.children_ == r2.children_;
      }
      
      friend bool
      operator!= (Record const& r1, Record const& r2)
      {
        return not (r1 == r2);
      }
    };
  
  template<typename VAL>
  const string Record<VAL>::TYPE_NIL = "NIL";
  
  
  
  template<typename VAL>
  class Record<VAL>::Mutator
    : boost::noncopyable
    {
      using Rec = Record<VAL>;
      
      Rec record_;
      
    public:
      Mutator()
        : record_()
        { }
      
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
      
      bool
      empty()  const
        {
          return record_.empty();
        }
      
      
      /* === functions to alter contents === */
      
      void
      setType (string const& newTypeID)
        {
          record_.type_ = newTypeID;
        }
      
      Mutator&
      type (string const& typeID)
        {
          setType (typeID);
          return *this;
        }
      
      template<typename X>
      Mutator&
      set (string const& key, X&& content)
        {
          VAL attribute(Rec::buildAttribute (key, std::forward<X>(content)));
          return set (std::move (attribute));
        }
      
      Mutator&
      set (VAL&& attribute)
        {
          string key = Rec::extractKey(attribute);
          if (isnil (key))
            throw error::Invalid ("Attempt to set an attribute with empty key");
          
          Rec::Storage& as =record_.attribs_;
          auto found = std::find_if (as.begin(),as.end()
                                    ,[=](VAL const& elm)
                                         {
                                           return key == extractKey(elm);
                                         });
          if (as.end() == found)
            as.push_back (std::forward<VAL> (attribute));
          else
            (*found) = (std::forward<VAL> (attribute));
          return *this;
        }
      
      Mutator&
      appendAttrib (VAL const& newAttrib)
        {
          REQUIRE (Rec::isAttribute(newAttrib));
          record_.attribs_.push_back (newAttrib);
          return *this;
        }
      
      Mutator&
      appendChild (VAL const& newChild)
        {
          record_.children_.push_back (newChild);
          return *this;
        }
      
      Mutator&
      prependChild (VAL const& newChild)
        {
          record_.children_.insert (record_.children_.begin(), newChild);
          return *this;
        }
      
      /* === low-level access (for diff application === */
      
      void
      swapContent (ContentMutator& alteredContent)
        {
          if (alteredContent.empty())
            alteredContent.preAllocateStorage(record_.attribs_.size(),
                                              record_.children_.size());
          std::swap (record_.attribs_, alteredContent.attribs);
          std::swap (record_.children_, alteredContent.children);
          alteredContent.resetPos();
        }
      
      /** get the tail element.
       * @return either the last child, or the last attribute, when children are empty.
       * @note typically this might be used to get back at the element "just added",
       *       as when muting a child node in diff application. But there is a loophole:
       *       we might have added an attribute even when there are already children.
       */
      VAL const&
      accessLast()
        {
          if (record_.empty())
            throw error::State("Record is empty, unable to access (last) element.");
          
          if (record_.children_.empty())
            return record_.attribs_.back();
          else
            return record_.children_.back();
        }
      
      
      /* === extension point for building specific value types === */
      /*
       * the following builder functions need to be specialised
       * to create a Record holding specific value types,
       * especially for building a tree like structure
       * with GenNode holding a Record<GenNode>
       */
      
      VAL genNode();
      VAL genNode(string const& symbolicID);
      
      template<typename X, typename...ARGS>
      Mutator& attrib (string const& key, X&& initialiser, ARGS&& ...args)
        {
          set (key, std::forward<X>(initialiser));
          return attrib (std::forward<ARGS>(args)...);
        }
      Mutator& attrib () { return *this; } // argument recursion end
      
      
      template<typename X, typename...ARGS>
      Mutator& scope (X const& initialiser, ARGS&& ...args)
        {
          appendChild (VAL(initialiser));
          return scope (std::forward<ARGS>(args)...);
        }
      Mutator& scope () { return *this; }
      
    };
  
  
  /** open an existing record for modification in-place.
   * @warning this function undermines the immutability of Record
   * @remarks exploits the specific and known memory layout of Rec::Mutator.
   *          This trickery is necessary to avoid copying a large and possibly
   *          nested object tree; this happens when applying a diff, when
   *          recursively descending into nested child objects.
   * @todo do we have a design mismatch here??      /////////////////////////////////////////TICKET #970
   * @see tree-diff-application.hpp
   */
  template<typename VAL>
  inline typename Record<VAL>::Mutator&
  mutateInPlace (Record<VAL>& record_to_mutate)
  {
    return reinterpret_cast<typename Record<VAL>::Mutator &> (record_to_mutate);
  }
  
  
  /**
   * wrapped record reference.
   * A helper for lib::GenNode and the diff representation.
   * RecordRef is copyable and movable, but like a reference
   * can not be rebound. It can be used to refer to a subtree
   * within the diff representation, without the need to copy.
   * @remarks this is almost identical to std::ref, with the
   *          notable difference that it can be default-created
   *          into "bottom" state; this also implies to have
   *          a NULL check on dereferentiation.
   */
  template<typename VAL>
  class RecordRef
    {
      using Target = Record<VAL>;
      
      Target* record_;
      
    public:
      /** by default create an
       *  invalid ("bottom") reference */
      RecordRef()  noexcept
        : record_(nullptr)
        { }
      
      /** create a reference bound to
       *  the given target; can not be rebound */
      RecordRef(Target& o)  noexcept
        : record_(&o)
        { }
      
      /** prevent moving into black hole */
      RecordRef(Target&&) = delete;
      
      RecordRef(RecordRef const&)  = default;
      RecordRef(RecordRef     &&)  = default;
      
      /** references can not be reassigned */
      RecordRef& operator= (RecordRef const&)  = delete;
      RecordRef& operator= (RecordRef      &)  = delete;
      
      /** assignment is not allowed, but moving is */
      RecordRef&
      operator= (RecordRef &&o)
        {
          std::swap(record_, o.record_);
          return *this;
        }
      
      
      explicit
      operator bool()  const
        {
          return bool(record_);
        }
      
      bool
      empty()  const
        {
          return not record_;
        }
      
      /** target is accessed by cast
       * @throws error::Logic on bottom reference
       */
      operator Target&()  const
        {
          if (!record_)
            throw error::Logic("attempt to dereference an unbound record reference"
                              ,error::LUMIERA_ERROR_BOTTOM_VALUE);
          return *record_;
        }
      
      Target*
      get()  const noexcept
        {
          return record_;
        }
      
      /** @note equality of references (instance pointers), not targets */
      friend bool
      operator== (RecordRef const& r1, RecordRef const& r2)
      {
        return r1.record_ == r2.record_;
      }
      friend bool
      operator!= (RecordRef const& r1, RecordRef const& r2)
      {
        return r1.record_ != r2.record_;
      }
    };
  
  
  
  
  /* === Extension point: Specialisations for attribute handling === */
  
  /**
   * Type configuration (extension point).
   * Data storage and access types.
   */
  template<>
  struct RecordSetup<string>
    {
      using Storage = std::vector<string>;
      using ElmIter = typename Storage::const_iterator;
      using Access  = string;      ///< data access by value copy
    };
  
  
  
  
  /* default handling defined for Record<string> */
  
  template<>
  inline string
  Record<string>::extractKey (string const& v)
  {
    size_t pos = v.find('=');
    if (string::npos == pos)
      return "";
    else
      return util::trim (v.substr (0,pos));
  }
  
  template<>
  inline string
  Record<string>::extractVal (string const& v)
  {
    size_t pos = v.find('=');
    if (string::npos == pos)
      return v;
    else
      return util::trim (v.substr (pos+1, v.length() - pos));
  }
  
  template<>
  inline bool
  Record<string>::isAttribute (string const& v)
  {
    return string::npos != v.find('=');
  }
  
  template<>
  inline bool
  Record<string>::isTypeID (string const& v)
  {
    return isAttribute(v)
        && "type" == extractKey(v);
  }
  
  template<>
  inline string
  Record<string>::extractTypeID (string const& v)
  {
    return extractVal(v);
  }
  
  template<>
  inline string
  Record<string>::renderAttribute (string const& attrib)
  {
    return extractKey(attrib) + " = " + extractVal(attrib);
  }
  
  template<>
  template<typename X>
  inline string
  Record<string>::buildAttribute (string const& key, X&& payload)
  {
    return string(key + " = " + extractVal(payload));
  }
  
  
  
  
  
  
  /* === Diagnostics === */
  
  template<typename VAL>
  Record<VAL>::operator std::string()  const
  {
#ifndef LIB_FORMAT_UTIL_H
    return "Record(...)";
#else
    using util::join;
    using lib::transformIterator;
    
    return "Rec("
         + (TYPE_NIL==type_? "" : type_+"| ")
         + join (transformIterator (this->attribs(), renderAttribute))
         + " |{"
         + join (this->scope())
         + "})"
         ;
#endif
  }
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_GEN_NODE_H*/
