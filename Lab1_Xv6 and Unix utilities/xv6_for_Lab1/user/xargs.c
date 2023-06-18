#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER_SIZE 512
// somecommand | xargs command
// 在xargs看来，somecommand的输出存放在标准输入流中

int main(int argc, char *argv[])
{
    char *new_argv[MAXARG];

    for(int i=0;i<argc;i++){
        new_argv[i] = argv[i];
    }

    char buf[BUFFER_SIZE];
    // int n = read(0, buf, BUFFER_SIZE);
    // 不可以只读一次，因为pipe是实时写的
    int k = 0;
    int n = 0;
    char *p = buf;
    while(1){
        k = read(0, p, BUFFER_SIZE - n);
        p += k;
        n += k;
        if(n == BUFFER_SIZE || k == 0){
            break;
        }
    }

    int flag = 0;
    char *start = buf;
    int index = argc;

    for(int i=0; i<n; i++){
        if(buf[i] == ' '){
            buf[i] = '\0';
            if(flag){
                flag = 0;
                new_argv[index] = start;
                index++;
            }
        }
        else if(buf[i] == '\n'){
            buf[i] = '\0';
            if(flag){
                new_argv[index] = start;
            }        
            int pid = fork();
            if(pid == 0){
                exec(new_argv[1], new_argv+1);
            }
            else if(pid > 0){
                wait(0);
            }

            flag = 0;
            index = argc;
            new_argv[index] = 0;
        }
        else{
            if(!flag){
                flag = 1;
                start = buf + i;
            }
        }
    }

    exit(0);
}