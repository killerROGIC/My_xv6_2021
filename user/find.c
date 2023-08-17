#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *dir, char *file){
    int fd;
    char buf[512], *p;
    struct stat st;         // 文件状态
    struct dirent de;       // 目录项
 
    strcpy(buf, dir);
    p = buf + strlen(buf);
    *p++ = '/';
    // 文件打开失败
    if( (fd = open(dir, 0)) < 0 ){
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }
    // 无法获取文件状态
    if( fstat(fd, &st) < 0 ){
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }
    // 参数非目录名
    if(st.type != T_DIR){
        fprintf(2, "find: %s is not a dir\n", dir);
        close(fd);
        return;
    }
    
    // 读取目录项
    while( read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0) continue;
 
        char *name = de.name;
        if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;     // 不在“.”和“..”目录中递归
 
        memmove(p, name, DIRSIZ);
        p[DIRSIZ] = 0;
 
        if(stat(buf, &st) < 0){
            printf("find: cannot stat %s\n", name);
            continue;
        }
 
        if(st.type == T_DIR){
            find(buf, file);
        } else if(strcmp(name, file) == 0) {
            printf("%s\n", buf);
        }
    }
    close(fd);
}
 
int main(int argc, char* argv[]){
    if(argc < 3){
        fprintf(1, "the arguments is too few...\n");
        exit(1);
    }
 
    find(argv[1], argv[2]);
    exit(0);
}