# FIFO through PIPE

This project is a first approach on a multithreaded program constructing a fifo as a communication way between threads. <br />

The master thread (main process) creates two threads. Each thread is responsible for reading or writing on the pipe. <br />
The synchronisation between them is via active waiting one another to write to the pipe or to read from it in case the pipe is full. <br />

There is a small bug in the code as 1 out of 50 runs could run slightly wrong (1% wrong read data). <br />

The program can be compiled through the Makefile. <br />
In order to run it gets as arguments the size of the pipe in bit and the text that will go through the pipe (continuesly)
```bash 
./fifo 4 abcdefghijklmnopqrstuvwxyz
```