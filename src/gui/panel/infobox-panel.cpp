/*
  InfoBoxPanel  -  A dockable panel to expose information and parameters

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

* *****************************************************/


/** @file infobox-panel.cpp
 ** Implementation of a (dockable) panel to display and manipulate parameters.
 */

#include "gui/gtk-base.hpp"
#include "gui/panel/infobox-panel.hpp"
#include "lib/format-string.hpp"

using util::_Fmt;
using Glib::ustring;

namespace gui  {
namespace panel{
  
  InfoBoxPanel::InfoBoxPanel (workspace::PanelManager& panelManager, Gdl::DockItem& dockItem)
    : Panel(panelManager, dockItem, getTitle(), getStockID())
    , twoParts_{Gtk::ORIENTATION_VERTICAL}
    , buttons_{}
    , frame_{"UI Integration Experiments"}
    , scroller_{}
    , textLog_{}
    {
      twoParts_.pack_start(frame_);
      twoParts_.pack_start(buttons_, Gtk::PACK_SHRINK);
      
      buttons_.set_layout (Gtk::BUTTONBOX_START);
      
      // buttons to trigger experiments
      button_1_.set_label ("_bang");
      button_1_.set_use_underline();
      button_1_.set_tooltip_markup ("<b>Experiment 1</b>:\ntrigger Proc-GUI roundtrip");
      button_1_.signal_clicked().connect(
                  mem_fun(*this, &InfoBoxPanel::experiment_1));
      buttons_.add (button_1_);
      //(End)buttons...
      
      frame_.add (scroller_);
      frame_.set_border_width (5);
      
      scroller_.set_shadow_type (Gtk::SHADOW_NONE);
      scroller_.set_border_width (10);
      
      // the vertical scrollbar will always be necessary....
      scroller_.set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
      scroller_.add (textLog_);
      
      textLog_.set_editable (false);
      
      // show everything....
      this->add (twoParts_);
      this->show_all();
    }
  
  const char*
  InfoBoxPanel::getTitle()
  {
    return _("InfoBox");
  }
  
  const gchar*
  InfoBoxPanel::getStockID()
  {
    return "panel_infobox";
  }
  
  
  
  void
  InfoBoxPanel::experiment_1()
  {
    frame_.set_label("Experiment 1... BANG");
    
    static uint bangNo{0};
    static _Fmt msgTemplate{"Bang #%d\n"};
    
    // According to the Gtkmm tutorial, TextView::scroll_to(iter) is not reliable;
    // rather we need to use a text mark and set that text mark to the insert position.
    // Actually, there is always one predefined text mark called "insert", which corresponds
    // to the text cursor. Thus it suffices to navigate to text end, insert and scroll into view.
    auto buff = textLog_.get_buffer();
    auto cursor = buff->get_insert();
    buff->move_mark (cursor, buff->end());
    buff->insert (buff->end(), ustring{msgTemplate % ++bangNo});
    textLog_.scroll_to (cursor);
  }
  
  
}}// namespace gui::panel
