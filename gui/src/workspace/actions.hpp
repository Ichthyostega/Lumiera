/*
  ACTIONS.hpp  -  Definition of a helper class for user actions
 
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
/** @file actions.hpp
 ** This file contains the definition of a helper class for the
 ** main workspace window object, which registers and handles
 ** user actions.
 ** @see mainwindow.hpp
 */

#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <gtkmm.h>

namespace lumiera {
namespace workspace {

class MainWindow;

  /**
   * A helper class which registers and handles
   * user action events.
   */
  class Actions
    {
    private:	
	  Actions(MainWindow &main_window);

	  /**
       * A reference to the MainWindow which owns
       * this helper */
	  MainWindow &mainWindow;

	  /* ===== Event Handlers ===== */
      void on_menu_file_new_generic();
      void on_menu_file_quit();
      void on_menu_others();

      void on_menu_choices_one();
      void on_menu_choices_two();

	  /* ===== Actions ===== */
	  Glib::RefPtr<Gtk::ActionGroup> actionGroup;
	  Glib::RefPtr<Gtk::RadioAction> m_refChoiceOne, m_refChoiceTwo;

	  friend class MainWindow;
  };

}   // namespace workspace
}   // namespace lumiera

#endif // ACTIONS_H
