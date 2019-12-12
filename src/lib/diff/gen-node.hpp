/*
  GEN-NODE.hpp  -  generic node element for tree like data representation

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


/** @file gen-node.hpp
 ** Generic building block for tree shaped (meta)data structures.
 ** A representation built from GenNode elements is intended to support
 ** (limited) introspection of data structures and exchange of mutations
 ** in the form of [diff messages](\ref diff-language.hpp).
 ** 
 ** Despite of the name, GenNode is \em not meant to be an universal
 ** data representation; rather it is limited to embody a fixed hard
 ** wired set of data types, able to stand-in for attributes
 ** and sub scope contents of the lumiera high-level data model.
 ** 
 ** # Anatomy of a GenNode
 ** 
 ** GenNode is a polymorphic value with well defined identity and type.
 ** Each element is conceived to be »unique within context« -- as defined
 ** by the immediately visible scope within a tree like structure.
 ** Beyond this identity metadata, each GenNode carries a DataCap, which
 ** is an inline container and attachment point for payload data. Simple
 ** attribute values can be carried alongside, while more complex types
 ** or entities bound to a reference and registration system (e.g. Placement)
 ** will be referred by a suitable reference representation (PlacementID).
 ** The DataCap is what creates the polymorphic nature, where the common
 ** interface is mostly limited to managemental tasks (copying of values,
 ** external representation).
 ** 
 ** To represent object-like structures and for building trees, a special
 ** kind of data type is placed into the DataCap. This type, Record<GenNode>
 ** is recursive and has the ability to hold both a a set of attributes
 ** addressable by-name and an (ordered) collection of elements treated
 ** as children within the scope of the given record.
 ** 
 ** ## The GenNode ID
 ** 
 ** Each GenNode holds an ID tag, allowing to establish _identical_ and _distinct_
 ** elements within a scope. This ID is based on lib::idi::BareEntryID, thereby
 ** providing a human readable symbolic part, and a hash value. By default, these
 ** GenNode IDs are fabricated such as to hold a non-reproducible, random hash
 ** value -- however, there are construction flavours allowing to pass in an
 ** pre-existing distinct Entry-ID.
 ** 
 ** 
 ** # Requirements
 ** 
 ** GenNode elements are to be used in the diff detection and implementation.
 ** This implies some requirements for the (opaque) elements used in diff:
 ** - they need to support the notion of equality
 ** - we need to derive a key type for usage in index tables
 **   - this implies the necessity to support std::less comparisons for tree-maps
 **   - and the necessity to support hash code generation for unordered (hash)maps
 ** - moreover, the elements need to be values, able to be copied and handled at will
 ** - it will be beneficial for these values to support move semantics explicitly
 ** - in addition, the tree diffing suggests a mechanism to re-gain the fully
 **   typed context, either based on some kind of embedded type tag, or
 **   alternatively by visitation and matching
 ** - finally, the handling of changes prompts us to support installation
 **   of a specifically typed _change handling closure_.
 ** 
 ** ## monadic nature?
 ** 
 ** As suggested by the usage for representation of tree shaped data, we acknowledge
 ** that GenNode could be a Monad. We support the basic operation \em construction,
 ** and the operation \em flatMap would be trivial to add. To fit in with this generic
 ** processing pattern, the one element flavours of GenNode are considered the special case,
 ** while the collective flavours form the base case -- every GenNode can be iterated.
 ** The \em construction requirement suggests that GenNode may be created readily, just
 ** by wrapping any given and suitable element, thereby picking up the element's type.
 ** 
 ** But the purpose and goal of the monadic approach is not clear yet (5/2015).
 ** To begin with, for the task of diff detection and application, it is sufficient
 ** to get the children as traversable collection and to offer a depth-first expansion.
 ** 
 ** @see GenNode_test
 ** @see diff-list-generation-test.cpp
 ** @see DiffDetector
 ** 
 */


#ifndef LIB_DIFF_GEN_NODE_H
#define LIB_DIFF_GEN_NODE_H


