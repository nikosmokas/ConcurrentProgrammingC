#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

volatile int blue, red, slots;
volatile int blue_left = 0, red_left = 0;
volatile int step_b = 0, step_r = 0, bridge_empty = 0;
volatile int blue_cnt = 0, red_cnt = 0, reds = 0, blues = 0;
pthread_mutex_t monitor_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t red_queue = PTHREAD_COND_INITIALIZER;
pthread_cond_t blue_queue = PTHREAD_COND_INITIALIZER;
pthread_cond_t bridge_queue = PTHREAD_COND_INITIALIZER;

void b_enter_bridge(){
    pthread_mutex_lock(&monitor_lock);

    while((reds > 0 || step_b == slots)){
        blue_cnt++;
        pthread_cond_wait(&blue_queue, &monitor_lock);
    }
    step_b++;
    blues++;
    if(blues == 1){
        printf("Blue cars passing!\n");
        bridge_empty = 1;
    }
    printf("Blue Car %d: %ld has reached the bridge!\n", blues, pthread_self());
    if(blues != slots){
        if(blue_cnt > 0){
            blue_cnt--;
            pthread_cond_signal(&blue_queue);
        }
        if(blues == blue_left){
            pthread_cond_signal(&bridge_queue);
        }
        else{
            pthread_cond_wait(&bridge_queue, &monitor_lock);
        }
    }

    pthread_mutex_unlock(&monitor_lock);
}

void r_enter_bridge(){
    pthread_mutex_lock(&monitor_lock);

    while(blues > 0 || step_r == slots){
        red_cnt++;
        pthread_cond_wait(&red_queue, &monitor_lock);
    }
    step_r++;
    reds++;
    if(reds == 1){
        printf("Red cars passing!\n");
        bridge_empty = 1;
    }
    printf("Red Car %d : %ld has reached the bridge!\n", reds, pthread_self());
    if(reds != slots){
        if(red_cnt > 0){
            red_cnt--;
            pthread_cond_signal(&red_queue);
        }
        if(reds == red_left){
            pthread_cond_signal(&bridge_queue);
        }
        else{
            pthread_cond_wait(&bridge_queue, &monitor_lock);
        }
    }
    
    pthread_mutex_unlock(&monitor_lock);
}

void b_exit_bridge(){
    pthread_mutex_lock(&monitor_lock);

    blues--;
    blue_left--;
    printf("Blue Car : %ld has passed the bridge\n", pthread_self());
    if(blues > 0 ){
        pthread_cond_signal(&bridge_queue);
    }
    
    if(blues == 0 && red_cnt > 0){
        bridge_empty = 0;
        step_b = 0;
        red_cnt--;
        pthread_cond_signal(&red_queue);
    }
    if(blues == 0){
        bridge_empty = 0;
        step_b = 0;
        pthread_cond_signal(&blue_queue);
    }

    pthread_mutex_unlock(&monitor_lock);
}

void r_exit_bridge(){
    pthread_mutex_lock(&monitor_lock);

    reds--;
    red_left--;
    printf("Red Car : %ld has passed the bridge\n", pthread_self());
    if(reds > 0){
        pthread_cond_signal(&bridge_queue);
    }
    
    if(reds == 0 && blue_cnt > 0){
        bridge_empty = 0;
        step_r = 0;
        blue_cnt--;
        pthread_cond_signal(&blue_queue);
    }
    if(reds == 0){
        bridge_empty = 0;
        step_r = 0;
        pthread_cond_signal(&red_queue);
    }

    pthread_mutex_unlock(&monitor_lock);
}


void *thread_red(){

    r_enter_bridge();

    r_exit_bridge();

    return NULL;
}


void *thread_blue(){

    b_enter_bridge();

    b_exit_bridge();

    return NULL;
}



int main(int argc, char * argv[]){

    if(argc != 4){
        printf("Wrong number of parameters\n");
        exit(EXIT_FAILURE);
    }

    int i, j;

    srand(time(NULL));
    slots = atoi(argv[1]);
    red = atoi(argv[2]);
    blue = atoi(argv[3]);

    printf("red: %d \nblue: %d\nslots: %d\n", red, blue, slots);

    pthread_t cars_r[red];
    pthread_t cars_b[blue];

    for(i = 0, j = 0; i < blue && j < red ; ){
        if(rand()%2 == 0){
            if(i == blue ){
                continue;
            }
            blue_left++;
            if(pthread_create(&cars_b[i], NULL, thread_blue, NULL) == -1 ){
                fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            i++;
        }
        else{   
            if(j == red ){
                continue;
            }
            red_left++;
            if(pthread_create(&cars_r[j], NULL, thread_red, NULL) == -1 ){
                fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            j++;
        }   
    }
    //printf("j= %d, i= %d\n", j, i);
    if(j == red){
        for(i = i; i < blue; i++){
            blue_left++;
            if(pthread_create(&cars_b[i], NULL, thread_blue, NULL) == -1 ){
                fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    else{
        for(j = j; j < red; j++){
            red_left++;
            if(pthread_create(&cars_r[j], NULL, thread_red, NULL) == -1 ){
                fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
                exit(EXIT_FAILURE);
            } 
        }
    }
    //printf("j= %d, i= %d\n", j, i);
    for (i = 0; i < blue + red; i++) {
        if(blue >= red){
            if (i < blue) {
                pthread_join(cars_b[i], NULL);
            }
            else {
                pthread_join(cars_r[i - blue], NULL);
            }
        }
        else{
            if (i < red) {
                pthread_join(cars_r[i], NULL);
            }
            else {
                pthread_join(cars_b[i - blue], NULL);
            }
        }
    }

    return 0;
}
