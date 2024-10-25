#include <xinu.h>

// Declare process ids for waiter and signaler
pid32 wpid, spid;

// Declare a semaphore for synchronization
sid32 sem;

// Declare a global counter for the waiter process
int counter = 1;

// Waiter function: Prints numbers from 1 to 2000, printing 21 numbers initially, then 5 numbers after being signaled
void waiter(void)
{
    int i;

    // Print 21 numbers initially
    kprintf("Waiter starting\n");
    for (i = 0; i < 21 && counter <= 2000; i++)
    {
        kprintf("%d ", counter++);
    }

    // Debugging: Check the total counter value after printing the first 21 numbers
    kprintf("\nWaiter finished printing the initial 21 numbers\n");

    while (counter <= 2000)
    {
        // Wait for the signaler to signal the semaphore before printing 5 more numbers
        kprintf("\nWaiter is waiting for semaphore...\n");
        wait(sem); // Wait for signal from signaler
        kprintf("Semaphore signaled! Proceeding to print 5 more numbers:\n");

        // Print 5 numbers after being signaled
        for (i = 0; i < 5 && counter <= 2000; i++)
        {
            kprintf("%d ", counter++);
        }
    }
}

// Signaler function: Prints a message and signals the waiter to proceed
void signaler(void)
{
    while (counter <= 2000)
    {
        // Print the message and signal the waiter
        kprintf("\nsignaler is running. Signaling semaphore...\n");
        signal(sem);  // Signal the waiter
        sleepms(500); // Add a delay so the signals and waits alternate properly
    }
}

// Main function: Initializes the semaphore and creates the two processes
int main(void)
{
    // Initialize the semaphore with 0
    sem = semcreate(0);

    // Create waiter and signaler processes
    wpid = create(waiter, 1024, 20, "waiter", 0);
    spid = create(signaler, 1024, 25, "signaler", 0);

    // Start both processes
    resume(wpid);
    resume(spid);

    return 0;
}