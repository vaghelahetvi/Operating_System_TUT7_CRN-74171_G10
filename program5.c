#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

// Structure for representing a process
typedef struct {
    char name[256];     // Name of the process
    int priority;       // Priority of the process
    int pid;            // Process ID (PID)
    int runtime;        // Runtime of the process in seconds
} Process;

// Structure for linked list node
typedef struct Node {
    Process process;    // Process information
    struct Node* next;  // Pointer to the next node in the list
} Node;

// Function to push a process onto the queue
void push(Node** queue, Process process) {
    // Create a new node
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    // If the queue is empty, set the new node as the head
    if (*queue == NULL) {
        *queue = newNode;
    } else {
        // Otherwise, find the last node and append the new node
        Node* temp = *queue;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to pop the first process from the queue
Process pop(Node** queue) {
    // If the queue is empty, return an empty process
    if (*queue == NULL) {
        Process empty = {"", -1, -1, -1};
        return empty;
    }

    // Remove the first node and return its process
    Node* temp = *queue;
    *queue = (*queue)->next;
    Process process = temp->process;
    free(temp);
    return process;
}

// Function to delete a process from the queue by name
Process delete_name(Node** queue, char* name) {
    // If the queue is empty, return an empty process
    if (*queue == NULL) {
        Process empty = {"", -1, -1, -1};
        return empty;
    }

    // If the first process matches the name, remove it
    if (strcmp((*queue)->process.name, name) == 0) {
        return pop(queue);
    }

    // Search for the process with the given name and remove it
    Node* prev = *queue;
    Node* curr = (*queue)->next;
    while (curr != NULL) {
        if (strcmp(curr->process.name, name) == 0) {
            prev->next = curr->next;
            Process process = curr->process;
            free(curr);
            return process;
        }
        prev = curr;
        curr = curr->next;
    }

    // Process not found, return an empty process
    Process notFound = {"", -1, -1, -1};
    return notFound;
}

int main() {
    // Initialize the queue as empty
    Node* queue = NULL;

    // Read process information from the file and add them to the queue
    FILE* file = fopen("processes_q5.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char name[256];
    int priority, runtime;
    while (fscanf(file, "%255s %d %d", name, &priority, &runtime) == 3) {
        Process process;
        strcpy(process.name, name);
        process.priority = priority;
        process.pid = 0; // Initialize PID to 0
        process.runtime = runtime;
        push(&queue, process);
    }
    fclose(file);

    // Process each task in the queue
    while (queue != NULL) {
        // Retrieve the next process from the queue
        Process process = pop(&queue);

        // Display process information
        printf("Executing process: %s (Priority: %d, PID: %d, Runtime: %d seconds)\n",
               process.name, process.priority, process.pid, process.runtime);

        // Fork a new process to execute the task
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            // Execute the process binary
            execl(process.name, process.name, NULL);

            // execl only returns if an error occurs
            perror("execl");
            exit(EXIT_FAILURE);
        } else { // Parent process
            // Update the PID of the process in the queue
            process.pid = pid;
            printf("Process %s started with PID: %d\n", process.name, pid);

            // Wait for the specified runtime
            sleep(process.runtime);

            // Terminate the process using SIGINT signal
            printf("Sending SIGINT signal to process %s (PID: %d)\n", process.name, process.pid);
            if (kill(pid, SIGINT) == -1) {
                perror("kill");
            }

            // Wait for the process to terminate
            int status;
            waitpid(pid, &status, 0);

            // Display process termination message
            printf("Process %s (Priority: %d, PID: %d, Runtime: %d seconds) terminated\n",
                   process.name, process.priority, process.pid, process.runtime);
        }
    }

    // All processes executed
    printf("All processes executed.\n");
    return 0;
}
