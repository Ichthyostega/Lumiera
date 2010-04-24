/*
  ADVICE.hpp  -  generic loosely coupled interaction guided by symbolic pattern
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file advice.hpp
 ** Expecting Advice and giving Advice: a cross-cutting collaboration of loosely coupled participants.
 ** This header exposes the basics of the advice system and the public access points. The advice system
 ** is a system wide singleton service, but clients never talk directly to this singleton; rather they
 ** use advice::Provision and advice::Request as access point.
 ** 
 ** \par Advice collaboration pattern
 ** Advice collaboration is a special pattern of interaction extracted from multiple use cases within
 ** Lumiera. Creating this abstraction was partially inspired by aspect oriented programming, especially
 ** the idea of cross-cutting the primary dependency hierarchy. Another source of inspiration where the
 ** various incarnations of properties with dynamic binding. For defining the actual binding, we rely
 ** on predicate notation and matching (later unification) as known from rule based systems.
 ** 
 ** <b>Definition</b>: Advice is an optional, mediated collaboration between entities taking on
 ** the roles of advisor and advised, thereby passing a custom piece of advice data, managed by
 ** the advice support system. The possibility of advice is created by both of the collaborators
 ** entering the system, where the advised entity exposes a point of advice, while the advising
 ** entity provides an actual advice value.
 ** 
 ** \par Collaborators
 ** - the advised entity
 ** - the advisor
 ** - point of advice
 ** - advice system
 ** - the binding
 ** - the advice
 ** 
 ** Usually, the \em advised entity opens the collaboration by requesting an advice. The \em advice itself
 ** is a piece of data of a custom type, which needs to be copyable. Obviously, both the advised and the
 ** advisor need to share knowledge about the meaning of this advice data. The actual advice collaboration
 ** happens at a \em point-of-advice, which needs to be derived first. To this end, the advised puts up an
 ** \em request by providing his \em binding, which is a pattern for matching. An entity about to give advice
 ** opens possible \advice \em channels by putting up an advisor binding, which similarly is a pattern. The
 ** advice \em system as mediator resolves both sides, by matching (which in the most general case could be
 ** an unification). This process creates an advice point \em solution -- allowing the advisor to fed the
 ** piece of advice into the advice channel, causing it to be placed into the point of advice. After passing
 ** a certain (implementation defined) break point, the advice leaves the influence of the advisor and gets
 ** exposed to the advised entities. Especially this involves copying the advice data into a location managed
 ** by the advice system. In the standard case, the advised entity accesses the advice synchronously and
 ** non-blocking. Typically, the advice data type is default constructible and thus there is always a basic
 ** form of advice available, thereby completely decoupling the advised entity from the timings related
 ** to this collaboration.
 ** 
 ** TODO WIP-WIP
 ** 
 ** @note as of 4/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces. Ichthyo expects this collaboration service to play a central role
 **       at various places within proc-layer.
 ** 
 ** @see configrules.hpp
 ** @see typed-lookup.cpp corresponding implementation
 ** @see typed-id-test.cpp
 ** 
 */


#ifndef LIB_ADVICE_H
#define LIB_ADVICE_H


#include "lib/error.hpp"
//#include "proc/asset.hpp"
//#include "proc/asset/struct-scheme.hpp"
//#include "lib/hash-indexed.hpp"
//#include "lib/util.hpp"
#include "lib/symbol.hpp"
#include "lib/advice/binding.hpp"

//#include <boost/operators.hpp>
//#include <tr1/memory>
//#include <iostream>
//#include <string>

namespace lib    {  ///////TODO: how to arrange the namespaces best?
namespace advice {
  
  /**
   * TODO type comment
   */
  template<class AD>
  class PointOfAdvice
    {
    public:
      /** define or re-define the binding
       *  specifically designating this attachment to the advice system.
       *  @note issuing this on an existing connection is equivalent
       *        to re-connecting with the new binding.  
       */
      void defineBinding (Binding const& binding);
      
      /** access the \em current piece of advice */
      AD const& getAdvice()  const;
      
      
      /* == policy definitions == */    ////TODO: extract into policy classes
      
      AD const& handleMissingSolution()  const;
    };
  
  
  /**
   * Access point for the advising entity (server).
   * TODO type comment
   */
  template<class AD>
  class Provision
    : public PointOfAdvice<AD>
    {
      
    public:
      AD const&
      getAdvice()  const
        {
          UNIMPLEMENTED ("how to embody the piece of advice...");
        }
      
      void setAdvice (AD const& pieceOfAdvice);
      void retractAdvice();
    };
  
  
  /**
   * Access point for the advised entity (client).
   * TODO type comment
   */
  template<class AD>
  class Request
    : public PointOfAdvice<AD>
    {
      Provision<AD>* solution_;
      
    public:
      AD const&
      getAdvice()  const
        {
          if (!solution_)
            return this->handleMissingSolution();
          else
            return solution_->getAdvice();
        }
      
    };
  

  
  
  
}} // namespace lib::advice
#endif
