#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>


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
} GraphList;



//function headers
//list.c
void freeList(List * list);
void freeListArray(List *listArray[], int n);
void printList(List * list);
void addToList(List * list, int val);
List * copyList(List * list);
int getListIndex(List * list, int n);
Cell * getListCellIndex(List * list, int n);
List ** copyListArray(List *array[], int n);
List * permuteList(List * list,  List * perm);

//graph.c
int * getCharList(Graph * g, Color c);
void printGraph(Graph * g);
void printGraphL(Graph * g);
Graph * createKn(int numVertices);
GraphList * newGraphList(int numGraphs);
Graph * copyGraph(Graph * g);
void destroyGraph(Graph * g);
Color getEdgeColor(Graph * g, int u, int v);
void setEdgeColor(Graph * g, int u, int v, Color c);
bool * hasNMinusOneEdges(Graph * g, Color c, int N);
int numColorEdges(Graph * g, Color c, int vertex);
Graph * getSubGraph(Graph * inGraph, Color c);
void shrinkGraphList(GraphList * gL, int newSize);
void destroyGraphList(GraphList * gL);
void mergeGraphLists(GraphList * gLA, GraphList * gLB);
void setGraph(GraphList * gL, Graph * g, int n);
Graph * getGraph(GraphList * gL, int n);

//util.c
void dumpMallinfo();
int getMallInfo();
GraphList * getNextSize(Graph * g);
bool hasK3(Graph * g, Color c);
bool hasK4(Graph * g, Color c);
int fact(int n);
List * decToFact(int n, int dig);
int * collapseVerts(List *verts[], int n);
bool recIsoCheck(List *vertsG[], List *vertsH[], int depth, Graph * g, Graph * h);
bool isColorIso(Graph * g, Graph * h);
void clean(GraphList * gL);
int run();
int cmpfunc(const void * a, const void * b);
