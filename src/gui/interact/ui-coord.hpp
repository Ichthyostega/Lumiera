/*
  UI-COORD.hpp  -  generic topological location addressing scheme within the UI

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ui-coord.hpp
 ** A topological addressing scheme to designate structural locations within the UI.
 ** Contrary to screen pixel coordinates, we aim at a topological description of the UI structure.
 ** This foundation allows us
 ** - to refer to some "place" or "space" within the interface
 ** - to remember and return to such a location
 ** - to move a work focus structurally within the UI
 ** - to describe and configure the pattern view access and arrangement
 ** 
 ** As starting point, we'll pick the notion of an access path within a hierarchical structure
 ** - the top-level window
 ** - the perspective used within that window
 ** - the panel within this window
 ** - a view group within the panel
 ** - plus a locally defined access path further down to the actual UI element
 ** 
 ** @todo WIP 9/2017 first draft ////////////////////////////////////////////////////////////////////////////TICKET #1106 generic UI coordinate system
 ** 
 ** @note UICoord is designed with immutability in mind; possibly we may decide to disallow assignment.
 ** 
 ** @see UICoord_test
 ** @see id-scheme.hpp
 ** @see view-spec-dsl.hpp
 ** @see view-locator.hpp
 */


#ifndef GUI_INTERACT_UI_COORD_H
#define GUI_INTERACT_UI_COORD_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/path-array.hpp"
#include "lib/util.hpp"

#include <cstring>
#include <string>
#include <vector>
#include <utility>


namespace gui {
namespace interact {
  
  namespace error = lumiera::error;
  
  using std::string;
  using lib::Literal;
  using lib::Symbol;
  using util::unConst;
  using util::isnil;
  using util::min;
  
  enum {
    UIC_INLINE_SIZE = 8
  };
  
  /* === predefined DSL symbols === */
  
  enum UIPathElm
    {
      UIC_WINDOW,
      UIC_PERSP,
      UIC_PANEL,
      UIC_VIEW,
      UIC_TAB,
      UIC_PATH
    };
  
  
  extern Symbol UIC_CURRENT_WINDOW; ///< window spec to refer to the _current window_ @see view-locator.cpp
  extern Symbol UIC_FIRST_WINDOW;   ///< window spec to refer to the _first window_ of the application
  extern Symbol UIC_ELIDED;         ///< indicate that a component is elided or irrelevant here
  
  
  
