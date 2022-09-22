/*
  ELEMENT-BOX-WIDGET.hpp  -  fundamental UI building block to represent a placed element

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file element-box-widget.hpp
 ** Widget to render an ID label with associated icon.
 ** This is a UI building block used at various places, e.g. to show
 ** the name of a clip in the media bins or in the timeline, to indicate
 ** the kind of an effect, or to render a marker label in the tracks or
 ** the timeline ruler. The common denominator of all those elements is
 ** that they combine some icon with a possibly abridged text and render
 ** them with a given indicator style, configurable via CSS. There is
 ** support for picking the icon and the indicator style based on some
 ** notion of _"type"._ 
 ** 
 ** @todo WIP-WIP-WIP as of 11/2018   ///////////////////////////////////////////////////////////////////////TICKET #1185
 ** @todo WIP-WIP-WIP as of 9/2022    ///////////////////////////////////////////////////////////////////////TICKET #1219
 ** 
 */


#ifndef STAGE_WIDGET_ELEMENT_BOX_WIDGET_H
#define STAGE_WIDGET_ELEMENT_BOX_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/model/expander-revealer.hpp"
#include "lib/builder-qualifier-support.hpp"

//#include "lib/util.hpp"
#include "lib/symbol.hpp"
#include "lib/format-string.hpp"

//#include <memory>
//#include <vector>
#include <string> //////TODO debugging



namespace stage  {
namespace widget {
  
  using lib::Literal;
  using std::string; /////TODO
  using util::_Fmt;  /////TODO debugging?
  
  /** the presentation intent for the ElementBoxWidget */
  enum Kind { MARK     ///< Widget is a pin or marks a position
            , SPAN     ///< Widget spans a time range
            , ITEM     ///< Widget represents an entity within a collection (Bin)
            , CONTENT  ///< Widget serves to represent a piece of content (Clip)
            };
  
  /** the type of content object to derive suitable styling */
  enum Type { VIDEO    ///< represents moving (or still) image data
            , AUDIO    ///< represents sound data
            , TEXT     ///< represents text content
            , AUTO     ///< represents automation
            , EVENT    ///< represents event streams or live connections
            , EFFECT   ///< represents a processor or transformer
            , LABEL    ///< represents a label or descriptor
            , RULER    ///< represents an overview ruler or TOC
            , GROUP    ///< represents a container to group other entities
            , META     ///< represents some meta entity
            };
  
  
  /**
   * A basic building block of the Lumiera UI.
   * Representation of an entity, with a marker icon, a menu, descriptive label and
   * possibly a content renderer (e.g. for a video clip). Depending on the presentation intent,
   * the widget can extend to a defined time range horizontally. Pre-defined styling and bindings
   * to expand the display and to invoke a menu are provided
   */
  class ElementBoxWidget
    : public Gtk::Frame
    {
      Gtk::Box label_;
      Gtk::Image icon_;
      Gtk::Label name_;
      
    public:
      class Strategy;
      
      template<class... QS>
      ElementBoxWidget (Kind kind, Type type, QS ...qualifiers);
      
      ElementBoxWidget (Strategy);
     ~ElementBoxWidget();
      
      // default copy acceptable
     
    private:/* ===== Internals ===== */
     
    };
  
  class ElementBoxWidget::Strategy
    : lib::BuilderQualifierSupport<Strategy>
    {
      Type type_;
      uString nameID_{"∅"};
      string logTODO_{nameID_};
      
      friend Qualifier kind(Kind);
      friend Qualifier name(string id);
      friend Qualifier expander(model::Expander&);
      
      public:
        template<class... QS>
        Strategy(Type type, Qualifier qual, QS... qs)
          : type_{type}
        {
          qualify(*this, qual, qs...);
        }
        
        /** decide upon the presentation strategy */
        void configure();
        
        Literal getIconID()  const;
        Gtk::IconSize getIconSize() const;
        
        cuString
        getName()  const
          {
            return nameID_;
          }
    };
  
  
  inline ElementBoxWidget::Strategy::Qualifier
  kind(Kind kind)
  {
    return ElementBoxWidget::Strategy::Qualifier{
            [=](ElementBoxWidget::Strategy& strategy)
              {
                strategy.logTODO_ += util::_Fmt{"+kind(%s)"} % kind;
              }};
  }
  
  inline ElementBoxWidget::Strategy::Qualifier
  name(string id)
  {
    return ElementBoxWidget::Strategy::Qualifier{
            [=](ElementBoxWidget::Strategy& strategy)
              {
                strategy.nameID_ = id;
              }};
  }
  
  inline ElementBoxWidget::Strategy::Qualifier
  expander(model::Expander& expander)
  {
    return ElementBoxWidget::Strategy::Qualifier{
            [&](ElementBoxWidget::Strategy& strategy)
              {
                strategy.logTODO_ += util::_Fmt{"+expander(%s)"} % &expander;
              }};
  }
  
  
  /** setup an ElementBoxWidget with suitable presentation style.
   * @param widgetKind the basic presentation intent
   * @param type       qualify the type of data represented by this object
   * @param qualifiers pass further qualifiers to fine-tune the presentation
   */
  template<class... QS>
  inline ElementBoxWidget::ElementBoxWidget (Kind widgetKind, Type type, QS ...qualifiers)
    : ElementBoxWidget{Strategy(type, kind(widgetKind), qualifiers...)}
  { }
  
  
}}// namespace stage::widget
#endif /*STAGE_WIDGET_ELEMENT_BOX_WIDGET_H*/
