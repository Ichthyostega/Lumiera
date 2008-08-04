/*
  timeline-ibeam-tool.cpp  -  Implementation of the IBeamTool class
 
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

#include "timeline-ibeam-tool.hpp"

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

IBeamTool::IBeamTool(TimelineWidget *timeline_widget) :
  Tool(timeline_widget)
{
}

ToolType
IBeamTool::get_type() const
{
  return IBeam;
}

Gdk::Cursor
IBeamTool::get_cursor() const
{
  return Gdk::Cursor(Gdk::XTERM);
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

