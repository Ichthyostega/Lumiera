/*
  VARIANT.hpp  -  simple variant wrapper (typesafe union)

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

*/


/** @file variant-o.hpp
 ** This file defines a simple alternative to boost::variant.
 ** It pulls in fewer headers and has a shorter code path, but also
 ** doesn't deal with alignment issues and is <b>not threadsafe</b>. 
 ** 
 ** Values can be stored using \c operator= . In order to access the value
 ** stored in lib::VariantO, you additionally need to define an access "functor" 
 ** - with a typedef "Ret" for the return type
 ** - providing a `static Ret access(ELM&)` function
 **   for each of the types used in the VariantO
 ** "The" typical example for such an access "functor" is the util::AccessCasted template.
 ** 
 ** @deprecated immature first try. Preferably use lib::Variant
 ** @todo the instance handling for the accessor seems somewhat
 **       clumsy: we're creating a static accessor instance for each type
 **       of access functor, and each of these accessors holds a trampoline table.
 **       Why are we building a table, while we're using a templated access functor
 **       anyway? This table inhibits inlining, and there seems to be no
 **       runtime benefit. This whole design indeed was a hastily made
 **       (prompted by actual need) first attempt. It should be rewritten
 **       from scratch (HIV 4/2015)
 ** @todo                         ///////////////////////////////////////TICKET #738 should be rewritten from scratch
 ** @todo                         ///////////////////////////////////////TICKET #141 write an unit test for variant
 ** @see wrapperptr.hpp usage example
 */


#ifndef LIB_VARIANT_O_H
#define LIB_VARIANT_O_H


#include "lib/meta/typelist-util.hpp"
#include "lib/meta/generator.hpp"
#include "lib/nocopy.hpp"




namespace lib {
  
  namespace variant {
    
    using lib::meta::count;
    using lib::meta::maxSize;
    using lib::meta::InstantiateWithIndex;
    
    /**
     * internal helper used to build a variant storage wrapper.
     * Parametrised with a collection of types, it provides functionality
     * to copy a value of one of these types into an internal buffer, while
     * remembering which of these types was used to place this copy.
     * The value can be later on extracted using a visitation like mechanism,
     * which takes a functor class and invokes a function \c access(T&) with
     * the type matching the current value in storage
     */
    template<typename TYPES>
    struct Holder
      {
        
        enum { TYPECNT = count<TYPES>::value
             , SIZE  = maxSize<TYPES>::value
             };
        
        
        /** Storage to hold the actual value */
        struct Buffer
          {
            char buffer_[SIZE];
            uint which_;
            
            Buffer() : which_(TYPECNT) {}
            
            void* 
            put (void)
              {
                deleteCurrent();
                return 0;
              }
            
            void
            deleteCurrent ();  // depends on the Deleter, see below
          };
        
        template<typename T, class BASE, uint idx>
        struct PlacementAdapter : BASE
          {
            T& 
            put (T const& toStore)
              {
                BASE::deleteCurrent(); // remove old content, if any
                                      //
                T& storedObj = *new(BASE::buffer_) T (toStore);
                BASE::which_ = idx; //    remember the actual type selected
                return storedObj;
              }
            
            using BASE::put;    //        inherited alternate put() for other types T
          };
        
        typedef InstantiateWithIndex< TYPES
                                    , PlacementAdapter
                                    , Buffer
                                    > 
                                    Storage;
        
        
        
        /** provide a dispatcher table based visitation mechanism */
        template<class FUNCTOR>
        struct CaseSelect
          {
            typedef typename FUNCTOR::Ret Ret;
            typedef Ret (Func)(Buffer&);
            
            Func* table_[TYPECNT];
            
            CaseSelect ()
              {
                for (uint i=0; i<TYPECNT; ++i)
                  table_[i] = 0;
              }
            
            template<typename T>
            static Ret
            trampoline (Buffer& storage)
              {
                T& content = reinterpret_cast<T&> (storage.buffer_);
                return FUNCTOR::access (content);
              }
            
