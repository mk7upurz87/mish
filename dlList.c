/**********************************************************
 * dlList.c
 * $Id: dlList.c,v 1.0 2014/04/17 18:20:13 csci243 Exp $
 *
 * implementation to the double linked list with internal,
 *      tracking cursor mechanism.
 *
 * @author: sdd7427 - Shaun DeVos
 *
 **********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

#define _DLL_NODE_
struct Node {
    void *data;
    struct Node *prev;
    struct Node *next;
};

typedef struct DlList_T {
    struct Node *head;
    size_t num;
} *DlList_T;

static int cursor = 0;

#define _DLL_IMPL_

#include "dlList.h"
#include "dlCursor.h"

int getCursor() {
    return cursor;
}

void setCursor(int n) {
    cursor = n;
}

void * getDataAtCursor( DlList_T lst ) {
    return dll_get(lst, cursor);
}

/**
 * dll_create constructs an instance of an empty double-linked list.
 * @return instance of double-linked list (this is a pointer).
 */
DlList_T dll_create( void ) {
    DlList_T newList = (DlList_T) malloc(sizeof(DlList_T));
    newList->head = 0;
    newList->num = 0;
    setCursor(0);
    return newList;
}

/**
 * dll_destroy clears and frees all storage associated with
 * the linked list data structure.
 * The function handles memory management of all data payloads.
 * @param lst the linked list to destroy.
 * @post lst is no longer usable; the function freed all its memory.
 */
void dll_destroy( DlList_T lst ) {
    if( lst == 0 ) {
        return ;
    }

    dll_clear( lst );
    free( lst );
}

/**
 * dll_clear clears the list content, making the list empty.
 * dll_clear assumes all data payloads are dynamic and frees their storage.
 * @param lst the linked list to clear.
 * @post lst is now in an empty state.
 */
void dll_clear( DlList_T lst ) {

    if(lst == 0) {
        return ;
    }

    struct Node *curr_node = lst->head;
    struct Node *node_tmp;
    
    // cycle through the linkedlist and free each node
    while(curr_node != 0) {
        node_tmp = curr_node->next;
        free(node_tmp);
        curr_node = node_tmp;
    }
    free( lst->head );
    lst->num = 0;
    cursor = 0;
}

/**
 * dll_move_to moves the cursor to the requested index, if index is valid.
 * @param lst the list whose cursor should move.
 * @param index the 0-based index to which to move the cursor.
 * @return true if the cursor move was successful.
 */
bool dll_move_to( DlList_T lst, int indx ) {

    // if the index is out of the bounds of the list
    if( indx > dll_size(lst) - 1 || indx < 0 ) {
        return false;
    }

    struct Node *node_tmp = lst->head;
    int i = 0;

    // cycle through the linkedlist to check that the index is on the list
    while(i < indx) {
        node_tmp = node_tmp->next;
        i++;
    }
    if( node_tmp != 0 ) {
        setCursor(indx);
        return true;
    }
    return false;
}

/**
 * dll_has_next returns whether or not the cursor refers to a valid position.
 * @param lst the list to check.
 * @return non-zero if the cursor refers to a valid current position.
 */
int dll_has_next( DlList_T lst ) {
    return dll_get(lst, getCursor()) == 0 ? 0 : 1;
}

/**
 * dll_next returns the current item and advance forward to next item.
 * The list module retains ownership of the memory address returned.
 * The pointer returned refers to the actual data; the client does not own it.
 * @param lst the list to iterate over.
 * @return the data pointer of the current item.
 * @pre the cursor refers to a valid current position.
 * @post the cursor refers to the position after the current one, if any.
 */
void * dll_next( DlList_T lst ) {

    int position = getCursor();
    void *item;

    if( dll_has_next(lst) ) {
        item = dll_get(lst, position);
     
        // only if the next position is valid, set cursor to it.
        int next_pos = position + 1;
        if(dll_get(lst, next_pos) != 0) {
            setCursor(next_pos);
        }
    }

    return item;
}

