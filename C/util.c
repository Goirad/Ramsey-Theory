#include "defs.h"


int memUsage = 0;
int memFreed = 0;
bool debug = false;

/*
  Used to monitor memory usage. Dividing by 1000 instead of 1024
  just to be able to read bytes at a glance.
*/
void dumpMallinfo(){
  struct mallinfo m = mallinfo();
  printf("used kbytes = %.3f\n", m.uordblks/1000.0);
}
int getMallInfo(){
  struct mallinfo m = mallinfo();
  return m.uordblks;
}
/*
  Creates and returns a list of 2^n graphs, where n is the number of
  vertices in g. Each graph has a different combination of edge colors,
  which is why there are so many of them.
*/
GraphList * getNextSize(Graph * g){
  int i, j;
  int n = g->n;
  GraphList * out = newGraphList(pow(2, n));
  for(i = 0; i < pow(2, n); i++){
    Graph * current = copyGraph(g);
    //edges has to be expanded a little. n(n-1)/2 + n = n(n+1)/2
    //Color * t = current->edges;
    current->edges = realloc(current->edges, (n*(n+1)/2) * sizeof *(current->edges));
    //free(t);
    current->n += 1;
    int k = i;
    //I use i as a combination of 2 colors.
    //ie 101 would mean the first edge is green, then red, then green again
    //this means i can use bitshifting and anding to decode it quickly
    for(j = n*(n-1)/2; j < n*(n+1)/2; j++){
      *(current->edges + j) = (k&1) + 1;
      k = k>>1;
    }
    setGraph(out, current, i);
  }
  return out;
}

/*
  Checks whether a graph has a K3 subgraph that is all color c
  Yes three nested for loops screams inefficient but hey at least
  it works. And it turns out it's not even our biggest bottleneck by a lot
*/
bool hasK3(Graph * g, Color c){
  int n = g->n;
  int * numEdges = getCharList(g, c);
  //the checks in between help us avoid checking impossible combos
  for(int i = 0; i < n - 2; i++){
    if( *(numEdges+i) >= 2) {
      for(int j = i + 1; j < n - 1; j++){
        if( *(numEdges+j) >= 2) {
          for(int k = j + 1; k < n; k++){
            if(
              *(numEdges+k) >= 2  &&
              getEdgeColor(g, i, j) == c &&
              getEdgeColor(g, j, k) == c &&
              getEdgeColor(g, i, k) == c
            ){
              free(numEdges);
              return true;
            }
          }
        }
      }
    }
  }
  free(numEdges);
  return false;
}

