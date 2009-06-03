/*
 *  slist.h - simple intrusive cyclic single linked list
 *
 *  Copyright (C)         Lumiera.org
 *    2009                Anton Yakovlev <just.yakovlev@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef SLIST_H
#define SLIST_H

#include <stddef.h>

/**
 * @file
 * Intrusive cyclic single linked list.
 *
 * List node is a structure, which consists only of a forward pointer. This is
 * much easier and makes code much cleaner, than to have forward pointer as is.
 * In a empty initialized node, this pointer points to the node itself. Note
 * that this pointer can never ever become NULL.
 *
 * This lists are used by using one node as 'root' node where it's pointer is
 * the head pointer to the actual list. Care needs to be taken to ensure not to
 * apply any operations meant to be applied to data nodes to the root node.
 * This way is the prefered way to use this lists.
 *
 * Alternatively one can store only a chain of data nodes and use a SList
 * pointer to point to the first item (which might be NULL in case no data is
 * stored). When using such approach care must be taken since most functions
 * below expect lists to have a root node.
 *
 * Due to nature of single linked list, there's no easy way to implement
 * functions, which need reverse passing through a list. But some of L1-list
 * interface functions need such ability (for example, when we need to find
 * previous element for current element). Because search of previous element
 * requires visiting of exactly N-1 nodes (where N is length of L1-list), we
 * use root node as start point. This gives to us probability of visiting
 * 1 <= C <= N-1 nodes, and, thus, speed up search.
 *
 * This header can be used in 2 different ways:
 * 1) (prerefered) just including it provides all functions as static inlined
 *    functions. This is the default
 * 2) #define LLIST_INTERFACE before including this header gives only the declarations
 *    #define LLIST_IMPLEMENTATION before including this header yields in definitions
 *    this can be used to generate a library. This is currently untested and not
 *    recommended.
 * The rationale for using inlined functions is that most functions are very
 * small and likely to be used in performance critical parts. Inlining can give
 * a hughe performance and optimization improvement here. The few functions
 * which are slightly larger are expected to be the less common used ones, so
 * inlining them too shouldn't be a problem either.
 */

/* TODO __STDC_VERSION__ 199901L
150) This macro was not specified in ISO/IEC 9899:1990 and was specified as 199409L in
ISO/IEC 9899/AMD1:1995. The intention is that this will remain an integer constant of type long
int that is increased with each revision of this International Standard.
*/

#ifdef HAVE_INLINE
#   define SLIST_MACRO static inline
#else
#   ifdef __GNUC__
#       define SLIST_MACRO static __inline__
#   else
#       define SLIST_MACRO static
#   endif
#endif

#if defined(SLIST_INTERFACE)
/* only the interface is generated */
#define SLIST_FUNC(proto, ...) proto
#elif defined(SLIST_IMPLEMENTATION)
/* generate a non inlined implementation */
#define SLIST_FUNC(proto, ...) proto { __VA_ARGS__ }
#else
/* all functions are macro-like inlined */
#define SLIST_FUNC(proto, ...) SLIST_MACRO proto { __VA_ARGS__ }
#endif

/*
 * Type of a slist node.
 */

#ifndef SLIST_DEFINED
#define SLIST_DEFINED

struct slist_struct {
    struct slist_struct* next;
};

#endif

typedef struct slist_struct slist;
typedef slist* SList;
typedef const slist* const_SList;
typedef slist** SList_ref;

/**
 * Macro to instantiate a local llist.
 *
 * @param name of the slist node
 */

#define SLIST_AUTO( name ) slist name = { &name }

/*
 * some macros for convenience
 */

#define slist_head slist_next
#define slist_insert_head( list, element ) slist_insert( list, element )

/**
 * Сast back from a member of a structure to a pointer of the structure.
 *
 * Example:
 *
 *  struct point {
 *      int x;
 *      int y;
 *      slist list;
 *  };
 *
 *  SList points = ...; // some initialization; must be the root of our list
 *
 *  SLIST_FOREACH( points, current_node ) {
 *      struct point* current_point = SLIST_TO_STRUCTP( current_node, struct point, list );
 *      printf( "point = ( %d, %d )\n", current_point -> x, current_point -> y );
 *  }
 *
 * @param list is a pointer to the SList member of the linked structures
 * @param type is type name of the linked structures
 * @param member is a name of the SList member of the linked structures
 */

