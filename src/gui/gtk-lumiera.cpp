/*
  GtkLumiera  -  The Lumiera GUI Application Object

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


/** @file gtk-lumiera.cpp
 */


#include "gui/gtk-base.hpp"
#include "gui/ctrl/ui-manager.hpp"
#include "gui/ui-bus.hpp"
#include "lib/depend.hpp"

#include "include/config-facade.h"

#include <gdlmm.h>


namespace gui {
  
  namespace error = lumiera::error;
  
  
  using std::string;
  
  
  
  
  namespace {
    
    /** storage for the Main Application object */
    lib::Depend<GtkLumiera> theApplicationInstance;
    
  }
  
  
  
  
  GtkLumiera&
  GtkLumiera::application()
  {
    return theApplicationInstance();
  }
  
  
  
  /** @remarks this function blocks until shutdown of the UI */
  void
  GtkLumiera::run()
  {
    //////////////////////TICKET #959 : establish the new backbone here / replaces Project and Controller
    UiBus uiBus;
    
    ctrl::UiManager uiManager(uiBus);
    uiManager.createApplicationWindow();
    uiManager.performMainLoop();
  }
  
  
}// namespace gui
