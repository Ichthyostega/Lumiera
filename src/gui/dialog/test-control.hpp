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
 */


#ifndef GUI_DIALOG_TEST_CONTROL_H
#define GUI_DIALOG_TEST_CONTROL_H


#include "gui/gtk-base.hpp"
#include "gui/dialog/dialog.hpp"

namespace gui {
namespace dialog {
  
  
  class TestControl
    : public Gtk::Dialog
    {
      Gtk::Notebook notebook_;
      
      Gtk::Box page_1_;
      Gtk::Button trigger_1_;
      
    public:
      TestControl (Gtk::Window &parent)
        : Dialog(_("Test and Diagnostics"), parent, Gtk::DIALOG_DESTROY_WITH_PARENT)
        , page_1_{Gtk::ORIENTATION_VERTICAL}
        {
          using namespace Gtk;
          
          // Setup the overall dialog layout
          set_border_width (BorderPadding);
          get_content_area()->pack_start (notebook_);
          
          //-----Page-1------------------------------
          trigger_1_.set_label ("_doIt");
          trigger_1_.set_use_underline();
          trigger_1_.set_tooltip_markup (_("<b>Ticket #1099</b>:\ntrigger Proc-GUI roundtrip"));
          trigger_1_.signal_clicked().connect(
                      mem_fun(*this, &TestControl::demoGuiRoundtrip));
          
          page_1_.pack_start (trigger_1_, PACK_SHRINK);
          notebook_.append_page (page_1_, _("#1099"));
          
          
          show_all_children();
        }
      
        
    private: /* ==== individual Test/Diagnostics routines ==== */
      
      /**
       * Ticket #1099 : perform a dummy round-trip to verify Proc-GUI integration.
       * This routine invokes the command 'xxx' down in Proc-Layer, passing the settings
       * from the radio buttons to select the flavour of feedback, and the text for feedback content.
       * The expected behaviour is for the invoked command to send a feedback via UI-Bus towards
       * the ErrorLogDisplay within the InfoboxPanel.
       */
      void
      demoGuiRoundtrip()
        {
          UNIMPLEMENTED ("collect command arguments and then send the command message for #1099");
        }
    };
  
  
}} // namespace gui::dialog
#endif /*GUI_DIALOG_TEST_CONTROL_H*/
