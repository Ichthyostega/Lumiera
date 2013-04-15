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

#include "lib/hierarchy-orientation-indicator.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include <boost/operators.hpp>
#include <tr1/functional>
#include <string>
#include <vector>
#include <cstdlib>


namespace lib {
namespace test {
  
  namespace { // test fixture: a random Tree to navigate...
    
    using std::rand;
    using std::tr1::function;
    using lib::transformIterator;
    using lib::iter_stl::eachAddress;
    using util::contains;
    using util::max;
    
    /* -- size of the test tree ---- */
    const uint MAX_CHILDREN_CNT(5);            // children per Node (5 means 0 to 4 children)
    const double CHILD_PROBABILITY(0.45);      // probability for a Node to have any children
    const uint TEST_SEQUENCE_LENGTH(50);       // test uses a sequence of Node trees
                                               // 5 - 45% - 50  produce roughly 1000 Nodes and tree depths of about 12
    uint nextChildID(1);
    
    
    /**
     * pick a random child count below #MAX_CHILDREN_CNT
     * with a probability to get any count above zero
     * as defined by CHILD_PROBABILITY
     */
    inline uint
    pick_random_count()
    {
      uint bottom((1.0/CHILD_PROBABILITY - 1) * MAX_CHILDREN_CNT);
      uint limit = bottom + MAX_CHILDREN_CNT;
      ASSERT (0 < bottom);
      ASSERT (bottom < limit);
      
      int cnt = (rand() % limit) - bottom;
      return max(0, cnt); 
    }
    
    /** (sub)tree of test data */
    struct Node
      : boost::equality_comparable<Node>
      {
        typedef std::vector<Node> Children;
        
        int id_;
        Children children_;
        
        
        Node(int i)  ///< build node explicitly without children
          : id_(i)
          { }
        
        
        Node()  ///<  build a random test subtree
          : id_(nextChildID++)
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
    exploreChildren (Node* node)
    {
      NodeSeq children_to_visit;
      build(children_to_visit).usingSequence (eachAddress (node->children_));
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
        
        static function<VisitationData(Node*)>
        create () { return NodeVisitor(); }
        
        
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
            return (0 - refLevel) + 1;
          } // by convention, root is an implicitly pre-existing context at level 0
      };
    
    
    /**
     * the core of this test: rebuilding a tree
     * based on visitation data, including the \em orientation
     * of the visitation path (up, down, siblings). After construction,
     * the embedded #children_ will reflect the original sequence as
     * described by the given treeTraversal.
     * @remarks this is a blueprint for the scheduler interface,
     *          which accepts a sequence of jobs with dependencies.
     */
    struct TreeRebuilder
      : Node
      {
        template<class IT>
        TreeRebuilder (IT treeTraversal)
          : Node(0)
          {
            populate (transformIterator (treeTraversal, 
                                         NodeVisitor::create()));
          }
        
      private:
        template<class IT>
        void
        populate (IT treeVisitation)
          {
                struct Builder
                  {
                    Builder (Node& startPoint)
                      : parent_(NULL)
                      , current_(&startPoint)
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
                    Node* parent_;
                    Node* current_;
                    
                    void
                    addNode (int id)
                      {
                        current_ = & parent_->makeChild(id);
                      }
                    
                    Node&
                    startChildTransaction()
                      {
                        Node& oldRefPoint (*parent_);
                        ASSERT (current_);
                        parent_ = current_;  // set new ref point
                        return oldRefPoint;
                      }
                    
                    void
                    commitChildTransaction(Node& refPoint)
                      {
                        parent_ = &refPoint;
                        current_ = parent_;
                      }
                  };
            
            
            Builder builder(*this); // pre-existing implicit root context
            builder.populateBy (treeVisitation);
          }
      };
    
  
  } //(End) test fixture
  
  
  
  
  
  /***************************************************************************
   * @test describing and rebuilding a tree structure
   *       while visiting the tree in depth first order.
   *       
   * @see HierarchyOrientationIndicator
   * @see DispatcherInterface_test
   */
  class HierarchyOrientationIndicator_test : public Test
    {
      
      virtual void run (Arg)
        {
          demonstrate_tree_rebuilding ();
        }
      
      /** @test demonstrate how a Node tree structure can be rebuilt
       * just based on the visitation sequence of an original tree.
       * This visitation captures the local data of the Node (here the ID)
       * and the orientation of the visitation path (down, next sibling, up)
       * 
       * This is a demonstration and blueprint for constructing the scheduler interface.
       * The Scheduler accepts a series of new jobs, but jobs may depend on each other,
       * and the jobs are created while exploring the dependencies in the render engine's
       * node graph (low-level-model).
       */
      void demonstrate_tree_rebuilding ( )
        {
          Node::Children testWood;
          for (uint i=0; i < TEST_SEQUENCE_LENGTH; ++i)
            testWood.push_back(Node());
          
          TreeRebuilder reconstructed (depthFirst (eachAddress (testWood)) >>= exploreChildren);
          
          CHECK (reconstructed.children_ == testWood);
        }
      
    };
  
  /** Register this test class... */
  LAUNCHER(HierarchyOrientationIndicator_test, "unit common");
  
}} // namespace lib
