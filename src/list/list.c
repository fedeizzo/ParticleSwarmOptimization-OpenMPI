#include "list.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

List newList() {
  List nl = NULL;
  nl = (List)malloc(sizeof(struct list_t));
  int rc = checkAllocationError(nl);
  if (rc == SUCCESS) {
    nl->head = NULL;
    nl->tail = NULL;
    nl->size = 0;
  }
  return nl;
}

bool isEmpty(const List list) {
  bool empty = 0;
  if (list->size <= 0) {
    empty = 1;
  }
  return empty;
}

int enqueue(List list, void *data) {
  int ret = SUCCESS;
  Element elem = (Element)malloc(sizeof(struct element_t));
  ret = checkAllocationError(elem);
  if (ret == SUCCESS) {
    elem->data = data;
    elem->next = NULL;
    elem->prev = list->tail;
    if (isEmpty(list)) {
      list->head = elem;
    } else {
      list->tail->next = elem;
    }
    list->tail = elem;
    list->size++;
  }
  return ret;
}

int push(List list, void *data) {
  int ret = SUCCESS;
  Element elem = (Element)malloc(sizeof(struct element_t));
  ret = checkAllocationError(elem);
  if (ret == SUCCESS) {
    elem->data = data;
    elem->next = list->head;
    elem->prev = NULL;
    if (isEmpty(list)) {
      list->tail = elem;
    } else {
      list->head->prev = elem;
    }
    list->head = elem;
    list->size++;
  }
  return ret;
}

void printList(const List list, void toString(void *)) {
  Element tmp_element = list->head;
  if (isEmpty(list)) {
    printf("The list is empty\n");
  } else {
    int i = 0;
    printf("The list has %d elements\n", list->size);
    while (tmp_element != NULL) {
      printf("[Item #%d] ", i);
      toString(tmp_element->data);
      tmp_element = tmp_element->next;
      i++;
    }
  }
}

// TODO comment
void destroyElements(List list, void deleteElement(void *)) {
  Element tmp_node;
  Element tmp_prev_node;
  if (!isEmpty(list)) {
    tmp_node = list->head;
    tmp_prev_node = NULL;
    while (tmp_node != NULL) {
      tmp_prev_node = tmp_node;
      tmp_node = tmp_node->next;
      deleteElement(tmp_prev_node->data);
      free(tmp_prev_node);
      list->size--;
    }
    list->head = NULL;
    list->tail = NULL;
  }
}

void destroyList(List list, void deleteElement(void *)) {
  if (list != NULL) {
    destroyElements(list, deleteElement);
    free(list);
  }
}

int dequeue(List list, void deleteElement(void *)) {
  int ret = SUCCESS;
  Element elem;
  if (isEmpty(list)) {
    ret = FAILURE;
  } else {
    elem = list->tail;
    list->tail = elem->prev;
    list->size--;
    if (isEmpty(list)) {
      list->head = NULL;
    } else {
      list->tail->next = NULL;
    }
    deleteElement(elem);
    free(elem);
  }
  return ret;
}

void *front(const List list) {
  void *data = NULL;
  if (!isEmpty(list)) {
    data = list->head->data;
  }
  return data;
}

int pop(List list, void deleteElement(void *)) {
  int ret = SUCCESS;
  if (list != NULL) {
    Element elem;
    if (isEmpty(list)) {
      ret = FAILURE;
    } else {
      elem = list->head;
      list->head = elem->next;
      list->size--;
      if (isEmpty(list)) {
        list->tail = NULL;
      } else {
        list->head->prev = NULL;
      }
      deleteElement(elem->data);
      free(elem);
    }
  } else {
    ret = FAILURE;
  }
  return ret;
}

void *tail(const List list) {
  void *data = NULL;
  if (!isEmpty(list)) {
    data = list->tail->data;
  }
  return data;
}

bool isIn(const List list, void *data, bool isEqual(void *, void *)) {
  bool found = 0;
  Element tmp_elem = list->head;

  while (!found && tmp_elem != NULL) {
    if (isEqual(tmp_elem->data, data)) {
      found = 1;
    }
    if (!found) {
      tmp_elem = tmp_elem->next;
    }
  }
  return found;
}
