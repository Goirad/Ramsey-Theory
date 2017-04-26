#include "defs.h"

/*
  Frees a list and all cells in it
*/
void freeList(List * list){
  Cell * next;
  if(list->length > 0){
    Cell * current = list->first;
    while(current->next != NULL){
      next = current->next;
      free(current);
      current = next;
    }
    free(current);
  }
  //printf("freed current\n");
  free(list);
  //printf("freed list\n");
}

/*
  Frees an array of lists with n sublists
*/
void freeListArray(List *listArray[], int n){
  for(int i = 0; i < n ; i++){
    freeList(listArray[i]);
  }
  free(listArray);
}

/*
  Prints a list in a single line, with values separated by spaces.
  Useful for debugging.
*/
void printList(List * list){
  Cell * current = list->first;
  printf("[");
  while(current->next != NULL){
    printf("%d ", current->value);
    current = current->next;
  }
  printf("%d]\n", current->value );
}

/*
  Adds val to the end of the list provided.
*/
void addToList(List * list, int val){
  if(list->length == 0){
    list->first = malloc(sizeof *list->first);
    list->first->value = val;
    list->first->next = NULL;

  }else{
    Cell * current = list->first;
    while(current->next != NULL){
      current = current->next;
    }
    current->next = malloc(sizeof *current->next);
    current = current->next;
    current->next = NULL;
    current->value = val;
  }
  list->length++;
}

/*
  Copies a list element by element. Useful for when you need to modify
  a list without destroying the original.
  Allocates dynamic memory, so remember to free it when you're done.
*/
List * copyList(List * list){
  List * copy = malloc(sizeof * copy);
  copy->length = 0;


  //if list has stuff, copy them
  if(list->length > 0){
    Cell * current = list->first;
    //copy->first = malloc(sizeof *copy->first);
    while(current->next != NULL){
      addToList(copy, current->value);
      current = current->next;
    }
    addToList(copy, current->value);
    return copy;
  }else{
    return copy;
  }
}

/*
  Creates a deep copy of an array of n lists.
*/
List ** copyListArray(List *array[], int n){
  List **result = malloc(n * sizeof *result);
  for(int i = 0; i < n; i++){
    //printf("copying %d\n", i);
    //result[i] = malloc(sizeof array[i]);
    //printf("calloced\n");
    result[i] = copyList(array[i]);
  }
  return result;
}


/*
  Returns the nth element of the list.
*/
int getListIndex(List * list, int n){
  Cell * current = list->first;
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
Cell * getListCellIndex(List * list, int n){
  Cell * current = list->first;
  int i = 0;
  while(i != n){
    current = current->next;
    i++;
  }
  return current;
}


/*
  This takes a permutation list perm and applies it to list,
  generating a new list that is then returned.
  For example
  list = [3 4 1 2 0]
  perm = [2 1 0 1 0]

  return = [1 4 3 0 2]
*/

List * permuteList(List * list, List * perm){
  //printf("entering permute list ------------------\n");

  List * result = malloc(sizeof(result));
  result->length = 0;
  Cell * current = perm->first;
  int start = getMallInfo();
  List * copy = copyList(list);
  //printf("after copy %d\n", getMallInfo() - start);
  //printList(copy);
  //printf(" : permed with : ");
  //printList(perm);
  //printf("perm : \n");
  //printList(perm);
  //while perm still has a next element
  while(current->next != NULL){
    //printList(copy);
    int address = current->value;
    //add the element at position address from copy into result
    addToList(result, getListIndex(copy, address));
    //printf("after adding to result %d\n", getMallInfo() - start);
    //remove it from copy
    if(address == 0){
      if(copy->first->next != NULL){
        Cell * toFree = copy->first;
        copy->first = copy->first->next;
        copy->length--;
        free(toFree);
        //printf("after freeing first %d\n", getMallInfo() - start);
      }else{
        free(copy->first);
        free(copy);
        //printf("after freeing first and last %d\n", getMallInfo() - start);
      }
    }else if(address == copy->length - 1){
      free(getListCellIndex(copy, address));
      //printf("after freeing last %d\n", getMallInfo() - start);
      getListCellIndex(copy, address-1)->next=NULL;
      copy->length--;
    }else{
      Cell * temp = getListCellIndex(copy, address);
      getListCellIndex(copy, address-1)->next = getListCellIndex(copy, address+1);
      free(temp);
      //printf("after freeing middle %d\n", getMallInfo() - start);
      copy->length--;
    }
    //go to the next element
    //printf("dunno\n");
    current = current->next;
  }
  //printList(copy);
  //we hit the last element of perm
  addToList(result, getListIndex(copy, current->value));
  //printf("after adding last %d\n", getMallInfo() - start);
  //printf("copy length %d\n", copy->length);
  //printf("copy is empty %d\n", copy->first->next == NULL);
  freeList(copy);
  //printf("after the end %d\n", getMallInfo() - start);
  //the first element is meaningless, so free and skip it
  return result;
}
