/*
  TEST-CONTROL.hpp  -  child window to trigger self tests and diagnostics

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


/** @file test-control.hpp
 ** Non-modal dialog window to trigger diagnostics and self test routines.
 ** This window can be launched from the help menu as child window of the current
 ** top-level workspace window and stays outside of the regular window and perspective
 ** management of the Lumiera UI. This window allows for quick-n-dirty development
 ** helper routines to be hooked up easily within the UI. Actions launched from here
 ** perform within the same environment as regular user interactions.
 ** 
 ** @todo as of 9/2018, this is a first rough draft, relevant for #1099          ////////////////////////////TICKET #1140 gradually augment the self-diagnostics controls in the UI
 ** @todo this header also features a design draft how to simplify building notebook widgets.
 **       Which could be polished and moved into a separate utility header eventually.
 */


#ifndef STAGE_DIALOG_TEST_CONTROL_H
#define STAGE_DIALOG_TEST_CONTROL_H


#include "stage/gtk-base.hpp"
#include "stage/dialog/dialog.hpp"
#include "stage/ctrl/bus-term.hpp"
#include "stage/model/tangible.hpp"
#include "include/ui-protocol.hpp"

#include "steam/cmd.hpp"
#include "include/gui-notification-facade.h"
#include "steam/mobject/session/dummy-session-connection.hpp"

#include "lib/test/test-helper.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/scoped-ptrvect.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>

namespace stage {
namespace dialog {
  
  using std::string;
  using std::forward;
  using lib::diff::GenNode;
  using util::sanitise;
  using util::_Fmt;
  
  
  /**
   * Building block for a segment within a dialog page.
   * This helper widget provides the typical sub section of a dialog
   * with several child widgets stacked vertically and enclosed within
   * a frame with a label. The frame serves as the parent widget, as far
   * as the widget hierarchy is concerned. Both parts are publicly accessible
   * as members, while providing a shortcut for publishing the box.
   */
  struct FrameBox
    {
      Gtk::Frame frame;
      Gtk::Box   box;
      
      FrameBox (cuString label,
                 Gtk::Orientation orientation =Gtk::ORIENTATION_VERTICAL)
        : frame{label}
        , box{orientation}
        {
          frame.add (box);
        }
      
      operator Gtk::Widget&() { return frame; }
      
      void
      pack_start(Gtk::Widget& child, Gtk::PackOptions options =Gtk::PACK_EXPAND_WIDGET, guint padding =0)
        {
          box.pack_start (child, options, padding);
        }
    };
  
  /** explicitly named shortcut for the typical dialog page content holder */
  class Page
    : public Gtk::Box
    , util::NonCopyable
    {
    public:
      Page()
        : Gtk::Box{Gtk::ORIENTATION_VERTICAL}
        { }
    };
  
