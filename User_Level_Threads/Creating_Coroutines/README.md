# Pipe with Co-routines

This program performs a pipe communication between two co-routines where one of them reads from the pipe and the other writes in it.<br>
The synchronisation between them is by awaking one another to write to the pipe, when the pipe is empty, or to read from it in case the pipe is full. <br />

In order to run the program you need to provide an argument of the pipe size in bits and then when the program is running it is always reading the input it will be given.
eg.
```bash
./fifo 5
```
and while it is running. 
```bash
abcdefghijklmnopqrstuvqxyz
```

## Disclaimer 
In order to exit the program you need to to give the number 0 as an input, though it's not always exiting.