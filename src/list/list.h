#ifndef __LIST_H_
#define __LIST_H_

// Import of the configuration elements
#include "../../include/config.h"
#include <stdbool.h>

/**
 * Element of the list: element_t
 *
 * [void] *data: data of the element
 * [struct Node] *next: next element of the list
 * [struct Node] *prev: previous element of the list
 */
typedef struct element_t {
  void *data;
  struct element_t *next;
  struct element_t *prev;
} element_t;

typedef element_t *Element;

/**
 * List data type
 *
 * [int] size: size of the list
 * [Node] head: pointer at the node of the list
 * [Node] tail: pointer at the tail fo the list
 *
 */
typedef struct list_t {
  int size;
  Element head;
  Element tail;
} list_t;

typedef list_t *List;

/**
 * List data type constructor
 *
 * Allocates a new list on the heap with size equal to 0
 * and both head and tail pointing to NULL
 *
 * returns [List] new list
 */
List newList();

/**
 * Method wich returns true if the list is empty and false if the list is not
 *
 * [List] list: constant pointer to the list
 *
 * returns [bool] depending on whether the list is empty or not
 */
bool isEmpty(const List list);

/**
 * Method which adds a new node data at the end of the list
 *
 * [List] list: pointer at the list structure
 * [void] data*: pointer at the data to add to the end of the list
 *
 * returns [int] error code
 */
int enqueue(List list, void *data);

/**
 * Method which adds a new node data at the beginning of the list
 *
 * [List] list: pointer at the list structure
 * [void] data: pointer at the node data to add at the beginning of the list
 *
 * returns [int] error code
 */

int push(List list, void *data);

/**
 * Method which prints the content of the list
 *
 * [List] list: pointer at the list structure
 * [void] toString: function to call on each element of the list
 */
void printList(const List list, void toString(void *));

/**
 * Method to deallocate the entire list
 *
 * [List] list: size of the list
 * [void] deleteElement: function to call on each element of the list
 */
void destroyList(List list, void deleteElement(void *));

/**
 * Method to remove an element at the end of the list
 *
 * [List] list: pointer at the list structure
 * [void] deleteElement: function to call on each element of the list
 *
 * returns [int] error code
 */
int dequeue(List list, void deleteElement(void *));

/**
 * Method to return the initial element of the list, the one at the beginning
 *
 * [List] list: pointer at the list structure
 *
 * returns [void] pointer at the data of the node
 */
void *front(const List list);

/**
 * Method to remove the element at the beginning of the list
 *
 * [List] list: pointer at the list structure
 * [void] deleteElement: function to call on each element of the list
 *
 * returns [int] error code
 */
int pop(List list, void deleteElement(void *));

/**
 * Method which returns the last element of the list
 *
 * [List] list: pointer to the list structure
 *
 * returns [void] pointer at the data of the node
 */
void *tail(const List list);

/**
 * Method which returns true if the element passed is in the list
 *
 * [List] list: pointer at the list structure
 * [void] *data: pointer at the data to search within the list
 * [bool] isEqual: function to use to compare the elements
 */
bool isIn(const List list, void *data, bool isEqual(void *, void *));
#endif