/**
 * dll_prev returns the current item and advance backward to previous item.
 * The list module retains ownership of the memory address returned.
 * The pointer returned refers to the actual data; the client does not own it.
 * @param lst the list to iterate over.
 * @return the data pointer of the current item.
 * @pre the cursor refers to a valid current position.
 * @post the cursor refers to the position before the current one, if any.
 */
void * dll_prev( DlList_T lst ) {

    int position = getCursor();
    if( !dll_has_next( lst )) {
        return (void *)0;
    }
    void *item = dll_get(lst, position);

    // only if the next position is valid, set it.
    int prev_pos = position - 1;
    if(dll_get(lst, prev_pos) != 0) {
        setCursor(prev_pos);
    }
    return item;
}

/**
 * dll_size returns the size of the list.
 * @param lst the subject list.
 * @return the count of items in the list.
 */
int dll_size( DlList_T lst ) {
    // if the list is not initialized, return a negative length
    return lst == 0 ? -1 : lst->num;
}

/** 
 * dll_append appends an item to the end of the list.
 * The function assumes ownership of the memory of the data payload.
 * @param lst the subject list.
 * @param data a pointer to the item to append.
 * @post cursor moves to refer to the appended item. the lst size grows by 1.
 */
void dll_append( DlList_T lst, void *data ) {
    dll_insert_at( lst, lst->num, data );
}

/**
 * dll_insert_at inserts an item at the requested position, if index is valid.
 * The client is responsible for memory management of the item's storage.
 * The function assumes ownership of the memory of the data payload.
 * @param lst the subject list.
 * @param index the 0-based position.
 * @param data a pointer to the item to append.
 * @pre index must be in the range [0...dll_size(lst) ).
 * @post if successful, cursor moves to position of inserted item.
 * @post if successful, the lst size grows by 1.
 */
void dll_insert_at( DlList_T lst, int indx, void *data ) {

    // the lst is valid and the index to insert is either in the list or at the
    // end
    if( indx > dll_size( lst ) || indx < 0 ) {
        return ;
    }

    // create a new node with the data inside of it
    struct Node *new_node = (struct Node *) malloc( sizeof(data) +
        ( sizeof(struct Node *) * 2 ) );
    new_node->data = data;

    // if the list has no elements in it
    if(dll_size(lst) == 0) {
        lst->head = new_node;
    }
    
    // the list has at least 1 element in it
    else {
        struct Node *prev_node;
        struct Node *curr_node = lst->head;
        
        // traverse the list to find the position to insert the new node
        // starting at the head of the list
        for(int i = 0; i < indx; i++) {
            prev_node = curr_node;
            curr_node = curr_node->next;
        }

        // if we are appending to the end of the list
        if(curr_node == 0) {
            prev_node->next = new_node;
            new_node->prev = prev_node;
        }

        // if we are replacing the head of the list
        else if(curr_node->prev == 0) {
            new_node->next = curr_node;
            curr_node->prev = new_node;
            lst->head = new_node;
        }

        // we are inserting into the middle of the list
        else {
            // link previous node to the new node
            prev_node->next = new_node;
            new_node->prev = prev_node;

            // link the next (current) node to the new node
            curr_node->prev = new_node;
            new_node->next = curr_node;
        }
    }
    
    setCursor(indx);
    lst->num++;
}

/**
 * dll_get returns a pointer to the item at index; item remains in list.
 * The list module retains ownership of the memory address returned.
 * The pointer returned refers to the actual data; the client does not own it.
 * @param lst the subject list.
 * @param index the 0-based position.
 * @return pointer to the item requested.
 * @pre index must be in the range [ 0...dll_size(lst) ).
 * @post cursor position does not change. lst size and content is unchanged.
 */
void * dll_get( DlList_T lst, int indx ) {

    // the lst is valid and the index is either in the list or at the end
    if( indx > dll_size( lst ) || indx < 0 ) {
        return 0;
    }

    struct Node *curr_node = lst->head;
    for( int i = 0; i != indx; i++ ) {
        curr_node = curr_node->next;
    }
    return curr_node != 0 ? curr_node->data : (void *)0;
}

