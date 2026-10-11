#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_HISTORY 100
#define MAX_COMMAND_LENGTH 1024

static char history[MAX_HISTORY][MAX_COMMAND_LENGTH];
static int history_count =0;
static int current_index=0;

void add_to_history(char const *cmd){
    if (history_count<MAX_HISTORY){
        strncpy(history[history_count],cmd,MAX_COMMAND_LENGTH-1);
        history[history_count][MAX_COMMAND_LENGTH-1]='\0';
        history_count++;
    }
    current_index = history_count;
}

const char* get_history_up(){
    if(current_index>0){
        current_index--;
        return history[current_index];
    }
    return NULL;
}
const char* get_history_down(){
    if(current_index<history_count-1){
        current_index++;
        return history[current_index];
    }else{
        current_index=history_count;
        return "";
    }
    return NULL;
}

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