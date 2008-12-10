/*
  track-track.hpp  -  Definition of the TrackBase class
 
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
/** @file track-base.hpp
 ** This file contains the definition of TrackBase, a class which
 ** represents a base functionality for tracks and sequences which
 ** are also track parents. This class wraps proc layer data
 */

#include "../gtk-lumiera.hpp"

#ifndef TRACK_BASE_HPP
#define TRACK_BASE_HPP

namespace gui {
namespace model {

class Track;

class TrackBase
{
public:
  TrackBase();

  virtual void add_child_track(Track* child) {};

  virtual const std::list< boost::shared_ptr<Track> >&
    get_child_tracks() const;
    
  const Glib::ustring get_name() const;
  
  void set_name(const Glib::ustring &name);
    
private:
  Glib::ustring name;

  static const std::list< boost::shared_ptr<Track> > NoChildren;
};

}   // namespace model
}   // namespace gui

#endif // TRACK_BASE_HPP