  /**
   * Helper widget to simplify construction and wiring of a [Notebook] widget.
   * Gtk::Notebook is quite powerful container foundation to build complex dialog widgets with
   * multiple pages on tabs. However, the construction, wiring and setup is notoriously tedious,
   * due to the repetitiveness and the sheer amount of child widgets spread over various pages.
   * 
   * This design draft is an attempt to mitigate the required boilerplate, without overly much
   * obscuring the structure. The basic idea is to package each page into a locally defined child
   * struct, which is actually heap allocated and managed automatically. This way, each child page
   * gets its own namespace, and wiring to other components is made explicit by passing named ctor
   * arguments -- while the overall structure  of building and wiring of widgets stays close to the
   * habits of [programming with GTKmm](https://developer.gnome.org/gtkmm-tutorial/stable/).
   * - define the pages as custom widgets, typically just as locally known struct types
   * - invoke #buildPage passing the type and tab label for each page
   * - define the wiring of the components within a page in the page's ctor
   * - possibly pass external dependencies for wiring into that ctor
   * @note the page widgets are actually heap allocated and managed automatically
   * @see stage::dialog::TestControl as a usage example
   * 
   * [Notebook]: https://developer.gnome.org/gtkmm-tutorial/stable/sec-multi-item-containers.html.en#sec-notebook
   */
  class Notebook
    : public Gtk::Notebook
    , lib::ScopedPtrVect<Gtk::Widget>
    {
    public:
      template<class PAG, typename...ARGS>
      Notebook&
      buildPage (cuString pageLabel, ARGS&& ...ctorArgs)
        {
          Gtk::Notebook::append_page (this->manage (new PAG(forward<ARGS> (ctorArgs)...))
                                     ,pageLabel);
          return *this;
        }
    };
  
  
  /**
   * A complex, tabbed-notebook-style non-modal dialog window,
   * dedicated to development, diagnostics and experimentation.
   * The TestControl can be launched from Lumiera's "Help" menu,
   * offers a (passive, up-link) [UI-Bus connection](\ref ui-bus.hpp)
   * and simplifies adding pages for occasional experiments and diagnostics.
   */
  class TestControl
    : public Gtk::Dialog
    {
      using Bus = ctrl::BusTerm&;
      
      Bus uiBus_;
      Notebook notebook_;
      
      
      /**
       * Ticket #1099 : perform a dummy round-trip to verify Steam-Stage integration.
       * This routine invokes the command `test_meta_displayInfo` and friends down in Steam-Layer,
       * passing the settings from the radio buttons to select the flavour of feedback, and the text
       * for feedback content. The expected behaviour is for the invoked command to send a feedback
       * via UI-Bus towards the ErrorLogDisplay within the InfoboxPanel.
       */
      struct Page1 : Page
        {
          Gtk::Entry content_;
          FrameBox seg_1_{_("log notification"), Gtk::ORIENTATION_HORIZONTAL},
                   seg_2_{_("mark via UI-Bus")};
          Gtk::Button trig_1_, trig_2_, trig_3_, trig_4_;
          Gtk::RadioButton level_info_{"Info"},
                           level_warn_{"Warn"},
                           level_erro_{"Error"};
          Gtk::Box         markParam_;
          Gtk::ComboBoxText actionID_{true};  // has free-text entry field
          
          int
          getLogLevel()
            {
              return level_info_.get_active()? NOTE_INFO :
                     level_warn_.get_active()? NOTE_WARN : NOTE_ERROR;
            }
          
          string
          getContent()
            {
              return string{content_.get_text()};
            }
          
          string
          getActionID()
            {
              return string{actionID_.get_entry_text()};
            }
          
          
          Page1 (Bus bus)
            {
              content_.set_tooltip_markup (_("<b>Ticket #1099</b>:\n"
                                             "text message content\n"
                                             "<i>when invoking a suitable action,\n"
                                             "it will be passed down and sent back</i>"));
              trig_1_.set_use_underline();
              trig_1_.set_label ("_display text");
              trig_1_.property_xalign() = 0;
              trig_1_.set_tooltip_markup (_("Trigger Steam-Stage <b>roundtrip</b>\n"
                                            "Steam invokes GuiNotification::displayInfo"));
              
              level_warn_.join_group(level_info_);
              level_erro_.join_group(level_info_);
              level_warn_.set_active();

              trig_2_.set_use_underline();
              trig_2_.property_xalign() = 0;
              trig_2_.set_label ("mark _error");
              trig_2_.set_tooltip_markup (_("trigger Steam-command, which in turn\n"
                                            "sends an error state mark via UI-Bus"));
              
              trig_3_.set_use_underline();
              trig_3_.property_xalign() = 0;
              trig_3_.set_label ("mark _info");
              trig_3_.set_tooltip_markup (_("trigger Steam-command, which in turn\n"
                                            "sends an info state mark via UI-Bus"));
              
              trig_4_.set_use_underline();
              trig_4_.set_label ("_mark");
              trig_4_.property_xalign() = 0;
              trig_4_.set_tooltip_markup (_("trigger Steam-command, which in turn\n"
                                            "sends an <b>state mark</b> message, using\n"
                                            "the message action-ID from the combobox"));
              actionID_.append (cuString{MARK_Flash});
              actionID_.append (cuString{MARK_reveal});
              actionID_.append (cuString{MARK_clearErr});
              actionID_.append (cuString{MARK_clearMsg});
              actionID_.append (cuString{MARK_expand});
              actionID_.append (cuString{MARK_reset});
              actionID_.set_active(1);
              actionID_.set_tooltip_markup("select the specific action-ID\n"
                                           "when sending a <b>mark</b> message.\n"
                                           "<u>note</u>: can enter arbitrary ID");
              
              markParam_.pack_start(trig_4_);
              markParam_.pack_start(actionID_, Gtk::PACK_SHRINK);
              
              seg_1_.pack_start (trig_1_, Gtk::PACK_EXPAND_WIDGET);
              seg_1_.pack_start (level_info_, Gtk::PACK_SHRINK);
              seg_1_.pack_start (level_warn_, Gtk::PACK_SHRINK);
              seg_1_.pack_start (level_erro_, Gtk::PACK_SHRINK);
              
              seg_2_.pack_start (trig_2_);
              seg_2_.pack_start (trig_3_);
              seg_2_.pack_start (markParam_);
              
              pack_start (content_, Gtk::PACK_SHRINK);
              pack_start (seg_1_,   Gtk::PACK_SHRINK);
              pack_start (seg_2_,   Gtk::PACK_SHRINK);
              
              // define the action triggers...
              trig_1_.signal_clicked().connect(
                          [&]{ bus.act (model::commandMessage (steam::cmd::test_meta_displayInfo, getLogLevel(), getContent())); });
              trig_2_.signal_clicked().connect(
                          [&]{ bus.act (model::commandMessage (steam::cmd::test_meta_markError,   getContent()));                });
              trig_3_.signal_clicked().connect(
                          [&]{ bus.act (model::commandMessage (steam::cmd::test_meta_markNote,    getContent()));                });
              trig_4_.signal_clicked().connect(
                          [&]{ bus.act (model::commandMessage (steam::cmd::test_meta_markAction,  getActionID(), getContent())); });
            }
        };
      
      
      /**
       * Ticket #1014 : populate the Timeline in the GUI.
       * This page allows to send various diff messages up into the UI, to provide timeline content.
       */
      struct Page2 : Page
        {
          Gtk::Entry dummy_;
          FrameBox part_1_{_("populate"), Gtk::ORIENTATION_HORIZONTAL}
                 , part_2_{_("modify content")}
                 ;
          Gtk::Button seq_1_, seq_2_;
          Gtk::Button mut_1_;
          
          string
          pickDummyID()
            {
              string dummyID = sanitise (dummy_.get_text());
              dummy_.set_text (string{_Fmt{"d%s%02d"} % lib::test::randStr(2) % (1 + rand() % 99)});
              return dummyID;
            }
          
          
          Page2 (Bus bus)
            {
              seq_1_.set_label ("Sequence 1");
              seq_1_.set_tooltip_markup (_("Push <b>population diff</b> up into the UI\n"
                                           "provides the typical simple default timeline structure"));
              
              seq_2_.set_label ("Sequence 2");
              seq_2_.set_tooltip_markup (_("Push <b>population diff</b> up into the UI\n"
                                           "provides a complex nested timeline structure"));
              
              dummy_.set_tooltip_markup (_("<i>dummy ID value</i>\n"
                                           "used to build names in the generated content"));
              dummy_.set_max_width_chars(12);
              pickDummyID();
              
              part_1_.pack_start (seq_1_, Gtk::PACK_SHRINK);
              part_1_.pack_start (seq_2_, Gtk::PACK_SHRINK);
              part_1_.pack_start (dummy_, Gtk::PACK_EXPAND_WIDGET);
              
              
              mut_1_.set_label ("move elements");
              mut_1_.set_tooltip_markup (_("randomly manipulate temporal position of dummy elements"));
              
              part_2_.pack_start (mut_1_);
              
              pack_start (part_1_, Gtk::PACK_SHRINK);
              pack_start (part_2_, Gtk::PACK_SHRINK);
              
              // define the action triggers...
              seq_1_.signal_clicked().connect(
                          [&]{ bus.act (model::commandMessage (steam::cmd::test_fake_injectSequence_1, pickDummyID())); });
              seq_2_.signal_clicked().connect(
                          [&]{ bus.act (model::commandMessage (steam::cmd::test_fake_injectSequence_2, pickDummyID())); });
              
              mut_1_.signal_clicked().connect(
                          [&]{ cerr << "gelldaschaugst..." <<endl; });
            }
        };
      
      
      
    public:
      TestControl (ctrl::BusTerm& upLink, Gtk::Window& parent)
        : Dialog(_("Test and Diagnostics"), parent, Gtk::DIALOG_DESTROY_WITH_PARENT)
        , uiBus_{upLink}
        {
          // Setup the overall dialog layout
          set_border_width (BorderPadding);
          get_content_area()->pack_start (notebook_);
          
          // construct and wire the pages...
          notebook_.buildPage<Page1> (_("#1099"), uiBus_);
          notebook_.buildPage<Page2> (_("Populate"), uiBus_);
          
          show_all();
          notebook_.set_current_page(-1);
        }
    };
  
  
}} // namespace stage::dialog
#endif /*STAGE_DIALOG_TEST_CONTROL_H*/
