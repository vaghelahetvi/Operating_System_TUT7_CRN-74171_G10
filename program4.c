#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        // Print PID and other information
        printf("Child process PID: %d\n", getpid());
        printf("Child process is running...\n");

        // Keep printing for 5 seconds
        for (int i = 0; i < 5; i++) {
            printf("Running... (%d seconds)\n", i + 1);
            sleep(1);
        }

        // Exit the child process
        printf("Child process is terminating.\n");
        exit(EXIT_SUCCESS);
    } else { // Parent process
        // Sleep for 5 seconds
        sleep(5);

        // Send SIGTSTP signal to the child process to suspend it
        if (kill(pid, SIGTSTP) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }

        printf("SIGTSTP signal sent to the child process. Suspending...\n");
        sleep(10); // Sleep for 10 seconds

        // Send SIGCONT signal to the child process to resume it
        if (kill(pid, SIGCONT) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }

        printf("SIGCONT signal sent to the child process. Resuming...\n");

        // Wait for the child process to terminate
        int status;
        waitpid(pid, &status, 0);
        printf("Child process terminated.\n");
    }

    return 0;
}