  /**
   * Describe a location within the UI through structural/topological coordinates.
   * A UICoord specification is a sequence of Literal tokens, elaborating a path descending
   * through the hierarchy of UI elements down to the specific UI element to refer.
   * @see UICoord_test
   */
  class UICoord
    : public lib::PathArray<UIC_INLINE_SIZE>
    {
      
    public:
      /**
       * UI-Coordinates can be created explicitly by specifying a sequence of Literal tokens,
       * which will be used to initialise and then normalise the underlying PathArray.
       * @warning Literal means _"literal"_ with guaranteed storage during the whole execution.
       * @remarks - in case you need to construct some part, then use \ref Symbol to _intern_
       *            the resulting string into the global static SymbolTable.
       *          - usually the Builder API leads to more readable definitions,
       *            explicitly indicating the meaning of the coordinate's parts.
       */
      template<typename...ARGS>
      explicit
      UICoord (ARGS&& ...args) : PathArray(std::forward<ARGS> (args)...) { }
      
      UICoord (UICoord&&)                 = default;
      UICoord (UICoord const&)            = default;
      UICoord (UICoord& o)     : UICoord((UICoord const&)o) { }
      
      UICoord& operator= (UICoord const&) = default;
      UICoord& operator= (UICoord &&)     = default;
      
      
      
      /* === Builder API === */
      
      class Builder;
      
      /** shortcut to allow init from builder expression */
      UICoord (Builder&& builder);
      
      /** Builder: start definition of UI-Coordinates rooted in the `firstWindow` */
      static Builder firstWindow();

      /** Builder: start definition of UI-Coordinates rooted in the `currentWindow` */
      static Builder currentWindow();
      
      /** Builder: start definition of UI-Coordinates rooted in given window */
      static Builder window (Literal windowID);
      
      //----- convenience shortcuts to start a copy-builder....
      Builder persp (Literal perspectiveID)  const;
      Builder panel (Literal panelID)const;
      Builder view  (Literal viewID) const;
      Builder tab   (Literal tabID)  const;
      Builder tab   (uint tabIdx)    const;
      Builder noTab ()               const;
      
      //----- convenience shortcuts to start mutation on a copy...
      Builder path (Literal pathDefinition) const;
      Builder append  (Literal elmID) const;
      Builder prepend (Literal elmID) const;
      
      
      
      /* === named component access === */
      
      Literal getWindow() const { return accesComponent (UIC_WINDOW);}
      Literal getPersp()  const { return accesComponent (UIC_PERSP); }
      Literal getPanel()  const { return accesComponent (UIC_PANEL); }
      Literal getView()   const { return accesComponent (UIC_VIEW);  }
      Literal getTab()    const { return accesComponent (UIC_TAB);   }
      
      
      
      /* === query functions === */
      
      /**
       * @remark _incomplete_ UI-Coordinates have some fragment of the path
       *         defined, but lacks the definition of an anchor point,
       *         i.e. it has no window ID
       */
      bool
      isIncomplete()  const
        {
          return not empty()
             and isnil (getWindow());
        }
      
      bool
      isComplete()  const
        {
          return not empty()
             and not isnil (getWindow());
        }
      
      
      /**
       * @remark an _explicit_ coordinate spec does not use wildcards
       *         and is anchored in a window spec
       */
      bool
      isExplicit()  const
        {
          return isComplete()
             and not util::contains (*this, Symbol::ANY);
        }
      
      
      bool
      isPresent (size_t idx)  const
        {
          Literal* elm = unConst(this)->getPosition(idx);
          return not isnil(elm)
             and *elm != Symbol::ANY;
        }
      
      
      bool
      isWildcard (size_t idx)  const
        {
          Literal* elm = unConst(this)->getPosition(idx);
          return elm
             and *elm == Symbol::ANY;
        }
      
      
      /**
       * Check if this coordinate spec can be seen as an extension
       * of the given parent coordinates and thus reaches further down
       * towards specific UI elements in comparison to the parent path
       * This constitutes a _partial order_, since some paths might
       * just be totally unrelated to each other not comparable.
       * @note we tolerate (but not demand) expansion/interpolation
       *       of the given parent, i.e. parent may be incomplete
       *       or contain `'*'` placeholders.
       * @todo 10/2017 have to verify suitability of this definition
       */
      bool
      isExtendedBelow (UICoord const& parent)  const
        {
          size_t subSiz = this->size(),
                 parSiz = parent.size(),
                 idx    = 0;
          
          if (parSiz >= subSiz)
            return false;
          
          while (idx < parSiz
                 and (   (*this)[idx]== parent[idx]
                      or Symbol::ANY == parent[idx]
                      or isnil (parent[idx])))
            ++idx;
          
          ENSURE (idx < subSiz);
          return idx == parSiz;
        } // meaning: this goes further down
      
      
      
      /* === String representation === */
      
      operator string()  const
        {
          if (isnil (*this))
            return "UI:?";
          
          string component = getComp();
          string path = getPath();
          
          if (isnil (component))
            return "UI:?/" + path;
          
          if (isnil (path))
            return "UI:" + component;
          else
            return "UI:" + component + "/" + path;
        }
      
      string
      getComp()  const
        {
          if (empty()) return "";
          
          size_t end = min (size(), UIC_PATH);
          size_t pos = indexOf (*begin());
          
          if (pos >= end)
            return ""; // empty or path information only
          
          string buff;
          buff.reserve(80);
          
          if (0 < pos) // incomplete UI-Coordinates (not anchored)
            buff += "?";
          
          for ( ; pos<end; ++pos )
            switch (pos) {
              case UIC_WINDOW:
                buff += getWindow();
                break;
              case UIC_PERSP:
                buff += "["+getPersp()+"]";
                break;
              case UIC_PANEL:
                buff += "-"+getPanel();
                break;
              case UIC_VIEW:
                buff += "."+getView();
                break;
              case UIC_TAB:
                if (UIC_ELIDED != getTab())
                  buff += "."+getTab();
                break;
              default:
                NOTREACHED ("component index numbering broken");
            }
          return buff;
        }
      
      string
      getPath()  const
        {
          size_t siz = size();
          if (siz <= UIC_PATH)
            return "";   // no path information
          
          string buff; // heuristic pre-allocation
          buff.reserve (10 * (siz - UIC_PATH));
          
          iterator elm = pathSeq();
          if (isnil (*elm))
            {  // irregular case : only a path fragment
              elm = this->begin();
              buff += "?/";
            }
          
          for ( ; elm; ++elm )
            buff += *elm + "/";
          
          // chop off last delimiter
          size_t len = buff.length();
          ASSERT (len >= 1);
          buff.resize(len-1);
          return buff;
        }
      
      /** iterative access to the path sequence section */
      iterator
      pathSeq()  const
        {
          return size()<= UIC_PATH? end()
                                  : iterator{this, unConst(this)->getPosition(UIC_PATH)};
        }
      
      
    private:
      /** @note Builder allowed to manipulate stored data */
      friend class Builder;
      
      size_t
      findStartIdx()  const
        {
          REQUIRE (not empty());
          return indexOf (*begin());
        }
      
      Literal
      accesComponent (UIPathElm idx)  const
        {
          Literal* elm = unConst(this)->getPosition(idx);
          return elm? *elm : Symbol::EMPTY;
        }
      
      void
      setComponent (size_t idx, Literal newContent)
        {
          Literal* storage = expandPosition (idx);
          setContent (storage, newContent);
        }
      
      
      /** replace / overwrite existing content starting at given index.
       * @param idx where to start adding content; storage will be expanded to accommodate
       * @param newContent either a single element, or several elements delimited by `'/'`
       * @note - a path sequence will be split at `'/'` and the components _interned_
       *       - any excess elements will be cleared
       * @warning need to invoke PathArray::normalise() afterwards
       */
      void
      setTailSequence (size_t idx, Literal newContent)
        {
          std::vector<Literal> elms;
          if (not isnil (newContent))
            {
              if (not std::strchr (newContent, '/'))
                {
                  // single element: just place it as-is
                  // and remove any further content behind
                  elms.emplace_back (newContent);
                }
              else
                { // it is actually a sequence of elements,
                  // which need to be split first, and then
                  // interned into the global symbol table
                  string sequence{newContent};
                  size_t pos = 0;
                  size_t last = 0;
                  while (string::npos != (last = sequence.find ('/', pos)))
                    {
                      elms.emplace_back (Symbol{sequence.substr(pos, last - pos)});
                      pos = last + 1;                         // delimiter stripped
                    }
                  sequence = sequence.substr(pos);
                  if (not isnil (sequence))
                    elms.emplace_back (Symbol{sequence});
            }   }
          
          setTailSequence (idx, elms);
        }
      
      /** replace the existing path information with the given elements
       * @note - storage will possibly be expanded to accommodate
       *       - the individual path elements will be _interned_ as Symbol
       *       - any excess elements will be cleared
       *       - the pathElms can be _empty_ in which case just
       *         any content starting from `idx` will be cleared
       * @warning need to invoke PathArray::normalise() afterwards
       */
      void
      setTailSequence (size_t idx, std::vector<Literal>& pathElms)
        {
          size_t cnt = pathElms.size();
          expandPosition (idx + cnt); // preallocate
          for (size_t i=0 ; i < cnt; ++i)
            setContent (expandPosition(idx + i), pathElms[i]);
          size_t end = size();
          for (size_t i = idx+cnt; i<end; ++i)
            setContent (expandPosition(i), nullptr);
        }
      
      
    public: /* ===== relational operators : equality and partial order ===== */
      
      friend bool
      operator== (UICoord const& l, UICoord const& r)
      {
        return static_cast<PathArray const&> (l) == static_cast<PathArray const&> (r);
      }
      
      friend bool
      operator<  (UICoord const& l, UICoord const& r)
      {
        return l.isExtendedBelow (r);
      }
      
      friend bool operator>  (UICoord const& l, UICoord const& r) { return (r < l);             }
      friend bool operator<= (UICoord const& l, UICoord const& r) { return (l < r) or (l == r); }
      friend bool operator>= (UICoord const& l, UICoord const& r) { return (r < l) or (l == r); }
      friend bool operator!= (UICoord const& l, UICoord const& r) { return not (l == r);        }
    };
  
  
  
  
  
