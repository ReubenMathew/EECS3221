#include <stdio.h>
#include <stdlib.h>
#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;

int tid = 0;

void reverseList()
{
    struct node *prevNode, *curNode;

    if (head != NULL)
    {
        prevNode = head;
        curNode = head->next;
        head = head->next;

        prevNode->next = NULL;

        while (head != NULL)
        {
            head = head->next;
            curNode->next = prevNode;

            prevNode = curNode;
            curNode = head;
        }

        head = prevNode;
    }
}

void schedule()
{
    reverseList();

    struct node *temp;
    temp = head;

    while (temp != NULL)
    {
        run(temp->task, temp->task->burst);
        temp = temp->next;
    }
}

void add(char *name, int priority, int burst)
{
    Task task = {
        .name = name,
        .priority = priority,
        .burst = burst,
        .tid = tid};

    Task *task_ptr = &task;
    Task *cpy = malloc(sizeof(task_ptr));
    *cpy = *task_ptr;

    insert(&head, cpy);
    tid++;
}