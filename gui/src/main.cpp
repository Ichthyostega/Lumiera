/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cpp
 * Copyright (C) Joel Holdsworth 2008 <joel@airwebreathe.org.uk>
 * 
 * main.cc is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * main.cc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with main.cc.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include <gtkmm.h>
#include <iostream>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include "main-window.h"

using namespace Lumiera::UI;

int main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);
	
	MainWindow main_window;
	
	kit.run(main_window);

	return 0;
}
