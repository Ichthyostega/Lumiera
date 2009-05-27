/*
  PlacementHierarchy(Test)  -  concept draft how to deal with the MObject hierarchy in Placements
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-ref.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "lib/util.hpp"

#include <boost/format.hpp>
#include <tr1/memory>
#include <iostream>
#include <cstdlib>

using std::tr1::shared_ptr;
using boost::format;
//using lumiera::Time;
//using util::contains;
using std::string;
using std::cout;
using std::rand;


namespace mobject {
namespace session {
namespace test    {
  
  
  /**
   * a dummy version of the MObject hierarchy,
   * used to experiment with the Placement template structure
   */
  struct MOj;
  

  template<class TY, class B =MOj>
  class Pla;
  
  template<>
  class Pla<MOj,MOj>
    : protected shared_ptr<MOj>
    {
    protected:
      typedef shared_ptr<MOj> Base;
      typedef void (*Deleter)(MOj*);
      
      Pla (MOj & subject, Deleter killer) 
        : Base (&subject, killer) {};
        
      friend class MOjFac;
      
    public:
      virtual MOj * 
      operator-> ()  const 
        { 
          ENSURE (*this); 
          return Base::operator-> (); 
        }      
      
      size_t use_count()  const { return Base::use_count(); }
      
      virtual ~Pla() {};
      
    };
  
  template<class TY, class B>
  class Pla
    : public Pla<B>
    {
    protected:
      typedef Pla<B> _Par;
      typedef typename _Par::Base Base;
      typedef typename _Par::Deleter Deleter;
      
      Pla (TY & mo, Deleter killer)
        : _Par (mo, killer)
        { };
        
      friend class MOjFac;
      
    public:
      virtual TY*
      operator-> ()  const
        {
          ENSURE (INSTANCEOF (TY, this->get()));
          return static_cast<TY*>
            (Base::operator-> ());
        }
    };
  
  
  class MOjFac;
    
  struct MOj 
    {
      static MOjFac create;
      
      int id_;
      
      MOj() : id_(rand() % 1000) {}
      virtual ~MOj() {}
      
      void show() { cout << "ID=" << id_ << "\n"; }
    };
  struct Sub1 : MOj { };
  struct Sub2 : MOj 
    {
      void specialAPI() { cout << "specialAPI()\n";}
    };
  struct Sub3 : Sub2
    { 
      
    };
  
  
  
  class MOjFac
    {
      static void deleterFunc (MOj* o) { delete o; }
      
    public:
      
      template<class MO>
      Pla<MO>
      special ()
        {
          return Pla<MO> (*new MO, &deleterFunc);
        }
      
      typedef Pla<Sub3,Sub2> Pla3;
      
      Pla3
      specialSub3 ()
        {
          return Pla3 (*new Sub3, &deleterFunc);
        }
    };
  
  MOjFac MOj::create;
  
  
  
  /*******************************************************************************
   * @test Currently (5/09) this is a concept draft separate of any existing types.
   *       The intention is to rework the Placement implementation based on the
   *       outcomes of this experiment. When doing so, this test could later on
   *       serve to document and cover the corresponding Placement properties.
   * @see  mobject::Placement
   * @see  mobject::MObject#create
   */
  class PlacementHierarchy_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          Pla<Sub1> pSub1    = MOj::create.special<Sub1>();
          Pla<Sub2> pSub2    = MOj::create.special<Sub2>();
          Pla<Sub2> pSub3    = MOj::create.specialSub3();
          
          Pla<MOj>  pSubM (pSub3);
          
          /////////////////////////////////TODO
          format fmt ("sizeof( %s ) = %d\n");
          cout << fmt % "Pla<Sub1>"   % sizeof(pSub1);
          cout << fmt % "Pla<Sub2>"   % sizeof(pSub2);
          cout << fmt % "Pla<Sub3>"   % sizeof(pSub3);
          
          pSub1->show();
          pSub2->show();
          pSub3->show();
          pSubM->show();
          pSub3->specialAPI();
          
          cout << "Hurgha!\n";
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementHierarchy_test, "unit session");
  
  
}}} // namespace mobject::session::test
