#include "arraylist.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

ArrayList newArrayList() {
  ArrayList al = NULL;
  al = (ArrayList)malloc(sizeof(struct array_list_t));
  int rc = checkAllocationError(al);
  if (rc == SUCCESS) {
    al->size = DEFAULT_SIZE;
    al->used = 0;
    al->sequence = (void **)malloc(sizeof(void *) * DEFAULT_SIZE);
    rc = checkAllocationError(al->sequence);
    if (rc != SUCCESS) {
      perror("Error in the space allocation for the ArrayList\n");
    }
  }
  return al;
}

ArrayList newArrayListInitialCapacity(int capacity) {
  ArrayList al = NULL;
  al = (ArrayList)malloc(sizeof(struct array_list_t));
  int rc = checkAllocationError(al);
  if (rc == SUCCESS) {
    if (capacity < 0) {
      printf(
          "Negative capacity specified, allocating an array of default size\n");
      capacity = DEFAULT_SIZE;
    }
    al->size = capacity;
    al->used = 0;
    al->sequence = (void **)malloc(sizeof(void *) * capacity);
    rc = checkAllocationError(al->sequence);
    if (rc != SUCCESS) {
      perror("Error in the space allocation for the ArrayList\n");
    }
  }
  return al;
}

bool isEmptyArray(const ArrayList arraylist) {
  bool empty = true;
  if (arraylist->used > 0) {
    empty = false;
  }
  return empty;
}

int push_back(ArrayList arraylist, void *data) {
  int rc = SUCCESS;
  if (arraylist->used == arraylist->size) {
    arraylist->size *= 2;
    arraylist->sequence =
        realloc(arraylist->sequence, arraylist->size * sizeof(void *));
    rc = checkAllocationError(arraylist->sequence);
    if (rc == FAILURE) {
      perror(
          "Error in the allocation of the new arraylist doubling the size\n");
    }
  }
  arraylist->sequence[arraylist->used] = data;
  arraylist->used += 1;
  return rc;
}

void *getDataAtIndex(const ArrayList arraylist, int index) {
  void *data = NULL;
  if (index >= 0 && index < arraylist->used) {
    data = arraylist->sequence[index];
  }
  return data;
}

void printArrayList(const ArrayList arraylist, void toString(void *)) {
  if (isEmptyArray(arraylist)) {
    printf("The ArrayList is empty\n");
  } else {
    int i = 0;
    for (i = 0; i < arraylist->used; i++) {
      printf("[Item #%d] ", i);
      if (arraylist->sequence[i] != NULL) {
        toString(arraylist->sequence[i]);
      }
    }
  }
}

void destroyItemsArrayList(ArrayList arraylist, void deleteData(void *)) {
  int i;
  for (i = 0; i < arraylist->used; i++) {
    deleteData((void *)arraylist->sequence[i]);
  }
  free(arraylist->sequence);
  arraylist->sequence = NULL;
  arraylist->used = 0;
}

void destroyArrayList(ArrayList arraylist, void deleteData(void *)) {
  destroyItemsArrayList(arraylist, deleteData);
  free(arraylist);
}

bool isInArray(const ArrayList arraylist, void *data,
               bool isEqual(void *, void *)) {
  bool presence = false;
  int i = 0;
  while (!presence && i < arraylist->used) {
    if (isEqual(arraylist->sequence[i], data)) {
      presence = true;
    }
    i++;
  }
  return presence;
}

ArrayList emptyArray(ArrayList arraylist, void deleteData(void *)) {
  int rc = SUCCESS;
  destroyItemsArrayList(arraylist, deleteData);
  arraylist->size = DEFAULT_SIZE;
  arraylist->sequence = (void **)malloc(sizeof(void *) * DEFAULT_SIZE);
  rc = checkAllocationError(arraylist->sequence);
  if (rc != SUCCESS) {
    perror("Error in the space allocation for the ArrayList\n");
  }
  return arraylist;
}

int deleteElement(ArrayList arraylist, int index, void deleteElement(void *)) {
  float load_factor;
  int rc = SUCCESS;
  if (index >= 0 && index < arraylist->used) {
    deleteElement(arraylist->sequence[index]);
    arraylist->sequence[index] = arraylist->sequence[arraylist->used - 1];
    arraylist->used--;
  }
  load_factor = (float)arraylist->used / arraylist->size;
  if (load_factor <= LOAD_FACTOR_LIMIT) {
    arraylist->size /= 2;
    arraylist->sequence =
        realloc(arraylist->sequence, arraylist->size * sizeof(void *));
    rc = checkAllocationError(arraylist->sequence);
    if (rc == FAILURE) {
      perror("Error in the allocation of the new arraylist halving the size\n");
    }
  }
  return rc;
}

int getNumberElements(ArrayList arraylist) { return arraylist->used; }

int changeElementAtIndex(ArrayList arraylist, void *data, int index,
                         void deleteData(void *)) {
  int rc = FAILURE;
  if (index >= 0 && index < arraylist->used) {
    deleteData(arraylist->sequence[index]);
    arraylist->sequence[index] = data;
    rc = SUCCESS;
  }
  return rc;
}

int insertAtIndex(ArrayList arraylist, void *data, int index) {
  int rc = SUCCESS;
  int i;
  if (index < 0) {
    rc = FAILURE;
  } else {
    if (arraylist->used == arraylist->size) {
      arraylist->size *= 2;
      arraylist->sequence =
          realloc(arraylist->sequence, arraylist->size * sizeof(void *));
      rc = checkAllocationError(arraylist->sequence);
      if (rc == FAILURE) {
        perror(
            "Error in the allocation of the new arraylist doubling the size\n");
      }
    }
    // Inserting at the end
    if (index > arraylist->used) {
      printf("Index not present in the array, as default appending the "
             "element at the end of the list\n");
      rc = push_back(arraylist, data);
    } else {
      // Shifting to the right the elements
      for (i = arraylist->used; i > index; i--) {
        arraylist->sequence[i] = arraylist->sequence[i - 1];
      }
      arraylist->sequence[index] = data;
      arraylist->used++;
    }
  }
  return rc;
}

