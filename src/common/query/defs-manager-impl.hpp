/*
  DEFS-MANAGER-IMPL.h  -  access to preconfigured default objects and definitions

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file defs-manager-impl.hpp
 ** Implementation of the core defaults-management operations.
 ** These generic implementations are factored out into a separate header (include)
 ** to improve readability. To actually generate code, it is necessary to pull in this
 ** include to create explicit template instantiations with the concrete types to be used
 ** for definition and retrieval of default-configured objects.
 ** 
 ** For the standard use-case within the session / Steam-Layer, this is performed for the
 ** core MObject types, alongside with the definition of the generic config-query-resolver.
 ** 
 ** @see config-resolver.cpp definition of the explicit specialisations for the session 
 ** @see steam::ConfigResolver
 **
 */




#ifndef LUMIERA_QUERY_DEFS_MANAGER_IMPL_H
#define LUMIERA_QUERY_DEFS_MANAGER_IMPL_H


#include "common/query/defs-manager.hpp"
#include "common/query/defs-registry.hpp"
#include "common/config-rules.hpp"
#include "lib/format-string.hpp"
#include "lib/error.hpp"

using util::_Fmt;

using steam::ConfigResolver;
using lumiera::query::QueryHandler;                    ///////TODO preliminary interface defined in config-rules.hpp


namespace lumiera{
namespace query  {
  
  
  
  
  /** initialise the most basic internal defaults. */
  DefsManager::DefsManager ()  noexcept
    : defsRegistry_(new DefsRegistry)
  {
    INFO (session, "Configure technical defaults of the session.");
    
  }
  
  
  
  /** @internal causes std::default_delete from `unique_ptr<DefsRegistry>`
   *  to be emitted here, where the declaration of DefsRegistry is available. */
  DefsManager::~DefsManager() {}
  
  
  void
  DefsManager::clear()
  {
    defsRegistry_.reset(new DefsRegistry);
  }
  
  
  
  template<class TAR>
  P<TAR>
  DefsManager::search  (const Query<TAR>& capabilities)
  {
    P<TAR> res;
    QueryHandler<TAR>& typeHandler = ConfigResolver::instance();
    for (DefsRegistry::Iter<TAR> i = defsRegistry_->candidates(capabilities);
         bool(*i) ; ++i )
      {
        res = *i;
        typeHandler.resolve (res, capabilities);
        if (res)
          return res;
      }
    return res; // "no solution found"
  }
  
  
  template<class TAR>
  P<TAR> 
  DefsManager::create  (const Query<TAR>& capabilities)
  {
    P<TAR> res;
    QueryHandler<TAR>& typeHandler = ConfigResolver::instance();
    typeHandler.resolve (res, capabilities);
    if (res)
      defsRegistry_->put (res, capabilities);
    return res;
  }
  
  
  template<class TAR>
  bool 
  DefsManager::define  (const P<TAR>& defaultObj, const Query<TAR>& capabilities)
  {
    P<TAR> candidate (defaultObj);
    QueryHandler<TAR>& typeHandler = ConfigResolver::instance();  
    typeHandler.resolve (candidate, capabilities);
    if (!candidate)
      return false;
    else
      return defsRegistry_->put (candidate, capabilities);
  }
  
  
  template<class TAR>
  bool 
  DefsManager::forget  (const P<TAR>& defaultObj)
  {
    return defsRegistry_->forget (defaultObj);
  }
  
  
  template<class TAR>
  P<TAR>
  DefsManager::operator() (const Query<TAR>& capabilities)
  {
    P<TAR> res (search (capabilities));
    if (res) 
      return res;
    else
      res = create (capabilities); // not yet known as default, create new
    
    if (!res)
      throw error::Config (_Fmt("The following Query could not be resolved: %s.")
                               % capabilities.rebuild().asKey()
                          , LERR_(CAPABILITY_QUERY) );
    else
      return res;
  }
  
}} // namespace lumiera::query
#endif /* LUMIERA_QUERY_DEFS_MANAGER_IMPL_H */
