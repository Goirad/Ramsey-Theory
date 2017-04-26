#include "defs.h"

/*
  Frees a list and all cells in it
*/
void freeList(Cell * list){
  Cell * current = list;
  Cell * next;
  while(current->next != NULL){
    next = current->next;
    free(current);
    current = next;
  }
  free(current);
}

/*
  Frees an array of lists with n sublists
*/
void freeListArray(Cell ** listArray, int n){
  for(int i = 0; i < n ; i++){
    freeList(listArray[i]);
  }
  free(listArray);
}

/*
  Prints a list in a single line, with values separated by spaces.
  Useful for debugging.
*/
void printList(Cell * list){
  Cell * current = list;
  printf("[");
  while(current->next != NULL){
    printf("%d ", current->value);
    current = current->next;
  }
  if(current->size == 1) printf("%d]\n", current->value );
  else printf("]\n");
}

/*
  Adds val to the end of the list provided.
*/
void addToList(Cell * list, int val){
  Cell * current = list;
  while(current -> next != NULL){
    current->size++;
    current = current->next;
  }
  current->size++;
  current->next = malloc(sizeof *current->next);
  current = current->next;
  current->size = 1;
  current->next = NULL;
  current->value = val;
}

/*
  Copies a list element by element. Useful for when you need to modify
  a list without destroying the original.
  Allocates dynamic memory, so remember to free it when you're done.
*/
Cell * copyList(Cell * list){
  Cell * t = calloc(1, sizeof *t);
  Cell * current = list;
  //if list has stuff, copy them
  if(current->size > 0){
    while(current->next != NULL){
      addToList(t, current->value);
      current = current->next;
    }
    addToList(t, current->value);
    Cell * ans = t->next;
    free(t);
    return ans;
  //if it's empty, just make a dummy list
  }else{
    t->size = 0;
    t->value = -1;
    t->next = NULL;
    return t;
  }
}

/*
  Returns the nth element of the list.
*/
int getListIndex(Cell * list, int n){
  Cell * current = list;
  int i = 0;
  while(i != n){
    current = current->next;
    i++;
  }
  return current->value;
}

/*
  Returns a pointer to the nth cell of the list.
*/
Cell* getListCellIndex(Cell * list, int n){
  Cell * current = list;
  int i = 0;
  while(i != n){
    current = current->next;
    i++;
  }
  return current;
}

/*
  Creates a deep copy of an array of n lists.
*/
Cell ** copyListArray(Cell *array[], int n){
  Cell **t = calloc(n, sizeof *t);
  for(int i = 0; i < n; i++){
    t[i] = copyList(array[i]);
  }
  return t;
}

/*
  This takes a permutation list perm and applies it to list,
  generating a new list that is then returned.
  For example
  list = [3 4 1 2 0]
  perm = [2 1 0 1 0]

  return = [1 4 3 0 2]
*/

Cell * permuteList(Cell * list, Cell * perm){
  Cell * result = calloc(1, sizeof(result));
  Cell * current = perm;
  Cell * copy = copyList(list);

  int copySize = copy->size;
  //while list still has a next element
  while(current->next != NULL){
    int address = current->value;
    //add the element at position address from copy into result
    addToList(result, getListIndex(copy, address));
    //remove it from copy
    if(address == 0){
      if(copy->next != NULL){
        Cell* toFree = copy;
        copy = copy->next;
        copySize--;
        free(toFree);
      }else{
        free(copy);
      }
    }else if(address == copySize - 1){
      free(getListCellIndex(copy, address));
      getListCellIndex(copy, address-1)->next=NULL;
      copySize--;
    }else{
      Cell * temp = getListCellIndex(copy, address);
      getListCellIndex(copy, address-1)->next = getListCellIndex(copy, address+1);
      free(temp);
      copySize--;
    }
    //go to the next element
    current = current->next;
  }
  //we hit the last element
  addToList(result, getListIndex(copy,current->value));
  freeList(copy);
  //the first element is meaningless, so free and skip it
  Cell * ans = result->next;
  free(result);
  return ans;
}
