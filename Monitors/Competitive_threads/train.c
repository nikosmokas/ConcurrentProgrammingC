#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

int seats, clients;
volatile int free_seats, full_seats, passengers = 0, queue = 0, count = 0, step = 0;
pthread_mutex_t monitor_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t ride_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t passenger_cond = PTHREAD_COND_INITIALIZER;


void ride(){

    pthread_mutex_lock(&monitor_lock);

    int i;
    
    if(free_seats != 0 || step != 0){
        if(step == 2){
            pthread_mutex_unlock(&monitor_lock);
            return;
        }
        if(step != 3){
            pthread_cond_wait(&ride_cond, &monitor_lock);
        }
    }

    if(passengers == 0){
        
        pthread_mutex_unlock(&monitor_lock);
        return;
    }
    printf("Passengers aboard for a ride !\n");
    //sleep(2);
    printf("Ride finished\n");
    step = 1;
    for(i = 0; i < seats; i++){
        pthread_cond_signal(&passenger_cond);
    }
    
    pthread_mutex_unlock(&monitor_lock);
}

void enter_train(){

    pthread_mutex_lock(&monitor_lock);
    while (1){
        free_seats--;
        if(free_seats > 0){
            printf("Passenger: %ld got aboard !\n", pthread_self());
            pthread_cond_wait(&passenger_cond, &monitor_lock);
            break;
        }
        else if(free_seats == 0){
            printf("Passenger: %ld got aboard last !\n", pthread_self());
            step = 3;
            pthread_cond_signal(&ride_cond);
            pthread_cond_wait(&passenger_cond, & monitor_lock);
            step = 1;
            break;
        }
        else{
            queue++;
            pthread_cond_wait(&wait_cond, &monitor_lock);
        }
    }
    
    pthread_mutex_unlock(&monitor_lock);
}

void exit_ride(){

    pthread_mutex_lock(&monitor_lock);
    int i ;
    printf("Passenger: %ld had a wonderfull ride!\n", pthread_self());
    
    passengers--;
    count++;
    if(count == seats){
        free_seats = seats;
        for(i = 0; i < count; i++){
            pthread_cond_signal(&wait_cond);
        }
        count = 0;
        printf("Passengers left: %d\n", passengers);
        step = 1;
    }
    if(passengers == 0){
        //printf("here\n");
        step = 2;
        pthread_cond_signal(&ride_cond);
    }
    pthread_mutex_unlock(&monitor_lock);
}


void *train(){
    while(passengers > 0){
        ride();
    }
    //printf("here\n");
    return NULL;
}

void *passenger(){

    enter_train();

    exit_ride();

    return NULL;
}



int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Wrong number of parameters\n");
        exit(EXIT_FAILURE);
    }

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