#define SLIST_TO_STRUCTP( list, type, member ) \
    ( ( type* ) ( ( ( char* )( list ) ) - offsetof( type, member ) ) )

/**
 * Iterate forward over a list.
 *
 * @param list the root node of the list to be iterated
 * @param node pointer to the iterated node
 */

#define SLIST_FOREACH( list, node ) \
    for ( SList node = slist_head( list ); ! slist_is_end( node, list ); slist_forward( &node ) )

/**
 * Iterate forward over a range.
 *
 * @param start first node to be interated
 * @param end node after the last node be iterated
 * @param node pointer to the iterated node
 */

#define SLIST_FORRANGE( start, end, node ) \
    for ( SList node = start; node != end; slist_forward( &node ) )

/**
 * Consume a list from head.
 * The body of this statement should remove the head from the list, else it would be a infinite loop
 *
 * @param list the root node of the list to be consumed
 * @param head pointer to the head node
 */

#define SLIST_WHILE_HEAD( list, head ) \
    for ( SList head = slist_head( list ); ! slist_is_empty( list ); head = slist_head( list ) )

/**
 * Initialize a new llist.
 * Must not be applied to a list node which is not empty! Lists need to be initialized
 * before any other operation on them is called.
 *
 * @param list node to be initialized
 */

SLIST_FUNC (
    void slist_init( SList list ),
    list -> next = list;
);

/**
 * Check if a node is not linked with some other node.
 */

SLIST_FUNC (
    int slist_is_empty( const_SList list ),
    return list -> next == list;
);

/**
 * Check if self is the only node in a list or self is not in a list.
 *
 * Warning:
 *      There's no check for empty list, so if you have a list with no items,
 *      you'll get seg fault here.
 *
 * @param list is root node of the list to be checked
 */

SLIST_FUNC (
    int slist_is_single( const_SList list ),
    return list -> next -> next == list;
);

/**
 * Check for the head of a list.
 *
 * @param list is root node of the list
 * @param head is expected head of the list
 */

SLIST_FUNC (
    int slist_is_head( const_SList list, const_SList head ),
    return list -> next == head;
);

/**
 * Check for the end of a list.
 * The end is by definition one past the tail of a list, which is the root node itself.
 *
 * @param list is root node of the list
 * @param end is expected end of the list
 */

SLIST_FUNC (
    int slist_is_end( const_SList list, const_SList end ),
    return list == end;
);

/**
 * Check if a node is a member of a list.
 *
 * @param list is root node of the list
 * @param member is node to be searched
 */

SLIST_FUNC (
    int slist_is_member( const_SList list, const_SList member ),
    for ( const_SList i = member -> next; i != member; i = i -> next ) {
        if ( i == list ) {
            return 1;
        }
    }
    return 0;
);

/**
 * Check the order of elements in a list.
 *
 * @param list is root node of the list
 * @param before is expected to be before after
 * @param after is expected to be after before
 */

SLIST_FUNC (
    int slist_is_before_after( const_SList list, const_SList before, const_SList after ),
    for ( const_SList i = before -> next; i != list; i = i -> next ) {
        if ( i == after ) {
            return 1;
        }
    }
    return 0;
);

/**
 * Count the nodes of a list.
 *
 * @param list is root node of the list
 * @return number of nodes in `list`
 */

SLIST_FUNC (
    unsigned slist_count( const_SList list ),
    unsigned cnt = 0;
    for ( const_SList i = list; i -> next != list; ++cnt, i = i -> next ) {
        ;
    }
    return cnt;
);

/**
 * Get next node.
 * Will not stop at tail.
 *
 * @param node is current node
 * @return node after current node
 */

SLIST_FUNC (
    SList slist_next( const_SList node ),
    return node -> next;
);

/**
 * Get previous node.
 *
 * @param list is root node of the list
 * @param node is current node
 * @return node before current node
 */

SLIST_FUNC (
    SList slist_prev( SList list, SList node ),
    while ( list -> next != node ) {
        list = list -> next;
    }
    return list;
);

/**
 * Remove a node from a list.
 *
 * @param list is root node of the list
 * @param node to be removed
 * @return node
 */

