#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

double time_diff(struct timeval, struct timeval);

int main(int argc, char** argv){
  struct timeval t1, t2;
  struct timezone tz;   // possibly not necessary

  // executable to be timed is passed via command line argument
  // it is up to the user to correctly pass an executable
  if (argv[1] == NULL){
    printf("Usage: Timer [executable]\n");
    return 1;
  } else if (strcmp(argv[1], "Timer") == 0) {
    printf("Hah, bad idea. Use an executable besides Timer.\n");
    return 1;
  }
  //FILE *log = fopen("log1.txt", "a+"); // file to store the startup times

  // PROCESS 1
  gettimeofday(&t1, &tz);   // "before" time
  int pid = fork();         // duplicate this process
  if (pid == 0)             // replace the child process with...
    execvp(argv[1], (argv+1));  // ...the executable from argument 1
  wait();
  gettimeofday(&t2, &tz);   // "after" time
  double seconds = time_diff(t1,t2);
  printf("coolshell awake time: %d minutes %.3f seconds.\n",(int)seconds/60,fmod(seconds,60.0));
  //fprintf(log, "P1:\nT1: %d sec + %d usec, T2: %d sec + %d usec\nDiff: %.7f sec\n",
  //       (int)t1.tv_sec,(int)t1.tv_usec,(int)t2.tv_sec,(int)t2.tv_usec,time_diff(t1, t2));

  //fclose(log);
  return 0;
}

// Returns difference of two timevals in seconds
double time_diff(struct timeval before, struct timeval after){
  double befT, aftT;
  befT = (double)before.tv_sec * 1000000 + (double)before.tv_usec; // micro
  aftT = (double)after.tv_sec * 1000000 + (double)after.tv_usec;   // micro
  return (aftT - befT) / 1000000.0;   // convert to sec
}
