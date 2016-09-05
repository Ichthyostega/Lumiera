/*
  Tangible  -  common implementation base of all relevant interface elements

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

* *****************************************************/


/** @file tangible.cpp
 ** Common base implementation of all tangible and connected interface elements.
 ** 
 ** @see abstract-tangible-test.cpp
 ** @see [explanation of the fundamental interactions](tangible.hpp)
 ** 
 */


#include "gui/model/tangible.hpp"
#include "gui/model/widget.hpp"
#include "gui/model/controller.hpp"
#include "lib/diff/gen-node.hpp"


namespace gui {
namespace model {
  
  
  
  Tangible::~Tangible() { }  // Emit VTables here...
  
  
  
  /** invoke the generic reset hook
   * @note the actual subclass has to override the doReset() hook
   *       to perform the actual clean-up work.
   * @note in case an actual reset happened, the implementation should
   *       return `true` from this `doReset()` hook. As a consequence,
   *       a new "reset" state mark is emitted, which causes the
   *       PresentationStateManager to discard any state
   *       previously recorded for this element.
   * @remarks the intention is that, after invoking reset(), the
   *       interface element or controller is in pristine (presentation) state
   */
  void
  Tangible::reset()
    {
      if (this->doReset())
        uiBus_.note (GenNode{"reset", true});
    }
  
  
  /** invoke the hook to clear error markers
   * @note the actual subclass has to override the doClearErr() hook...
   * @note and similar to the reset() call, also the implementation should
   *       return `true` in case any actual (sticky) error state has been cleared.
   *       Again, this causes emitting of a "resetErr" state mark, which will
   *       purge any sticky error state remembered within the state manager.
   * @remarks usually, most error markers are _not sticky_, that is, they will
   *       be forgotten when the session ends. In this case, the implementation
   *       doesn't need to care for anything special. Only in those cases, where
   *       actually an error state has to be preserved, the implementation should
   *       - emit an "Error" state mark, with the sticky error state in payload
   *       - be prepared to recognise when this sticky error state is fed back
   *       - actually signal the sticky state has to be purged when it is cleared.
   */
  void
  Tangible::clearErr()
    {
      if (this->doClearErr())
        uiBus_.note (GenNode{"clearErr", true});
    }
  
  
  /** invoke the hook to clear notification messages
   * @remarks everything is symmetrical to reset() and clearErr() here...
   */
  void
  Tangible::clearMsg()
    {
      if (this->doClearMsg())
        uiBus_.note (GenNode{"clearMsg", true});
    }
  
  
  /** highlight the element visually to catch the user's attention
   * @remarks this is meant as a short transient visual change,
   *       just to indicate something of relevance happened here.
   */
  void
  Tangible::markFlash()
    {
      this->doFlash();
    }
  
  
  /** push a notification (or warning) message to the element.
   * @param message notification text
   * @note the actual interface response need to be coded
   *       in the concrete subclass within doMsg().
   * @remarks the intention is for this message to be somehow
   *       visible at this element, e.g. as mouse over.
   *       When this notification is meant to be "sticky" / permanent,
   *       then the mentioned doMsg() implementation function should
   *       return true; in this case we'll emit a "state mark notification",
   *       which will be recorded by the PresentationStateManager, under
   *       the property name "`Message`" for this UI-Element.
   *       This mechanism allows to persist such UI states.
   */
  void
  Tangible::markMsg (string message)
    {
      if (this->doMsg (message))
        uiBus_.note (GenNode{"Message", message});
    }
  
  
  /** push an error state tag to the element
   * @remarks everything detailed at markMsg applies here too.
   */
  void
  Tangible::markErr (string error)
    {
      if (this->doErr (error))
        uiBus_.note (GenNode("Error", error));
    }
  
  
  /**
   * Expand this element and remember the expanded state.
   * This is a generic Slot to connect UI signals against.
   * @note The concrete Widget or Controller has to override the
   *       ::doExpand() extension point to provide the actual UI
   *       behaviour. If this virtual method returns `true`, the
   *       state change is deemed relevant and persistent, and
   *       thus a "state mark" is sent on the UI-Bus.
   */
  void
  Tangible::slotExpand()
  {
    if (this->doExpand(true))
      uiBus_.note (GenNode("expand", true));
  }
  
  
  /**
   * Collapse or minimise this element and remember the collapsed state.
   * This is a generic Slot to connect UI signals against.
   */
  void
  Tangible::slotCollapse()
  {
    if (this->doExpand(false))
      uiBus_.note (GenNode("expand", false));
  }
  
  
  /**
   * @todo 12/2015 not clear yet what needs to be done
   * @remarks the intention is to request the given child
   *          to be brought into sight. We need to set up some kind
   *          of children registration, but better not do this in
   *          a completely generic fashion, for danger of overengineering.
   *          Moreover, it is not clear yet, who will issue this request
   *          and at which element the initial request can/will be targeted.
   */
  void
  Tangible::slotReveal(ID child)
  {
    this->doReveal(child);
    this->doRevealYourself();
  }
  
  
  
