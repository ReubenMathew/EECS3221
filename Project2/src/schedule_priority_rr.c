#include <stdio.h>
#include <stdlib.h>
#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;
Task *tasks;

int tid = 0;
float turnaround = 0.0;
float waiting = 0.0;
float response = 0.0;

void bubbleSort(struct node *start);
void reverseList();
void taskArray(struct node *start);
void printTasks(Task *tasks, int length);
void mux(Task *tasks, int length);
void rr(Task *tasks, int length);

void rr(Task *tasks, int length)
{
    int inProgress = length;
    int flag = 0, total = 0, i = 0;

    while (inProgress > 0)
    {
        Task *task = &tasks[i];
        if (tasks[i].burst < QUANTUM && tasks[i].burst > 0)
        {
            run(task, task->burst);
            total += task->burst;
            task->burst = 0;
            flag = 1;
        }
        else if (task->burst > 0)
        {
            run(task, QUANTUM);
            task->burst -= QUANTUM;
            flag = task->burst == 0 ? 1 : flag;
            total += QUANTUM;
        }
        if (task->burst == 0 && flag == 1)
        {
            waiting += total * 1.0;
            inProgress--;
            flag = 0;
        }
        i++;
        if (i == length)
        {
            i = 0;
        }
        // printf("%d %d\n", i, inProgress);
    }
}

void mux(Task *tasks, int length)
{
    if (length > 1)
    {
        rr(tasks, length);
    }
    else
    {
        Task *task = &tasks[0];
        run(task, task->burst);
    }
}

void schedule()
{
    reverseList();
    bubbleSort(head);
    taskArray(head);

    int currPriority = tasks[0].priority;
    Task *tempArr = malloc(length * sizeof *tempArr);

    Task *ptr = NULL;

    int idx = 0;
    ptr = tempArr;

    for (int i = 0; i < length; i++)
    {
        if (currPriority != tasks[i].priority)
        {
            mux(tempArr, idx);
            idx = 0;
            free(tempArr);
            tempArr = malloc(length * sizeof *tempArr);
            ptr = tempArr;
            tempArr[idx] = tasks[i];
            idx++;
        }
        else
        {
            tempArr[idx] = tasks[i];
            idx++;
            ptr++;
        }
        currPriority = tasks[i].priority;
    }
    mux(tempArr, idx);

    waiting = waiting / length;

    print_stats(waiting, 0, 0);
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
        .burstBalance = burst,
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

void printTasks(Task *tasks, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%s %d %d, ", tasks[i].name, tasks[i].priority, tasks[i].burst);
    }
    printf("\n");
}

void taskArray(struct node *temp)
{
    int i = 0;
    Task *ptr = NULL;
    tasks = malloc(length * sizeof *tasks);

    ptr = tasks;

    while (temp != NULL)
    {
        tasks[i] = *temp->task;
        temp = temp->next;
        ptr++;
        i++;
    }
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
    struct node *ptr;
    struct node *left = NULL;

    /* Checking for empty list */
    if (start == NULL)
        return;

    do
    {
        swapped = 0;
        ptr = start;

        while (ptr->next != left)
        {
            if (ptr->task->priority < ptr->next->task->priority)
            {
                swap(ptr, ptr->next);
                swapped = 1;
            }
            ptr = ptr->next;
        }
        left = ptr;
    } while (swapped);
}

void reverseList()
{
    struct node *prev, *curr;

    if (head != NULL)
    {
        prev = head;
        curr = head->next;
        head = head->next;

        prev->next = NULL;

        while (head != NULL)
        {
            head = head->next;
            curr->next = prev;

            prev = curr;
            curr = head;
        }

        head = prev;
    }
}