/**
 * dll_set replaces the pointer to the item with the data value.
 * The function transfers ownership of the old memory to the client.
 * The client is responsible for memory management of the old, returned data.
 * @param lst the subject list.
 * @param index the 0-based position.
 * @param data a pointer to the item to set, replacing the existing entry.
 * @return pointer to the item that was replaced.
 * @pre index must be in the range [ 0...dll_size(lst) ).
 * @post data is the value of the list at the position of the index.
 */
void * dll_set( DlList_T lst, int indx, void *data ) {

    // the lst is valid and the index is either in the list or at the end
    if( indx > dll_size( lst ) || indx < 0 ) {
        return 0;
    }

    struct Node *curr_node = lst->head;

    for( int i = 0; i != indx; i++ ) {
        curr_node = curr_node->next;
    }
    void *curr_data = curr_node->data;
    curr_node->data = data;

    return curr_data;
}

/**
 * dll_pop removes the item at the index and returns its data pointer.
 * The function transfers ownership of the old memory to the client.
 * The client is responsible for the memory management of the data removed.
 * The cursor moves to the next position, if present. Otherwise the cursor
 * moves to the previous position in the list, if present; Otherwise the
 * cursor is invalid since this function deleted the last line in the list.
 * @param lst the subject list.
 * @param index the 0-based position.
 * @return pointer to the item removed.
 * @pre index must be in the range [ 0...dll_size(lst) ).
 * @post cursor moves to adjacent position or is invalid if list is now empty.
 */
void * dll_pop( DlList_T lst, int indx ) {

    // the lst is valid and the index to insert is either in the list or at the
    // end
    if( indx > dll_size( lst ) || indx < 0 ) {
        return (void *)0;
    }

    struct Node *curr_node = lst->head;
    for(int i = 0; i < indx; i++) {
        curr_node = curr_node->next;
    }

    void *curr_data = (void *)0;

    // if the head of the list is not removed already
    if( curr_node != 0 ) {
        
        // track the sides of the gap to tie the list back together
        struct Node *prev_node = curr_node->prev;
        struct Node *next_node = curr_node->next;

        // if popping the head of the list
        if( curr_node == lst->head ) {
            lst->head = next_node;

            // if the next node is a valid node, then the next node will be at
            // the cursor's current position
            if( next_node != 0 ) {
                setCursor(dll_index( lst, next_node ));
            }
        }

        // if popping the end of the list
        else if( next_node == 0 ) {
            prev_node->next = 0;
            setCursor(dll_index( lst, prev_node ));
        }

        // if popping from the middle of the list
        else {
            next_node->prev = prev_node;
            prev_node->next = next_node;
            setCursor(dll_index( lst, next_node ));
        }
        
        // decrement the length of the list by 1 and return a new reference to the
        // data from the deleted node.
        lst->num--;
        curr_data = curr_node->data;
        free(curr_node);
    }

    return curr_data;
}

/**
 * dll_index returns a 0-based index of the data in the list or -1 if absent.
 * @param lst the subject list.
 * @param data a pointer to the item to find.
 * @return 0-based index of the data in the list or -1 if absent.
 * @post cursor position does not change. lst size and content is unchanged.
 */
int dll_index( DlList_T lst, void *data ) {
    
    // the lst is not valid return a negative value
    if( lst == 0 ) {
        return -1;
    }
    
    struct Node *curr_node = lst->head;

    int i = 0;
    while( curr_node != 0 && curr_node->data != data ) {
        curr_node = curr_node->next;
        i++;
    }
    return i;
}

/**
 * dll_empty checks for an empty list.
 * @param lst: the subject list.
 * @return true if the list is empty.
 * @post cursor position does not change. lst size and content is unchanged.
 */
bool dll_empty( DlList_T lst ) {
    return dll_size(lst) == 0;
}