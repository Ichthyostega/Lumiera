/*
  NOTIFICATION-HUB.hpp  -  receive and reroute notification messages

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file notification-hub.hpp
 ** Controller to receive and handle notification messages for the user.
 ** This component is addressed in order to show error, warning and notification messages
 ** in the UI. It is responsible for maintaining a local log of these messages, and to
 ** allocate an appropriate display widgets, to show such notifications asynchronously.
 ** On first use, an InfoboxPanel is allocated to hold an ErrorLogDisplay widget for
 ** presentation of those messages
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
 ** needs to be shown, the controller will ensure to allocate an appropriate widget again.
 ** Moreover, the widget can be _expanded_ or _collapsed_, without affecting its content.
 ** - information messages are just added to the buffer without much ado. No attempt is made
 **   to reveal or expand the widget (but if necessary, a new widget is allocated)
 ** - error messages also set an error marker state, and cause expansion of the display widget.
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


#ifndef STAGE_CTRL_NOTIFICATION_HUB_H
#define STAGE_CTRL_NOTIFICATION_HUB_H

#include "stage/widget/error-log-display.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "stage/model/controller.hpp"
#include "stage/model/w-link.hpp"

#include <functional>


namespace stage {
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
      
      
      /** population and manipulation of persistent content via UI-Bus */
      void
      buildMutator (lib::diff::TreeMutator::Handle buffer)  override
        {
          buffer.emplace(
            TreeMutator::build()
          );
          UNIMPLEMENTED ("define and implement what need to be reflected from asset::ErrorLog");
        }
    
      
      /* ==== Tangible interface ==== */
      
      virtual bool
      doReset()  override
        {
          if (not widget_) return false;
          
          widget_->clearAll();
          widget_->expand (false);
          return true;
        }
      
      virtual bool
      doMsg (string text)  override
        {
          getWidget().addInfo (text);
          return false; // logging is no persistent state
        }
      
      virtual bool
      doClearMsg ()  override
        {
          if (widget_)
            widget_->clearInfoMsg();
          return false; // not persistent (sticky)
        }
      
      virtual bool
      doErr (string text)  override
        {
          getWidget().addError (text);
          return false;
        }
      
      virtual bool
      doClearErr ()  override
        {
          if (widget_)
            widget_->turnError_into_InfoMsg();
          return false; // not persistent (sticky)
        }
      
      /** adds special treatment for a state mark tagged as `"Warning"` */
      virtual void
      doMark (GenNode const& stateMark)  override
        {
          if (stateMark.idi.getSym() == MARK_Warning)
            getWidget().addWarn (stateMark.data.get<string>());
          else
            // forward to default handler
            Controller::doMark (stateMark);
        }
      
      virtual void
      doFlash()  override
        {
          if (not widget_) return;
          widget_->reveal();
          widget_->triggerFlash();
        }
      
      
    public:
      using WidgetAllocator = std::function<widget::ErrorLogDisplay&(void)>;
      
      
      NotificationHub (ID identity, ctrl::BusTerm& nexus, WidgetAllocator wa)
        : model::Controller{identity, nexus}
        , allocateWidget_{wa}
        , widget_{}
        {
          installExpander([&](){ return widget_ and widget_->expand; }
                         ,[&](bool yes)
                               {
                                 if (widget_ or yes)
                                   getWidget().expand (yes);
                               }
                         );
          installRevealer([&](){ getWidget().reveal(); }); // implementation implies also expand
        }
      
     ~NotificationHub() { };
      
      
    private:
      /** external operation to find or allocate an log display widget */
      WidgetAllocator allocateWidget_;
      
      /** collaboration with a log display allocated elsewhere */
      WLink<widget::ErrorLogDisplay> widget_;
      
      
      widget::ErrorLogDisplay&
      getWidget()
        {
          if (not widget_)
            widget_.connect (allocateWidget_());
          return *widget_;
        }
    };
  
  
  
}}// namespace stage::ctrl
#endif /*STAGE_CTRL_NOTIFICATION_HUB_H*/
