#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>
#include <limits.h>
#define false 0
#define true 1

/*********************************************************************************************
 * List data types
 ********************************************************************************************/

/***
 * List element data type
 ***/
typedef struct _node
{
    int value;          /**< the value to be stored in the list element */
    struct _node *next; /**< pointer to the next element of the linked list */
} node_t;

/***
 *  List data type
 *
 *  It uses two dummy elements, one at the head
 *  and one at the tail of the list.
 *  These dummy elements are ignored in the implementation
 ***/
typedef struct _list
{
    node_t *head; /**< pointer to the head dummy-element */
    node_t *tail; /**< pointer to the tail dummy-element */
} list_t;

////this is for automatic processing, specify data types before this line
#ifndef BUILD_INCLUDE_FOR_VALIDATOR

////enable multiple threads for parallel add, find and remove
//#define START_PARALLEL

/*********************************************************************************************
 * Function declarations to manage access to the list
 ********************************************************************************************/

/***
 * Creates a new list
 *
 * @returns      the pointer to the new list
 ***/
list_t *init_list()
{
    // Allocate the list data structure
    list_t *list = (list_t *)calloc(1, sizeof(list_t));
    // Add head and tail elements
    list->head = (node_t *)calloc(1, sizeof(node_t));
    list->tail = (node_t *)calloc(1, sizeof(node_t));
    list->head->value = -1;
    list->tail->value = -1;
    list->head->next = list->tail;
    list->tail->next = NULL;
    return list;
}

/***
 * Destroys a list and frees all remaining elements
 *
 * @param list   the list to be destroyed, this pointer is invalid after the call
 ***/
void destroy_list(list_t *list)
{
    node_t *head = list->head;
    node_t *tail = list->tail;
    list->head = NULL;
    node_t *elem = head;
    node_t *next = head->next;
    while (elem != NULL && next != NULL)
    {
        free(elem);
        elem = next;
        next = next->next;
    }
    //last remaining element is tail
    free(tail);
}

/***
 * add an element to the list
 *
 * @param list   pointer to the list
 * @param item   the value to be added in the list
 * @returns      false if the value is already contained, true if the value was added
 ***/
int list_add(list_t *list, int item)
{

    node_t *pred = list->head;

    //ignore head element
    node_t *curr = pred->next;

    //find correct postition of item in list
    while (item < curr->value && curr != list->tail)
    {
        pred = curr;
        curr = curr->next;
    }

    //is item already in list?
    if (curr->value == item)
    {
        return false;
    }

    //create new node and add value
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = item;
    new_node->next = curr;
    pred->next = new_node;
    return true;
}