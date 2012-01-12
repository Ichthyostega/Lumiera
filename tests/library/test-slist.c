/*
 *  test-slist.c  -  test the linked list lib
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

#include "lib/slist.h"
#include "lib/test/test.h"

#include <sys/time.h>
#include <nobug.h>

typedef struct item {
    int key;
    slist list;
} item_t;

int cmp( const_SList a, const_SList b ) {

    item_t* x = SLIST_TO_STRUCTP( a, item_t, list );
    item_t* y = SLIST_TO_STRUCTP( b, item_t, list );

    if ( x -> key < y -> key ) {
        return -1;
    }

    if ( x -> key > y -> key ) {
        return +1;
    }

    return 0;
    
}

TESTS_BEGIN

/*
 * 1. Basic:
 *      SLIST_AUTO( name )
 *      void slist_init( SList list )
 *      int slist_is_empty( const_SList list )
 *      int slist_is_single( const_SList list )
 *      int slist_is_head( const_SList list, const_SList head )
 *      int slist_is_end( const_SList list, const_SList end )
 *      int slist_is_member( const_SList list, const_SList member )
 *      int slist_is_before_after( const_SList list, const_SList before, const_SList after )
 */

TEST (basic) {

    SLIST_AUTO( listX );
    slist listY;
    SLIST_AUTO( nodeA );
    SLIST_AUTO( nodeB );

    ECHO ("%d", slist_is_end( &listX, &listX ) );
    
    slist_init( &listY );

    ECHO ("%d", slist_is_empty( &listY ) );
    
    slist_insert( &listX, &nodeA );
    ECHO ("%d", slist_is_empty( &listX ) );
    ECHO ("%d", slist_is_single( &listX ) );
    ECHO ("%d", slist_is_head( &listX, &nodeA ) );
    ECHO ("%d", slist_is_end( &listX, &nodeA ) );
    ECHO ("%d", slist_is_member( &listX, &nodeA ) );
    ECHO ("%d", slist_is_member( &listX, &nodeB ) );

    slist_insert( &nodeA, &nodeB );
    ECHO ("%d", slist_is_empty( &listX ) );
    ECHO ("%d", slist_is_single( &listX ) );
    ECHO ("%d", slist_is_head( &listX, &nodeB ) );
    ECHO ("%d", slist_is_end( &listX, &nodeB ) );
    ECHO ("%d", slist_is_member( &listX, &nodeB ) );

    ECHO ("%d", slist_is_before_after( &listX, &nodeA, &nodeB ) );
    ECHO ("%d", slist_is_before_after( &listX, &nodeB, &nodeA ) );

}

/*
 * 2. Insert/delete:
 *      slist_insert_head( list, element )
 *      SList slist_insert( SList head, SList node )
 *      SList slist_insert_list( SList xnode, SList ylist )
 *      SList slist_insert_range( SList node, SList start, SList end )
 *      SList slist_unlink( SList list, SList node )
 */

TEST (insert_delete) {

    SLIST_AUTO( listX );
    SLIST_AUTO( nodeA );
    SLIST_AUTO( nodeB );
    SLIST_AUTO( nodeC );

    slist_insert_head( &listX, &nodeA );
    slist_insert( &nodeA, &nodeB );
    slist_insert( &nodeB, &nodeC );
    ECHO ("%d", slist_next( &listX ) == &nodeA );
    ECHO ("%d", slist_next( &nodeA ) == &nodeB );
    ECHO ("%d", slist_next( &nodeB ) == &nodeC );
    ECHO ("%d", slist_next( &nodeC ) == &listX );

    slist_unlink( &listX, &nodeA );
    ECHO ("%d", slist_next( &listX ) == &nodeB );

    slist_insert( &listX, &nodeA );
    ECHO ("%d", slist_next( &listX ) == &nodeA );

    SLIST_AUTO( listY );

    slist_insert_list( &listY, &listX );
    ECHO ("%d", slist_is_empty( &listX ) );
    ECHO ("%d", slist_next( &listY ) == &nodeA );
    ECHO ("%d", slist_next( &nodeA ) == &nodeB );
    ECHO ("%d", slist_next( &nodeB ) == &nodeC );
    ECHO ("%d", slist_next( &nodeC ) == &listY );

    slist_insert_range( &listX, &nodeA, &nodeB );
    ECHO ("%d", slist_next( &listX ) == &nodeA );
    ECHO ("%d", slist_next( &nodeA ) == &nodeB );
    ECHO ("%d", slist_next( &nodeB ) == &listX );

    ECHO ("%d", slist_is_single( &listY ) );
    ECHO ("%d", slist_next( &listY ) == &nodeC );
    ECHO ("%d", slist_next( &nodeC ) == &listY );
    
}

