#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>   // for fork(), execvp(), _exit()
#include <sys/types.h> // for pid_t
#include <sys/wait.h>  // for wait(), waitpid()

#include "procsub.h"

int process_sub(struct command *cmd, int *wstatus);

int run(struct command *cmd, int *wstatus){
    pid_t pid=fork();

    if (pid==0) //child process
    {
        int fd=-1;

        char buff[cmd->numargs][200];
        char *args[cmd->numargs + 2];
        char cmd_name[200];
        strcpy(cmd_name,cmd->prog);
        args[0]=cmd_name;
        for (int i = 0; i < cmd->numargs; i++)
        {
            
            if (cmd->args[i].type==STRING)        //plain arg
            {
                strcpy(buff[i],cmd->args[i].data.str);
            }else{                                //contain new cmd
                int a;
                char path[100];
                int fd=process_sub(&cmd->args[i].data.cmd, &a);
                snprintf(path,sizeof(path),"/dev/fd/%d", fd);
                strcpy(buff[i], path);
            }
            args[i+1]=buff[i];
          
        }
        args[cmd->numargs+1]=NULL;
        //close(fd);
        execvp(cmd->prog,args);

        perror("execvp failed");
        if (fd!=-1) close(fd);
        exit(127);

    }else{    //parent
        wait(wstatus);
        if (WIFEXITED(*wstatus)&&WEXITSTATUS(*wstatus)==127)
        {
            exit(127);
        }

        return 0;
        

    }
    
    
}


int process_sub(struct command *cmd, int *wstatus){  //returns fd of output
    
    int pfd[2];
    pipe(pfd);
    pid_t pid;
    if ((pid=fork())==-1)
    {
        return -1;
    }

    else if (pid==0) //child process
    {
        int fd=-1;

        close(pfd[0]);
        dup2(pfd[1],STDOUT_FILENO);
        close(pfd[1]);
        char buff[cmd->numargs][200];
        char *args[cmd->numargs + 2];
        char cmd_name[200];
        strcpy(cmd_name,cmd->prog);
        args[0]=cmd_name;
 
        for (int i = 0; i < cmd->numargs; i++)
        {
            
            if (cmd->args[i].type==STRING)        //plain arg
            {
                strcpy(buff[i],cmd->args[i].data.str);
            }else{                                //contain new cmd
                int a;
                char path[100];
                int fd=process_sub(&cmd->args[i].data.cmd, &a);
                snprintf(path,sizeof(path),"/dev/fd/%d", fd);
                strcpy(buff[i], path);
            }
            args[i+1]=buff[i];
        }
        args[cmd->numargs+1]=NULL;
        //close(fd);
        execvp(cmd->prog,args);          //all cmd are converted into fd
        
        perror("execvp failed");
        if (fd!=-1) close(fd);
        exit(127);

    }else{    //parent
        close(pfd[1]);
        wait(wstatus);
        if (WIFEXITED(*wstatus)&&WEXITSTATUS(*wstatus)==127)
        {
            exit(127);
        }

        return pfd[0];
        

    }
    
}