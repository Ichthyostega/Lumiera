/*
  TANGIBLE.hpp  -  a tangible element of the user interface

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


/** @file tangible.hpp
 ** Abstraction: a tangible element of the User Interface.
 ** This is a generic foundation for any elements of more than local relevance
 ** within the Lumiera UI.<br/> Any such element is connected to the [UI-Bus](\ref ui-bus.hpp).
 ** 
 ** ## rationale
 ** 
 ** Simple user interfaces can be built by wiring up the actions right within the
 ** code processing the trigger of actions. This leads to core functionality littered
 ** and tangled with presentation code. The next step towards a more sane architecture
 ** would be to code a forwarding call into every UI action, invoking some core facade
 ** in turn. This approach works, but is repetitive and thus lures the lazy programmer
 ** into taking shortcuts. Since we can foresee the Lumiera UI to become quite challenging
 ** in itself, we prefer to introduce a **mediating backbone**, impersonating the role
 ** of the _Model_ and the _Controler_ in the [MVC-Pattern] in common UI architecture.
 ** 
 ** The MVC-Pattern as such is fine, and probably the best we know for construction of
 ** user interfaces. But it doesn't scale well towards the integration into a larger and
 ** more structured system. There is a tension between the Controller in the UI and other
 ** parts of an application, which as well need to be _in control._ And, even more important,
 ** there is a tension between the demands of UI elements for support by a model, and the
 ** demands to be placed on a core domain model of a large scale application. This tension is
 ** resolved by enacting these roles while transforming the requests and demands into _Messages._
 ** 
 ** This way, we separate between immediate local control of UI state and the more global,
 ** generic concerns of interaction control and command binding. The immediately tangible
 ** "mechanics" of the UI shall be implemented in a conventional way, right within the
 ** concrete widget (or controller) code. But, since any widget concerned with more than
 ** local behaviour will inherit from \ref Tangible, the embedded [UI-Bus terminal](\ref ctrl::BusTerm)
 ** `Tangible::uiBus_`
 ** can be used for interaction with core services.
 ** 
 ** ## the generic interface element API
 ** 
 ** The _generic interface element_ based on [Tangible](\ref model::Tangible) covers a set of behaviour
 ** common to all elements of the interface. This behaviour is targeted towards the _integration_
 ** with the core application. Beyond that, there are still several concerns regarding presentation,
 ** like a common styling. These are addressed the conventional way, through a common \ref StyleManager.
 ** The following discussion focuses on the aspects of integration with the core.
 ** 
 ** For one reason or another, any element in the UI can appear and go away.
 ** This lifecycle behaviour corresponds to attachment and deregistration at the UI-Bus
 ** 
 ** In regular, operative state, an interface element may initiate _actions_, which translate
 ** into _commands_ at the session interface. To complicate matters, there might be higher-level,
 ** cooperative _gestures_ implemented within the interface, leading to actions being formed
 ** similar to sentences of spoken language, with the help of a _focus concept_ -- anyway, this
 ** means, in the end, there is a _subject_ and a _predicate_. These need to be bound in order
 ** to form an _action_. And some interface element takes on or relates to the role of the
 ** underlying, the subject, the **tangible element**.
 ** 
 ** @todo Some actions are very common and can be represented by a shorthand.
 ** An example would be to tweak some property, which means to mutate the attribute of a
 ** model element known beforehand. Such tweaks are often caused by direct interaction,
 ** and thus have the tendency to appear in flushes, which we want to batch in order to
 ** remove some load from the lower layers.
 ** 
 ** And then there are manipulations that _alter presentation state:_ Scrolling, canvas dragging,
 ** expanding and collapsing, moving by focus or manipulation of a similar presentation control.
 ** These manipulations in itself do not constitute an action. But there typically is some widget
 ** or controller, which is responsible for the touched presentation state. If this entity judges
 ** the state change to be relevant and persistent, it may [send](\ref BusTerm::note()) a
 ** **state mark** into the UI-Bus -- expecting this marked state to be remembered.
 ** In turn this means the bus terminal might feed a state mark back into the
 ** tangible element, expecting this state to be restored.
 ** 
 ** A special case of state marking is the presentation of _transient feedback._
 ** Such feedback is pushed from "somewhere" towards given elements, which react through an
 ** implementation dependent visual state change (flushing, colour change, marker icon).
 ** If such state marking is to be persistent, the interface element has in turn to send
 ** a specific state mark. An example would be a permanent error flag with an explanatory
 ** text showed in mouse over.
 ** 
 ** And finally, there are the _essential updates_ -- any changes in the model _for real._
 ** These are sent as notifications just to some relevant top level element, expecting this element
 ** to request a [diff](\ref tree-diff.hpp) and to mutate contents into shape recursively.
 ** 
 ** ## Interactions
 ** 
 ** - **lifecycle**: connect to an existing term, supply the [EntryID](\ref Tangible::ID) of the new element.
 **   This interaction also implies, that the element automatically detaches itself at end of life.
 ** - **act**: send a \ref GenNode representing the action
 ** - **note**: _send_ a GenNode representing the _state mark_
 ** - **mark**: _receive_ a GenNode representing the _feedback,_ a replayed _state mark_ or _generic message._
 ** - **diff**: prompt the element to retrieve a diff, which
 **   - either is an incremental status update
 **   - or is a from-scratch reconfiguration
 ** 
 ** Beside these basic interactions, the generic element also exposes some common signal slots
 ** - slotExpand() prompts the element to transition into expanded / unfolded state.
 **   If this state is to be sticky, the element answers with a _state mark_
 ** - slotReveal() prompts the element to bring the indicated child into sight.
 **   Typically, this request will "bubble up" recursively.
 ** These slots are defined to be `sigc::trackable` for automated disconnection
 ** see [sigc-track] for an explanation.
 ** 
 ** [MVC-Pattern]: http://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller
 ** [sigc-track]: http://issues.lumiera.org/ticket/940#comment:3 "Ticket #940"
 ** 
 ** @see \ref AbstractTangible_test
 ** @see \ref BusTerm_test
 ** 
 */


