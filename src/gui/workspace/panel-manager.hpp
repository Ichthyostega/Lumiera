/*
  panel-manager.hpp  -  Definition of the panel manager object

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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
/** @file panel-manager.hpp
 ** This file contains the definition of the panel manager object.
 **
 ** @see actions.hpp
 */

#ifndef PANEL_MANAGER_HPP
#define PANEL_MANAGER_HPP

#include <gdl/gdl.h>
#include <typeinfo>

#include "gui/panels/panel.hpp"

using namespace gui::panels;

namespace gui {
namespace workspace {

/**
 * A class to managers DockItem objects for WorkspaceWindow.
 */
class PanelManager
{
public:

  /**
   * Constructor
   * @param workspace_window A reference to the owner WorkspaceWindow
   * object.
   */
  PanelManager(WorkspaceWindow &workspace_window);
  
  /**
   * Destructor.
   */
  ~PanelManager();
  
  /**
   * Initializes this dock manager and creates the dock and all it's
   * widgets.
   * @remarks This function must be called only once as the first call
   * after construction.
   */
  void setup_dock();
  
  /**
   * Gets a pointer to the dock object.
   * @remarks Note that this must not be called before setup_dock.
   */
  GdlDock* get_dock() const;
  
  /**
   * Gets a pointer to the dock bar.
   * @remarks Note that this must not be called before setup_dock.
   */
  GdlDockBar* get_dock_bar() const;
  
  /**
   * Returns a reference to the owner workspace window.
   */
  WorkspaceWindow& get_workspace_window();
  
  /**
   * Shows a panel given a description index.
   * @param description_index The index of the panel type to show.
   */
  void show_panel(const int description_index);
  
  /**
   * Switches a panel from one type to another, without touching the
   * underlying GdlDockItem.
   * @param old_panel The panel which will be transofrmed to a new type.
   * @param description_index The index of the panel description that
   * will be instantiated.
   */
  void switch_panel(panels::Panel &old_panel,
    const int description_index);
  
  /**
   * Splits a panel into two panels of the same type.
   * @param panel The panel to split.
   * @param split_direction The direction to split the panel in.
   */
  void split_panel(panels::Panel &panel,
    Gtk::Orientation split_direction);

public:

  /**
   * Gets the number of panel descriptions.
   */
  static int get_panel_description_count();

  /**
   * Gets a panel description's stock id.
   * @param index The index of the panel to retrieve.
   * @return Returns the stock id of a panel at this index.
   */
  static const gchar* get_panel_stock_id(const int index);
  
  /**
   * Gets a panel description's title.
   * @param index The index of the panel to retrieve.
   * @return Returns the title of a panel at this index.
   */
  static const char* get_panel_title(int index);

private:

  /**
   * Creates the standard panel layout.
   */
  void create_panels();
  
  /**
   * Find the index of a panel description given the class name.
   * @param class_name The name of the object class to search for.
   * @return Returns the index of the panel description found, or -1
   * if no description was found for this type.
   */
  int find_panel_description(const char* class_name) const;
  
  /**
   * Creates a panel by description index.
   * @param index The index of the description to instantiate.
   * @return Returns a pointer to the new instantiated panel object.
   */
  panels::Panel* create_panel_by_index(const int index);
  
  /**
   * Creates a panel by description index with a given GdlDockItem
   * @param index The index of the description to instantiate.
   * @param dock_item The GdlDockItem to attach this panel to
   * @return Returns a pointer to the new instantiated panel object.
   */
  panels::Panel* create_panel_by_index(
    const int index, GdlDockItem *dock_item);

  /**
   * Creates a panel by class name.
   * @param class_name The name of the object class to create.
   * @return Returns a pointer to the new instantiated panel object.
   */
  panels::Panel* create_panel_by_name(const char* class_name);

  /**
   * Gets the type of a given panel.
   * @param panel The Panel to get the type of
   * @return Returns the index of the panel description found, or -1
   * if no description was found for this type.
   */
  int get_panel_type(panels::Panel* const panel) const;
  
  /**
   * Removes a panel from the panel list and deletes it.
   * @param panel The panel to remove and delete.
   */
  void remove_panel(panels::Panel* const panel);
  
  /**
   * Removes all panels from the panel list and deletes them.
   */
  void clear_panels();
  
private:
  
