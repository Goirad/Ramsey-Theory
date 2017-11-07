#include "defs.h"
/*
  Returns the characteristic list of a graph.
  The characteristic list is a list with 1 number for each vertex,
  and each number is how many c colored edges there are leaving that vertex.
  Two graphs having the same characteristic list is a necessary but
  not sufficient condition for them to be color isomorphic.
  Remember to free this list when you're done with it!
*/
intList * getCharList(Graph * g, Color c){
  intList * charList = newIntList(g->n);
  int i, j;
  for(i = 0; i < g->n; i++){
    char colorEdges = 0;
    for(j = 0; j < g->n; j++){
      if(getEdgeColor(g, i, j) == c){
        colorEdges++;
      }
    }
    setIntListIndex(charList, i, colorEdges);
  }


  //charlist contains a list where the value at i is the number of red edges from i

  intList * charList2 = newIntList(g->n);
  for(i = 0; i < g->n; i++){
    int k = 0;
    for(j = 0; j < g->n; j++){
      if(i != j && getEdgeColor(g, i, j) == c){
        k += getIntListIndex(charList, j);
      }
    }
    setIntListIndex(charList2, i, k);
  }

  for(i = 0; i < g->n; i++){

    int a = getIntListIndex(charList, i);
    int b = getIntListIndex(charList2, i);
    if (a == 0) {
      //printf("a: %d b:%d\n", a, b);
    }
    setIntListIndex(charList, i, (a<<21) + (b<<5) + 31);
  }
  freeIntList(charList2);
  /*
  for(int i = 0; i < g->n; i++){
    int x = getIntListIndex(charList, i);
    if (!x) {
      //printIntList(charList);
      //printGraphL(g);

    }
  }*/



/*
  char x = getIntListIndex(charList, 0);
  bool u = true;
  for(i = 0; i < g->n; i++){
    if( getIntListIndex(charList, i) != x ){
      u = false;
      break;
    }
  }
  if(0){
    printIntList(charList);
    printGraphL(g);
  }*/
  return charList;
}


int dijkstra(Graph * g, int vert){
  intList * lengths = newIntList(g->n);
  intList * unvisited = newIntList(g->n);

  for(int i = 0; i < g->n; i++){
    setIntListIndex(unvisited, i, 1);
    if (i == vert) {
      setIntListIndex(lengths, i, 0);
    }else{
      setIntListIndex(lengths, i, 1000);
    }
  }

  int current = vert;
  int currentLength = getIntListIndex(lengths, current);
  while (getNthNonNegValue(unvisited, 0) != -1) {
    for (int i = 0; i < g->n; i++){
      if (i != current && getEdgeColor(g, current, i) == RED && currentLength + 1 < getIntListIndex(lengths, i)){
        setIntListIndex(lengths, i, currentLength + 1);
      }
    }
    setIntListIndex(unvisited, current, -1);
    intList * t = newIntList(g->n) ;

    for(int i = 0; i < g->n; i++){
      if (getIntListIndex(unvisited, i) > 0) {
        setIntListIndex(t, i, getIntListIndex(lengths, i));
      }else{
        setIntListIndex(t, i, 1000);
      }
      current = intListMinIndex(t);
      currentLength = intListMin(t);
    }

    int lens[g->n];

    for(int i = 0; i < g->n; i++){
      lens[getIntListIndex(lengths, i)] ++;
    }

    return (lens[0] << 27) + (lens[1] << 22) + (lens[2] << 17);
    //find shortest path and go from there

  }
}

/*
  Prints one half of the graph's adjacency matrix, to have a
  human readable representation of the graph.
*/
void printGraph(Graph * g){
  int n = g->n;
  int i, j;
  printf("  ");
  for(i = 0; i < n; i++){
    printf("%d ", i);
  }
  printf("\n");
  i = 0;
  while (i < n){
    printf("%d ", i);
    for(j = 0; j < i; j++){
      printf("%d ", *(g->edges + i*(i-1)/2 + j));
    }
    printf("\n");
    i++;
  }
}

