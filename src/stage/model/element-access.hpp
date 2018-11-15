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
 ** generates a control structure of its own. And another kind of generic access happens
 ** when we _navigate_ the topological UI structure for focus management.
 ** 
 ** This interface defines an abstract service to translate a generic element designation
 ** into a (C++ language level) access to internal structures of the UI toolkit (GTK in our case).
 ** This access to low-level structures proceeds in two stages:
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


#ifndef STAGE_MODEL_ELEMENT_ACCESS_H
#define STAGE_MODEL_ELEMENT_ACCESS_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/result.hpp"
#include "include/limits.h"
#include "lib/variant.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/access-casted.hpp"
#include "stage/interact/ui-coord.hpp"

#include <string>


namespace Gtk {
  class Widget;
}
namespace stage {
namespace model {
  namespace error = lumiera::error;
  
  using interact::UICoord;
  using lib::meta::Types;
  using std::string;
  
  class Tangible;
  
  /**
   * Interface: access UI elements by navigating the UI topology.
   * 
   * @see stage::interact::Navigator
   * @see ElementAccess_test
   */
  class ElementAccess
    : util::NonCopyable
    {
      
    public:
      virtual ~ElementAccess () { }   ///< this is an interface
      
      
      /* == Access by Location == */
      
      template<class TAR>
      lib::Result<TAR&> access (UICoord const& destination);
      
      UICoord locate_or_create (UICoord const& destination, size_t limitCreation = LUMIERA_MAX_ORDINAL_NUMBER);
      
      
    protected:
      using RawResult = lib::Variant<Types<model::Tangible*, Gtk::Widget*>>;
      
      /** @internal drill down according to coordinates, maybe create element */
      virtual RawResult performAccessTo (UICoord::Builder &, size_t limitCreation)  =0;
      
    private:
      template<class TAR>
      struct TypeConverter;
    };
  
  
  
  
  /** @internal helper to perform conversion to the desired result type.
   * This is a bit of tricky metaprogramming to deal with the problem that we can not
   * assume a single base interface for all the UI elements or widgets accessible through
   * UI-Coordinates. Rather we have to deal with a small set of possible base interfaces,
   * and thus the actual [access function](\ref #performAccessTo) returns a _variant record_
   * holding a pointer, and internally tagged with the base interface type to apply. Now the
   * public API functions are templated to the _result type as expected by the invoking clinent_
   * and thus we get a matrix of possible cases; when the expected result type is _reachable by
   * dynamic downcast_ from the actual base interface type returned by the internal access function,
   * we can perform this `dynamic_cast`. Otherwise the returned result proxy object is marked as empty.
   * @remark technically this solution relies on the lib::Variant::Visitor to provide a NOP default
   *         implementation. The TypeConverter template is instantiated with the desired target type
   *         and thus only overwrites _the first case where an conversion is possible._ In all other
   *         cases the default implementation does nothing and thus retains the embedded result proxy
   *         in empty state.
   */
  template<class TAR>
  struct ElementAccess::TypeConverter
    : RawResult::Visitor
    {
      lib::Result<TAR&> result{"not convertible to desired target widget"};
      
      template<typename X>                             // note the "backward" use. We pick that base interface
      using canUpcast = std::is_convertible<TAR*, X>; //  into which our desired result type can be upcast, because
                                                     //   we know the then following dynamic_cast (downcast) can succeed
      using Base = typename RawResult::FirstMatching<canUpcast>::Type;
      
      virtual void
      handle (Base& pb)  override
        {
          if (pb)
            {
              TAR* downcasted = dynamic_cast<TAR*> (pb);
              if (downcasted)
                result = *downcasted;
            }
          else
            result = lib::Result<TAR&>{"access returns empty answer"};
        }
    };
  
  
  
  
  
  
  /** Navigate the UI topology to access the designated component.
   * @tparam TAR type of result element expected at the designated location
   * @return suitably converted direct (language) reference to the desired element
   *         wrapped as _result proxy_
   * @note when access was not possible because either the element does not exist,
   *       or is not convertible to the desired target type, the result proxy is empty
   *       and convertible to `bool(false)`
   */
  template<class TAR>
  inline lib::Result<TAR&>
  ElementAccess::access (UICoord const& destination)
  {
    UICoord::Builder targetLocation{destination.rebuild()};
    TypeConverter<TAR> converter;
    RawResult targetElm = performAccessTo (targetLocation, 0);
    targetElm.accept (converter);
    return converter.result;
  }
  
  
  /** Navigate to the designated component, possibly create the element and parents
   * @param limitCreation limit on the number of similar UI elements that can be created
   *         within a scope. A value of zero prohibits creating of new elements.
   * @return location of the actual element to use, as UI-coordinates.
   * @note when access was not possible because the element could not been created,
   *       the indicated location returned is an _empty coordinate spec._
   */
  inline UICoord
  ElementAccess::locate_or_create (UICoord const& destination, size_t limitCreation)
  {
    UICoord::Builder targetLocation{destination.rebuild()};
    performAccessTo (targetLocation, limitCreation);
    return targetLocation;
  }
  
  
  
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_ELEMENT_ACCESS_H*/
