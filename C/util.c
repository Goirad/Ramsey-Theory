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
    qsort(current->charListSorted->values, n + 1 ,sizeof(*current->charListSorted->values),cmpfunc);
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
long fact(int n){
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
void decToFact(intList * dest, long n, int dig){
  int temp[dig];
  long num = n;
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

void collapseVerts(intList2D * verts, intList * dest, int n){
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
bool recIsoCheck(intList2D * vertsG, intList2D * vertsH, int depth, Graph * g, Graph * h, isoCheckMemBlock * memBlock){
  //printf("depth = %d / %d\n", depth, g->n-1);
  //if (h->isNull) return true;
  if(depth >= g->n-1){
    collapseVerts(vertsG, memBlock->collapseVertsG, g->n);
    collapseVerts(vertsH, memBlock->collapseVertsH, h->n);
    for(int i = 0; i < g->n-1; i++){
      for(int j = i + 1; j < g->n; j++){
        if(getEdgeColor(g, getIntListIndex(memBlock->collapseVertsG, i), getIntListIndex(memBlock->collapseVertsG, j))
          != getEdgeColor(h, getIntListIndex(memBlock->collapseVertsH, i), getIntListIndex(memBlock->collapseVertsH, j))){
          return false;
        }
      }
    }
    return true;
  }else{
    if(vertsG->arrays[depth]->length > 0){
      for(long i = 0; i < fact(vertsG->arrays[depth]->length); i++){
        decToFact(memBlock->perm, i, memBlock->origVertsG->arrays[depth]->length);
        permuteList(memBlock->origVertsG->arrays[depth], memBlock->vertsG->arrays[depth], memBlock->perm, memBlock->permScratch);
        bool ans = recIsoCheck(vertsG, vertsH, depth + 1, g, h, memBlock);
        if (ans) return true;
      }
      return false;
    }else{
      return recIsoCheck(vertsG, vertsH, depth + 1, g, h, memBlock);
    }
  }
}

/*
  Simple function used by qsort
*/
int cmpfunc (const void * a, const void * b){
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
bool isColorIso(Graph * g, Graph * h, isoCheckMemBlock * memBlock){

  int n = g->n;

  int result = memcmp(g->charListSorted->values, h->charListSorted->values, n*sizeof(*h->charListSorted->values));

  if(result == 0){
    intList * charListG = g->charList;
    intList * charListH = h->charList;
    for(int i = 0; i < n; i++){
      memBlock->vertsG->arrays[i]->length = 0;
      memBlock->vertsH->arrays[i]->length = 0;
      memBlock->origVertsG->arrays[i]->length = 0;
    }
/*
    intList * c = copyIntList(charListG);
    intList * c2 = copyIntList(charListH);
    int k = 1;
    for(int i = 0; i < n; i++){
      int x = getIntListIndex(c, i);
      if(x > k){
        for(int j = 0; j < n; j++){
          if(getIntListIndex(c, j) == x){
            setIntListIndex(c, j, k);
          }
          if(getIntListIndex(c2, j) == x){
            setIntListIndex(c2, j, k);

          }
        }
        k++;
      }
    }
*/
    for(int i = 0 ; i < n; i++){
      setIntListIndex(memBlock->vertsG->arrays[charListG->values[i]], memBlock->vertsG->arrays[charListG->values[i]]->length, i);
      (memBlock->vertsG->arrays[charListG->values[i]]->length)++;
      setIntListIndex(memBlock->origVertsG->arrays[charListG->values[i]], memBlock->origVertsG->arrays[charListG->values[i]]->length, i);
      (memBlock->origVertsG->arrays[charListG->values[i]]->length)++;
      setIntListIndex(memBlock->vertsH->arrays[charListH->values[i]], memBlock->vertsH->arrays[charListH->values[i]]->length, i);
      (memBlock->vertsH->arrays[charListH->values[i]]->length)++;
    }

    bool ans = recIsoCheck(memBlock->vertsG, memBlock->vertsH, 0, g, h, memBlock);
    return ans;
  }else{
    return false;
  }
}

void * MTPollingThread(void * args){
  MTPollingArgs * args1 = (MTPollingArgs *) args;
  isoCheckMemBlock * memBlock = getIsoCheckMemBlock(getGraph(args1->gL, 0)->n);

  Graph * current = args1->current;
  for(int j = args1->numGraphs - 1; j > args1->i; j--){
    Graph * other = getGraph(args1->gL, j);
    if (!other->isNull){
      if(isColorIso(current, other, memBlock)){
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
  isoCheckMemBlock * memBlock = getIsoCheckMemBlock(getGraph(args1->gL, 0)->n);
  int threadBase = args1->base + args1->mod;
  for(int j = 0; j < args1->blockSize &&  threadBase + j < args1->numGraphs; j += args1->numThreads){
    Graph * current = getGraph(args1->gL, threadBase + j);
    if(!current->isNull){
    	for(int k = args1->numGraphs - 1; k > threadBase + j; k--){
  			Graph * other = getGraph(args1->gL, k);
  			if (!other->isNull){
          //("checking iso\n");
  				if(isColorIso(current, other, memBlock)){
            //printf("adding to list %d %d\n", args1->deletes->length, args1->deletes->size);

            //printf("done adding to list\n");
            //if(!other->isNull){
            if (k < threadBase + args1->blockSize){
              other->isNull = true;
            }else{

            }
            addToIntListME(args1->deletes, k, 1000);
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
*/


/*

  Check if we have more verification block space, if so reserve one.





*/

int cmpGraphs(Graph * g, Graph * h){
	return memcmp(g->charListSorted->values, h->charListSorted->values, g->n * sizeof * g->charListSorted->values);
}

int cmpGraphsWL(GraphList * gL, int i, int j){
	Graph * g = getGraph(gL, i);
	Graph * h = getGraph(gL, j);
	return cmpGraphs(g, h);
}

/* Skeleton
 *
 * Check if any veri slots are free, if so reserve one
 *
 * If we have a current graph to check, go ahead and do so, if not get a new chunk
 * 	this includes setting up finding out how long the chunk is etc
 *
 * Check the graph against the rest of the chunk, at the end verify it
 *
 * Scan for the next non-null graph in chunk, set it to current
 *
 * Goto step 1
 *
 *
 */


void * MTSlicesThread(void * rawArgs){

	MTSlicesArgs * args = (MTSlicesArgs *) rawArgs;
  isoCheckMemBlock * memBlock = getIsoCheckMemBlock(getGraph(args->gL, 0)->n);
  int threadBase = args->base + args->mod;
	//printf("Thread %d starting...\n", args->mod);
	int chunkStart;
	int current;
	int chunkEnd;

  bool validRoom = false;
	bool hasCurrent = false;

	pthread_mutex_lock(args->validatedMutex);
	if(*args->validated < args->blockSize){
		(*args->validated)++;
		validRoom = true;
	}
	pthread_mutex_unlock(args->validatedMutex);

	//printf("Thread %d got a slot\n", args->mod);

	if(validRoom){
		if(!TSS_Pop(args->chunkStack, &current)){
			hasCurrent = true;
			//printf("%d chunk\n", current);
			//identify chunk bounds, chunkStart, current, chunkEnd
			chunkStart = current;
			chunkEnd = chunkStart;
      args->charListStr = getIntListStr(getGraph(args->gL, chunkStart)->charListSorted);
			while(chunkEnd + 1 < args->numGraphs && !cmpGraphsWL(args->gL, chunkStart, chunkEnd)){
				chunkEnd++;
			}
		}
	}

	//printf("Thread %d got block %d, %d graphs\n", args->mod, chunkStart, chunkEnd-chunkStart + 1);

	while(validRoom && hasCurrent){
    args->current = current;
    args->chunkStart = chunkStart;
    args->chunkLength = chunkEnd - chunkStart;
		Graph * g = getGraph(args->gL, current);

		if(current != chunkEnd){
			for(int i = current + 1; i <= chunkEnd; i++){
				Graph * other = getGraph(args->gL, i);
				if(!other->isNull && isColorIso(g, other, memBlock)){
					other->isNull = true;
					(*args->deleted)++;
          //printf("%d\n\n\n\n\n\n\n\n\n\n\n", (*args->deleted));
				}
			}
		}

		g->isValidated = true;
		hasCurrent = false;

		//find the next non null valid graph in the chunk

		for(int i = current + 1; i < chunkEnd; i++){
			if(!getGraph(args->gL, i)->isNull){
				current = i;
				hasCurrent = true;
				break;
			}
		}

		//try and reserve the next slot
		//printf("Trying to reserve next slot\n");
		pthread_mutex_lock(args->validatedMutex);

		if(*args->validated < args->blockSize){
			(*args->validated)++;
			validRoom = true;
		}else{
			validRoom = false;
		}
		pthread_mutex_unlock(args->validatedMutex);
		//printf("Next slot reserved\n");
		//if no current graph, get a new one
		if(validRoom && !hasCurrent){
			//we haven't gone through all of this chunk
			if(!TSS_Pop(args->chunkStack, &current)){
				hasCurrent = true;

				//identify chunk bounds, chunkStart, current, chunkEnd
				chunkStart = current;
				chunkEnd = chunkStart + 1;
        free(args->charListStr);
        args->charListStr = getIntListStr(getGraph(args->gL, chunkStart)->charListSorted);
				while(!cmpGraphsWL(args->gL, chunkStart, chunkEnd)){
					chunkEnd++;
				}
				//printf("Thread %d got block %d, %d graphs\n", args->mod, chunkStart, chunkEnd-chunkStart + 1);
			}
		}

	}
  //printf("thread stopped\n");
  *args->isActive = 0;
  freeIsoCheckMemBlock(memBlock);
  //free(args);
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
/*
	if(g1->isValidated && !h1->isValidated){
		return -1;
	}else if(!g1->isNull && h1->isValidated){
		return 1;
	}else{
		return memcmp(gCharList->values, hCharList->values, gCharList->length * sizeof * gCharList->values);
	*/
  if(g1->isValidated && !h1->isValidated){
    return -1;
  }else if(!g1->isValidated && h1->isValidated){
    return 1;
  }else{
    return memcmp(gCharList->values, hCharList->values, gCharList->length * sizeof * gCharList->values);
  }

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

		int blockSize = 5000;

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
  int blockSize = 5000;

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
      if(!memcmp(current->charListSorted->values, other->charListSorted->values, current->charListSorted->length  * sizeof * current->charList->values)){
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
            if(isColorIso(current, other, memBlock)){
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
    //if (isMajor) dumpGraphList(gL, n, m);
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

	/* TODO Sort graphs first so that the loop doesn't have to check against
	 * every othe graph, benchamark and see if constant memchecks are worth it
	 * TODO
	 * TODO
	 * TODO
	 * TODO
	 * TODO
	 */

  //has to check every graph with every other graph
  //but the first one invalidates a lot of others, so not quite n^2
  //printf("Starting on color Isos...\n");
  int blockSize = 1000;

  int activeIndex = gL->activeIndex;
  int blockID = 0;

  //printf("Entering main loop %d\n", numGraphs);
  int deleted = 0;
  isoCheckMemBlock * memBlock;
  int i = gL->activeIndex;
  if(numGraphs > 0){
    memBlock = getIsoCheckMemBlock(getGraph(gL, 0)->n);
  }

  while (i < numGraphs){
    //checks a block
    for (int j = 0; j < blockSize && i + j < numGraphs; j++){
      Graph * current = getGraph(gL, i + j);
      if (!current->isNull){
        for(int k = numGraphs - 1; k > i + j; k--){
          Graph * other = getGraph(gL, k);
          if (!other->isNull){
            if(isColorIso(current, other, memBlock)){
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
    if (isMajor) dumpGraphList(gL, n, m);
  }
  //printf("about to clear %d graphs\n", numGraphs);
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

void freeIsoCheckMemBlock(isoCheckMemBlock * block){
  freeIntList2D(block->vertsG);
  freeIntList2D(block->origVertsG);
  freeIntList2D(block->vertsH);

  freeIntList(block->collapseVertsG);
  freeIntList(block->collapseVertsH);
  freeIntList(block->perm);
  freeIntList(block->permScratch);
  free(block);
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
  ts.tv_nsec = 200000000L;

  //printf("Starting on color Isos...\n");
  int blockSize = 5000;

  int activeIndex = gL->activeIndex;
  int blockID = 0;

  //printf("Entering main loop %d\n", numGraphs);
  int deleted[maxThreads];
  for(int i = 0; i < maxThreads; i++){
    deleted[i] = 0;
  }
  while(gL->activeIndex < numGraphs){
	  //printf("sorting list\n");
	  sortGraphList(gL);
	  //printf("making stack\n");
	  //So now we need to generate the stack
	  struct TSStack * stack = TSS_New();
	  Graph * current = getGraph(gL, activeIndex);
	  TSS_Push(stack, activeIndex);
	  //printf("finding chunks\n");
	  for(int i = activeIndex + 1; i < numGraphs; i++){

	    Graph * other = getGraph(gL, i);
	    //printf("%d, %d, %d\n", i, numGraphs, 3);
	    if(cmpGraphs(current, other)){
			  //printf("about to push\n");
        //printf("pushing %d\n", i);
	      TSS_Push(stack, i);
			  //printf("post push\n");
	      current = getGraph(gL, i);
	    }
	  }
    //printf("stack populated\n");
    //checks a block
    int j = 0;
    pthread_t pids[maxThreads];
    int validated = 0;
    pthread_mutex_t * validatedMutex = malloc(sizeof * validatedMutex);
    pthread_mutex_init(validatedMutex, NULL);
    MTSlicesArgs ** args = malloc(maxThreads * sizeof * args);
    int actives[maxThreads];


    for (int t = 0; t < maxThreads; t++){
      args[t] = malloc(sizeof * args[t]);
      args[t]->numGraphs = numGraphs;
      args[t]->gL = gL;
      args[t]->base = activeIndex;
      args[t]->blockSize = blockSize;
      args[t]->mod = t;
      args[t]->charListStr = "Hi";
      args[t]->numThreads = maxThreads;
      args[t]->validated = &validated;
      args[t]->deleted = &deleted[t];
      args[t]->validatedMutex = validatedMutex;
      args[t]->chunkStack = stack;
      args[t]->isActive = &actives[t];
      actives[t] = 1;
      pthread_create(&pids[t], NULL, MTSlicesThread, args[t]);


    }
    int numActive = sumArray(actives, maxThreads);
    //printf("%d active\n", numActive);
    while(validated < blockSize && isMajor && validated + activeIndex < numGraphs && numActive > 0){
      int tot = sumArray(deleted, maxThreads);
      numActive = sumArray(actives, maxThreads);
      //printf("j = %d\n", j);
      time_t now = time(0);
      int delta = (now - then);
        printf("Thread    Chunk  Length Current     G/s Active Characteristic List\n");
      for(int i = 0; i < maxThreads; i++){
        printf("%6d %8d %7d %7d %7.3f      %d %s\n", args[i]->mod, args[i]->chunkStart, args[i]->chunkLength, args[i]->current - args[i]->chunkStart, 1.0* *args[i]->deleted/delta, *args[i]->isActive, args[i]->charListStr);
      }
      printf("%d %3.1f%% done with block %d, %d / %d or %3.1f%% total, averaging %3.6f g/s (%3.2f / t), total of %d gs over %02d:%02d:%02d used %.3f KB\n",
      stack->list->length, (100.0 * validated) / blockSize, blockID, activeIndex, numGraphs, 100.0 * activeIndex/numGraphs,
      (tot)*1.0/delta, (tot)*1.0/delta/maxThreads, tot, delta/3600, (delta/60)%60, delta%60, getMallInfo()/1000.0);

      for(int i = 0; i < maxThreads + 1; i++){
        printf("\033[F");
      }
      printf("\033[F\033[J");
      nanosleep(&ts, NULL);
    }
    //printf("done with loop\n");
    for (int k = 0; k < maxThreads; k++){
      //printf("%d\n", k);
      void * status;
      pthread_join(pids[k], &status);
    }

    blockID++;
    //destroyGraphList(temp);
		//printf("clearing\n");
    clearGraphList(gL);
		//printf("done clearing\n");
    numGraphs = gL->size;
    if(activeIndex + blockSize < numGraphs){
      activeIndex +=  blockSize;
      gL->activeIndex = activeIndex;
    }else{
      activeIndex = numGraphs;
      gL->activeIndex = numGraphs;
    }
	  TSS_Destroy(stack);
    if (isMajor) {
      sortGraphList(gL);
      dumpGraphList(gL, n, m);
    }
  }
  clearGraphList(gL);
 }
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

    //printf("size = %d\n", num);
    for(int j = 0; j < num; j++){
      printf("Generating the next size: %3d%% done... %.3f KB %d graphs\n", (int)(j*100/num), getMallInfo()/1000.0, (*(graphTiers + i))->size);
      GraphList * gL = getNextSize(getGraph(*(graphTiers + i - 1), j));
      //printf("Done generating...\n");
      if(cleanType == 0){
        cleanST(gL, args.n, args.m, false);
      }else if(cleanType == 1){
        //cleanST(gL, args.n, args.m, false);
        cleanMTSlices(gL, args.n, args.m, args.maxThreads, false);
      }else if(cleanType == 2){
        cleanMTPolling(gL, args.n, args.m, args.maxThreads, false);
      }else if(cleanType == 3){
        //cleanST(gL, args.n, args.m, false);
        sortGraphList(gL);
        experimentalSortedCleaner(gL, args.n, args.m, false);
      }
      printf("\033[F\033[J");
      //printf("done cleaning\n");
      for(int k = 0; k < gL->size; k++){
        getGraph(gL, k)->isValidated = false;
      }
      mergeGraphLists(*(graphTiers + i), gL);
    }
    printf("After generating next size: ");
    dumpMallinfo();

    dumpGraphList(*(graphTiers + i), args.n, args.m);
    printf("%d has %d graphs raw\n", i+1, (*(graphTiers + i))->size);
    printf("Cleaning this set up...\n");
    if(cleanType == 0){
      cleanST(*(graphTiers + i), args.n, args.m, true);
    }else if(cleanType == 1){
      //cleanST(*(graphTiers + i), args.n, args.m, true);
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
