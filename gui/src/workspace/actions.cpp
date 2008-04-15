/*
  Actions.cpp  -  Definition of the main workspace window object
 
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
 
* *****************************************************/

#include "../gtk-lumiera.hpp"
#include "actions.hpp"
#include "main-window.hpp"

namespace lumiera {
namespace gui {
namespace workspace {

  Actions::Actions(MainWindow &main_window) :
	mainWindow(main_window)
  {
	actionGroup = Gtk::ActionGroup::create();

	// File|New sub menu:
	actionGroup->add(Gtk::Action::create("FileNewStandard",
		  Gtk::Stock::NEW, "_New", "Create a new file"),
	  sigc::mem_fun(*this, &Actions::on_menu_file_new_generic));

	actionGroup->add(Gtk::Action::create("FileNewFoo",
		  Gtk::Stock::NEW, "New Foo", "Create a new foo"),
	  sigc::mem_fun(*this, &Actions::on_menu_file_new_generic));

	actionGroup->add(Gtk::Action::create("FileNewGoo",
		  Gtk::Stock::NEW, "_New Goo", "Create a new goo"),
	  sigc::mem_fun(*this, &Actions::on_menu_file_new_generic));

	// File menu:
	actionGroup->add(Gtk::Action::create("FileMenu", "File"));
	// Sub-menu.
	actionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW));
  actionGroup->add(Gtk::Action::create("FileRender", _("Render...")),
    Gtk::AccelKey("<shift>R"),
	  sigc::mem_fun(*this, &Actions::on_menu_file_render));
	actionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
	  sigc::mem_fun(*this, &Actions::on_menu_file_quit));

	// Edit menu:
	actionGroup->add(Gtk::Action::create("EditMenu", "Edit"));
	actionGroup->add(Gtk::Action::create("EditCopy", Gtk::Stock::COPY),
	sigc::mem_fun(*this, &Actions::on_menu_others));
	actionGroup->add(Gtk::Action::create("EditPaste", Gtk::Stock::PASTE),
	sigc::mem_fun(*this, &Actions::on_menu_others));
	actionGroup->add(Gtk::Action::create("EditSomething", "Something"),
	  Gtk::AccelKey("<control><alt>S"),
	  sigc::mem_fun(*this, &Actions::on_menu_others));

	// Choices menu, to demonstrate Radio items
	actionGroup->add( Gtk::Action::create("ChoicesMenu", "Choices") );
	Gtk::RadioAction::Group group_userlevel;
	m_refChoiceOne = Gtk::RadioAction::create(group_userlevel, "ChoiceOne", "One");
	actionGroup->add(m_refChoiceOne,
	sigc::mem_fun(*this, &Actions::on_menu_choices_one) );
	m_refChoiceTwo = Gtk::RadioAction::create(group_userlevel, "ChoiceTwo", "Two");
	actionGroup->add(m_refChoiceTwo,
	sigc::mem_fun(*this, &Actions::on_menu_choices_two) );

	// Help menu:
	actionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
	actionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::HELP),
	sigc::mem_fun(*this, &Actions::on_menu_others) );
  }

  void
  Actions::on_menu_file_render()
  {
    mainWindow.renderDialog->run();
  }

  void
  Actions::on_menu_file_quit()
  {
    mainWindow.hide(); // Closes the main window to stop the Gtk::Main::run().
  }

  void
  Actions::on_menu_file_new_generic()
  {
    g_message("A File|New menu item was selecteda.");
  }

  void
  Actions::on_menu_others()
  {
    g_message("A menu item was selected.");
  }

  void
  Actions::on_menu_choices_one()
  {
    Glib::ustring message;
    //if(m_refChoiceOne->get_active())
    //    message = "Choice 1 was selected.";
    //else
        message = "Choice 1 was deselected";

    g_message(message.c_str());
  }

  void
  Actions::on_menu_choices_two()
  {
    Glib::ustring message;
    //if(_main_window.m_refChoiceTwo->get_active())
    //    message = "Choice 2 was selected.";
    //else
        message = "Choice 2 was deselected";

    g_message(message.c_str());
  }

}   // namespace workspace
}   // namespace gui
}   // namespace lumiera

