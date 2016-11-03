/*
  SIZE-TRAIT.hpp  -  helpers and definitions to deal with the size of some known types

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file §§§
 ** TODO §§§
 */


#ifndef LIB_META_SIZE_TRAIT_H
#define LIB_META_SIZE_TRAIT_H



#include <vector>
#include <string>


namespace lib {
namespace meta {
  
  
  /** 
   * A collection of constants to describe the expected size
   * of some known classes, without needing to include the
   * respective headers. This is an optimisation to improve
   * compilation times and/or reduce size of the generated
   * object files in debug mode. To get those sizes computed
   * in a fairly portable way, but without much overhead,
   * we mimic the memory layout of "the real thing"
   * with some reasonable simplifications:
   * - the size of vectors doesn't really depend on the elements
   * - our strings, streams and buffers use just simple chars
   * 
   * \par Interface
   * The purpose of this whole construction is to pull off
   * some constants based on sizeof expressions:
   * - the size of a string
   * - the size of a vector
   * - the size of a boost::format
   * 
   * @warning this setup is quite fragile and directly relies
   *          on the implementation layout of the GNU STL and Boost.
   *          Whenever using this stuff, make sure to place an assertion
   *          somewhere down in the implementation level to check against
   *          the size of the real thing.
   */
  class SizeTrait
    { 
      //-------------------------------------mimicked-definitions--
      
      typedef std::vector<size_t> Vector;
      typedef std::vector<bool>   BVector;
      
      struct CompatAllocator
        : std::allocator<char>
        { };
      
      struct Locale
        {
          void* _M_impl;
        };
      
      template<class T>
      struct Optional
        {
          bool m_initialized_;
          T    m_storage_;
        };
      
      enum IOS_Openmode 
        { 
          _S_app        = 1L << 0,
          _S_ate        = 1L << 1,
          _S_bin        = 1L << 2,
          _S_in         = 1L << 3,
          _S_out        = 1L << 4,
          _S_trunc      = 1L << 5,
          _S_ios_openmode_end = 1L << 16 
        };      
      
      struct BasicStringbuf
        {
          char * _M_in_beg;
          char * _M_in_cur;
          char * _M_in_end;
          char * _M_out_beg;
          char * _M_out_cur;
          char * _M_out_end;
          
          Locale _M_buf_locale;
          
          virtual ~BasicStringbuf() { }
        };
      
      struct BasicAltstringbuf
        : BasicStringbuf
        {
          char *         putend_;
          bool     is_allocated_;
          IOS_Openmode     mode_;
          CompatAllocator alloc_;
        };
      
      struct BoostFormat
        {
          Vector  items_;
          BVector bound_;       // note: differs in size
          int     style_;
          int     cur_arg_;
          int     num_args_;
          mutable bool  dumped_;
          std::string   prefix_;
          unsigned char exceptions;
          BasicAltstringbuf buf_;
          Optional<Locale>  loc_;
        };
      //-------------------------------------mimicked-definitions--
      
      
    public:/* ===== Interface: size constants ===== */ 
      
      enum { ALIGNMENT       = sizeof(size_t)
           
           , STRING          = sizeof(std::string)
           , VECTOR          = sizeof(Vector)
           , BVECTOR         = sizeof(BVector)
           
           , BOOST_FORMAT    = sizeof(BoostFormat)
           };
    };
  
  
}} // namespace lib::meta
#endif