            Ret
            invoke (Buffer& storage)
              {
                if (TYPECNT <= storage.which_)
                  return FUNCTOR::ifEmpty ();
                else
                  {
                    Func& access = *table_[storage.which_];
                    return access (storage);
                  }
              }
          };
        
        
        /** initialise the dispatcher (trampoline)
         *  for the case of accessing type T */
        template< class T, class BASE, uint i >
        struct CasePrepare
          : BASE
          {
            CasePrepare () : BASE()
              {
                BASE::table_[i] = &BASE::template trampoline<T>;
              }
          };
        
        
        /** access the variant's inline buffer,
         *  using the configured access functor.
         * @note the actual accessor instance
         *       is created on demand (static)
         * @todo shouldn't this rather be located within
         *       the Holder::Storage created by clients??
         */
        template<class FUNCTOR>
        static typename FUNCTOR::Ret
        access (Buffer& buf)
        {
          typedef InstantiateWithIndex< TYPES
                                      , CasePrepare
                                      , CaseSelect<FUNCTOR>
                                      > 
                                      Accessor;
          static Accessor select_case;
          return select_case.invoke(buf);
        }
        
        
        struct Deleter
          {
            typedef void Ret;
            
            template<typename T>
            static void access (T& elem) { elem.~T(); }
            
            static void ifEmpty () { }
          };
      };
      
      
      template<typename TYPES>
      inline void
      Holder<TYPES>::Buffer::deleteCurrent ()
      {
        access<Deleter>(*this); // remove old content, if any
        which_ = TYPECNT;      //  mark as empty
      }
      
  } // namespace variant
  
  
  
  
  
  
  
  
  /**
   * A variant wrapper (typesafe union) capable of holding a value of any
   * of a bounded collection of types. The value is stored in a local buffer
   * directly within the object and may be accessed by a typesafe visitation.
   * 
   * \par
   * This utility class is similar to boost::variant and indeed was implemented
   * (5/08) in an effort to replace the latter in a draft solution for the problem
   * of typesafe access to the correct wrapper class from within some builder tool.
   * Well -- after finishing this "exercise" I must admit that it is not really
   * much more simple than what boost::variant does internally. At least we are
   * pulling in fewer headers and the actual code path is shorter compared with
   * boost::variant, at the price of being not so generic, no care for
   * alignment issues within the buffer or for thread safety.
   * 
   * @deprecated immature first design attempt. obsolete. Please use lib::Variant     ////////////////TICKET #738
   * @param TYPES   collection of possible types to be stored in this variant object
   * @param Access  policy how to access the stored value
   */
  template< typename TYPES
          , template<typename> class Access  
          >
  class VariantO
    : util::NonCopyable
    {
      
      typedef variant::Holder<TYPES> Holder;
      typedef typename Holder::Deleter Deleter;
      
      
      /** storage: buffer holding either an "empty" marker,
       *  or an instance of one of the configured payload types */ 
      typename Holder::Storage holder_;
      
      
    public:
      void reset () { holder_.deleteCurrent();}
      
      /** store a copy of the given argument within the
       *  variant holder buffer, thereby typically casting 
       *  or converting the given source type to the best 
       *  suited (base) type (out of the collection of possible
       *  types for this VariantO instance)
       */ 
      template<typename SRC>
      VariantO&
      operator= (SRC src)
        {
          if (src) holder_.put (src);  // see Holder::PlacementAdaptor::put
          else     reset();
          return *this;
        }
      
      /** retrieve current content of the variant,
       *  trying to cast or convert it to the given type.
       *  Actually, the function \c access(T&) on the 
       *  Access-policy (template param) is invoked with the
       *  type currently stored in the holder buffer.
       *  May return NULL if conversion fails.
       */
      template<typename TAR>
      TAR
      get ()
        {
          typedef Access<TAR> Extractor;
          return Holder::template access<Extractor> (this->holder_);
        }
    };
  
} // namespace lib 
#endif
