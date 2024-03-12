#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for process information
struct proc {
    char name[256];
    int priority;
    int pid;
    int runtime;
};

// Linked list node structure
struct node {
    struct proc process;
    struct node *next;
};

// Function to push a process to the linked list (queue)
void push(struct proc process, struct node **queue) {
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    if (new_node == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    new_node->process = process;
    new_node->next = NULL;

    if (*queue == NULL) {
        *queue = new_node;
    } else {
        struct node *temp = *queue;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

// Function to remove and return the next item from the queue
struct proc pop(struct node **queue) {
    if (*queue == NULL) {
        printf("Queue is empty\n");
        exit(EXIT_FAILURE);
    }

    struct proc popped_process = (*queue)->process;
    struct node *temp = *queue;
    *queue = (*queue)->next;
    free(temp);

    return popped_process;
}

// Function to delete a process by name from the queue
struct proc delete_name(char *name, struct node **queue) {
    struct node *current = *queue;
    struct node *prev = NULL;
    while (current != NULL) {
        if (strcmp(current->process.name, name) == 0) {
            if (prev == NULL) {
                *queue = current->next;
            } else {
                prev->next = current->next;
            }
            struct proc deleted_process = current->process;
            free(current);
            return deleted_process;
        }
        prev = current;
        current = current->next;
    }
    return (struct proc){.name = "", .priority = -1, .pid = -1, .runtime = -1}; // Return empty process if not found
}

// Function to delete a process by pid from the queue
struct proc delete_pid(int pid, struct node **queue) {
    struct node *current = *queue;
    struct node *prev = NULL;
    while (current != NULL) {
        if (current->process.pid == pid) {
            if (prev == NULL) {
                *queue = current->next;
            } else {
                prev->next = current->next;
            }
            struct proc deleted_process = current->process;
            free(current);
            return deleted_process;
        }
        prev = current;
        current = current->next;
    }
    return (struct proc){.name = "", .priority = -1, .pid = -1, .runtime = -1}; // Return empty process if not found
}

int main() {
    FILE *file = fopen("processes.txt", "r");
    if (file == NULL) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    struct node *queue = NULL;
    struct proc process;
    while (fscanf(file, "%[^,], %d, %d, %d\n", process.name, &process.priority, &process.pid, &process.runtime) != EOF) {
        push(process, &queue);
    }
    fclose(file);

    // Delete the process named "emacs"
    struct proc deleted_process = delete_name("emacs", &queue);
    printf("Deleted process named 'emacs': Name: %s, Priority: %d, PID: %d, Runtime: %d\n", deleted_process.name, deleted_process.priority, deleted_process.pid, deleted_process.runtime);

    // Delete the process with PID 12235
    deleted_process = delete_pid(12235, &queue);
    printf("Deleted process with PID 12235: Name: %s, Priority: %d, PID: %d, Runtime: %d\n", deleted_process.name, deleted_process.priority, deleted_process.pid, deleted_process.runtime);

    // Iterate through the remaining processes in the queue
    printf("Remaining processes in the queue:\n");
    while (queue != NULL) {
        struct proc current_process = pop(&queue);
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n", current_process.name, current_process.priority, current_process.pid, current_process.runtime);
    }

    return 0;
}