  /**
   * An event handler for when the panel is shown or hidden.
   * @param panel A pointer to the panel that was hidden.
   */
  void on_panel_shown(panels::Panel *panel);

private:
  
  /**
   * A reference to the owner workspace window object.
   */
  WorkspaceWindow &workspaceWindow;
  
  /**
   * The pointer to GDL dock widget.
   * @remarks This value is NULL until setup_dock has been called.
   */
  GdlDock *dock;
  
  /**
   * The pointer to GDL dock bar widget.
   * @remarks This value is NULL until setup_dock has been called.
   */
  GdlDockBar *dockBar;
  
  /**
   * The pointer to GDL dock layout object.
   * @remarks This value is NULL until setup_dock has been called.
   */
  GdlDockLayout *dockLayout;

  /**
   * Pointers to the 4 root place holders.
   * @remarks All 4 entries are NULL until setup_dock has been called.
   */
  GdlDockPlaceholder *dockPlaceholders[4];

  /**
   * The list of created panels.
   */
  std::list< panels::Panel* > panels;
  
  /**
   * An accumulator for the panel id.
   */
  static unsigned short panelID;

private:
  
  /**
   * A class to describe and instantiate Panel types.
   */
  class PanelDescription
    {
    protected:
    
      typedef panels::Panel* (*const CreatePanelProc)(
        PanelManager&, GdlDockItem*);
    
    protected:
      /**
       * Constructor
       * @param classInfo The typeid of the Panel class
       * @param title The localized title that will be shown on the
       * panel.
       * @param stock_id The Stock ID for this type of panel.
       * @param create_panel_proc A pointer to a function that will
       * instantiate the panel object.
       */
      PanelDescription(const std::type_info &class_info,
        const char *title, const gchar *stock_id,
        CreatePanelProc create_panel_proc) :
        classInfo(class_info),
        titleName(title),
        stockID(stock_id),
        createPanelProc(create_panel_proc)
        {
          REQUIRE(titleName);
        }
        
    public:
      /**
       * Returns a reference to the typeid of the class.
       */
      const std::type_info& get_class_info() const
        {
          return classInfo;
        }
    
      /**
       * Returns a pointer to the string name of the class.
       */
      const char* get_class_name() const
        {
          return classInfo.name();
        }
      
      /**
       * Returns the localized title that will be shown on the panel.
       */
      const char* get_title() const
        {
          ENSURE(titleName);
          return titleName;
        }
        
      /**
       * Returns the Stock ID for this type of panel.
       */
      const gchar* get_stock_id() const
        {
          ENSURE(stockID);
          return stockID;
        }
    
      /**
       * Creates an instance of this panel.
       * @param panel_manager The owner panel manager.
       * @param dock_item The GdlDockItem that will host this panel.
       * @return Returns a pointer to the panel object.
       */
      panels::Panel* create(
        PanelManager &panel_manager, GdlDockItem* dock_item) const
        {
          REQUIRE(createPanelProc);
          return createPanelProc(panel_manager, dock_item);
        }
      
    private:
      /**
       * A reference to the typeid of this class
       */
      const std::type_info &classInfo;
      
      /**
       * The localized title that will be shown on the panel.
       */
      const char* const titleName;
      
      /**
       * The Stock ID for this type of panel.
       */
      const gchar* const stockID;
      
      /**
       * A pointer to a function that will instantiate the panel object.
       */
      CreatePanelProc createPanelProc;
    };
  
  /**
   * A helper class that will create PanelDescription objects.
   * @param P The type of panels::Panel that the PanelDescription will 
   * describe.
   */
  template<class P> class Panel : public PanelDescription
    {
    public:
      /**
       * Constructor
       */
      Panel() :
        PanelDescription(typeid(P), P::get_title(),
          P::get_stock_id(), Panel::create_panel)
        {}
      
    private:
      /**
       * A helper function that will create a panel of type P
       * @param panel_manager The owner panel manager.
       * @param dock_item The GdlDockItem that will host this panel.
       * @return Returns a pointer to the panel object.
       */
      static panels::Panel* create_panel(
        PanelManager &panel_manager, GdlDockItem* dock_item)
          {
            return new P(panel_manager, dock_item);
          }
    };
  
  /**
   * The list of panel descriptions.
   */
  static const PanelDescription panelDescriptionList[];
};

}   // namespace workspace
}   // namespace gui

#endif // PANEL_MANAGER_HPP
