#include "defs.h"



intList * newIntList(int n){
  intList * list = malloc(sizeof * list);
  list->length = n;
  list->size = n;
  if(n > 0) {
	  list->values = malloc(n * sizeof * list->values);
	}
	return list;
}

char * getIntListStrNorm(intList * list){

  intList * c = copyIntList(list);
  int k = 0;
  for(int i = 0; i < list->length; i++){
    int x = getIntListIndex(c, i);
    if(x > k){
      for(int j = 0; j < list->length; j++){
        if(getIntListIndex(c, j) == x){
          setIntListIndex(c, j, k);
        }
      }
      k++;
    }
  }

  char * ans = getIntListStr(c);
  freeIntList(c);
  return ans;
}

int intListMin(intList * list){
  int x = 1000;
  for(int i = 0; i < list->length; i++){
    if (getIntListIndex(list, i) < x) x = getIntListIndex(list, i);
  }
  return x;
}
int intListMinIndex(intList * list){
  int x = 1000;
  int index = -1;
  for(int i = 0; i < list->length; i++){
    if (getIntListIndex(list, i) < x) {
      x = getIntListIndex(list, i);
      index = i;
    }
  }
  return index;
}

char * getIntListStr(intList * list){
  char * ans = malloc(200);
  strcat(ans, "[");
  char * buf = malloc(200);

  for(int i = 0; i < list->length - 1; i++){
    sprintf(buf, "%d, ", getIntListIndex(list, i));
    strcat(ans, buf);
  }
  sprintf(buf, "%d]", getIntListIndex(list, list->length - 1));
  strcat(ans, buf);
  return ans;
}

intList2D * newIntList2D(int n){
	intList2D * list = malloc(sizeof * list);
	list->length = n;
	list->arrays = malloc(n * sizeof * list->arrays);
	return list;

}
/*
  Frees a list and all cells in it
*/
void freeIntList(intList * list){
  if(list->length > 0){
    free(list->values);
  }
  //printf("freed current\n");
  free(list);
  //printf("freed list\n");
}

/*
  Frees an array of lists with n sublists
*/
void freeIntList2D(intList2D * listArray){
  for(int i = 0; i < listArray->length ; i++){
    freeIntList(listArray->arrays[i]);
  }
	free(listArray->arrays);
  free(listArray);
}

/*
  Prints a list in a single line, with values separated by spaces.
  Useful for debugging.
*/
void printIntList(intList * list){
  printf("[");
	if(list->length > 1){
	  for(int i = 0; i < list->length - 1; i++){
	    printf("%d ", list->values[i]);
	  }
	  printf("%d]\n", list->values[list->length - 1]);
	}else if(list->length == 1){
		printf("%d]\n", list->values[0]);
	}else{
		printf("]\n");
	}
}

/*
  Adds val to the end of the list provided.
*/

void addToIntListME(intList * list, int val, int growthRate){
  if(list->length == 0){
    list->values = malloc(growthRate *sizeof *list->values);
    list->values[0] = val;
    list->length++;
    list->size = growthRate;
  }else if(list->length >= list->size){
    list->values = realloc(list->values, (list->length + growthRate) * sizeof *list->values);
    list->values[list->length] = val;
    list->length++;
    list->size += growthRate;
  }else{
    list->values[list->length] = val;
    list->length++;
  }
}

void addToIntList(intList * list, int val){
  if(list->length == 0){
    list->values = malloc( sizeof *list->values);
    list->values[0] = val;
    list->length++;
  }else{
    list->values = realloc(list->values, (list->length + 1) * sizeof *list->values);
    list->values[list->length] = val;
    list->length++;
  }
}

/*
  Copies a list element by element. Useful for when you need to modify
  a list without destroying the original.
  Allocates dynamic memory, so remember to free it when you're done.
*/
intList * copyIntList(intList * list){
  intList * copy = malloc(sizeof * copy);
  //if list has stuff, copy them
  if(list->length > 0){
		copy->length = list->length;
    copy->size = list->size;
    copy->values = malloc(copy->size * sizeof * copy->values);
    memcpy(copy->values, list->values, list->size * sizeof * list->values);
    return copy;
  }else{
    copy->length = 0;
    copy->size = 0;
    return copy;
  }
}

void copyIntListTo(intList * src, intList * dest){
  dest->length = src->length;
  for(int i = 0; i < dest->length; i++){
    setIntListIndex(dest, i, getIntListIndex(src, i));
  }
}

/*
  Creates a deep copy of an array of n lists.
*/
intList2D * copyIntList2D(intList2D * array){
  intList2D * result = newIntList2D(array->length);
	result->length = array->length;
  for(int i = 0; i < array->length; i++){
    result->arrays[i] = copyIntList(array->arrays[i]);
  }
  return result;
}



/*
  Returns the nth element of the list.
*/
int getIntListIndex(intList * list, int n){
	if(n > list->length){
		printf("Out of bounds error: %d / %d\n", n, list->length);
	}else{
  	return list->values[n];
	}
}
void setIntListIndex(intList * list, int index, int n){
	//printf("setting %d out of %d\n", index, list->length);
  list->values[index] = n;
}


void setNthNonNegValue(intList * list, int n, int value){
  int found = -1;
  for(int i = 0; i < list->length; i++){

    if(getIntListIndex(list, i) >= 0) found++;
    if(found == n) {
      setIntListIndex(list, i, value);
      return;
    }
  }
}

int getNthNonNegValue(intList * list, int n){
  int found = -1;
  for(int i = 0; i < list->length; i++){

    if(getIntListIndex(list, i) >= 0) found++;
    if(found == n) {
      return getIntListIndex(list, i);
    }
  }
  return -1;
}

/*
  This takes a permutation list perm and applies it to list,
  generating a new list that is then returned.
  For example
  list = [3 4 1 2 0]
  perm = [2 1 0 1 0]

  return = [1 4 3 0 2]
*/
void permuteList(intList * src, intList * dest, intList * perm, intList * permScratch){
  permScratch->length = src->length;

  //if list has stuff, copy them
  //printf("about to copy\n");
  if(src->length > 0){
		permScratch->length = src->length;
    memcpy(permScratch->values, src->values, src->length * sizeof * src->values);
  }

  dest->length = src->length;
  for(int i = 0; i < src->length; i++){
    int address = getIntListIndex(perm, i);
    setIntListIndex(dest, i, getNthNonNegValue(permScratch, address));
    setNthNonNegValue(permScratch, address, -1);
  }
}
