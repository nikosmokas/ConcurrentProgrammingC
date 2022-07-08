#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "routines.h"
#define N 500

char *fifo, *end;
int pipesize;
volatile int readb = 0, writeb = 0, counter = 0;
co_t main_rout, read_rout, write_rout;
char *file;
volatile int eof = 0;

void *reader(){
    //sleep(0.5);
    int i = 0;
    int res;
    char character;
    //printf("read\n");
    
    while(1){
        if(eof == 1){
            break;
        }
        res = read(STDIN_FILENO, &character, sizeof(char));
        if(res == 0){
            eof = 1;
        }
        fifo[i] = character;
        i++;
        readb++;
        //printf("characher: %c\n", character);
        if(i == pipesize  || eof == 1){
            my_coroutines_switchto(&write_rout);
            i = 0;
        }
    }

    return NULL;
}

void *writer(){

    char character;
    int i;

    //printf("write\n");

    while(1){
        character = fifo[i];
        i++;
        writeb++;
        if(eof == 1 && readb == writeb){
            //write(STDOUT_FILENO, "----eof----\n", strlen("----eof----\n") * sizeof(char));
            break;
        }
        write(STDOUT_FILENO, &character, sizeof(char));
        //write(STDOUT_FILENO, "\n", sizeof(char));
        
        if(i == pipesize){
            i = 0;
            my_coroutines_switchto(&read_rout);
        }
    }
    return NULL;
}



int main(int argc, char *argv[]){

    if(argc < 2){
        fprintf(stderr, "Wrong input (need parameter for pipe size and text to be piped)\n");
        return -1;
    }
    pipesize = atoi(argv[1]);
    //printf("pipesize: %d\n", pipesize);

    fifo = (char*)malloc(pipesize * sizeof(char));

    my_coroutines_init(&main_rout);

    my_coroutines_create(&read_rout, (void*)&reader, NULL);


    my_coroutines_create(&write_rout, (void*)&writer, NULL);

    my_coroutines_switchto(&read_rout);
    
    my_coroutines_destroy(&read_rout);
    my_coroutines_destroy(&write_rout);

    return 0;
}