/*
  panel-manager.hpp  -  Definition of the panel manager object
 
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
 
*/
/** @file panel-manager.hpp
 ** This file contains the definition of the panel manager object.
 **
 ** @see actions.hpp
 */

#ifndef PANEL_MANAGER_HPP
#define PANEL_MANAGER_HPP

#include <libgdl-1.0/gdl/gdl.h>

#include "../panels/panel.hpp"

using namespace gui::panels;

namespace gui {
namespace workspace {

/**
 * A class to managers DockItem objects for WorkspaceWindow.
 **/
class PanelManager
{
public:

  /**
   * Constructor
   * @param workspace_window A reference to the owner WorkspaceWindow
   * object.
   **/
  PanelManager(WorkspaceWindow &workspace_window);
  
  /**
   * Destructor.
   **/
  ~PanelManager();
  
  /**
   * Initializes this dock manager and creates the dock and all it's
   * widgets.
   * @remarks This function must be called only once as the first call
   * after construction.
   **/
  void setup_dock();
  
  /**
   * Gets a pointer to the dock object.
   * @remarks Note that this must not be called before setup_dock.
   **/
  GdlDock* get_dock() const;
  
  /**
   * Gets a pointer to the dock bar.
   * @remarks Note that this must not be called before setup_dock.
   **/
  GdlDockBar* get_dock_bar() const;

private:

  /**
   * Creates the standard panel layout.
   **/
  void create_panels();

  /**
   * Creates a panel by class name.
   * @param class_name The name of the object class to create.
   **/
  boost::shared_ptr<panels::Panel> create_panel_by_name(
    const char* class_name);

private:
  
  /**
   * A reference to the owner workspace window object.
   **/
  WorkspaceWindow &workspaceWindow;
  
  /**
   * The pointer to GDL dock widget.
   * @remarks This value is NULL until setup_dock has been called.
   **/
  GdlDock *dock;
  
  /**
   * The pointer to GDL dock bar widget.
   * @remarks This value is NULL until setup_dock has been called.
   **/
  GdlDockBar *dockBar;
  
  /**
   * The pointer to GDL dock layout object.
   * @remarks This value is NULL until setup_dock has been called.
   **/
  GdlDockLayout *dockLayout;

  /**
   * Pointers to the 4 root place holders.
   * @remarks All 4 entries are NULL until setup_dock has been called.
   **/
  GdlDockPlaceholder *dockPlaceholders[4];

  /**
   * The list of created panels.
   **/
  std::list< boost::shared_ptr<panels::Panel> > panels;

private:
  
  /**
   * A class to describe and instantiate Panel types.
   **/
  class PanelDescription
    {
    protected:
      /**
       * Constructor
       * @param class_name The name of the Panel class
       * @param title The localized title that will be shown on the
       * panel.
       * @param create_panel_proc A pointer to a function that will
       * instantiate the panel object.
       **/
      PanelDescription(const char* class_name, const char *title,
        boost::shared_ptr<panels::Panel> (*const create_panel_proc)(
          WorkspaceWindow&)) :
        className(class_name),
        titleName(title),
        createPanelProc(create_panel_proc)
        {
          REQUIRE(className);
          REQUIRE(titleName);
        }
        
    public:
      /**
       * Returns a pointer to the string name of class.
       **/
      const char* get_class_name() const
        {
          ENSURE(className);
          return className;
        }
      
      /**
       * Returns the localized title that will be shown on the panel.
       **/
      const char* get_title() const
        {
          ENSURE(titleName);
          return titleName;
        }
    
      /**
       * Creates an instance of this panel.
       **/
      boost::shared_ptr<panels::Panel> create(
        WorkspaceWindow& owner_window) const
        {
          REQUIRE(createPanelProc);
          return createPanelProc(owner_window);
        }
      
    private:
      /**
       * A pointer to the string name of class.
       **/
      const char* className;
      
      /**
       * The localized title that will be shown on the panel.
       **/
      const char* titleName;
      
      /**
       * A pointer to a function that will instantiate the panel object.
       **/
      boost::shared_ptr<panels::Panel> (*const createPanelProc)(
        WorkspaceWindow&);
    };
  
  /**
   * A helper class that will create PanelDescription objects.
   * @param P The type of panels::Panel that the PanelDescription will 
   * describe.
   **/
  template<class P> class Panel : public PanelDescription
    {
    public:
      /**
       * Constructor
       **/
      Panel() :
        PanelDescription(typeid(P).name(), P::get_title(),
          Panel::create_panel)
        {}
      
    private:
      /**
       * A helper function that will create a panel of type P
       * @param workspace_window The owner workspace window.
       * @return Returns a shared pointer to the panel object.
       **/
      static boost::shared_ptr<panels::Panel> create_panel(
        WorkspaceWindow &workspace_window)
          {
            return boost::shared_ptr<panels::Panel>(
              new P(workspace_window));
          }
    };
  
  /**
   * The list of panel descriptions.
   **/
  static const PanelDescription panelDescriptionList[];
};

}   // namespace workspace
}   // namespace gui

#endif // PANEL_MANAGER_HPP
