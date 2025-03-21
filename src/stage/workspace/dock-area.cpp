/*
  DockArea  -  maintain a docking area within the WorkspaceWindow

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file dock-area.cpp
 ** Implementation of dockable UI panels, implemented with the
 ** help of lib GDL (»Gnome Docking Library«, formerly aka »Gnome Design Library«)
 ** @todo will be transformed into a Dock entity as of 6/2018   /////////////////////////////////////////////TICKET #1144  refactor dock handling
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1144  TODO step-by-step...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1144  - understand the existing start-up sequence
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1144  - rewrite it in clean shape
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1145  NOTE: not populate the UI with content anymore!!!!

#include "stage/workspace/dock-area.hpp"

#include "stage/panel/assets-panel.hpp"
#include "stage/panel/viewer-panel.hpp"
#include "stage/panel/infobox-panel.hpp"
#include "stage/panel/timeline-panel.hpp"

#include "include/logging.h"
#include "lib/util-foreach.hpp"

using namespace boost;       ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace std;         ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace Gtk;         ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage {
namespace workspace {
  
  const DockArea::PanelDescription
    DockArea::panelDescriptionList[] = {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1144
      DockArea::Panel<TimelinePanel>(),
      DockArea::Panel<InfoBoxPanel>(),
      DockArea::Panel<ViewerPanel>(),
      DockArea::Panel<AssetsPanel>()
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1144
    };
  
  unsigned short DockArea::panelID = 0;
  
  
  
  DockArea::DockArea (WorkspaceWindow& owner)
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
  
  
  
  DockArea::~DockArea()
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
  DockArea::setupDock()
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
  DockArea::getDock()
  {
    return dock_;
  }
  
  
  Gdl::DockBar&
  DockArea::getDockBar()
  {
    return dockBar_;
  }
  
  
  WorkspaceWindow&
  DockArea::getWorkspaceWindow()
  {
    return workspaceWindow_;
  }
  
  
  bool
  DockArea::hasPanel (const int description_index)
  {
    return util::has_any (panels_, [=](panel::Panel* panel)
                                    {
                                      return getPanelType(panel) == description_index;
                                    });
  }

  panel::Panel&
  DockArea::showPanel (const int description_index)
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
         // ENSURE(dock_item);
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
  DockArea::switchPanel (panel::Panel& old_panel, const int description_index)
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
  DockArea::splitPanel (panel::Panel& panel, Gtk::Orientation split_direction)
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
        ERROR(stage, "Unknown split_direction: %d", split_direction);
        return;
        break;
      }
      
      panel.getDockItem().dock(
          new_panel->getDockItem(),placement);
  }
  
  
  int
  DockArea::getPanelDescriptionCount()
  {
    return sizeof(panelDescriptionList) / sizeof(PanelDescription);
  }
  
  
  const gchar*
  DockArea::getPanelStockID (int index)
  {
    REQUIRE (index >= 0 && index < getPanelDescriptionCount());
    return panelDescriptionList[index].getStockID();
  }
  
  
  const char*
  DockArea::getPanelTitle (int index)
  {
    REQUIRE (index >= 0 && index < getPanelDescriptionCount());
    return panelDescriptionList[index].getTitle();
  }
  
  
  void
  DockArea::createPanels()
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
  DockArea::findPanelDescription (const char* class_name)
  {
    REQUIRE(class_name);
    
    const int count = getPanelDescriptionCount();
    for(int i = 0; i < count; i++)
      {
        if (strstr(panelDescriptionList[i].getClassName(), class_name))
          return i;
      }
    
    ERROR (stage, "Unable to find a description with class name %s", class_name);
    return -1;
  }
  
  
  panel::Panel*
  DockArea::createPanel_by_index (const int index)
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
  DockArea::createPanel_by_index (const int index, Gdl::DockItem &dock_item)
  {
    // Create the panel object
    panel::Panel *panel =  panelDescriptionList[index].create(*this, dock_item);
    ENSURE(panel);
    panel->show_all();
    
    // Connect event handlers
    panel->signal_hidePanel().connect(sigc::bind(
      sigc::mem_fun(*this, &DockArea::on_panel_shown), panel));
    
    // Add the panel to the list
    panels_.push_back(panel);
    
    return panel;
  }
  
  
  panel::Panel*
  DockArea::createPanel_by_name (const char* class_name)
  {
    REQUIRE(class_name);
    const int index = findPanelDescription(class_name);
    return createPanel_by_index(index);
  }
  
  
  int
  DockArea::getPanelType (panel::Panel* const panel)  const
  {
    REQUIRE(panel);
    
    const type_info &info = typeid(*panel); 
    const int count = getPanelDescriptionCount();
    for(int i = 0; i < count; i++)
      {
        if(info == panelDescriptionList[i].getClassInfo())
          return i;
      }
    
    ERROR(stage, "Unable to find a description with with this class type");
    return -1;
  }
  
  
  void
  DockArea::removePanel (panel::Panel* const panel)
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
  DockArea::clearPanels()
  {
      ///////////////////////////////////////////////////////TICKET #195 : this whole approach smells like an obsolete "C-style" approach.  We should strive to let the runtime system do such stuff for us whenever possible, eg. by using smart pointers
    
    list< panel::Panel* >::iterator i;
    for(i = panels_.begin(); i != panels_.end(); i++)
      delete *i;
    panels_.clear();
  }
  
  
  void
  DockArea::on_panel_shown (panel::Panel* panel)
  {
    REQUIRE(panel);
    if(panel->is_shown() || panel->is_iconified()) return;
    
    removePanel(panel);
  }
  
  
  
}}// namespace stage::workspace
