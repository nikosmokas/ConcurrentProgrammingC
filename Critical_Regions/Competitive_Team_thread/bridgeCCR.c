#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "CCR.h"

#define bridge_time 0

CCR_DECLARE(CCR_bridge);

volatile int blue, red, slots;
volatile int blue_left, red_left;
volatile int reds, blues;
volatile int blue_turn = 0; 
volatile int red_turn = 1;
volatile int cars_on_bridge = 0;

void b_enter_bridge(){
    
    if (red_left == 0) {
        red_turn = 0;
        blue_turn = 1;
            
    }
    
    if (cars_on_bridge < slots || cars_on_bridge < red_left) {
        cars_on_bridge++;
        printf("Blue Car : %ld has passed the bridge\n", pthread_self());
        blue_left--;
    }
    
}

void r_enter_bridge(){
    
    if (blue_left == 0) {
        red_turn = 1;
        blue_turn = 0;
    }
    
    if (cars_on_bridge < slots || cars_on_bridge < red_left) {
        cars_on_bridge++;
        printf("Red Car : %ld has passed the bridge\n", pthread_self());
        red_left--;
        
    }
    
}

void b_exit_bridge(){
    
    if (cars_on_bridge == slots || blue_left == 0) {
        //sleep(bridge_time);
        cars_on_bridge = 0;
        printf("Bridge is empty from blue cars\n");
        if (red_left != 0) {
            blue_turn = 0;
            red_turn = 1;
        }
    }
    
    
}

void r_exit_bridge(){

    if (cars_on_bridge == slots || red_left == 0) {
        //sleep(bridge_time);
        cars_on_bridge = 0;
        printf("Bridge is empty from red cars\n");
        if (blue_left != 0) {
            red_turn = 0;
            blue_turn = 1;
        }
    }

} 


void *thread_red() {
    
    
    CCR_EXEC(CCR_bridge,red_turn,
             
    
    r_enter_bridge();
    
    r_exit_bridge();
    
    )

    return (NULL);
}


void * thread_blue(void * arg){
    
    
    CCR_EXEC(CCR_bridge,blue_turn, 
    
    b_enter_bridge();
    
    b_exit_bridge();
    
    )

    return (NULL);
}



int main(int argc, char * argv[]){

    if(argc != 4){
        printf("Wrong number of parameters\n");
        exit(EXIT_FAILURE);
    }

    int i, j;
    
    CCR_INIT(CCR_bridge);

    slots = atoi(argv[1]);
    red = atoi(argv[2]);
    blue = atoi(argv[3]);
    blue_left = blue;
    red_left = red;

    printf("Red: %d \nBlue: %d\nSlots: %d\n", red, blue, slots);

    pthread_t *cars_r;
    pthread_t *cars_b;
    cars_r = (pthread_t*)malloc(sizeof(pthread_t) * red);
    cars_b = (pthread_t*)malloc(sizeof(pthread_t) * blue);

    for(i = 0, j = 0; i < blue && j < red ; ){
        if(rand()%2 == 0){
            if(i == blue ){
                continue;
            }
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
            if(pthread_create(&cars_r[j], NULL, thread_red, NULL) == -1 ){
                fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            j++;
        }   
    }
    if(j == red){
        for(i = i; i < blue; i++){
            if(pthread_create(&cars_b[i], NULL, thread_blue, NULL) == -1 ){
                fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    else{
        for(j = j; j < red; j++){
            if(pthread_create(&cars_r[j], NULL, thread_red, NULL) == -1 ){
                fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
                exit(EXIT_FAILURE);
            } 
        }
    } 
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
                pthread_join(cars_b[i - red], NULL);
            }
        }
    }
    
    /*for (i = 0; i < blue; i++) {
        pthread_create(&cars_b[i], NULL, thread_blue, NULL);
    }
    for (i = 0; i < red; i++) {
        pthread_create(&cars_r[i], NULL, thread_red, NULL);
    }
    
    for (i = 0; i < blue; i++) {
		pthread_join(cars_b[i],NULL);
	}
	for (i = 0; i < red; i++) {
		pthread_join(cars_r[i], NULL);
	}*/
	
    
    printf("Cars have passed the bridge! Goodbye! passed!\n");

    return 0;
}
