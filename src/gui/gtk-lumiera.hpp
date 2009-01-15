/*
  gtk-lumiera.cpp  -  simple placeholder to make the compile work
 
* *****************************************************/


#ifndef GTK_LUMIERA_HPP
#define GTK_LUMIERA_HPP

#include <gtkmm.h>
#include <nobug.h>               // need to include this after gtkmm.h, because types.h from GTK tries to shaddow the ERROR macro from windows, which kills NoBug's ERROR macro
#include <vector>
#include <boost/utility.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>

extern "C" {
#include <gavl/gavltime.h>


/** Dummy function just to demonstrate loading the GUI as 
 *  shared module from the SCons build.  */
void start_dummy_gui ();

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
  


  
/* ===== The Application Class ===== */

/**
 *  The main application class.
 */
class GtkLumiera : private boost::noncopyable
{
public:
  void main(int argc, char *argv[]);
  

/* ===== Global Constants ===== */
public:
/**
 *  The name of the application 
 */
static const gchar* AppTitle;
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


