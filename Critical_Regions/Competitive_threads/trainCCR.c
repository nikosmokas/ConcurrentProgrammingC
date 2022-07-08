#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "CCR.h"
#define CCR 1

int seats, clients;
volatile int free_seats, full_seats, passengers = 0, queue = 0, count = 0, step = 0;
volatile int finish = 0;
CCR_DECLARE(CCR_train);

//volatile int ride = 0;

void ride(){
    if(passengers == 0){
        return;
    }
    printf("Passengers aboard for a ride !\n");
    sleep(2);
    printf("Ride finished\n");
    step = 1;
    finish = 1; 
}

void enter_train(){

    free_seats--;
    printf("Passenger: %ld got aboard !\n", pthread_self());

}

void exit_train(){

    free_seats++;
    printf("Passenger: %ld had a wonderfull ride!\n", pthread_self());
    if(free_seats == seats ){
        
    }
}


void *train(){
    while(passengers > 0){

        CCR_EXEC(CCR_train, (free_seats == 0 && finish == 0) || passengers == 0,   
        ride();
        );
    }

    
    return NULL;
}

void *passenger(){

    CCR_EXEC(CCR_train, free_seats > 0 && finish == 0,
    
    enter_train();  
    );

    CCR_EXEC(CCR_train, (free_seats == 0 || finish == 1) && step == 1, 
    
    exit_train();
    
    );
    if(free_seats == seats){
        step = 0;
        finish = 0;
        printf("Passengers left: %d!\n", passengers - 1);
    }
    passengers--;

    return NULL;
}



int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Wrong number of parameters\n");
        exit(EXIT_FAILURE);
    }
    CCR_INIT(CCR_train);
    seats = atoi(argv[1]);
    free_seats = seats;
    full_seats = 0;

    clients = atoi(argv[2]);
    passengers = clients;

    pthread_t threads[clients + 1];
    int i;

    if(pthread_create(&threads[clients], NULL, train, NULL) == -1){
        fprintf(stderr, "Error in semctl: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    passengers = 0;
    for(i = 0; i < clients; i++){
        passengers++;
        if(i % 5 == 0){
            sleep(5);
        }
        if(pthread_create(&threads[i], NULL, passenger, NULL) == -1){
            fprintf(stderr, "Error in semctl: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    for(i = 0; i < clients ; i++){
        pthread_join(threads[i], NULL);
    }
    pthread_join(threads[clients], NULL);
    printf("HOPE YOU ENJOYED YOUR RIDE\n");

    return 0;
}