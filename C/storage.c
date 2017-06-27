/*
 * The goal of these methods is to allow the program to write out files with found
 * graphs to avoid having to recalculate them, instead just reading them into memory.
 * Also eventually useful as the number of graphs per tier exceeds practical system
 * memory sizes.
 * The graphs will be stored in plaintext as a string of 0s and 1s, representing edges.
 * In addition the file will have a header with some additional information, like
 * the sizes of each graph, as well as what K(n, m) it is a part of.
 * The first line is number of vertices, 2nd is n and m and every other line is a new graph.
 * Each tier is stored in a separate file.
 *
 * eg
 * 5
 * 3 4
 * 1000100111
 * 0101010010
 * etc
 */
#include "defs.h"

void dumpGraphList(GraphList * gL, int n, int m, bool raw){
  if (gL->size > 0){
    int numVertices = getGraph(gL, 0)->n;
    FILE * fp;
    char name[100];
    if (raw) {
      sprintf(name, "/home/goirad/Documents/Ramsey-Theory/Graphs/K(%d, %d)/K(%d, %d) - %dR.txt", n, m, n, m, numVertices);
    }else{
      sprintf(name, "/home/goirad/Documents/Ramsey-Theory/Graphs/K(%d, %d)/K(%d, %d) - %d.txt", n , m, n, m, numVertices);
    }
    fp = fopen(name, "w+");
    fprintf(fp, "%d\n%d\n", n, m);
    fprintf(fp, "%d\n", numVertices);
    fprintf(fp, "%d\n", gL->size);
    fprintf(fp, "%d\n", gL->activeIndex);
    for (int i = 0; i < gL->size; i++){
      Graph * g = getGraph(gL, i);
      for (int j = 0; j < numVertices * (numVertices - 1)/2; j++){
        fprintf(fp, "%d", getEdgeColorRaw(g, j));
      }
      fprintf(fp, "\n");
    }
    fclose(fp);
  }
}

void dumpAppendGraphList(GraphList * gL, int n, int m, bool raw, int ID){
  if (gL->size > 0){
    int numVertices = getGraph(gL, 0)->n;
    FILE * fp;
    char name[100];
    if (raw) {
      sprintf(name, "/home/goirad/Documents/Ramsey-Theory/Graphs/K(%d, %d)/K(%d, %d) - %dR - %d.txt", n, m, n, m, numVertices, ID);
    }else{
      sprintf(name, "/home/goirad/Documents/Ramsey-Theory/Graphs/K(%d, %d)/K(%d, %d) - %d - %d.txt", n , m, n, m, numVertices, ID);
    }
    fp = fopen(name, "w+");
    fprintf(fp, "%d\n%d\n", n, m);
    fprintf(fp, "%d\n", numVertices);
    fprintf(fp, "%d\n", gL->size);
    for (int i = 0; i < gL->size; i++){
      Graph * g = getGraph(gL, i);
      for (int j = 0; j < numVertices * (numVertices - 1)/2; j++){
        fprintf(fp, "%d", getEdgeColorRaw(g, j));
      }
      fprintf(fp, "\n");
    }
    fclose(fp);
  }
}

GraphList * readGraphList(FILE * fp){
  char buff[255];

  fgets(buff, 255, fp);
  int n = atoi(buff);

  fgets(buff, 255, fp);
  int m = atoi(buff);

  fgets(buff, 255, fp);
  int numVertices = atoi(buff);

  fgets(buff, 255, fp);
  int numGraphs = atoi(buff);

  fgets(buff, 255, fp);
  int activeIndex = atoi(buff);

  GraphList * gL = newGraphList(numGraphs);
  for(int i = 0; i < numGraphs; i++){
    fgets(buff, 255, fp);
    setGraph(gL, newGraph(numVertices, buff), i);
  }
  gL->activeIndex = activeIndex;
  return gL;
}

bool fileExists (char * fileName){
  struct stat stats;
  return (stat(fileName, &stats) == 0);
}

tier * findLatest(int n, int m){
  char buff[255];
  int best = 0;
  char bestFile[255];
  bool isRaw;
  for (int i = 2; i < 100; i++){
    sprintf(buff, "/home/goirad/Documents/Ramsey-Theory/Graphs/K(%d, %d)/K(%d, %d) - %dR.txt", n, m, n, m, i);
    if (fileExists(buff)){
      //printf("Found tier %d\n", i);
      //printf("In %s\n", buff);
      strcpy(bestFile, buff);
      best = i;
      isRaw = true;
    }else{

      sprintf(buff, "/home/goirad/Documents/Ramsey-Theory/Graphs/K(%d, %d)/K(%d, %d) - %d.txt", n, m, n, m, i);
      if (fileExists(buff)){
        //printf("Found tier %d\n", i);
        //printf("In %s\n", buff);
        strcpy(bestFile, buff);
        best = i;
        isRaw = false;
      }else{
        printf("Highest tier found\n");
        break;
      }
    }


  }
  tier * t = malloc(sizeof * t);

  if (best > 2){
    FILE * fp;
    fp = fopen(bestFile, "r");
    printf("opening %s\n", bestFile);
    GraphList * gL = readGraphList(fp);
    printf("back to you jim %d\n", isRaw);
    fclose(fp);
    t->n = n;
    t->m = m;
    t->tier = best;
    t->gL = gL;
    t->isRaw = isRaw;
  }else{
    t->n = n;
    t->m = m;
    t->tier = 0;
    t->gL = NULL;
    t->isRaw = false;
  }

  return t;
}
