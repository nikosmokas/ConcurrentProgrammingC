# Semaphores

This project contains three programs that achieve multithreaded execution without any problem using binary semaphores.

The binary semaphores are being implemented in each program in the mysem.c file.<br>
There are the functions for creating, destroying the semaphores as well as the raising the semaphore value to 1 and decreasing it to 0. 

## Binary Semaphores 

The idea behind binary semaphores is the same with normal semaphores the only difference is that they can get only the value of 0 and 1. If you try to decrease the semaphore's number while the value is 1, the value will decrease to 0 and the thread will continue, though if the value is 0 and you try to decrease it the value will stay at 0 and thread will get blocked. If you try to raise the semaphore's value from 1 the thread will just continue working and if the samaphore's value is 0 it will raise it to 1 and it will awake the blocked thread.

## Disclaimer
Because of this implementation in case there is a context-switch when the semaphore's functions have been called, bugs can happen though I haven't gotten any to report. Just mentioning this small chance.  
