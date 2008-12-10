/*
  observable-list.hpp  -  Defines the observable std::list class
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
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
/** @file observable-list.hpp
 ** This file contains the definition of the observable list class
 */

#ifndef OBSERVABLE_LIST_HPP
#define OBSERVABLE_LIST_HPP

#include <list>
#include <sigc++/sigc++.h>

namespace lumiera
{

/**
 * An observable_list is an STL list with an inbuilt sigc++ signal that
 * allows observers to be notified when changes are made to the list.
 **/
template<class T, class Allocator = std::allocator<T> >
class observable_list
{
public:
  /* ===== Typedefs ===== */
  typedef typename std::list<T, Allocator>::iterator iterator;
  typedef typename std::list<T, Allocator>::const_iterator
    const_iterator;
  typedef typename std::list<T, Allocator>::iterator reverse_iterator;
  typedef typename std::list<T, Allocator>::const_reverse_iterator
    const_reverse_iterator;
  typedef typename std::list<T, Allocator>::reference reference;
  typedef typename std::list<T, Allocator>::const_reference
    const_reference;
  typedef typename std::list<T, Allocator>::size_type size_type;

public:
  /* ===== Constructors ===== */
  explicit observable_list(const Allocator& allocator = Allocator()) :
    list(allocator) {};
    
  explicit observable_list(size_type n, const T& value = T(),
    const Allocator& allocator = Allocator()) :
    list(n, value, allocator) {};
    
  template<class InputIterator>
    observable_list(InputIterator first, InputIterator last,
      const Allocator& allocator = Allocator()) :
    list(first, last, allocator) {};
    
  observable_list(const std::list<T,Allocator>& x) :
    list(x) {};
    
  observable_list(const observable_list<T,Allocator>& x) :
    list(x.list) {};
    
public:
  /* ===== Iterators ===== */
  iterator begin() { return list.begin(); }
  const_iterator begin() const { return list.begin(); }
  iterator end() { return list.end(); }
  const_iterator end() const { return list.end(); }    
  reverse_iterator rbegin() { return list.rbegin(); }
  const_reverse_iterator rbegin() const { return list.rbegin(); }
  reverse_iterator rend() { return list.rend(); }
  const_reverse_iterator rend() const { return list.rend(); }  
  
public:
  /* ===== Capacity ===== */
  bool empty() const { return list.empty(); };
  size_type size() const { return list.size(); };
  size_type max_size() const { return list.max_size(); };
  
  void resize(size_type sz, T c = T())
    {
      list.resize(sz, c);
      changed.emit();
    }
    
public:
  /* ===== Element Access ===== */
  reference front() { return list.front(); };
  const_reference front() const { return list.front(); };
  reference back() { return list.back(); };
  const_reference back() const { return list.back(); };
  
public:
  /* ===== Modifiers ===== */
  template<class InputIterator>
  void assign(InputIterator first, InputIterator last)
    {
      list.assign(first, last);
      changed.emit();
    }
    
  void assign(size_type n, const T& u)
    {
      list.assign(n, u);
      changed.emit();
    } 
    
  iterator insert(iterator position, const T& x)
    {
      iterator i = list.insert(position, x);
      changed.emit();
      return i;
    } 
  
  void insert(iterator position, size_type n, const T& x)
    {
      list.insert(position, n, x);
      changed.emit();
    }
  
  template <class InputIterator>
  void insert(iterator position,
    InputIterator first, InputIterator last )
    {
      list.insert(position, first, last);
      changed.emit();
    } 
  
  iterator erase(iterator position)
    {
      iterator i = list.erase(position);
      changed.emit();
      return i;
    }
    
  iterator erase ( iterator first, iterator last )
    {
      iterator i = list.erase(first, last);
      changed.emit();
      return i;
    }
    
  void swap(std::list<T, Allocator>& lst)
    {
      list.swap(lst);
      changed.emit();
    }

  void clear()
    {
      list.clear();
      changed.emit();
    }

  void push_front(const T& x)
    {
      list.push_front(x);
      changed.emit();
    }
    
  void pop_front()
    {
      list.pop_front();
      changed.emit();
    }

  void push_back(const T& x)
    {
      list.push_back(x);
      changed.emit();
    }
    
  void pop_back()
    {
      list.pop_back();
      changed.emit();
    }

  void splice(iterator position, std::list<T,Allocator>& x)
    {
      list.splice(position, x);
      changed.emit();
    }
    
  void splice(iterator position, std::list<T,Allocator>& x, iterator i)
    {
      list.splice(position, x, i);
      changed.emit();
    }
    
  void splice(iterator position, std::list<T,Allocator>& x,
    iterator first, iterator last)
    {
      list.splice(position, x, first, last);
      changed.emit();
    }

  void remove(const T& value)
    {
      list.remove(value);
      changed.emit();
    }
  
  template <class Predicate>
  void remove_if(Predicate pred)
    {
      list.remove_if(pred);
      changed.emit();
    }

  void unique()
    {
      list.unique();
    }
    
  template<class BinaryPredicate>
  void unique(BinaryPredicate binary_pred)
    {
      list.unique(binary_pred);
    }
  
  void merge(std::list<T,Allocator>& x)
    {
      list.merge(x);
      changed.emit();
    }
  
  template<class Compare>
  void merge(std::list<T,Allocator>& x, Compare comp)
    {
      list.merge(x, comp);
      changed.emit();
    }
  
  void sort()
    {
      list.sort();
      changed.emit();
    }
  
  template<class Compare>
  void sort(Compare comp)
    {
      list.sort(comp);
      changed.emit();
    }
    
  void reverse()
    {
      list.reverse();
      changed.emit();
    }
    
public:
  /* ===== Signals ===== */
  
  /**
   * Access to the signal which will be emit every time the list is
   * changed in some way.
   * @return Returns the signal object which will emit notifications.
   **/
  sigc::signal<void>& signal_changed()
    {
      return changed;
    }

private:
  sigc::signal<void> changed;
  std::list<T, Allocator> list;
};

} // namespace lumiera

#endif // OBSERVABLE_LIST_HPP
