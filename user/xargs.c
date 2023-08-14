#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include <stdbool.h>
 
int seq_num = MAXARG;
 
 
void exec1(char **argv){
    if(fork() == 0){
        exec(argv[0], argv);
    } else {
        wait(0);
    }
}
 
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(2, "too few arguments...");
        exit(1);
    }
 
    char buf[512];
    int read_n = 0;
    int read_total = 0;
 
    while( (read_n = read(0, buf + read_total, 512)) > 0 ){
        read_total += read_n;
    }
    int len = read_total;
 
    for(int i = 0; i < len; i ++){
        if(buf[i] == '\n') buf[i] = ' ';
    }
 
    int cmd_ptr = 1;
    bool is_set_n = false;
 
    // set the max args
    if(strcmp(argv[1], "-n") == 0){
        is_set_n = true;
        cmd_ptr = 3;
    }
 
    char* cmd_argv[MAXARG];
    int idx = 0;
    for (int i = cmd_ptr; i < argc; i++) {
        char *arg = (char *)malloc(strlen(argv[i])+1); 
        strcpy(arg, argv[i]);
        cmd_argv[idx] = arg;
        idx ++;
    }
 
    if(is_set_n){
        int index = 0;
        char arg[MAXARG];
        memset(arg, 0, MAXARG); // clear the space
        for(int i = 0; i < len; i++){
            if(buf[i] == ' ') { // can spilt out a arg
 
                arg[index++] = '\0';
                cmd_argv[idx++] = arg;
 
                // run
                exec1(cmd_argv);
 
                index = 0;
                memset(arg, 0, MAXARG); // clear the space
 
                idx --;
                continue;
            }
            arg[index ++] = buf[i];
        }
    } else {
        int prev = 0;
        // should spilt the args with ' '
        // and put them into cmd_argv sequentially
        for(int i = 0; i < len; i ++){
            if(buf[i] == ' '){
                char* add_arg = (char *)malloc(i - prev);
                memcpy(add_arg, buf+prev, i - prev);
                prev = i+1; // attention
                cmd_argv[idx++] = add_arg;
            }
        }
        exec1(cmd_argv);
    }
 
    exit(0);
}