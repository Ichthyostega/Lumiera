/*
  HierarchyOrientationIndicator(Test)  -  verify generation details
  
  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>
  
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
#include "lib/util.hpp"
//#include "lib/util-foreach.hpp"

#include "lib/hierarchy-orientation-indicator.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/itertools.hpp"

//#include <boost/lexical_cast.hpp>
#include <boost/operators.hpp>
#include <iostream>  //////////////////////////////TODO
#include <tr1/functional>
#include <string>
#include <vector>
#include <cstdlib>

//using boost::lexical_cast;
using util::contains;
using std::string;
using util::isnil;
using std::cout;
using std::endl;

namespace lib {
namespace test {
  
  namespace { // test fixture: a random Tree to navigate...
    
    namespace error=lumiera::error;
    
    using std::rand;
//  using std::vector;
    using std::tr1::ref;
    using std::tr1::function;
    using lib::IterStateWrapper;
    using lib::transformIterator;
    
    const uint MAX_ID(100);
    const uint MAX_CHILDREN(5);
    const double CHILD_PROBABILITY(0.06);
    const uint CHILDREN_TOTAL_LIMIT(20);
    
    const uint CHILDREN_SEED(20);
    uint random_children_created(0);
    
    
    /**
     * pick a random child count below #MAX_CHILDREN
     * with a probability to get any count above zero
     * as defined by CHILD_PROBABILITY
     */
    inline uint
    pick_random_count()
    {
      uint bottom((1.0/CHILD_PROBABILITY - 1) * MAX_CHILDREN);
      uint limit = bottom + MAX_CHILDREN;
      ASSERT (0 < bottom);
      ASSERT (bottom < limit);
      
      ++random_children_created;
      int cnt = (rand() % limit) - bottom;
//      if (random_children_created > CHILDREN_TOTAL_LIMIT) cnt=0;
      if (0 < MAX (0, cnt)) cout << "Kau: "<< cnt <<endl;
      return MAX (0, cnt); 
    }
    
    
    struct Node
      : boost::equality_comparable<Node>
      {
        typedef std::vector<Node> Children;
        typedef RangeIter<Children::iterator> ChildSeq;
        
        int id_;
        Children children_;
        
        Node(int i)
          : id_(i)
          { }
        
        Node()
          : id_(rand() % MAX_ID)
          {
            uint c = pick_random_count();
            for (uint j=0; j<c; ++j)  // populate with c random children
              children_.push_back(Node());
          }
        
        Node const&
        child (uint i)  const
          {
            REQUIRE (i < children_.size());
            return children_[i];
          }
        
        ChildSeq
        childSequence()
          {
            return ChildSeq (children_.begin(), children_.end());
          }
        
        bool
        hasChild (Node const& o)
          {
            return util::contains (children_, o);
          }
        
        Node&
        makeChild (int childID)
          {
            children_.push_back (Node(childID));
            return children_.back();
          }
      };
    
    
    inline bool
    have_equivalent_children (Node const& l, Node const& r)
    {
      if (l.children_.size() != r.children_.size()) return false;
      for (uint i=0; i<l.children_.size(); ++i)
        if (l.child(i) != r.child(i)) return false;
      return true; 
    }
    
    inline bool
    operator== (Node const& l, Node const& r)
    {
      return l.id_ == r.id_
          && have_equivalent_children(l,r);
    }
    
    
    typedef lib::IterQueue<Node*> NodeSeq;
    
    
    /**
     * Function to generate a depth-first tree visitation
     */
    NodeSeq
    exploreChildren (Node* ref)
    {
      Node& node(*ref);
      NodeSeq children_to_visit;
      build(children_to_visit).usingSequence (AddressExposingIter<Node::ChildSeq>(node.childSequence()));
      return children_to_visit;
    }
    
    
    
    struct VisitationData
      {
        int id;
        int orientation;
        
        VisitationData(int refID,
                       int direction =0)
          : id(refID)
          , orientation(direction)
          { }
      };
    
    /**
     * This functor visits the nodes to produce the actual test data.
     * The intention is to describe a visitation path through a tree structure
     * by a sequence of "up", "down", and "level" orientations. The test we're
     * preparing here will attempt to re-create a given tree based on these
     * directional information. The actual visitation path is created by
     * a depth-first exploration of the source tree.
     */
    class NodeVisitor
      {
        typedef std::deque<Node*> NodePath;
        typedef NodePath::reverse_iterator PathIter;
        
        NodePath path_;
        
      public:
        // using default ctor and copy operations
        
        VisitationData
        operator() (Node* node)
          {
            int direction = establishRelation (node);
            return VisitationData(node->id_, direction);
          }
        
      private:
        /** Helper for this test only: find out about the hierarchical relation.
         *  In the real usage situation, the key point is that we \em record
         *  this relation on-the-fly, when visiting the tree, instead of
         *  determining it after the fact. */
        int
        establishRelation (Node* nextNode)
          {
            REQUIRE (nextNode);
            uint level = path_.size();
            uint refLevel = level;
            for (PathIter p = path_.rbegin();
                 0 < level ; --level, ++p )
              {
                Node* parent = *p;
                if (parent->hasChild (*nextNode))
                  {
                    // visitation continues with children below this level
                    path_.resize(level);
                    path_.push_back(nextNode);
                    return (level - refLevel) + 1;
                  }
              }
            ASSERT (0 == level);
            
            // nextNode not found as child (i.e. fork) within current tree path
            // --> start new tree path at root
            path_.clear();
            path_.push_back(nextNode);
            return 0; // by convention, here the root is an implicitly pre-existing context
          }
      };
    
    
    
    struct TreeRebuilder
      : Node
      {
        template<class IT>
        TreeRebuilder (IT treeTraversal)
          : Node(0)
          {
            populate (transformIterator (treeTraversal, 
                                         function<VisitationData(Node*)>(NodeVisitor())));
          }
        
      private:
        template<class IT>
        void
        populate (IT treeVisitation)
          {
                struct Builder
                  {
                    Builder (Node& startPoint)
                      : parent(&startPoint)
                      , current(0)
                      { }
                    
                    void
                    populateBy (IT& treeVisitation)
                      {
                        while (treeVisitation)
                          {
                            int direction = treeVisitation->orientation; 
                            if (direction < 0)
                              {
                                treeVisitation->orientation += 1;
                                return;
                              }
                            else
                            if (direction > 0)
                              {
                                treeVisitation->orientation -= 1;
                                Node& refPoint = startChildTransaction();
                                populateBy (treeVisitation);
                                commitChildTransaction(refPoint);
                              }
                            else
                              {
                                addNode (treeVisitation->id);
                                ++treeVisitation;
                              }}}
                      
                  private:
                    Node* parent;
                    Node* current;
                    
                    void
                    addNode (int id)
                      {
                        current = & parent->makeChild(id);
                      }
                    
                    Node&
                    startChildTransaction()
                      {
                        Node& oldRefPoint (*parent);
                        ASSERT (current);
                        parent = current;  // set new ref point
                        return oldRefPoint;
                      }
                    
                    void
                    commitChildTransaction(Node& refPoint)
                      {
                        parent = &refPoint;
                        current = parent;
                      }
                  };
            
            
            Builder builder(*this); // pre-existing implicit root context
            builder.populateBy (treeVisitation);
          }
        
      };
    
  
  } //(End) test fixture
  
  
  
  
  
  /***************************************************************************
   * @test cover various detail aspects regarding
   *       - weakness of 
   *       
   * @see HashIndexed_test
   * @see HierarchyOrientationIndicator
   */
  class HierarchyOrientationIndicator_test : public Test
    {
      
      virtual void run (Arg)
        {
          demonstrate_tree_rebuilding ();
        }
      
      /** @test demonstrate a serious weakness of
       * When...
       * 
       * This problem is especially dangerous when...
       */
      void demonstrate_tree_rebuilding ( )
        {
          Node::Children testWood;
          for (uint i=0; i < CHILDREN_SEED; ++i)
            testWood.push_back(Node());
          
          using iter_stl::eachElm;
          using lib::AddressExposingIter;

          
          TreeRebuilder reconstructed (depthFirst (AddressExposingIter<Node::ChildSeq>(eachElm (testWood))) >>= exploreChildren);
          
          cout << reconstructed.children_.size() << "=?=" << testWood.size();
          CHECK (reconstructed.children_ == testWood);
        }
      
    };
  
  /** Register this test class... */
  LAUNCHER(HierarchyOrientationIndicator_test, "unit common");
  
}} // namespace lib
