/* The purpose of these methods is to process user input, allowing the program
 * to dynamically use different numbers of threads and to calculate different
 * things.
 *
 *
 */

 #include "defs.h"

cmdLineArgs processArgs(int argc, char **argv){
  int n, m;
  int maxIters;
  int maxThreads;
  int type;

  if (argc < 6) {
    printf("Use 5 int args:parallism, n, m, maxIterations, maxThreads\n");
    printf("For parallism: 0 is single threaded, 1 is multithreaded by slicing, and 2 is multithreaded by polling.\n");
  }else{
    type = atoi(argv[1]);
    n = atoi(argv[2]);
    m = atoi(argv[3]);
    maxIters = atoi(argv[4]);
    maxThreads = atoi(argv[5]);
  }
  printf("n = %d, m = %d\n", n, m);
  printf("maxIters = %d, maxThreads = %d\n", maxIters, maxThreads);
  cmdLineArgs args = {type, n, m, maxIters, maxThreads};

  return args;
}