SLIST_FUNC (
    SList slist_unlink( SList list, SList node ),
    SList prev_node = slist_prev( list, node );
    prev_node -> next = node -> next;
    return node -> next = node;
);

/**
 * Insert a node after another.
 *
 * @param head is node after which we want to insert
 * @param node is node which shall be inserted after `head`. Could already linked to a list from where it will be removed.
 * @return head
 */

SLIST_FUNC (
    SList slist_insert( SList head, SList node ),
    if ( ! slist_is_empty( node ) ) {
        slist_unlink( node, node );
    }
    node -> next = head -> next;
    head -> next = node;
    return head;
);

/**
 * Move the content of a list after a node in another list.
 *
 * @param xnode is node after which we want to insert a list
 * @param ylist is root node of the list which shall be inserted after self. This list will be empty after call.
 * @return xnode
 */

SLIST_FUNC (
    SList slist_insert_list( SList xnode, SList ylist ),
    if ( ! slist_is_empty( ylist ) ) {
        SList tail = slist_prev( ylist, ylist ); // search for the Y list tail
        tail -> next = xnode -> next;
        xnode -> next = ylist -> next;

        ylist -> next = ylist; // clear the Y list
    }
    return xnode;
);

/**
 * Move a range of nodes after a given node.
 *
 * @param node is node after which the range shall be inserted
 * @param start first node in range to be moved
 * @param end node after the last node of the range
 * @return node
 */

SLIST_FUNC (
    SList slist_insert_range( SList node, SList start, SList end ),
    // insert range
    SList tail = slist_prev( start, end ); // search for the end of range
    tail -> next = node -> next;
    node -> next = start -> next;
    // fix list
    start -> next = end;
    return node;
);

/**
 * Swap a node with its next node.
 * Advancing will not stop at tail, one has to check that if this is intended.
 *
 * @param list is root node of the list
 * @param node is node to be advaced
 * @return node
 */

SLIST_FUNC (
    SList slist_advance( SList list, SList node ),
    SList prev = slist_prev( list, node );
    prev -> next = node -> next;
    node -> next = node -> next -> next;
    prev -> next -> next = node;
    return node;
);

/**
 * Advance a pointer to a node to its next node.
 *
 * @param node pointer-to-pointer to the current node. `node` will point to the next node after this call.
 */

SLIST_FUNC (
    void slist_forward( SList_ref node ),
    *node = ( *node ) -> next;
);

/**
 * Get the nth element of a list (this function does not stop at head/tail).
 *
 * @param list is root node of the list to be queried
 * @param n is number of element to find
 * @return |n|-th element of list
 */

SLIST_FUNC (
    SList slist_get_nth( SList list, unsigned int n ),
    while ( n-- > 0 ) {
        list = slist_next( list );
    }
    return list;
);

/**
 * Get the nth element of a list with a stop node.
 *
 * @param list is root node of the list to be queried
 * @param n is number of element to find
 * @param stop is node which will abort the iteration
 * @return |n|-th element of list or NULL if `stop` node has been reached
 */

SLIST_FUNC (
    SList slist_get_nth_stop( SList list, unsigned int n, const_SList stop ),
    while ( n-- > 0 ) {
        list = slist_next( list );
        if ( list == stop ) {
            return NULL;
        }
    }
    return list;
);

/**
 * Sort a list.
 *
 * This is iterative version of bottom-up merge sort for (L1/L2) linked-list:
 *  + there's no recursion
 *  + there's no extra stackspace allocation (only a few bytes for locals)
 * Such implementation should be optimal and fast enough.
 *
 * Maybe this function is too big for inlining (though I don't think so), so
 * maybe somebody can make it smaller without losing perfomance? ;)
 *
 * @param list is root node of a list to be sorted
 * @param cmp is compare function of 2 SList items
 * @return list
 */

typedef int ( *slist_cmpfn )( const_SList a, const_SList b );

