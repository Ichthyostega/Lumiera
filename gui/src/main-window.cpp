//           main-window.cpp
//  Tue Apr  8 23:54:36 2008
//  Copyright  2008  joel
//  <joel@<host>>

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA

#include <gtkmm.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#include "gtk-lumiera.h"
#include "main-window.h"

namespace Lumiera {
namespace UI {

MainWindow::MainWindow()
{
	set_title(_("Lumiera"));
	set_default_size(1024, 768);
}
	
}	
}
