// File: $Id: queueADT.c,v 1.3 2014/07/14 20:43:51 csci243 Exp $
//
// Description:  The Queue ADT module
//
// Author:  sdd7427@rit.edu

#ifndef _QUEUE_IMPL_
#define _QUEUE_IMPL_

// include external libraries
#include <stdlib.h>
#include <stdio.h>

// A way of holding the data and placing it an ordered line
struct node { 
    void *data;
    struct node *prev;
    struct node *next;
};
typedef struct node *Node;

// Implementation of the QueueADT Object
struct queue {
    Node head;
    Node tail;
};
typedef struct queue *QueueADT;

#include "queueADT.h"

//Local variables
bool use_fifo = true;
int (*compare)(const void *a,const void *b);

/**
 * Create a QueueADT which uses the supplied function as a comparison
 * routine.
 *
 * The comparison function takes two void* parameters, and returns
 * an integer result which indicates the relationship between the
 * two things:
 *
 *  Result  Relationship
 *  ======  ===========
 *   < 0    a < b
 *   = 0    a == b
 *   > 0    a > b
 *
 * where ">" and "<" are dependent upon the data being compared
 *
 * @param cmp the address of the comparison function to be used for
 *    ordering this queue, or NULL if standard FIFO behavior is desired
 * @return a pointer to a QueueADT, or NULL if the allocation fails
 */
QueueADT que_create( int (*cmp)(const void *a,const void *b) ) {
    if(cmp != 0) {
        use_fifo = false;
    } else {
        // the compare function is defined and stored as a global variable
        compare = cmp;
    }
    // QueueADT q = (QueueADT) malloc( sizeof(struct queue) );
    QueueADT q = { (Node *)0, (Node *)0 };
    // q->head = 0;
    // q->tail = 0;
    return q ;
}

/**
 * Tear down and deallocate the supplied QueuADT.
 *
 * @param queue - the QueueADT to be manipulated
 */
void que_destroy( QueueADT queue ) {
    que_clear(queue);
    free(queue);
}

/**
 * Remove all contents from the supplied QueueADT.
 *
 * @param queue - the QueueADT to be manipuated
 */
void que_clear( QueueADT queue ) {
    Node n;
    while( !que_empty(queue) ) {
        n = que_remove(queue);
        free(n);
    }
}

/**
 * Insert the specified data into the Queue in the appropriate place
 *
 * Uses the queue's comparison function to determine the appropriate
 * place for the insertion.
 *
 * @param queue the QueueADT into which the value is to be inserted
 * @param data the data to be inserted
 * @exception If the value cannot be inserted into the queue for
 *     whatever reason (usually implementation-dependent), the program
 *     should terminate with an error message.  This can be done by
 *     printing an appropriate message to the standard error output and
 *     then exiting with EXIT_FAILURE, or by having an assert() fail.
 */
void que_insert( QueueADT queue, void *data ) {
    
    if(use_fifo) {
        int (*compare)(int*, int*) = ({
            int __fn__ (int x, int y) { return x - y; }
            __fn__;
        });
    }
    // create a new node with the new data
    Node new_node = (Node) malloc( sizeof( struct node ) );
    new_node->data = data;

    // check if the head is null, if null then set to head
    if( que_empty( queue ) ) {
        queue->head = new_node;
        queue->tail = new_node;
        return ;
    }

    Node curr_node = queue->head;
    // check how the head data compares to the new data
    while( curr_node != 0 ) {

        // the new_node is sorted before the current node
        if( compare(curr_node->data, new_node->data) >= 0 ) {
            if( curr_node == queue->head ) {
                new_node->next = curr_node;
                curr_node->prev = new_node;
                queue->head = new_node;
            }

            // if curr_node is the head then make the head of the queue the new node
            // what if its the tail?
            new_node->next = curr_node;
            return ;
        }
        // the new_node is sorted after the current node
        else if(compare(curr_node->data, new_node->data) < 0) {

            // the current node is the end, place on the end
            if(curr_node == queue->tail) {
                curr_node->next = new_node;
                return ;
            }
        }
    }
}

/**
 * Remove and return the first element from the QueueADT.
 *
 * @param queue the QueueADT to be manipulated
 * @return the value that was removed from the queue
 * @exception If the queue is empty, the program should terminate
 *     with an error message.  This can be done by printing an
 *     appropriate message to the standard error output and then
 *     exiting with EXIT_FAILURE, or by having an assert() fail.
 */
void *que_remove( QueueADT queue ) {
    if(que_empty(queue)) {
        fputs("Cannot remove item from an empty queue.", stderr);
        exit(EXIT_FAILURE);
    }

    Node old_head = queue->head;
    queue->head = queue->head->next;
    return old_head->data;
}

/**
 * Indicate whether or not the supplied Queue is empty
 *
 * @param the QueueADT to be tested
 * @return true if the queue is empty, otherwise false
 */
bool que_empty( QueueADT queue ) {
    bool result = queue->head == 0 && queue->tail == 0;
    return result;
}

#endif

// Revisions:
//  $Log: queueADT.c,v $
//  Revision 1.3  2014/07/14 20:43:51  csci243
//  Implemented remainging, still getting malloc problems...
//
//  Revision 1.2  2014/07/13 16:00:34  csci243
//  Implemented create, destroy, and clear with errors
//
//  Revision 1.1  2014/07/13 12:56:23  csci243
//  Initial revision
//
