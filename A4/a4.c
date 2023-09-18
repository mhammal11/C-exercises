#include <stdio.h>
#include <sys/types.h>

int main() {

    pid_t pid = fork();

    printf("%d linked to: %d \n", getpid(), getppid()); // Parent process and first child each linked to their parents

    if (pid > 0) {
        wait();
        pid_t pid1 = fork();
        
        if (pid1 == 0) {
            printf("%d linked to: %d \n", getpid(), getppid()); // Second child linked to parent
        } else {
            wait();

            pid_t pid2 = fork();
            if (pid2 == 0) {
                printf("%d linked to: %d \n", getpid(), getppid()); // Third child linked to parent
            } else {
                wait();

                pid_t pid3 = fork();
        
                if (pid3 == 0) {
                    printf("%d linked to: %d \n", getpid(), getppid()); // Fourth child linked to parent
                } else {
                    wait();
                }
            }
        }
    }
    return 0;
}
