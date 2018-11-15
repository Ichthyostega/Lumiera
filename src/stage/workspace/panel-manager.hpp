/*
  PANEL-MANAGER.hpp  -  management of dockable GDL panels

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
 ** Management of dockable panels.
 ** 
 ** @todo 2017 need to clarify the intended behaviour of panels
 **                               ///////////////////////////////////////////////////////////////////////////TICKET #1097  clarify the role and behaviour of Panels
 ** @deprecated shall be transformed into a Dock entity as of 6/2018   //////////////////////////////////////TICKET #1144  refactor dock handling
 ** 
 ** @see actions.hpp
 */

#ifndef GUI_WORKSPACE_PANEL_MANAGER_H
#define GUI_WORKSPACE_PANEL_MANAGER_H

#include "stage/panel/panel.hpp"

#include <gdlmm.h>
#include <typeinfo>


using namespace gui::panel;       ///////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace gui {
namespace workspace {
  
  
  /**
   * A class to manage DockItem objects for WorkspaceWindow.
   * @todo this code is smelly and needs some clean-up        ///////////////////////////////TICKET #1026
   */
  class PanelManager
    {
      /** reference to the owner workspace window object */
      WorkspaceWindow& workspaceWindow_;
      
      /** The pointer to GDL dock widget.
       * @remarks This value is NULL until setup_dock has been called.
       */
      Gdl::Dock dock_;
      
      /** The pointer to GDL dock bar widget.
       * @remarks This value is NULL until setup_dock has been called.
       */
      Gdl::DockBar dockBar_;
      
      /** The pointer to GDL dock layout object.
       * @remarks This value is NULL until setup_dock has been called.
       */
      Glib::RefPtr<Gdl::DockLayout> dockLayout_;
      
      /** Pointers to the 4 root place holders.
       * @remarks All 4 entries are NULL until setup_dock has been called.
       */
      GdlDockPlaceholder *dockPlaceholders_[4];
      
      /** list of all panels created */
      std::list<panel::Panel*> panels_;
      
      /**
       * An accumulator for the panel id.
       */
      static unsigned short panelID;
      
      
    public:
      PanelManager (WorkspaceWindow&);
     ~PanelManager();
      
      /**
       * Initialises this dock manager and creates the dock and all it's widgets.
       * @remarks This function must be called only once as the first call after construction.
       */
      void setupDock();
      
      /**
       * Gets a pointer to the dock object.
       * @remarks Note that this must not be called before setup_dock.   ///////////////////////////////TICKET #1026 : code smell
       */
      Gdl::Dock& getDock();
      
      /**
       * Gets a pointer to the dock bar.
       * @remarks Note that this must not be called before setup_dock.
       */
      Gdl::DockBar& getDockBar();
      
      /**
       * Returns a reference to the owner workspace window.
       */
      WorkspaceWindow& getWorkspaceWindow();                             ///////////////////////////////TICKET #1026 : code smell, unclear dependency relation
      
      /**
       * Shows a panel given a description index.
       * @param description_index The index of the panel type to show.
       * @return existing or new Panel, docked within the realm of this PanelManager.
       */
      panel::Panel& showPanel (const int description_index);
      
      /** was the indicated panel already allocated within this PanelManager's realm? */
      bool hasPanel (const int description_index);
      
      /**
       * Switches a panel from one type to another,
       * without touching the underlying GdlDockItem.
       * @param old_panel The panel which will be transformed to a new type.
       * @param description_index The index of the panel description
       *        that will be instantiated.
       */
      void switchPanel (panel::Panel& old_panel, const int description_index);
      
      /**
       * Splits a panel into two panels of the same type.
       * @param panel The panel to split.
       * @param split_direction The direction to split the panel in.
       */
      void splitPanel (panel::Panel& panel, Gtk::Orientation split_direction);
      
      
    public:
      /**
       * retrieve the internal type-ID corresponding to the given panel implementation type.
       * @return internal index into the #panelDescriptionList, or `-1` if not found.
       * @deprecated the whole concept of panel identification needs overhaul 8/2018
       */
      template<class P>
      static int findPanelID();
      
      /** Gets the number of panel descriptions. */
      static int getPanelDescriptionCount();
      
      /**
       * Gets a panel description's stock id.
       * @param index The index of the panel to retrieve.
       * @return Returns the stock id of a panel at this index.
       */
      static const gchar* getPanelStockID (const int index);
      
      /**
       * Gets a panel description's title.
       * @param index The index of the panel to retrieve.
       * @return Returns the title of a panel at this index.
       */
      static const char* getPanelTitle (int index);
      
      
    private:
      /** Creates the standard panel layout.*/
      void createPanels();
      
      /**
       * Find the index of a panel description given the class name.
       * @param class_name The name of the object class to search for.
       * @return Returns the index of the panel description found, or -1
       * if no description was found for this type.
       */
      static int findPanelDescription (const char* class_name);
      
