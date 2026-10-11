#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_HISTORY 100
#define MAX_COMMAND_LENGTH 1024

static char history[MAX_HISTORY][MAX_COMMAND_LENGTH];
static int history_count = 0;
static int current_index = 0;
static struct termios org_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &org_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &org_termios);
    atexit(disable_raw_mode);

    struct termios raw = org_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void add_to_history(char const *cmd) {
    if (strlen(cmd) == 0) return; // Dont save empty command
    if (history_count < MAX_HISTORY) {
        strncpy(history[history_count], cmd, MAX_COMMAND_LENGTH - 1);
        history[history_count][MAX_COMMAND_LENGTH - 1] = '\0';
        history_count++;
    }
    current_index = history_count;
}

const char* get_history_up() {
    if (current_index > 0) {
        current_index--;
        return history[current_index];
    }
    return NULL;
}

const char* get_history_down() {
    if (current_index < history_count - 1) {
        current_index++;
        return history[current_index];
    } else {
        current_index = history_count;
        return ""; // Return empty string to clear line when going past the latest command
    }
}

void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

void read_input(char* buffer) {
    int len = 0;
    buffer[0] = '\0'; 

    enable_raw_mode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\n') {
            buffer[len] = '\0';
            printf("\n");
            break;
        } else if (c == 127 || c == '\b') { // Backspace
            if (len > 0) {
                len--;
                buffer[len] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == 27) { 
            char sequence[2];
            if (read(STDIN_FILENO, &sequence[0], 1) == 1 && read(STDIN_FILENO, &sequence[1], 1) == 1) {
                if (sequence[0] == '[') {
                    const char* replacement = NULL;
                    if (sequence[1] == 'A') { // Up arrow
                        replacement = get_history_up();
                    } else if (sequence[1] == 'B') { // Down arrow
                        replacement = get_history_down();
                    }

                    if (replacement != NULL) {
                        while (len > 0) {
                            printf("\b \b");
                            len--;
                        }
                        len = snprintf(buffer, MAX_INPUT, "%s", replacement);
                        printf("%s", buffer);
                        fflush(stdout);
                    }
                }
            }
        } else { 
            if (len < MAX_INPUT - 1) {
                buffer[len++] = c;
                buffer[len] = '\0';
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }
}

int main() {
    char input[MAX_INPUT];
    char* args[MAX_ARGS];

    while (1) {
        printf("\n<tinyshell> ");
        fflush(stdout);

        read_input(input);

        if (strlen(input) == 0) {
            continue;
        }

        add_to_history(input);

        if (strcmp(input, "exit") == 0) {
            printf("Have a nice day!\n");
            break;
        }

        if (strcmp(input, "history") == 0) {
            print_history();
            continue;
        }

        char* token = strtok(input, " ");
        int i = 0;

        while (token != NULL && i < MAX_ARGS) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        if (args[0] == NULL) {
            continue;
        }

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: Missing argument!\n");
            } else if (chdir(args[1]) != 0) {
                perror("cd failed");
            }
            continue;
        }

        pid_t pid = fork();

        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            printf("Exit status: %d\n", status);
        } else {
            perror("fork error");
        }
    }
    return 0;
}