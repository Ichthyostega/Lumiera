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
 ** @todo WIP-WIP and in 9/2018 this draft gradually shifts towards a prototype
 **       how to deal with custom configured widget behaviour, and how to integrate
 **       with our GUI framework                /////////////////////////////////////////////////////////////TICKET #1099 : build a prototype/dummy for UI <-> Proc integration  
 ** 
 */


#ifndef GUI_WIDGET_ERROR_LOG_DISPLAY_H
#define GUI_WIDGET_ERROR_LOG_DISPLAY_H

#include "gui/gtk-base.hpp"
#include "gui/model/expander-revealer.hpp"
#include "include/gui-notification-facade.h"
#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <memory>
#include <vector>



namespace gui  {
namespace widget {
  
  using lib::Literal;
  using std::vector;
  
  namespace {
    
    const Literal TAG_ERROR{"ERROR"};           /////////////////////////////////////////////////////////////TICKET #1168 : find a way to manage style of custom extended UI elements
    const Literal TAG_WARN{"WARN"};
    
    
    using Tag = Glib::RefPtr<Gtk::TextBuffer::Tag>;
    
    /**
     * @internal inject some generic standard styles for use in `TextView` components.
     * @param tagTable reference to a `TagTable` bound with an existing `Gtk::TextBuffer`.
     * @todo 9/2018 dummy placeholder code, later to be transformed into a framework    /////////////////////TICKET #1168 : find a way to manage style of custom extended UI elements
     */
    inline void
    populateStandardTextTags (Glib::RefPtr<TextBuffer::TagTable> tagTable)
    {
      Tag errorTag = Gtk::TextBuffer::Tag::create (cuString{TAG_ERROR});
      errorTag->property_background() = "Yellow";                ////////////////////////////////////////////TICKET #1168 : should be retrieved from a central location
      errorTag->property_weight() = PANGO_WEIGHT_BOLD;
      tagTable->add (errorTag);
      
      Tag warnTag = Gtk::TextBuffer::Tag::create (cuString{TAG_WARN});
      warnTag->property_background() = "LightYellow";            ////////////////////////////////////////////TICKET #1168 : should be retrieved from a central location
      tagTable->add (warnTag);
    }
  }
  
  
  /**
   * @todo WIP-WIP as of 9/2017
   *       Just a text display box with scrollbars.
   *       Need to add formatting etc.
   */
  class ErrorLogDisplay
    : public Gtk::ScrolledWindow
    {
      
      using Mark = Glib::RefPtr<Gtk::TextBuffer::Mark>;
      
      vector<Mark> errorMarks_;
      Gtk::TextView textLog_;

    public:
     ~ErrorLogDisplay() { };
      
      ErrorLogDisplay()
        : Gtk::ScrolledWindow()
        , errorMarks_{}
        , textLog_{}
        {
          set_size_request (200, 80);           // must be > 50 for the scrollbar to work properly
          property_expand() = true;             // always grab any available additional space
          set_border_width (10);
          set_shadow_type  (Gtk::SHADOW_NONE);
          
          // the vertical scrollbar will always be necessary....
          set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
          this->add (textLog_);
          textLog_.set_editable (false);
          
          populateStandardTextTags (textLog_.get_buffer()->get_tag_table());
        }
      
      model::Expander expand;
      model::Revealer reveal;
      
      
      void
      clearAll()
        {
          UNIMPLEMENTED ("empty buffer and discard all error bookmarks");
        }
      
      /** just add normal information message to buffer,
       *  without special markup and without expanding the widget */
      void
      addMsg (string text)
        {
          showMsg (NOTE_INFO, text);
        }
      
      /** present an error notification prominently.
       *  Adds the error text, formatted accordingly to stand out,
       *  but also stores a [Mark] to bookmark the presence of this
       *  error entry. And finally expand the display if collapsed.
       *  
       * [Mark]: https://developer.gnome.org/gtkmm-tutorial/stable/sec-textview-buffer.html.en#textview-marks
       */
      void
      addError (string text)
        {
          showMsg (NOTE_ERROR, text);
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
      
      
      
    private:/* ===== Internals ===== */
      
      /** add message entry to the (ever growing) text buffer.
       * @remark According to the [GTKmm tutorial], `TextView::scroll_to(iter)` is not reliable;
       *         rather we need to use a text mark and set that text mark to the insert position.
       *         Actually, there is always one predefined text mark [called "insert"][insert-mark],
       *         which corresponds to the text cursor. Thus it suffices to navigate to text end,
       *         insert and scroll into view.
       * 
       * [GTKmm tutorial]: https://developer.gnome.org/gtkmm-tutorial/stable/sec-textview-buffer.html.en#textview-marks
       * [insert-mark]: https://developer.gnome.org/gtkmm/3.22/classGtk_1_1TextMark.html#details
       */
      auto
      addEntry (string const& text, Literal markupTagName =nullptr)
        {
          auto buff = textLog_.get_buffer();
          auto cursor = buff->get_insert();
          buff->move_mark (cursor, buff->end());
          if (markupTagName)
            buff->insert_with_tag(buff->end(), text, cuString{markupTagName});
          else
            buff->insert (buff->end(), text);
          textLog_.scroll_to (cursor);
          return cursor;
        }
      
      void
      showMsg (NotifyLevel severity, string const& text)
        {
          //////////////////////////////////////////////////TICKET #1102 : add formatting according to the error level
          switch (severity) {
            case NOTE_ERROR:
                errorMarks_.emplace_back(
                    addEntry ("ERROR: "+text, TAG_ERROR));
                if (not expand.isExpanded())
                  expand (true);
              break;
            case NOTE_WARN:
                addEntry ("WARN: "+text, TAG_WARN);
              break;
            default:
                addEntry (text);
              break;
          }
        }
    };
  
  
}}// namespace gui::widget
#endif /*GUI_WIDGET_ERROR_LOG_DISPLAY_H*/
