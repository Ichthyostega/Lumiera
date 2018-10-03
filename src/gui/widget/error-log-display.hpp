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
#include "lib/format-string.hpp"
#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <memory>
#include <utility>
#include <vector>



namespace gui  {
namespace widget {
  
  using util::_Fmt;
  using lib::Literal;
  using std::make_pair;
  using std::vector;
  using std::move;
  
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
  
  
  
  /*********************************************************************//**
   * Widget to display log and error messages.
   * Based on a multiline text display box with scrollbars.
   * Warning and error messages are highlighted by special formatting.
   * And [error entries are treated specially](\ref addError()), insofar
   * they are tracked by an index, allowing to build additional convenience
   * features later on...
   * @remarks this is _just a widget_ intended for direct use. By default,
   *   it is managed by the \ref NotificationHub, which is a controller,
   *   attached to the UI-Bus and responding to the [UI-Element protocol]
   *   (\ref tangible.hpp).
   */
  class ErrorLogDisplay
    : public Gtk::ScrolledWindow
    {
      
      using Mark = Glib::RefPtr<Gtk::TextBuffer::Mark>;
      using Entry = std::pair<Mark,Mark>;
      
      vector<Entry> errorMarks_;
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
      
      
      /** empty buffer and discard all error bookmarks */
      void
      clearAll()
        {
          errorMarks_.clear();
          size_t lineCnt = textLog_.get_buffer()->get_line_count();
          string placeholder;
          if (lineCnt > 0)
            placeholder = _Fmt{_("───════ %d preceding lines removed ════───\n")} % lineCnt;
          textLog_.get_buffer()->set_text (placeholder);  // discard existing content
        }
      
      
      /** just add normal information message to buffer,
       *  without special markup and without expanding the widget */
      void
      addInfo (string text)
        {
          addEntry (text);
        }
      
      /** add an information message, formatted more prominent as warning */
      void
      addWarn (string text)
        {
          addEntry ("WARNING: "+text, TAG_WARN);
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
          errorMarks_.emplace_back(
              addEntry ("ERROR: "+text, TAG_ERROR));
          if (not expand.isExpanded())
            expand (true);
        }
      
      /**
       * clear all mere information messages;
       * retain just the previously tagged errors
       * @remark in fact populates a new buffer
       */
      void
      clearInfoMsg()
        {
          auto newBuff = Gtk::TextBuffer::create (textLog_.get_buffer()->get_tag_table());
          vector<Entry> newMarks;
          for (Entry& entry : errorMarks_)
            {
              newBuff->insert (newBuff->end(), "\n");
              auto pos  = newBuff->end();
              --pos;
              newMarks.emplace_back(
                make_pair (
                    newBuff->create_mark (pos, true),     // "left gravity" : stays to the left of inserted text
                    newBuff->create_mark (pos, false))); //  "right gravity": sticks right behind the inserted text))
              
              newBuff->insert (pos                     //    copy from old to new buffer, complete with formatting tag
                              ,entry.first->get_iter()
                              ,entry.second->get_iter()
                              );
            }
           // install the reduced new buffer
          auto oldBuff = textLog_.get_buffer();
          textLog_.set_buffer(newBuff);
          swap (errorMarks_, newMarks);
          
          // add a marker line to indicate the removed old log contents
          int oldLines = oldBuff->get_line_count();
          int newLines = newBuff->get_line_count();
          ASSERT (oldLines >= newLines);
          addInfo (_Fmt{_("───════ %d old log lines removed ════───\n")} % (oldLines-newLines));
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
       * @return pair of anonymous marks bracketing the content added
       * @remark an entry is content sans the following line break, which is appended automatically.
       *         We inject the content _between_ two marks, which will adjust when content is altered.
       * @remark According to the [API doc], `TextView::scroll_to(iter)` is not reliable; preferably
       *         we should use a text mark and set that text mark to the [insert position][insert-mark].
       *         The handling of marks and tags is described in the [GTKmm tutorial].
       * @warning Each entry creates a new pair of marks. Not sure about the impact on performance...
       * 
       * [GTKmm tutorial]: https://developer.gnome.org/gtkmm-tutorial/stable/sec-textview-buffer.html.en#textview-marks
       * [insert-mark]: https://developer.gnome.org/gtkmm/3.22/classGtk_1_1TextMark.html#details
       * [API doc]: https://developer.gnome.org/gtkmm/3.22/classGtk_1_1TextView.html#a8412941c4da9a71a381052d6049164e4
       */
      Entry
      addEntry (string const& text, Literal markupTagName =nullptr)
        {
          auto buff = textLog_.get_buffer();
          buff->insert (buff->end(), "\n");
          auto pos  = buff->end();
          --pos;
          auto begin = buff->create_mark (pos, true);  // "left gravity" : stays to the left of inserted text
          auto after = buff->create_mark (pos, false);//  "right gravity": sticks right behind the inserted text
          if (markupTagName)
            buff->insert_with_tag(pos, text, cuString{markupTagName});
          else
            buff->insert (pos, text);
          textLog_.scroll_to (begin);
          return make_pair (move(begin), move(after));
        }
    };
  
  
}}// namespace gui::widget
#endif /*GUI_WIDGET_ERROR_LOG_DISPLAY_H*/
