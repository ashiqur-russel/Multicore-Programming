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

/***
 *  remove an element from the list
 *
 * @param list   pointer to the list
 * @param item   the value to be removed from the list
 * @returns      false if the value not contained, true if the value was removed
 ***/
int list_remove(list_t *list, int item)
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

    //is item in list? if yes, remove node, correct pointers and free memory
    if (item == curr->value)
    {
        pred->next = curr->next;
        free(curr);
        return true;
    }
    //only reached if item is not in list
    return false;
}

/***
 * Find an element in the list
 *
 * @param list   pointer to the list
 * @param item   the value searched for in the list
 * @returns      false if the value not contained, true if the value is contained
 ***/
int list_find(list_t *list, int item)
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

    //is item in list?
    if (item == curr->value)
    {
        return true;
    }
    //only reached if item is not in list
    return false;
}

/*********************************************************************************************
 * Function declarations overwritten by OpenSubmit
 ********************************************************************************************/

void print_list(list_t *);
void prefill_list(list_t *, const int);

/***
 * In this function we can define tests for implementation
 *
 * @param list    a pointer to the list
 ***/
void check_list(list_t *list)
{
    //write your own tests here
}

/*********************************************************************************************
 *  changes should be made inside the list functions.
 ********************************************************************************************/

int main(int argc, char **argv)
{

    int max_length = 1000;
    unsigned int test_ops = 100000;

    unsigned int i;
    list_t *list = init_list();
    srand(100);
    prefill_list(list, max_length);

    //start test scenario with calls to the different list functions
    printf("Starting test...\n");
    fflush(stdout);
    double start = omp_get_wtime();
#ifdef START_PARALLEL
#pragma omp parallel for private(i)
#endif
    // This loop is parallelized
    for (i = 0; i < test_ops; i++)
    {
        int r = rand() % max_length;
        switch (i % 3)
        {
        case 0:
            list_add(list, r);
            break;
        case 1:
            list_find(list, r);
            break;
        case 2:
            list_remove(list, r);
            break;
        default:
            printf("Default case reached!\n");
        }
    }
    check_list(list);

    double end = omp_get_wtime();
    printf("Time taken: %f seconds\n", end - start);

    destroy_list(list);

    return EXIT_SUCCESS;
}

/*********************************************************************************************
 * The remainder of this file consists of helper functions.
 * These functions should not be changed
 ********************************************************************************************/

/***
 * fills the list with (max_length/2) random elements for the test scenario
 *
 * @param list         a pointer to the list
 * @param max_length   the maximum number of elements to be contained in the list
 ***/
void prefill_list(list_t *list, const int max_length)
{
    int i;
    //  prefill the data structure
    for (i = 0; i < max_length / 2; i++)
    {
        int r = rand() % max_length;
        list_add(list, r);
    }
}

/***
 * Print the list
 *
 * @param list    a pointer to the list
 ***/
void print_list(list_t *list)
{
    node_t *d;
    if (list->head == NULL || list->tail == NULL)
    {
        return;
    }

    for (d = list->head; d != NULL; d = d->next)
    {
        printf("%d ", d->value);
    }
    printf("\n");
}

#endif
