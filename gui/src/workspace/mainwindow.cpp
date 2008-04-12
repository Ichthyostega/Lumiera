/*
  MainWindow.cpp  -  Definition of the main workspace window object
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/

#include <gtkmm/stock.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include "gtk-lumiera.hpp"
#include "mainwindow.hpp"

namespace lumiera {
namespace gui {
namespace workspace {
	
  MainWindow::MainWindow()
    : actions(*this)
  {
    create_ui();
  }

  MainWindow::~MainWindow()
  {
  }

  void
  MainWindow::create_ui()
  {
    //----- Configure the Window -----//
    set_title(AppTitle);
    set_default_size(1024, 768);

    // The UI will be nested within a VBOX
    add(box);

    uiManager = Gtk::UIManager::create();
    uiManager->insert_action_group(actions.actionGroup);

    add_accel_group(uiManager->get_accel_group());

    //Layout the actions in a menubar and toolbar:
    Glib::ustring ui_info = 
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='FileMenu'>"
        "      <menu action='FileNew'>"
        "        <menuitem action='FileNewStandard'/>"
        "        <menuitem action='FileNewFoo'/>"
        "        <menuitem action='FileNewGoo'/>"
        "      </menu>"
        "      <separator/>"
        "      <menuitem action='FileRender'/>"
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='EditMenu'>"
        "      <menuitem action='EditCopy'/>"
        "      <menuitem action='EditPaste'/>"
        "      <menuitem action='EditSomething'/>"
        "    </menu>"
        "    <menu action='ChoicesMenu'>"
        "      <menuitem action='ChoiceOne'/>"
        "      <menuitem action='ChoiceTwo'/>"
        "    </menu>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='HelpAbout'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action='FileNewStandard'/>"
        "  </toolbar>"
        "</ui>";

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try
    {
      uiManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
      g_error("building menus failed: ");
      return;
    }
#else
    std::auto_ptr<Glib::Error> ex;
    uiManager->add_ui_from_string(ui_info, ex);
    if(ex.get())
    {
      g_error("building menus failed: ");
      return;
    }
#endif //GLIBMM_EXCEPTIONS_ENABLED

    // Get the menubar and toolbar widgets, and add them to a container widget:
    Gtk::Widget* menu_bar = uiManager->get_widget("/MenuBar");
    if(menu_bar) box.pack_start(*menu_bar, Gtk::PACK_SHRINK);

    Gtk::Widget* toolbar = uiManager->get_widget("/ToolBar") ;
    if(toolbar) box.pack_start(*toolbar, Gtk::PACK_SHRINK);

    show_all_children();
  }

}   // namespace workspace
}   // namespace gui
}   // namespace lumiera