/*
  Pretty much the same as has K3. We could probably even write a recursive one
  to check for hasKn, but that will have to be investigated later.
*/
bool hasK4(Graph * g, Color c){
  int n = g->n;
  int * numEdges = getCharList(g, c);

  for(int h = 0; h < n - 3; h++) {
    if( *(numEdges+h) >= 3) {
      for(int i = h + 1; i < n - 2; i++){
        if( *(numEdges+i) >= 3) {
          for(int j = i + 1; j < n - 1; j++){
            if( *(numEdges+j) >= 3) {
              for(int k = j + 1; k < n; k++){
                if(
                  *(numEdges+k) >= 3  &&
                  getEdgeColor(g, h, i) == c &&
                  getEdgeColor(g, h, j) == c &&
                  getEdgeColor(g, h, k) == c &&
                  getEdgeColor(g, i, j) == c &&
                  getEdgeColor(g, i, k) == c &&
                  getEdgeColor(g, j, k) == c
                ){
                  free(numEdges);
                  return true;
                }
              }
            }
          }
        }
      }
    }
  }
  free(numEdges);
  return false;
}
/*
  Pretty much the same as has K3. We could probably even write a recursive one
  to check for hasKn, but that will have to be investigated later.
*/
bool hasK5(Graph * g, Color c){
  int n = g->n;
  int * numEdges = getCharList(g, c);

  for(int h = 0; h < n - 4; h++) {
    if( *(numEdges+h) >= 4) {
      for(int i = h + 1; i < n - 3; i++){
        if( *(numEdges+i) >= 4) {
          for(int j = i + 1; j < n - 2; j++){
            if( *(numEdges+j) >= 4) {
              for(int k = j + 1; k < n - 1 ; k++){
                if(*(numEdges + k) >= 4){
                  for(int l = k + 1; l < n; l++){
                    if(
                      *(numEdges+l) >= 4  &&
                      getEdgeColor(g, h, i) == c &&
                      getEdgeColor(g, h, j) == c &&
                      getEdgeColor(g, h, k) == c &&
                      getEdgeColor(g, h, l) == c &&

                      getEdgeColor(g, i, j) == c &&
                      getEdgeColor(g, i, k) == c &&
                      getEdgeColor(g, i, l) == c &&

                      getEdgeColor(g, j, k) == c &&
                      getEdgeColor(g, j, l) == c &&

                      getEdgeColor(g, k, l) == c
                    ){
                      free(numEdges);
                      return true;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  free(numEdges);
  return false;
}

/*
  Simple iterative factorial function
*/
int fact(int n){
  int t = 1;
  for(int  i = 2; i <= n; i++){
    t *= i;
  }
  return t;
}

/*
  Now this is cool. It turns out there is a natural way to iterate over permutations
  by taking every number from 0 to n! in decimal and converting it to base !
  Here is the link for example and explanation:
  https://en.wikipedia.org/wiki/Factorial_number_system

  I have this return a Cell when really it could be done in an array much faster.

  n is the number to convert
  dig is the number of digits the answer should have
*/
int * decToFact(int n, int dig){
  int temp[dig];
  int num = n;
  int * ans = malloc(dig * sizeof *ans);
  for(int i = 0; i < dig; i++){
    temp[i] = num % (i+1);
    num /= (i+1);
  }
  for (int i = 0; i < dig; i++) {
    ans[i] = temp[dig-1-i];
  }
  return ans;
}


/*
  Takes an array of lists and collapses it into a single list with n elements
  in the order they appear in the array.
  []
  [2 0]
  [3]
  [1]
  collapses to [2 0 3 1]
*/
int * collapseVerts(List *verts[], int n){
  int * result = malloc(n * sizeof(int));
  int found = 0;
  for(int i = 0; i < n; i++){
    Cell * current = verts[i]->first;
    if(verts[i]->length > 0){
      while(current->next != NULL){
        result[found] = current->value;
        found++;
        current = current->next;
      }
      result[found] = current->value;
      found++;
    }
  }
  return result;
}

/*
  Now we get to the meat and potatoes of the algorithm. This function recursively
  checks whether 2 graphs are color isomorphic by checking every valid permutation
  of g's vertices.
  Base Case:
    We have permuted all vertices. We collapse down those vertices into an array
    for faster access. Then we check that every edge between the two arrays
    matches. If they do, we return true and escape all the way to the first
    call of recIsoCheck. Otherwise we return false and continue to check other permutations.
  Recursive Step:
    Iterate through all permutations of the vertices of g that have depth red edges
    each time call recIsoCheck one layer deeper.
    If it returns true, clean up and return true.
    If false, check next permutation.
*/
bool recIsoCheck(List *vertsG[], List *vertsH[], int depth, Graph * g, Graph * h){
  //printf("depth = %d / %d\n", depth, g->n-1);
  if(depth >= g->n-1){
    int * gList = collapseVerts(vertsG, g->n);
    int * hList = collapseVerts(vertsH, h->n);
    //printf("collapsed\n");
    for(int i = 0; i < g->n-1; i++){
      for(int j = i + 1; j < g->n; j++){
        if(getEdgeColor(g, gList[i], gList[j]) != getEdgeColor(h, hList[i], hList[j])){
          free(gList);
          free(hList);
          return false;
        }
      }
    }
    free(gList);
    free(hList);

    return true;
  }else{
    if(vertsG[depth]->length > 0){
      for(int i = 0; i < fact(vertsG[depth]->length); i++){

        int * perm = decToFact(i, vertsG[depth]->length);


        List * permVertsG = permuteList(vertsG[depth], perm);

        free(perm);
        List ** copy = copyListArray(vertsG, g->n);
        //printf("copied list\n");
        freeList(copy[depth]);
        copy[depth] = permVertsG;
        //printf("entering recursively\n");
        bool ans = recIsoCheck(copy, vertsH, depth + 1, g, h);
        //printf("leaving recursively\n");
        freeListArray(copy, g->n);
        if(ans) return true;
      }
      return false;
    }else{
      return recIsoCheck(vertsG, vertsH, depth + 1, g, h);
    }
  }
}

/*
  Simple function used by qsort
*/
int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

/*
  Returns true if the two graphs are color isomorphic and false otherwise.
  This function sets up the infrastructure to call recIsoCheck.
  It checks first that the two graphs have compatible characteristic lists
  as described in our presentation.
  Then it splits the vertices into groups with the same number of red edges.
  Then it calls recIsoCheck

  Can be optimized quite a bit
*/
bool isColorIso(Graph * g, Graph * h){
  //printf("at start of isColorIso %d\n", getMallInfo());

  int * charListGSorted = getCharList(g, RED);
  int * charListHSorted = getCharList(h, RED);

  int n = g->n;
  qsort(charListGSorted,n,sizeof(int),cmpfunc);
  qsort(charListHSorted,n,sizeof(int),cmpfunc);


  int result = memcmp(charListGSorted, charListHSorted, n*sizeof(int));
  free(charListGSorted);
  free(charListHSorted);

  if(result == 0){
    int * charListG = getCharList(g, RED);
    int * charListH = getCharList(h, RED);
    List **vertsG = malloc(n * sizeof *vertsG);
    List **vertsH = malloc(n * sizeof *vertsH);
    //printf("entering if\n");
    for(int i = 0; i < n; i++){
      vertsG[i] = malloc(sizeof *vertsG[i]);
      vertsH[i] = malloc(sizeof *vertsH[i]);
      vertsG[i]->length = 0;
      vertsH[i]->length = 0;
    }
    for(int i = 0 ; i < n; i++){
      addToList(vertsG[charListG[i]], i);
      addToList(vertsH[charListH[i]], i);
    }
    //printf("entering\n");
    //printf("before : ");
    //dumpMallinfo();
    bool ans = recIsoCheck(vertsG, vertsH, 0, g, h);
    //printf("after  : ");
    //dumpMallinfo();
    //printf("exiting\n");
    freeListArray(vertsG, n);
    freeListArray(vertsH, n);

    ///free(vertsG);
    //free(vertsH);
    free(charListH);
    free(charListG);
    //printf("at end of isColorIso %d\n", getMallInfo());
    return ans;
  }else{
    //printf("at end of isColorIso %d\n", getMallInfo());
    return false;
  }
}

/*
  Accepts a graphList and modifies it so that it contains only one
  representative of each color isomorphism class remains. In other words, no
  duplicates. It also filters out any invalid graphs, which are
  graphs that have either a red K3 or a green K4.
*/
void clean(GraphList * gL){
    int numGraphs = gL->size;
    int foundGraphs = 0;


    int temp = numGraphs;
    //Now we have to check there are no red K3s and no green K4s
    //Honestly its probably more efficient to check this first,
    //should be tested
    for(int i = 0; i < numGraphs; i++){
      Graph * current = getGraph(gL, i);
      if(!current->isNull){
        bool K3 = hasK4(current, RED);
        bool K4 = hasK4(current, GREEN);
        if(K3 | K4){
          current->isNull = true;
          temp--;
        }
      }
      if(gL->size > 1 && i%1 == 0){
        printf("%3.1f%% done checking for Kns... %d / %d", (i*100.0/gL->size), i, temp);
        printf("\n\033[F\033[J");
      }
    }
    //has to check every graph with every other graph
    //but the first one invalidates a lot of others, so not quite n^2
    for(int i = 0; i < numGraphs; i++){
      Graph * current = getGraph(gL, i);
      if(!current->isNull){
        for(int j = numGraphs - 1; j > i; j--){
          Graph * other = getGraph(gL, j);
          if(!other->isNull){
           if(isColorIso(current, other)){
              other->isNull = true;
              temp--;
            }
          }
        }
      }
      //just displays an updating percent because this tends to take the longest
      //time and it's nice to see how it's coming along
      if(gL->size > 1 && i%1 == 0){
        printf("%3.1f%% done checking color isos... %d / %d", (i*100.0/gL->size), i, temp);
        printf("\n\033[F\033[J");
      }
    }
    GraphList * cleanedGraphs = newGraphList(numGraphs);

    for(int i = 0; i < numGraphs; i++){
      Graph * current = getGraph(gL, i);
      if(!current->isNull){
        *(*cleanedGraphs->graphs + foundGraphs) = *(*gL->graphs + i);
        foundGraphs++;
      }
    }


    for(int i = 0; i < foundGraphs; i++){
      if((*(*gL->graphs + i))->isNull) destroyGraph(*(*gL->graphs + i));
      *(*gL->graphs + i) = *(*cleanedGraphs->graphs + i);

    }
    destroyGraphList(cleanedGraphs);
    shrinkGraphList(gL, foundGraphs);

 }

/*
  Implements the bulk of the algorithm as described in the project proposal.
  Returns the smallest int n such that there are no valid colorings of Kn.
*/
int run(){
  int tiers = 7;
  //creates an array of graphlists
  GraphList ** graphTiers = malloc(tiers * sizeof(*graphTiers));

  printf("Generating First GraphList\n");
  *graphTiers = newGraphList(1);
  printf("Generating K1\n");
  **((*graphTiers)->graphs) = createKn(1);

  printf("starting mem usage:\n");
  dumpMallinfo;
  for(int i = 1; i < tiers; i++){
    printf("Starting row %d\n", i + 1);
    *(graphTiers + i) = newGraphList(0);
    printf("Before generating next size: ");
    dumpMallinfo();
    int num = (*(graphTiers + i - 1))->size;
    for(int j = 0; j < num; j++){
      printf("Generating the next size: %3d%% done... %.3f KB %d graphs", (int)(j*100/num), getMallInfo()/1000.0, (*(graphTiers + i))->size);
      //printf("\n\033[F\033[J");
      GraphList * gL = getNextSize(getGraph(*(graphTiers + i - 1), j));
      //it helps to clean before merging to keep the sizes low for
      //final row cleaning
      //printf("done generating next size\n");
      printf("\n");
      clean(gL);
      printf("\033[F\033[J");
      //printf("done cleaning\n");
      mergeGraphLists(*(graphTiers + i), gL);
    }
    printf("After generating next size: ");
    dumpMallinfo();

    printf("%d has %d graphs raw\n", i+1, (*(graphTiers + i))->size);
    printf("Cleaning this set up...\n");

    clean(*(graphTiers + i));

    printf("%d has %d graphs cleaned\n",i+1, (*(graphTiers + i))->size);

    printf("-------------------\n%d : %d\n", i + 1, (*(graphTiers + i))->size);
  }
}



int main(){
  time_t before;
  time_t after;
  before = time(NULL);
  run();
  after = time(NULL);
  printf("That took %d s\n", after - before);
  return 0;
}
