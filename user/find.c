#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *dir, char *file){
    struct stat st;
    struct dirent de;
    int fd;
    char buf[512], *p;
 

    strcpy(buf, dir);
    p = buf + strlen(buf);
    *p++ = '/';
 
    if( (fd = open(dir, 0)) < 0 ){
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }
 
    if( fstat(fd, &st) < 0 ){
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }
 
    if(st.type != T_DIR){
        fprintf(2, "find: %s is not a dir\n", dir);
        close(fd);
        return;
    }
    
    // read the every file or dir in the `dir` sequentially
    while( read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0) continue;
 
        char *name = de.name;
        if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue; // not consider the . and ..
 
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
 
// find <dir_name> <file_name>
// find all the <file_name> in the <dir_name>
int
main(int argc, char* argv[]){
    if(argc < 3){
        fprintf(1, "the arguments is too few...\n");
        exit(1);
    }
 
    find(argv[1], argv[2]);
    exit(0);
}