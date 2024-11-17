/*
  FORMATS.hpp  -  formats for displaying and specifying time

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file formats.hpp
 ** Definition of time code formats
 ** This header is part of the Lumiera time and timecode handling library
 ** and defines the interfaces and types to deal with the common set of
 ** time code formats encountered in video editing. The generic handling
 ** of _quantised time_ can be parametrised to support and comply to these
 ** specific time code formats.
 */


#ifndef LIB_TIME_FORMATS_H
#define LIB_TIME_FORMATS_H

#include "lib/time/timevalue.hpp"
#include "lib/meta/no-instance.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/generator.hpp"
#include "lib/typed-counter.hpp"

#include <memory>
#include <string>
#include <bitset>

namespace lumiera {
namespace error {
  LUMIERA_ERROR_DECLARE (INVALID_TIMECODE); ///< timecode format error, illegal value encountered.
}}


namespace lib {
namespace time {
  
  
  // ====== forward declarations of concrete Timecode types
  
  class FrameNr;
  class SmpteTC;
  class HmsTC;
  class Secs;
  
  
  class Quantiser; // API for grid aligning
  using QuantR = Quantiser const&;
  using PQuant = std::shared_ptr<const Quantiser>;
  
  
  namespace format {
    
    using std::string;
    using lib::meta::NoInstance; // the following types are for metaprogramming only...
    
    
    /**
     * Frame count as timecode format.
     * An integral number used to count frames
     * can be used as a simple from of time code.
     * Indeed the Lumiera vault layer mostly relies
     * on these frame counts. As with any timecode, the
     * underlying framerate/quantisation remains implicit.
     */
    struct Frames
      : NoInstance<Frames>
      {
        static TimeValue    parse (string const&, QuantR);
        static void       rebuild (FrameNr&, QuantR, TimeValue const&);
        static TimeValue evaluate (FrameNr const&, QuantR);
      };
    
    
    /**
     * Widely used standard media timecode format.
     * A SMPTE timestamp addresses individual frames,
     * by specifying time as hour-minute-second plus the
     * frame number within the actual second.
     */
    struct Smpte
      : NoInstance<Smpte>
      {
        static TimeValue    parse (string const&, QuantR);
        static void       rebuild (SmpteTC&, QuantR, TimeValue const&);
        static TimeValue evaluate (SmpteTC const&, QuantR);
        static uint  getFramerate (QuantR, TimeValue const&);
        static void applyRangeLimitStrategy (SmpteTC&);
      };
    
    
    /**
     * The informal hours-minutes-seconds-millisecond timecode.
     * As such, this timecode is quantisation agnostic, but usually
     * it is used to address some frame or block or otherwise quantised
     * entity in time. HMS-Timecode is similar to SMPTE, but uses a
     * floating point milliseconds value instead of the frame count
     */
    struct Hms
      : NoInstance<Hms>
      {
        static TimeValue    parse (string const&, QuantR);
        static void       rebuild (HmsTC&, QuantR, TimeValue const&);
        static TimeValue evaluate (HmsTC const&, QuantR);
      };
    
    
    /**
     * Simple timecode specification as fractional seconds.
     * Similar to HMS, a specification of seconds is quantisation agnostic,
     * but usually some implicit quantisation is used anyway, be it on actual
     * data frames, audio frames, or just on some smaller time interval, e.g.
     * full milliseconds.
     * @note Seconds is implemented as rational number and thus uses
     *       decimal format, not the usual sexagesimal time format
     */
    struct Seconds
      : NoInstance<Seconds>
      {
        static TimeValue    parse (string const&, QuantR);
        static void       rebuild (Secs&, QuantR, TimeValue const&);
        static TimeValue evaluate (Secs const&, QuantR);
      };
    
    
    
    
    
    template<class FMT>
    struct Traits;
    
    template<>
    struct Traits<Frames>
      {
        using TimeCode = FrameNr;
      };
    
    template<>
    struct Traits<Smpte>
      {
        using TimeCode = SmpteTC;
      };
    
    template<>
    struct Traits<Hms>
      {
        using TimeCode = HmsTC;
      };
    
    template<>
    struct Traits<Seconds>
      {
        using TimeCode = Secs;
      };
    
    
    
    /* == Descriptor to define Support for specific formats == */
    
    using lib::meta::Types;
    using lib::meta::Node;
    using lib::meta::NullType;
    
    /**
     * Descriptor to denote support for a specific (timecode) format.
     * This helper can be used to configure a selection of specific
     * timecode formats to be or not to be supported by some facility.
     * Formats are described by the format descriptor types defined in
     * this header (or elsewhere for additional formats). This helper
     * establishes an numeric ID at runtime and uses a bitset to keep
     * track of the support for a given format.
     * 
     * @note preconfigured limit #MAXID on the absolute number of
     * different Format types; maybe needs to be increased accordingly.
     * @warning the actual numeric IDs might vary on each run
     * @see TypedContext
     */
    class Supported
      {
        enum { MAXID = 8 };
        
        std::bitset<MAXID> flags_;
        
        template<class F>
        IxID
        typeID()  const
          {
            return TypedContext<Supported>::ID<F>::get();
          }
        
        template<class F, class FS>
        Supported
        define(Node<F,FS>) ///< @internal set the flag for one Format in the typelist
          {
            flags_.set (typeID<F>());
            return define(FS());
          }
        Supported define(NullType) { return *this;} ///< @internal recursion end
        
        Supported() { } ///< @note use #formats to set up a new descriptor
        
        
      public:
        /** build a new Descriptor to denote support for all the Formats,
         *  @param TY typelist holding all the Format types to support
         */
        template<typename TY>
        static Supported
        formats()
          {
            typedef typename TY::List SupportedFormats;
            return Supported().define(SupportedFormats());
          }
        
        /** check if a specific Format is supported */
        template<class F>
        bool
        check()  const
          {
            return flags_[typeID<F>()];
          }
      };
    
    /**
     * predefined standard configuration:
     * Descriptor for supporting all the classical timecode formats
     */
    struct SupportStandardTimecode
      : Supported
      {
        SupportStandardTimecode()
          : Supported(formats< Types<Hms,Smpte,Frames,Seconds> >())
          { }
      };
    
    
}}} // lib::time::format
#endif
