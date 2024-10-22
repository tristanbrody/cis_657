/*  main.c  - main */

// main.c for CIS 657 (OS 10pm) Group 4 - Lab 2 Question 1

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>
void sayHello(void), sayGoodbye(void);
/*------------------------------------------------------------------------
 * main - Example of creating processes and enqueueing them in Xinu
 *------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    // in order to add to the queue or access elements of it, we need the queue ID. So, we can start by just creating a new queue
    qid16 q = newqueue();

    printf("Queue ID: %d\n", q);

    // the lines below add new processes to the queue. Here we are just enqueueing suspended process IDs to the queue (other than PID 1, which will map to currently running main). This way, the process doesn't have to enter an infinite loop in order to still be running by the time we print it
    pid32 pid = 1;
    enqueue(pid, q);

    pid32 next_pid = create(sayHello, 1024, 40, "Hi", 0);
    insert(next_pid, q, 19);

    pid32 next_pid2 = create(sayGoodbye, 1024, 40, "Bye", 0);
    insert(next_pid2, q, 18);

    if (isempty(q))
    {
        printf("The queue is empty\n");
    }
    else
    {
        // we can start off by getting the head of the queue and printing that, which will just be an ID
        qid16 current_pid = queuehead(q);
        printf("Queue head: %d\n", current_pid);
        printf("%-10s %10s %10s %10s %20s\n", "PID", "Key", "Prev", "Next", "Process Name");
        printf("------------------------------------------------------------------\n");

        // next, we enter a while loop - as long as the current_pid isn't EMPTY or equal to the ID of the queuetail, we keep walking the list
        while ((current_pid = queuetab[current_pid].qnext) != EMPTY)
        {

            if (current_pid == queuetail(q))
            {
                // alternate way to break out of the loop if the EMPTY check does not work
                break;
            }
            int32 key = queuetab[current_pid].qkey;
            int32 prev = queuetab[current_pid].qprev;
            int32 next = queuetab[current_pid].qnext;

            printf("%-10d %10d %10d %10d %20s\n", current_pid, key, prev, next, proctab[current_pid].prname);
        }
        printf("------------------------------------------------------------------\n");
        // print the tail of the queue, which will just be an ID
        printf("Queue tail: %d\n", queuetail(q));
    }

    return OK;
}

// below are dummy functions so we can pass something to the additional processes we created and enqueued
void sayHello(void)
{
    printf("Hello");
}

void sayGoodbye(void)
{
    printf("Goodbye");
}