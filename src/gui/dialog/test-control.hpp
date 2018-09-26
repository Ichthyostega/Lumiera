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
 ** @todo as of 9/2018, this is a first rough draft, relevant for #1099          ////////////////////////////TICKET #1074 gradually augment the self-diagnostics controls in the UI
 ** @todo this header also features a design draft how to simplify building notebook widgets.
 **       Which could be polished and moved into a separate utility header eventually.
 */


#ifndef GUI_DIALOG_TEST_CONTROL_H
#define GUI_DIALOG_TEST_CONTROL_H


#include "gui/gtk-base.hpp"
#include "gui/dialog/dialog.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "lib/scoped-ptrvect.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/nocopy.hpp"

#include <utility>

#if true  /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1099 : WIP-WIP-WIP
namespace proc {
  namespace asset {
    namespace meta {
      class ErrorLog;
      
      extern lib::idi::EntryID<ErrorLog> theErrorLog_ID;
} } }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1099 : WIP-WIP-WIP
namespace gui {
namespace dialog {
  
  using std::forward;
  using lib::diff::GenNode;
  
  
  /**
   * Building block for a segment within a dialog page.
   * This helper widget provides the typical sub section of a dialog
   * with several child widgets stacked vertically and enclosed within
   * a frame with a label. The frame serves as the parent widget, as far
   * as the widget hierarchy is concerned. Both parts are publicly accessible
   * as members, while providing a shortcut for publishing the box.
   */
  struct FrameVBox
    {
      Gtk::Frame frame;
      Gtk::Box   box;
      
      FrameVBox (cuString label)
        : frame{label}
        , box{Gtk::ORIENTATION_VERTICAL}
        {
          frame.add (box);
        }
      
      operator Gtk::Widget&() { return frame; }
      
      void
      pack_start(Gtk::Widget& child, Gtk::PackOptions options = Gtk::PACK_EXPAND_WIDGET, guint padding = 0)
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
   * multiple pages on tabs. However, the construction, wiring an setup is notoriously tedious,
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
   * @see gui::dialog::TestControl as a usage example
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
   * A complex, tabbed-notebook style non-modal dialog window,
   * dedicated to development, diagnostics and experimentation.
   * The TestControl can be launched from Lumiera's "Help" menu,
   * offers an (passive, up-link) [UI-Bus connection](\ref ui-bus.hpp)
   * and simplifies adding pages for occasional experiments and diagnostics.
   */
  class TestControl
    : public Gtk::Dialog
    {
      using Bus = ctrl::BusTerm&;
      
      Bus uiBus_;
      Notebook notebook_;
      
      
      /**
       * Ticket #1099 : perform a dummy round-trip to verify Proc-GUI integration.
       * This routine invokes the command 'xxx' down in Proc-Layer, passing the settings
       * from the radio buttons to select the flavour of feedback, and the text for feedback content.
       * The expected behaviour is for the invoked command to send a feedback via UI-Bus towards
       * the ErrorLogDisplay within the InfoboxPanel.
       */
      struct Page1 : Page
        {
          FrameVBox seg_1_{_("log notification")},
                    seg_2_{_("mark via UI-Bus")};
          Gtk::Button trigger_1_;
          
          Page1(Bus bus)
            {
              trigger_1_.set_label ("_doIt");
              trigger_1_.set_use_underline();
              trigger_1_.set_tooltip_markup (_("<b>Ticket #1099</b>:\ntrigger Proc-GUI roundtrip"));
              trigger_1_.signal_clicked().connect(
                          [&]{ demoGuiRoundtrip(bus); });
              
              seg_1_.pack_start (trigger_1_, Gtk::PACK_SHRINK);
              pack_start (seg_1_);
              pack_start (seg_2_);
            }
          
          void
          demoGuiRoundtrip (Bus bus)
            {
              TODO ("collect command arguments and then send the command message for #1099");
              ID errorLogID = proc::asset::meta::theErrorLog_ID;
              bus.mark (errorLogID, GenNode{"Message", "Lalü"});
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
          
          show_all();
        }
    };
  
  
}} // namespace gui::dialog
#endif /*GUI_DIALOG_TEST_CONTROL_H*/
