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

    // Iterate through the queue and print process details
    printf("Processes in the queue:\n");
    struct node *current = queue;
    while (current != NULL) {
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n", current->process.name, current->process.priority, current->process.pid, current->process.runtime);
        current = current->next;
    }

    return 0;
}

