/*
  button-bar.cpp  -  Implementation of the button bar widget
 
  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
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

#include "button-bar.hpp"

#include <nobug.h>
#include <algorithm>

using namespace Gtk;
using namespace Glib;
using namespace sigc;
using namespace std;

namespace gui {
namespace widgets {

ButtonBar::ButtonBar()
{
  set_flags(Gtk::NO_WINDOW);
}

void
ButtonBar::append(Widget &widget)
{
  pack_start(widget, Gtk::PACK_SHRINK);
}

void
ButtonBar::on_size_request(Gtk::Requisition* requisition)
{
  REQUIRE(requisition);
    
  requisition->width = 0;
  requisition->height = 0;
  
  Box::BoxList &list = children();
  Box::BoxList::const_iterator i;
  for(i = list.begin(); i != list.end(); i++)
    {
      Widget *widget = (*i).get_widget();
      REQUIRE(widget);
      
      Requisition child_requisition = widget->size_request();
      requisition->width += child_requisition.width;
      requisition->height = max(requisition->height,
        child_requisition.height);
    }
  
  ENSURE(requisition->width >= 0);
  ENSURE(requisition->height >= 0);
}
  
void
ButtonBar::on_size_allocate(Gtk::Allocation& allocation)
{
  //Use the offered allocation for this container:
  set_allocation(allocation);
  
  int offset = 0;
  Box::BoxList &list = children();
  Box::BoxList::const_iterator i;
  
  for(i = list.begin(); i != list.end(); i++)
    {
      Widget *widget = (*i).get_widget();
      REQUIRE(widget);
     
      const Requisition child_requisition = widget->size_request();
      const Gtk::Allocation child_allocation(
          allocation.get_x() + offset,
          allocation.get_y(),
          child_requisition.width,
          allocation.get_height());
          
      offset += child_requisition.width;
      
      if(offset > allocation.get_width())
        widget->set_child_visible(false);
      else
        {
          widget->size_allocate(child_allocation);        
          widget->set_child_visible(true);
        }
    }
}

} // widgets
} // gui
