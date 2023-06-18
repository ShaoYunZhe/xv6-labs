#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  
  if(fork() == 0){
    char str;
    read(p[0], &str, 1);
    fprintf(0, "%d: received ping\n", getpid());
    write(p[1], &str, 1);
  }
  else{
    char str = 'a';
    write(p[1], &str, 1);
    wait(0);  // very important!!!!
    read(p[0], &str, 1);
    fprintf(0, "%d: received pong\n", getpid());
  }
  exit(0);
}
