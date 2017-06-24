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

  if (argc < 5) {
    printf("Use 4 int args, n, m, maxIterations, maxThreads\n");
  }else{
    n = atoi(argv[1]);
    m = atoi(argv[2]);
    maxIters = atoi(argv[3]);
    maxThreads = atoi(argv[4]);
  }
  printf("n = %d, m = %d\n", n, m);
  printf("maxIters = %d, maxThreads = %d\n", maxIters, maxThreads);
  cmdLineArgs args = {n, m, maxIters, maxThreads};

  return args;
}