  /* === Builder API === */
  
  class UICoord::Builder
    {
    protected:
      UICoord uic_;
      
      template<typename...ARGS>
      explicit
      Builder (ARGS&& ...args)      : uic_{std::forward<ARGS> (args)...} { }
      Builder (UICoord && anonRef)  : uic_{std::move(anonRef)} { }
      Builder (UICoord const& base) : uic_{base} { }
      
      Builder (Builder const&)            = delete;
      Builder& operator= (Builder const&) = delete;
      Builder& operator= (Builder &&)     = delete;
      
      /** builder instances created by UICoord */
      friend class UICoord;

    public:
      /** @remark moving a builder instance is acceptable */
      Builder (Builder &&)  = default;
      
      
      /* == Builder functions == */
      
      /** change UI coordinate spec to define it to be rooted within the given window
       * @note this function allows to _undefine_ the window, thus creating an incomplete spec */
      Builder
      window (Literal windowID)
        {
          uic_.setComponent (UIC_WINDOW, windowID);
          return std::move (*this);
        }
      
      /** augment UI coordinates to mandate a specific perspective to be active within the window */
      Builder
      persp (Literal perspectiveID)
        {
          uic_.setComponent (UIC_PERSP, perspectiveID);
          return std::move (*this);
        }
      
      /** augment UI coordinates to indicate a specific view to be used */
      Builder
      panel (Literal panelID)
        {
          uic_.setComponent (UIC_PANEL, panelID);
          return std::move (*this);
        }

