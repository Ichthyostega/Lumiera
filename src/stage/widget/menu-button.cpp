/*
  MenuButton  -  button widget to invoke a menu

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2012,            Michael R. Fisher <mfisher31@gmail.com>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file menu-button.cpp
 ** Implementation of the MenuButton widget, to display a menu when clicked.
 */


#include "stage/widget/menu-button.hpp"


#define POPUP_SLUG "TheMenu"
#define POPUP_PATH "/" POPUP_SLUG


namespace stage {
namespace widget{
  
  namespace {
    const int CAPTION_PADDING = 4;
    const Gtk::ArrowType  ARROW_TYPE = Gtk::ARROW_DOWN;
    const Gtk::ShadowType SHADOW_TYPE = Gtk::SHADOW_NONE;
  }
  
  
  
  MenuButton::MenuButton()
    : ToggleButton()
    , arrow_{ARROW_TYPE, SHADOW_TYPE}
    , uimanager_{Gtk::UIManager::create()}
    , actions_{Gtk::ActionGroup::create()}
    {
      setupButton();
    }
  
  
  MenuButton::MenuButton (Gtk::StockID const& stock_id)
    : ToggleButton{}
    , image_{stock_id, Gtk::ICON_SIZE_MENU}
    , caption_{}
    , arrow_{ARROW_TYPE, SHADOW_TYPE}
    {
      Gtk::StockItem stock_item;
      REQUIRE (Gtk::StockItem::lookup (stock_id, stock_item));
      caption_.set_text_with_mnemonic (stock_item.get_label());
      hBox_.pack_start (image_);
      setupButton();
    }
  
  
  MenuButton::MenuButton (cuString& label, bool mnemonic)
    : ToggleButton()
    , caption_(label, mnemonic)
    , arrow_(ARROW_TYPE, SHADOW_TYPE)
    {
      setupButton();
    }
  
  
  
  Gtk::Widget*
  MenuButton::get (uString slug)
  {
    //TODO: if (slug == "Menu") return &get_menu();
    uString path (POPUP_PATH);
    path.append("/");
    return uimanager_->get_widget(path.append(slug));
  }
  
  
  Gtk::Menu&
  MenuButton::get_menu()
  {
    uString path{"/"};
    path.append (POPUP_SLUG);
    auto* p_menu = dynamic_cast<Gtk::Menu*> (uimanager_->get_widget(path));
    REQUIRE (p_menu);
    return *p_menu;
  }
  
  
  void
  MenuButton::append (uString& slug, uString &title,
                      sigc::slot<void>& callback, bool toggle)
  {
    if (not toggle)
      actions_->add (Gtk::Action::create (slug, title,""), callback);
    else
      actions_->add (Gtk::ToggleAction::create (slug, title,"",false), callback);
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1181 based on Interface-builder // @deprecated
    uimanager_->add_ui (uimanager_->new_merge_id()           // ID for the merged UI, see gtk_ui_manager_new_merge_id().
                       ,uString{"ui/"}.append(POPUP_SLUG)    // schematic path in the UI
                       ,slug                                 // name for the added UI element
                       ,slug                                 // name for the action to be proxied
                       ,Gtk::UI_MANAGER_AUTO                 // type of the UI element to add
                       ,false                                // if the UI element is added before its siblings (otherwise added after them)
                       );
    
    uimanager_->ensure_update();
  }
  
  
  void
  MenuButton::append (CStr slug, CStr title,
                      sigc::slot<void>& callback, bool toggle)
  {
    uString uSlug{slug};
    uString uTitle{_(title)};
    append (uSlug, uTitle, callback, toggle);
  }
  
  
  void
  MenuButton::appendSeparator()
  {
    uimanager_->add_ui_separator(
        uimanager_->new_merge_id(),
        uString{"ui/"}.append(POPUP_SLUG),
        "Separator", Gtk::UI_MANAGER_SEPARATOR,
        false);
  }
  
  
  void
  MenuButton::popup()
  {
    get_menu().popup (sigc::mem_fun (this, &MenuButton::on_menu_position)
                     ,0                                      // The mouse button which was pressed to initiate the event
                     ,gtk_get_current_event_time());         // activate_time The time at which the activation event occurred.
    this->set_active();
  }
  
  
  void
  MenuButton::setupButton()
  {
    uimanager_ = Gtk::UIManager::create();
    actions_   = Gtk::ActionGroup::create();
    uimanager_->insert_action_group(actions_);
    
    // Setup the UIManager with a popup menu
    const guint rootId = uimanager_->new_merge_id();
    uimanager_->add_ui (rootId
                       ,"ui"
                       ,POPUP_SLUG
                       ,POPUP_SLUG
                       ,Gtk::UI_MANAGER_POPUP
                       );
    get_menu().signal_deactivate().connect(
        sigc::mem_fun (this, &MenuButton::on_menu_deactivated));
    
    arrow_.set (Gtk::ARROW_DOWN, Gtk::SHADOW_NONE);
    
    hBox_.pack_start (caption_, Gtk::PACK_EXPAND_WIDGET, CAPTION_PADDING);
    hBox_.pack_start (arrow_);
    
    add (hBox_);
    show_all();
  }
  
  
  void
  MenuButton::on_pressed()
  {
    this->popup();
  }
  
  
  void
  MenuButton::on_menu_deactivated()
  {
    this->set_active (false);
  }
  
  
  void
  MenuButton::on_menu_position (int& x, int& y, bool& push_in)
  {
    Glib::RefPtr<Gdk::Window> window = get_window();
    REQUIRE(window);
    
    window->get_origin(x, y);
    auto allocation = get_allocation();
    x += allocation.get_x();
    y += allocation.get_y() + allocation.get_height();
    
    push_in = true;
  }
  
  
}}// stage::widget
