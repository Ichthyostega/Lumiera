/*
  TRACK-PROFILE.hpp  -  building the 3D profile of tracks for timeline presentation

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file track-profile.hpp
 ** Abstraction to build the layout for the track spaces within timeline display.
 ** In Lumiera, tracks are arranged into a fork of nested shapes, which structure
 ** is parallelled into nested structure of TrackBody elements. A tree walk over
 ** this structure yields a sequence of adjacent timeline elements, like overview
 ** rulers, content area and nested child track display. This sequence can then
 ** be transformed into suitable drawing instructions to create a 3D shaded
 ** display, clearly highlighting the complex structure of the track arrangement.
 ** 
 ** @todo WIP-WIP-WIP as of 6/2019
 ** 
 */


#ifndef STAGE_TIMELINE_TRACK_PROFILE_H
#define STAGE_TIMELINE_TRACK_PROFILE_H

#include "stage/gtk-base.hpp"
#include "lib/verb-visitor.hpp"

#include "lib/symbol.hpp"
#include "lib/util.hpp"

//#include <memory>
#include <utility>
#include <vector>



namespace stage  {
namespace timeline {
  
  using lib::Literal;
  using util::isnil;
  using std::forward;
  
  class ProfileInterpreter
    {
      public:
        virtual ~ProfileInterpreter() { }   ///< this is an interface
        
        virtual void ruler(uint h)   =0;    ///< represent an overview/ruler track with the given height
        virtual void gap(uint h)     =0;    ///< represent a gap to structure the display
        virtual void content(uint h) =0;    ///< represent a content area with the given vertical extension
        virtual void open()          =0;    ///< indicate entering a nested structure, typically as 3D inset
        virtual void close(uint n)   =0;    ///< indicate the end of `n` nested structures, typically by ascending back `n` levels
        virtual void prelude(uint f) =0;    ///< start rack presentation at top of the timeline, with `f` pinned (always visible) elements
        virtual void coda(uint pad)  =0;    ///< the closing part of the timeline at the bottom of the track display, with `pad` additional padding
        
        static const size_t MAX_ARG_SIZE = sizeof(size_t);
    };
  
  /**
   * Description of the structure and arrangement of tracks for display in the UI.
   * This sequence of verb tokens especially details the _profile_ of a vertical
   * cross-section; the nested structure of the track fork is translated into
   * a series of steps and insets, running alongside the timeline display.
   * To decouple the drawing code -- thus allowing for later customisations --
   * we let the individual TrackBody elements just emit these structure description.
   * @todo WIP-WIP as of 4/2019
   */
  struct TrackProfile
    {
      using SlopeVerb = lib::VerbPack<ProfileInterpreter, void, ProfileInterpreter::MAX_ARG_SIZE>;
      using Elements =  std::vector<SlopeVerb>;
      
      Elements elements;
      
      // default constructible, standard copy operations
      
      explicit
      operator bool()  const
        {
          return not isnil (elements);
        }
      
      void
      performWith (ProfileInterpreter& interpreter)
        {
          for (auto& slopeVerb : elements)
              slopeVerb.applyTo (interpreter);
        }
      
    private:/* ===== Internals: handling tokens ===== */
      
      template<typename FUN, typename...ARGS>
      void
      append (FUN&& handler, Literal token, ARGS&&... params)
        {
          elements.emplace_back (forward<FUN>(handler), token, forward<ARGS>(params)...);
        }
      
#define TOKEN_BUILDER(_TOK_)          \
      template<typename...ARGS>        \
      void                              \
      append_ ## _TOK_ (ARGS&&... params)\
        {                                 \
          this->append (&ProfileInterpreter::_TOK_, STRINGIFY(_TOK_), forward<ARGS>(params)...); \
        }
      
    public:
      TOKEN_BUILDER (ruler)
      TOKEN_BUILDER (gap)
      TOKEN_BUILDER (content)
      TOKEN_BUILDER (open)
      TOKEN_BUILDER (close)
      TOKEN_BUILDER (prelude)
      TOKEN_BUILDER (coda)
      
      void
      addSlopeDown()
        {
          this->append_open();
        }
      
      void
      addSlopeUp()
        {
          if (lastEntryIs("close"))
            incrementLastCloseSlope();
          else
            append_close (1);
        }
      
    private:
      bool
      lastEntryIs (Literal expectedToken)
        {
          return not isnil(elements)
             and elements.back()->getID() == expectedToken;
        }
      
      void
      incrementLastCloseSlope()
        {
          REQUIRE (lastEntryIs ("close"));
          using EmbeddedPayload = lib::VerbHolder<ProfileInterpreter, void(uint)>;
          EmbeddedPayload& embedded = static_cast<EmbeddedPayload&>(elements.back().getPayload()); 
          uint& slopeDepth = std::get<0> (embedded.args_);
          
          ++ slopeDepth;
        }
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TRACK_PROFILE_H*/
