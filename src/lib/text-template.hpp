/*
  TEXT-TEMPLATE.hpp  -  minimalistic text substitution engine

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file text-template.hpp
 ** A minimalistic text templating engine with flexible data binding.
 ** 
 ** @todo WIP-WIP-WIP 3/2024
 ** @see TextTemplate_test
 ** @see gnuplot-gen.hpp
 ** @see SchedulerStress_test
 */


#ifndef LIB_TEXT_TEMPLATE_H
#define LIB_TEXT_TEMPLATE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-util.hpp"
//#include "lib/util.hpp"

//#include <cmath>
//#include <limits>
#include <vector>
#include <string>
//#include <stdint.h>
//#include <boost/rational.hpp>


namespace lib {
  
//  using Rat = boost::rational<int64_t>;
//  using boost::rational_cast;
//  using std::abs;
  using std::string;
  
  
  namespace {// preconfigured TextTemplate data bindings
    
  }
  
  
  /**
   * Text template substitution engine
   */
  class TextTemplate
    : util::MoveOnly
    {
      enum Clause {
          IF, FOR
        };
      enum Code {
          TEXT, KEY, COND, JUMP, ITER, LOOP
        };
      
      /** cross-references by index number */
      using Idx = size_t;
      
      struct ParseCtx
        {
          Clause clause;
          Idx begin{0};
          Idx after{0};
        };
      
      struct Action
        {
          Code code{TEXT};
          string val{""};
          Idx refIDX{0};
          
          template<class IT>
          string instantiate (IT&);
        };
      
      /** Binding to a specific data source.
       * @note requires partial specialisation */
      template<class DAT, typename SEL=void>
      struct InstanceCore
        {
          static_assert (not sizeof(DAT),
                         "unable to bind this data source "
                         "for TextTemplate instantiation");
        };
      
      /** the text template is compiled into a sequence of Actions */
      using ActionSeq = std::vector<Action>;
      
      using PipeTODO = std::vector<string>;
      using InstanceIter = decltype (explore (std::declval<PipeTODO const&>()));
      
    public:
      TextTemplate(string spec)
        { }
      
      template<class DAT>
      InstanceIter
      render (DAT const& data)  const;
      
      template<class DAT>
      static string
      apply (string spec, DAT const& data);
    };
  
  
  
  /** */
  template<class DAT>
  inline TextTemplate::InstanceIter
  TextTemplate::render (DAT const& data)  const
  {
    UNIMPLEMENTED ("actually instantiate the text template");
  }
  
  /** */
  template<class DAT>
  inline string
  TextTemplate::apply (string spec, DAT const& data)
  {
    return util::join (TextTemplate(spec).render (data)
                      ,"");
  }
  
  
  
}// namespace lib
#endif /*LIB_TEXT_TEMPLATE_H*/