#ifndef STAGE_MODEL_TANGIBLE_H
#define STAGE_MODEL_TANGIBLE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "stage/ctrl/bus-term.hpp"
#include "stage/model/expander-revealer.hpp"
#include "lib/diff/diff-mutable.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/symbol.hpp"

#include <sigc++/trackable.h>
#include <utility>
#include <string>


namespace stage {
namespace model {
  
  using std::string;
  using lib::Symbol;
  
  
  /**
   * Interface common to all UI elements of relevance for the Lumiera application.
   * Any non-local and tangible interface interaction will at some point pass through
   * this foundation element, which forms the joint and attachment to the UI backbone,
   * which is the [UI-Bus](\ref ui-bus.hpp). Any tangible element acquires a distinct identity
   * and has to be formed starting from an already existing bus nexus.
   * @see [explanation of the basic interactions](\ref tangible.hpp)
   * @warning Tangible is `NonCopyable` for good reason: the UI-Bus Nexus adds a direct
   *          reference into the routing table, tied to the given Tangible's ID (identity.
   *          Consequently you must not store tangibles in STL containers, since these
   *          might re-allocate and thus change the location in memory.
   */
  class Tangible
    : public sigc::trackable
    , public lib::diff::DiffMutable
    , util::NonCopyable
    {
    public:
      using ID = ctrl::BusTerm::ID;
      using LuidH = lib::hash::LuidH;
    protected:
      using Rec     = lib::diff::Rec;
      using GenNode = lib::diff::GenNode;
      
      ctrl::BusTerm uiBus_;
      
      Expander expand_;
      Revealer reveal_;
      
      
      Tangible(ID identity, ctrl::BusTerm& nexus)
        : uiBus_{nexus.attach(identity, *this)}
        , expand_{}
        , reveal_{}
        { }
      
    public:
      virtual ~Tangible();  ///< this is an interface
      
      operator string() const;
      operator LuidH()  const { return uiBus_.getID().getHash(); }
      
      ID getID()        const { return uiBus_.getID();}
      
      void reset();
      void clearMsg();
      void clearErr();
      
      template<typename...ARGS>
      void invoke (Symbol cmdID, ARGS&&...);
      void invoke (Symbol cmdID, Rec&& arguments);
      
      void slotExpand();
      void slotCollapse();
      void slotReveal();
      
      void markFlash();
      void markMsg (string message);
      void markErr (string error);
      void mark(GenNode const&);
      
      void installExpander (Expander::ProbeFun, Expander::ChangeFun);
      void installRevealer (Revealer::RevealeItFun);
      
    protected:
      virtual bool doReset()           =0;
      virtual bool doClearMsg()        =0;
      virtual bool doClearErr()        =0;
      virtual bool doExpand (bool yes);
      virtual void doReveal ();
      
      virtual bool doMsg (string)          =0;
      virtual bool doErr (string)          =0;
      virtual void doFlash()               =0;
      virtual void doMark(GenNode const&)  =0;
      
    public:
      virtual void buildMutator (lib::diff::TreeMutator::Handle)  =0;
      
      
      /** override default size traits for diff application.
       * @remarks this value here is hard coded, based on what
       *  can be expected for diff application to UI elements.
       */
      friend constexpr size_t
      treeMutatorSize (const Tangible*)
      {
        return 512;
      }
    };
  
  
  
