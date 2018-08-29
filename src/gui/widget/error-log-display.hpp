/*
  ERROR-LOG-DISPLAY.hpp  -  display of error messages in a text box

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file error-log-display.hpp
 ** Widget to display error messages to be accessible in non-modal way.
 ** Notifications are shown in a simple text window with scrollbars; new
 ** entries can be added with a severity level, causing the widget to scroll
 ** down to the last line of the content buffer.
 ** 
 ** ## Lifecycle considerations
 ** 
 ** The ErrorLogDisplay is typically managed as child of a container widget.
 ** However, the ctrl::NotificationHub as controller does a lookup and attaches
 ** to an ErrorLogDisplay found within the InfoBoxPanel, using it as holder for
 ** information and error messages pushed into the GUI. For that reason, it is
 ** important really _to destroy_ the ErrorLogDisplay, when it is taken out of
 ** service. Since ErrorLogDisplay inherits from `sigc::tangible`, it is
 ** automatically detached from the WLink implementing this cross-attachment,
 ** when actually the dtor is called.
 ** @note this is a special convention; usually it is sufficient just to hide()
 **       a GTK widget, because it is then left alone by the event handling and
 **       automatically cleaned up at shutdown. Moreover, if you ` remove()` a
 **       widget from a container, the container no longer manages the widget,
 **       so you are responsible to take ownership in such situations. And,
 **       as said, in case of the ErrorLogDisplay, you should `delete` it.
 ** @remark the standard use case is to have the ErrorLogDisplay sitting
 **       within the InfoBoxPanel. If the latter is destroyed, it destroys
 **       its children and all works as expected without much ado.
 ** 
 ** @todo WIP-WIP-WIP as of 9/2017 this is a first draft of a widget to be
 **       used as receiver by the GuiNotificationService.
 ** 
 */


#ifndef GUI_WIDGET_ERROR_LOG_DISPLAY_H
#define GUI_WIDGET_ERROR_LOG_DISPLAY_H

#include "gui/gtk-base.hpp"
#include "include/gui-notification-facade.h"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace gui  {
namespace widget {
  
  
  /**
   * @todo WIP-WIP as of 9/2017
   *       Just a text display box with scrollbars.
   *       Need to add formatting etc.
   */
  class ErrorLogDisplay
    : public Gtk::ScrolledWindow
    {
    public:
     ~ErrorLogDisplay() { };
      
      ErrorLogDisplay()
        : Gtk::ScrolledWindow()
        , textLog_{}
        {
          set_size_request (200, 80);           // must be > 50 for the scrollbar to work properly 
          set_border_width (10);
          set_shadow_type  (Gtk::SHADOW_NONE);
          
          // the vertical scrollbar will always be necessary....
          set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
          this->add (textLog_);
          textLog_.set_editable (false);
        }
      
      
      void
      showMsg (NotifyLevel severity, string const& text)
        {
          //////////////////////////////////////////////////TICKET #1102 : add formatting according to the error level
          switch (severity) {
            case NOTE_ERROR:
                addEntry ("ERROR: "+text);
              break;
            case NOTE_WARN:
                addEntry ("WARN: "+text);
              break;
            default:
                addEntry (text);
              break;
          }
        }
      
      bool
      expand (bool yes)
        {
          UNIMPLEMENTED ("Maintain expansion state, build a collapsed representation.");
          return false; //////TODO true if state change happened; this makes the state persistent
        }
      
      void
      clearAll()
        {
          UNIMPLEMENTED ("empty buffer and discard all error bookmarks");
        }
      
      void
      addMsg (string text)
        {
          UNIMPLEMENTED ("add normal information message to buffer");
        }
      
      void
      addError (string text)
        {
          UNIMPLEMENTED ("add error with special tag and bookmark");
        }
      
      void
      clearInfoMsg()
        {
          UNIMPLEMENTED ("clear all mere information messages; then re-insert the errors with tags");
        }
      
      void
      turnError_into_InfoMsg()
        {
          UNIMPLEMENTED ("visit all errors and downgrade the markup; discard all bookmarks");
        }
      
      void
      triggerFlash()
        {
          UNIMPLEMENTED ("paint and set callback-timeout to return to normal state");
        }
      
      void
      revealYourself()
        {
          UNIMPLEMENTED ("store a link to parent. Ask parent to reveal us. Switch to expanded display");
        }

      
      
    private:/* ===== Internals ===== */
      
      Gtk::TextView textLog_;
      
      /** add message entry to the (ever growing) text buffer.
       * @remark According to the Gtkmm tutorial, TextView::scroll_to(iter) is not reliable;
       *         rather we need to use a text mark and set that text mark to the insert position.
       *         Actually, there is always one predefined text mark called "insert", which corresponds
       *         to the text cursor. Thus it suffices to navigate to text end, insert and scroll into view.
       */
      void
      addEntry (string const& text)
        {
          auto buff = textLog_.get_buffer();
          auto cursor = buff->get_insert();
          buff->move_mark (cursor, buff->end());
          buff->insert (buff->end(), text);
          textLog_.scroll_to (cursor);
        }
    };
  
  
}}// namespace gui::widget
#endif /*GUI_WIDGET_ERROR_LOG_DISPLAY_H*/