#include "lib/error.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/diff/record.hpp"
#include "lib/variant.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>
#include <deque>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
  using std::string;
  
  struct GenNode;
  struct Ref;
  
  /** Define actual data storage and access types used */
  template<>
  struct RecordSetup<GenNode>
    {
      using Storage = std::vector<GenNode>;
      using ElmIter = typename Storage::const_iterator;
      
      /** using const reference data access
       *  relevant for handling large subtrees */
      using Access  = GenNode const&;
    };
  
  
  
  
  using Rec = Record<GenNode>;
  using RecRef = RecordRef<GenNode>;
  using MakeRec = Rec::Mutator;
  using DataValues = meta::Types<int
                                ,int64_t
                                ,short
                                ,char
                                ,bool
                                ,double
                                ,string
                                ,time::Time
                                ,time::Offset
                                ,time::Duration
                                ,time::TimeSpan
                                ,hash::LuidH
                                ,RecRef
                                ,Rec
                                >;
  
  
  
  class DataCap
    : public Variant<DataValues>
    {
    public:
      template<typename X>
      DataCap(X&& x)
        : Variant<DataValues>(std::forward<X>(x))
        { }
      
                                       ////////////////////////TICKET #963  Forwarding shadows copy operations -- generic solution??
      DataCap(DataCap const&) =default;
      DataCap(DataCap&&)      =default;
      DataCap(DataCap& o)
        : DataCap((DataCap const&)o)
        { }
      
      DataCap& operator= (DataCap const&)  =default;
      DataCap& operator= (DataCap&&)       =default;
      
      bool matchData (DataCap const&)  const;
      bool matchNum  (int64_t)         const;
      bool matchTxt  (string const&)   const;
      bool matchTime (time::TimeValue) const;
      bool matchBool (bool)            const;
      bool matchDbl  (double)          const;
      bool matchLuid (hash::LuidH)     const;
      bool matchRec  (RecRef const&)   const;
      bool matchRec  (Rec const&)      const;
      
      struct Locator;
      Locator expand()  const;
      
      operator string() const;
      
      template<typename X>
      X& get();
      template<typename X>
      X const& get()  const;
      
      /** determine if payload constitutes a nested scope ("object") */
      bool isNested()  const;
      
      /** peek into the type field of a nested `Record<GenNode>` */
      string recordType()  const;
      
      /** visit _children_ of a nested `Record<GenNode>` */
      Rec::scopeIter
      childIter()  const
        {
          const Rec* rec = unConst(this)->maybeGet<Rec>();
          if (!rec)
            return Rec::scopeIter();
          else
            return rec->scope();
        }
    };
  
  
  /** generic data element node within a tree */
  struct GenNode
    {
      class ID
        : public idi::BareEntryID
        {
          friend struct GenNode;
          
          template<typename X>
          ID (X*, string const& symbolicID)
            : idi::BareEntryID (symbolicID,
                                idi::getTypeHash<X>())
            { }
          
          ID (idi::BareEntryID&& rawD)
            : idi::BareEntryID{move (rawD)}
            { }
          
        public:
          explicit
          ID (GenNode const& node)
            : ID(node.idi)
            { }
          
          // standard copy operations acceptable
          
          operator string()  const
            {
              return "ID(\""+getSym()+"\")";
            }
        };
      
      
      //------GenNode Data fields---
      
      ID      idi;
      DataCap data;
      
      
      template<typename X>
      GenNode(X&& val)
        : idi(&val, buildChildID<X>())
        , data(std::forward<X>(val))
        { }
      
      template<typename X>
      GenNode(string const& symbolicID, X&& val)
        : idi(&val, symbolicID)
        , data(std::forward<X>(val))
        { }
      
      GenNode(string const& symbolicID, const char* text)
        : GenNode(symbolicID, string(text))
        { }
      
      GenNode(const char* text)
        : GenNode(string(text))
        { }
      
                                       ////////////////////////TICKET #963  Forwarding shadows copy operations -- generic solution??
      GenNode(GenNode const&)  =default;
      GenNode(GenNode&&)       =default;
      GenNode(GenNode& o)  : GenNode((GenNode const&)o) { }
      GenNode(Ref const& r);
      GenNode(Ref &  r);
      GenNode(Ref && r);
      
      /** copy assignment
       * @remarks we need to define our own version here for sake of sanity.
       *    The reason is that we use inline storage (embedded within lib::Variant)
       *    and that we deliberately _erase_ the actual type of data stored inline.
       *    Because we still do want copy assignment, in case the payload data
       *    supports this, we use a "virtual copy operator", where in the end
       *    the storage buffer within lib::Variant has to decide if assignment
       *    is possible. Only data with the same type may be assigned and we
       *    prevent change of the (implicit) data type through assignment.
       *    This check might throw, and for that reason we're better off
       *    to perform the _data assignment_ first. The probability for
       *    EntryID assignment to fail is low (but it may happen!).
       * @note the use of inline storage turns swapping of data
       *    into an expensive operation, involving a temporary.
       *    This rules out the copy-and-swap idiom.
       */
      GenNode&
      operator= (GenNode const& o)
        {
          if (&o != this)
            {
              data = o.data;
              idi = o.idi;
            }
          return *this;
        }
      
      GenNode&
      operator= (GenNode&& o)
        {
          ASSERT (&o != this);
          data = std::forward<DataCap>(o.data);
          idi  = std::forward<ID>(o.idi);
          return *this;
        }
      
      //note: NOT defining a swap operation, because swapping inline storage is pointless!
      
      
      
      
      /** @internal diagnostics helper */
      operator string()  const
        {
          return "GenNode-"+string(idi)+"-"+string(data);
        }
      
      bool
      isNamed()  const
        {
          return not util::startsWith (idi.getSym(), "_CHILD_");
        }
      
      bool
      isTypeID()  const
        {
          return "type" == idi.getSym();
        }
      
      template<typename X>
      bool contains (X const& elm)  const;
      
      
      bool matches (GenNode const& o)  const { return this->matches(o.idi); }  ///< @note _not_ comparing payload data. Use equality for that…
      bool matches (ID const& id)      const { return idi == id;            }
      bool matches (int     number)    const { return data.matchNum(number);}
      bool matches (int64_t number)    const { return data.matchNum(number);}
      bool matches (short   number)    const { return data.matchNum(number);}
      bool matches (char    number)    const { return data.matchNum(number);}
      bool matches (double  number)    const { return data.matchDbl(number);}
      bool matches (string text)       const { return data.matchTxt(text);}
      bool matches (const char* text)  const { return data.matchTxt(text);}
      bool matches (time::TimeValue t) const { return data.matchTime(t);  }
      bool matches (bool b)            const { return data.matchBool(b);  }
      bool matches (hash::LuidH h)     const { return data.matchLuid(h);  }
      bool matches (RecRef const& ref) const { return data.matchRec(ref); }
      bool matches (Rec const& rec)    const { return data.matchRec(rec); }
      
      class ScopeExplorer;
      
      struct ScopeExplorerIterator;
      using iterator = ScopeExplorerIterator;
      
      iterator begin()      ;
      iterator begin() const;
      iterator end()        ;
      iterator end()   const;
      
      
      using ChildDataIter = TransformIter<Rec::scopeIter, DataCap const&>;
      
      /** visit the _data_ of nested child elements
       * @return an iterator over the DataCap elements of all children,
       *         in case this GenNode actually holds a Record.
       *         Otherwise an empty iterator.
       * @note this iterator visits _only_ the children, which are
       *         by definition unnamed. It does _not_ visit attributes.
       */
      friend ChildDataIter
      childData (GenNode const& n)
      {
        return ChildDataIter{ n.data.childIter()
                            , [](GenNode const& child) ->DataCap const&
                                {
                                  return child.data;
                                }
                            };
      }
      
      friend ChildDataIter
      childData (Rec::scopeIter&& scopeIter)
      {
        return ChildDataIter{ std::forward<Rec::scopeIter>(scopeIter)
                            , [](GenNode const& child) ->DataCap const&
                                {
                                  return child.data;
                                }
                            };
      }
      
      
      friend string
      name (GenNode const& node)
      {
        return node.idi.getSym();
      }
      
      friend bool
      operator== (GenNode const& n1, GenNode const& n2)
      {
        return n1.idi == n2.idi
            && n1.data.matchData(n2.data);
      }
      
      friend bool
      operator!= (GenNode const& n1, GenNode const& n2)
      {
        return not (n1 == n2);
      }
      
      /**
       * allow for storage in ordered containers, ordering
       * based on the human-readable ID within the GenNode.
       * @warning this constitutes a _weaker equivalence_ than
       *       given by the equality comparison (`operator==`),
       *       since GenNode::ID is an EntryID, which also includes
       *       the type parameter into the identity (hash). This means,
       *       two GenNodes with different real payload type but same
       *       ID symbol will not be equal, but be deemed equivalent
       *       by this IDComparator. This can be dangerous when building
       *       a set or map based on this comparator.
       */
      struct IDComparator
        {
          bool
          operator() (GenNode const& left, GenNode const& right)  const
            {
              return left.idi.getSym() < right.idi.getSym();
            }
        };
      
      /** fabricate a GenNode with the literally given ID */
      template<typename X>
      static GenNode
      asAttribute (idi::BareEntryID && rawID, X&& payload)
        {
          return GenNode{ID{move (rawID)}, DataCap{forward<X> (payload)}};
        }
      
      
    protected:
      /** @internal for dedicated builder subclasses */
      GenNode (ID&& id, DataCap&& d)
        : idi(std::move(id))
        , data(std::move(d))
        { }
      
      template<typename X>
      static GenNode::ID
      fabricateRefID (string const& symbolicID)
        {
          X* typeID(0);
          return ID(typeID, symbolicID);
        }
      
    private:
      template<typename X>
      static string
      buildChildID()
        {
          return "_CHILD_" + idi::generateSymbolicID<X>();
        }
    };
  
  
  
  /**
   * metafunction to detect types able to be wrapped into a GenNode.
   * Only a limited and fixed set of types may be placed within a GenNode,
   * as defined through the typelist `lib::diff::DataValues`. This metafunction
   * allows to enable or disable specialisations and definitions based on the
   * fact if a type in question can live within a GenNode.
   */
  template<typename ELM>
  struct can_wrap_in_GenNode
    {
      using Yes = lib::meta::Yes_t;
      using No  = lib::meta::No_t;
      
      template<class X>
      static Yes check(typename variant::CanBuildFrom<X, DataValues>::Type*);
      template<class X>
      static No  check(...);
      
    public:
      static const bool value = (sizeof(Yes)==sizeof(check<ELM>(0)));
    };
  
  
  
  
  /* === iteration / recursive expansion === */
  
  
  /**
   * @internal Helper to refer to any element position,
   * irrespective if on top level or within a nested scope
   * @remarks typically used within lib::IterStateWrapper
   * @see DataCap#expand()
   */
  struct DataCap::Locator
    {
      const GenNode* node_;
      Rec::iterator scope_;
      
      Locator()
        : node_(nullptr)
        { }
      
      Locator(GenNode const& n)
        : node_(&n)
        { }
      
      Locator(Rec const& r)
        : node_(nullptr)
        , scope_(r.begin())
        { }
      
      const GenNode *
      get()  const
        {
          return node_? node_
                      : scope_? scope_.operator->()
                              : nullptr;
        }
      
      /* === Iteration control API for IterStateWrapper == */
      
      bool
      checkPoint()  const
        {
          return this->get();
        }
      
      GenNode const&
      yield()  const
        {
          return *get();
        }
      
      void
      iterNext()
        {
          if (node_)
            node_ = nullptr;
          else
            ++scope_;
        }
    };
  
  
  /**
   * Building block for monad-like depth-first expansion of a GenNode.
   * When used within lib::IterStateWrapper, the result is an Iterator
   * to visit the contents of a GenNodet tree recursively depth-fist.
   */
  class GenNode::ScopeExplorer
    {
      using ScopeIter = IterStateWrapper<const GenNode, DataCap::Locator>;
      
      std::deque<ScopeIter> scopes_;
      
    public:
      ScopeExplorer() { }
      ScopeExplorer(GenNode const& n)
        {
          scopes_.emplace_back(n);
        }
      
      size_t
      depth()  const
        {
          return scopes_.size();
        }
      
      /* === Iteration control API for IterStateWrapper == */
      
      bool
      checkPoint()  const
        {
          return not  scopes_.empty()
             and bool(scopes_.back());
        }
      
      GenNode const&
      yield()  const
        {
          return *(scopes_.back());
        }
      
      void
      iterNext()
        {
          ScopeIter& current = scopes_.back();
          scopes_.emplace_back (current->data.expand());
          ++current;
          while (not scopes_.empty() and not scopes_.back())
            scopes_.pop_back();
        }
      
      friend bool
      operator== (ScopeExplorer const& s1, ScopeExplorer const& s2)
      {
        return not s1.scopes_.empty()
            && not s2.scopes_.empty()
            && s1.scopes_.size() == s2.scopes_.size()
            && s1.yield() == s2.yield();
      }
    };
  
  
  /** @internal Core operation to expand nested scopes recursively */
  inline DataCap::Locator
  DataCap::expand()  const
    {
      Rec* val = unConst(this)->maybeGet<Rec>();
      if (!val)
        return Locator();
      else
        return Locator(*val);
    }
  
  
  struct GenNode::ScopeExplorerIterator
    : IterStateWrapper<const GenNode, ScopeExplorer>
    {
      using IterStateWrapper<const GenNode, ScopeExplorer>::IterStateWrapper;
      
      size_t level()  const { return unConst(this)->stateCore().depth(); }
    };
  
  
  inline GenNode::iterator GenNode::begin()        { return iterator(*this); }
  inline GenNode::iterator GenNode::begin() const  { return iterator(*this); }
  inline GenNode::iterator GenNode::end()          { return iterator(); }
  inline GenNode::iterator GenNode::end()   const  { return iterator(); }
  
  template<typename X>
  inline bool
  GenNode::contains (X const& elm)  const
  {
    for (auto & n : *this)
      if (n.matches(elm))
        return true;
    return false;
  }
  
  
  
  
  /* === References : special treatment on element access === */
  
  template<typename X>
  inline X&
  DataCap::get()
    {
      return Variant<DataValues>::get<X>();
    }
  
  template<typename X>
  inline X const&
  DataCap::get()  const
    {
      return Variant<DataValues>::get<X>();
    }
  
  /** especially when accessing for a Record,
   * a payload of type \c RecordRef<Record<GenNode>> (aka RecRef)
   * will be automatically dereferenced. Effectively this allows
   * a GenNode with a RecRef payload to "stand in" for a node holding
   * a full Record inline. And it allows the construction of a special
   * \link #Ref Ref-GenNode \endlink, which even shares the \em identity
   * (the ID) of the referenced record-GenNode.
   * @note effectively this opens an indirect loophole to const correctness,
   *       since it is possible explicitly to retrieve the RecRef from a
   *       const GenNode and then to access the referred-to Record
   *       without const. In case this turns out to be problematic,
   *       we'd have to alter the semantics of RecRef
   */
  template<>
  inline Rec&
  DataCap::get()
    {
      Rec* rec = maybeGet<Rec>();
      if (rec) return *rec;
      
      return Variant<DataValues>::get<RecRef>();
    }
  
  template<>
  inline Rec const&
  DataCap::get()  const
    {
      Rec* rec = unConst(this)->maybeGet<Rec>();
      if (rec) return *rec;
      
      return Variant<DataValues>::get<RecRef>();
    }
  
  /**
   * @return either the contents of a nested record's type field
   *         or the util::BOTTOM_INDICATOR, when not a record.
   * @remarks this function never raises an error, even if the element
   *         in fact doesn't constitute a nested scope. Effectively this
   *         allows to "peek" into the contents to some degree.
   */
  inline string
  DataCap::recordType()  const
    {
      Rec* nested = unConst(this)->maybeGet<Rec>();
      if (!nested)
        {
          RecRef* ref = unConst(this)->maybeGet<RecRef>();
          if (ref and not ref->empty())
            nested = ref->get();
        }
      
      return nested? nested->getType()
                   : util::BOTTOM_INDICATOR;
    }
  
  inline bool
  DataCap::isNested()  const
    {
      return util::BOTTOM_INDICATOR != recordType();
    }
  
  
  
  
  /**
   * Constructor for a specially crafted 'ref GenNode'.
   * The identity record of the generated object will be prepared
   * such as to be identical to a regular GenNode with Record payload.
   * @note slicing in usage is intentional
   */
  struct Ref
    : GenNode
    {
      /** create an empty ID stand-in.
       * @note the purpose is to create a symbolic reference by name
       */
      explicit
      Ref(string const& symbolicID)
        : GenNode(fabricateRefID<Rec> (symbolicID)//note: seeds the type hash with Rec, not RecRef
                 , DataCap(RecRef()))            // note: places NIL into the reference part
        { }
      
      /** build reference to a Record, using the original ID
       * @throw error::Logic when oNode does not hold a Record<GenNode>
       */
      Ref(GenNode& oNode)
        : GenNode(ID(oNode)
                 , DataCap(RecRef(oNode.data.get<Rec>())))
        { }
      
      static const Ref I;       ///< symbolic ID ref "_I_"
      static const Ref NO;      ///< symbolic ID ref "_NO_"
      static const Ref END;     ///< symbolic ID ref "_END_"
      static const Ref THIS;    ///< symbolic ID ref "_THIS_"
      static const Ref CHILD;   ///< symbolic ID ref "_CHILD_"
      static const Ref ATTRIBS; ///< symbolic ID ref "_ATTRIBS_"
    };
  
  
  // slice down on copy construction...
  inline GenNode::GenNode(Ref const& r)  : idi(r.idi), data(r.data)   { }
  inline GenNode::GenNode(Ref &  r)      : idi(r.idi), data(r.data)   { }
  inline GenNode::GenNode(Ref && r)      : idi(std::move(r.idi)),
                                           data(std::move(r.data))    { }

  
  /* === Specialisation to add fluent GenNode builder API to Record<GenNode> === */
  
  template<>
  inline GenNode
  MakeRec::genNode()
  {
    return GenNode{std::move(record_)};
  }
  
  template<>
  inline GenNode
  MakeRec::genNode (idi::BareEntryID rawID)
  {
    return GenNode::asAttribute (std::move(rawID), std::move(record_));
  }
  
  template<>
  inline GenNode
  MakeRec::genNode (string const& symbolicID)
  {
    return GenNode{symbolicID, std::move(record_)};
  }
  
  
  /* === Extension point to apply a tree-diff === */
  
  /** implementation is provided by the "diff framework"
   * @see tree-mutator-gen-node-binding.hpp
   * @see tree-diff.cpp (implementation)
   */
  template<>
  void MakeRec::buildMutator (BufferHandle buff);
  
  
  
  /* === Specialisation for handling of attributes in Record<GenNode> === */
  
  template<>
  inline bool
  Rec::isAttribute (GenNode const& attrib)
  {
    return attrib.isNamed();
  }
  
  template<>
  inline bool
  Rec::isTypeID (GenNode const& attrib)
  {
    return attrib.isTypeID();
  }
  
  template<>
  inline string
  Rec::extractTypeID (GenNode const& v)
  {
    return isTypeID(v)? v.data.get<string>()
                      : Rec::TYPE_NIL;
  }
  
  template<>
  inline string
  Rec::extractKey (GenNode const& v)
  {
    return isAttribute(v)? v.idi.getSym()
                         : "";
  }
  
  template<>
  inline GenNode const&
  Rec::extractVal (GenNode const& v)
  {
    return v;
  }
  
  template<>
  inline string
  Rec::renderAttribute (GenNode const& a)
  {
    return a.idi.getSym() +" = "+ string(a.data);
  }
  
  template<>
  template<typename X>
  inline GenNode
  Rec::buildAttribute (string const& key, X&& payload)
  {
    return GenNode{key, forward<X>(payload)};
  }
  
  
  
}// namespace lib::diff

namespace variant {
    using diff::Rec;
    
    /**
     * specialisation allowing to build a GenNode with a
     * nested record from a Record::Mutator (aka `MakeRec`)
     */
    template<typename TYPES>
    struct CanBuildFrom<diff::MakeRec, Node<Rec, TYPES>>
      : std::true_type
      {
        using Type = Rec;
      };

}} // namespace lib::variant
#endif /*LIB_DIFF_GEN_NODE_H*/
