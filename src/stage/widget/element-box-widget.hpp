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
 ** This is a UI building block used at various places, e.g. to show the name of a clip
 ** in the media bins or in the timeline, to indicate the kind of an effect, or to render
 ** a marker label in the tracks or the timeline ruler. The common denominator of all those
 ** elements is that they combine some icon with a possibly abridged text and render them
 ** with a given indicator style, configurable via CSS. There is support for picking the
 ** icon and the indicator style based on some notion of _"type"._
 ** 
 ** # Usage
 ** Build a custom widget with ElementBoxWidget as base class; invoke the base ctor using
 ** the DSL qualifier syntax to select from the preconfigured layout options
 ** - the stage::widget::Kind specifies the basic usage situation
 ** - expand / collapse functionality can be directly wired with a model::Tangible
 ** - a special _size constrained layout_ mode can be activated, to allow for display
 **   on a time calibrated canvas; the actual translation from time to pixel size must
 **   be done by the concrete subclass (e.g. stage::model::Clip), accessible through
 **   the _getter-λ_ passed to the `constrained(λ)` clause.
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

#include "lib/format-string.hpp" ////////TODO debugging
#include "lib/symbol.hpp"

#include <functional>
#include <utility>
#include <string> //////TODO debugging
#include "lib/format-cout.hpp" ////////TODO debugging



namespace stage  {
namespace widget {
  
  using std::move;
  using lib::Literal;
  using std::string; /////TODO
  using util::_Fmt;  /////TODO debugging?
  
  /** the presentation intent for the ElementBoxWidget */
  enum Kind { MARK     ///< Widget is a pin or marks a position
            , SPAN     ///< Widget spans a time range
            , ITEM     ///< Widget represents an entity within a collection (Bin)
            , CONTENT  ///< Widget serves to represent a piece of content (Clip)
            };
  
  /** the type of content object to derive suitable styling (background colour, icon) */
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
  
  using SizeGetter = std::function<int()>;
  
  /**
   * A basic building block of the Lumiera UI.
   * Representation of an entity, with a marker icon, a menu, descriptive label and
   * possibly a content renderer (e.g. for a video clip). Depending on the presentation intent,
   * the widget can extend to a defined time range horizontally. Pre-defined styling and bindings
   * to expand the display and to invoke a menu are provided
   * @todo consider policy based design,   //////////////////////////////////////////////////////////////////TICKET #1239 : re-evaluate Design, maybe use Policy Based Design
   *       but need more exposure and real world usage as of 9/22
   */
  class ElementBoxWidget
    : public Gtk::Frame
    {
      using _Base = Gtk::Frame;
      struct Strategy
        {
          SizeGetter getWidth{};
          SizeGetter getHeight{};
          
          bool is_size_constrained()  const { return bool(getWidth); }
          bool shall_control_height() const { return bool(getHeight); }
        };
      
      /** actual layout strategy binding for this widget */
      Strategy strategy_;
      
      Gtk::Box label_;
      Gtk::Image icon_;
      Gtk::Label name_;
      
    public:
      class Config;
      
      template<class... QS>
      ElementBoxWidget (Kind kind, Type type, QS ...qualifiers);
      
      ElementBoxWidget (Config);
     ~ElementBoxWidget();
      
      // default copy acceptable
      
    private:/* ===== Internals ===== */
      
      Gtk::SizeRequestMode get_request_mode_vfunc()              const final;
      void get_preferred_width_vfunc (int&, int&)                const override;
      void get_preferred_height_vfunc (int&, int&)               const override;
      void get_preferred_height_for_width_vfunc (int, int&,int&) const override;
      void on_size_allocate (Gtk::Allocation&)                         override;
      
      void imposeSizeConstraint(int, int);
      Gtk::Requisition labelFullSize_{};
    };
  
  
  class ElementBoxWidget::Config
    : lib::BuilderQualifierSupport<Config>
    {
      Type type_;
      uString nameID_{"∅"};
      SizeGetter widthConstraint_;
      SizeGetter heightConstraint_;
      string logTODO_{nameID_};    //////////////////////////////////////////////////////////////////////////TICKET #1219 : Placeholder for detailed layout configuration
      
      friend Qualifier kind(Kind);
      friend Qualifier name(string id);
      friend Qualifier expander(model::Expander&);
      friend Qualifier constrained(SizeGetter);
      friend Qualifier constrained(SizeGetter,SizeGetter);
      
      public:
        template<class... QS>
        Config(Type type, Qualifier qual, QS... qs)
          : type_{type}
        {
          qualify(*this, qual, qs...);
        }
        
        /** decide upon the presentation strategy */
        Strategy buildLayoutStrategy(ElementBoxWidget&);
        
        Literal getIconID()  const;
        Gtk::IconSize getIconSize() const;
        
        cuString
        getName()  const
          {
            return nameID_;
          }
    };
  
  
  /** qualify the basic use case for the new ElementBoxWidget */
  inline ElementBoxWidget::Config::Qualifier
  kind(Kind kind)
  {
    return ElementBoxWidget::Config::Qualifier{
            [=](ElementBoxWidget::Config& config)
              {
                config.logTODO_ += util::_Fmt{"+kind(%s)"} % kind;
              }};
  }
  
  /** define the name-ID displayed in the caption */
  inline ElementBoxWidget::Config::Qualifier
  name(string id)
  {
    return ElementBoxWidget::Config::Qualifier{
            [=](ElementBoxWidget::Config& config)
              {
                config.nameID_ = id;
              }};
  }
  
  /** provide an expand/collapse button, wired with the given Expander */
  inline ElementBoxWidget::Config::Qualifier
  expander(model::Expander& expander)
  {
    return ElementBoxWidget::Config::Qualifier{
            [&](ElementBoxWidget::Config& config)
              {
                config.logTODO_ += util::_Fmt{"+expander(%s)"} % &expander;
              }};
  }
  
  /**
   * switch in to size-constrained layout mode.
   * The base size allocation (without borders and margin) will span exactly
   * the horizontal extension as retrieved from invoking the SizeGetter.
   */
  inline ElementBoxWidget::Config::Qualifier
  constrained(SizeGetter widthConstraint)
  {
    return ElementBoxWidget::Config::Qualifier{
            [wC=move(widthConstraint)](ElementBoxWidget::Config& config) -> void
              {
                config.widthConstraint_ = move(wC);
              }};
  }
  
  /**
   * activate size-constrained layout mode, similar as #constrained(SizeGetter).
   * Additionally, also the vertical extension is controlled in this variant.
   */
  inline ElementBoxWidget::Config::Qualifier
  constrained(SizeGetter widthConstraint, SizeGetter heightConstraint)
  {
    return ElementBoxWidget::Config::Qualifier{
            [wC=move(widthConstraint),hC=move(heightConstraint)]
            (ElementBoxWidget::Config& config) -> void
              {
                config.widthConstraint_ = move(wC);
                config.heightConstraint_ = move(hC);
              }};
  }
  
  
  
  /** setup an ElementBoxWidget with suitable presentation style.
   * @param widgetKind the basic presentation intent
   * @param type       qualify the type of data represented by this object
   * @param qualifiers pass further qualifiers to fine-tune the presentation
   */
  template<class... QS>
  inline ElementBoxWidget::ElementBoxWidget (Kind widgetKind, Type type, QS ...qualifiers)
    : ElementBoxWidget{Config(type, kind(widgetKind), qualifiers...)}
  { }
  
  
}}// namespace stage::widget
#endif /*STAGE_WIDGET_ELEMENT_BOX_WIDGET_H*/
