/*  main.c  - main */

// main.c for CIS 657 (OS 10pm) Group 4 - Lab 2 Question 2

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// globally bool variables and semaphores we'll need to pass process execution back and forth between the waiter and signaler functions required for question 2
bool exit_signaler = false;
sid32 waiter_sem;
sid32 signaler_sem;
sid32 lock_to_prevent_race_condition;
void signaler(void), waiter(void);
bool first_run = true;
sid32 sem;

// globally declare process ID variables
pid32 wpid;
pid32 sid;

/*------------------------------------------------------------------------
 * main - Example of process synchronization in Xinu
 *------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    // create all the semaphores we'll need. Initially we'll allow the waiter semaphore to run, and will block the signaler semaphore
    waiter_sem = semcreate(1);
    signaler_sem = semcreate(0);

    // the additional semaphore will act as a mutex for additional checks
    lock_to_prevent_race_condition = semcreate(1);
    sem = semcreate(20);

    // initialize and start the processes for waiter and signaler. We will set them to the same priority and use the semaphores and mutex to pass process execution back and forth + avoid race condition
    wpid = resume(create(waiter, 1024, 30, "waiter", 0));
    sid = resume(create(signaler, 1024, 30, "signaler", 0));

    return OK;
}

void signaler(void)
{
    // the exit_signaler bool variable acts as an additional validation to ensure the waiter function can kill signaler at the end of waiter's loop as required by question 2
    while (!exit_signaler)
    {
        // the signaler_sem starts out blocked, so it needs to wait for a signal from waiter
        wait(signaler_sem);
        wait(lock_to_prevent_race_condition);

        // once both semaphores above signal, we print a line in signaler
        printf("signaler is running\n");

        // after printinge the line above, we signal all the relevant semaphores and yield process control back to waiter
        signaln(sem, 5);
        signal(lock_to_prevent_race_condition);
        signal(waiter_sem);
        yield(); // Yield to give control back to waiter
    }
}

void waiter(void)
{
    // in waiter, we need 2 counter variables - one that holds the decremented integer managed by thee main for loop, and one that is used within the for loop to ensure that after the first 21 numbers run, future runs of waiter will only run 5 times before yielding back to signaler
    int32 i;
    int32 num_printed = 0;

    for (i = 1; i <= 2000; i++)
    {
        if (first_run)
        {
            while (i < 22)
            {
                i++;
            }
            first_run = false;
            signal(sem);
        }

        // unless it's the very first run of waiter where we print 21 numbers above, we should wait for a signal from signaler before starting
        wait(waiter_sem);

        if (num_printed == 0)
        {
            wait(lock_to_prevent_race_condition);
        }

        // print statement that will run 5 times per additional run of waiter
        printf("%d\n", i);
        num_printed++;

        // after num_printed reaches 5, we can yield control back to signaler in the following if block
        if (num_printed == 5)
        {
            num_printed = 0;
            signal(lock_to_prevent_race_condition);
            signal(signaler_sem);
            yield();
        }
        else
        {
            signal(waiter_sem);
        }
    }

    // after the loop of 2000 decremented integers is fully completed, we can set exit_signaler to true to kill the while loop in signaler, can unblock signaler and kill the process ID for good measure
    wait(lock_to_prevent_race_condition);
    exit_signaler = true;
    signal(signaler_sem);
    signal(lock_to_prevent_race_condition);
    kill(sid);
}
