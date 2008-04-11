/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gui
 * Copyright (C)  2008 <>
 * 
 * gui is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * gui is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with gui.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifndef ACTIONS_H
#define ACTIONS_H

#include <gtkmm.h>

namespace Lumiera {
namespace Workspace {

class MainWindow;

class Actions
{
private:	
	Actions(MainWindow &main_window);

	//----- Event Handlers -----//
    void on_menu_file_new_generic();
    void on_menu_file_quit();
    void on_menu_others();

    void on_menu_choices_one();
    void on_menu_choices_two();

	//----- Actions -----//
	Glib::RefPtr<Gtk::ActionGroup> _action_group;
	Glib::RefPtr<Gtk::RadioAction> m_refChoiceOne, m_refChoiceTwo;

	// Reference to the main window
	MainWindow &_main_window;

	friend class MainWindow;
};

}   // namespace Workspace
}   // namespace Lumiera

#endif // ACTIONS_H