/*
 * 3. Movements:
 *      slist_head()
 *      SList slist_next( const_SList node )
 *      SList slist_prev( SList list, SList node )
 *      SList slist_advance( SList list, SList node )
 *      void slist_forward( SList_ref node )
 */

TEST (movement) {

    SLIST_AUTO( listX );
    SLIST_AUTO( nodeA );
    SLIST_AUTO( nodeB );
    SLIST_AUTO( nodeC );

    slist_insert_head( &listX, &nodeA );
    slist_insert( &nodeA, &nodeB );
    slist_insert( &nodeB, &nodeC );

    ECHO ("%d", slist_next( &listX ) == &nodeA );
    ECHO ("%d", slist_next( &nodeA ) == &nodeB );
    ECHO ("%d", slist_next( &nodeB ) == &nodeC );
    ECHO ("%d", slist_next( &nodeC ) == &listX );

    ECHO ("%d", slist_prev( &listX, &listX ) == &nodeC );
    ECHO ("%d", slist_prev( &listX, &nodeC ) == &nodeB );
    ECHO ("%d", slist_prev( &listX, &nodeB ) == &nodeA );
    ECHO ("%d", slist_prev( &listX, &nodeA ) == &listX );
    
    slist_advance( &listX, &nodeA );
    ECHO ("%d", slist_next( &listX ) == &nodeB );
    ECHO ("%d", slist_next( &nodeB ) == &nodeA );
    ECHO ("%d", slist_next( &nodeA ) == &nodeC );
    ECHO ("%d", slist_next( &nodeC ) == &listX );

    SList node = &listX;
    slist_forward( &node );
    ECHO ("%d", node == &nodeB );
    
}

/*
 * 4. Enumerations:
 *      SLIST_TO_STRUCTP( list, type, member )
 *      SLIST_FOREACH( list, node )
 *      SLIST_FORRANGE( start, end, node )
 *      SLIST_WHILE_HEAD( list, head )
 */

TEST (enumerations) {

    SLIST_AUTO( list );
    
    item_t nodeA = { 'A', { NULL } };
    item_t nodeB = { 'B', { NULL } };
    item_t nodeC = { 'C', { NULL } };
    item_t nodeD = { 'D', { NULL } };

    slist_init( &nodeA.list );
    slist_init( &nodeB.list );
    slist_init( &nodeC.list );
    slist_init( &nodeD.list );

    slist_insert( &list, &nodeA.list );
    slist_insert( &nodeA.list, &nodeB.list );
    slist_insert( &nodeB.list, &nodeC.list );
    slist_insert( &nodeC.list, &nodeD.list );

    SLIST_FOREACH ( &list, node ) {
        item_t* item = ( item_t* ) SLIST_TO_STRUCTP( node, item_t, list );
        ECHO ("%c", item -> key );
    }
    ECHO ("," );

    ECHO ("---" );

    SLIST_FORRANGE ( &nodeB.list, &nodeD.list, node ) {
        item_t* item = ( item_t* ) SLIST_TO_STRUCTP( node, item_t, list );
        ECHO ("%c", item -> key );
    }
    ECHO ("," );

    ECHO ("---" );

    SLIST_WHILE_HEAD ( &list, head ) {
        item_t* item = ( item_t* ) SLIST_TO_STRUCTP( head, item_t, list );
        ECHO ("%c ", item -> key );
        slist_unlink( &list, head );
    }
    ECHO ("," );

    ECHO ("%d", slist_is_empty( &list ) );
    
}