/*
  Prints the graph the way it is stored internally, as a single 1D array of edge
  colors.
*/
void printGraphL(Graph * g){
  for(int i = 0; i < g->n*(g->n - 1)/2; i++){
    printf("%d", *(g->edges + i));
  }
  printf("\n");
}

/*
  Creates a pointer to a complete graph with numVertices vertices
  and where every vertex is connected to every other vertex.
  Note: Remember to free graphs when you are done using them to
  avoid a memory leak.
  See destroyGraph
*/
Graph * createKn(int numVertices) {
  int numEdges = numVertices * (numVertices - 1) / 2;
  Graph * Kn = malloc(sizeof(*Kn));
  Kn->isNull = false;
  Kn->edges =  malloc(numEdges * sizeof *(Kn->edges));
  Kn->n = numVertices;
  Kn->charList = getCharList(Kn, RED);
  Kn->charListSorted = getCharList(Kn, RED);
  qsort(Kn->charListSorted->values,numVertices,sizeof(int),cmpfunc);
  int edgeCount;
  for (edgeCount = 0; edgeCount < numEdges; edgeCount++) {
    *(Kn->edges + edgeCount) = RED;
  }

  return Kn;
}

/*
  Returns a pointer to an initialized graphList that can store
  numGraphs graphs
*/
GraphList * newGraphList(int numGraphs){
  GraphList * gL = malloc(sizeof *gL);
  gL->size = numGraphs;
  gL->activeIndex = 0;
  gL->graphs = malloc(sizeof *(gL->graphs));
  *gL->graphs = calloc(numGraphs , sizeof *(gL->graphs));
  return gL;
}

/*
  Returns a pointer to a new graph, identical to g.
  Remember to free this memory when you're done with it.
  See destroyGraph
*/
//TODO broken, doesn't copy charLists
Graph * copyGraph(Graph * g){
  Graph * out = malloc(sizeof *out);
  int n = g->n;
  out->edges = malloc(n*(n-1)/2 * sizeof *(out->edges));
  for(int i = 0; i < n*(n-1)/2; i++){
    *(out->edges + i) = *(g->edges + i);
  }
  out->n = g->n;
  out->isNull = g->isNull;
  return out;
}

/*
  Frees not only the graph pointer but also its edge array.
  Extremely important to avoid memory leaks.
*/
void destroyGraph(Graph * g){
  if(g != NULL){
    if(g->isNull){
      free(g->edges);
      freeIntList(g->charList);
      freeIntList(g->charListSorted);
      free(g);
    }else{
      printf("woops tried to free non null graph\n");
    }
  }else{
    printf("woops tried to free null graph pointer\n");
  }
}

/*
  Returns the color of the edge between vertices m and n in graph g
*/
Color getEdgeColor(Graph * g, int n, int m){
  if( n == m ) {
    return NONE;
  }else if (n > m){
    int base = n*(n-1)/2;
    return *(g->edges + base + m);
  }else{
    int base = m*(m-1)/2;
    return *(g->edges + base + n);
  }
}


Color getEdgeColorRaw(Graph * g, int n){
  return *(g->edges + n);
}
/*
  Sets the color of the edge between vertices m and n in g to color c
*/
void setEdgeColor(Graph * g, int n, int m, Color c){
  if (n > m){
    int base = n*(n-1)/2;
    *(g->edges + base + m) = c;
  }else{
    int base = m*(m-1)/2;
    *(g->edges + base + n) = c;
  }
}

/*
  Returns the number of edges in g adjacent to vertex that
  are color c.
  Currently not used for anything
*/
int numColorEdges (Graph * g, Color c, int vertex) {
	int counter = 0;
	int numVertices = g->n;

	for (int i = 0; i < numVertices; i++) {
		if (getEdgeColor(g, vertex, i) == c) {
			counter++;
		}
	}

	return counter;
}

/*
  Returns a graph with all the same vertices as inGraph but only with edges
  that are color C.
  Remember to free the subgraph when you're done!
  Currently not used for anything
*/
Graph * getSubGraph(Graph * inGraph, Color col){

  Graph * outGraph = malloc(sizeof(*outGraph));
  int i;
  int n = inGraph->n;
  outGraph->n = n;
  int numEdges = n * (n - 1) / 2;

  int k = 0;
  for(i = 0; i < numEdges; i++){
    if(*(inGraph->edges + i) == col){
      *(outGraph->edges + k) = *(inGraph->edges + i);
      k++;
    }
  }
  return outGraph;
}

