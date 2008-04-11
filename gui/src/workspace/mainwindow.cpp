//           main-window.cpp
//  Tue Apr  8 23:54:36 2008
//  Copyright  2008  joel
//  <joel@airwebreathe.org.uk>

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA

#include <gtkmm/stock.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include "gtk-lumiera.hpp"
#include "mainwindow.hpp"

namespace Lumiera {
namespace Workspace {
	
MainWindow::MainWindow()
    : _actions(*this)
{
    create_ui();
}

MainWindow::~MainWindow()
{
}

void MainWindow::create_ui()
{
    //----- Configure the Window -----//
    set_title(AppTitle);
    set_default_size(1024, 768);

    // The UI will be nested within a VBOX
    add(_box);

    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(_actions._action_group);

    add_accel_group(m_refUIManager->get_accel_group());

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
        m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
        g_error("building menus failed: ");
        return;
    }
#else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if(ex.get())
    {
        g_error("building menus failed: ");
        return;
    }
#endif //GLIBMM_EXCEPTIONS_ENABLED

    // Get the menubar and toolbar widgets, and add them to a container widget:
    Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
    if(pMenubar)
        _box.pack_start(*pMenubar, Gtk::PACK_SHRINK);

    Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar") ;
    if(pToolbar)
        _box.pack_start(*pToolbar, Gtk::PACK_SHRINK);

    show_all_children();
}

}   // namespace Workspace
}   // namespace Lumiera

