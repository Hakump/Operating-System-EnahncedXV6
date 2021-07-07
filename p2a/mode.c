//
// Created by ZK Hao on 2/14/2019.
//
char *buffer=NULL;
size_t size = 0;
char *fircmd = "echo";
char *scmd = "ls";
int main(){
    while (1){
        if (getline(&buffer,&size,stdin)!=-1){
            if (strcmp(buffer,"quit")==0) break ;
            else{
                pid_t pid;
                int fd[2];

                pipe(fd);
                pid = fork();

                if(pid==0)
                {
                    dup2(fd[WRITE_END], STDOUT_FILENO);
                    close(fd[READ_END]);
                    close(fd[WRITE_END]);
                    execlp(fircmd, fircmd, "-l", (char*) NULL);
                    fprintf(stderr, "Failed to execute '%s'\n", firstcmd);
                    exit(1);
                }
                else
                {
                    pid=fork();

                    if(pid==0)
                    {
                        dup2(fd[READ_END], STDIN_FILENO);
                        close(fd[WRITE_END]);
                        close(fd[READ_END]);
                        execlp(scmd, scmd, secarg,(char*) NULL);
                        fprintf(stderr, "Failed to execute '%s'\n", scmd);
                        exit(1);
                    }
                    else
                    {
                        int status;
                        close(fd[READ_END]);
                        close(fd[WRITE_END]);
                        waitpid(pid, &status, 0);
                    }
                }
            }

        }
    }

}