void clearGraphList(GraphList * gL){
  GraphList * cleanedGraphs = newGraphList(gL->size);
  int foundGraphs = 0;
  for(int i = 0; i < gL->size; i++){
    Graph * current = getGraph(gL, i);
    if(!current->isNull){
      *(*cleanedGraphs->graphs + foundGraphs) = *(*gL->graphs + i);
      foundGraphs++;
    }
  }
	//printf("%d graphs originally, %d now\n", gL->size, foundGraphs);
  for(int i = 0; i < foundGraphs; i++){
    if((*(*gL->graphs + i))->isNull) destroyGraph(*(*gL->graphs + i));
    *(*gL->graphs + i) = *(*cleanedGraphs->graphs + i);

  }
  destroyGraphList(cleanedGraphs);
  shrinkGraphList(gL, foundGraphs);
}


/*
  Takes a graph list that only has a few non null graphs at the start
  and frees all the null graphs. It also reallocates the array's memory
*/
void shrinkGraphList(GraphList * gL, int newSize){
  for(int i = newSize; i < gL->size; i++){
    Graph * current = getGraph(gL, i);
    if(current->isNull){
      destroyGraph(current);
    }
  }

  int oldSize = gL->size;
  gL->size = newSize;
  *gL->graphs = realloc(*gL->graphs, newSize * sizeof **gL->graphs);
}

/*
  Destroys a graph list, frees all null graphs and all necessary pointers
*/
void destroyGraphList(GraphList * gL){
  for(int i = 0; i < gL->size; i++){
    if(!(getGraph(gL, i) == NULL)){
      if(getGraph(gL, i)->isNull){
        destroyGraph(getGraph(gL, i));
      }
    }
  }

  free(*gL->graphs);
  free(gL->graphs);
  free(gL);

}

/*
  Merges two graphlists together, or rather merges gLB into gLA
  Unnecessary data is freed in gLB
*/
void mergeGraphLists(GraphList * gLA, GraphList * gLB){
  *gLA->graphs = realloc(*gLA->graphs, sizeof *gLA->graphs * (gLA->size + gLB->size));

  for(int i = gLA->size; i < gLA->size + gLB->size; i++){
    setGraph(gLA, getGraph(gLB, i - gLA->size), i);
  }

  gLA->size = gLA->size + gLB->size;
  destroyGraphList(gLB);

}

/*
  Sets the nth graph of gL to be g
*/
void setGraph(GraphList * gL, Graph * g, int n){
  *(*gL->graphs + n) = g;
}

/*
  Returns a pointer to the nth graph of gL
*/
Graph * getGraph(GraphList * gL, int n){
  return *(*gL->graphs + n);
}

Graph * newGraph(int n, char * raw){
  Graph * g = malloc(sizeof * g);
  g->edges = malloc(n*(n-1)/2 * sizeof *(g->edges));
  g->n = n;
  g->isNull = false;
  g->isValidated = false;

  for(int i = 0; i < strlen(raw) - 1; i++){
    g->edges[i] = raw[i] - '0';
  }
  g->charList = getCharList(g, RED);
  g->charListSorted = getCharList(g, RED);
  qsort(g->charListSorted->values, n, sizeof(*g->charListSorted->values),cmpfunc);

  //TODO investigate assigning complexityClass instead to the largest partition
  int best = 1;
  int currentBest = 1;
  int current = getIntListIndex(g->charListSorted, 0);
  for (int i = 1; i < n; i++){
    if(getIntListIndex(g->charListSorted, i) != current) {
      if (currentBest > best) best = currentBest;
      currentBest = 0;
      current = getIntListIndex(g->charListSorted, i);
    }else{
      currentBest ++;
    }
  }
  if (currentBest > best) best = currentBest;
  g->complexityClass = best;
  return g;
}