  /**
   * Prepare a command or action for actual invocation, once the execution context
   * has been established. The action is not executed right away, but it is now ready
   * and bound to the concrete arguments supplied with the [record](\ref lib::diff::Rec).
   * @param prototype handle to a command instantiation, to be readied for invocation
   * @param arguments suitable tuple of values, to be used to outfit the prototype
   */
  void
  Tangible::prepareCommand (Cmd const& prototype, Rec&& arguments)
  {
    uiBus_.act (prototype.bind (std::forward<Rec>(arguments)));
  }
  
  
  /**
   * Actually trigger execution of an action or command.
   * @param preparedAction handle pointing to a command definition,
   *        which needs to be outfitted with arguments and ready for invocation.
   */
  void
  Tangible::issueCommand (Cmd const& preparedAction)
  {
    uiBus_.act (preparedAction.bang());
  }
  
  
  
  /** generic handler for all incoming "state mark" messages */
  void
  Tangible::mark (GenNode const& stateMark)
  {
    if (stateMark.idi.getSym() == "Flash")
      this->doFlash();
    else
    if (stateMark.idi.getSym() == "Error")
      this->markErr (stateMark.data.get<string>());
    else
    if (stateMark.idi.getSym() == "Message")
      this->markMsg (stateMark.data.get<string>());
    else
      this->doMark(stateMark);
  }
  
  
  /**
   * default implementation and catch-all handler for receiving »state mark« messages.
   * Such messages serve either to cause a presentation state effect specific to this
   * element, or they are used to re-play a former state change to restore some
   * specific UI state captured within a past working session. Events handled here:
   * - *expand* with a `bool` argument calls the `doExpand(bool)` virtual function.
   *   It is up to the concrete element to give this a tangible meaning, e.g. a track
   *   might switch to detail view and a clip might reveal attached effects.
   * - *reset* restores the element to the hard wired default, by invoking `doReset()`
   * - *revealYourself* prompts the element to take the necessary actions to bring
   *   itself into view. There is no requirement for an element to even implement
   *   this call, but those which do typically know some kind of _"parent object"_
   *   to forward this request, by invoking `doReveal(myID)` on this parent.
   *   For instance, a clip might ask the enclosing track, which in turn might
   *   call the enclosing timeline display for help, resulting in a scroll action
   *   to bring the clip into sight.
   * @note this is a default implementation for a virtual function declared _abstract_
   *       with the intention for derived classes to tail-call this default handler.
   */
  void
  Tangible::doMark (GenNode const& stateMark)
  {
    if (stateMark.idi.getSym() == "expand")
      this->doExpand (stateMark.data.get<bool>());
    else
    if (stateMark.idi.getSym() == "reset")
      this->reset();
    else
    if (stateMark.idi.getSym() == "clearMsg")
      this->clearMsg();
    else
    if (stateMark.idi.getSym() == "clearErr")
      this->clearErr();
    else
    if (stateMark.idi.getSym() == "revealYourself")
      this->doRevealYourself();
  }
  
  
  
}} // namespace gui::model
