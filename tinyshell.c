#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64


int main(){
    char input[MAX_INPUT];
    char* args[MAX_ARGS];
    while(1){
        printf("\n<tinyshell>");
        fflush(stdout);

        if(fgets(input, MAX_INPUT, stdin)==NULL){
            perror("fgets eroor");
            continue ;
        }

        input[strcspn(input, "\n")] = '\0';

        if(strcmp(input,"exit")==0){
            printf("Have a nice day!");
            break;
        }

        char* token =strtok(input, " ");
        int i=0;

        while(token!=NULL && i< MAX_ARGS){
            args[i]=token;
            token =strtok(NULL, " ");
            i++;
        }
        args[i]=NULL;
        if(args[0] == NULL){
            continue;
        }

        if(strcmp(args[0], "cd") == 0){
            if(args[1] == NULL){
                fprintf(stderr, "cd: Missing argument!\n");
            } else if(chdir(args[1]) != 0){
                perror("cd failed");
            }
            continue ;
            
        }

        pid_t pid=fork();

        if (pid ==0){
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);

        }else if(pid>0){
            int status;
            waitpid(pid, &status, 0);
            printf("Exist status :%d\n",status);
        }else{
            perror("fork error");
        }


    }
    return 0;
}