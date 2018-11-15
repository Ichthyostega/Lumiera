/*
  MenuButton  -  button widget to invoke a menu

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


/** @file menu-button.cpp
 ** Implementation of the MenuButton widget, to display a menu when clicked.
 */


#include "stage/widget/menu-button.hpp"


using namespace Gtk;      ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace Glib;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace sigc;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!


#define POPUP_SLUG "TheMenu"
#define POPUP_PATH "/" POPUP_SLUG

namespace stage {
namespace widget {
  
  namespace {
    const int CaptionPadding = 4;
    const ArrowType arrowType = ARROW_DOWN;
    const ShadowType shadowType = SHADOW_NONE;
  }
  
  
  
  MenuButton::MenuButton()
    : ToggleButton()
    , arrow(arrowType, shadowType)
    , uimanager(UIManager::create())
    , actions(ActionGroup::create())
    {
      setupButton();
    }
  
  
  MenuButton::MenuButton (StockID const& stock_id)
    : ToggleButton()
    , image(stock_id, ICON_SIZE_MENU)
    , caption()
    , arrow(arrowType, shadowType)
    {
      StockItem stock_item;
      REQUIRE (StockItem::lookup (stock_id, stock_item));
      caption.set_text_with_mnemonic (stock_item.get_label());
      hBox.pack_start (image);
      setupButton();
    }
  
  
  MenuButton::MenuButton (cuString& label, bool mnemonic)
    : ToggleButton()
    , caption(label, mnemonic)
    , arrow(arrowType, shadowType)
    {
      setupButton();
    }
  
  
  
  Gtk::Widget*
  MenuButton::get (uString slug)
  {
    //TODO: if (slug == "Menu") return &get_menu();
    uString path (POPUP_PATH);
    path.append("/");
    return uimanager->get_widget(path.append(slug));
  }
  
  
  Menu&
  MenuButton::get_menu()
  {
    uString path ("/");
    path.append (POPUP_SLUG);
    Menu* p_menu = dynamic_cast<Menu*>(
                uimanager->get_widget(path));
    REQUIRE (p_menu);
    return *p_menu;
  }
  
  
  void
  MenuButton::append (uString &slug, uString &title,
                      sigc::slot<void> &callback, bool toggle)
  {
    if (!toggle)
      actions->add(Action::create(slug, title,""), callback);
    else
      actions->add(ToggleAction::create(slug, title,"",false), callback);
    
    uimanager->add_ui(uimanager->new_merge_id(),
                      ustring("ui/").append(POPUP_SLUG),
                      slug, slug, Gtk::UI_MANAGER_AUTO,
                      false);
    uimanager->ensure_update();
  }
  
  
  void
  MenuButton::append (const char *slug, const char* title,
                      sigc::slot<void>& callback, bool toggle)
  {
    uString uSlug (slug);
    uString uTitle (_(title));
    append (uSlug, uTitle, callback, toggle);
  }
  
  
  void
  MenuButton::appendSeparator()
  {
    uimanager->add_ui_separator(
        uimanager->new_merge_id(),
        ustring("ui/").append(POPUP_SLUG),
        "Separator", Gtk::UI_MANAGER_SEPARATOR,
        false);
  }
  
  
  void
  MenuButton::popup()
  {
    get_menu().popup(mem_fun(this, &MenuButton::on_menu_position),
      0, gtk_get_current_event_time());
    set_active();
  }
  
  
  void
  MenuButton::setupButton()
  {
    uimanager = UIManager::create();
    actions = ActionGroup::create();
    uimanager->insert_action_group(actions);
    
    // Setup the UIManager with a popup menu
    const guint rootId = uimanager->new_merge_id();
    uimanager->add_ui(rootId, "ui",
        POPUP_SLUG, POPUP_SLUG,
        Gtk::UI_MANAGER_POPUP);
    
    get_menu().signal_deactivate().connect(mem_fun(
      this, &MenuButton::on_menu_deactivated));
    
    arrow.set(ARROW_DOWN, SHADOW_NONE);
    
    hBox.pack_start(caption, PACK_EXPAND_WIDGET, CaptionPadding);
    hBox.pack_start(arrow);
    
    add(hBox);
    show_all();
  }
  
  
  void
  MenuButton::on_pressed()
  {
    popup();
  }
  
  
  void
  MenuButton::on_menu_deactivated()
  {
    set_active(false);
  }
  
  
  void
  MenuButton::on_menu_position (int& x, int& y, bool& push_in)
  {
    Glib::RefPtr<Gdk::Window> window = get_window();
    REQUIRE(window);
    
    window->get_origin(x, y);
    const Allocation allocation = get_allocation();
      x += allocation.get_x();
      y += allocation.get_y() + allocation.get_height();
      
      push_in = true;
  }
  
  
}}// stage::widget