SLIST_FUNC (
    SList slist_sort( SList list, slist_cmpfn cmp ),
    if ( ! slist_is_single( list ) ) {

        unsigned int length = slist_count( list );

        // `max_size` is a half of minimum power of 2, greater of equal to `length`
        // ( 2 * max_size = 2^k ) >= length
        // We need `max_size` value for proper binary division of a list for sorting.
        
        unsigned long long max_size = 1;
        while ( ( max_size << 1 ) < length ) {
            max_size <<= 1;
        }

        // The main idea of bottom-up merge sort is sequential merging of each pair
        // of sequences of { 1, .. 2^k, .. max_size } length. That's all. :)

        for ( unsigned int size = 1; size <= max_size; size <<= 1 ) {

            // On each iteration:
            //  * `result` points to the current node of global (merged/sorted) list.
            //    thus, we can holds all nodes are linked.
            //  * `left` and `right` points to begin of (sub)lists for merging.
            
            SList result = list;
            SList left = list -> next;
            SList right;

            // Process each pairs of sequences of size=2^k length.
            
            for ( unsigned int position = 0; position < length; position += size + size ) {

                right = slist_get_nth_stop( left, size, list );

                unsigned int size_left = size;
                unsigned int size_right = right == NULL ? 0 : size;

                // Here we have 2 sublists of `size_left` and `size_right` sizes.
                // Implementation of `merge` function is next three loops.

                while ( ( size_left > 0 ) && ( size_right > 0 ) ) {

                    if ( cmp( left, right ) <= 0 ) {
                        
                        result -> next = left;
                        
                        left = left -> next;
                        if ( left == list ) {
                            size_left = 0;
                        } else {
                            size_left--;
                        }

                    } else {
                        
                        result -> next = right;
                        
                        right = right -> next;
                        if ( right == list ) {
                            size_right = 0;
                        } else {
                            size_right--;
                        }
                        
                    }
                    
                    result = result -> next;
                    
                }

                while ( size_left > 0 ) {
                    result -> next = left;
                    result = left;
                    
                    left = left -> next;
                    if ( left == list ) {
                        break;
                    }
                    size_left--;
                }

                while ( size_right > 0 ) {
                    result -> next = right;
                    result = right;
                    
                    right = right -> next;
                    if ( right == list ) {
                        break;
                    }
                    size_right--;
                }

                // go to begin of next pair of sequences
                
                left = right;
                
            }

            // here `result` points to the last node of a list.
            // we wanna keep cyclic list.
            
            result -> next = list;
            
        }

    }
    return list;
)
            
/**
 * Find the first occurence of an element in a list.
 * Does not change the order of a list.
 *
 * @param list is root node of a list to be searched
 * @param pattern is template for the element being searched
 * @param cmp is compare function of 2 SList items
 * @return pointer to the found SList element or NULL if nothing found
 */

SLIST_FUNC (
    SList slist_find( const_SList list, const_SList pattern, slist_cmpfn cmp ),
    SLIST_FOREACH( list, node ) {
        if ( cmp( node, pattern ) == 0 ) {
            return node;
        }
    }
    return NULL;
)

/**
 * Find the first occurence of an element in an unsorted list.
 *
 * Searches the list until it finds the searched element and moves it then to
 * the head. Useful if the order of the list is not required and few elements
 * are frequently searched.
 *
 * @param list is root node of a list to be searched
 * @param pattern is template for the element being searched
 * @param cmp is compare function of 2 SList items
 * @return pointer to the found SList element (head) or NULL if nothing found
 */

SLIST_FUNC (
    SList slist_ufind( SList list, const_SList pattern, slist_cmpfn cmp ),
    SLIST_FOREACH( list, node ) {
        if ( cmp( node, pattern ) == 0 ) {
            slist_insert_head( list, node );
            return node;
        }
    }
    return NULL;
)

/**
 * Find the first occurence of an element in a sorted list.
 *
 * Searches the list until it finds the searched element, exits searching when
 * found an element biggier than the searched one.
 *
 * @param list is root node of a list to be searched
 * @param pattern is template for the element being searched
 * @param cmp is compare function of 2 SList items
 * @return pointer to the found SList element (head) or NULL if nothing found
 */

SLIST_FUNC (
    SList slist_sfind( const_SList list, const_SList pattern, slist_cmpfn cmp ),
    SLIST_FOREACH( list, node ) {
        
        int result = cmp( node, pattern );
        
        if ( result == 0 ) {
            return node;
        } else if ( result > 0 ) {
            break;
        }
        
    }
    return NULL;
)

#endif /* SLIST_H */
