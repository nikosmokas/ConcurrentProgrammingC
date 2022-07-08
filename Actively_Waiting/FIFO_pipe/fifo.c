#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>



volatile char *fifo;
int pipe_size;
volatile int readB = 0, writeB = 0;
int endread = 0 ;
int length;
char *str;
volatile int countW = 0, countR =0;




void pipe_init(int size){

    fifo = (char*)malloc(size);
    pipe_size = size ;
}


void pipe_write(char write_char){ 

    fifo[writeB] = write_char;
    printf("write_char : %c\n", write_char);
    writeB++;
    countW++;
    //printf("pipe_Write 1: writeb: %d, readb: %d\n",writeB, readB );

    //sleep(2);
    
    if(writeB == pipe_size){
        //while (writeB != readB && eof != 1){}
        while(countW - countR >= pipe_size );
        
        writeB = 0;
        //printf("to bita allakse\n");
        //printf("pipe_Write 2: writeb: %d, readb: %d\n",writeB, readB );
    }
}

void pipe_read(char *read_char){

    *read_char = fifo[readB];
    printf("read_char : %c\n", *read_char);
    readB++;
    countR++;
    if(countW == length){
        //printf("edo\n");
        //printf("readB %d , pipesize: %d , countW: %d , length: %d , countR: %d\n", readB, pipe_size, countW, length, countR);
    }
    //printf("pipe_Read 1: writeb: %d, readb: %d\n",writeB, readB );
    if(readB == pipe_size  && countR != length){
        while (countR == countW);
        
        //printf("edo\n");
        //sleep(2);
        readB = 0;
        //printf("pipe_Read 2: writeb: %d, readb: %d\n",writeB, readB );
    }
}


void pipe_close(){

    printf("telos\n");
    endread = 1;
    free((char *)fifo);
}

void *thread_read(){

    //printf("mesa sto thread read\n");
    //sleep(2);
    char read_char;

    while( countW != length || (countW != countR) ){
        
        while(countW > countR){
            pipe_read(&read_char); 
            //sleep(2);
            //printf(" %d , %d\n", writeB, pipe_size );
        }
        
    }
    pipe_close();
    return NULL;
}



void *thread_write(void *ch){

    int i = 0;
    //printf("mesa sto thread write\n");
    //sleep(2);
    while(i < length){
        //printf("%d: %d , %d\n", i, writeB, pipe_size );
        //sleep(2);

        pipe_write(*(str + i));
        i++;
    }

    //printf("terma i thread write\n");
    return NULL;
}

int main (int argc, char *argv[]){

    pthread_t thread_1, thread_2;
    int res, size;
   

    if(argc < 3){
        fprintf(stderr, "Wrong input (need parameter for pipe size and text to be piped)\n");
        return -1;
    }

    length = strlen(argv[2]);
    str = (char*)malloc(length*sizeof(char));
    strcpy(str, argv[2]);
    printf("%s : size = %d\n", str, length);

    size = atoi(argv[1]);
    //printf("size: %d\n", size);
    pipe_init(size);
    //printf("meta tin init\n");

    res = pthread_create(&thread_1, NULL, thread_write, str);
    if( res != 0){
        pipe_close();
        fprintf(stderr, "Error on creating the write end of the pipe: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    //printf("meta tin init\n");

    res = pthread_create(&thread_2, NULL, thread_read, NULL);
    if(res != 0){
        pipe_close();
        fprintf(stderr, "Error on creating the reading end of the pipe: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }


    while (endread != 1 );


    return 0;
}