#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>


#define THREAD 
 

volatile int threads_done = 0;
volatile int work_done = 0;
//volatile int threads_end = 0;
volatile int numbers_left;
volatile int threads_num, sum ;
volatile int count = 0;

struct workers
{
    int num;
    int avail;
    long int id;
    int finish;
};
struct workers *work;


int primetest(int number){

    if(number == 1)
        return 0;
    int i;

    for(i = 2; i <= number / 2; i++){
         
        if(number % i == 0){
            return 0;
        }
    }
    return 1;
}

void *worker(){

    while(threads_done == 0);
    int i, res;
    long int id;
    id = pthread_self(); 

    for(i = 0; i < threads_num; i++){
        if(id == work[i].id)
            break;
    }

    while(1){

        if (work_done == 1){
            break;
        }
        if(work[i].num == -2){
            continue;
        }

        res = primetest(work[i].num);
        if (res == 0 ){
                printf("Number: %d isn't a prime number!\n", work[i].num);
        }
        else{
            printf("Number: %d is a prime number!\n", work[i].num);
        }
        work[i].avail = 0;
        work[i].num = -2;
        if (work_done == 1){
            break;
        }
        
    }
    //threads_end++;

    work[i].finish = 1;

    //printf("Thread %ld have finished !\n", id);
    return NULL;
}

int main(int argc, char *argv[]){


    int i, pos, j = 0;
    threads_num = atoi(argv[1]);
    numbers_left = argc - 2;
    sum = argc - 2;
    int numbers[argc - 2];

    work = (struct workers *) malloc(threads_num * sizeof(struct workers));
    pthread_t threads[threads_num];

    for(i = 0; i < argc - 2; i++){
        numbers[i] = atoi(argv[i+2]);   
    }
    
    for(i = 0; i < threads_num; i++){

        if(pthread_create(&threads[i], NULL, worker, NULL) != 0){
            fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        work[i].avail = 0;
        work[i].id = threads[i];
        work[i].num = -2;
        work[i].finish = 0;
    }
    printf("threads have been created\n");

    threads_done = 1;
    pos = numbers[j];
    while (numbers_left > 0){
        j = 0;
        //printf("numbers[%d] =  %d\n", j, numbers[j]);
        //sleep(2);
        
        while(numbers[j] == -1){
            
            if(numbers_left == 0)
                break;
            j++;
            pos = numbers[j];
            if(j == sum)
                j = 0;
        }
        //printf("pos: %d\n", pos);
        //sleep(1);
        i = 0;
        while (1){
            if(numbers_left == 0)
                break;
            if(work[i].avail == 0){
                //printf("edo\n");
                work[i].avail = 1;
                work[i].num = pos;
                numbers[j] = -1;
                numbers_left--;
                //count++;
                break;
            }
            else{   
                i++;
            }
            if(i == threads_num){
                i = 0;
            }
        }
    }

    for(i = 0; i < threads_num; i++){
        while(work[i].avail == 1){
            pthread_yield();
        }
    }
    work_done++;

    for(i = 0; i < threads_num; i++){
        while(work[i].finish == 0){
            pthread_yield();
        }
    }
    
    free(work);
    return 0;
}

