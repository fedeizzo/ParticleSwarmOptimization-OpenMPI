#ifndef __ARRAY_LIST_H_
#define __ARRAY_LIST_H_

// Import of the configuration elements
#include "../../include/config.h"
#include <stdbool.h>

// Default size of an arraylist
#define DEFAULT_SIZE 10
// Expansion factor which lead to a constant amortized computational cost for
// insertion
#define EXPANSION_FACTOR 2
// Load factor for halving the size of the array, this lead to a constant
// amortized computational cost for removal
#define LOAD_FACTOR_LIMIT 0.25

/**
 * @struct array_list_t
 * @brief ArrayList data type
 *
 * @var array_list_t::size
 * size of the array list (allocated space)
 * @var array_list_t::used
 * occupied cells of the array list (used space)
 * @var array_list_t::sequence
 * dynamic array
 */
typedef struct array_list_t {
  int size; // size of the array list (allocated space)
  int used;
  void **sequence;
} array_list_t;

typedef array_list_t *ArrayList;

/**
 * ArrayList data type constructor
 *
 * Allocates a new ArrayList on the heap with size equal to DEFAULT_SIZE
 *
 * returns [ArrayList] a new arraylist
 */
ArrayList newArrayList();

/**
 * ArrayList data type constructor
 *
 * Allocates a new ArrayList on the heap with size equal to capacity
 * @param capacity: size to initialize the array
 *
 * returns [ArrayList] a new arraylist
 */
ArrayList newArrayListInitialCapacity(int capacity);

/**
 * Method employed to check whether the ArrayList is empty or not
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 *
 * returns [bool] whether the array is empty or not
 */
bool isEmptyArray(const ArrayList arraylist);

/**
 * Method used to add element at the end of the ArrayList
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [void] *data: data to insert in the ArrayList
 *
 * returns [int] error code
 */
int push_back(ArrayList arraylist, void *data);

/**
 * Method to get the element at at the specified index
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [int] index: index of the element to retrieve
 */
void *getDataAtIndex(const ArrayList arraylist, int index);

/**
 * Method used to print the entire arraylist
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [void] toString: toString method used to print the elments of the ArrayList
 */
void printArrayList(const ArrayList arraylist, void toString(void *));

/**
 * Method to destroy the ArrayList
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [void] deleteData: deleteData method used to deallocate the elements of the
 * ArrayList
 */
void destroyArrayList(ArrayList arraylist, void deleteData(void *));

/**
 * Method employed to look for an element in the ArrayList
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [void] *data: data to insert in the ArrayList
 * [bool] isEqual: method used to compare the element of the arraylist
 *
 * returns [bool] whether the element is in the array or not
 */
bool isInArray(const ArrayList arraylist, void *data,
               bool isEqual(void *, void *));

/**
 * Method used to delete the element of the ArrayList
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [int] index: index of the element to delete
 * [void] deleteData: method used to deallocate the element of the
 * ArrayList
 *
 * returns [int] error code
 */
int deleteElement(ArrayList arraylist, int index, void deleteData(void *));

/**
 * Method used to empty the array
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [void] deleteData: method used to deallocate the elements of the
 * ArrayList
 *
 * returns [ArrayList] new ArrayList
 */
ArrayList emptyArray(ArrayList arraylist, void deleteData(void *));

/**
 * Method used to return the number of elements in the ArrayList
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 *
 * returns [int] number of elements
 */
int getNumberElements(ArrayList arraylist);

/**
 * Method used to change the element at a specific index
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [void] *data: data to replace
 * [int] index: intex where to replace the element
 * [void] deleteData: function which is used to delete the replaced element
 *
 * returns [int] error code
 */
int changeElementAtIndex(ArrayList arraylist, void *data, int index,
                         void deleteData(void *));

/**
 * Method used to insert the element at a specific index, shifting all the other
 * elements to the right. If the element is over the last index resent in the
 * array it just append the elment at the end of the arraylist
 *
 * [ArrayList] arraylist: pointer to the head of the arraylist
 * [void] *data: data to insert
 * [int] index: intex where to insert the element
 *
 * returns [int] error code
 */
int insertAtIndex(ArrayList arraylist, void *data, int index);
#endif
