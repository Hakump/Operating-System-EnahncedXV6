#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <syscall.h>

char *args[100], *his[512];
int argc, num_his = 0;

char *path[100];
int num_path = 1;
const char *common = " \t\r\n\v\f";

int exec_input();
int buildin_funct(char *str, int need);

void error_msg(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int get_input_args(char *args[], char *str, const char *sep){
    argc = 0;
    if (str  == NULL) return 0;
    char *token = NULL;
    token = strtok(str, sep);
    while (token != NULL){
        args[argc] = malloc(strlen(token)); args[argc] = token;argc++;
        token = strtok(NULL, sep);
    }
    if (argc == 0) return 0;
    args[argc-1][strlen(args[argc-1])]='\0';
    args[argc]=NULL;
    return argc;
}

int MAX(int n){
    return n > 0 ? n : 0;
}

void my_history(int n){
    if (argc == 1){n = num_his;}
    for (int i = MAX(num_his - n); i < num_his; i++){
        printf("%s", his[i]);
    }
    return;
}

int my_path(char* str, char *c){
    char temp[100];
    for (int i = 0; i < num_path; i++){
        strcpy(temp,path[i]);
        if (strcmp(&c[strlen(path[i])-1],"/")){
            temp[strlen(path[i])] = '/';
            temp[strlen(path[i])+1] = '\0';
        }
        strcat(temp,str);
        if (access(temp, X_OK)==0) {strcpy(c,temp); return 1;}
    }
    error_msg();
    return 0;
}

void combine(char *str, char *d){
    if (buildin_funct(str,0)==0){
        if (my_path(args[0],d)!=0){
            execv(d,args);
            error_msg();
        }
    }
}

char* separate_args(char *str, char *b){
    char* break_point = strstr(str, b);
    break_point[0] = '\0';
    return break_point+1;
}

int rdr(char *str){
    char *next = separate_args(str,">");
    char *output[20];
    get_input_args(args,str,common);
    char i[100];
    get_input_args(output,next,common);
    if (output[0] == NULL) return -1;
    if (fork()==0){
        // The permission was wrong
        int out = open(output[0],O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (out > 0){
            dup2(out,STDOUT_FILENO);
            dup2(out,STDERR_FILENO);
            combine(str,i);
        }
    }else{
        wait(NULL);
    }
    return 1;
}

int piping(char* str){
    char *next = separate_args(str,"|");
    char c[100];
    char d[100];
    int p[2];
    pipe(p);
    get_input_args(args,str,common);
    if (fork()==0){
        dup2(p[1],STDOUT_FILENO);//STDIN_FILENO
        dup2(p[1],STDERR_FILENO);
        combine(str,c);
    } else {
        wait(NULL);
        close(p[1]);
        if (fork()==0){
            dup2(p[0],STDIN_FILENO);
            get_input_args(args,next,common);
            combine(str,d);
        } else{
            wait(NULL);
            close(p[0]);
        }
    }
    return 1;
}

int buildin_funct(char* str, int need_get){
    if (need_get && get_input_args(args,str,common)==0) {return -1;}
    if (strcmp(args[0], "exit")==0){
        exit(0);
    } else if (strcmp(args[0], "cd")==0){
        if (argc != 2 || args[1] == NULL) {error_msg(); return -1;}
        args[1][strlen(args[1])] = '/';
        if (chdir(args[1])==-1) {error_msg(); return -1;}
    } else if(strcmp(args[0], "history")==0){
        if (argc > 2){error_msg(); return -1;}
        if (argc == 1){my_history(num_his);}
        else if (strcmp(args[1],"0")!=0 && atof(args[1])){
            {my_history((int)(atof(args[1])+0.5));}
        }
    } else if(strcmp(args[0], "path")==0){
        if (argc < 2) {error_msg();return -1;}
        //num_path = 1;
        for (int i = 1; i < num_path; i++){
            free(path[i]);
        }
        for (int i = 1; i < argc; i++){
            path[i] = malloc(strlen(args[i]));
            strcpy(path[i],args[i]);
        }
        num_path = argc;//num_path + argc -1;
    } else{
        return 0;
    }
    return 1;
}

int exec_input(){
    char i[100];
    if (my_path(args[0],i)==1) {
        if (fork()==0){
            fflush(stdout);fflush(stderr);
            execv(i, args);
        } else {
            wait(NULL);
            fflush(stdout);fflush(stderr);
        }
    }
    return 0;
}

int run_command(char *buffer){
    int rp = 0;
    if (strstr(buffer, "|")!= NULL) {rp += 1;}
    if (strstr(buffer, ">")!=NULL) {rp += 2;}
    if (rp == 3){error_msg(); return 0;}
    if (rp == 2){
        rdr(buffer);
    } else if (rp == 1){
        piping(buffer);
    } else {
        if (buildin_funct(buffer,1)==0){ // not the buildin command
            exec_input();
        }
    }
    return 0;
}

int main(int argcs, char *argv[]){
    char *buffer=NULL;
    size_t size = 0;
    path[0] = "/bin/";
    if (argv[1]!=NULL){
        FILE *fp;
        if (argcs > 2 || (fp = fopen(argv[1],"r"))==NULL)
        {error_msg(); exit(1);}
        while (getline(&buffer, &size, fp)!=-1)
            run_command(buffer);
        fclose(fp);
        exit(0);
    }
    while (1){
        printf("wish> ");
        fflush(stdout);
        if(getline(&buffer, &size, stdin)==-1)
            exit(0);
        his[num_his] = (char *)malloc(100);//strlen(buffer)
        strcpy(his[num_his],buffer);num_his++;// buffer
        run_command(buffer);
    }
}