      /** augment UI coordinates to indicate a specific view to be used */
      Builder
      view (Literal viewID)
        {
          uic_.setComponent (UIC_VIEW, viewID);
          return std::move (*this);
        }
      
      /** augment UI coordinates to indicate a specific tab within the view" */
      Builder
      tab (Literal tabID)
        {
          uic_.setComponent (UIC_TAB, tabID);
          return std::move (*this);
        }
      
      /** augment UI coordinates to indicate a tab specified by index number */
      Builder
      tab (uint tabIdx)
        {
          uic_.setComponent (UIC_TAB, Symbol{"#"+util::toString (tabIdx)});
          return std::move (*this);
        }
      
      /** augment UI coordinates to indicate that no tab specification is necessary
       * @remarks typically this happens when a panel just holds a simple view */
      Builder
      noTab()
        {
          uic_.setComponent (UIC_TAB, UIC_ELIDED);
          return std::move (*this);
        }
      
      
      /** augment UI coordinates by appending a further component at the end.
       * @note the element might define a sequence of components separated by `'/'`,
       *       in which case several elements will be appended.
       */
      Builder
      append (Literal elm)
        {
          if (not isnil(elm))
            uic_.setTailSequence (uic_.size(), elm);
          return std::move (*this);
        }
      
      /** augment partially defined UI coordinates by extending them towards the root */
      Builder
      prepend (Literal elmID)
        {
          if (not uic_.isIncomplete())
            throw error::Logic ("Attempt to prepend "+elmID
                               +" to the complete rooted path "+string(uic_));
          
          uic_.setComponent (uic_.findStartIdx() - 1, elmID);
          return std::move (*this);
        }
      
      /**
       * augment UI coordinates to define a complete local path
       * @param pathDef a path, possibly with multiple components separated by `'/'`
       * @note any existing path definition is completely replaced by the new path
       */
      Builder
      path (Literal pathDef)
        {
          uic_.setTailSequence (UIC_PATH, pathDef);
          return std::move (*this);
        }

      /** possibly shorten this path specification to a limited depth */
      Builder
      truncateTo (size_t depth)
        {
          uic_.truncateTo (depth);
          return std::move (*this);
        }
    };
  
  
  
  /**
   * @note this ctor is used to "fix" and normalise
   *  the contents established in the Builder thus far.
   */
  inline
  UICoord::UICoord (Builder&& builder)
    : UICoord{std::move (builder.uic_)}
    {
      PathArray::normalise();
    }
  
  
  /** @return a minimally defined Builder, allowing to define further parts;
   *   to finish the definition, cast / store it into UICoord, which itself is immutable.
   */
  inline UICoord::Builder
  UICoord::currentWindow()
  {
    return window (UIC_CURRENT_WINDOW);
  }
  
  inline UICoord::Builder
  UICoord::firstWindow()
  {
    return window (UIC_FIRST_WINDOW);
  }

  /** @return aBuilder with just the windowID defined */
  inline UICoord::Builder
  UICoord::window (Literal windowID)
  {
    return Builder{windowID};
  }
  
  
  /** @return a Builder holding a clone copy of the original UICoord,
   *          with the perspective information set to a new value.
   * @remarks This Builder can then be used do set further parts
   *          independently of the original. When done, store it
   *          as new UICoord object. To achieve real mutation,
   *          assign it to the original variable. */
  inline UICoord::Builder
  UICoord::persp (Literal perspectiveID)  const
  {
    return Builder(*this).persp (perspectiveID);
  }
  
  inline UICoord::Builder
  UICoord::panel (Literal panelID)  const
  {
    return Builder(*this).panel (panelID);
  }

  inline UICoord::Builder
  UICoord::view (Literal viewID)  const
  {
    return Builder(*this).view (viewID);
  }
  
  inline UICoord::Builder
  UICoord::tab (Literal tabID)  const
  {
    return Builder(*this).tab (tabID);
  }
  
  inline UICoord::Builder
  UICoord::tab (uint tabIdx)  const
  {
    return Builder(*this).tab (tabIdx);
  }
  
  inline UICoord::Builder
  UICoord::noTab ()  const
  {
    return Builder(*this).noTab();
  }
  
  /**
   * convenience builder function so set a full path definition
   * @note the given path string will be split at `'/'` and the
   *       resulting components will be stored/retrieved as Symbol
   */
  inline UICoord::Builder
  UICoord::path (Literal pathDefinition)  const
  {
    return Builder(*this).path (pathDefinition);
  }
  
  inline UICoord::Builder
  UICoord::append (Literal elmID)  const
  {
    return Builder(*this).append (elmID);
  }
  
  inline UICoord::Builder
  UICoord::prepend (Literal elmID)  const
  {
    return Builder(*this).prepend (elmID);
  }
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_COORD_H*/
