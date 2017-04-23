/*
  WIZARD.hpp  -  controller user help and assistance

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

*/


/** @file wizard.hpp
 ** Global help controller.
 ** The Wizard is a global controller to handle launching the user help,
 ** context sensitive help and further user assistance and support UI.
 ** @remark User help is planned to rely on the [UserManual], possibly
 **         using an embedded browser control or an installed local copy
 **         of the manual.
 ** @todo The infrastructure for context sensitive help needs to be defined
 ** 
 ** [UserManual]: http://www.lumiera.org/documentation/user/manual.html "»User Manual«"
 ** 
 ** @todo WIP 3/2017 early draft of the UI top-level controllers
 ** 
 ** @see interaction-director.hpp
 ** @see actions.hpp
 */


#ifndef GUI_INTERACT_WIZARD_H
#define GUI_INTERACT_WIZARD_H

#include "gui/gtk-base.hpp"

#include <boost/noncopyable.hpp>
//#include <string>
//#include <memory>


namespace gui {
namespace ctrl {
  class GlobalCtx;
}
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
//  class SpotLocator;
  
  
  
  /**
   * Global cross-cutting navigation in interface space
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class Wizard
    : boost::noncopyable
    {
      ctrl::GlobalCtx& globalCtx_;
    
    public:
      Wizard (ctrl::GlobalCtx&);
     ~Wizard ();
      
      void show_HelpAbout();
    private:
      
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_WIZARD_H*/
