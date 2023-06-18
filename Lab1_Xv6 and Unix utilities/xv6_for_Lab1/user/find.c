#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return p;
}

void find(char *path, char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  fd = open(path, 0);
  fstat(fd, &st);

  switch(st.type){
  case T_DEVICE:
    break;
  case T_FILE: 
    if(!strcmp(fmtname(path),filename)){
        printf("%s\n",path);
    }
    break;
  case T_DIR:
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;

      if (!strcmp(de.name, ".") || !strcmp(de.name, "..")){
        continue;
      }

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      find(buf, filename);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "usage: find [root] [filename]\n");
    exit(1);
  }
  
  find(argv[1], argv[2]);
  exit(0);
}