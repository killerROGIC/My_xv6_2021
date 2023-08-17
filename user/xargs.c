#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include <stdbool.h>    
 
#define MAX_LEN 512
 
int main(int argc, char *argv[]) {
    // 参数数目不对
    if (argc - 1 >= MAXARG || argc < 2) {
        printf("wrong arguments nember.\n");
        exit(1);
    }

    char line[MAX_LEN];
    char *p = line;
    char *x_argv[MAXARG];
    int i;
    for (i = 0; i < argc; i++) {
        x_argv[i] = argv[i + 1];
    }

    // 记录数据字节数
    int rsz = sizeof(char);
    while (rsz == sizeof(char)) {
        int word_begin = 0;
        int word_end = 0;
        int arg_cnt = i - 1;

        // 读取一行
        while (1) {
            // 读取字符
            rsz = read(0, p, sizeof(char));
            if (++word_end >= MAX_LEN) {
                printf("arguments is too long.\n");
                exit(1);
            }

            // 空字符/换行符
            if (*p == ' ' || *p == '\n' || rsz != sizeof(char)) {
                *p = 0;
                x_argv[arg_cnt++] = &line[word_begin];
                word_begin = word_end;
                if (arg_cnt >= MAXARG) {
                    printf("too many arguments.\n");
                    exit(1);
                }
                if (*p == '\n' || rsz != sizeof(char))
                    break;
            }
            ++p;
        }

        if (fork() == 0) {
            exec(argv[1], x_argv);
        }
        wait(0);
    }
    exit(0);
}