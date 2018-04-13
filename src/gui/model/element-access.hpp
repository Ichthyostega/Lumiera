/*
  ELEMENT-ACCESS.hpp  -  access to generic elements in the UI

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file element-access.hpp
 ** Interface to discover and access raw UI elements in a cross cutting way.
 ** We have several orthogonal identification and access schemes within the UI.
 ** A naively written UI application just attaches the core logic below some widgets and
 ** controllers -- not only does this lead to a hard to maintain codebase, this approach
 ** is even outright impossible for Lumiera, since the core is able to run standalone and
 ** the UI is loaded as plug-in, which places us into the situation to connect a self
 ** contained core with a self contained UI. This is a binding, which, as a sideline, also
 ** generates a control structure of its own. An another kind of generic access happens
 ** when we _navigate_ the topological UI structure for focus management.
 ** 
 ** This interface defines an abstract service to translate a generic element designation
 ** into a (language level) access to internal structures of the UI toolkit (GTK in our case).
 ** This access to low-level structure proceeds in two stages:
 ** - navigate down the UI topology. Optionally, this may involve a mutation (create element)
 ** - evaluate the result (found, not found, element created) and access the target,
 **   possibly with conversion (which might fail)
 ** 
 ** @note as of 4/2018 this is a first draft and WIP-WIP-WIP
 ** @todo WIP  ///////////////////////TICKET #1134
 ** 
 ** @see ElementAccess_test
 ** @see elem-access-dir.hpp implementation
 ** 
 */


#ifndef GUI_MODEL_ELEMENT_ACCESS_H
#define GUI_MODEL_ELEMENT_ACCESS_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/result.hpp"
#include "include/limits.h"
#include "lib/variant.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/access-casted.hpp"
#include "gui/interact/ui-coord.hpp"
//#include "lib/format-string.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"
#include "lib/test/test-helper.hpp" ////////TOD-o

#include <string>


namespace Gtk {
  class Widget;
}
namespace gui {
namespace model {
  namespace error = lumiera::error;
  
  using interact::UICoord;
  using lib::meta::Types;
//  using util::isnil;
  using std::string;
  
  class Tangible;
  
  /**
   * Interface: access UI elements by navigating the UI topology.
   * 
   * @see gui::interact::Navigator
   * @see ElementAccess_test
   */
  class ElementAccess
    : util::NonCopyable
    {
      
    public:
      virtual ~ElementAccess () { }   ///< this is an interface
      
      
      template<class TAR>
      using Result = lib::Result<TAR>;
      
      /* == Access by Location == */
      
      template<class TAR>
      Result<TAR&> access (UICoord destination);
      
      template<class TAR>
      Result<TAR&> access_or_create (UICoord destination, size_t limitCreation = LUMIERA_MAX_ORDINAL_NUMBER);
      
      
    protected:
      using RawResult = lib::Variant<Types<model::Tangible*, Gtk::Widget*>>;
      
      /** @internal drill down according to coordinates, maybe create element */
      virtual RawResult performAccessTo (UICoord, size_t limitCreation)    =0;
      
    private:
      template<class TAR>
      struct TypeConverter;
    };
  
  
  namespace {
    using TypeSeq = Types<model::Tangible*, Gtk::Widget*>;
    
    using lib::meta::Node;
    using lib::meta::NullType;
    
    template<typename T>
    struct Identity
      {
        using Type = T;
      };
    
    template<class TYPES, template<class> class _P_>
    struct FirstMatching
      {
        static_assert(not sizeof(TYPES), "None of the possible Types fulfils the condition");
      };
    template<class...TYPES, template<class> class _P_>
    struct FirstMatching<Types<TYPES...>, _P_>
      : FirstMatching<typename Types<TYPES...>::List, _P_>
      { };
    template<class T, class TYPES, template<class> class _P_>
    struct FirstMatching<Node<T,TYPES>, _P_>
      : std::conditional_t<_P_<T>::value, Identity<T>, FirstMatching<TYPES, _P_>>
      {  };
    
  }
  template<class TAR>
  struct ElementAccess::TypeConverter
    : RawResult::Visitor
    {
      Result<TAR&> result;
      
      template<typename X>
      using canCast = std::is_convertible<TAR*, X>;
      
      using Base = typename FirstMatching<TypeSeq, canCast>::Type;
      
      void
      accept (Base* pb)
        {
          if (pb)
            result = *dynamic_cast<TAR*> (pb);
        }
    };
  
  
  
  
  
  
  /** Navigate the UI topology to access the designated component.
   * @tparam TAR type of result element expected at the designated location
   * @return suitably converted direct (language) reference to the desired element
   *         wrapped as _result proxy_
   * @throw error::Invalid when the designated element exists, but is not
   *        type or conversion compatible to the expected result type
   * @note when access was not possible because the element does not exist,
   *       the result proxy is empty and convertible to `bool(false)`
   */
  template<class TAR>
  inline ElementAccess::Result<TAR&>
  ElementAccess::access (UICoord destination)
  {
    return access_or_create<TAR> (destination, 0);
  }
  
  
  /** Navigate to the designated component, possibly create the element and parents
   * @return suitably converted direct (language) reference to the desired element
   *         wrapped as _result proxy_
   * @note when access was not possible because the element could not been created,
   *       the result proxy is empty and convertible to `bool(false)`
   */
  template<class TAR>
  inline ElementAccess::Result<TAR&>
  ElementAccess::access_or_create (UICoord destination, size_t limitCreation)
  {
    TypeConverter<TAR> converter;
    RawResult targetElm = performAccessTo (destination, limitCreation);
    targetElm.accept (converter);
    if (converter.result.isValid())
      return converter.result;
    else
      return "In current UI, there is no element at location "+string(destination);
  }
  
  
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_ELEMENT_ACCESS_H*/
