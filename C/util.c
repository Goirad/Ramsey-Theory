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
    current->charList = getCharList(current, RED);
    current->charListSorted = getCharList(current, RED);
    qsort(current->charListSorted->values,n + 1,sizeof(*current->charListSorted->values),cmpfunc);
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
  intList * numEdges = getCharList(g, c);
  //the checks in between help us avoid checking impossible combos
  for(int i = 0; i < n - 2; i++){
    if( getIntListIndex(numEdges, i) >= 2) {
      for(int j = i + 1; j < n - 1; j++){
        if( getIntListIndex(numEdges, j) >= 2) {
          for(int k = j + 1; k < n; k++){
            if(
              getIntListIndex(numEdges, k) >= 2  &&
              getEdgeColor(g, i, j) == c &&
              getEdgeColor(g, j, k) == c &&
              getEdgeColor(g, i, k) == c
            ){
              freeIntList(numEdges);
              return true;
            }
          }
        }
      }
    }
  }
  freeIntList(numEdges);
  return false;
}

/*
  Pretty much the same as has K3. We could probably even write a recursive one
  to check for hasKn, but that will have to be investigated later.
*/
bool hasK4(Graph * g, Color c){
  int n = g->n;
  intList * numEdges = getCharList(g, c);

  for(int h = 0; h < n - 3; h++) {
    if(getIntListIndex(numEdges, h) >= 3) {
      for(int i = h + 1; i < n - 2; i++){
        if(getIntListIndex(numEdges, i) >= 3) {
          for(int j = i + 1; j < n - 1; j++){
            if(getIntListIndex(numEdges, j) >= 3){
              for(int k = j + 1; k < n; k++){
                if(
                  getIntListIndex(numEdges, k) >= 3  &&
                  getEdgeColor(g, h, i) == c &&
                  getEdgeColor(g, h, j) == c &&
                  getEdgeColor(g, h, k) == c &&
                  getEdgeColor(g, i, j) == c &&
                  getEdgeColor(g, i, k) == c &&
                  getEdgeColor(g, j, k) == c
                ){
                  freeIntList(numEdges);
                  return true;
                }
              }
            }
          }
        }
      }
    }
  }
  freeIntList(numEdges);
  return false;
}
/*
  Pretty much the same as has K3. We could probably even write a recursive one
  to check for hasKn, but that will have to be investigated later.
*/
bool hasK5(Graph * g, Color c){
  int n = g->n;
  intList * numEdges = getCharList(g, c);
	//int tested = 0;
  for(int h = 0; h < n - 4; h++) {
    if( getIntListIndex(numEdges, h) >= 4) {
      for(int i = h + 1; i < n - 3; i++){
        if( getIntListIndex(numEdges, i) >= 4) {
          for(int j = i + 1; j < n - 2; j++){
            if( getIntListIndex(numEdges, j) >= 4) {
              for(int k = j + 1; k < n - 1 ; k++){
                if(getIntListIndex(numEdges, k) >= 4){
                  for(int l = k + 1; l < n; l++){
										//tested++;
                    if(
                      getIntListIndex(numEdges, l) >= 4  &&
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
											//if (n > 10) printf("%d tested\n", tested);
                      freeIntList(numEdges);
                      return true;
                    }
										//if (n > 10 && tested > 1000) printf("%d tested\n", tested);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  freeIntList(numEdges);
	//if (n > 10) printf("%d tested\n", tested);
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
intList * decToFact(int n, int dig){
  int temp[dig];
  int num = n;
  intList * ans = newIntList(dig);
  for(int i = 0; i < dig; i++){
    temp[i] = num % (i+1);
    num /= (i+1);
  }
  for (int i = 0; i < dig; i++) {
    setIntListIndex(ans, dig-1-i, temp[i]);
  }
  return ans;
}

void decToFact1(intList * dest, int n, int dig){
  int temp[dig];
  int num = n;
  dest->length = dig;
  for(int i = 0; i < dig; i++){
    temp[i] = num % (i+1);
    num /= (i+1);
  }
  for (int i = 0; i < dig; i++) {
    setIntListIndex(dest, dig-1-i, temp[i]);
  }
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
intList * collapseVerts(intList2D * verts, int n){
  intList * result = newIntList(n);
  int found = 0;
	//printf("collapsing verts\n");
  for(int i = 0; i < n; i++){
		//printIntList(verts->arrays[i]);
    if(verts->arrays[i]->length > 0){

      for(int j = 0; j < verts->arrays[i]->length; j++){
        setIntListIndex(result, found, getIntListIndex(verts->arrays[i], j));
        found++;
      }
    }
  }
	//printf("result = ");
	//printIntList(result);
  return result;
}


void collapseVerts1(intList2D * verts, intList * dest, int n){
  int found = 0;
	//printf("collapsing verts\n");
  //dest->length = 0;
  for(int i = 0; i < n; i++){
		//printIntList(verts->arrays[i]);
    if(verts->arrays[i]->length > 0){

      for(int j = 0; j < verts->arrays[i]->length; j++){
        //setIntListIndex(result, found, )

        setIntListIndex(dest, found, getIntListIndex(verts->arrays[i], j));
        found++;
      }
    }
  }
	//printf("result = ");
	//printIntList(result);
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

bool recIsoCheck(intList2D * vertsG, intList2D * vertsH, int depth, Graph * g, Graph * h){
  //printf("depth = %d / %d\n", depth, g->n-1);
  if (h->isNull) return true;
  if(depth >= g->n-1){
    intList * gList = collapseVerts(vertsG, g->n);
    intList * hList = collapseVerts(vertsH, h->n);
    //printf("collapsed\n");
    for(int i = 0; i < g->n-1; i++){
      for(int j = i + 1; j < g->n; j++){
				//printGraph(g);
				//printIntList(gList);
        if(getEdgeColor(g, getIntListIndex(gList, i), getIntListIndex(gList, j))
          != getEdgeColor(h, getIntListIndex(hList, i), getIntListIndex(hList, j))){
          freeIntList(gList);
          freeIntList(hList);
          return false;
        }
      }
    }
    freeIntList(gList);
    freeIntList(hList);

    return true;
  }else{
    if(vertsG->arrays[depth]->length > 0){
      for(int i = 0; i < fact(vertsG->arrays[depth]->length); i++){

        intList * perm = decToFact(i, vertsG->arrays[depth]->length);


        intList * permVertsG = permuteList(vertsG->arrays[depth], perm);

        freeIntList(perm);
        intList2D * copy = copyIntList2D(vertsG);
        //printf("copied list\n");
        freeIntList(copy->arrays[depth]);
        copy->arrays[depth] = permVertsG;
				//printIntList(permVertsG);
        //printf("entering recursively\n");
				//printIntList2D(copy);
        bool ans = recIsoCheck(copy, vertsH, depth + 1, g, h);
        //printf("leaving recursively\n");
        freeIntList2D(copy);
        if(ans) return true;
      }
      return false;
    }else{
      return recIsoCheck(vertsG, vertsH, depth + 1, g, h);
    }
  }
}



bool recIsoCheck1(intList2D * vertsG, intList2D * vertsH, int depth, Graph * g, Graph * h, isoCheckMemBlock * memBlock){
  //printf("depth = %d / %d\n", depth, g->n-1);
  //if (h->isNull) return true;
  if(depth >= g->n-1){
    //printf("------\n");
    //for(int i = 0; i < g->n; i++){
      //printIntList(vertsG->arrays[i]);
    //}

    collapseVerts1(vertsG, memBlock->collapseVertsG, g->n);
    //printIntList(memBlock->collapseVertsG);
    collapseVerts1(vertsH, memBlock->collapseVertsH, h->n);
    //printf("collapsed\n");
    for(int i = 0; i < g->n-1; i++){
      for(int j = i + 1; j < g->n; j++){
				//printGraph(g);
				//printIntList(gList);

        if(getEdgeColor(g, getIntListIndex(memBlock->collapseVertsG, i), getIntListIndex(memBlock->collapseVertsG, j))
          != getEdgeColor(h, getIntListIndex(memBlock->collapseVertsH, i), getIntListIndex(memBlock->collapseVertsH, j))){
          //freeIntList(gList);
          //freeIntList(hList);
          return false;
        }
      }
    }
    //freeIntList(gList);
    //freeIntList(hList);

    return true;
  }else{
    //copyIntList1(memBlock->origVertsG->arrays[depth], vertsG->arrays[depth]);
    if(vertsG->arrays[depth]->length > 0){
      for(int i = 0; i < fact(vertsG->arrays[depth]->length); i++){
        //printf("i = %d\n", i);
        //printf("----\n");
        //memBlock->perm->length = memBlock->origVertsG->length;
        decToFact1(memBlock->perm, i, memBlock->origVertsG->arrays[depth]->length);

        //printIntList(memBlock->perm);
        //intList * permVertsG = permuteList(vertsG->arrays[depth], perm);
        //printIntList(memBlock->origVertsG->arrays[depth]);
        permuteList1(memBlock->origVertsG->arrays[depth], memBlock->vertsG->arrays[depth], memBlock->perm, memBlock->permScratch);
        //printIntList(memBlock->vertsG->arrays[depth]);
        //freeIntList(perm);
        //intList2D * copy = copyIntList2D(vertsG);
        //printf("copied list\n");
        //freeIntList(copy->arrays[depth]);
        //copy->arrays[depth] = permVertsG;
				//printIntList(permVertsG);
        //printf("entering recursively\n");
				//printIntList2D(copy);
        bool ans = recIsoCheck1(vertsG, vertsH, depth + 1, g, h, memBlock);
        //printf("leaving recursively\n");
        //freeIntList2D(copy);
        if (ans) return true;
      }
      return false;
    }else{
      return recIsoCheck1(vertsG, vertsH, depth + 1, g, h, memBlock);
    }
  }
}

/*
  Simple function used by qsort
*/
int cmpfunc (const void * a, const void * b)
{
   return ( *(char*)a - *(char*)b );
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

  //intList * charListGSorted = getCharList(g, RED);
  //intList * charListHSorted = getCharList(h, RED);

  int n = g->n;
  //qsort(charListGSorted->values,n,sizeof(int),cmpfunc);
  //qsort(charListHSorted->values,n,sizeof(int),cmpfunc);


  int result = memcmp(g->charListSorted->values, h->charListSorted->values, n*sizeof(*h->charListSorted->values));

  if(result == 0){
    intList * charListG = g->charList;
    intList * charListH = h->charList;
    intList2D * vertsG = newIntList2D(n);
    intList2D * vertsH = newIntList2D(n);
    //printf("entering if\n");
    for(int i = 0; i < n; i++){
      vertsG->arrays[i] = newIntList(0);
      vertsH->arrays[i] = newIntList(0);
    }
		//printf("Char List = \n");
		//printIntList(charListG);
    for(int i = 0 ; i < n; i++){
      addToIntList(vertsG->arrays[charListG->values[i]], i);
      addToIntList(vertsH->arrays[charListH->values[i]], i);
    }
    //printf("entering\n");
    //printf("before : ");
    //dumpMallinfo();
    bool ans = recIsoCheck(vertsG, vertsH, 0, g, h);
    //printf("after  : ");
    //dumpMallinfo();
    //printf("exiting\n");
    freeIntList2D(vertsG);
    freeIntList2D(vertsH);

    //printf("at end of isColorIso %d\n", getMallInfo());
    return ans;
  }else{
    //printf("at end of isColorIso %d\n", getMallInfo());
    return false;
  }
}


bool isColorIso1(Graph * g, Graph * h, isoCheckMemBlock * memBlock){
  //printf("at start of isColorIso %d\n", getMallInfo());

  //intList * charListGSorted = getCharList(g, RED);
  //intList * charListHSorted = getCharList(h, RED);

  int n = g->n;
  //qsort(charListGSorted->values,n,sizeof(int),cmpfunc);
  //qsort(charListHSorted->values,n,sizeof(int),cmpfunc);


  int result = memcmp(g->charListSorted->values, h->charListSorted->values, n*sizeof(*h->charListSorted->values));

  if(result == 0){
    //printf("match\n");
    intList * charListG = g->charList;
    intList * charListH = h->charList;
    //printf("entering if\n");
    /*for(int i = 0; i < n; i++){
      vertsG->arrays[i] = newIntList(0);
      vertsH->arrays[i] = newIntList(0);
    }*/
		//printf("Char List = \n");
		//printIntList(charListG);
    //printf("setting verts array lengths\n");
    for(int i = 0; i < n; i++){
      memBlock->vertsG->arrays[i]->length = 0;
      memBlock->vertsH->arrays[i]->length = 0;
      memBlock->origVertsG->arrays[i]->length = 0;
    }
    //printf("setting verts arrays\n");
    for(int i = 0 ; i < n; i++){
      setIntListIndex(memBlock->vertsG->arrays[charListG->values[i]], memBlock->vertsG->arrays[charListG->values[i]]->length, i);
      (memBlock->vertsG->arrays[charListG->values[i]]->length)++;
      setIntListIndex(memBlock->origVertsG->arrays[charListG->values[i]], memBlock->origVertsG->arrays[charListG->values[i]]->length, i);
      (memBlock->origVertsG->arrays[charListG->values[i]]->length)++;
      setIntListIndex(memBlock->vertsH->arrays[charListH->values[i]], memBlock->vertsH->arrays[charListH->values[i]]->length, i);
      (memBlock->vertsH->arrays[charListH->values[i]]->length)++;
      //addToIntList(vertsG->arrays[charListG->values[i]], i);
      //addToIntList(vertsH->arrays[charListH->values[i]], i);
    }
    /*
    printf("------\n");
    for(int i = 0; i < g->n; i++){
      printIntList(memBlock->vertsG->arrays[i]);
    }*/
    //printf("entering\n");
    //printf("before : ");
    //dumpMallinfo();
    //printf("entering rec\n");
    bool ans = recIsoCheck1(memBlock->vertsG, memBlock->vertsH, 0, g, h, memBlock);
    //printf("after  : ");
    //dumpMallinfo();
    //printf("exiting\n");
    //freeIntList2D(vertsG);
    //freeIntList2D(vertsH);

    //printf("at end of isColorIso %d\n", getMallInfo());
    return ans;
  }else{
    //printf("at end of isColorIso %d\n", getMallInfo());
    return false;
  }
}

void * MTPollingThread(void * args){
  MTPollingArgs * args1 = (MTPollingArgs *) args;
  Graph * current = args1->current;
  for(int j = args1->numGraphs - 1; j > args1->i; j--){
    Graph * other = getGraph(args1->gL, j);
    if (!other->isNull){
      if(isColorIso(current, other)){
        other->isNull = true;
        (*args1->temp)--;
      }
    }
  }
  free(args);
  (*args1->numActive)--;
}
/*
void * MTSlicesThread(void * args){
	MTSlicesArgs * args1 = (MTSlicesArgs *) args;
  int threadBase = args1->base + args1->mod;
  //while(k < args1->base + args1->blockSize && k < args1->numGraphs){
  for(int j = 0; j < args1->blockSize &&  threadBase + j < args1->numGraphs; j += args1->numThreads){
    Graph * current = getGraph(args1->gL, threadBase + j);
    if(!current->isNull){
    	for(int k = args1->numGraphs - 1; k > threadBase + j; k--){
  			Graph * other = getGraph(args1->gL, k);
  			if (!other->isNull){
  				if(isColorIso(current, other)){
            if(!other->isNull){
  					  other->isNull = true;
  					  (*args1->temp)--;
              (*args1->deleted)++;
            }
  				}
  			}
    	}
    }
    (*args1->counter)++;
  }

  free(args);
}
*/

void * MTSlicesThread(void * args){

	MTSlicesArgs * args1 = (MTSlicesArgs *) args;
  isoCheckMemBlock * memBlock = getIsoCheckMemBlock(getGraph(args1->gL, 0)->n);
  int threadBase = args1->base + args1->mod;
  //while(k < args1->base + args1->blockSize && k < args1->numGraphs){
  for(int j = 0; j < args1->blockSize &&  threadBase + j < args1->numGraphs; j += args1->numThreads){
    Graph * current = getGraph(args1->gL, threadBase + j);
    if(!current->isNull){
    	for(int k = args1->numGraphs - 1; k > threadBase + j; k--){
  			Graph * other = getGraph(args1->gL, k);
  			if (!other->isNull){
          //printf("checking iso\n");
  				if(isColorIso1(current, other, memBlock)){
            //if(!other->isNull){
  					  //other->isNull = true;
  					  (*args1->temp)--;
              (*args1->deleted)++;
            //}
  				}
  			}
    	}
    }
    (*(args1->counter))++;
  }

  free(args);
}

void cleanKns(GraphList * gL, int n, int m, int * tempGraphs, bool isMajor){
  int numGraphs = gL->size;
  for(int i = 0; i < numGraphs; i++){
    Graph * current = getGraph(gL, i);
    //printGraph(current);
    if(!current->isNull){
      //printf("checking K3\n");
      bool Kn;
      if (n == 3) {
        Kn = hasK3(current, RED);
      }else if(n == 4){
        Kn = hasK4(current, RED);
      }else if(n == 5){
        Kn = hasK5(current, RED);
      }


      if(Kn){
          current->isNull = true;
          (*tempGraphs)--;
      }else{
        //printf("checking K5\n");
        bool Km;
        if (m == 3) {
          Km = hasK3(current, GREEN);
        }else if(m == 4){
          Km = hasK4(current, GREEN);
        }else if(m == 5){
          Km = hasK5(current, GREEN);
        }
        if(Km){
          current->isNull = true;
          (*tempGraphs)--;
        }
      }
    }
    if(gL->size > 5000 && i%100 == 0){
      printf("%3.1f%% done checking for Kns... %d / %d", (i*100.0/gL->size), i, *tempGraphs);
      printf("\n\033[F\033[J");
    }
  }
  clearGraphList(gL);
  if (isMajor) dumpGraphList(gL, n, m);
}

int graphPPPComparator(const void * g, const void * h){
  Graph * g1 = *((Graph **) g);
  Graph * h1 = *((Graph **) h);
  //printf("hi\n");
  intList * gCharList = g1->charListSorted;
  intList * hCharList = h1->charListSorted;
  return memcmp(gCharList->values, hCharList->values, gCharList->length);
}


void sortGraphList(GraphList * gL){
  qsort(*gL->graphs, gL->size, sizeof(*(gL->graphs)), graphPPPComparator);
}
void cleanMTPolling(GraphList * gL, int n, int m, int maxThreads, bool isMajor){

    int originalSize = gL->size;
    int numGraphs = gL->size;
    int foundGraphs = 0;

    int tempGraphs = numGraphs;
    if(!isMajor) {
      cleanKns(gL, n, m, &tempGraphs, isMajor);
      numGraphs = gL->size;
    }

    //has to check every graph with every other graph
    //but the first one invalidates a lot of others, so not quite n^2
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000L;
    //nanosleep(&ts, NULL);

		int blockSize = 1000;

		int activeIndex = gL->activeIndex;
    int blockID = 0;
		int i = gL->activeIndex;

    while(i < numGraphs){
      int j = 0;
      int threads = 0;
      int numActive = 0;
      pthread_t pids[blockSize];
      //checks a block
      while (j < blockSize && i < numGraphs){
        if (isMajor && originalSize > 2000) nanosleep(&ts, NULL);
  			if (numActive < maxThreads){
  	      Graph * current = getGraph(gL, i);
  	      if(!current->isNull){
  					MTPollingArgs * args = malloc(sizeof * args);
  					args->current = current;
  					args->temp = &tempGraphs;
  					args->numGraphs = numGraphs;
  					args->gL = gL;
  					args->i = i;
  					args->numActive = &numActive;
  					numActive++;
  					pthread_create(&pids[threads], NULL, MTPollingThread, args);
  					threads++;
  				}
          i++;
          j++;

  			}
        if (isMajor) {
          printf("%3.1f%% done with block %d / %d", (100.0 * j) / blockSize, blockID, numGraphs / blockSize + 1);
          printf("\n\033[F\033[J");
        }
      }
      //printf("done with loop\n");
      for (int k = 0; k < blockSize && k < threads; k++){
        //printf("%d\n", k);
  			void * status;
  			pthread_join(pids[k], &status);
  		}
      //printf("done joining\n");
      int found = 0;
      blockID++;
      for(int l = activeIndex; l < activeIndex + blockSize && l < numGraphs ; l++){
        Graph * g = getGraph(gL, l);
        if (! g->isNull){
          found++;
        }
      }
      //printf("done finding %d\n", found);
      GraphList * temp = newGraphList(found);
      found = 0;
      for(int l = activeIndex; l < activeIndex + blockSize && l < numGraphs; l++){
        Graph * g = getGraph(gL, l);

        if (! g->isNull){

          setGraph(temp, g, found);
          found++;

        }
      }
      //  printf("done copying %d\n", found);
      //if (isMajor) dumpAppendGraphList(temp, n, m, false, blockID);
      /*for(int l = 0; l < blockSize && l < numGraphs; l++){
        Graph * g = getGraph(gL, l);
        g->isNull = true;
      }*/
      //printf("a\n");
      destroyGraphList(temp);
      //printf("b\n");
      clearGraphList(gL);
      numGraphs = gL->size;
      activeIndex +=  found;
      gL->activeIndex = activeIndex;
      i = activeIndex;
      //printf("c\n");
      if (isMajor) dumpGraphList(gL, n, m);
      //printf("d\n");
			if(gL->size > 100){
        //printf("\033[F\033[J");
        printf("%3.1f%% done checking color isos... %d / %d %d active", (100.0 * blockID / (originalSize * 1.0 / blockSize)), blockID, originalSize / blockSize + 1, numActive);
        printf("\n\033[F\033[J");
      }
    }

    clearGraphList(gL);
}

void experimentalSortedCleaner(GraphList * gL, int n, int m, bool isMajor){
  //printf("n = %d, m = %d\n", n, m);
  int originalSize = gL->size;
  int numGraphs = gL->size;
  int origin = gL->activeIndex;
  time_t then = time(0);
  //printf("starting Kn checking\n");
  int tempGraphs = numGraphs;
  if(!isMajor) {
    //printf("Set has %d raw\n", numGraphs);
    cleanKns(gL, n, m, &tempGraphs, isMajor);
    numGraphs = gL->size;
    //printf("Set has %d cleaned Kns\n", numGraphs);
  }

  //has to check every graph with every other graph
  //but the first one invalidates a lot of others, so not quite n^2
  //printf("Starting on color Isos...\n");
  int blockSize = 1000;

  int activeIndex = gL->activeIndex;
  int blockID = 0;

  //printf("Entering main loop %d\n", numGraphs);
  int deleted = 0;

  int i = gL->activeIndex;
  isoCheckMemBlock * memBlock = getIsoCheckMemBlock(getGraph(gL, 1)->n);
  while (i < numGraphs){
    Graph * current = getGraph(gL, i);
    int found1 = 1;

    while(i + found1 < numGraphs){
      Graph * other = getGraph(gL, i + found1);
      if(!memcmp(current->charListSorted->values, other->charListSorted->values, current->charListSorted->length)){
        found1 ++;
      }else{
        break;
      }
    }
    int found2 = 0;
    for (int j = 0; j < found1 - 1; j++){
      Graph * current = getGraph(gL, i + j);
      if (!current->isNull){
        found2 ++;
        for(int k = j + 1; k < found1; k++){
          Graph * other = getGraph(gL, i+k);
          if (!other->isNull){
            if(isColorIso1(current, other, memBlock)){
              other->isNull = true;
              tempGraphs--;
              deleted++;
            }
          }
        }
      }
      if(j % 10 == 0 && isMajor){
        time_t now = time(0);
        int delta = (now - then);
        printf("(%d/%d) %3.1f%% done with block %d, %d / %d or %3.1f%% total, averaging %3.6f g/s, total of %d gs over %02d:%02d:%02d\n",
        found2, found1, (100.0 * j) / found1, blockID, activeIndex, numGraphs, 100.0 * activeIndex/numGraphs, (deleted)*1.0/delta, deleted, delta/3600, (delta/60)%60, delta%60);
        printf("\033[F\033[J");
      }
    }

    //cleans up after the block is checked
    blockID++;
    int found = 0;
    for(int l = activeIndex; l < activeIndex + found1 && l < numGraphs ; l++){
      Graph * g = getGraph(gL, l);
      if (! g->isNull){
        found++;
      }
    }
    //printf("done finding %d\n", found);
    GraphList * temp = newGraphList(found);
    found = 0;
    for(int l = activeIndex; l < activeIndex + found1 && l < numGraphs; l++){
      Graph * g = getGraph(gL, l);
      if (! g->isNull){
        setGraph(temp, g, found);
        found++;
      }
    }

    destroyGraphList(temp);
    clearGraphList(gL);
    numGraphs = gL->size;
    activeIndex +=  found;
    gL->activeIndex = activeIndex;
    i = activeIndex;
    if (isMajor) dumpGraphList(gL, n, m);
  }
  clearGraphList(gL);
}

void cleanST(GraphList * gL, int n, int m, bool isMajor){
  //printf("n = %d, m = %d\n", n, m);
  int originalSize = gL->size;
  int numGraphs = gL->size;
  int origin = gL->activeIndex;
  time_t then = time(0);
  //printf("starting Kn checking\n");
  int tempGraphs = numGraphs;
  if(!isMajor) {
    //printf("Set has %d raw\n", numGraphs);
    cleanKns(gL, n, m, &tempGraphs, isMajor);
    numGraphs = gL->size;
    //printf("Set has %d cleaned Kns\n", numGraphs);
  }

  //has to check every graph with every other graph
  //but the first one invalidates a lot of others, so not quite n^2
  //printf("Starting on color Isos...\n");
  int blockSize = 5000;

  int activeIndex = gL->activeIndex;
  int blockID = 0;

  //printf("Entering main loop %d\n", numGraphs);
  int deleted = 0;

  int i = gL->activeIndex;
  isoCheckMemBlock * memBlock = getIsoCheckMemBlock(getGraph(gL, 0)->n);
  while (i < numGraphs){
    //checks a block
    for (int j = 0; j < blockSize && i + j < numGraphs; j++){
      Graph * current = getGraph(gL, i + j);
      if (!current->isNull){
        for(int k = numGraphs - 1; k > i + j; k--){
          Graph * other = getGraph(gL, k);
          if (!other->isNull){
            if(isColorIso1(current, other, memBlock)){
              if(!other->isNull){
                other->isNull = true;
                tempGraphs--;
                deleted++;
              }
            }
          }
        }
      }
      if(j % 10 == 0 && isMajor){
        time_t now = time(0);
        int delta = (now - then);
        printf("%3.1f%% done with block %d, %d / %d or %3.1f%% total, averaging %3.6f g/s, total of %d gs over %02d:%02d:%02d\n",
        (100.0 * j) / blockSize, blockID, activeIndex, numGraphs, 100.0 * activeIndex/numGraphs, (deleted)*1.0/delta, deleted, delta/3600, (delta/60)%60, delta%60);
        printf("\033[F\033[J");
      }
    }

    //cleans up after the block is checked
    blockID++;
    int found = 0;
    for(int l = activeIndex; l < activeIndex + blockSize && l < numGraphs ; l++){
      Graph * g = getGraph(gL, l);
      if (! g->isNull){
        found++;
      }
    }
    //printf("done finding %d\n", found);
    GraphList * temp = newGraphList(found);
    found = 0;
    for(int l = activeIndex; l < activeIndex + blockSize && l < numGraphs; l++){
      Graph * g = getGraph(gL, l);
      if (! g->isNull){
        setGraph(temp, g, found);
        found++;
      }
    }

    destroyGraphList(temp);
    clearGraphList(gL);
    numGraphs = gL->size;
    activeIndex +=  found;
    gL->activeIndex = activeIndex;
    i = activeIndex;
    //if (isMajor) dumpGraphList(gL, n, m);
  }
  clearGraphList(gL);
}

isoCheckMemBlock * getIsoCheckMemBlock(int numVerts){
  isoCheckMemBlock * block = malloc(sizeof * block);
  block->vertsG = newIntList2D(numVerts);
  block->origVertsG = newIntList2D(numVerts);
  block->vertsH = newIntList2D(numVerts);
  for(int i = 0; i < numVerts; i++){
    block->vertsG->arrays[i] = newIntList(numVerts);
    block->vertsH->arrays[i] = newIntList(numVerts);
    block->origVertsG->arrays[i] = newIntList(numVerts);
  }
  block->collapseVertsG = newIntList(numVerts);
  block->collapseVertsH = newIntList(numVerts);
  block->perm = newIntList(numVerts);
  block->permScratch = newIntList(numVerts);
  return block;
}

int sumArray(int * arr, int size){
  int total = 0;
  for(int i = 0; i < size; i++){
      total += arr[i];
  }
  return total;
}

void cleanMTSlices(GraphList * gL, int n, int m, int maxThreads, bool isMajor){
  //printf("n = %d, m = %d\n", n, m);
  int originalSize = gL->size;
  int numGraphs = gL->size;
  int origin = gL->activeIndex;
  time_t then = time(0);
  //printf("starting Kn checking\n");
  int tempGraphs = numGraphs;
  if(!isMajor) {
    //printf("Set has %d raw\n", numGraphs);
    cleanKns(gL, n, m, &tempGraphs, isMajor);
    numGraphs = gL->size;
    //printf("Set has %d cleaned Kns\n", numGraphs);
  }

  //has to check every graph with every other graph
  //but the first one invalidates a lot of others, so not quite n^2
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 100000000L;

  //printf("Starting on color Isos...\n");
  int blockSize = 5000;

  int activeIndex = gL->activeIndex;
  int blockID = 0;

  //printf("Entering main loop %d\n", numGraphs);
  int deleted = 0;

  int i = gL->activeIndex;
  while(i < numGraphs){
    //checks a block
    int j = 0;
    pthread_t pids[maxThreads];
    int progs[maxThreads];
    for (int t = 0; t < maxThreads; t++){
      MTSlicesArgs * args = malloc(sizeof * args);
      args->temp = &tempGraphs;
      args->numGraphs = numGraphs;
      args->gL = gL;
      args->base = activeIndex;
      args->blockSize = blockSize;
      args->mod = t;
      args->numThreads = maxThreads;
      progs[t] = 0;
      args->counter = &progs[t];
      args->deleted = &deleted;
      pthread_create(&pids[t], NULL, MTSlicesThread, args);

    }
    int tot = sumArray(progs, maxThreads);
    while(tot < blockSize && isMajor && tot + activeIndex < numGraphs){

      //printf("j = %d\n", j);
      time_t now = time(0);
      int delta = (now - then);
      printf("%3.1f%% done with block %d, %d / %d or %3.1f%% total, averaging %3.6f g/s (%3.2f / t), total of %d gs over %02d:%02d:%02d used %.3f KB\n",
      (100.0 * tot) / blockSize, blockID, activeIndex, numGraphs, 100.0 * activeIndex/numGraphs, (deleted)*1.0/delta, (deleted)*1.0/delta/maxThreads, deleted, delta/3600, (delta/60)%60, delta%60, getMallInfo()/1000);
      printf("\033[F\033[J");
      nanosleep(&ts, NULL);
      tot = sumArray(progs, maxThreads);
    }
    //printf("done with loop\n");
    for (int k = 0; k < maxThreads; k++){
      //printf("%d\n", k);
      void * status;
      pthread_join(pids[k], &status);
    }

    //cleans up after the block is checked
    blockID++;
    int found = 0;
    for(int l = activeIndex; l < activeIndex + blockSize && l < numGraphs ; l++){
      Graph * g = getGraph(gL, l);
      if (! g->isNull){
        found++;
      }
    }
    //printf("done finding %d\n", found);
    GraphList * temp = newGraphList(found);
    found = 0;
    for(int l = activeIndex; l < activeIndex + blockSize && l < numGraphs; l++){
      Graph * g = getGraph(gL, l);
      if (! g->isNull){
        setGraph(temp, g, found);
        found++;
      }
    }

    destroyGraphList(temp);
    clearGraphList(gL);
    numGraphs = gL->size;
    activeIndex +=  found;
    gL->activeIndex = activeIndex;
    i = activeIndex;
    //if (isMajor) dumpGraphList(gL, n, m);
  }
  clearGraphList(gL);
 }


/*
  Accepts a graphList and modifies it so that it contains only one
  representative of each color isomorphism class remains. In other words, no
  duplicates. It also filters out any invalid graphs, which are
  graphs that have either a red K3 or a green K4.
*/
/*
void cleanMTSlices(GraphList * gL, int n, int m, int maxThreads, bool isMajor){
    //printf("n = %d, m = %d\n", n, m);
    int originalSize = gL->size;
    int numGraphs = gL->size;
    int origin = gL->activeIndex;
    time_t then = time(0);
		//printf("starting Kn checking\n");
    int tempGraphs = numGraphs;
    if(!isMajor) {
      //printf("Set has %d raw\n", numGraphs);
      cleanKns(gL, n, m, &tempGraphs, isMajor);
      numGraphs = gL->size;
      //printf("Set has %d cleaned Kns\n", numGraphs);
    }

    //has to check every graph with every other graph
    //but the first one invalidates a lot of others, so not quite n^2
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100000000L;

    //printf("Starting on color Isos...\n");
		int blockSize = 1000;

		int activeIndex = gL->activeIndex;
    int blockID = 0;

    //printf("Entering main loop %d\n", numGraphs);
    int deleted = 0;

		int i = gL->activeIndex;
    while(i < numGraphs){
      //checks a block
      int j = 0;
      pthread_t pids[maxThreads];
      for (int t = 0; t < maxThreads; t++){
        MTSlicesArgs * args = malloc(sizeof * args);
        args->temp = &tempGraphs;
        args->numGraphs = numGraphs;
        args->gL = gL;
        args->base = activeIndex;
        args->blockSize = blockSize;
        args->mod = t;
        args->numThreads = maxThreads;
        args->counter = &j;
        args->deleted = &deleted;
        pthread_create(&pids[t], NULL, MTSlicesThread, args);

      }

      while(j < blockSize && isMajor && j  + activeIndex < numGraphs){
        //printf("j = %d\n", j);
        time_t now = time(0);
        int delta = (now - then);
        printf("%3.1f%% done with block %d, %d / %d or %3.1f%% total, averaging %3.6f g/s (%3.2f / t), total of %d gs over %02d:%02d:%02d\n",
        (100.0 * j) / blockSize, blockID, activeIndex, numGraphs, 100.0 * activeIndex/numGraphs, (deleted)*1.0/delta, (deleted)*1.0/delta/maxThreads, deleted, delta/3600, (delta/60)%60, delta%60);
        printf("\033[F\033[J");
        nanosleep(&ts, NULL);
      }
      //printf("done with loop\n");
      for (int k = 0; k < maxThreads; k++){
        //printf("%d\n", k);
  			void * status;
  			pthread_join(pids[k], &status);
  		}

      //cleans up after the block is checked
      blockID++;
      int found = 0;
      for(int l = activeIndex; l < activeIndex + blockSize && l < numGraphs ; l++){
        Graph * g = getGraph(gL, l);
        if (! g->isNull){
          found++;
        }
      }
      //printf("done finding %d\n", found);
      GraphList * temp = newGraphList(found);
      found = 0;
      for(int l = activeIndex; l < activeIndex + blockSize && l < numGraphs; l++){
        Graph * g = getGraph(gL, l);
        if (! g->isNull){
          setGraph(temp, g, found);
          found++;
        }
      }

      destroyGraphList(temp);
      clearGraphList(gL);
      numGraphs = gL->size;
      activeIndex +=  found;
      gL->activeIndex = activeIndex;
      i = activeIndex;
      if (isMajor) dumpGraphList(gL, n, m);
    }
    clearGraphList(gL);
 }
*/

/*
  Implements the bulk of the algorithm as described in the project proposal.
  Returns the smallest int n such that there are no valid colorings of Kn.
*/
int run(cmdLineArgs args){
  int cleanType = args.type;
  int tiers = args.maxIters;
  int i = 0;
  tier * bestFound = findLatest(args.n, args.m);
  //creates an array of graphlists
  GraphList ** graphTiers = malloc(tiers * sizeof(*graphTiers));

  if (bestFound->tier > 2){
    *(graphTiers + bestFound->tier - 1) = bestFound->gL;
    sortGraphList(bestFound->gL);
    i = bestFound->tier;
  }else{
    printf("Generating First GraphList\n");
    *graphTiers = newGraphList(1);
    printf("Generating K1\n");
    **((*graphTiers)->graphs) = createKn(1);
    i = 1;
  }

  //printf("size = %d\n", (*(graphTiers + bestFound->tier - 1))->size);
  printf("starting mem usage:");
  dumpMallinfo();

  if (bestFound->tier > 2 && bestFound->gL->activeIndex < bestFound->gL->size) {
    printf("Cleaning existing graphs %d\n", bestFound->gL->size);
    if(cleanType == 0){
      cleanST(*(graphTiers + bestFound->tier - 1), args.n, args.m, true);
    }else if(cleanType == 1){
      cleanMTSlices(*(graphTiers + bestFound->tier - 1), args.n, args.m, args.maxThreads, true);
    }else if(cleanType == 2){
      cleanMTPolling(*(graphTiers + bestFound->tier - 1), args.n, args.m, args.maxThreads, true);
    }else if(cleanType == 3){
      experimentalSortedCleaner(*(graphTiers + bestFound->tier - 1), args.n, args.m, true);
    }
    printf("Done cleaning existing graphs %d left\n", bestFound->gL->size);
  }

  for(i; i < tiers; i++){
    printf("Starting row %d\n", i + 1);
    *(graphTiers + i) = newGraphList(0);
    printf("Before generating next size: ");
    dumpMallinfo();

    int num = (*(graphTiers + i - 1))->size;

    printf("size = %d\n", num);
    for(int j = 0; j < num; j++){
      printf("Generating the next size: %3d%% done... %.3f KB %d graphs\n", (int)(j*100/num), getMallInfo()/1000.0, (*(graphTiers + i))->size);
      GraphList * gL = getNextSize(getGraph(*(graphTiers + i - 1), j));
      //printf("Done generating...\n");
      if(cleanType == 0){
        cleanST(gL, args.n, args.m, false);
      }else if(cleanType == 1){
        cleanMTSlices(gL, args.n, args.m, args.maxThreads, false);
      }else if(cleanType == 2){
        cleanMTPolling(gL, args.n, args.m, args.maxThreads, false);
      }else if(cleanType == 3){
        //cleanST(gL, args.n, args.m, false);
        sortGraphList(gL);
        experimentalSortedCleaner(gL, args.n, args.m, false);
      }
      printf("\033[F\033[J");
      mergeGraphLists(*(graphTiers + i), gL);
    }
    printf("After generating next size: ");
    dumpMallinfo();
    if(cleanType == 3){
      printf("Sorting %d graphs\n", (*(graphTiers + i))->size);
      sortGraphList((*(graphTiers + i)));
      printf("Done sorting\n");
    }
    dumpGraphList(*(graphTiers + i), args.n, args.m);
    printf("%d has %d graphs raw\n", i+1, (*(graphTiers + i))->size);
    printf("Cleaning this set up...\n");
    if(cleanType == 0){
      cleanST(*(graphTiers + i), args.n, args.m, true);
    }else if(cleanType == 1){
      cleanMTSlices(*(graphTiers + i), args.n, args.m, args.maxThreads, true);
    }else if(cleanType == 2){
      cleanMTPolling(*(graphTiers + i), args.n, args.m, args.maxThreads, true);
    }else if(cleanType == 3){
      experimentalSortedCleaner(*(graphTiers + i), args.n, args.m, true);
    }

    printf("%d has %d graphs cleaned\n",i+1, (*(graphTiers + i))->size);
    dumpGraphList(*(graphTiers + i), args.n, args.m);
    printf("-------------------\n");
    printf("%d : %d\n", i + 1, (*(graphTiers + i))->size);

  }
}


/*
  Every say 10000 graphs, join all threads up that point, remove all nulls, and write to file
  the graphs you've found so far. Then continue for the next thousand, etc. Also update the raw file to only include those
  graphs that haven't been culled yet

  I'd have to update the getlatest function to return the latest raw file that still has files in it

*/


int main(int argc, char **argv){
  cmdLineArgs args = processArgs(argc, argv);

  time_t before;
  time_t after;
  before = time(NULL);
  run(args);
  after = time(NULL);
  printf("That took %d s\n", after - before);
  return 0;
}
