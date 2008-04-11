/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>
#include "actions.hpp"

namespace Lumiera {
namespace Workspace {
	
class MainWindow : public Gtk::Window
{
public:
	MainWindow();
	virtual ~MainWindow();
	
protected:
    void create_ui();
       
    // Helpers
    Actions _actions;

    // Child widgets
    Gtk::VBox _box;

    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
};

}   // namespace Workspace
}   // namespace Lumiera

#endif // MAIN_WINDOW_H
