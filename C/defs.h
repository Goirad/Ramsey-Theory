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
  int size;
  int * values;
} intList;
/*
typedef struct TSStack {
	int * list;
	int length;
	int size;
	int growthRate;
	pthread_mutex_t * mutex;
} TSStack;
*/
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
  intList * charList;
  intList * charListSorted;
  bool isValidated;
  int complexityClass;
} Graph;

typedef struct cmdLineArgs {
  int type;
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

typedef struct MTSlicesArgs {
	int * temp;
  int base;
  int blockSize;
  int numThreads;
  int mod;
	int numGraphs;
  char * charListStr;
  int current;
  int chunkStart;
  int chunkLength;
  int * deleted;
	GraphList * gL;
  intList * deletes;
  int * validated;
  pthread_mutex_t * validatedMutex;
  struct TSStack * chunkStack;
  int * isActive;
  pthread_mutex_t * totalActiveMutex;
  int * totalActive;
} MTSlicesArgs;
typedef struct MTPollingArgs {
  Graph * current;
	int * temp;
	int numGraphs;
	GraphList * gL;
	int i;
	int * numActive;
} MTPollingArgs;

typedef struct isoCheckMemBlock {
  intList2D * origVertsG;
  intList2D * vertsG;
  intList2D * vertsH;
  intList * collapseVertsG;
  intList * collapseVertsH;
  intList * perm;
  intList * permScratch;

} isoCheckMemBlock;



//function headers
//list.c
void freeIntList(intList * list);
void freeIntList2D(intList2D * listArray);
void printIntList(intList * list);
void addToIntList(intList * list, int val);
void addToIntListME(intList * list, int val, int growthRate);
intList * copyIntList(intList * list);
void copyIntListTo(intList * src, intList * dest);
int getIntListIndex(intList * list, int n);
//Cell * getListCellIndex(List * list, int n);
intList2D * copyIntList2D(intList2D * array);
void permuteList(intList * src, intList * dest,  intList * perm, intList * permScratch);
intList * newIntList(int n);
intList2D * newIntList2D(int n);
void setIntListIndex(intList * list, int index, int val);
int getIntListIndex(intList * list, int n);
char * getIntListStr(intList * list);
char * getIntListStrNorm(intList * list);
int intListMin(intList * list);
int intListMinIndex(intList * list);
int getNthNonNegValue(intList * list, int n);

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
long fact(int n);
void decToFact(intList * dest, long n, int dig);
void collapseVerts(intList2D * verts, intList * dest, int n);
bool recIsoCheck(intList2D * vertsG, intList2D * vertsH, int depth, Graph * g, Graph * h, isoCheckMemBlock * memBlock);
bool isColorIso(Graph * g, Graph * h, isoCheckMemBlock * memBlock);
isoCheckMemBlock * getIsoCheckMemBlock(int numVerts);
void freeIsoCheckMemBlock(isoCheckMemBlock * block);


void cleanMTSlices(GraphList * gL, int n, int m, int maxThreads, bool isMajor);
void cleanMTPolling(GraphList * gL, int n, int m, int maxThreads, bool isMajor);
void cleanST(GraphList * gL, int n, int m, bool isMajor);
int run(cmdLineArgs args);
int cmpfunc(const void * a, const void * b);

//cmdline.c
cmdLineArgs processArgs(int argc, char **argv);

//storage.c
void dumpGraphList(GraphList * gL, int n, int m);
//void dumpAppendGraphList(GraphList * gL, int n, int m, int ID);
GraphList * readGraphList(FILE * fp);
tier * findLatest(int n, int m);


typedef struct TSStack {
	intList * list;
	int length;
	int size;
	int growthRate;
	pthread_mutex_t * mutex;
} TSStack;


//Function Declarations

struct TSStack * TSS_New();
void TSS_Destroy(struct TSStack * stack);
void TSS_Push(struct TSStack * stack, int value);
int  TSS_Pop(struct TSStack * stack, int * dest);
void TSS_SetGrowthRate(struct TSStack *stack,
                       int             rate);
