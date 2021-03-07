#include <stdio.h>
#include <stdlib.h>
#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;

int tid = 0;
float turnaround = 0.0;
float waiting = 0.0;
float response = 0.0;

void bubbleSort(struct node *start);

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

float waiting_time(struct node *temp)
{
    int sum = 0;

    while (temp != NULL)
    {
        // printf("%d\n", temp->task->waiting);
        sum += temp->task->waiting;
        temp = temp->next;
    }

    return (float)sum / length;
}

float turnaround_time(struct node *temp)
{
    int sum = 0;

    while (temp != NULL)
    {
        sum += temp->task->turnaround;
        temp = temp->next;
    }

    return (float)sum / length;
}

void waiting_time_helper(struct node *temp, int slice)
{
    while (temp != NULL)
    {
        if (temp->task->completed == 0)
        {
            // printf("Updating %s : slice = %d\n", temp->task->name, slice);
            temp->task->waiting += slice;
        }
        temp = temp->next;
    }
}

void turnaround_time_helper(struct node *temp)
{
    temp->task->turnaround = temp->task->burst + temp->task->waiting;
    temp = temp->next;
}

void schedule()
{
    reverseList();
    bubbleSort(head);
    struct node *temp;
    temp = head;

    while (temp != NULL)
    {
        temp->task->completed = 1;
        waiting_time_helper(head, temp->task->burst);
        turnaround_time_helper(temp);

        run(temp->task, temp->task->burst);
        temp = temp->next;
    }

    response = waiting = waiting_time(head);
    turnaround = turnaround_time(head);

    print_stats(waiting, turnaround, response);
}

void add(char *name, int priority, int burst)
{
    int turnaround = 0;
    int waiting = 0;
    int response = 0;

    Task task = {
        .name = name,
        .priority = priority,
        .burst = burst,
        .tid = tid,
        .completed = 0,
        .turnaround = turnaround,
        .waiting = waiting,
        .response = response};

    Task *task_ptr = &task;
    Task *cpy = malloc(sizeof *cpy);
    *cpy = *task_ptr;

    insert(&head, cpy);
    tid++;
}

void swap(struct node *a, struct node *b)
{
    Task *temp = a->task;
    a->task = b->task;
    b->task = temp;
}

void bubbleSort(struct node *start)
{
    int swapped;
    struct node *ptr1;
    struct node *lptr = NULL;

    /* Checking for empty list */
    if (start == NULL)
        return;

    do
    {
        swapped = 0;
        ptr1 = start;

        while (ptr1->next != lptr)
        {
            if (ptr1->task->burst > ptr1->next->task->burst)
            {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}
