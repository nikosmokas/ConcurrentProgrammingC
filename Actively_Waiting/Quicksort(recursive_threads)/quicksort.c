#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>


int numbers;
int *numbersArray;
int endOfAnalysis = 0;
int masterThreadID;




void * worker(void * array) {
    int *masterArray = (int *)array;
    int sizeOfArray = masterArray[0];
    
    ////////
    printf("Master Array size: %d Array: ", sizeOfArray);
    for (int i = 1; i <= sizeOfArray; i++) {
        printf("%d ", masterArray[i]);
    }
    printf("\n");
    ///////
    
    if (sizeOfArray >= 2) {
        pthread_t thread1, thread2;
        int pivotPos = sizeOfArray/2+1;
        int pivot = masterArray[pivotPos];
        int *left = (int *)malloc(sizeof(int));
        left[0] = 0;
        int *right = (int *)malloc(sizeof(int));
        right[0] = 0;
        printf("Pivot: %d\n", masterArray[pivotPos]);
        for(int i = 1; i <= sizeOfArray; i++) {
            
            if (i == pivotPos) {
                continue;
            }
            if (masterArray[i] > masterArray[pivotPos]) {
                right[0] = right[0] + 1;
                right = (int *)realloc(right, sizeof(int)*(right[0]+1));
                right[right[0]] = masterArray[i];
            }
            else {
                left[0] = left[0] + 1;
                left = (int *)realloc(left, sizeof(int)*(left[0]+1));
                left[left[0]] = masterArray[i];
            }
        }
        
        /////////
        printf("Size of left array: %d\nArray: ", left[0]);
        for (int j = 1; j <= left[0]; j++) {
            printf("%d ", left[j]);
        }
        printf("\n");
        printf("Size of right array: %d\nArray: ", right[0]);
        for (int k = 1; k <= right[0]; k++) {
            printf("%d ", right[k]);
        }
        printf("\n");
        //////////
        
        int leftSize = left[0];
        int rightSize = right[0];
        
        int res1 = pthread_create(&thread1, NULL, worker, right);
        int res2 = pthread_create(&thread2, NULL, worker, left);
        if (res1 != 0 || res2 != 0) {
            printf("Error creating workers!\n");
            exit(2);
        }
        
        while(left[0] == leftSize || right[0] == rightSize) {
            pthread_yield();
        }
        
        if (left[0] >= 0) {
            for(int i = 0; i <= left[0]; i++) {
                masterArray[i+1] = left[i+1];
            }
        }
        masterArray[left[0] + 2] = pivot;
        if (right[0] >= 0) {
            for(int i = 0; i <= right[0]; i++) {
                masterArray[left[0] + 3 + i] = right[i+1];
            }
        }
        masterArray[0] = masterArray[0] - 1;
        
        
        
        printf("Size of array: %d\nArray: ", masterArray[0]);
        for (int i = 0; i <= masterArray[0]; i++) {
            printf("%d ", masterArray[i+1]);
        }
        printf("\n");
        
        
        int thisID = pthread_self();
        
        if (masterThreadID == thisID) {
            endOfAnalysis = 1;
        }
        
    }
    else {
        masterArray[0] = masterArray[0] - 1;
        printf("Master Array size after that shit: %d \n", masterArray[0]);
        return NULL;
    }
    
}
    
    
    



int main(int argc, char *argv[]){
    
    if(argc < 3){
        fprintf(stderr, "Wrong number of arguments\n");
        return -1;
    }
    
    pthread_t sorter;
    
    numbers = argc-1;
    numbersArray = (int *)malloc(sizeof(int)*(numbers+1));
    
    numbersArray[0] = numbers;
    for (int i = 0; i < numbers; i++) {
        numbersArray[i+1] = atoi(argv[i+1]);
    }
    
    /*for (int i = 0; i < numbers; i++) {
        printf("Number %d: %d\n", i, numbersArray[i]);
    }*/
   
    int res = pthread_create(&sorter, NULL, worker, numbersArray);
    masterThreadID = sorter;
   
    while (endOfAnalysis != 1) {
        pthread_yield();
    }
    
    printf("-------Final Result-------\n");
    printf("Size of array: %d\nArray: ", numbersArray[0]);
    for (int i = 0; i < numbers; i++) {
        printf("%d ", numbersArray[i+1]);
    }
    printf("\n");
    
    
    
}
    
    
