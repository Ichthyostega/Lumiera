/*
  button-bar.cpp  -  Implementation of the button bar widget

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

* *****************************************************/

#include "gui/widgets/button-bar.hpp"
#include "include/logging.h"

#include <algorithm>

using namespace Gtk;
using namespace Glib;
using namespace sigc;
using namespace std;

namespace gui {
namespace widgets {

ButtonBar::ButtonBar()
: last_width (calculate_width())
{
  set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  set_has_window(false);
}

void
ButtonBar::append(Widget &widget)
{
  pack_start(widget, Gtk::PACK_SHRINK);
  last_width = calculate_width();
}


void
ButtonBar::on_size_request(Gtk::Requisition* requisition)
{
#if 0
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
#endif
}

int
ButtonBar::calculate_width()
{
  typedef vector<Widget*> BoxList;
  BoxList list = get_children();
  BoxList::const_iterator i;

  int w = 0;
  for(i = list.begin(); i != list.end(); i++)
    {
      Widget *widget = *i;
      REQUIRE(widget);
      w += widget->get_width();
    }
  REQUIRE(w >= 0);
  return w;
}

Gtk::SizeRequestMode
ButtonBar::get_request_mode_vfunc() const
{
  return Gtk::SIZE_REQUEST_CONSTANT_SIZE;
}

void
ButtonBar::get_preferred_width_vfunc(int& minimum_width,
                                     int& natural_width) const
{
  minimum_width = natural_width = last_width;
}

void
ButtonBar::get_preferred_height_for_width_vfunc(int width,
                                                int& minimum_height,
                                                int& natural_height) const
{
  Gtk::Box::get_preferred_height_for_width_vfunc(
      width, minimum_height, natural_height
  );
}

void
ButtonBar::get_preferred_height_vfunc(int& minimum_height,
                                      int& natural_height) const
{
  FIXME("Calculate height from child widgets");
  minimum_height = natural_height = 30;
}

void
ButtonBar::get_preferred_width_for_height_vfunc(int height,
                                                int& minimum_width,
                                                int& natural_width) const
{
  minimum_width = natural_width = last_width;
}


void
ButtonBar::on_size_allocate(Gtk::Allocation& allocation)
{
  //Use the offered allocation for this container:
  set_allocation(allocation);
  
  int offset = 0;
  typedef vector<Widget*> BoxList;
  BoxList list = get_children();
  BoxList::const_iterator i;
  
  for(i = list.begin(); i != list.end(); i++)
    {
      Widget *widget = *i;
      REQUIRE(widget);
     
      int cw,ch;
      widget->get_size_request(cw,ch);
      Gtk::Allocation child_allocation(
          allocation.get_x() + offset,
          allocation.get_y(),
          cw, ch);
          
      offset += cw;
      
      if(get_direction() == TEXT_DIR_RTL)
        {
          child_allocation.set_x(
            2*allocation.get_x() + allocation.get_width() - 
            child_allocation.get_x() - child_allocation.get_width());
        }
      
      if(offset > allocation.get_width())
        widget->set_child_visible(false);
      else
        {
          widget->size_allocate(child_allocation);        
          widget->set_child_visible(true);
        }
    }

  /* In case we get resized */
  last_width = calculate_width();
}


} // widgets
} // gui