  /** convenience shortcut to build a message suitable for command invocation
   * @param args... sequence of arguments to be packaged into a lib::diff::Rec for invocation
   */
  template<typename...ARGS>
  inline lib::diff::GenNode
  commandMessage (Symbol cmdID, ARGS&&... args)
  {
    using lib::diff::Rec;
    using lib::diff::GenNode;
    using GenNodeIL = std::initializer_list<GenNode>;
    
    
    return GenNode (string{cmdID},
                    Rec(Rec::TYPE_NIL_SYM
                       ,GenNodeIL{}
                       ,GenNodeIL {std::forward<ARGS> (args)...}));
  }                  // not typed, no attributes, all arguments as children
  
  
  /** convenience shortcut to issue a command with several arguments */
  template<typename...ARGS>
  inline void
  Tangible::invoke (Symbol cmdID, ARGS&&... args)
  {
    uiBus_.act (commandMessage (cmdID, std::forward<ARGS> (args)...));
  }
  
  
  /**
   * Configure the (optional) functionality to expand or collapse the UI-Element.
   * @param detectCurrExpansionState a lambda or function<bool()> to retrieve if the element is currently expanded
   * @param howto_expand_collapse a lambda or function<void(bool)> to switch the element's expansion state
   * @note unless this setup function is invoked, the expand/collapse functionality remains disabled;
   *       invoking the #slotExpand() or sending **mark** "`expand`" messages via UI-Bus has no effect then.
   */
  inline void
  Tangible::installExpander (Expander::ProbeFun detectCurrExpansionState,
                             Expander::ChangeFun howto_expand_collapse)
  {
    expand_ = Expander{move (detectCurrExpansionState), move (howto_expand_collapse)};
  }
  
  
  /**
   * Configure the (optional) functionality to bring the UI-Element into sight.
   * @param how_to_uncover_the_element a lambda or function<void()> to actually cause the necessary actions.
   * @note unless this setup function is invoked, the "`reveal`" functionality remains disabled.
   *       Typically this setup will be done by an owning parent container, binding to some internals
   *       and also recursively invoking the "`reveal`" action on the container.
   */
  inline void
  Tangible::installRevealer (Revealer::RevealeItFun how_to_uncover_the_element)
  {
    reveal_ = Revealer{move (how_to_uncover_the_element)};
  }

  
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_TANGIBLE_H*/
