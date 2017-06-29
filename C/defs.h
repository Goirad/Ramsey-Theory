#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/*
  A simple linked list, it is currently a big bottleneck so it Will
  be refined or replaced in future versions.
*/
typedef struct Cell {
  int value;
  struct Cell * next;
} Cell;

typedef struct List {
  int length;
  Cell * first;
} List;

typedef struct intList {
  int length;
  int * values;
} intList;

typedef struct intList2D {
  int length;
  intList ** arrays;
} intList2D;
/*
  The color of an edge. NONE means no edge.
*/
typedef enum Color {
  NONE = 0, RED = 1, GREEN = 2
} Color;


/*
  Stores some data about a graph. n is the number of vertices.
  edges is an array that actually represents the graph.
  isNull is used to clear a graph before actually freeing it.
  Let G = [0 1 2 1]
          [1 0 1 1]
          [2 1 0 2]
          [1 1 2 0] be the adjacency matrix of a graph.
  edges stores the graph as just the right half of its matrix,
  read column by column. So for G it would be
  G = [1 2 1 1 1 2]
  This is I think a fairly compact solution.
*/
typedef struct Graph {
  int n;
  Color * edges;
  bool isNull;
} Graph;

typedef struct cmdLineArgs {
  int n;
  int m;
  int maxIters;
  int maxThreads;
} cmdLineArgs;

/*
  Stores an array of graph pointers
  Future:
  ***graphs -> **Graph -> *Graph -> Graph
                .
                . size of them
                .
               **Graph -> *Graph -> Graph
*/
typedef struct GraphList {
  int size;
  Graph *** graphs;
  int activeIndex;
} GraphList;

typedef struct tier {
  int n;
  int m;
  int tier;
  GraphList * gL;
} tier;

typedef struct isColorIsoThreadArgs {
	int * temp;
  int base;
  int blockSize;
  int numThreads;
  int mod;
	int numGraphs;
  int *  counter;
  int * deleted;
	GraphList * gL;
} isColorIsoThreadArgs;

//function headers
//list.c
void freeIntList(intList * list);
void freeIntList2D(intList2D * listArray);
void printIntList(intList * list);
void addToIntList(intList * list, int val);
intList * copyIntList(intList * list);
int getIntListIndex(intList * list, int n);
//Cell * getListCellIndex(List * list, int n);
intList2D * copyIntList2D(intList2D * array);
intList * permuteList(intList * list,  intList * perm);
intList * newIntList(int n);
intList2D * newIntList2D(int n);
void setIntListIndex(intList * list, int index, int val);
int getIntListIndex(intList * list, int n);

//graph.c
intList * getCharList(Graph * g, Color c);
void printGraph(Graph * g);
void printGraphL(Graph * g);
Graph * createKn(int numVertices);
GraphList * newGraphList(int numGraphs);
Graph * copyGraph(Graph * g);
void destroyGraph(Graph * g);
Color getEdgeColor(Graph * g, int u, int v);
Color getEdgeColorRaw(Graph * g, int n);
void setEdgeColor(Graph * g, int u, int v, Color c);
bool * hasNMinusOneEdges(Graph * g, Color c, int N);
int numColorEdges(Graph * g, Color c, int vertex);
Graph * getSubGraph(Graph * inGraph, Color c);
void shrinkGraphList(GraphList * gL, int newSize);
void destroyGraphList(GraphList * gL);
void mergeGraphLists(GraphList * gLA, GraphList * gLB);
void setGraph(GraphList * gL, Graph * g, int n);
Graph * getGraph(GraphList * gL, int n);
Graph * newGraph(int n, char * raw);
void clearGraphList(GraphList * gL);


//util.c
void dumpMallinfo();
int getMallInfo();
GraphList * getNextSize(Graph * g);
bool hasK3(Graph * g, Color c);
bool hasK4(Graph * g, Color c);
int fact(int n);
intList * decToFact(int n, int dig);
intList * collapseVerts(intList2D * verts, int n);
bool recIsoCheck(intList2D * vertsG, intList2D * vertsH, int depth, Graph * g, Graph * h);
bool isColorIso(Graph * g, Graph * h);
void clean(GraphList * gL, int n, int m, int maxThreads, bool isMajor);
int run(cmdLineArgs args);
int cmpfunc(const void * a, const void * b);

//cmdline.c
cmdLineArgs processArgs(int argc, char **argv);

//storage.c
void dumpGraphList(GraphList * gL, int n, int m);
//void dumpAppendGraphList(GraphList * gL, int n, int m, int ID);
GraphList * readGraphList(FILE * fp);
tier * findLatest(int n, int m);
