#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


int mysem_create(int key, int init){
    int semid;
    //sleep(1);
    //printf("after ftok\n");
    //sleep(1);
    semid = semget(key, 1, IPC_CREAT | 00700);
    if(semid == -1){
        fprintf(stderr, "Error in semget: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(semctl(semid, 0, SETVAL, init) == -1){
        fprintf(stderr, "Error in semctl: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return semid;
}

int mysem_destroy(int semid){
    if(semctl(semid, 0, IPC_RMID) == -1){
        fprintf(stderr, "Error in semctl: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("sems are now destroyed\n");
    return 0;
}

int mysem_down(int semid){
    int res;
    struct sembuf operation;
    operation.sem_num = 0;
    operation.sem_op = -1;
    operation.sem_flg = 0;  
    res = semop(semid, &operation, 1);
    if(res == -1){
        fprintf(stderr, "Error in semop: %s in Line: %d\n", strerror(errno), __LINE__);
        mysem_destroy(semid);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int mysem_up(int semid){
    int res, val;
    struct sembuf operation;
    val = semctl(semid, 0, GETVAL);
    if(val == 1){
        //printf("Lost up call !\n");
        return -2;
    }
    operation.sem_num = 0;
    operation.sem_op = +1;
    operation.sem_flg = IPC_NOWAIT;
    res = semop(semid, &operation, 1);
    if(res == -1){
        fprintf(stderr, "Error in semop: %s in LINE: %d\n", strerror(errno), __LINE__);
        mysem_destroy(semid);
        exit(EXIT_FAILURE);
    }
    return 0;
}