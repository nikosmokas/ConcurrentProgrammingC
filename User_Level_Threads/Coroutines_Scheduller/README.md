# Co-routines Scheduler

This program implements the Master Thread and Worker Threads idea though checking for prime numbers. <br>
Though in this project the threads are co-routines being created in the routines_orig.c file. <br>

The routines_orig.c file contains all the functions that are being used in the main program. Also there is the implentantion of how the co-routines are being executed in a round-robin, the implementation of the self_made semaphores for the co-routines.<br>

It is an almost whole scheduler that has functions for user-level thread initiation, creation, deletion, a handler for user's as well as system's signals. It switches among them either round-robin, being responsible for blocking the ones that they reducing the semaphore under zero value and awaking them when another co-routine raises the value.<br>

## Disclaimer 

The scheduler does not treat fair the blocked co-routines as they just get blocked instead of getting in a FIFO list.