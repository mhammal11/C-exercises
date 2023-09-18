#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define MAX_LINE 80

static char newStr[MAX_LINE];

static int deleteSpace(char str[MAX_LINE]){

int j=0;
for(int i = 0; i<strlen(str)+1; i++){
if(str[i] != ' '){

j=i;
break;
}
else continue;
}

for(int i= 0;i<strlen(str); i++){
if(j<strlen(str)){
//printf("asdasdd");
newStr[i]=str[j];
j++;
}
}
}

typedef struct {
  char **array;
  size_t used;
  size_t size;
} list;


static list history;

static void add_history(const char * cmd){
  if (history.used == history.size) {
    history.size *= 2;
    history.array = (char **)realloc(history.array, history.size * sizeof(char *));
  }
  history.array[history.used++] = strdup(cmd);
}

static void execute(char *command){


    static char *args[MAX_LINE/2 + 1];

    int i = 0;
    int j;
    args[i] = strtok(command," ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }


    if(strcmp(args[i-1], "&") != 0){

        pid_t pid;
        pid = fork();
        if(pid < 0) {
			perror("fork");

        }
        else if (pid == 0) {

            execvp(args[0],args);
            perror("execvp");
            exit(0);
            int j;
            for(j=0;j<i;j++)
                args[j] = NULL;
        }
        else wait(NULL);
    }

    else {
        //printf("asda\n");
        pid_t pid;
        pid = fork();
        if(pid < 0){
            perror("fork");
            return 1; }
        else if (pid == 0){
            args[i-1] = NULL;
            execvp(args[0],args);
            perror("execvp");
            exit(0);

      } else { printf("\n\n");
  }
  }
  }

  static int execute_hist(char * command){
  char temp[MAX_LINE] = {'\0'};
  for(int i=1;i<strlen(command); i++){
        if(command[i] != '\0' && isdigit(command[i])){
            temp[i-1] = command[i];
        }
        else return 0;
  }
  return atoi(temp);
  }


int main(void) {

history.array = (char **)malloc(2 * sizeof(char *));
history.used = 0;
history.size = 2;

static char *args[MAX_LINE/2 + 1];
char command[MAX_LINE] = {'\0'};
int should_run = 1;

while(should_run) {

    for(int i = 0 ; i<MAX_LINE;i++){
        command[i] = '\0';
        newStr[i] = '\0';
    };


    printf("osh> ");
    fflush(stdout);
    scanf ("%[^\n]%*c", command);

    deleteSpace(command);


    if(command[0] == '\0'){
    fgetc(stdin);
    }
        if(command[0] != '\0' && newStr[0] != '\0'){


        if(strcmp(newStr, "exit") == 0) {
            break;
            should_run = 0;
            }
        else if(strcmp(newStr, "history") == 0){
    for(int i=history.used-1 ; i>-1 ; i--){
        printf("%d: %s\n" , (i+1), history.array[i]);

    };

            }
        else if(strcmp(newStr, "!!") == 0){
            if(history.used == 0) printf("No commands in history.\n");
            else{
                execute(history.array[history.used-1]);
                add_history(history.array[history.used-1]);
            }

        }
        else if(newStr[0] == '!'){
            int temp = execute_hist(newStr);
            if(temp){
            if(history.used == 0) printf("No commands in history.\n");
            else if(temp <= history.used ){
                execute(history.array[temp-1]);
                add_history(history.array[temp-1]);
            }
            else printf("No such command in history.\n");
            }
            else{
                execute(newStr);
                add_history(newStr);
            }
        }
        else {
        execute(newStr);
        add_history(newStr);
        }
    }
  }
  return 0;
}