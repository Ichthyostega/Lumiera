/*
  PanelManager  -  management of dockable GDL panels

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

* *****************************************************/


/** @file panel-manager.cpp
 ** Implementation of dockable UI panels, implemented with the
 ** help of lib GDL (»Gnome Docking Library«, formerly aka »Gnome Design Library«)
 ** @deprecated shall be transformed into a Dock entity as of 6/2018
 */


#include "gui/workspace/panel-manager.hpp"

#include "gui/panel/assets-panel.hpp"
#include "gui/panel/viewer-panel.hpp"
#include "gui/panel/infobox-panel.hpp"
#include "gui/panel/timeline-panel.hpp"
#include "gui/panel/timeline-panel-obsolete.hpp"

#include "include/logging.h"

using namespace boost;       ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace std;         ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace Gtk;         ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace gui {
namespace workspace {
  
  const PanelManager::PanelDescription
    PanelManager::panelDescriptionList[] = {
      PanelManager::Panel<TimelinePanel>(),
      PanelManager::Panel<TimelinePanelObsolete>(),
      PanelManager::Panel<InfoBoxPanel>(),
      PanelManager::Panel<ViewerPanel>(),
      PanelManager::Panel<AssetsPanel>()
    };
  
  unsigned short PanelManager::panelID = 0;
  
  
  
  PanelManager::PanelManager (WorkspaceWindow& owner)
    : workspaceWindow_(owner)
    , dock_()
    , dockBar_(dock_)
    , dockLayout_()
    {
      /* Create the DockLayout */
      dockLayout_ = Gdl::DockLayout::create(dock_);
      
      /* Setup the Switcher Style */
      Glib::RefPtr<Gdl::DockMaster> dock_master = dock_.property_master();
      dock_master->property_switcher_style() = Gdl::SWITCHER_STYLE_ICON;
      
      memset(&dockPlaceholders_, 0, sizeof(dockPlaceholders_)); 
    }
  
  
  
  PanelManager::~PanelManager()
  {
      ///////////////////////////////////////////////////////TICKET #195 : violation of policy, dtors must not do any work 
      ///////////////////////////////////////////////////////TICKET #172 : observed as a reason for crashes when closing the GUI. It was invoked after end of main, when the GUI as already gone.
    
#if false ///////////////////////////////////////////////////TICKET #937 : disabled for GTK-3 transition. TODO investigate why this logic existed...    
      ///////////////////////////////////////////////////////TICKET #1027
    for(int i = 0; i < 4; i++)
      if(dockPlaceholders_[i])
        g_object_unref(dockPlaceholders_[i]);
        
    clearPanels();
#endif    ///////////////////////////////////////////////////TICKET #937 : (End)disabled for GTK-3 transition.
  }
  
  
  
  void
  PanelManager::setupDock()
  {
      ///////////////////////////////////////////////////////TICKET #1027 : investigate what would be the proper way to do this with gdlmm (C++ binding). No direct usage of GDL !

    REQUIRE(dockPlaceholders_[0] == NULL && dockPlaceholders_[1] == NULL &&
      dockPlaceholders_[2] == NULL && dockPlaceholders_[3] == NULL);
    dockPlaceholders_[0] = GDL_DOCK_PLACEHOLDER(gdl_dock_placeholder_new(
      "ph1", GDL_DOCK_OBJECT(dock_.gobj()), GDL_DOCK_TOP, FALSE));
    dockPlaceholders_[1] = GDL_DOCK_PLACEHOLDER(gdl_dock_placeholder_new(
      "ph2", GDL_DOCK_OBJECT(dock_.gobj()), GDL_DOCK_BOTTOM, FALSE));
    dockPlaceholders_[2] = GDL_DOCK_PLACEHOLDER(gdl_dock_placeholder_new(
      "ph3", GDL_DOCK_OBJECT(dock_.gobj()), GDL_DOCK_LEFT, FALSE));
    dockPlaceholders_[3] = GDL_DOCK_PLACEHOLDER(gdl_dock_placeholder_new(
      "ph4", GDL_DOCK_OBJECT(dock_.gobj()), GDL_DOCK_RIGHT, FALSE));
    ENSURE(dockPlaceholders_[0] && dockPlaceholders_[1] &&
      dockPlaceholders_[2] && dockPlaceholders_[3]);
    
    createPanels();
  }
  
  
  Gdl::Dock&
  PanelManager::getDock()
  {
    return dock_;
  }
  
  
  Gdl::DockBar&
  PanelManager::getDockBar()
  {
    return dockBar_;
  }
  
  
  WorkspaceWindow&
  PanelManager::getWorkspaceWindow()
  {
    return workspaceWindow_;
  }
  
  
  panel::Panel&
  PanelManager::showPanel (const int description_index)
  {
    // Try and find the panel and present it if possible
    list< panel::Panel* >::iterator i;
    for(i = panels_.begin(); i != panels_.end(); i++)
      {
        panel::Panel* const panel = *i;
        if (getPanelType(panel) == description_index)
          {
            if (!panel->is_shown()) panel->show();
            
            Gdl::DockItem &dock_item = panel->getDockItem();
            dock_item.present(dock_);
            ENSURE (panel);
            return *panel;
          }
      }
    
    // Create the new panel
    panel::Panel *new_panel = createPanel_by_index (description_index);
    
    // Dock the item
    dock_.add_item(new_panel->getDockItem(), Gdl::DOCK_FLOATING);
    
    ENSURE (new_panel);
    return *new_panel;
  }
  
  
  void
  PanelManager::switchPanel (panel::Panel& old_panel, const int description_index)
  {
    REQUIRE (description_index >= 0 &&
             description_index < getPanelDescriptionCount());
    
    // Get the dock item
    Gdl::DockItem &dock_item = old_panel.getDockItem();
    
    // Release the old panel
    removePanel (&old_panel);
    
    // Create the new panel
    createPanel_by_index (description_index, dock_item);
  }
  
  
  void
  PanelManager::splitPanel (panel::Panel& panel, Gtk::Orientation split_direction)
  {
    
    // Create the new panel
    const int index = getPanelType(&panel);
    panel::Panel *new_panel = createPanel_by_index(index);
    
    // Dock the panel
    Gdl::DockPlacement placement = Gdl::DOCK_NONE;
    switch(split_direction)
      {
      case ORIENTATION_HORIZONTAL:
        placement = Gdl::DOCK_RIGHT;
        break;
      
      case ORIENTATION_VERTICAL:
        placement = Gdl::DOCK_BOTTOM;
        break;
      
      default:
        ERROR(gui, "Unknown split_direction: %d", split_direction);
        return;
        break;
      }
      
      panel.getDockItem().dock(
          new_panel->getDockItem(),placement);
  }
  
  
  int
  PanelManager::getPanelDescriptionCount()
  {
    return sizeof(panelDescriptionList) / sizeof(PanelDescription);
  }
  
  
  const gchar*
  PanelManager::getPanelStockID (int index)
  {
    REQUIRE (index >= 0 && index < getPanelDescriptionCount());
    return panelDescriptionList[index].getStockID();
  }
  
  
  const char*
  PanelManager::getPanelTitle (int index)
  {
    REQUIRE (index >= 0 && index < getPanelDescriptionCount());
    return panelDescriptionList[index].getTitle();
  }
  
  
  void
  PanelManager::createPanels()
  {
                                                            ///////////////////////////////TICKET #1026 : code smell, use types directly instead          
    panel::Panel* assetsPanel =   createPanel_by_name("AssetsPanel");
    panel::Panel* viewerPanel =   createPanel_by_name("InfoBoxPanel");
    panel::Panel* timelinePanel = createPanel_by_name("TimelinePanel");
    
    dock_.add_item(assetsPanel->getDockItem(),Gdl::DOCK_LEFT);
    dock_.add_item(timelinePanel->getDockItem(),Gdl::DOCK_BOTTOM);
    dock_.add_item(viewerPanel->getDockItem(),Gdl::DOCK_RIGHT);
  }
  
  
  int
  PanelManager::findPanelDescription (const char* class_name)  const
  {
    REQUIRE(class_name);
    
    const int count = getPanelDescriptionCount();
    for(int i = 0; i < count; i++)
      {
        if (strstr(panelDescriptionList[i].getClassName(), class_name))
          return i;
      }
    
    ERROR (gui, "Unable to find a description with class name %s", class_name);
    return -1;
  }
  
  
  panel::Panel*
  PanelManager::createPanel_by_index (const int index)
  {
    REQUIRE(index >= 0 && index < getPanelDescriptionCount());
    
    // Make a unique name for the panel
    char name[5];
    snprintf(name, sizeof(name), "%X", panelID++);
    
    // Create a dock item
    return createPanel_by_index(index,
                                 *new Gdl::DockItem(name,"",Gdl::DOCK_ITEM_BEH_NORMAL));
  }
  
  
  panel::Panel*
  PanelManager::createPanel_by_index (const int index, Gdl::DockItem &dock_item)
  {
    // Create the panel object
    panel::Panel *panel =  panelDescriptionList[index].create(*this, dock_item);
    ENSURE(panel);
    panel->show_all();
    
    // Connect event handlers
    panel->signal_hidePanel().connect(sigc::bind(
      sigc::mem_fun(*this, &PanelManager::on_panel_shown), panel));
    
    // Add the panel to the list
    panels_.push_back(panel);
    
    return panel;
  }
  
  
  panel::Panel*
  PanelManager::createPanel_by_name (const char* class_name)
  {
    REQUIRE(class_name);
    const int index = findPanelDescription(class_name);
    return createPanel_by_index(index);
  }
  
  
  int
  PanelManager::getPanelType (panel::Panel* const panel)  const
  {
    REQUIRE(panel);
    
    const type_info &info = typeid(*panel); 
    const int count = getPanelDescriptionCount();
    for(int i = 0; i < count; i++)
      {
        if(info == panelDescriptionList[i].getClassInfo())
          return i;
      }
    
    ERROR(gui, "Unable to find a description with with this class type");
    return -1;
  }
  
  
  void
  PanelManager::removePanel (panel::Panel* const panel)
  {
    REQUIRE(panel);
    
    list< panel::Panel* >::iterator i;
    for(i = panels_.begin(); i != panels_.end(); i++)
      {
        if(*i == panel)
          {
            delete panel;
            panels_.erase(i);
            break;
          }
      }
  }
  
  
  void
  PanelManager::clearPanels()
  {
      ///////////////////////////////////////////////////////TICKET #195 : this whole approach smells like an obsolete "C-style" approach.  We should strive to let the runtime system do such stuff for us whenever possible, eg. by using smart pointers
    
    list< panel::Panel* >::iterator i;
    for(i = panels_.begin(); i != panels_.end(); i++)
      delete *i;
    panels_.clear();
  }
  
  
  void
  PanelManager::on_panel_shown (panel::Panel* panel)
  {
    REQUIRE(panel);
    if(panel->is_shown() || panel->is_iconified()) return;
    
    removePanel(panel);
  }
  
  
  
}}// namespace gui::workspace
