/*
  SessionServiceAccess(Test)  -  accessing implementation level session services
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/


#include "lib/test/run.hpp"
#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/testsession1.hpp"
#include "proc/mobject/session/session-services.hpp"
#include "lib/singleton.hpp"
//#include "lib/util.hpp"
//#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

//using boost::format;
  using lib::Singleton;
  using boost::lexical_cast;
  using std::ostream;
  using std::string;
  using std::cout;
  using std::endl;


namespace mobject {
namespace session {
namespace test    {
  
  
  namespace { // what follows is a simulated (simplified) version
             //  of the complete Session + SessionManager setup.....
    
    
    /* === Interface level === */
    
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
    
    
    /* === Implementation level === */
    
    struct TSessionImpl : TSession
      {
        static uint magic_;
        
        /* ==== Session API ==== */
        void externalOperation() ;
        
        /* ==== Implementation level API ==== */
        void implementationService() ;
        
        /* ==== internals ==== */
        TSessionImpl()
          {
            ++magic_;
            cout << "creating new Session " << magic_ << endl;
          }
        
        operator string() const
          {
            return string("Session-Impl(")
                 + lexical_cast<string>(magic_)
                 + ")"; 
          }
      };
    
    inline ostream&
    operator<< (ostream& os, TSessionImpl const& simpl)
      {
        return os << string(simpl);
      }
    
    void
    TSessionImpl::externalOperation()
      {
        cout << *this << "::externalOperation()" << endl;
      }
    
    /* ==== Implementation level API ==== */
    void
    TSessionImpl::implementationService()
      {
        cout << *this << "::implementationService()" << endl;
      }
    
    
    
    struct TSessManagerImpl : TSessManager
      {
        scoped_ptr<TSessionImpl> pImpl_;
        
        TSessManagerImpl()
          : pImpl_(0)
          { }
        
        TSessionImpl*
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
            scoped_ptr<TSessionImpl> tmpS (new TSessionImpl);
            pImpl_.swap (tmpS);
          }
      };
    
    
    /* === storage and basic configuration === */
    
    uint TSessionImpl::magic_;
    
    TSessManager& TSession::current = Singleton<TSessManagerImpl>()();
                                    //note: comes up already during static initialisation
    
    
  } // (END) simulated session management
  
  
  
  
  
  /*******************************************************************************
   * Verify the access mechanism used by Proc-Layer internals for 
   * accessing implementation level APIs of the session.
   * 
   * Actually, this test uses setup of the real session,
   * complete with interfaces, implementation and a
   * session manager frontend.
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
          invoke_implService();
        } 
      
      
      void
      access_defaultSession ()
        {
          cout << "Session not yet used...." << endl;
          TSession::current->externalOperation();
        }
      
      
      void
      make_newSession ()
        {
          TSession::current.reset();
          TSession::current->externalOperation();
        }
      
      
      void
      invoke_implService ()
        {
          ///////////////////////////////TODO
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionServiceAccess_test, "function session");
  
  
  
}}} // namespace mobject::session::test
