/*
  gtk-lumiera.cpp  -  simple placeholder to make the compile work
 
* *****************************************************/


#ifndef GTK_LUMIERA_HPP
#define GTK_LUMIERA_HPP

#include <gtkmm.h>
#include <nobug.h>
#include <vector>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include "lib/util.hpp"

#include <libgdl-1.0/gdl/gdl-dock-layout.h>

extern "C" {
#include <gavl/gavltime.h>
}

NOBUG_DECLARE_FLAG(gui);

#ifdef ENABLE_NLS
#  include <libintl.h>
#  define _(String) gettext (String)
#  define gettext_noop(String) String
#  define N_(String) gettext_noop (String)
#else
#  define _(String) (String)
#  define N_(String) String
#  define textdomain(Domain)
#  define bindtextdomain(Package, Directory)
#endif

/**
 * The namespace of all GUI code.
 */
namespace gui {
  
/* ===== Global Constants ===== */

/**
 *  The name of the application 
 */
static const gchar* AppTitle = "Lumiera";


  
/* ===== The Application Class ===== */

/**
 *  The main application class.
 */
class GtkLumiera : private boost::noncopyable
{
public:
  int main(int argc, char *argv[]);
  

};

/**
 *  Returns a reference to the global application object
 */
GtkLumiera& application();



namespace workspace {

class WorkspaceWindow;

/**
* A helper class which registers and handles
* user action events.
*/
class Actions
{
private:	
  Actions(WorkspaceWindow &workspace_window);

  /* ===== Internals ===== */
private:
  /**
   * Updates the state of the menu/toolbar actions
   * to reflect the current state of the workspace */
  void update_action_state();

  /** A reference to the MainWindow */
  WorkspaceWindow &workspaceWindow;

  /* ===== Event Handlers ===== */
private:
  void on_menu_file_quit();
  void on_menu_help_about();
  
  // Temporary Junk
  void on_menu_others();


  /* ===== Actions ===== */
private:
  Glib::RefPtr<Gtk::ActionGroup> actionGroup;

  /* ===== Internals ===== */
private:
  bool is_updating_action_state;
  
  friend class WorkspaceWindow;
};



/** 
* A Mock main window
*/
class WorkspaceWindow : public Gtk::Window
{
public:
  WorkspaceWindow();
  
  ~WorkspaceWindow();

private:
  void create_ui();


  /* ===== UI ===== */
private:
  Glib::RefPtr<Gtk::UIManager> uiManager;
  Gtk::VBox baseContainer;
  Gtk::HBox dockContainer;
  
  //----- Dock Frame -----//
  Gtk::Widget *dock;
  Gtk::Widget *dockbar;
  GdlDockLayout *layout;
  
  //----- Status Bar -----//
  Gtk::Statusbar statusBar;

  /** instance of the actions helper class, which
   * registers and handles user action events */
  Actions actions;

  friend class Actions;
  
};





}   // namespace workspace


}   // namespace gui

#endif // GTK_LUMIERA_HPP


