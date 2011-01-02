/*
  controller.hpp  -  Declaration of the controller object

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>

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
/** @file controller/controller.hpp
 ** This file contains the definition of the controller object
 */

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "playback-controller.hpp"


namespace gui {

namespace model {
class Project;
} // namespace model
  
namespace controller { 

class Controller
{
public:
  Controller(model::Project &modelProject);

  PlaybackController& get_playback_controller();

private:
  model::Project &project;
  
  PlaybackController playback;
};

}   // namespace controller
}   // namespace gui

#endif // CONTROLLER_HPP

