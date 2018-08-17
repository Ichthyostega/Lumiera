/*
  NOTIFICATION-HUB.hpp  -  receive and reroute notification messages

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file notification-hub.hpp
 ** Controller to receive and handle notification messages for the user.
 ** This component is addressed in order to show error, warning and notification messages
 ** in the UI. It is responsible for maintaining a local log of these messages, and to
 ** allocate an appropriate display widgets, to show such notifications asynchronously.
 ** On first use, an InfoboxPanel is allocated to hold an ErrorlogWidget for presentation
 ** of those messages
 ** 
 ** # Notification Controller Behaviour
 ** 
 ** This controller serves the purpose to present information and error messages to the user.
 ** For the actual display, it allocates an appropriate view, placed into a docking pane
 ** reserved for information display, in accordance to the configured screen layout
 ** @todo 8/2018 for now we just use and possibly create a hard wired display within the
 **       first (primary) application window. Moreover, we do so directly, instead of
 **       consulting the (not yet fully implemented) ViewSpec mechanism. (via ViewLocator).
 ** 
 ** The actual widget for information display is prepared, but it is actually allocated when
 ** the need for information display arises. Which means, the user may close this display
 ** widget, thereby discarding its current information content -- but when the next notification
 ** needs to be shown, the controller will ensure to allocate an appropriate widget again. Moreover,
 ** the widget can be _expanded_ or _collapsed_, without affecting its content.
 ** - information messages are just added to the buffer without much ado. No attempt is made
 **   to reveal or expand the widget (but if necessary, a new widget is allocated)
 ** - error messages also set an error marker state (*TODO* reflect this in the presentation),
 **   and they cause the display widget to be expanded
 **   (*TODO* 8/18 consider also call the doRevealYourself() function)
 ** - the error state can be _cleared_, which also demotes all error messages to mere information.
 ** - information content can also be _cleared_, which removes all mere information messages,
 **   while retaining the error entries.
 ** - the doReset() operation completely clears the log contents, collapses the widget and clears state.
 ** - expanding of the display widget is state marked, irrespective if it happened by user interaction
 **   or as result of some display. However, the actual message content is _not_ state marked; it needs
 **   to be persisted elsewhere (in the session) and replayed from there if desired.
 ** 
 ** @see error-log-display.hpp
 ** @see notification-service.hpp
 */


#ifndef GUI_CTRL_NOTIFICATION_HUB_H
#define GUI_CTRL_NOTIFICATION_HUB_H

#include "gui/widget/error-log-display.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "gui/model/controller.hpp"
#include "gui/model/w-link.hpp"

#include <functional>
//#include <memory>
//#include <list>


namespace gui {
namespace ctrl {
  
  using lib::diff::TreeMutator;
  using model::WLink;
  
  
  
  
  /**
   * Service to receive and display error, warning and notification messages.
   * These are sent over the UI-Bus through the NotificationService; after receiving
   * such a message, this controller ensures to display the message and alert the
   * user, while not blocking the overall UI. 
   */
  class NotificationHub
    : public model::Controller
    {
      
      
      /** content population and manipulation via UI-Bus */
      void
      buildMutator (lib::diff::TreeMutator::Handle buffer)  override
        {
      //  using Attrib = std::pair<const string,string>;
      //  using lib::diff::collection;
          
          buffer.create (
            TreeMutator::build()
          );
          UNIMPLEMENTED ("define and implement what need to be reflected from asset::ErrorLog");
        }
    
      
      /* ==== Tangible interface ==== */
      
      virtual bool
      doReset()  override
        {
          UNIMPLEMENTED ("Clear log contents, collapse widget, clear error state");
        }
      
      virtual bool
      doExpand (bool yes)  override
        {
          UNIMPLEMENTED ("Allocate Widget if necessary, expand widget");
        }
      
      virtual void
      doRevealYourself()  override
        {
          UNIMPLEMENTED ("TODO: how can the Log-Widget be 'revealed'?? Possibly allocate Widget, expand it. But how to make it visible?");
        }
      
      virtual bool
      doMsg (string text)  override
        {
          UNIMPLEMENTED ("Possibly allocate Widget, place text into its buffer. No need to expand");
        }
      
      virtual bool
      doClearMsg ()  override
        {
          UNIMPLEMENTED ("remove all mere information messages");
        }
      
      virtual bool
      doErr (string text)  override
        {
          UNIMPLEMENTED ("Set error state. Allocate Widget if necessary, expand widget, place error message into its buffer");
        }
      
      virtual bool
      doClearErr ()  override
        {
          UNIMPLEMENTED ("clear error state. If widget exists, turn all error entries into mere information entries");
        }
      
      virtual void
      doFlash()  override
        {
          UNIMPLEMENTED ("If widget exists: expand it, trigger its flash function (paint with timeout). TODO also doRevealYourself.");
        }
      
      
    public:
      using WidgetAllocator = std::function<widget::ErrorLogDisplay&(void)>;
      
      
      NotificationHub (ID identity, ctrl::BusTerm& nexus, WidgetAllocator wa)
        : model::Controller{identity, nexus}
        , allocateWidget_{wa}
        , widget_{}
        { }
      
     ~NotificationHub() { };
      
    private:
      
      /** external operation to find or allocate an log display widget */
      WidgetAllocator allocateWidget_;
      
      /** collaboration with a log display allocated elsewhere */
      WLink<widget::ErrorLogDisplay> widget_;
      
    };
  
  
  
  /** */
  
  
  
}}// namespace gui::ctrl
#endif /*GUI_CTRL_NOTIFICATION_HUB_H*/
