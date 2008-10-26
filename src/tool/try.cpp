/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */ 

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>
// 4/08  - conversions on the value_type used for boost::any
// 5/08  - how to guard a downcasting access, so it is compiled in only if the involved types are convertible
// 7/08  - combining partial specialisation and subclasses 
// 10/8  - abusing the STL containers to hold noncopyable values


//#include <nobug.h>
#include "proc/nobugcfg.hpp"

#include <iostream>
#include <typeinfo>
#include <boost/format.hpp>
#include <boost/noncopyable.hpp>
#include <vector>

#include "lib/scopedholder.hpp"

using std::string;
using std::cout;
using std::vector;
using boost::format;


  namespace {
      boost::format fmt ("<%2i>");
      long checksum = 0;
  }
  
  
  namespace funny {
  
    
    class Mrpf 
      : boost::noncopyable
      {
        long secret_;
        
        typedef Mrpf _ThisType;
        
      public:
        Mrpf() 
          : secret_(0) 
          {
            cout << "Mrpf() this=" << this <<"\n";
          }
        ~Mrpf() 
          { 
            checksum -= secret_; 
            cout << "~Mrpf() this=" << this <<"  skeret="<<secret_<<"\n";
          }
        
        Mrpf (const Mrpf& o)
          : secret_(0)
          {
            cout << "Mrpf( ref ) this=" <<this << " o="<<&o<<"\n";
            ASSERT (!o.secret_);
          }
        
        Mrpf&
        operator= (Mrpf const& ref)
          {
            cout << "Mrpf=ref   this=" <<this << " ref="<<&ref<<"\n";
            ASSERT (!(*this));
            ASSERT (!ref);
            return *this;
          }
        
        void
        setup (long x=0)
          {
            secret_ = (x? x : (rand() % 10000));
            checksum += secret_;
            cout << "this="<<this<<" ....setup-->"<<secret_<<"\n";
          }
        
        long getIt() const { return secret_; }
        
        
        typedef long _ThisType::*unspecified_bool_type;
        
        /** implicit conversion to "bool" */
        operator unspecified_bool_type()  const // never throws
          {
            return secret_?  &_ThisType::secret_ : 0;
          }
        
        bool operator! ()  const { return 0==secret_; }
        
        
      protected:
        friend void transfer_control (Mrpf& from, Mrpf& to);
        
      };
      
      
    void
    transfer_control (Mrpf& from, Mrpf& to)
    {
      ASSERT (!to.secret_);
      cout << "transfer... from="<<&from<<" to="<<&to<<"\n";
      to.secret_ = from.secret_;
      from.secret_ = 0;
    }
  }
  
  template<class TY, class PAR = std::allocator<TY> >
  class Allocator_TransferNoncopyable
    {
      typedef Allocator_TransferNoncopyable<TY,PAR> _ThisType;
      
      PAR par_;
      
      
    public:
      typedef typename PAR::size_type       size_type;
      typedef typename PAR::difference_type difference_type; 
      typedef typename PAR::pointer         pointer;
      typedef typename PAR::const_pointer   const_pointer;
      typedef typename PAR::reference       reference;
      typedef typename PAR::const_reference const_reference;
      typedef typename PAR::value_type      value_type;

      template<typename _Tp1>
        struct rebind
        { typedef Allocator_TransferNoncopyable<_Tp1, PAR> other; };

      Allocator_TransferNoncopyable()  { }

      Allocator_TransferNoncopyable(const _ThisType& __a)
        : par_(__a.par_) { }
      Allocator_TransferNoncopyable(const PAR& __a)
        : par_(__a) { }

      template<typename X>
      Allocator_TransferNoncopyable(const std::allocator<X>&) { }

      ~Allocator_TransferNoncopyable() { }

      
      
      size_type     max_size()                      const { return par_.max_size(); }
      pointer       address(reference r)            const { return par_.address(r); }
      const_pointer address(const_reference cr)     const { return par_.address(cr); }
      pointer       allocate(size_type n, const void *p=0){ return par_.allocate(n,p); }
      void          deallocate(pointer p, size_type n)    { return par_.deallocate(p,n); }
      void          destroy(pointer p)                    { return par_.destroy(p); }
      
      
      void 
      construct (pointer p, const TY& ref)
        {
          cout << "Allo::construct( p="<<p<<", ref="<<&ref<<" )\n";
          new(p) TY();
          ASSERT (p);
          ASSERT (!(*p));
          if (ref)
            transfer_control (const_cast<TY&>(ref), *p);
        }
    };
  
  template<typename _T1, typename _T2>
  inline bool
  operator== (Allocator_TransferNoncopyable<_T1> const&, Allocator_TransferNoncopyable<_T2> const&)
  { return true; }

  template<typename _T1, typename _T2>
  inline bool
  operator!= (Allocator_TransferNoncopyable<_T1> const&, Allocator_TransferNoncopyable<_T2> const&)
  { return false; }
  
  
  
  typedef Allocator_TransferNoncopyable<funny::Mrpf> Allo; 
  
int 
main (int argc, char* argv[])
  {
    
    NOBUG_INIT;
    
    vector<funny::Mrpf, Allo> mrmpf;
    
    mrmpf.reserve(2);
    mrmpf.push_back(funny::Mrpf());
    mrmpf[0].setup(33);
    
    cout << ".....resize:\n";
    
    mrmpf.resize(5);
    
    
    cout <<  "Summmmmmmmmmmmmmmmmmmm="<<checksum<<"\n";
    mrmpf.clear();
    cout <<  "Summmmmmmmmmmmmmmmmmm="<<checksum<<"\n";
    
    cout <<  "\ngulp\n";
    
    
    return 0;
  }