/*
 * 5. Counting:
 *      unsigned slist_count( const_SList list )
 *      SList slist_get_nth( SList list, int n )
 *      SList slist_get_nth_stop( SList list, int n, const_SList stop )
 */

TEST (count) {

    SLIST_AUTO( list );
    SLIST_AUTO( nodeA );
    SLIST_AUTO( nodeB );
    SLIST_AUTO( nodeC );

    slist_insert( &list, &nodeA );
    slist_insert( &nodeA, &nodeB );
    slist_insert( &nodeB, &nodeC );

    ECHO ("%u", slist_count( &list ) );
    ECHO ("%d", slist_get_nth( &list, 3 ) == &nodeC );
    ECHO ("%d", slist_get_nth_stop( &list, 3, &nodeC ) == NULL );
    
}

/*
 * 6. Sort:
 *      SList slist_sort( SList list, slist_cmpfn cmp )
 */

TEST (sort) {

    srand( time( NULL ) );
    
    SLIST_AUTO( list );

    unsigned int n = 1000000;

    item_t* items;
    if ( ( items = ( item_t* ) malloc( sizeof( item_t ) * n ) ) == NULL ) {
        return 1; // ERROR: not enough memory
    }

    for ( unsigned int i = 0; i < n; i++ ) {
        items[ i ].key = rand();
        slist_init( &items[ i ].list );
        slist_insert( &list, &items[ i ].list );
    }
    
    slist_sort( &list, cmp );

    int is_first_cmp = 1;
    int prev_key = 0;
    
    SLIST_FOREACH ( &list, x ) {
        item_t* item = SLIST_TO_STRUCTP( x, item_t, list );
        if ( is_first_cmp ) {
            is_first_cmp = 0;
        } else if ( prev_key > item -> key ) {
            return 2; // ERROR: wrong order of elements
        }
        prev_key = item -> key;
    }

    free( items );
    
    return 0;
    
}

/*
 * 7. Search:
 *      SList slist_find( const_SList list, const_SList pattern, slist_cmpfn cmp )
 *      SList slist_ufind( SList list, const_SList pattern, slist_cmpfn cmp )
 *      SList slist_sfind( const_SList list, const_SList pattern, slist_cmpfn cmp )
 */

TEST (search) {

    SLIST_AUTO( list );
    
    item_t nodeA = { 'A', { NULL } };
    item_t nodeB = { 'B', { NULL } };
    item_t nodeC = { 'C', { NULL } };
    item_t nodeD = { 'D', { NULL } };
    item_t nodeX = { '?', { NULL } };

    slist_init( &nodeA.list );
    slist_init( &nodeB.list );
    slist_init( &nodeC.list );
    slist_init( &nodeD.list );

    slist_insert( &list, &nodeA.list );
    slist_insert( &nodeA.list, &nodeB.list );
    slist_insert( &nodeB.list, &nodeC.list );
    slist_insert( &nodeC.list, &nodeD.list );

    nodeX.key = 'C';

    ECHO ("%d", slist_find( &list, &nodeX.list, cmp ) == &nodeC.list );
    ECHO ("%d", slist_ufind( &list, &nodeX.list, cmp ) == &nodeC.list );
    ECHO ("%d", slist_next( &nodeC.list ) == &nodeA.list );

    nodeX.key = 'A';
    ECHO ("%d", slist_sfind( &list, &nodeX.list, cmp ) == NULL );
    
}

TESTS_END
