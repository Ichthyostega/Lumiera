/*
  gtk-lumiera.cpp  -  simple placeholder to make the compile work
 
* *****************************************************/

#include <gtkmm.h>
#include <nobug.h>

#include <gtkmm/stock.h>

using namespace Gtk;


#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include <libgdl-1.0/gdl/gdl-tools.h>
#include <libgdl-1.0/gdl/gdl-dock.h>
#include <libgdl-1.0/gdl/gdl-dock-item.h>
#include <libgdl-1.0/gdl/gdl-dock-placeholder.h>
#include <libgdl-1.0/gdl/gdl-dock-bar.h>

#include "gtk-lumiera.hpp"

NOBUG_CPP_DEFINE_FLAG(gui);

using namespace Gtk;
using namespace Glib;
using namespace gui;

GtkLumiera the_application;

int
main (int argc, char *argv[])
{
  return the_application.main(argc, argv);
}

extern "C"
void
start_dummy_gui ()
{
  NOTICE(gui, "This is a placeholder for the Lumiera GTK-GUI starting....");
}






namespace gui {

int
GtkLumiera::main(int argc, char *argv[])
{
  NOBUG_INIT;

  Main kit(argc, argv);
  
  Glib::set_application_name(AppTitle);

  workspace::WorkspaceWindow main_window;

  kit.run(main_window); 
  
  return 23;
}


GtkLumiera&
application()
{
  return the_application;  
}


namespace workspace {
	
WorkspaceWindow::WorkspaceWindow()
: actions(*this)
{
    
  layout = NULL;
  create_ui();
}

WorkspaceWindow::~WorkspaceWindow()
{
  REQUIRE(layout != NULL);
  g_object_unref(layout);
}


void
WorkspaceWindow::create_ui()
{    
  //----- Configure the Window -----//
  set_title(AppTitle);
  set_default_size(1024, 768);

  //----- Set up the UI Manager -----//
  // The UI will be nested within a VBox
  add(baseContainer);

  uiManager = Gtk::UIManager::create();
  uiManager->insert_action_group(actions.actionGroup);

  //Layout the actions in a menubar and toolbar:
  Glib::ustring ui_info = 
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='FileMenu'>"
      "      <menuitem action='FileQuit'/>"
      "    </menu>"
      "    <menu action='HelpMenu'>"
      "      <menuitem action='HelpAbout'/>"
      "    </menu>"
      "  </menubar>"
      "  <toolbar  name='ToolBar'>"
      "  </toolbar>"
      "</ui>";

  try
    {
      uiManager->add_ui_from_string(ui_info);
    }
  catch(const Glib::Error& ex)
    {
      ERROR(gui, "Building menus failed: %s", ex.what().data());
      return;
    }

  //----- Set up the Menu Bar -----//
  Gtk::Widget* menu_bar = uiManager->get_widget("/MenuBar");
  ASSERT(menu_bar != NULL);
  baseContainer.pack_start(*menu_bar, Gtk::PACK_SHRINK);
  
  //----- Set up the Tool Bar -----//
  Gtk::Toolbar* toolbar = dynamic_cast<Gtk::Toolbar*>(
    uiManager->get_widget("/ToolBar"));
  ASSERT(toolbar != NULL);
  toolbar->set_toolbar_style(TOOLBAR_ICONS);
  baseContainer.pack_start(*toolbar, Gtk::PACK_SHRINK);
  

  //----- Create the Dock -----//
  dock = Glib::wrap(gdl_dock_new());
  layout = gdl_dock_layout_new((GdlDock*)dock->gobj());
  
  dockbar = Glib::wrap(gdl_dock_bar_new ((GdlDock*)dock->gobj()));

  dockContainer.pack_start(*dockbar, PACK_SHRINK);
  dockContainer.pack_end(*dock, PACK_EXPAND_WIDGET);
  baseContainer.pack_start(dockContainer, PACK_EXPAND_WIDGET);
    
  //----- Create the status bar -----//
  statusBar.set_has_resize_grip();
  baseContainer.pack_start(statusBar, PACK_SHRINK);
 
  show_all_children();
  

}


  /* -- Actions -- */ 


Actions::Actions(WorkspaceWindow &workspace_window) :
  workspaceWindow(workspace_window),
  is_updating_action_state(false)
{
  workspace_window.signal_show ().connect_notify(sigc::mem_fun(this, &Actions::update_action_state));

  //----- Create the Action Group -----//
  actionGroup = ActionGroup::create();
  
  // File menu
  actionGroup->add(Action::create("FileMenu", _("_File")));
  actionGroup->add(Action::create("FileQuit", Stock::QUIT),
    sigc::mem_fun(*this, &Actions::on_menu_file_quit));

  // Help Menu
  actionGroup->add(Action::create("HelpMenu", _("_Help")) );
  actionGroup->add(Action::create("HelpAbout", _("say hello...")),
  sigc::mem_fun(*this, &Actions::on_menu_help_about) );
}

void
Actions::update_action_state()
{
  is_updating_action_state = true;
  is_updating_action_state = false;
}

/* ===== File Menu Event Handlers ===== */

void
Actions::on_menu_file_quit()
{
  workspaceWindow.hide(); // Closes the main window to stop the Gtk::Main::run().
}

/* ===== Help Menu Event Handlers ===== */

void
Actions::on_menu_help_about()
{
  g_message("Hello Lumi World");
}



}   // namespace workspace



}   // namespace gui


