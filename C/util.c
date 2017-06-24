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

  intList * charListGSorted = getCharList(g, RED);
  intList * charListHSorted = getCharList(h, RED);

  int n = g->n;
  qsort(charListGSorted->values,n,sizeof(int),cmpfunc);
  qsort(charListHSorted->values,n,sizeof(int),cmpfunc);


  int result = memcmp(charListGSorted->values, charListHSorted->values, n*sizeof(int));
  freeIntList(charListGSorted);
  freeIntList(charListHSorted);

  if(result == 0){
    intList * charListG = getCharList(g, RED);
    intList * charListH = getCharList(h, RED);
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

    ///free(vertsG);
    //free(vertsH);
    freeIntList(charListH);
    freeIntList(charListG);
    //printf("at end of isColorIso %d\n", getMallInfo());
    return ans;
  }else{
    //printf("at end of isColorIso %d\n", getMallInfo());
    return false;
  }
}

void * isColorIsoThread(void * args){
	isColorIsoThreadArgs * args1 = (isColorIsoThreadArgs *) args;
	Graph * current = args1->current;
	for(int j = args1->numGraphs - 1; j > args1->i; j--){
			//printf("Checking %d vs %d\n", i, j);
			Graph * other = getGraph(args1->gL, j);
			//if (current->isNull) break;
			if (!other->isNull){
				//printGraph(args.current);
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
  Accepts a graphList and modifies it so that it contains only one
  representative of each color isomorphism class remains. In other words, no
  duplicates. It also filters out any invalid graphs, which are
  graphs that have either a red K3 or a green K4.
*/
void clean(GraphList * gL, int n, int m, int maxThreads){
    //printf("n = %d, m = %d\n", n, m);
    int numGraphs = gL->size;
    int foundGraphs = 0;

		//printf("starting Kn checking\n");
    int temp = numGraphs;
    //Now we have to check there are no red K3s and no green K4s
    //Honestly its probably more efficient to check this first,
    //should be tested
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
	          temp--;
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
	          temp--;
					}
				}
      }
      if(gL->size > 5000 && i%100 == 0){
        printf("%3.1f%% done checking for Kns... %d / %d", (i*100.0/gL->size), i, temp);
        printf("\n\033[F\033[J");
      }
    }
		//printf("done Kn checking, starting color iso checking %d\n", numGraphs);
    //has to check every graph with every other graph
    //but the first one invalidates a lot of others, so not quite n^2

		pthread_t pids[numGraphs];
		int threads = 0;
		int activeIndex = 0;
		int numActive = 0;
		int i = 0;
    while(i < numGraphs){
			if (numActive < maxThreads){

	      Graph * current = getGraph(gL, i);
	      if(!current->isNull){
					isColorIsoThreadArgs * args = malloc(sizeof * args);
					args->current = current;
					args->temp = &temp;
					args->numGraphs = numGraphs;
					args->gL = gL;
					args->i = i;
					args->numActive = &numActive;
					numActive++;
					pthread_create(&pids[threads], NULL, isColorIsoThread, args);
					threads++;
				}
				i++;
			}
			if(gL->size > 1000 && i%40 == 0){
        printf("%3.1f%% done checking color isos... %d / %d %d active", (i*100.0/gL->size), i, temp, numActive);
        printf("\n\033[F\033[J");
      }
    }

		for (int k = 0; k < threads; k++){
			void * status;
			pthread_join(pids[k], &status);
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
int run(cmdLineArgs args){
  int tiers = args.maxIters;
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
      clean(gL, args.n, args.m, args.maxThreads);
      printf("\033[F\033[J");
      //printf("done cleaning\n");
      mergeGraphLists(*(graphTiers + i), gL);
    }
    printf("After generating next size: ");
    dumpMallinfo();

    printf("%d has %d graphs raw\n", i+1, (*(graphTiers + i))->size);
    printf("Cleaning this set up...\n");

    clean(*(graphTiers + i), args.n, args.m, args.maxThreads);

    printf("%d has %d graphs cleaned\n",i+1, (*(graphTiers + i))->size);

    printf("-------------------\n%d : %d\n", i + 1, (*(graphTiers + i))->size);
  }
}



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
