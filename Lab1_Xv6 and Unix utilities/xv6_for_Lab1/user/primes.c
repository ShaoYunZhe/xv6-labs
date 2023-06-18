#include "kernel/types.h"
#include "user/user.h"

void filter(int fd){
  int p[2];
  pipe(p);

  int prime;
  if(!read(fd, &prime, 4)){
    exit(0);
  }

  if(fork()){
    fprintf(0, "prime %d\n", prime);

    int n;
    while(read(fd, &n, 4)){
      if(n % prime){
        write(p[1], &n, 4);
      }
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
  else{
    close(p[1]);
    filter(p[0]);
  }
}


int main(int argc, char *argv[]){
  int p[2];
  pipe(p);

  if(fork()){
    int n;
    for(n = 2; n <= 35; n++){
      write(p[1], &n, 4);
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
  else{
    close(p[1]);
    filter(p[0]);
    exit(0); // never reached but necessary for compiler
  }
}
