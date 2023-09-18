#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

// The maximum length command 
#define MAX_LINE 80

static char string[MAX_LINE];

typedef struct {
    char ** array;
    size_t space;
    size_t size;
}
list;

static list history;

// Appends commands to history after being used
static void add_history(const char * command) {
    if (history.space == history.size) {
        history.size *= 2;
        history.array = (char ** ) realloc(history.array, history.size * sizeof(char * ));
    }
    history.array[history.space++] = strdup(command);
}

// Invoked upon execution of the custom "history" command
static int execute_history(char * command) {
    char temp[MAX_LINE] = {'\0'};
    for (int i = 1; i < strlen(command); i++) {
        if (command[i] != '\0' && isdigit(command[i])) {
        temp[i - 1] = command[i];
        } else {
            return 0;
        }
    }
    return atoi(temp);
}

// For a given command in the shell, this function deals with the space character
static int remove_space(char input_str[MAX_LINE]) {
    int j = 0;
    for (int i = 0; i < strlen(input_str) + 1; i++) {
        if (input_str[i] != ' ') {
            j = i;
            break;
        } else {
            continue;
        }
    }
    for (int i = 0; i < strlen(input_str); i++) {
        if (j < strlen(input_str)) {
            string[i] = input_str[j];
            j++;
        }
    }
}


static void execute_command(char * command) {

    static char * args[MAX_LINE / 2 + 1];
    int i = 0;
    args[i] = strtok(command, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }

    if (strcmp(args[i - 1], "&") == 0) {
        pid_t pid;
        pid = fork();
        if (pid == 0) {
            args[i - 1] = NULL;
            execvp(args[0], args);
            perror("execvp");
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            return 1;
        } else {
            printf("\n\n");
        }
    } else {
        pid_t pid;
        pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp");
            exit(0);
            int j;
            for (j = 0; j < i; j++) {
                args[j] = NULL;
            };
        } else if (pid < 0) {
            perror("fork");
        } else {
            wait(NULL);
        }
    }
}

int main(void) {

    // Create history list
    history.array = (char ** ) malloc(2 * sizeof(char * ));
    history.space = 0;
    history.size = 2;

    static char * args[MAX_LINE / 2 + 1]; // Command line arguments
    char command[MAX_LINE] = {'\0'};
    int should_run = 1; // Flag to determine when to exit program

    while (should_run) {
        for (int i = 0; i < MAX_LINE; i++) {
            command[i] = '\0';
            string[i] = '\0';
        };

        printf("osh> ");
        fflush(stdout); // Flush output out of buffer stream
        scanf("%[^\n]%*c", command); // Scanner that takes in the user command

        remove_space(command);

        // Check user input for a command and get next character if a space is encountered
        if (command[0] == '\0') {
            fgetc(stdin);
        }
        // Check user input for a command
        if (command[0] != '\0' && string[0] != '\0') {
            // If user types exit, exit program
            if (strcmp(string, "exit") == 0) {
                break;
                should_run = 0;
            // If user types !!, execute the last command in history
            } else if (strcmp(string, "!!") == 0) {
                // If there are no commands in history, return error message
                if (history.space == 0) {
                    printf("No commands in history.\n");
                } else {
                    execute_command(history.array[history.space - 1]);
                    add_history(history.array[history.space - 1]);
                }
            // If user types ! check next number and execute the command in history associated with the number
            } else if (string[0] == '!') {
                if (execute_history(string)) {
                    // If there are no commands in history, return error message
                    if (history.space == 0) {
                        printf("No commands in history.\n");
                    } else if (execute_history(string) <= history.space) {
                        execute_command(history.array[execute_history(string) - 1]);
                        add_history(history.array[execute_history(string) - 1]);
                    } else {
                        // If user selects incorrect number, return error message
                        printf("No such command in history.\n");
                    }
                } else {
                    execute_command(string);
                    add_history(string);
                }
            // If user types history, return a list of the last 5 commands entered by the user
            } else if (strcmp(string, "history") == 0) {
                // Counter to return only the last 5 commands
                int counter = 0;
                for (int i = history.space - 1; i > -1; i--) {
                    if (counter < 5) {
                        printf("%d: %s\n", (i + 1), history.array[i]);
                    }
                    // Increment counter
                    counter++;
                };
            } else {
                execute_command(string);
                add_history(string);
            }
        }
    }
    return 0;
}