// ================== Test main =======================

void floatToString(void *data) { printf("%f\n", (*(float *)data)); }
void deleteFloat(void *data) {
  float *f_data = (float *)data;
  free(f_data);
}
bool isEqualFloat(void *first, void *second) {
  float *f_first, *f_second;
  f_first = (float *)first;
  f_second = (float *)second;
  return f_first == f_second;
}

/**
 * Method used in order to test the list data structure
 */
// int main(int argc, char **argv) {
int main_test_arraylist(int argc, char **argv) {
  // Some data
  int size;
  int i;
  int rc;
  float element;
  float *data;
  int choice;
  // New list
  ArrayList array;
  array = newArrayList();
  if (array == NULL) {
    printf("There were some problems in the list allocation");
    exit(FAILURE);
  }
  // List initialization
  printf(
      "Enter the number of elements you want to insert into the array list: ");
  scanf("%d", &size);
  if (size <= 0) {
    printf("Anything to do here\n");
  }
  for (i = 0; i < size; i++) {
    printf("Insert the %d element: ", i + 1);
    scanf("%f", &element);
    data = (float *)malloc(sizeof(float));
    if (data == NULL) {
      printf("There were some problems in the memory allocation\n");
      exit(FAILURE);
    }
    *data = element;
    rc = push_back(array, (void *)data);
    if (rc == FAILURE) {
      printf("There were some problems in the data insertion\n");
      exit(1);
    }
  }
  // Operation which the main can carry out
  do {
    printf("Which operation would you like to do?\n");
    printf("Insert 0 for printing the array\n");
    printf("Insert 1 for checking if the array is empty\n");
    printf("Insert 2 for inserting elements at the end of the array\n");
    printf("Insert 3 for inserting elements at a given index of the array\n");
    printf("Insert 4 for printing the element of the array in a given index\n");
    printf("Insert 5 for changing the element of the array in a given index\n");
    printf("Insert 6 for deleting the element of the array in a given index "
           "[note that the the indexes of the array may change]\n");
    printf("Insert 7 for emptying the array\n");
    printf("Insert 8 for checking the present of an element in the array\n");
    printf("Insert 9 for destroying the array\n");
    printf("Your choice: ");

    scanf("%d", &choice);
    switch (choice) {
    case 0:
      printArrayList(array, floatToString);
      break;
    case 1:
      printf("Is the list empty? %d\nSize: %d\n", isEmptyArray(array),
             getNumberElements(array));
      break;
    case 2:
      printf("Insert the new element to append to the array: ");
      scanf("%f", &element);
      data = (float *)malloc(sizeof(float));
      if (data == NULL) {
        printf("There were some problems in the memory allocation\n");
        exit(FAILURE);
      }
      *data = element;
      rc = push_back(array, (void *)data);
      if (rc == FAILURE) {
        printf("There were some problems in the data insertion\n");
        exit(1);
      }
      break;
    case 3:
      printf("Insert the new element to insert in the list: ");
      scanf("%f", &element);
      data = (float *)malloc(sizeof(float));
      if (data == NULL) {
        printf("There were some problems in the memory allocation\n");
        exit(FAILURE);
      }
      *data = element;
      printf("Insert the index where to insert the element: ");
      scanf("%d", &i);
      rc = insertAtIndex(array, (void *)data, i);
      if (rc == FAILURE) {
        printf("There were some problems in the data insertion\n");
        exit(1);
      }
      break;
    case 4:
      printf("Insert the index of the element to retrieve: ");
      scanf("%d", &i);
      data = (float *)getDataAtIndex(array, i);
      if (data == NULL) {
        printf("The list is empty or the element is NULL: size = %d\n",
               getNumberElements(array));
      } else {
        printf("The element at position %d of the array is %f\n", i, *data);
      }
      break;
    case 5:
      printf("Insert the new element to insert in the list: ");
      scanf("%f", &element);
      data = (float *)malloc(sizeof(float));
      if (data == NULL) {
        printf("There were some problems in the memory allocation\n");
        exit(FAILURE);
      }
      *data = element;
      printf("Insert the index of the element to change: ");
      scanf("%d", &i);
      rc = changeElementAtIndex(array, (void *)data, i, deleteFloat);
      if (rc == SUCCESS) {
        printf("Element changed successfully\n");
      } else {
        printf("There were some problems changing the element\n");
      }
      break;
    case 6:
      printf("Insert the index of the element to delete: ");
      scanf("%d", &i);
      rc = deleteElement(array, i, deleteFloat);
      if (rc == SUCCESS) {
        printf("Element removed successfully\n");
      } else {
        printf("There were some problems deleting the element\n");
      }
      break;
    case 7:
      array = emptyArray(array, deleteFloat);
      if (array == NULL) {
        printf("There were some problems emptying the list\n");
        exit(FAILURE);
      }
      break;
    case 8:
      printf("Insert the element to find in the array: ");
      scanf("%f", &element);
      printf("Is the element in the list? %d \n",
             isInArray(array, (void *)&element, isEqualFloat));
      break;
    default:
      destroyArrayList(array, deleteFloat);
      printf("ArrayList deallocated, the program will end\n");
      break;
    }
  } while (choice >= 0 && choice < 9);

  return 0;
}
