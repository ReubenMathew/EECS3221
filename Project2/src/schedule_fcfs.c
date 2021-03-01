#include <stdio.h>
#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;

int tid = 0;

void schedule()
{
}

void add(char *name, int priority, int burst)
{
    Task *task = {
        name,
        priority,
        burst};

    printf("[%s] [%d] [%d]\n", task->name, task->priority, task->burst);
    tid++;
    // insert(&head, task);

    // if (tid == 5)
    // {
    //     traverse(head);
    // }
}