      /**
       * Creates a panel by description index.
       * @param index The index of the description to instantiate.
       * @return Returns a pointer to the new instantiated panel object.
       */
      panel::Panel* createPanel_by_index (const int index);
      
      /**
       * Creates a panel by description index with a given GdlDockItem
       * @param index The index of the description to instantiate.
       * @param dock_item The GdlDockItem to attach this panel to
       * @return Returns a pointer to the new instantiated panel object.
       */
      panel::Panel* createPanel_by_index (const int index, Gdl::DockItem& dock_item);
      
      /**
       * Creates a panel by class name.
       * @param class_name The name of the object class to create.
       * @return Returns a pointer to the new instantiated panel object.
       */
      panel::Panel* createPanel_by_name (const char* class_name);
      
      /**
       * Gets the type of a given panel.
       * @param panel The Panel to get the type of
       * @return Returns the index of the panel description found, or -1
       * if no description was found for this type.
       */
      int getPanelType (panel::Panel* const panel) const;
      
      /**
       * Removes a panel from the panel list and deletes it.
       * @param panel The panel to remove and delete.
       */
      void removePanel (panel::Panel* const panel);
      
      /** Removes all panels from the panel list and deletes them.*/
      void clearPanels();
      
      
    private:
      /**
       * An event handler for when the panel is shown or hidden.
       * @param panel A pointer to the panel that was hidden.
       */
      void on_panel_shown (panel::Panel *panel);
      
      
      
    private:
      
      /**
       * A class to describe and instantiate Panel types.
       */
      class PanelDescription
        {
        protected:
          typedef panel::Panel* (*const CreatePanelProc)(PanelManager&, Gdl::DockItem&);
                                                            ///////////////////////////////TICKET #1026 : code smell, why not just using inheritance?          
        private:
          /** reference to the typeID of this class */
          const std::type_info& classInfo_;
          
          /** localised title that will be shown on the panel. */
          const char* const titleName_;
          
          /** Stock ID for this type of panel.*/
          const gchar* const stockID_;
          
          /**pointer to a function that will instantiate the panel object */
          CreatePanelProc createPanelProc_;
          
          
        protected:
          /**
           * @param classInfo The typeID of the Panel class
           * @param title The localised title that will be shown on the panel.
           * @param stock_id The Stock ID for this type of panel.
           * @param create_panel_proc A pointer to a function that will instantiate the panel object.
           */
          PanelDescription (std::type_info const& classInfo
                           ,const char* title
                           ,const gchar* stockID
                           ,CreatePanelProc createPanelProc)
            : classInfo_(classInfo)
            , titleName_(title)
            , stockID_(stockID)
            , createPanelProc_(createPanelProc)
            {
              REQUIRE(titleName_);
            }
            
          
        public:
          const std::type_info& getClassInfo()  const
            {
              return classInfo_;
            }
          
          const char* getClassName()  const
            {
              return classInfo_.name();
            }
          
          /** the localised title that will be shown on the panel */
          const char* getTitle()  const
            {
              ENSURE(titleName_);
              return titleName_;
            }
          
          const gchar* getStockID()  const
            {
              ENSURE(stockID_);
              return stockID_;
            }
          
          
          /**
           * Creates an instance of this panel.
           * @param panel_manager The owner panel manager.
           * @param dock_item The GdlDockItem that will host this panel.
           * @return Returns a pointer to the panel object.
           */
          panel::Panel*
          create (PanelManager& panelManager, Gdl::DockItem& dockItem)  const
            {
              REQUIRE(createPanelProc_);
              return createPanelProc_ (panelManager, dockItem);
            }
        };
      
      
      
      /**
       * A helper class that will create PanelDescription objects.
       * @param P The type of panel::Panel that the PanelDescription will describe.
       */
      template<class P>
      class Panel
        : public PanelDescription
        {
        public:
          Panel()
            : PanelDescription (typeid(P)
                               ,P::getTitle()
                               ,P::getStockID()
                               ,Panel::createPanel)
            { }
          
        private:
          /**helper function to create a panel of type P
           * @param panel_manager The owner panel manager.
           * @param dock_item The Gdl::DockItem that will host this panel.
           * @return pointer to the created panel object.
           */
          static panel::Panel*
          createPanel (PanelManager& panelManager, Gdl::DockItem& dockItem)
            {
              return new P(panelManager, dockItem);
            }
        };
      
      
      /** The list of panel descriptions */
      static const PanelDescription panelDescriptionList[];
    };
  
  
  
  
  template<class P>
  inline int
  PanelManager::findPanelID()
  {
    return PanelManager::findPanelDescription (typeid(P).name());
  }
  
  
  
}}// namespace gui::workspace
#endif /*GUI_WORKSPACE_PANEL_MANAGER_H*/
