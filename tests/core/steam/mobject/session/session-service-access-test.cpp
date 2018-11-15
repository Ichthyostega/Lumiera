/*
  SessionServiceAccess(Test)  -  accessing implementation level session services

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file session-service-access-test.cpp
 ** unit test \ref SessionServiceAccess_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session.hpp"
#include "lib/meta/generator.hpp"
#include "lib/format-cout.hpp"
#include "lib/depend.hpp"

#include <boost/lexical_cast.hpp>
#include <memory>
#include <string>


namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using lib::Depend;
  using boost::lexical_cast;
  using std::unique_ptr;
  using std::string;
  
  
  namespace { // what follows is a simulated (simplified) version
             //  of the complete Session + SessionManager setup.....
    
    using lib::meta::Types;
    using lib::meta::InstantiateChained;
    
    
    
    /* === Interface level === */                              //----------------corresponding-to-session.hpp
    
    struct TSessManager;
    typedef TSessManager& PSess;
    
    struct TSession
      {
        virtual ~TSession () { }
        static TSessManager& current;
        
        virtual void externalOperation ()    =0;
      };
    
    struct TSessManager
      {
        /** access to the current session */
        virtual TSession* operator-> ()      =0;
        
        virtual void reset ()                =0;
        virtual ~TSessManager() { };
      };
    
    
    
    
    /* === Service level API === */                            //----------------internal-API-definition-headers
    
    struct InternalAPI_1
      {
        virtual ~InternalAPI_1() {}
        virtual uint getMagic()     =0;
        
        static InternalAPI_1& access();
      };
    
    struct InternalAPI_2
      {
        static void invokeImplementationService();
      };
    
    
    
    /* === Implementation level === */                         //----------------corresponding-to-session-impl.hpp
    
    struct TSessionImpl : TSession
      {
        static uint magic_;
        
        /* ==== Session API ==== */
        void externalOperation() ;
        
        /* ==== Implementation level API ==== */
        void implementationService() ;
        
        /* ==== internals ==== */
        
        TSessionImpl();
        operator string() const;
      };
    
    
    
    template<class API, class IMPL>
    struct TServiceAccessPoint;
    
    template<class IMPL>
    struct TServiceAccessPoint<InternalAPI_1, IMPL>
      : IMPL
      , InternalAPI_1
      {
        uint
        getMagic ()
          {
            return IMPL::magic_;
          }
      };
    
    template<class IMPL>
    struct TServiceAccessPoint<InternalAPI_2, IMPL>
      : IMPL
      {
        void
        forwardServiceInvocation()
          {
            IMPL::implementationService();
          }
      };
    
    template< typename IMPS
            , class FRONT
            , class SESS
            >
    class TSessionServices
      : public InstantiateChained<typename IMPS::List, TServiceAccessPoint, SESS>
      {
      public:
        
        static FRONT& current;
        
        template<class API>
        API&
        get()
          {
            return *this;
          }
      };
    
    
    
    
    
    /* === storage and basic session manager configuration === */
    
    struct TSessManagerImpl;
    
    typedef TSessionServices< Types<InternalAPI_1,InternalAPI_2>
                            , TSessManagerImpl
                            , TSessionImpl
                            > SessionImplAPI;
    
    
    
    struct TSessManagerImpl : TSessManager
      {
        unique_ptr<SessionImplAPI> pImpl_;
        
        TSessManagerImpl()
          : pImpl_{}
          { }
        
        SessionImplAPI*
        operator-> ()
          {
            if (!pImpl_)
              this->reset();
            return pImpl_.get();
          }
        
        
        /* ==== Manager API ==== */
        void
        reset ()
          {
            unique_ptr<SessionImplAPI> tmpS {new SessionImplAPI};
            pImpl_.swap (tmpS);
          }
      };
    
    
    uint TSessionImpl::magic_;
    
    TSessManager& TSession::current = Depend<TSessManagerImpl>()();
                                     //note: already during static initialisation
    
    template<>
    TSessManagerImpl& SessionImplAPI::current = static_cast<TSessManagerImpl&> (TSession::current);
    
    
    
    
    
    
    
    
    /* === Implementation of service access === */             //----------------corresponding-to-session-services.cpp
    
    InternalAPI_1&
    InternalAPI_1::access()
    {
      return SessionImplAPI::current->get<InternalAPI_1>();
    }
    
    void
    InternalAPI_2::invokeImplementationService()
    {
      SessionImplAPI::current->forwardServiceInvocation();
    }
    
    
    
    
    
    /* === Implementation of Session internals === */          //----------------corresponding-to-session-impl.cpp
      
    TSessionImpl::operator string() const
    {
      return string("Session-Impl(")
           + lexical_cast<string>(magic_)
           + ")"; 
    }
    
    TSessionImpl::TSessionImpl()
    {
      ++magic_;
      cout << "creating new Session " << magic_ << endl;
    }
    
    void
    TSessionImpl::externalOperation()
    {
      cout << this << "::externalOperation()" << endl;
    }
    
    /* ==== Implementation level API ==== */
    inline void
    TSessionImpl::implementationService()
    {
      cout << this << "::implementationService()" << endl;
    }
    
    
  } // (END) simulated session management
  
  
  
  
  
  
  
  
  
  
  /***************************************************************************//**
   * Verify the access mechanism both to the pubic session API and
   * to implementation level APIs used by Steam-Layer internals.
   * 
   * Actually, this test uses a simulated setup of the real session,
   * complete with interfaces, implementation and session manager frontend.
   * 
   * @see session-impl.hpp the real thing
   * @see SessionServices; 
   */
  class SessionServiceAccess_test : public Test
    {
      virtual void
      run (Arg) 
        {
          access_defaultSession();
          make_newSession();
          invoke_implServices();
        } 
      
      
      /** @test accessing an non-existing session
       *        causes creation of a new TSessionImpl instance.
       *        After that, the public API function gets invoked. 
       */
      void
      access_defaultSession ()
        {
          cout << "Session not yet used...." << endl;
          TSession::current->externalOperation();
        }
      
      
      /** @test invoking the management API to close the session.
       *        The next public API invocation will create
       *        a new TSessionImpl instance. 
       */
      void
      make_newSession ()
        {
          TSession::current.reset();
          TSession::current->externalOperation();
        }
      
      
      /** example of an one-liner, as it might be used
       *  internally by implementation code within Steam-Layer */
      uint magic() { return InternalAPI_1::access().getMagic(); }
      
      /** @test accessing implementation-level APIs */
      void
      invoke_implServices ()
        {
          cout << "current Session-Impl-ID = " << magic() << endl;
          InternalAPI_2::invokeImplementationService();
          
          cout << "now resetting this session." << endl;
          TSession::current.reset();
          
          InternalAPI_2::invokeImplementationService();  // invocation creates new session as side effect
          cout << "current Session-Impl-ID = " << magic() << endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionServiceAccess_test, "function session");
  
  
  
}}}} // namespace proc::mobject::session::test
